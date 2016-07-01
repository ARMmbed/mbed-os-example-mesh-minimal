# Example Mesh application for mbed OS

This applications is the simplest one to utilizes our Mesh networking stack. All it does is join the unsecure *6LoWPAN-ND* network.

## Setup

### Download the application

```
mbed import mbed-os-example-mesh-minimal
cd mbed-os-example-mesh-minimal
```

### Optional: change channel settings

See the file `mbed_app.json` for an example of defining an IEEE 802.15.4 channel to use.

### Compile the application

```
mbed compile -m K64F -t GCC_ARM
```

A binary should be generated at the end of the build process.

### Connect the RF shield to the board

By default we are using the Atmel AT86RF233 which can be purchased [here](https://firefly-iot.com/product/firefly-arduino-shield-2-4ghz/).
Place the shield on top of your board and power it up.

### Program the target

Drag and drop to the target to program the application.

### Update the firmware of the Border Router

You can read the instructions on updating the firmware of your K64F working as 6LoWPAN Border Router [here](https://github.com/ARMmbed/mbed-os-example-client#mbed-gateway).

Please do not forget to connect the Ethernet cable between the Border Router and your home/office router. Then power up the board.

## Testing

Once both the Border Router and the target is up and running, then you can verify its correct behaviour. Open a serial console and see the IP address obtained by the device. For example:

```
connected. IP = 2001:db8:a0b:12f0::1
```

You can use this IP address to `ping` from your PC and verify that the connection is working correctly.

### Optional: changing the radio driver

In order to run a 6LoWPAN-ND network, you need a working RF driver for Nanostack. This example uses the Atmel AT86RF233 by default. 

To change the RF driver:

1. Uninstall the default driver:
```
mbed remove atmel-rf-driver
```

2. Install the new driver:
```
mbed add <your RF driver address>
```

3. Recompile your application:
```
`mbed compile`
```
