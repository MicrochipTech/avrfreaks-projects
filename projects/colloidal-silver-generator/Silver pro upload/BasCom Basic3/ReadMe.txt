This version of the firmware runs on the atmega8-16 Dip, tqfp or MLF pacakages.

Just placeing this hex file on the Atmega won't make it work right, the fuse setting in this atmega 8 needs to be changed. It comes with the internal oscillator set but it also has the clock divider set, So it's default frequency will be 1 mhz.

If left at the default setting, things will be 8 times slower than they should be, PWM, timing cycles and what not.

The Clk/div fuse setting in the ATMEAG8 needs to be disabled so that It will run at 8 mhz.


If you do not have a way to change the fuse setting, or program the chip,  maybe you have a friend or know of somebody that can do it for you.

Or, you could purchase a programmer yourself.  If none of these options are available or don't suit you needs, then you can always have me send you a pre-programmed ATMEGA8.

You will have to pay for the cost of the IC and shipping and handling.

Robert.

PS. you can get in touch with me via the email address of: support@silverfolk.com