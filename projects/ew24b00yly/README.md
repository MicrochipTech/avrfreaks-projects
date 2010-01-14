# EW24B00YLY

Uploaded by Kiklopius on 2010-01-14 09:18:00 (rating 0 out of 5)

## Summary

The driver is meant to be used along with ATNGW100 board and Linux running on it. The driver overtakes system console (or any other that is currently being used), thus standard output stream (text) gets displayed on the LCD EW24B00YLY (it bases on the T6963C LCD graphics controller chip). To achieve that, the driver uses GPIOs which are wired with the board expansion connectors and where the LCD comes connected later. The LCD back light and VEE generator (a negative power supply) are driven by two PWM channels, hence back light brightness and text contrast can be controlled from sysfs.


[http://www.pajkc.eu/programming/...](http://www.pajkc.eu/programming/ew24b00yly-0.2.0/index.html)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
