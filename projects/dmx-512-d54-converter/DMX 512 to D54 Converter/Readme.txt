DMX512 to D54 Converter
Portia CyberLogic
portia2220@yahoo.com

12 Dec. 2010
------------------------------------------------------------------------------

This is a converter from the popular DMX512 protocol to the dead D54 protocol.

A friend working in selling lighting equipments asked me to save his ass, when he sold a new "Mintor" console to a big client as an upgrade to there "Strand EC90" dimmer,
A ready made solutions are available in the market, but he has a problem of he can't wait the shipping & delivery time of 4 weeks, so I did the whole project in 4 days,

The only D54 available data I found was Wikipedia which is based on old Strand doc. I updated wikipedia D54 topic according what I found later after I finished.

This's the wikipedia link after update:
http://en.wikipedia.org/wiki/D54_%28protocol%29

You may notice that +-12 volt path is not regulated, that's because I only want +-5 volt signal swing & LF353 has high supply rejection ratio.

In the forums I found someone asking how to produce negative voltage from AVR (I can't remember where),

I produced -5 volt sync signal using 2 transistors connected to pin PC7,
When PC7 is low => Q2 collector will give -5 volts,
When PC7 is high => Q2 collector will give 0 volts.
This works fine to this project.
There may be a better way, I'd like to know about.

In the schematic U3 is symbolized as a connector it's 8 pin DIL IC SN75176B, but it's not found in my schematic capture software, so I used 8 pin connector instead.

Sooner I'll post another version of the firmware reads DMX without using USART to free it for other purposes.

I wrote the whole thing in AVR assembly which I love it.

------------------------------------------------------------------------------
Any comments or update are welcomed:
Portia CyberLogic
portia2220@yahoo.com
------------------------------------------------------------------------------
<EOF>