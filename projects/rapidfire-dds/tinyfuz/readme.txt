
Rapid fire with atmel tiny45
Does not do sleeper mod, just a demonstration of a more advanced signal generator. The output on the two triger pins is a high speed digital signal that when placed with a cap, will filter out the erroneous, higher speed noise.

Advantage of this mod is that it mimics the actual signal, precived by the controller, as if someone was actualy capable of moving at such speeds. It should be much harder to detect than the previous, simple signal mods.

Compile with winavr at winavr.sourceforge.net

Pin 8 is pos +
Pin 4 is gnd -
Pin 5 is Ltrig,
 need 1k resistor between this pin an Ltrig,
 and a .1uf cap bewtween Ltrig and ground
Pin 6 is Rtrig,
 need 1k resistor between this pin an Rtrig,
 and a .1uf cap between Rtrig and ground

Justin Goff
stonefoz@myfastmail.com
