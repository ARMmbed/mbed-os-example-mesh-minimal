![](./resources/official_armmbed_example_badge.png)
# Example mesh application for Mbed OS

With this application, you can use the [mesh networking API](https://os.mbed.com/docs/latest/apis/mesh-api.html) that [Mbed OS](https://github.com/ARMmbed/mbed-os) provides.

The application demonstrates a light control application, where devices can control the LED status of all devices in the network.
The application can be built for the unsecure 6LoWPAN-ND or Wi-SUN network.

See the [6LoWPAN overview](https://os.mbed.com/docs/latest/reference/mesh-tech.html) for the definition of star and mesh networks. These same principles apply also to Wi-SUN protocol.


## Mbed OS build tools

### Mbed CLI 2
Starting with version 6.5, Mbed OS uses Mbed CLI 2. It uses Ninja as a build system, and CMake to generate the build environment and manage the build process in a compiler-independent manner. If you are working with Mbed OS version prior to 6.5 then check the section [Mbed CLI 1](#mbed-cli-1).
1. [Install Mbed CLI 2](https://os.mbed.com/docs/mbed-os/latest/build-tools/install-or-upgrade.html).
1. From the command-line, import the example: `mbed-tools import mbed-os-example-mesh-minimal`
1. Change the current directory to where the project was imported.

### Mbed CLI 1
1. [Install Mbed CLI 1](https://os.mbed.com/docs/mbed-os/latest/quick-start/offline-with-mbed-cli.html).
1. From the command-line, import the example: `mbed import mbed-os-example-mesh-minimal`
1. Change the current directory to where the project was imported.


## Setup

### Change the channel settings (optional)

See the file `mbed_app.json` for an example of defining an IEEE 802.15.4 channel to use.

### Selecting optimal Nanostack configuration

To optimize the flash usage, select a proper configuration for Nanostack. The configuration depends mostly on the preferred use case.

Select the protocol the network is based on:

- 6LoWPAN-ND.
- Wi-SUN.

Select the device role:

- Mesh network. A router. (default)
- Star network. Nonrouting device. Also known as a host or sleepy host.

Modify your `mbed_app.json` file to see which Nanostack and [Mbed Mesh API](https://os.mbed.com/docs/latest/apis/mesh-api.html) configuration to use.

Example configuration files are provide under `configs/` directory. You may override the `mbed_app.json` with either of these.

|configuration file|Use for|
|------------------|-------|
|`configs/mesh_6lowpan.json` | 6LoWPAN-ND based mesh network. |
|`configs/mesh_wisun_S2LP.json` | Wi-SUN based mesh network. |

An example of the `mbed_app.json` file:

```
...
    "target_overrides": {
        "*": {
            "nanostack.configuration": "lowpan_router",
            "nsapi.default-mesh-type": "LOWPAN",
            "mbed-mesh-api.6lowpan-nd-panid-filter": "0xffff",
            "mbed-mesh-api.6lowpan-nd-channel-page": 0,
            "mbed-mesh-api.6lowpan-nd-channel": 12,
            "mbed-mesh-api.6lowpan-nd-channel-mask": "(1<<12)",
            "mbed-mesh-api.heap-size": 14000,
            "mbed-trace.enable": false,
            "platform.stdio-convert-newlines": true,
            "platform.stdio-baud-rate": 115200,
            "atmel-rf.provide-default": true,
            "mcr20a.provide-default": false,
            "target.device_has_add": ["802_15_4_PHY"],
            "target.network-default-interface-type": "MESH"
        },
```

#### 6LoWPAN-ND

**nsapi.default-mesh-type: LOWPAN**

| Device role           | `"nsapi.default-mesh-type"` | `"nanostack.configuration"` | `"mbed-mesh-api.6lowpan-nd-device-type"` |
|:----------------------|:----------------------------|:----------------------------|:-----------------------------------------|
| Mesh router (default) | `"LOWPAN"`                  | `"lowpan_router"`           | `"NET_6LOWPAN_ROUTER"`                   |
| Nonrouting device     | `"LOWPAN"`                  | `"lowpan_host"`             | `"NET_6LOWPAN_HOST"`                     |

#### Wi-SUN

**nsapi.default-mesh-type: WISUN**

| Device role           | `"nsapi.default-mesh-type"` | `"nanostack.configuration"` |
|:----------------------|:----------------------------|:----------------------------|
| Mesh router (default) | `"WISUN"`                   | `"ws_router"`               |


### Requirements for hardware

The networking stack used in this example requires TLS functionality to be enabled on Mbed TLS.
On devices where hardware entropy is not present, TLS is disabled by default. This results in compile time failures or linking failures.

To learn why entropy is required, read the [Entropy Sources Porting guide](https://github.com/ARMmbed/mbed-os-5-docs/blob/development/docs/porting/target/entropy.md).

See [Notes on different hardware](https://github.com/ARMmbed/mbed-os-example-mesh-minimal/blob/master/Hardware.md) for known combinations of development boards and RF shields that have been tested.

You also need to check how LEDs and buttons are configured for your hardware, and update .json accordingly.

### Changing the radio driver

To run a 6LoWPAN-ND network, you need a working RF driver for Nanostack. This example uses the Atmel AT86RF233 by default.

To change the RF driver modify the `mbed_app.json` file by setting preferred RF driver `provide_default` value to true, For example, to use MCR20a RF driver:

```json
"atmel-rf.provide-default": false,
"mcr20a.provide-default": true,
```

To run a Wi-SUN network, there is currently only one supported Radio, STM S2LP.

Note that in order for [STM S2LP arduino development kits](https://www.st.com/resource/en/user_manual/dm00346440.pdf) to work, a jumper wire is required to connect pin 4 of port `CN2` to pin 6 of port `CN1`.

To use the S2LP, modify the `mbed_app.json` file by setting:

```json
"s2lp.provide-default": true,
```

Note that the provide-default for each radio driver defaults to false, so there is no need to list all the possible drivers in the `mbed_app.json`.

### Connect the RF shield to the board

We are using the Atmel AT86RF233, which you can [purchase](https://l-tek.si/web-shop/l-tek-6lowpan-arduino-shield-2-4ghz/). Place the shield on top of your board, and power it up.

### Update the firmware of the border router

This example supports the following border router:

- [Nanostack-border-router](https://github.com/ARMmbed/nanostack-border-router).

The border router supports [static and dynamic backhaul configuration](https://github.com/ARMmbed/nanostack-border-router#the-backhaul-configuration-options). The static configuration is good for testing, but the dynamic one works if your network infrastructure is supplying an IPv6 address. Make sure that you use the appropiate mode.

Remember to connect the Ethernet cable between the border router and your home/office router. Then power on the board.

## Building and running

1. Connect a USB cable between the USB port on the board and the host computer.
1. Run the following command to build the example project and program the microcontroller flash memory:

    * Mbed CLI 2

    ```bash
    $ mbed-tools compile -m <TARGET> -t <TOOLCHAIN> --flash
    ```

    * Mbed CLI 1

    ```bash
    $ mbed compile -m <TARGET> -t <TOOLCHAIN> --flash
    ```

Your PC may take a few minutes to compile your code.

## Testing

By default the application is built for the LED control demo, in which the device sends a multicast message to all devices in the network when the button is pressed. All devices that receive the multicast message will change the LED status (red LED on/off) to the state defined in the message. The following applies only to the case when the border router is set-up.

As soon as both the border router and the target are up and running you can verify the correct behaviour. Open a serial console and see the IP address obtained by the device.

<span class="notes">**Note:** This application uses the baud rate of 115200.</span>

```
connected. IP = 2001:db8:a0b:12f0::1
```

<span class="notes">**Note:** Connection can take up to 15 minutes.</span>

You can use this IP address to `ping` from your PC and verify that the connection is working correctly.


## Memory optimizations

On some limited platforms, for example NCS36510 or KW24D, building this application might run out of RAM or ROM.
In those cases, you might try following these instructions to optimize the memory usage.

### Mbed TLS configuration

The custom Mbed TLS configuration can be set by adding `"macros": ["MBEDTLS_USER_CONFIG_FILE=\"mbedtls_config.h\""]` to the `mbed_app.json` file. The example Mbed TLS config `mbedtls_config.h` minimizes the RAM and ROM usage of the application. The configuration works on K64F, but it is not guaranteed to work on every Mbed Enabled platform.

This configuration file saves you 8.7 kB of RAM but uses 6.8 kB of more flash.

### Disabling the LED control example

You can disable the LED control example by specifying `enable-led-control-example": false` in the `mbed_app.json`

This saves you about 2.5 kB of flash.

### Change network stack's event loop stack size

Nanostack's internal event-loop is shared with Pelion Device Management Client and therefore requires lots of stack to complete the security handshakes using TLS protocols.
In case client functionality is not used, you can define the following to use 2kB of stack

`"nanostack-hal.event_loop_thread_stack_size": 2048`

This saves you 4kB of RAM.

### Change Nanostack's heap size

Nanostack uses internal heap, which you can configure in the .json.

In `mbed_app.json`, you find the following line:

```
"mbed-mesh-api.heap-size": 15000,
```

For 6LoWPAN, you can try 12kB. For the smallest memory usage, configure the node to be in nonrouting mode. See [module-configuration](https://github.com/ARMmbed/mbed-os/tree/master/features/nanostack/FEATURE_NANOSTACK/mbed-mesh-api#module-configuration) for more detail.

### Move Nanostack's heap inside the system heap

Nanostack's internal heap can be moved within the system heap.
This helps on devices that have split RAM and compiler fails to fit statically allocated symbols into one section, for example, the NXP KW24D device.

Mesh API has the [use-malloc-for-heap](https://github.com/ARMmbed/mbed-os/blob/master/features/nanostack/FEATURE_NANOSTACK/mbed-mesh-api/README.md#configurable-parameters-in-section-mbed-mesh-api) option to help this.

Add the following line to `mbed_app.json` to test:

```
"mbed-mesh-api.use-malloc-for-heap": true,
```

### Use release profile

For devices with small memory, we recommend using release profiles for building and exporting. Please see the documentation about [Build profiles](https://os.mbed.com/docs/latest/tools/build-profiles.html).

Examples:


* Mbed CLI 2

```bash
$ mbed-tools compile -m <TARGET> -t <TOOLCHAIN> --profile release
```

* Mbed CLI 1

```bash
$ mbed compile -m <TARGET> -t <TOOLCHAIN> --profile release
```

## Troubleshooting

If you have problems, you can review the [documentation](https://os.mbed.com/docs/latest/tutorials/debugging.html) for suggestions on what could be wrong and how to fix it.

### License and contributions

The software is provided under Apache-2.0 license. Contributions to this project are accepted under the same license. Please see [contributing.md](CONTRIBUTING.md) for more info.

This project contains code from other projects. The original license text is included in those source files. They must comply with our license guide.

