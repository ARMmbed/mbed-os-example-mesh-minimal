# Example mesh application for mbed OS

This application is the simplest one to utilize our mesh networking stack. It just joins your device to the unsecure *6LoWPAN-ND* network.

## Setup

### Download the application

```
mbed import mbed-os-example-mesh-minimal
cd mbed-os-example-mesh-minimal
```

### Change the channel settings (optional)

See the file `mbed_app.json` for an example of defining an IEEE 802.15.4 channel to use.

### Compile the application

```
mbed compile -m K64F -t GCC_ARM
```

A binary is generated at the end of the build process.

### Connect the RF shield to the board

By default, we are using the Atmel AT86RF233 and it can be purchased [here](https://firefly-iot.com/product/firefly-arduino-shield-2-4ghz/). Place the shield on top of your board and power it up.

### Program the target

Drag and drop the binary to the target to program the application.

### Update the firmware of the border router

You can read the instructions on updating the firmware of your K64F working as 6LoWPAN border router [here](https://github.com/ARMmbed/mbed-os-example-client#mbed-gateway).

Please do not forget to connect the Ethernet cable between the border router and your home/office router. Then power up the board.

## Testing

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

**NOTE:** Please make sure that the _mbed_app.json_ is also updated to reflect the usage of a particular RF driver. For example,

```json
      "radio-type":{
            "help": "options are ATMEL, MCR20",
            "value": "ATMEL"
        },
```
## Important note (Multi-platform support)

mbed OS provides you with total control of the device. However, some defaults are always loaded if you do not provide proper information regarding them. This becomes evident when you switch among platforms. On some platforms, a particular pin might be reserved for a particular functionality (depending upon the MCU) which thus cannot be used generally. If the Ardurino form factor is not available, or the required peripherial is on a different pin, the pins values can be set manually. For example, you can add *"atmel-rf.spi-rst": "D4"* to your `mbed_app.json` file. This will set the SPI_RST pin to D4 of the GPIO.

```json
{
    "target_overrides": {
        "*": {
            "target.features_add": ["NANOSTACK", "LOWPAN_ROUTER", "COMMON_PAL"],
            "atmel-rf.spi-rst": "D4"
        }
    }
}
```

Desired work flow for switching the platform:

1. Check the platform pin map from [mbed Platforms](https://developer.mbed.org/platforms/).
2. Make sure that the desired GPIO pin is free by looking at the data sheet of the particular MCU. Most of the data sheets are available on [mbed Platforms](https://developer.mbed.org/platforms/). 
3. If necessary, change the pin or pins by using the mbed OS config mechanism. To get more information about the configuration system see the [documentation](https://github.com/ARMmbed/mbed-os/blob/master/docs/config_system.md).
