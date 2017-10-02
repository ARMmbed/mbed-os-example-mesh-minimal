# Notes on different hardware

As this example runs on multiple different mbed OS development boards there might be combinations of board and RF shields that may, or may not, work together due to pin collision or other reasons.

This page aims to collect information regarding different hardware combinations.

## RF shields

Following RF shield have been used with mbed OS mesh examples.

* [FIREFLY 6LOWPAN ARDUINO SHIELD](https://firefly-iot.com/product/firefly-arduino-shield-2-4ghz/)
* [Freedom Development Board for MCR20A](http://www.nxp.com/products/software-and-tools/hardware-development-tools/freedom-development-boards/freedom-development-board-for-mcr20a-wireless-transceiver:FRDM-CR20A)
* [X-NUCLEO-IDS01A4](http://www.st.com/content/st_com/en/products/ecosystems/stm32-open-development-environment/stm32-nucleo-expansion-boards/stm32-ode-connect-hw/x-nucleo-ids01a4.html)
* [DVK-NCS36510-MBED-GEVB](https://developer.mbed.org/platforms/NCS36510/) Development board, contains internal RF chip.

## Tested development boards

Following table shows which development boards have been tested. It does not present our current testing infrastructure, so we cannot guarantee all combinations but we do our best to ensure it is up to date.

| board / RF shield | Atmel | MCR20A | X-NUCLEO-IDS01A4 |
|-------------------|-------|-----|------------------|
| K64F | <span style='background-color: #5f5;'>Yes</span> | <span style='background-color: #5f5;'>Yes</span> | |
| <span style='background-color: #ff5;'>NUCLEO_F429ZI **(1)**</span> | <span style='background-color: #5f5;'>Yes</span> | <span style='background-color: #5f5;'>Yes</span> | <span style='background-color: #ff5;'>Modified, **(3)**</span> |
| NUCLEO_F401RE | <span style='background-color: #5f5;'>Yes</span> | | |
| UBLOX_EVK_ODIN_W2 | <span style='background-color: #5f5;'>Yes</span> | <span style='background-color: #f00;'>No. **(2)**</span> | |
| Onsemi NCS36510 <span style='background-color: #5f5;'>(internal RF)</span> | | | |
| NXP KW24D <span style='background-color: #5f5;'>(internal RF)</span> | | <span style='background-color: #5f5;'>Yes **(4)**</span> | |
| <span style='background-color: #ff5;'>NUCLEO_L476RG **(6)**</span> | <span style='background-color: #5f5;'>Yes</span> | | |


**Notes:**

1. If ethernet driver is enabled, requires HW modifications if RF shield uses SPI1. See [Driver notes](https://github.com/ARMmbed/sal-nanostack-driver-stm32-eth) and [nanostack-borderrouter-private Issue #17](https://github.com/ARMmbed/nanostack-border-router-private/issues/17)
2. Pin collision, see [mesh-minimal Issue 55](https://github.com/ARMmbed/mbed-os-example-mesh-minimal/issues/55)
3. X-NUCLEO-IDS01A4 expansion board required modifications to be used in mbed OS. See [Driver readme](https://github.com/ARMmbed/stm-spirit1-rf-driver)
4. KW24D have MCR20A chip integrated. Use the same driver.
5. KW24D has limited amount of memory (two banks of 32 RAM). For example, for the Thread end device use ARM or GCC_ARM. And for GCC_ARM use [this heap configuration](https://github.com/ARMmbed/mbed-os-example-mesh-minimal/#move-nanostacks-heap-inside-the-system-heap).
6. NUCLEO_L476RF have LEDs on same pins that are used by Atmel RF shield so those should not be used in Light control demo. (LED1 & SPI_SCK == PA_5)
