PS/2 Keyboard to TI-99/4A Scan Matrix Adapter
---------------------------------------------
by M.Brent - http://harmlesslion.com/software/ps2
---------------------------------------------

Software Version 1.3 - Board Rev 2 (untested)

This is my PS/2 keyboard to TI-99/4A adapter! Due to time constraints
on my part, I decided to release it and let people build their own.

However, you may note no source code. At this time I'm not interested
in releasing the source code.

This code is intended for the Atmel ATMEGA16 running on a 20MHz 
oscillator clock. Yes, I know the max rating is 16MHz, and I know
overclocking instead of optimizing the code is lame, but I only
needed just that little bit more. ;) It seems to run quite
reliably at this clock rate, but of course, you take your own
risks. At 16MHz you will occasionally see incorrect keypresses,
but you can use it if you want.

There should be a folder in here containing the Schematic and PCB
layout - it's done using the software from ExpressPCB.com. They
worked well enough for me - if you want to use someone else I'm
afraid you'll have to deal with that yourself. :)

Please pay attention to the licensing I've enclosed. In a nutshell,
it says you may use this code yourself however you like, but you
can not distribute it or anything based on it for money - that
includes non-profit! If you want to go into business making and 
selling these, or if you want to organize a user-group buy, please 
bring me into the loop. I'll give you a discount on bulk programmed 
and tested processors. ;)

That said, I'll sell anyone pre-programmed and tested processors,
in case you don't have a way to program the Atmel. The circuit appears
to work fine wirewrapped (there's just not much to it, really, it's
all software.)

To use it, just write ps2toTI.hex into your Atmel MEGA16. Set your fuses
as follows:
-Clear On-Chip Debug
-Clear JTAG Interface (very important!!)
-Clear Preserve EEPROM memory through the Chip Erase cycle
-Set Boot Flash section size=128 words
-Clear CKOPT fuse
-Set Brown-out detection level at VCC=4.0V
-Set Ext. Clock; Start-up time; 6CK + 64ms

I can't help you a lot with the parts, especially the cable
connectors (this is one reason I'm not making them). The most important
parts, though, are available at Digikey:

U1	ATMEGA16PI	ATMEGA16-16PI-ND
U2	OSC 20MHz	CTX199-ND

// Hooking it up:
// PortA is all output to CRU (rows)
// 0 - >0006
// 1 - >0008
// 2 - >000A
// 3 - >000C
// 4 - >000E
// 5 - >0010
// 6 - >0012
// 7 - >0014
//
// PortB is only used during debugging and is not active in the release
//
// PortC is all input from keyboard (columns):
// 0 - Column 0 (pin 12) LSB
// 1 - Column 1 (pin 13) 
// 2 - Column 2 (pin 14) 
// 3 - Column 3 (pin 15) 
// 4 - Column 4 (pin 9)  
// 5 - Column 5 (pin 8)  
// 6 - Alpha lock (pin 6)
// 7 - future use (wire to joystick 2/pin 7 for mouse support?) MSB
//
// PortD is general IO:
// 0 - Serial RX for debugging only (disabled in release builds) (19.2k 8N1)
// 1 - Serial TX for debugging only (disabled in release builds) (19.2k 8N1)
// 2 - PS/2 Clock input (interrupt 0)
// 3 - PS/2 Data IO
// 4 - Optional output - Alt-F9 pressed
// 5 - Optional output - Alt-F10 pressed
// 6 - Optional output - Alt-F11 pressed
// 7 - Optional output - Alt-F12 pressed
//
// Run this puppy at 16MHz or 20MHz (external clock)
// 20 is preferred, occasional glitches at 16.

Scroll Lock has a function that is not quite intuitive - it toggles the mode of the arrow
keys. When scroll lock is OFF, the arrow keys function as though you pressed FCTN-E/S/D/X.
When scroll lock is ON, the arrow keys simply return E/S/D/X, and so may be used in games
and other programs for navigation.

Alt maps to FCTN, although it should be rarely needed now. The old keymaps should mostly
still work (ie: ALT-I will return a question mark!) Alt-Equals will still be quit. The
editor keys are based on the mappings used in the TI Editor/Assembler package and TI BASIC.
Some of these mappings may not be available to BASIC (for example, Page Down is FCTN-4,
so it will terminate a BASIC program). Likewise for F4.

As a side effect of the ref-counting and mapping code, SHIFT on the PS2 keyboard does
NOT map directly to SHIFT on the TI side. Normally this is transparent, but some devices,
such as the Horizon RAMdisk, use holding SHIFT as an indication to bypass boot code. If
you need this functionality, use the Windows key on a Windows keyboard - this maps
directly to the TI SHIFT key. Note your keyboard must be able to start up quickly enough
to beat the RAMdisk boot, but most seem to.

Finally, Control-Break will actually end up sending CTRL-FCTN-4 - this is an invalid
character to KSCAN, but it will terminate a running BASIC program. It doesn't work in 
all versions of BASIC, though. (You can also press F4, Alt-4, or Page Down, as they 
all map to FCTN-4). 

Please note that this device is only tested on a stock 99/4A machine. Accelerated machines
may be able to scan the keyboard faster than this device can respond, which may cause
ghosted keys or incorrect keystrokes to appear. If you test with an accelerated machine,
I'd be interested in knowing if it worked.

Extended Keys:
PC Key			TI Keys
--------------  -----------------------------------
Up Arrow		FCTN-E (see note above on Scroll Lock)
Down Arrow		FCTN-X (see note above on Scroll Lock)
Left Arrow		FCTN-S (see note above on Scroll Lock)
Right Arrow 	FCTN-D (see note above on Scroll Lock)
Tab				FCTN-7
F1				FCTN-1
F2				FCTN-2
F3				FCTN-3
F4				FCTN-4
F5				FCTN-5
F6				FCTN-6
F7				FCTN-7
F8				FCTN-8
F9				FCTN-9
F10				FCTN-0
Insert			FCTN-2
Delete			FCTN-1
Page Down		FCTN-4
Page Up			FCTN-6

------------------------------------------------------------------------
Also, a number of keys with no direct analogy were mapped into control
keys for use with a TI program, if you choose to use them. Test them
first as they are not guaranteed the same on all keyboards (or to even
exist for that matter!).
------------------------------------------------------------------------
Left Windows	Shift (use this for RAMdisk boots, etc)
Right Windows	Shift (use this for RAMdisk boots, etc)
F11				CTRL-1
F12				CTRL-2
Home			CTRL-U
End				CTRL-V
Esc				FCTN-9 (back)
WWW Search		CTRL-N
Print Screen	CTRL-A
WWW Favorites	CTRL-T
WWW Refresh		CTRL-S
Volume Down		CTRL-I
Mute			CTRL-G
WWW Stop		CTRL-R
Calculator		CTRL-L
Windows Apps	CTRL-Z
WWW Forward		CTRL-Q
Volume Up		CTRL-H
Play/Pause		CTRL-F
Power			CTRL-C
WWW Back		CTRL-P
WWW Home		CTRL-O
Stop			FCTN-4
Sleep			CTRL-D
My Computer		CTRL-M
EMail			CTRL-K
Next track		FCTN-6
Media Select	CTRL-J
Wake			CTRL-E

------------------------------------------------------------------------
There are some external control keys you can hook up any way you like 
(for instance, tie one to the console reset line and you should be 
able to reset the console!) These lines will go low while you press 
Alt-F9 through Alt-F12. They will return to tri-stated when you release
the function key. 

You can hold combinations of the keys, too, although it is not 
recommended to hammer on them too hard. :)

Note that you can not use this to reset the console using pin 1 on
the TI cartridge port. Pin 1 on the cartridge port requires -5v to reset,
which is not available to this system. However, if you connect the
output to pin 5 on the TIM9904 chip (a small 20-pin chip located near
the clock crystal that manages clock and reset), you can reset the
console using this. No promises or guarantees that this is safe for
the console or the PS/2 controller, but it works for me! :)
------------------------------------------------------------------------
Also, pressing Ctrl-Alt-Delete will reset the keybord controller, If
it has lost state but is still able to read keys, this will reboot the
controller entirely (and will not affect the TI(*)). Press Ctrl-Alt-Delete,
release, and wait about 1 second for the LEDs to blink. Do not press
any keys while you are waiting (the reset may fail otherwise). On reset
the controller expects that the keyboard was powered on with it, and
waits for a second to hear from the keyboard before forcing it to be
reset. 

(* - Note: if you have connected the output pins, efforts have been
made to ensure they do not toggle during controller reset, but this
can not be guaranteed! It *probably* will not toggle, but be aware
that it is possible!)
------------------------------------------------------------------------

//
// (C) 2006 Mike Brent aka Tursi aka HarmlessLion.com
// This software is provided AS-IS. No warranty
// express or implied is provided.
//
// This notice defines the entire license for this software.
// All rights not explicity granted here are reserved by the
// author.
//
// You may redistribute this software provided the original
// archive is UNCHANGED and a link back to my web page,
// http://harmlesslion.com, is provided as the author's site.
// It is acceptable to link directly to a subpage at harmlesslion.com
// provided that page offers a URL for that purpose
//
// Source code, if available, is provided for educational purposes
// only. You are welcome to read it, learn from it, mock
// it, and hack it up - for your own use only.
//
// Please contact me before distributing derived works or
// ports so that we may work out terms. I don't mind people
// using my code but it's been outright stolen before. In all
// cases the code must maintain credit to the original author(s).
//
// -COMMERCIAL USE- Contact me first. I didn't make
// any money off it - why should you? ;) If you just learned
// something from this, then go ahead. If you just pinched
// a routine or two, let me know, I'll probably just ask
// for credit. If you want to derive a commercial tool
// or use large portions, we need to talk. ;)
//
// Commercial use means ANY distribution for payment, whether or
// not for profit.
//
// http://harmlesslion.com - visit the web page for contact info
//
