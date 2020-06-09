"""
Copyright 2019 ARM Limited
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""

import sys
from icetea_lib.bench import Bench
from icetea_lib.TestStepError import TestStepFail


class Testcase(Bench):
    def __init__(self):
        Bench.__init__(self, name="mesh_minimal_startup",
                        title = "Mesh minimal example application startup",
                        status = "released",
                        type = "regression",
                        subtype = "startup",
                        author = "Kimmo Lepikko",
                        purpose = "Verifies that the application starts up correctly and connects to border router",
                        component=["6LoWPAN"],
                        requirements={
                           "duts": {
                               '*': { #requirements for all nodes
                                    "count":1,
                                    "type": "hardware",
                                    "application":{ "name":"TEST_APPS-mesh_minimal_startup", "version": "1.0", "init_cli_cmds": [], "post_cli_cmds": [] },
                                    "rf_channel": 18
                               },
                               "1": {"nick":  "client"},
                           }}
        )

    def setup(self):

        # Reset device so you will get traces in HW environment
        self.reset_dut(1)

    def case(self):

        # 1. Verify that client starts the bootstrap (received immidiately after reset)
        for i in range(1, 20):
            self.delay(1)
            found = self.verify_trace_skip_fail(1, "Start mesh-minimal application") \
                    or self.verify_trace_skip_fail(1, "Connecting...") \
                    or self.verify_trace_skip_fail(1, "Connection failed!") \
                    or self.verify_trace_skip_fail(1, "Connected.")
            if found:
                break

        if not found:
            raise TestStepFail("1. Startup message not found")

        self.logger.info("1. Startup OK")

        # 2. Verify 6LoWPAN bootstrap (fails to timeout or bootstrap failure trace)
        bootstrap_complete = "Connected. IP ="
        bootstrap_fail = "Connection failed!"
        loops = 210 # Connection can take up a long time (>1980 seconds in case of Wi-SUN).
        for i in range (1, loops + 1):
            self.logger.info("Waiting for connection " + str(i) + "/" + str(loops))
            self.delay(10)
            if self.verify_trace_skip_fail(1, bootstrap_complete):
                self.logger.info("2. Bootstrap OK")
                break
            if i == loops or self.verify_trace_skip_fail(1, bootstrap_fail):
                raise TestStepFail("2. Bootstrap failed")
