# Power Down Mode Example

Uploaded by danni on 2009-10-14 20:21:00 (rating 0 out of 5)

## Summary

The following example code demonstrate the implementation of the power down mode on the AVR.  

The application light a LED and flash a LED.  

The key can be used to switch both LEDs on and off.  

If the LEDs are on, a timeout of 20 seconds switch off also.  

During the off state, the power down mode was entered.


The code use a very efficient key debouncing routine:  

[https://www.avrfreaks.net/index.p...](https://www.avrfreaks.net/index.php?module=Freaks%20Academy&func=viewItem&item_type=project&item_id=1801)


It use also a macro to access portpins as bitvariables:  

[https://www.avrfreaks.net/index.p...](https://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=67368)


Peter Dannegger

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
