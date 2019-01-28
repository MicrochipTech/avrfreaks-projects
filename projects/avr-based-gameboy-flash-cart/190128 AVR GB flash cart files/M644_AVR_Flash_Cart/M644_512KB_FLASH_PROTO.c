#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include "../01_INCLUDE/usart_funcs.h"
#include "memory_latched.h"
#include "GameBoy_Info.h"
#include "terminal_menu.h"
#include "eeprom_stuff.h"

/*
  AVR Freaks release of this code:
  - For Mega644, but Mega644P or similar series should work - note the placement of the flash addresses though.
  - 25.8048MHz crystal required. I have 15pF caps for mine, check yours
  - Terminal baud rate is 115200

  Because the schematic may be a little hard to understand, and has had a couple of pin changes, the following 
  is what you should ensure your circuit reflects if you want to use this code as it stands.

  Pinout:
    PORTA:
      0 RAM A13
      1 RAM A14
      2 Gameboy_CS (doubles as a CLK line for 1 of the latches)
      3 RUMBLE pack (AKA RAM A16) (effectively a free IO pin if you need it for anything else - it's used in MBC5 only atm)
      4 Flash /WE
      5 Latch /OE
      6 LED2/ mode detect (note! Mode detect is hardwired to PA6 on the PCB, but I no longer use it in this code)
      7 Internal RAM enable (it's a bit of a misnomer, effectively it's the AVR's RAM control pin)

    PORTB:
      - 8-bit port connected to all device data pins, pretty easy

    PORTC:
      0-7 MA14-20 (AKA the ROM banking address lines, note MA19 and MA20 go to the decoder when using 512KB flash)
      8 Gameboy_A8

    PORTD:
      0 RXD
      1 TXD
      2 Gameboy_CLK (doubles as a CLK line for 1 of the latches)
      3 Gameboy_Write (the RAM write line)
      4 Gameboy_Read (AKA RAM and ROM /OE)
      5 Gameboy_A13
      6 Gameboy_A14
      7 Gameboy_A15

  Have fun!

  - Blake
*/

/*
  This project has been created to create the interface functions for the 512KB flash
  chips that I am using in my Gamepak project.

  The main thing to do is get the flash reading and writing without issue first.

  Project stages to date:
  1 - Write a reliable Ymodem transfer function for upload and download
  2 - Added flash memory writing capability, direct to chip without latches and a main menu
  3 - Changed addressing mode to use latches, and added startup mode identification
  4 - Added SRAM and associated SRAM download/dump features, added decoder and SRAM_CS OR gate
    - Trying overclocking! (overclock to get more CLKs for MBC5 later, also to a UART compatible 
      speed!)
    - Overclocking to 25.8048MHz Seems stable... (is below Uzebox speed, so hoping is good)
    - Added OR gate on GB_CLK and INTERNAL_RAMEN lines. May need to swap an LED line for GB_CS.
      The swap may be necessary as GB_CS will be floating when the GB is disconnected otherwise.
      I could try adding a pull down on this line, and then putting a pull up on INTERNAL_RAMEN,
      which will cause the OR gate to be at a 1 state...
    - Adding the decoder...
    - It works! Now I need to modify the SST functions so I can test for presence of memory chips 
      at each decoder address, and write and read from multiple ICs as if they're a single address 
      space
    - Got multi-chip ID working. Need to test writing and dumping a large ROM across both...
    - Attempting to split the terminal menu out of the main project c file. Main work will be
      done in terminal_menu.c now
    - Split complete. I appear to have an issue (possibly undetected) for writing and reading to 
      two or more flash ICs. Trying to figure out what the problem is. It always seems to start 
      around 0x3F00ish
      - This issue moving to 0x5455-ish. It's got something to do with writing to an address >512k
        I cannot for my life figure it out. It's either an issue with the sst_write function, or
        it's an issue with the way the decoder IC switches. I cannot figure it though!!!
    - Due to the issue above affecting ROMs >512k, I'm going to move on and only try ROMs <=512k
        - Cancel that, pushed through and now appear to have fixed the programming issue. I'd say it
      was timing related. I've added an extra NOP cycle in the flash writing function. I've also
      heavily modified the flash functions to simplify debugging (by consolidating functions)
    - Now moving onto EEPROM. Seems to work. Took out anything to do with CLK prescaler changing in
      the write routine, as it broke the YMODEM transfer stuff for some reason...
    - Next: The MBC Assembly code integration...
    - Added some power reduction code
    - Added the .S file. Had to change where my memory mapping was to avoid conflict with the rest of
      the C based programming side of the project. Shifted it up to 32kB, with some jumps just under
      this address
    - Hmmm... Was hoping this would work out of the box better. Alas it didn't, so I'm putting this 
      project to the side to focus on writing the MBC controller code. Means making prototype boards

    30 Dec 18:
  5 - Okay, back to this code because I got some PCBs made
  6 - Debugged the MBC code, guess what, all this GUI stuff and file transfer code was fine. The issue
      was timing as A15 is not set at the same time as the rest of the address pins. GB:CTR revealed 
      this titbit. Changed the pinout from the original revisions of the code, but that was easily fixed.
    - Pay careful attention to your bus read and write timings. The code is optimised for 25.8048MHz.
      I have made comments where I have found timing issues (see the 16M mode stuff)
    - Added mode switch code for between RAM/ROM use of the high bits. Need a game to test this though.
      Worth noting, I may need to change the code to keep the high bits when mode switching, which I
      don't currently do. Nothing says I should discard them though...
    - Added MBC3 without RTC to test Wario Land II (.gb version). It loads, but crashes to the start
      screen after trying to start the first level. I checked the ROM on VisualBoyAdvance too, which
      also crashes around the same spot. Interesting... I don't know what the error could be. No one
      appears to talk about it on the net (that I have found). I didn't look too much at emulating an
      RTC yet.
(7?)- Starting to add MBC5 support to try Pokemon Yellow.
    - MBC5 support added. Pokemon yellow loads, saves and can dump save files. Need that FRAM now...
    - Just found my GBA works with Pok Yel, but crashes on the intro video on the SP and pocket. I
      can't say why as yet, but I'm thinking it's a timing issue. Don't know why there is a difference
      between consoles though. I'm thinking it might be a ROM bank change issue, which means my read
      data cycles might be out of spec. I'll backup this project and see if I can't work it out. It 
      may mean I need to modify all MBC mode code. Testing MBC5 first
    - Fixed MBC3 SRAM access bug. Now Warioland 2 works (at least on the normal GBA)
  8 - I have been unable to resolve the GBA vs GBA SP and GB pocket debacle where neither of the other
      consoles work with the flash cart properly in MBC3 or MBC5 modes. I could play Wario Blast on the
      SP without trouble (MBC1), however changing to other games with other MBC modes immediately brings errors.
      As such, this version of the project will be rewriting the assembly code in an attempt to speed up
      data reads by the GB. My idea is that I will IJMP before A15 is set, and then do a test on that 
      line at the other end. Because this will be an SBIC (or SBIS) and an RJMP, the code will be 1-2
      cycles faster than IN, ANDI, IJMP :) . If I get this correct, this will prove if the GBA SP and 
      GB pocket read off the bus faster than the GBA. The only way this will work is if A15 is always
      held high before choosing hi or lo though. GB: Complete Technical Reference suggests this is the
      case
    - Modified JMP table for MBC3 is ugly, but seems to work better for the GBA SP. GB Pocket Nintendo logo is
      now completely stuffed up though, so as usual, 1 fix breaks another. Although this code seems 
      better for the GBA SP, it doesn't completely resolve whatever the root cause of lock ups are.
      The GBA will still play the game fine, however the SP makes it to the first cutscene and crashes
      after a few seconds. Will continue to work at the issue...
    - I've just thought of an attempt to make DSM work without extra hardware for MBC5. I'll try and 
      read and set PORTC based on the value of A14 before looking to IJMP. That will save READ CLK cycles,
      but will incur additional cycles on writes. Let's see how this goes...

  20 Jan 19
  9 - The last attempt didn't work, but I made a hardware fix to enable DSM to work. I've added muxes 
      between MA (PORTC) pins and the flash address pins, which are selected by A14. This means that
      when A14 is low, 0s are set on MUXA pins. When A14 is hi, MA is set on MUXB pins. This effectively
      makes all reads of lo 16kB to Bank 0, and reads of hi 16kB to Bank XX. Much faster than doing
      PORTC changes by the MCU. Beautiful :) This also seems to have resolved the GBA vs GBA SP issue.
      I can only assume that it was a data read delay that was causing the error between the two consoles
    - DSM code added. The MCU looks to take a reading of CLK between cycle 7-13. If in DSM, CLK will
      have gone lo by this time. If CLK remains hi, then we're still in normal speed mode. Easy.
    - I just tried Bomberman Quest, which fails to run correctly under MBC5. It apparently doesn't
      use DSM, which means it should work like Pokemon Yellow. It crashes after the Nintendo screen
      though and I'm not sure why. I tried changing MBC modes, and the game boots in MBC3 mode, which
      suggests I might have too much lag in the RAM code for MBC5 to make it work. I'm going to try 
      and rewrite the MBC5 code to work like the MBC3 code.
    - Added a menu item to put the AVR into GB Cartridge mode. It's meant to give a sign off message
      before going into the mode, but it doesn't. Strange, but it works as expected otherwise.
    - Hmmm. Bomberman quest works if you put OUT instructions back in while using the multiplexers.
      This is counter-intuitive. The MUX should switch the pins when A14 changes. The only thing I 
      can see is if all zeroes is not being produced by the MUXes, which may be a wiring fault on my 
      part. I'm going to revert the code to make MBC5 use its original code, and not that of MBC3.
    - Code reversion did not work out well. The game runs, but has artefacts and crashes frequently.
      Guess I'll keep MBC3 code and try and figure out why the MUX isn't working as intended...
    - While writing MBC2 code, I just noted A13 is not connected. Instead, MBC2 relies on A8 to 
      identify when RAM enable or ROM bank select is active. I have not connected A8... I may be
      able to connect it to MA21, and make this pin an input. It is not used as an output in any
      MBC mode as my boards don't support a ROM large enough (<=2MB only). It means that I cannot
      currently test MBC2 at all as I cannot tell when I should be ROM banking or RAM enabling...
    - Added a bootloader (separate project). Bootloader uses 115200 baud and YMODEM to be consistent
      with this implmentation. Seems to work well :) Now I can update flash without ISP!
    - Changed the mode detection pin to PD0. This looks for RX being pulled hi. Had to make this change
      because the diode decreases the system voltage. Turns out it'll run on 4.7V, but it's probably not
      great. The only issue with using PD0 is that it'll be a floating input in GB mode, which might
      accidentally put the cart into the wrong mode. Ah well, hasn't stuffed up just yet. This does
      also free up the LED for debug messages!
    - Removed MA21 from the decoder and connected it to A8 in order to proceed with MBC2 testing.
      Playing Kid Icarus, there are some significant stability issues, but the game appears to save
      and restore correctly. I can download a save file to the PC without issue, but may have some 
      errors coming back the other way. I'll have to investigate if I've changed something else by
      accident or if it was the save file format from the emulator that caused issues.
    - Confirmed there appears to be an error in the RAM code for downloading save files to the GB. 
    - I'm thoroughly confused. While reading RAM after writing shows errors, the actual file appears
      to be correct. Que???

    Todo:
    - MBC testing and debugging (including cycle counting) for MBC4M32k and MBC5. I don't think many
      international games used MBC4M32k mode. Japanese games seem to have more usage. I'm just going
      by the number of games with >8kB of SRAM (-> Found 1 for 4M32k mode testing, Warioland 2 -> Nope, may be MBC3)
    * Finish menu items flash new firmware
    * Figure a cunning plan to support MBC5 in double speed mode. I don't think I can though... I'm
      unsure how this will affect games, (other than crashing them). I wish I could detect DSM 
      beforehand when flashing and provide a warning against running that game. There doesn't seem
      to be a flag that states it will be used though. It will just happen somewhere in code...
    * See if I can't emulate support for MBC2 - shouldn't be hard actually, I'll just use my real RAM!
    - See if MBC1-Multicart support can be added - harder to achieve, will require masking on ROM and RAM lines
    - Need to move flash writing code into the ymodem code. This will prove if I can speed up the
      ROM transfer or not. It should be faster as writing a byte to flash should be faster than 
      receiving 8 bits via UART!
    
    - NOTE: This code is going to have to be carefully revised for the board shrink. I've changed
      the data and MA pins around to make routing of the PCB easier. Use a critical eye to test it.
      I'll also need to run a jump wire from A8 to MA21

*/

int main() {

  PORTA |= (1 << PA7); // Pull up INTRAMEN ASAP - may be the reason save files are corrupting without this. Need to test

//  uint8_t startMode = PINA & (1 << PA6); // Mode detect!
  uint8_t startMode = PIND & (1 << PD0); // Mode detect!

  if(startMode) {

    // Set up the flash and SRAM chip port pins
    memory_init();

    // Set up the UART
    usart_init_Xlarge(115200); 
    /* - 57600 seems to be my fastest error free BAUD without optimising my code.
       - Transfer rates at this speed appear to be ~43000bps to the AVR and ~51000bps from the AVR :)
       - I did test taking out the flash write to see where the bottle neck was (is it ymodem or flash).
          When I did that, the download to AVR speed went up to ~51000bps too, which seems to be the max
          feasible speed for 57600. So, is there a way I could streamline this process to get faster 
          programming? I wonder if writing a byte at a time from ymodem is possible. You wouldn't get CRC
          checks until afterwards though. Also, I'm doing a whole chip erase atm when writing, so how would
          I fix this if a CRC error appeared as I haven't got the byte-write program correct!
    */

    // Initialise values
    flashAddress.address32 = 0;
    get_GBROM_info(&gbRomData);
    read_gbeepromdat(&eepData);

    if((eepData.MBCMode != CART_MODE_MBC1_16M) && 
       (eepData.MBCMode != CART_MODE_MBC1_4M) && 
       (eepData.MBCMode != CART_MODE_MBC5) && 
       (eepData.MBCMode != CART_MODE_MBC2) && 
       (eepData.MBCMode != CART_MODE_MBC3)) {

      // Default values are incorrect. Require writing EEPROM values
      usart_transmit_str_P(PSTR("\r\nInvalid EEPROM values detected. Writing default values\r\n"));

      // Set the default mode 
      if(gbRomData.MBCtype < 0x04) {
        // MBC1 or ROM only
    
        if(gbRomData.RAMsizeBytes > 8192 && gbRomData.ROMsizeBytes <= 524288) {
          eepData.MBCMode = CART_MODE_MBC1_4M;
          strcpy_P(eepData.MBCName, PSTR("MBC1_4M "));
        } else if(gbRomData.RAMsizeBytes > 8192 && gbRomData.ROMsizeBytes > 524288) {
          eepData.MBCMode = CART_MODE_MBC3;
          strcpy_P(eepData.MBCName, PSTR("MBC3    "));
          usart_transmit_str_P(PSTR("WARNING: ROM and RAM size both exceed MBC1 mode sizes. Assuming MBC3 mode\r\n"));
        } else {
          eepData.MBCMode = CART_MODE_MBC1_16M;
          strcpy_P(eepData.MBCName, PSTR("MBC1_16M"));
        }

      } else if(gbRomData.MBCtype == 0x05 || gbRomData.MBCtype == 0x06) {
        // MBC2
        eepData.MBCMode = CART_MODE_MBC2;
        strcpy_P(eepData.MBCName, PSTR("MBC2    "));
      
      } else if(gbRomData.MBCtype >= 0x0F && gbRomData.MBCtype <= 0x13) {
        // MBC3
        eepData.MBCMode = CART_MODE_MBC3;
        strcpy_P(eepData.MBCName, PSTR("MBC3    "));

      } else if(gbRomData.MBCtype >= 0x19) {
        // MBC5
        eepData.MBCMode = CART_MODE_MBC5;
        strcpy_P(eepData.MBCName, PSTR("MBC5    "));
      } else {
        // default to MBC116M8k unless someone else says...
        eepData.MBCMode = CART_MODE_MBC1_16M;
        strcpy_P(eepData.MBCName, PSTR("MBC1_16M"));
      }
      program_initial_gbeepromdat(eepData.MBCMode);
      wait_for_continue();
    }

    // Turn off unnecessary peripherals
    PRR |= (uint8_t)((1<<PRADC)|(1<<PRSPI)|(1<<PRTIM0)|(1<<PRTIM1)|(1<<PRTIM2)|(1<<PRTWI));

    show_menu(); // Loops in here. Don't expect to come out.
  
  } else {
    // MBC mode. Jump to the correct handler

    launch_mbc_mode();
  }

  while(1) {
    // But just in case...
  }
  return 0;
}


