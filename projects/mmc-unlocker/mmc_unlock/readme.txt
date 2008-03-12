Images are horribly blurred, sorry (that's what I get for using a camera phone!). Anyway hopefully they'll help to see how I wired mine up.

PORTA	-> grey flat cable ->	switches (only sw0 is used)
PC0/PC1	-> green/yellow    ->	PE0/PE2 (yes, 2, not 1 ;) )
PORTB	-> grey flat cable ->	LEDs (not necessary, just echoes chars typed in terminal)
PD0/PD1	-> white/blue      ->	RXD/TXD (not necessary, just says 'Press a key' and echoes what you type)


The first two listed are really the only useful ones.

The switch is used to start erase.

On the STK500, the expansion header 0 (the one with the MMC card in the photos) has PortC connected to it, but the port pins are staggered; so on any side I'll only get pins 0,2,4,6 or 1,3,5,7, not enough of course. I wanted to connect all 7 pins (incluing power), without having to get or build anything extra.

Pinout of MMC (starting on side with notch / furthest from grey cable) is:
1 / CS	- out, high (inactive)
2 / DI	- in, pull-up
3 / GND	- 
4 / VCC	- 
5 / CLK	- out, low
6 / GND	- 
7 / DO	- out, any


Right after PinC.6 on the expansion header are the power supply pins VTG & GND, and after that is PortE (which isn't used on the processor I'm using, mega16). So I just placed the card with the power pins centered around the power (the card is between a dual-row pin header with some paper to fill in the space). Note the MMC is placed in the photo starting from PinC.2, not PinC.0; this leaves pinc 0 & 1 unconnected for now.

So now the card is connected to pins like this:

Header0    MMC
=======    ===
PinC.2  -> Data Out
PinC.4  -> GND (pc4 is permanently grounded, that's why this pin isn't listed in the source)
PinC.6  -> clk
VTG     -> VCC
GND     -> GND
PinE.0  -> Data In
PinE.2  -> Chip Select

As you can see this sorta fits nicely around the power pins.

Now, remember that PortE is unused by this processor, so we can re-direct the port E pins to PinC.0 & PinC.1 using the pin headers for PortE and PortC (where you plug in grey cables). This is what I used the green/yellow cable for in the diagram.

Now we end up with this:

Header0                MMC
=======                ===
PinC.2             ->  Data Out
PinC.4             ->  GND (pin is permanently grounded)
PinC.6             ->  clk
VTG                ->  VCC
GND                ->  GND
PinC.0  -> PinE.0  ->  Data In
PinC.1  -> PinE.2  ->  Chip Select

