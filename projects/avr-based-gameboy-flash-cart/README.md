# AVR based Gameboy flash cart

Uploaded by Ylennic on 2019-01-28 08:37:00 (rating 0 out of 5)

## Summary

This project marks 20 years since I obtained I my first Gameboy (a Pocket). Feeling nostalgic over Christmas, I pulled out my GB only to discover dead batteries everywhere in my carts. Whilst I could (and have) replaced them, I thought to myself if I couldn't find a way to overcome the inevitability of save game failure in a regular cart. Thus led to an odyssey of discovery on how I could achieve this, mainly deciding if I could in fact design my own flash cart from scratch...


 


There is much about this project that can be improved, and indeed a lot of the sloppiness is due to the amount of free time and resources that I have at my fingertips (and am willing to spend). Either way it's been an education and a fun (yet frustrating at times) path to producing a pretty functional prototype. I want to share it here in the hope that someone else may be interested in aiding me to fix the code, and maybe even expand the capabilities of the design.


 


The long and short of it is that I have a design that works on a GBA quite well, not so well on a GBA SP, and unknown compatibility with earlier revisions of the GB (although my old pocket didn’t seem to like what I did test with it much). If you want to check out the H/W design, I’ve done it on Altium CircuitMaker if you’re interested in setting up similar H/W (<https://circuitmaker.com/Projects/Details/Blake-Newman/181222-MBC-Prototype>)


 


I do make some assumptions that you may have read some of the links during the write up, but here are a couple of critical things to think about for this project:


- The GB has a 16-bit address space, obviously max 64KB of addresses. 


- Only the lowest 32KB of this is reserved for external flash. The rest is RAM and registers. - - There is an external RAM mapping of 8KB somewhere halfwayish up the address space


- To support large games and more than 8KB of flash, ROM and RAM banking is necessary


- Banking works by writing values to ROM. Obviously ROM can’t be written to, but there is an intermediate IC between the GB and ROM, called the memory bank controller, which takes the value that is written and converts this to a ROM page number. The ROM page number is the bits used to control A14 and up on the flash IC.


- The 32KB of accessible ROM space is split into two 16KB sections. The first ALWAYS shows Bank 0 data. The second is where banking occurs to expand the memory size. 


- RAM banking occurs as a swapping of the whole 8KB page.


 


The design specs I started with were:


- the Memory Bank Controller had to be emulated by an AVR, preferably an ATMEGA644 since I had a couple lying around


- I had to code it in AS4, mainly because I was familiar with it, although very rusty, with it (I haven’t pulled out MCUs for a number of years now and didn’t want to have to learn AS7 and any new avr-gcc stuff as an extra hurdle in the time I had on holidays)


- The design had to be self-contained, not requiring an additional device to program the cart


- It would support as many MBC types as possible, starting with MBC1 and preferably being capable of MBC5 double speed mode for GBC compatibility.


- I had to be able to prototype it on a breadboard


 


The chips the design eventually ended up with to prototype were:


- MEGA644


- 1MB flash, using two Microchip SST39SF040 (it’s hard to find 5V flash these days!)


- 32KB SRAM (I had ordered FRAM online before starting, but because I did this on the cheap, delivery took a while and it was prototyped using SRAM)


- 2x 8-bit Latches for to make up for a lack of pins


- a decoder to select between flash chips (mixed H/W and S/W solution like most of this project)


- an OR gate (required to do FRAM pre-charging in H/W)


- 2x quad muxes to do fast switching of RAM banks 


 


So yes, the design is a little unwieldy, but the idea wasn’t to be doing this project with a CPLD, which could have replaced many of the parts!


 


Stage 1: Can an AVR even Gameboy?


 


Stage 1 of this project involved a fair bit of research. Researching how an MBC worked, what the functions of the MBC were, how they differed, etc. There was only evidence of one person considering an AVR as an MBC that I found, and an Arduino at that, which had fallen flat (<https://forums.nesdev.com/viewtopic.php?f=20&t=11173>). This led me to question if I was sunk too, but also heightened the challenge. I sat down and did the frequency/cycle calculations myself, and came to the conclusion that it might be on the borderline. 


 


Before spending money on additional parts, I wondered if an AVR itself could be a flash cart. As mentioned, I had M644s at my disposal, which have a wealth of flash. The smallest GB games were 32KB, and had no MBC (called ROM-only). I figured it was easy enough to test this theory, and hacked up an old GB game to gain access to a functional cartridge edge connector (RIP Nigel Mansell’s World Championship Racing). It turns out that yes, with a bit of ingenuity, 20MHz is sufficient to play tetris directly from an AVR! This bolstered by hopes that MBCs would be possible. I thank this page for it’s useful pinout info <https://gekkio.fi/blog/2015-02-28-mooneye-gb-cartridge-analysis-tetris.html>


 


 


Stage 2: What do you need to pretend to be an MBC?


 


The main types of MBC were:


- MBC1 (most common for GB games)


- MBC2 (built-in 4-bit RAM, not too common)


- MBC3 (fancy because of its built-in RTC that was well known for Pokemon Gold/Silver)


- MBC5 (introduced for GBC, can be found in GB backwards compatible games, and only one that I’ve found that needs to support double speed mode)


 


Each has different features, which directly relate to how much flash it supports, how much RAM you can connect to it and if it has any special features (tilt, rumble, RTC etc…). I put together a map of the required pins to hook up to the AVR and easily discovered they exceeded what the MCU had. With some careful consideration of what my design parameters were (eg, I would only support up to 2MB of flash, and therefore required no pins to support up to 8MB), I came to the conclusion that the design was doable, however would be right on the edge (and indeed I have had to hack the board a number of times to support things I did not think about originally). This is what led to the pinout of the AVR in the design. It was also constructed to take advantage of bit masking and memory jumps in the minimal number of instructions.


 


I also reconsidered the number of clock cycles I had available to me and support for loading games via UART. The fastest F\_CPU to the rated M644 speed is 18.432MHz. Too slow. I have read the opinions of people about overclocking AVRs and the likely effects, and decided that I would look beyond 20MHz anyway. The good news is UzeBox has been a fine example of where M644s (another coincidence) appear to handle overclocking without too many detrimental effects. I ended up choosing 25.8048MHz as a nice UART compatible speed, and a little lower than UzeBox, just in case. This proved to be an excellent choice, and I’ve not had cause for concern from this speed boost.


 


The best references I found for MBC pinouts were usually images on Google search, however the best information I found on MBC functions was at <http://gbdev.gg8.se/wiki/articles/Memory_Bank_Controllers>


 


From here I proceeded to getting some code running. Somewhat foolishly, I didn’t even work on the MBC code until after getting my GUI loading software working. Some of that was due to the fact that I couldn’t test without a game anyway!


 


Stage 3: The GUI side of house


 


The GUI side of house is meant to be the admin side of the cart that the Gameboy never sees. It only runs when you’re hooked up to a PC, and only affects how the cart functions when in use, never interfering in the data (except as an MBC would!). The aim was to do transfers via serial connection, and my serial terminal application of choice was HyperTerminal, as it also supported various file transfer protocols.


 


Coding was a bit of a staged process, which included:


- Getting a suitable file transfer method from the PC – I started with XMODEM, but moved to YMODEM, due to the advantage of knowing file size (and XMODEM caused me some trouble– I have since fixed this even though it isn’t used).


- Making the file transfer bi-directional


- Programming the flash without latches (needed the file transfer protocol first!)


- Adding a menu to select whether a download or dump needs to occur


- Adding latches to free up IO


- Adding a decoder for multi-flash chip support and then addressing the ICs as a single address space


- Supporting the RAM IC (much easier than flash!)


- Adding EEPROM support (built in, yes, but still crucial!)


- Adding mode detection (is it GB or UART connection?)


- Adding a bootloader using YMODEM (this wasn’t done until very recently though)


 


The order may have swapped here and there, and bug fixing usually arose out of sequence as I supported more features that pushed the functionality of the code (and as I thought up more test cases). I actually ended up putting the whole design on a breadboard, which was the biggest tangle of wires I’ve ever seen, but it worked. Now I could move onto the MBC code…


 


Stage 4: MBC support


 


This stage was the first major setback. The project is mixed C and assembly, the GUI being in C and the MBC code in assembly (no real surprise as cycles are only critical in MBC mode). I wrote the first MBC code (MBC1) based off the gbdev info, which I thought would work out of the box (it was a naïve hope based on the ease of doing ROM-only support earlier). Because of the timing constraints of the cart, I couldn’t debug easily with all that was in place and I ended up tearing the breadboard circuit apart in an effort to go back to basics and simplify the effort. 


 


Before destroying the carefully constructed web of wires, I put tetris onto one flash IC and super Mario onto another. This way, I could see if the problem was related to the M644 or the flash, and as the project progressed (and I rebuilt the complexity of the circuit) I could move from simple games that had no ROM banking, to a small game with ROM banking (although no SRAM). 


 


The critical part of pretending to be an MBC is the GB bus timing, specifically when is data good to read, and when must you present it back to the GB. Unsurprisingly, this is why flash carts normally use CPLDs, because this is all hardwired and fast. Using an AVR to do the job is a bit like replacing a plumbing tree with a man who runs the contents from one pipe to the others, whilst deciding where the cup should be dumped. He may miss something at either end, which just ends up breaking everything and making a mess. The following references were my initial gotos for the GB bus protocol:


<https://dhole.github.io/post/gameboy_cartridge_emu_2/>


<https://www.insidegadgets.com/2018/04/18/building-a-2mb-mbc5-gameboy-cart-part-1-cpld-as-the-mbc-and-adding-flash-as-our-rom/>


<http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf> (p137 for the protocol diagram)


 


Note that whilst *mostly* accurate, these documents omit a critical fact that rears its head when not using a CPLD. The Gameboy address lines are not all changed at the same time. A15 is delayed. Finding this actually held me up for a few weeks, and I designed and sent off for a PCBs prototype in an effort to prove that the breadboard was the issue. I had suspicions that it wasn’t as everything else worked, but couldn’t prove it definitively without test equipment. It wasn’t a bad decision in hindsight, but it was something I could have potentially overcome with a little more attention. The upside was that I had a nice PCB that I could plug straight into a GB (specifically a GBA)! The big reveal about the bus timings came when I finally stumbled upon the following:


<https://gekkio.fi/blog/2018-02-05-errata-for-reverse-engineering-fine-details-of-game-boy-hardware.html>


<https://gekkio.fi/files/gb-docs/gbctr.pdf> (Gameboy: Complete Technical Reference)


 


All was so clear! So, with a calculator I sat down and determined the delays I needed to read true data and lo and behold, I played tetris!


 


Stage 5: More MBC modes!


 


Getting SRAM up and running was trivial after getting banking working, and I literally followed what gbdev said. MBC1 support was pretty good by now, but there is still some doubt in my mind as to what the functionality of ROM hi bits/RAM bank select should be when you switch from 16M/8k mode to 4M/32k mode (read the gbdev doc to see where this arises).


 


I was soon considering how to expand my repertoire from MBC1 compatibility. I figured MBC5 was the next most useful, as it covered GBC games. Conveniently, it is also the most cycle efficient mode, as it requires little pin masking for its functions. Pokemon Yellow was my test ROM in this case, and I had it running without too many issues. Around this time, I noted that my GBA was quite happy with my test hardware, but if I plugged my board into the GBA SP, it was a different story. I still haven’t solved why this is the case, but I suspect the hardware must run slightly differently.


 


I didn’t add MBC5 double speed mode support at this time, as I was already pushing my luck with reading data on time in the other modes. The issue is that you must set the correct ROM bank output within 10 cycles of CLK going high (@ 25.8048MHz), else the game will hang. I was doing this with bare nanoseconds to spare for a 1.05 MHz cartridge clock, which obviously meant that I had buckly’s of doing it at 2.10MHz. It’s a little out of sequence, but I’ll discuss the solution in the following paragraph.


 


The answer to getting double speed mode working was to put a mux (AND gates probably would work as well) on the lines from the AVR to the flash and using A14 to select whether bank 0 or the AVR latched bank should be used. Bank 0 was set on the mux by grounding all the inputs, an idea I’d flogged from a hardware design for a Sega Master System Slot-2 flash cartridge. To make it clear why A14 is used, this line selects the upper or lower 16KB of the flash space. Neat! I have had some unknown issues with this fix, which I have yet to explain (once again due to lack of test equipment), namely that I still had freezes when playing some games which pointed back to Bank 0 or Bank XX not being selected correctly by A14. That said, I have not had an issue when in double speed mode, for which this hardware change is crucial.


 


Back to other MBC modes. I moved onto MBC3 next, as how I was going to emulate the built-in 512 bytes of RAM on an MBC2 eluded me (though it turned out this wasn’t too much of a problem!). MBC3 was a bit of a pain in the backside because of the RTC. RTC values are read and written from the RAM address space. Because of this, I needed to overcome the fact that normally I had very little involvement in RAM access, which aside from enabling or disabling the RAM, was done in hardware. Now I needed to make sure I was disabling the physical RAM when the RTC was selected, and somewhat vice versa. I ended up finding my code was not fast enough to do this, which was discovered by loading Pokemon Red. The start screen of the game normally shows pokemon that swap out through a cycle. The sprites for these pokemon are stored in RAM. All I saw was corruption, which indicated that the RAM was either not being read from or written to in time. This forced some creative thinking as to how to make the MBC code run faster. 


 


The crux of the functionality of all of my MBC modes is that the AVR will mask reading A15, A14, A13 and READ to determine what function it should be executing. These lines are connected to a high nibble on a port. Because decision trees are slow, I came up with the idea of creating a jump table in flash memory, and using IJMP to jump directly to the code that should be running for the bits presented (this is how I made the original ROM-only idea work). It would have been fine concept, if not for the NOPs that I had to introduce because of the delayed setting of A15. If I could determine a way to jump earlier, I could save myself a couple of cycles!


 


The solution turned out to be accepting that A15 would be set late, and jumping regardless. This meant that I still needed to read A15 after the jump, but the combination of instructions required to do this (SBIC and SBIS) were faster at the other end, saving me 1-2 cycles. This proved to be sufficient for MBC3 RAM access to work. Of course, the trade-off is that my jump tables are now no longer very neat. Ah, the sacrifices.


 


Now that this was functional, I turned to MBC2. The supposed advantage of this MBC for game makers is that it required no additional RAM chip, although it only provided a limited amount of storage itself. I was concerned as to how I would implement this with the AVR, when I realised that I had no need of internal RAM access, as my flash cart already had 32KB that was freely available! Unfortunately, I discovered that I did not map all the required pins from stage 2 as thoroughly as necessary. 


 


There is another key difference in MBC2 from the remaining MBCs. As mentioned, the others use A13-15 to determine what function they should perform. MBC2 does not connect A13, and instead has A0-A8 connected directly. This makes sense for the purpose of addressing the chip’s 512 RAM addresses, but stuffed up my jump table concept because the functions of ROM banking and enabling/disabling RAM are normally selected by A13. Instead, MBC2 relies on the value of A8 to differentiate. So with a soldering iron and some reallocation of pins I removed the pin used to allow 8MB of flash – only 1 game out there even had this much ROM – and connected A8. Using the same pre-A15 IJMP table as MBC3, I had plenty of time to read A8 and make MBC2 work.


 


Stage 6: Tidying it up


 


This stage has mainly been about trying to decrease the prevalence of freezes and crashes. I’ve about exhausted my own abilities, and since I’m back at work, I don’t have a lot of time to keep chasing it. The one thing that still eludes me is a pseudo-RTC for MBC3. I haven’t figured a way to get a value out within the 10 cycle window.


 


I’ve also revised the PCB design and done a board shrink, which is now 1.5cm larger than a regular cart (but not bad for what I thought it might be!). My aim for the board shrink was not to use smaller than SOIC packages, because I can’t solder smaller components with my tools. I’d be happy for someone to have a crack at a redesign though. The hardest part for the shrink was finding a suitable 5V memory IC. I ended up finding a Macronix 1MB chip, which I’ve put two of onto the board. There will be a fair few code changes required too, as I changed the AVR pinout to make routing easier. I’m yet to receive the boards, but hope to assemble one over the next couple of months.


 


I intend to throw the necessary code up for my prototype work as is. It’s a bit ugly, but not too unwieldy and there is commenting, although the quality could be better. 


 


I’m happy to have a crack at answering questions on this topic too, should anyone prove interested.


 


Update (03 Mar 19): Added link below to board shrink project and uploaded compatible revision of code for board shrink. Fuse settings (previously forgotten!) should also be (double check yourself though):


HF: 0xD8 (disable JTAG and set BOOTRST) 


LF: 0xD7 (disable CKDIV8, Full swing crystal, 16k + 14k CK)


EF: 0x04 (4.3V brown-out - probably too low, but is a just in case measure)


[https://circuitmaker.com/Project...](https://circuitmaker.com/Projects/Details/Blake-Newman/190119-AVR-MBC-Shrink)

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
- Portable Game Consoles
