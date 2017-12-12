properties ([[$class: 'ParametersDefinitionProperty', parameterDefinitions: [
  [$class: 'StringParameterDefinition', name: 'mbed_os_revision', defaultValue: '', description: 'Revision of mbed-os to build. Use format "pull/PR-NUMBER/head" to access mbed-os PR'],
  [$class: 'BooleanParameterDefinition', name: 'smoke_test', defaultValue: false, description: 'Enable to run HW smoke test after building']
  ]]])

if (params.mbed_os_revision == '') {
  echo 'Use mbed OS revision from mbed-os.lib'
} else {
  echo "Use mbed OS revisiong ${params.mbed_os_revision}"
  if (params.mbed_os_revision.matches('pull/\\d+/head')) {
    echo "Revision is a Pull Request"
  }
}
echo "Run smoke tests: ${params.smoke_test}"


// Map RaaS instances to corresponding test suites
// Run only selected set of tests.
// Board: K64F
// Radios: Atmel & MCR20A
// configurations: 6LP + Thread
def raas = [
  "lowpan_mesh_minimal_smoke_k64f_atmel.json": "8001",
//  "lowpan_mesh_minimal_smoke_k64f_mcr20.json": "8034",
//  "lowpan_mesh_minimal_smoke_429zi_atmel.json": "8030",
//  "lowpan_mesh_minimal_smoke_429zi_mcr20.json": "8033",
//  "lowpan_mesh_minimal_smoke_ublox_atmel.json": "8031",
//  "thread_mesh_minimal_smoke_k64f_atmel.json": "8007"
  "thread_mesh_minimal_smoke_k64f_mcr20.json": "8034",
//  "thread_mesh_minimal_smoke_429zi_atmel.json": "8030",
//  "thread_mesh_minimal_smoke_429zi_mcr20.json": "8033",
//  "thread_mesh_minimal_smoke_ublox_atmel.json": "8031"
  ]

// List of targets with supported RF shields to compile
def targets = [
  "K64F": ["ATMEL", "MCR20"],
  "NUCLEO_F401RE": ["ATMEL", "MCR20"],
  "NUCLEO_F429ZI": ["ATMEL", "MCR20"],
  //"NCS36510": ["NCS36510"],
  "UBLOX_EVK_ODIN_W2": ["ATMEL"],
  "KW24D": ["MCR20"]
  ]

// Map toolchains to compilers
def toolchains = [
  ARM: "armcc",
  GCC_ARM: "arm-none-eabi-gcc",
  IAR: "IAR-linux"
  ]

// Supported RF shields
def radioshields = [
  "ATMEL",
  "MCR20",
  "NCS36510"
  ]

// Mesh interfaces: 6LoWPAN and Thread
def meshinterfaces = [
  "6lp",
  "thd"
  ]

def stepsForParallel = [:]

// Jenkins pipeline does not support map.each, we need to use oldschool for loop
for (int i = 0; i < targets.size(); i++) {
  for(int j = 0; j < toolchains.size(); j++) {
    for(int k = 0; k < radioshields.size(); k++) {
      for(int l = 0; l < meshinterfaces.size(); l++) {
        def target = targets.keySet().asList().get(i)
        def allowed_shields = targets.get(target)
        def toolchain = toolchains.keySet().asList().get(j)
        def compilerLabel = toolchains.get(toolchain)
        def radioshield = radioshields.get(k)
        def meshInterface = meshinterfaces.get(l)

        def stepName = "${target} ${toolchain} ${radioshield} ${meshInterface}"
        if(allowed_shields.contains(radioshield)) {
          stepsForParallel[stepName] = buildStep(target, compilerLabel, toolchain, radioshield, meshInterface)
        }
      }
    }
  }
}

def parallelRunSmoke = [:]

// Need to compare boolean against string value
if ( params.smoke_test == true ) {
  // Generate smoke tests based on suite amount
  for(int i = 0; i < raas.size(); i++) {
    def suite_to_run = raas.keySet().asList().get(i)
    def raasPort = raas.get(suite_to_run)
    // Parallel execution needs unique step names. Remove .json file ending.
    def smokeStep = "${raasPort} ${suite_to_run.substring(0, suite_to_run.indexOf('.'))}"
    parallelRunSmoke[smokeStep] = run_smoke(targets, toolchains, radioshields, meshinterfaces, raasPort, suite_to_run)
  }
}

timestamps {
  parallel stepsForParallel
  parallel parallelRunSmoke
}

def buildStep(target, compilerLabel, toolchain, radioShield, meshInterface) {
  return {
    stage ("${target}_${compilerLabel}_${radioShield}_${meshInterface}") {
      node ("${compilerLabel}") {
        deleteDir()
        dir("mbed-os-example-mesh-minimal") {
          checkout scm
          def config_file = "mbed_app.json"

          if ("${meshInterface}" == "thd") {
            config_file = "./configs/mesh_thread.json"
            // Use systest Thread Border Router for testing (CH=18, PANID=BAAB)
            execute("sed -i '/mbed-mesh-api.thread-device-type\":/a \"mbed-mesh-api.thread-config-channel\": 18,' ${config_file}")
            execute("sed -i '/mbed-mesh-api.thread-device-type\":/a \"mbed-mesh-api.thread-config-panid\": \"0xBAAB\",' ${config_file}")
            execute("sed -i 's/\"nanostack.configuration\": \"thread_router\"/\"nanostack.configuration\": \"thread_end_device\"/'  ${config_file}")
            execute("sed -i 's/\"mbed-mesh-api.thread-device-type\": \"MESH_DEVICE_TYPE_THREAD_ROUTER\"/\"mbed-mesh-api.thread-device-type\": \"MESH_DEVICE_TYPE_THREAD_MINIMAL_END_DEVICE\"/' ${config_file}")
            }

          if ("${meshInterface}" == "6lp") {
            config_file = "./configs/mesh_6lowpan.json"
            // Use systest 6LoWPAN Border Router for testing (CH=17, PANID=ABBA)
            execute("sed -i 's/\"mbed-mesh-api.6lowpan-nd-channel\": 12/\"mbed-mesh-api.6lowpan-nd-channel\": 17/' ${config_file}")
            execute("sed -i 's/\"mbed-mesh-api.6lowpan-nd-panid-filter\": \"0xffff\"/\"mbed-mesh-api.6lowpan-nd-panid-filter\": \"0xABBA\"/' ${config_file}")
          }

          if ("${radioShield}" == "MCR20") {
            // Replace default rf shield
            execute("sed -i 's/\"value\": \"ATMEL\"/\"value\": \"MCR20\"/' ${config_file}")
          }

          if ("${radioShield}" == "NCS36510") {
            // Replace default rf shield
            execute("sed -i 's/\"value\": \"ATMEL\"/\"value\": \"NCS36510\"/' ${config_file}")
          }

          // For KW24D, we need to optimize for low memory
          if ("${target}" == "KW24D") {
            // Use optimal mbed TLS config file, need double escaping of '\' characters, first ones to escape Grooy, second ones to escape shell
            // Need to use SH shells
            sh("sed -i '/\"target_overrides\"/ i \"macros\": [\"MBEDTLS_USER_CONFIG_FILE=\\\\\"mbedtls_config.h\\\\\"\"],' ${config_file}")
            // Limit mesh heap size to 11kB
            execute("sed -i 's/mbed-mesh-api.heap-size\": .*,/mbed-mesh-api.heap-size\": 11000,/' ${config_file}")
            // Limit event loop heap size
            execute("sed -i '/target.features_add/ i \"nanostack-hal.event_loop_thread_stack_size\": 2048,' ${config_file}")
            if ("${toolchain}" == "GCC_ARM" && "${meshInterface}" == "thd") {
                execute("sed -i 's/mbed-mesh-api.heap-size\": .*,/mbed-mesh-api.heap-size\": 15000,/' ${config_file}")
                execute("sed -i '/mbed-mesh-api.thread-device-type\":/a \"mbed-mesh-api.use-malloc-for-heap\": true,' ${config_file}")
            }
          }

          // Activate traces
          execute("sed -i 's/\"mbed-trace.enable\": false/\"mbed-trace.enable\": true/' ${config_file}")

          // Set mbed-os to revision received as parameter
          execute ("mbed deploy --protocol ssh")
          if (params.mbed_os_revision != '') {
            dir ("mbed-os") {
              if (params.mbed_os_revision.matches('pull/\\d+/head')) {
                execute("git fetch origin ${params.mbed_os_revision}:PR")
                execute("git checkout PR")
              } else {
                execute ("git checkout ${params.mbed_os_revision}")
              }
            }
          }

          execute ("mbed compile --build out/${target}_${toolchain}_${radioShield}_${meshInterface}/ -m ${target} -t ${toolchain} -c --app-config ${config_file}")
        }
        stash name: "${target}_${toolchain}_${radioShield}_${meshInterface}", includes: '**/mbed-os-example-mesh-minimal.bin'
        archive '**/mbed-os-example-mesh-minimal.bin'
        step([$class: 'WsCleanup'])
      }
    }
  }
}

def run_smoke(targets, toolchains, radioshields, meshinterfaces, raasPort, suite_to_run) {
  return {
    // Remove .json from suite name
    def suiteName = suite_to_run.substring(0, suite_to_run.indexOf('.'))
    stage ("smoke_${raasPort}_${suiteName}") {
      node ("mesh-test") {
        deleteDir()
        dir("mbed-clitest") {
          git "git@github.com:ARMmbed/mbed-clitest.git"
          execute("git checkout ${env.LATEST_CLITEST_STABLE_REL}")
        }
        dir("testcases") {
          git "git@github.com:ARMmbed/mbed-clitest-suites.git"
          execute("git checkout master")
          execute("git submodule update --init --recursive")
          dir("6lowpan") {
            execute("git checkout master")
          }
          dir("thread-tests") {
            execute("git checkout master")
          }
        }

        for (int i = 0; i < targets.size(); i++) {
          for(int j = 0; j < toolchains.size(); j++) {
            for(int k = 0; k < radioshields.size(); k++) {
              for(int l = 0; l < meshinterfaces.size(); l++) {
                def target = targets.keySet().asList().get(i)
                def allowed_shields = targets.get(target)
                def toolchain = toolchains.keySet().asList().get(j)
                def radioshield = radioshields.get(k)
                def meshInterface = meshinterfaces.get(l)
                // Skip unwanted combination
                if (target == "NUCLEO_F401RE" && toolchain == "IAR") {
                  continue
                }
                if(allowed_shields.contains(radioshield)) {
                  unstash "${target}_${toolchain}_${radioshield}_${meshInterface}"
                }
              }
            }
          }
        }
        env.RAAS_USERNAME = "user"
        env.RAAS_PASSWORD = "user"
        execute("./mbed-clitest/clitest.py --tcdir testcases --suitedir testcases/suites/ --suite ${suite_to_run} --type hardware --reset --raas 62.44.193.186:${raasPort} --raas_share_allocs --failure_return_value -vvv -w --log log_${raasPort}_${suiteName}")
        archive "log_${raasPort}_${suiteName}/**/*"
      }
    }
  }
}
