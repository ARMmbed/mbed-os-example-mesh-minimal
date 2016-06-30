# Example Mesh application for mbed OS

This applications is the simplest one to utilizes our Mesh networking stack. All it does is join the unsecure *6LoWPAN-ND* network.

See the file `mbed_app.json` for an example of defining an IEEE 802.15.4 channel to use.

## Changing the radio driver

In order to run a 6LoWPAN-ND network, you need a working RF driver for Nanostack. This example uses the Atmel AT86RF233. 

To change drivers:

__1__

```
mbed remove atmel-rf-driver
```

__2__

```
mbed add <your RF driver address>
```

__3__

```
`mbed compile`
```
