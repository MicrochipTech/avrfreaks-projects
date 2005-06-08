speaker/buzzer

because of limited attiny15 flash size it can play kind of short tunes. however i tried to make the code as small as possible, though it can theoretically be made even smaller :)

no schematics, `cauze it's very simple:
  just hook the attiny on the 3V battery, and drop a buzzer/small speaker on 4th io port, program it and it's playing!


by the way, you also need a 1-2kOhm resistor to Vcc on pin1, or you can toggle reset-on-poweron fuse.

Successefully used this one for a christmass card :)

Feel free to use this code.

PS. sorry, no comments in the code. i hope it is self-explanatory. however if somebody want, i will add some comments.



------------------
Andrius Mikonis
University of Vilnius
Physics faculty
Telecomunication physics and electronics

andrius.mikonis@ff.vu.lt
or vienas@delfi.lt