#############################################################################
# This is the configuration for all the makefiles.  You must modify this file
# to reflect what MCU you are using, where you have installed avr-gcc, and
# so on in order for the makefiles to work.  All the makefiles include this
# configuration file.
#############################################################################

#############################################################################
# You will almost certainly have to change the values in this section.
#############################################################################

MCU=atmega328p         # Some Arduino Duemilanove boards
# MCU=atmega168        # Some Arduino Duemilanove and all Diecimilla boards.

UPLOAD_RATE=57600      # For ATmega328p, from boards.txt in Arduino dist.
# UPLOAD_RATE=19200    # For ATmega168, from boards.txt in Arduino dist.

PORT=COM3              # Windows.  May be COM3, COM4, COM8...
# PORT=/dev/ttyUSB*    # GNU/Linux.  Leave the *, it will find the right one.

# I couldn't find avrdude.conf in the Windows arduino v17 distro.
# I had to copy it from the GNU/Linux distro.
AVRDUDE_CONFIG=YOUR-DIR-HERE/arduino-0017/avrdude.conf


#############################################################################
# You are less likely to need to change the variables in this section.
#############################################################################

AVRDUDE_PROGRAMMER= stk500v1

CC= avr-gcc
AR= avr-ar
OBJCOPY= avr-objcopy
AVRDUDE= avrdude
RM= rm



