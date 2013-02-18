# Illuminate

Uploaded by ProjectsByKEC on 2013-02-18 08:03:00 (rating 0 out of 5)

## Summary

After deciding we wanted new matching light fixtures over the couch and kitchen table in our fifth wheel camper, my wife and I picked out a couple of cheap flush mount fixtures from Walmart. Rather than just stripping the 12V DC sockets out of the old fixtures to exchange with the 120V AC sockets in the new, I decided to use LEDs and add some touch sensor circuitry to control them. The control circuit is relatively simple in design, centered around an ATtiny25 microcontroller and the AT42QT1010 touch sensor, also from Atmel. In summary, the microcontroller waits in a low power sleep mode until the light fixture is touched, at which time, the LEDs are turned on through a MOSFET constant current source. When the fixture is touched again, (and again) the lights brighten. A fourth touch turns off the lights and puts the circuit back to sleep. The brightness is controlled by a pulse from the MCU timer output. 


The complete project description can be found on my website:  

[www.projectsbykec.com/projects/m...](http://www.projectsbykec.com/projects/modifications/fifth-wheel/illuminate)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
