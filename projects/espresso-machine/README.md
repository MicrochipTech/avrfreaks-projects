# Espresso Machine

Uploaded by pLaing on 2007-12-12 03:49:00 (rating 0 out of 5)

## Summary

This project was undertaken to repair a faulty Delonghi Cafe Venezia Espresso Machine. 


The Cafe Venezia is a "pump" machine, with "coffee" and "steam" modes. Heat is controlled by two snap action disc thermostats, 95C for coffee, 125C for steam.  

The thermostats are somewhat light duty for the 1100 watt boiler  

Consequently, they fail after several years if use. They were replaced with relays controlled by an AVR ATTiny24 . Boiler temperature is monitored by a type K thermocouple through an Analog Devices ADC595 thermocouple interface.  

Accurate temperature readings were achieved by using the ADC "noise reduction mode" feature if the MCU.  

Power to the relays is interlocked through a 130C thermostat serving as a safety shutdown. MCU and ADC595 are powered with 5V using a 9V "wall wart" through a 78L05 5V regulator.  

The project was built with "Programmer's Notepad 2"  

The MCU was programmed using the SPI serial bus.  

This is my first AVR project. It was inspired by Lady Ada's "MiniPOV" project. The serial programming interface is the same as on the MiniPOV. The pinout is different as that project used a ATTiny2313.


** CAUTION **.  

The boiler in an espresso machine can be dangerous.  

You MUST ensure that you have installed suitable HARD WIRED SAFETY THERMOSTAT, that will shut down power to the boiler in an overheat situation. The existing THERMAL FUSE must also be retained.  

Proceed with care and common sense. If you are not confident in your ability to do a safe job, DO NOT PROCEED.


RAMBLING NOTES:  

- Opening the Coffee machine without destroying it, can be a major challenge. They use "tamper proof" screws which are difficult to remove. I have had to of these machines open. One used Torx T10, the other, Torx T20 tamper proof screws. I made up a long shank driver by tack welding a bit to an old screwdriver. The tamper proof screws are at the bottom of deep plastic "wells". If you are adventurous, and do not have a suitable driver, you can open it quite nicely by drilling out the top of the wells. Grip a 1/2" drill bit in vice-grip pliers, carefully drill the top of the plastic well until it breaks through. Use "plastic" glue to refit it when you are done.  

- Thermocouple. You can buy one or make it. I made one thermocouple from some type K thermocouple extension wire. You need to weld the junction. A spot welder is best for that. I made do with a MIG welder, turned down to low heat.  

-The original thermostats are clamped on to the side of the boiler with a small spring steel clamp. There is a bolt on the top of the boiler which fastens the clamp. I originally bolted the thermocouple and the safety thermostat on the top of the boiler. Although it worked, it's better to have the thermocouple lower down on the side of the boiler where the original thermostats were. I made a little aluminum angle bracket to pinch the thermocouple against the side of the boiler. Put some heat transfer grease (white heatsink compound) on it. The safety thermostat can bolt on the top of the boiler.  

- Find a small AC Adapter. Mounting it can be a challenge. Keep it away from where water might drip on it. I drilled hole for tie wraps, and used some rubber to prevent rattles. The pump vibrates heavily, and anything loose will rattle.  

- If you have a 12V AC Adapter it may be simpler and cheaper to use relays with 12VDC coils instead of 120VAC. In that case, no opto isolators are required, just a pair of common NPN transistors (2SC945, PN2222 etc.) to drive the relay coils. I used 120V relays because I had them.


** CAUTION **.  

The boiler in an espresso machine can be dangerous.  

You MUST ensure that you have installed suitable HARD WIRED SAFETY THERMOSTAT, that will shut down power to the boiler in an overheat situation. The existing THERMAL FUSE must also be retained.  

Proceed with care and common sense. If you are not confident in your ability to do a safe job, DO NOT PROCEED.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
