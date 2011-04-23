this is a complete linux/unix based avr setup with a sample project.

INSTRUCTIONS:
cd into this folder,
*************************
If you have never used a command line avr programmer, run the shell script to download and install or update
avr-gcc and avrdude. 
cut and paste into terminal:
./avrsetup.sh

YOU SHOULD ONLY NEED TO RUN THIS ONCE!!!

*************************
Then, connect your programmer via usb.
When both green lights are on, run:

./ctohw.sh

"C file TO HardWare"
Builds .hex file and uploads it to board, provided avrisp2 is connected and configured.
*If you tried to run this without running avrsetup.sh, try running it. If you get error messages, check your circuit.
*If you edit the code for a different device/configuration/etc,
 running ctohw.sh will rebuild the project and upload to board.
*Included is a build, located in the default folder. These files should work as an AVRStudio (4 def, 5 maybe) project.

To calculate the right fuses for any project, goto:
http://frank.circleofcurrent.com/fusecalc/fusecalc.php?chip=atmega328p

To determine programmers/ports goto:
http://www.nongnu.org/avrdude/user-manual/avrdude_4.html#Option-Descriptions

Currently set up to run kosher using avrisp2 and atmega 328p (28PDIP), 8MHz internal oscillator, no clkdiv8

thanks to
http://doswa.com/blog/2010/08/24/avrdude-5-10-with-ftdi-bitbang/#more-460
http://frank.circleofcurrent.com/fusecalc/fusecalc.php?chip=atmega328p
http://www.avrfreaks.net/

Do whatever you want with this, but keep it open source. No bombs.

With any questions or comments please email:
aaron.w.rackoff@gmail.com

