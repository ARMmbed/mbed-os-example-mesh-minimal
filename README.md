# Example Mesh application for mbed OS

This applications tries to be the simplest one that utilizes our Mesh networking stack.

Basically only thing it does, is to join unsecure *6LoWPAN-ND* network.

See file `mbed_app.json` for example how to define IEEE 802.15.4 channel to use.

## Changing the radio driver

In order to run 6LoWPAN-ND network, you need working RF driver for Nanostack.

Provided example uses Atmel AT86RF233. To replace that do
```
mbed remove atmel-rf-driver
```

and then
```
mbed add <your RF driver address>
```

And then just `mbed compile`
