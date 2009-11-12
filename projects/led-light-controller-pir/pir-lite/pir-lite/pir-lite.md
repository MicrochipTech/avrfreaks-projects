Low power LED night-light
=========================


Every time I see household lighting left on overnight for "security",
I think of the coal-smoke belching into the atmosphere to power
it. This kind of silliness has got to stop, also it will and ought to
become prohibitively expensive if carbon-taxing ever gets of the
ground. My kids, however, (aged variously 1-7) have a habit of getting
up several times in the night to bump their way to the bathroom.

We've been leaving a (low wattage) light on to facilitate this. Now,
you can go out and buy motion-sensitive night-light devices for
basically peanuts, but I never did like to do anything the easy way
when I could make a production of it, so I designed my own. It makes a
neat trainer project for the Atmel ATtiny microcontrollers. The
primary design elements are:

- Pre-built Passive-Infrared motion detector module
- 0.5 watt high-brightness LED (or string of LEDs)
- Atmel ATTiny45 Microcontroller
- Ability to override sensor with an on/off switch
- Adjustable brightness via holding down on/off switch
- Timed fade-out to allow enough time to leave the room
- Also suitable (without PIR sensor) for fishtank lighting

## LED Lighting

I orginally chose a 10mm 0.5W LED, because it is bright enough to
light a bathroom or hallway at night, yet does not require any
heatsink.  A resistor is used to limit the maximum current to around
150mA at 5v.

Since I first created this project LED arrays have become stupidly
cheap, typically sold as automotive interior and underbody lighting.

Arrays of 10-100 SMD leds, either in grids or strings are available
pre-assembled with integrated current-limiting resistors suitable for
12v power.  Typically 4 or 5 LEDs are chained with 1 resistor, and 1-20
chains are connected in parallel.  The entire array consumes only tens
or hundreds of milliamps at 12v, and produces a quite impressive
amount of light.  The version of the project presented here is capable
of driving LEDs from either 5v (via regulator) or from the external
voltage input (eg 12v).

## PIR module

Pre-assembled Passive Infra-red sensor modules are available that
combine a PIR sensor, control circuirty and lens.   You simply supply
power and monitor a TTL output line.   One source of these is
http://futurlec.com/

## Circuit and software details

### Voltage regulator - use an MCP1700 or 78L05.   If you are powering
  your LED from the regulator, choose a regulator that can supply
  enough current.

### Microcontroller - an 8-pin ATTiny45 is used here, althoug it is
    hardly worked hard.   Only about 400 bytes of flash is used, and
    there are several pins spare.   There's enough capacity left over
    to implement a communications interface, so your household LED
    lighting could be remote-controlled.   I plan to work on this
    enhancement soon.

### LED control - a transistor is used to switch the LED.   A general
    purpose medium power TO92 switching transistor capable of handling
    around 500ma (eg 2N2222, or 2930??) is suitable.

    The pads for the transistor are arranged to allow use of a TO92 or
    TO220 package.

    A jumper selects whether the LED and transistor are powered from
    regulated 5v or external (12v) power.

    A current limiting resitor may be fitted to the board, or replaced
    by a link when using a LED array with integral resistors.

    The transistor itself may be used to limit LED current if operated
    in analog mode.   I'm a digital guy, so I didn't try that.
    A good guide to selecting a base-current resistor value is at http://....

    The microntroller drives the transistor with a PWM signal,
    allowing the LED (or LED array) brightness may be varied over 256
    steps from 0 to 255.  This allows the main LED to be left in a
    low-brightness state as a night light, and also allows the light
    to be faded in and out gradually, which is kinder on
    sleep-deprived eyes.

### PIR interface - a 3 pin header provides power and ground to the
    PIR module, along with the TTL output signal.  The PIR signal is
    connected to an input pin and monitored via a pin change
    interrupt.

    The PIR module itself inhibits re-triggering for an interval after
    each trigger which can be adjusted via a trimmer pot.

    If the LED array is off, and the PIR triggers, the LED array is
    faded on quickly.  A 5 minute timer is started after which the LED array is
    faded off again.   If a PIR trigger occurs while the array is lit,
    the 5 minute timer is reset.   The effect of this is for the light
    to remain on while you are moving about, and to slowly fade off off five
    minutes after you leave the room.    The on/off switch can
    override the timer for fast-off or remain-on.

### Push-button Switch - Internal pullup is used to hold the button input pin high
    unless the push-button connects the pin directly to ground.
    De-bouncing is performed in software.

    If the LED is off, the push button causes it to fade on quickly
    (over about 1 second).  A release or second push while fading on
    freezes the light at that brightness.

    If the LED is on, the push button causes it to begin fading off
    (over about 4 sec).  A second push during fade-off freezes the
    light at that brighness (a dimmer).  A third push resumes the
    fade-to-black (off).

### Diagnostic LED - There are at least 2 pins spare (more if you 
    disable the RESET pin, or use fewer inputs).  One of them has a 
    normal LED connected to act as a power-on status light, handy for
    finding the on-switch in the dark if you do not employ a PIR sensor.   

    Headers are included for other spare pins for future use.   

    Some possible expansion ideas

    - a photosensor to turn LEDs off if room lighting or daylight is
    present

    - a communication interface (I2C, DOW or RS422) to allow remote
    control of lighting modules by home-automation systems, and use of
    the PIR sensors in as an alarm system


## Construction

The circuit was prototyped and tested on a solderless breadboard,
where the microcontroller was programmed in-circuit.  

A version was then constructed on proto-board, of the type whose
tracks are laid out like a solderless breadboard.  This is a very
handy way of migrating a breadboarded circuit to a soldered
equivalent.

Once the design was proven to be workable, a PCB version was etched.
A few shortcomings were made evident by the first PCB version - some
of the tracks could have been made wider to give resistance to lifting
due to component torquing.   It would have been handy to run 5v to all
the input jumpers, so that active sensors could be connected to any of
the spare pins.

