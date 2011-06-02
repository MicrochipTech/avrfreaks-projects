# Libmad AVR32 UC3 optimization

Uploaded by sma on 2011-06-01 18:18:00 (rating 0 out of 5)

## Summary

Update: libmad is already included in the ASF since 2.5.1, check [www.atmel.com/asf](http://www.atmel.com/asf).


This patch adds optimized routines for the AVR32 UC architecture (used in all AT32UC3 series) in Libmad MP3 decoder. Works with both GCC and IAR compilers.


MAD information can be found here: [http://www.underbit.com/products...](http://www.underbit.com/products/mad/).  

Current patch is for libmad 0.15.1b.


Libmad source code is available here: [http://sourceforge.net/projects/...](http://sourceforge.net/projects/mad/files/libmad/0.15.1b/libmad-0.15.1b.tar.gz/download)


To apply the patch:  

1-download the libmad 0.15.1b source code.  

2-copy the libmad-0.15.1b-avr32-uc3-optimization.patch in the root directory of the libmad source code.  

3-run patch -p1 -i libmad-0.15.1b-avr32-uc3-optimization.patch in an appropriate tool shell.


Expected output:  

patching file Dmod.dat  

patching file Makefile.am  

patching file Makefile.in  

patching file aclocal.m4  

patching file avr32/AT32UCR1/GCC/build.sh  

patching file avr32/AT32UCR1/IAR/build.sh  

patching file avr32/AT32UCR1/IAR/config.h  

patching file avr32/AT32UCR1/IAR/libmad-at32ucr1.ewp  

patching file avr32/AT32UCR1/IAR/libmad-at32ucr1.eww  

patching file avr32/AT32UCR1/build.sh  

patching file avr32/AT32UCR2/GCC/build.sh  

patching file avr32/AT32UCR2/IAR/build.sh  

patching file avr32/AT32UCR2/IAR/config.h  

patching file avr32/AT32UCR2/IAR/libmad-at32ucr2.ewp  

patching file avr32/AT32UCR2/IAR/libmad-at32ucr2.eww  

patching file avr32/AT32UCR2/build.sh  

patching file avr32/build.sh  

patching file bit.c  

patching file bit.h  

patching file config.guess  

patching file config.h.in  

patching file config.sub  

patching file configure  

patching file configure.ac  

patching file dct32\_avr32.S  

patching file dct32\_avr32.s82  

patching file decoder.c  

patching file depcomp  

patching file fixed.h  

patching file frame.h  

patching file huffman.c  

patching file imdct\_avr32.S  

patching file imdct\_avr32.s82  

patching file imdct\_l\_arm.S  

patching file install-sh  

patching file layer3.c  

patching file layer3\_imdct\_s\_avr32.s82  

patching file ltmain.sh  

patching file mad.h  

patching file missing  

patching file mkinstalldirs  

patching file msvc++/Makefile.in  

patching file msvc++/config.h  

patching file msvc++/libmad.dsp  

patching file msvc++/mad.h  

patching file synth.c  

patching file synth.h  

patching file synth\_avr32.S  

patching file synth\_avr32.s82  

patching file version.c

## Compilers

- AVR32 GNU Toolchain

## Tags

- General Lib.functions
- AVR32 GNU Toolchain
