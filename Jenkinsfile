// List of targets to compile
def targets = [
  "K64F",
  "NUCLEO_F401RE",
  "NUCLEO_F429ZI",
  "NCS36510",
  "UBLOX_EVK_ODIN_W2"
  ]
  
// Map toolchains to compilers
def toolchains = [
  ARM: "armcc",
  GCC_ARM: "arm-none-eabi-gcc",
  IAR: "iar_arm"
  ]

// Supported RF shields
def radioshields = [
  "atmel",
  "mcr20"
  ]

// Mesh interfaces
def configurations = [
  "6lowpan",
  "thread"
  ]
  
def stepsForParallel = [:]

// Jenkins pipeline does not support map.each, we need to use oldschool for loop
for (int i = 0; i < targets.size(); i++) {
  for(int j = 0; j < toolchains.size(); j++) {
    for(int k = 0; k < radioshields.size(); k++) {
      for(int l = 0; l < configurations.size(); l++) {
        def target = targets.get(i)
        def toolchain = toolchains.keySet().asList().get(j)
        def compilerLabel = toolchains.get(toolchain)
        def radioshield = radioshields.get(k)
        def config = configurations.get(l)

        def stepName = "${target} ${toolchain} ${radioshield} ${config}"
        stepsForParallel[stepName] = buildStep(target, compilerLabel, toolchain, radioshield, config)
      }
    }
  }
}

timestamps {
  parallel stepsForParallel
}

def buildStep(target, compilerLabel, toolchain, radioShield, configName) {
  return {
    stage ("${target}_${compilerLabel}_${radioShield}_${configName}") {
      node ("${compilerLabel}") {
        deleteDir()
        dir("mbed-os-example-mesh-minimal") {
          checkout scm

          if ("${radioShield}" == "mcr20") {
            // Replace default rf shield
            execute("sed -i 's/\"value\": \"ATMEL\"/\"value\": \"MCR20\"/' mbed_app.json")
          }
          
          if ("${configName}" == "thread") {
            // Change interface to thread
            execute("sed -i 's/\"value\": \"MESH_LOWPAN\"/\"value\": \"MESH_THREAD\"/' mbed_app.json")

            //Change operation mode to Thread router
            execute("sed -i 's/\"NANOSTACK\", \"LOWPAN_ROUTER\", \"COMMON_PAL\"/\"NANOSTACK\", \"THREAD_ROUTER\", \"COMMON_PAL\"/' mbed_app.json")
          }
  
          execute ("mbed deploy --protocol ssh")
          // Checkout latest mbed-os master
          dir("mbed-os") {
            sh "git checkout master"
            sh "git pull"
          }
          execute ("mbed compile --build .build/${target}_${compilerLabel}_${radioShield}_${configName}/ -m ${target} -t ${toolchain} -c")
          archive '**/mbed-os-example-client.bin'
        }
      }
    }
  }
}
