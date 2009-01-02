# Fast/Easy Switch Debouncer

Uploaded by bbumgarner on 2009-01-02 15:29:00 (rating 0 out of 5)

## Summary

See:


[http://www.friday.com/bbum/2008/...](http://www.friday.com/bbum/2008/04/05/using-a-vertical-stack-counter-to-debounce-switches/)


For a full write-up. 


The project uses a reusable vertical stack counter based debouncing routine to debounce 2 switches simultaneously. The code, as is, could debounce up to 8 switches simultaneously (though they would need to be on the same PORT -- you'll need to do a touch of bit-shifting if you want to debounce switches across multiple PORTs).


It could be easily expanded to handle more switches; 8 additional at a time.

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
