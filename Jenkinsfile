properties ([[$class: 'ParametersDefinitionProperty', parameterDefinitions: [
  [$class: 'StringParameterDefinition', name: 'mbed_os_revision', defaultValue: 'master', description: 'Revision of mbed-os to build'],
  [$class: 'BooleanParameterDefinition', name: 'smoke_test', defaultValue: false, description: 'Enable to run HW smoke test after building']
  ]]])

echo "Run smoke tests: ${smoke_test}"

try {
  echo "Verifying build with mbed-os version ${mbed_os_revision}"
  env.MBED_OS_REVISION = "${mbed_os_revision}"
} catch (err) {
  def mbed_os_revision = "master"
  echo "Verifying build with mbed-os version ${mbed_os_revision}"
  env.MBED_OS_REVISION = "${mbed_os_revision}"
}

// Map RaaS instances to corresponding test suites
def raas = [
  "lowpan_mesh_minimal_smoke_k64f_atmel.json": "8001",
  "lowpan_mesh_minimal_smoke_k64f_mcr20.json": "8034",
  "lowpan_mesh_minimal_smoke_429zi_atmel.json": "8030",
  "lowpan_mesh_minimal_smoke_429zi_mcr20.json": "8033",
  "lowpan_mesh_minimal_smoke_ublox_atmel.json": "8031",
  "thread_mesh_minimal_smoke_k64f_atmel.json": "8007",
  "thread_mesh_minimal_smoke_k64f_mcr20.json": "8034",
  "thread_mesh_minimal_smoke_429zi_atmel.json": "8030",
  "thread_mesh_minimal_smoke_429zi_mcr20.json": "8033",
  "thread_mesh_minimal_smoke_ublox_atmel.json": "8031"
  ]

// List of targets with supported RF shields to compile
def targets = [
  "K64F": ["ATMEL", "MCR20"],
  "NUCLEO_F401RE": ["ATMEL", "MCR20"],
  "NUCLEO_F429ZI": ["ATMEL", "MCR20"],
  //"NCS36510": ["NCS36510"],
  "UBLOX_EVK_ODIN_W2": ["ATMEL"]
  ]
  
// Map toolchains to compilers
def toolchains = [
  ARM: "armcc",
  GCC_ARM: "arm-none-eabi-gcc",
  IAR: "iar_arm"
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

        // Skip unwanted combination
        if (target == "NUCLEO_F401RE" && toolchain == "IAR") {
          continue
        }

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
if ( smoke_test == "true" ) {
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

          if ("${radioShield}" == "MCR20") {
            // Replace default rf shield
            execute("sed -i 's/\"value\": \"ATMEL\"/\"value\": \"MCR20\"/' mbed_app.json")
          }

          if ("${radioShield}" == "NCS36510") {
            // Replace default rf shield
            execute("sed -i 's/\"value\": \"ATMEL\"/\"value\": \"NCS36510\"/' mbed_app.json")
          }

          if ("${meshInterface}" == "thd") {
            // Change mesh interface to thread
            execute("sed -i 's/\"value\": \"MESH_LOWPAN\"/\"value\": \"MESH_THREAD\"/' mbed_app.json")

            // Change operation mode to Thread router
            execute("sed -i 's/\"NANOSTACK\", \"LOWPAN_ROUTER\", \"COMMON_PAL\"/\"NANOSTACK\", \"THREAD_ROUTER\", \"COMMON_PAL\"/' mbed_app.json")

            // Increase HEAP size
            execute("sed -i 's/\"mbed-mesh-api.heap-size\": 10000/\"mbed-mesh-api.heap-size\": 32000/' mbed_app.json")

            // Use systest Thread border router for testing (CH=18, PANID=BAAB)
            execute("sed -i 's/\"mbed-mesh-api.thread-config-channel\": 12/\"mbed-mesh-api.thread-config-channel\": 18/' mbed_app.json")
            execute("sed -i 's/\"mbed-mesh-api.thread-config-panid\": \"0xDEFA\"/\"mbed-mesh-api.thread-config-panid\": \"0xBAAB\"/' mbed_app.json")
            // Activate traces
            execute("sed -i 's/\"mbed-trace.enable\": false/\"mbed-trace.enable\": true/' mbed_app.json")
          }

          if ("${meshInterface}" == "6lp") {
          // Use systest border router for testing
            execute("sed -i 's/\"mbed-mesh-api.6lowpan-nd-channel\": 12/\"mbed-mesh-api.6lowpan-nd-channel\": 18/' mbed_app.json")
          }

          // Set mbed-os to revision received as parameter
          execute ("mbed deploy --protocol ssh")
          dir ("mbed-os") {
            execute ("git checkout ${env.MBED_OS_REVISION}")
          }

          execute ("mbed compile --build out/${target}_${toolchain}_${radioShield}_${meshInterface}/ -m ${target} -t ${toolchain} -c")
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
      node ("linux_test") {
        deleteDir()
        dir("mbed-clitest") {
          git "git@github.com:ARMmbed/mbed-clitest.git"
          execute("git checkout ${env.LATEST_CLITEST_REL}")
          execute("git submodule update --init --recursive testcases")

          dir("testcases") {
            execute("git checkout master")
            dir("6lowpan") {
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
          execute("python clitest.py --suitedir testcases/suites/ --suite ${suite_to_run} --type hardware --reset --raas 193.208.80.31:${raasPort} --failure_return_value -vvv -w --log log_${raasPort}_${suiteName}")
          archive "log_${raasPort}_${suiteName}/**/*"
        }
      }
    }
  }
}
