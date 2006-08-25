High Fuse	=	OxD7
1101	SPI Enable
0111	Preserve EEPROM during program, no BOD

Low Fuse 	=	0xED
1110	SUT after BOD
1101	Crystal 3.0 to 8.0MHz, fast rising power

avrdude -p m168 -c usbasp -P lpt0 -u -U lfuse:w:0xED:m -U hfuse:w:0xD7:m
