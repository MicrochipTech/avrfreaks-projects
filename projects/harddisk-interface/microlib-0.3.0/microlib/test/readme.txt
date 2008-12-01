Test data for Serial IDE Interface (side.rom)
---------------------------------------------

You can find here several data for testing the serial
IDE Interface:

config.txt        -  Read the harddisk's configuration sector
init.txt          -  Initialize the hardware
sleep.txt         -  Put harddisk into sleep mode
wakup.txt         -  Put harddisk into normal operation mode
status.txt        -  Read harddisk's status information
R-d-h-ss-cccc.txt -  Read sector ss with head h on cylinder cccc / drive d
W-d-h-ss-cccc.txt -  Write a sector with some test data

These files contain the commands described in the ide interface
documentation. The commands are embedded in SLIP data frames, so that
you can simply send the data to the serial port connected with the
IDE interface.


May 04th, 2002

Stefan Heesch
heesch@cybervillage.de
