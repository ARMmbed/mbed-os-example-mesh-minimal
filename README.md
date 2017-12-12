# Example mesh application for Mbed OS

This application is the simplest one to utilize the [mesh networking API](https://os.mbed.com/docs/latest/reference/mesh.html) that that [Mbed OS](https://github.com/ARMmbed/mbed-os) provides.

By default the application demonstrates a simple light control application, where devices can control the LED status of all devices in the network.
The application can be built for the unsecure 6LoWPAN-ND or Thread network.

See [6LoWPAN overview](https://os.mbed.com/docs/latest/tutorials/mesh.html) for the definition of star and mesh networks. These same principles apply also to Thread protocol.

## Setup

### Download the application

```
mbed import mbed-os-example-mesh-minimal
cd mbed-os-example-mesh-minimal
```

### Change the channel settings (optional)

See the file `mbed_app.json` for an example of defining an IEEE 802.15.4 channel to use.

### Selecting optimal Nanostack configuration

If you want to optimize the flash usage, you need to select a proper configuration for Nanostack. The configuration depends mostly on the preferred use case.

Select the protocol the network is based on:

- 6LoWPAN-ND
- Thread

Select the device role:

- Mesh network. A router. (default)
- Star network. Non routing device. Also known as a host, or sleepy host.

Modify your `mbed_app.json` file to tell which Nanostack and [Mbed Mesh API](https://github.com/ARMmbed/mbed-os/blob/master/features/nanostack/FEATURE_NANOSTACK/mbed-mesh-api/README.md) configuration to use.

An example of the `mbed_app.json` file:

```
...
        "mesh-type":{
            "help": "options are MESH_LOWPAN, MESH_THREAD",
            "value": "MESH_LOWPAN"
        }
    },
    "target_overrides": {
        "*": {
            "target.features_add": ["NANOSTACK", "COMMON_PAL"],
            "nanostack.configuration": "lowpan_router",            
            "mbed-mesh-api.6lowpan-nd-device-type": "NET_6LOWPAN_ROUTER",
            "mbed-mesh-api.thread-device-type": "MESH_DEVICE_TYPE_THREAD_ROUTER",
            "mbed-mesh-api.heap-size": 32000,
            "mbed-trace.enable": false
        }
    }
```

The following tables show the values you should use in the `mbed_app.json` file for your devices in different networks.

- For 6LoWPAN-ND based network use `mesh-type: MESH_LOWPAN`.
- For Thread based network use `mesh-type: MESH_THREAD`.

#### 6LoWPAN-ND

**mesh-type: MESH_LOWPAN**

|Device role|`nanostack.configuration` value|`mbed-mesh-api.6lowpan-nd-device-type`|
|-----------|-------------------------|------------------------------------|
|Mesh router (default) | lowpan_router | NET_6LOWPAN_ROUTER |
|Non routing device | lowpan_host | NET_6LOWPAN_HOST |

#### Thread

**mesh-type: MESH_THREAD**

|Device role|`nanostack.configuration` value|`mbed-mesh-api.thread-device-type`|
|-----------|-------------------------|------------------------------------|
|Mesh router (default) | thread_router | MESH_DEVICE_TYPE_THREAD_ROUTER |
|Non routing device | thread_end_device | MESH_DEVICE_TYPE_THREAD_SLEEPY_END_DEVICE |

##### Thread commissioning

By default the Thread application uses the static network link configuration defined in the [mesh-api configuration file](https://github.com/ARMmbed/mbed-os/blob/master/features/nanostack/FEATURE_NANOSTACK/mbed-mesh-api/mbed_lib.json).
If you want to use the Thread commissioning add the following lines to the [thread configuration file](https://github.com/ARMmbed/mbed-os-example-mesh-minimal/blob/master/configs/mesh_thread.json).

* `"mbed-mesh-api.thread-use-static-link-config": false`
* `"macros": ["MBEDTLS_USER_CONFIG_FILE=\"mbedtls_config.h\""]`

### Requirements for hardware

The networking stack used in this example requires TLS functionality to be enabled on mbed TLS.
On devices where hardware entropy is not present, TLS is disabled by default. This would result in compile time failures or linking failures.

To learn why entropy is required, read the [TLS Porting guide](https://docs.mbed.com/docs/mbed-os-handbook/en/5.2/advanced/tls_porting/).

See [Notes on different hardware](https://github.com/ARMmbed/mbed-os-example-mesh-minimal/blob/master/Hardware.md) for known combinations of development boards and RF shields that have been tested.

You also need to check how LEDs and buttons are configured for your hardware, and update .json accordingly.

### Compile the application

#### For 6LoWPAN
```
mbed compile -m K64F -t GCC_ARM --app-config configs/mesh_6lowpan.json
```

#### For Thread

```
mbed compile -m K64F -t GCC_ARM --app-config configs/mesh_thread.json
```

A binary is generated in the end of the build process.

### Connect the RF shield to the board

By default, we are using the Atmel AT86RF233 and it can be purchased [here](https://firefly-iot.com/product/firefly-arduino-shield-2-4ghz/). Place the shield on top of your board and power it up.

### Program the target

Drag and drop the binary to the target to program the application.

### Update the firmware of the border router

This example supports the following border router:

- [Nanostack-border-router](https://github.com/ARMmbed/nanostack-border-router)

The border router supports static and dynamic backhaul configuration. The static configuration is good for testing but the dynamic one works if your network infrastructure is supplying an IPv6 address. Make sure that you use the appropiate mode.

Remember to connect the Ethernet cable between the border router and your home/office router. Then power up the board.

## Testing

By default the application is built for the LED control demo, in which the device sends a multicast message to all devices in the network when the button is pressed. All devices that receive the multicast message will change the LED status (red LED on/off) to the state defined in the message. Note, that the Thread devices can form a network without the existance of the border router. The following applies only to the case when the border router is set-up.

As soon as both the border router and the target are up and running you can verify the correct behaviour. Open a serial console and see the IP address obtained by the device.

<span class="notes">**Note:** This application uses the baud rate of 115200.</span>

```
connected. IP = 2001:db8:a0b:12f0::1
```

You can use this IP address to `ping` from your PC and verify that the connection is working correctly.

### Changing the radio driver (optional)

To run a 6LoWPAN-ND network, you need a working RF driver for Nanostack. This example uses the Atmel AT86RF233 by default.

To change the RF driver:

1. Uninstall the Atmel RF driver.

        mbed remove atmel-rf-driver

2. Install the new driver. (For example, for the FRDM-CR20A radio shield based on the MCR20A device.)

        mbed add mcr20a-rf-driver

3. Recompile your application.

        mbed compile -m K64F -t GCC_ARM

**Note:** Make sure that the `mbed_app.json` file is also updated to reflect the usage of a particular RF driver. For example,

```json
      "radio-type":{
            "help": "options are ATMEL, MCR20",
            "value": "ATMEL"
        },
```

## Memory optimizations

On some limited platforms, for example NCS36510 or KW24D, building this application might run out of RAM or ROM.
In those cases, you might try following these instructions to optimize the memory usage.

### Mbed TLS configuration

The custom Mbed TLS configuration can be set by adding `"macros": ["MBEDTLS_USER_CONFIG_FILE=\"mbedtls_config.h\""]` to the `.json` file. The [example Mbed TLS config](https://github.com/ARMmbed/mbed-os-example-mesh-minimal/blob/master/mbedtls_config.h) minimizes the RAM and ROM usage of the application. The configuration works on K64F, but it is not guaranteed to work on every Mbed Enabled platform.

This configuration file saves you 8.7 kB of RAM but uses 6.8 kB of more flash.

### Disabling the Led control example

The led control example can be disabled by specifying `enable-led-control-example": false` in the `mbed_app.json`

This will save you about 2.5 kB of flash.

### Change network stack's event loop stack size

Nanostack's internal event-loop is shared with Mbed Client and is therefore
requiring lots of stack to complete the security hanshakes using TLS protocols.
In case client functionality is not used, following can be defined to use 2kB of stack

`"nanostack-hal.event_loop_thread_stack_size": 2048`

This will save you 4kB of RAM.

### Change Nanostack's heap size

Nanostack uses internal heap, which can be configured .json. A thread end device with comissioning enabled requires atleast 15kB in order to run.

In `mbed_app.json` you will find the following line:
```
"mbed-mesh-api.heap-size": 15000,
```
For 6LoWPAN, you can try 12kB. For smallest memory usage, you should configure node to be in non-routing mode. See [module-configuration](https://github.com/ARMmbed/mbed-os/tree/master/features/nanostack/FEATURE_NANOSTACK/mbed-mesh-api#module-configuration)

### Move Nanostack's heap inside the system heap

Nanostack's internal heap can be moved within the system heap.
This helps on devices which have split RAM and compiler fails to fit statically allocated
symbols into one section. For example NXP KW24D device.

Mesh API has [use-malloc-for-heap](https://github.com/ARMmbed/mbed-os/blob/master/features/nanostack/FEATURE_NANOSTACK/mbed-mesh-api/README.md#configurable-parameters-in-section-mbed-mesh-api) option to help this.

Add following line to `mbed_app.json` to test:
```
"mbed-mesh-api.use-malloc-for-heap": true,
```

### Use release profile

For devices with small memory, we recommend using release profiles for building and exporting. Please see the documentation about [Build profiles](https://os.mbed.com/docs/latest/tools/build-profiles.html)

Examples:
```
$ mbed export -m KW24D -i make_iar --profile release
OR
$ mbed compile -m KW24D -t IAR --profile release
```

## Troubleshooting

If you have problems, you can review the [documentation](https://os.mbed.com/docs/latest/tutorials/debugging.html) for suggestions on what could be wrong and how to fix it.
