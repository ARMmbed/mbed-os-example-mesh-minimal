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

<span class="notes">**Note:** This application uses a baud rate of 115200.</span>

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

## Important Note (Multi-platform support)

mbed-OS provides the developer with total control of the device. However some defaults are always loaded if the user does not provide proper information regarding them. This becomes evident when a user switches among platforms. On some platforms a particular pin might be reserved for a particular functionality (depending upon the MCU) which thus cannot be used generally. A good example of such phenomenon is the use of atmel-rf-sheild with [Nucleo F401RE platform](https://developer.mbed.org/platforms/ST-Nucleo-F401RE/). 
If user do not provide particular pin configuration for the atmel-rf-driver (sometimes a desired behaviour) the driver falls back to a default Arduino form factor, see [atmel-rf-driver pin assignment](https://github.com/ARMmbed/atmel-rf-driver/blob/master/source/driverAtmelRFInterface.h). This fallback mechanism works on most of the platforms, however in the above mentioned case, there is a catch. Fall back mechanism sets the GPIO pin D5 as a designated Reset pin for SPI (SPI_RST) in the radio driver. Whereas this particular pin is assigned by the MCU to debugging in Nucleo F401RE. This will result in hard fault ofcourse. The solution is to map the conflicting pins to a free GPIO pin. For example, the user can add *"atmel-rf.spi-rst": "D4"* to his/her mbed_app.json file. This will set the SPI_RST pin to D4 of the GPIO. 

```json
{
    "target_overrides": {
        "*": {
            "target.features_add": ["IPV6", "COMMON_PAL"],
            "atmel-rf.spi-rst": "D4"
        }
    }
}
```

Desired work flow in such situations (if it may arise) should be:

1.  Checking the platform pinmap from [mbed Platforms](https://developer.mbed.org/platforms/).
2. Making sure that the desired GPIO pin is free by looking at the data sheet of the particular MCU. Most of the data sheets are available on  [mbed Platforms](https://developer.mbed.org/platforms/). 
3. If necessary, change the pin or pins by using the mbed -OS config mechanism. You can get more informations about the configuration system in the [documentation](https://github.com/ARMmbed/mbed-os/blob/master/docs/config_system.md)

## Known Issues

If you are using Thread as the network interface, once your device joins the Thread Border Router, i.e., you can see the IP address assigned to your device, and you hard reset the client device using reset button, the client device will not be able to join the network again and you will see a warning:

```
[WARN][mleS]: dropping packet because mle frame counter is not higher
```

Border router will start dropping packets because of MLE frame counter mismatch.  For more information about MLE frame counter feature see the [MLE-05 RFC](https://tools.ietf.org/id/draft-kelsey-intarea-mesh-link-establishment-05.html#rfc.section.7.6). 
After 240 seconds, the default link timeout triggers the Border router to unregister the lost end device and the BR drops all the counters for that particular device. 
Now your should should be able to join the network again. 

