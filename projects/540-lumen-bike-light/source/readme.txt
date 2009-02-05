
	Interfacing Compact Flash card to AT90S2313 and playing wave file from it

	Michal Strug 2005

	tested with 32MB card and 12MHz clock, rs232 connection at 57600


To write file into a cf card, it is recommended to set starting and ending sector values in wavcf2avr.asm file (rcv_file routine), and then run cfwriter program with parameter: file name.

To play wave, first turn on device, then communicate with device with any program by rs232 (ex. Docklight), then send one byte (any, to start main loop). Device should answer with '?' char.
Then send one of four characters:
1 - to write file to card (don't use it, only cfwriter program should using it)
2 - to play file1
3 - to play file2
4 - to play file3


Contents of my Compact Flash card:

file:		starting	ending		quality:		file
		sector:		sector:					size:
file1.raw	0x000c3		0x028c6		22 kHz 8bit mono	5 344 768 bytes
file2.raw	0x02b00		0x05e8a		22 kHz 8bit mono	6 755 226 bytes
file3.raw	0x05f00		0x0b092		44 kHz 8bit mono	10 694 144 bytes

raw file means that it is wave file without header (without first 44 bytes for microsoft pcm wave file)
Creative Wave Studio can write waves direct to raw format
Of course you can write all file to cf card and skip first 44 bytes.