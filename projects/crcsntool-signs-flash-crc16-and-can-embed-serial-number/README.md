# CRCSNTOOL signs flash with a crc16 and can embed a serial number

Uploaded by alank2 on 2015-09-27 00:52:00 (rating 5 out of 5)

## Summary

CRCSNTOOL 1.02  

\_\_\_\_\_\_\_\_\_\_\_\_\_\_


 


This tool can:


  Sign flash code with a crc16 so it can validate itself at runtime.  

  Embed a 32 bit serial number that can be retrieved by the code at runtime.  

  Update HEX and ELF files.


 


It addresses all the shortcomings of using srec\_cat for this purpose:


  SREC\_CAT only works on HEX files.  

  The SREC\_CAT crc method fills flash code to maximum size increasing programming time.  

  The lack of ELF update capability means that debugging must disable crc checking.



The way it does this is by using a PROGMEM string with a specific signature.  The tool locates the PROGMEM string using the signature and then updates the CRC and Serial Number.  Then early in the code you can calculate the crc16 of the flash and compare it to the singature written by the tool.  If it matches then the code is the same, if not it is corrupted.


The first thing to do is to add this line to your source code:



```

  const char MsgCRCL[] PROGMEM = "cRcL032xxyyyyzz";
```
The "032" in this line must be changed to the amount of flash you have in kilobytes.  The rest of this string must be left alone.


The "xx" is a placeholder for the crc16 of flash code.  

The "yyyy" is a placeholder for the 32bit serial number.  

The "zz" is a placeholder for the crc16 of the serial number.


You may need to add these includes to your source code if you don't have them already:



```

  #include "string.h"
  #include "avr/pgmspace.h"
  #include "util/crc16.h"
```
To test the crc, use this code.  It can handle anything smaller than 64K, but will be easy to modify if you need to test larger flash sizes.



```

    //test crc (code works on less than 64K, will need to be modified for 64K or larger
    {
      uint16\_t ui1,ui2,ui3;

      ui2=((pgm\_read\_byte(MsgCRCL+5)-'0')*10+(pgm\_read\_byte(MsgCRCL+6)-'0'))*1024;
      ui3=0xffff;
      for (ui1=0;ui1<ui2;ui1++)
        if (ui1<(uint16\_t)MsgCRCL+7 || ui1>(uint16\_t)MsgCRCL+14)
          ui3=\_crc16\_update(ui3,pgm\_read\_byte(ui1));
      ui3=~ui3;
      if (ui3!=pgm\_read\_word(MsgCRCL+7))
        {
          //crc failure

          //debug indicate failure here

          for(;;);
        }
    }
```
To test and retrieve the serial number, use this code:



```

    //serial no
    {
      uint16\_t ui1,ui3;

      ui3=0xffff;
      for (ui1=0;ui1<4;ui1++)
        ui3=\_crc16\_update(ui3,pgm\_read\_byte(MsgCRCL+9+ui1));
      ui3=~ui3;
      if (ui3!=pgm\_read\_word(MsgCRCL+13))
        {
          //crc failure

          //debug indicate failure here

          for(;;);
        }
      else memcpy\_P(&serialno,MsgCRCL+9,4);
    }
```
In AVR Studio 6 you can automatically call this tool as a post build event by going to project properties, build events, post build event command line and add:



```

  crcsntool "$(MSBuildProjectDirectory)\$(Configuration)\$(OutputFileName).hex" "$(MSBuildProjectDirectory)\$(Configuration)\$(OutputFileName).elf"
```
This will update both the HEX and ELF files and works seamlessly with AVR Studio, you can debug, start without debugging, etc. and it will be crc16 signed.


You can add a -serialno option to the post build event, but its real power is when you want to use the SAME code and just keep updating it with a new serial number.  To do this, just build your project and then copy your HEX/ELF file out of the release directory into another directory.  Then you can use the crcsntool to change the serial number on it each time you need to.  The advantage here is that you are not building the code each time with the possibility of it changing due to a software change, code change, etc.  You can truly ensure that your 1.23 version is always *the same* even though you an change the serial number.


Here is an example of my flash.bat for anyone who doesn't want to spend an hour with the atprogram help screens:



```

  @echo off
  echo .
  echo Erasing and Writing Fuses
  atprogram -v -t atmelice -d attiny1634 -i isp -cl 125khz program -c -fs --verify -f release\newproject.elf
  echo .
  echo Writing Program and Lockbits
  atprogram -v -t atmelice -d attiny1634 -i isp -cl 2mhz program -fl -ee -lb --verify -f release\newproject.elf
```
This programs an ELF file which can contain flash code, eeprom code, and fuses all in one file.


The first line uses a slower speed, erases the chip, and programs and verifies fuses.


Then it is released and the second line can program at a higher speed.  It programs and verifies the flash, eeprom, and lockbits.

## Compilers

- Atmel Studio 6

## Tags

- Complete code
- Atmel Studio 6
