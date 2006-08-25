High Fuse	=	OxC9
Low Fuse 	=	0x9F

avrdude -p m8 -c usbasp -P lpt0 -u -U lfuse:w:0x9F:m -U hfuse:w:0xC9:m
