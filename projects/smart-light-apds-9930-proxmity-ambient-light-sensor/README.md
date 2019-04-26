# Smart light with APDS-9930 (proxmity & ambient light sensor)

Uploaded by NStorm on 2019-04-26 08:33:00 (rating 5 out of 5)

## Summary

Auto light


 


Attiny10 & APDS-9930 based automatic light/switch


 


<https://github.com/N-Storm/autolight>


 


This project utilizes small ATtiny10 mcu to work with APDS-9930 ambient light (AL) and proximity I2C sensor. This is used for automatic "smart" light in a drawer. Once the drawer are open the APDS-9930 will notice proximity change and trigger a preset interrupt and MCU will come from sleep to check if AL level are below set (i.e. it's dark around). If the set condition for AL level will be met, it will enable HIGH level on PB3 pin. Which is connected to a MOSFET or any other switch to enable LED light.


Reset shares same PB3 pin so it must be disabled via fuses. You will have to apply +12V to PB3/RESET pin to reprogram attiny10 after flashing RSTDISBL fuse.


The PB2 port are used as INT0 for sensor interrupts to wake MCU from sleep on proximity change event.

## Links

- [Sources](https://github.com/N-Storm/autolight)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
- #attiny10
- light control
- Home Appliances
