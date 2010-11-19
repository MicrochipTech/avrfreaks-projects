
/*

RFM12B node system.


:******************************************************************************************
17/Nov/2010  (V0.81)

Found the bug in the remote-node command response.
A count check used a hard coded number left in from earlier, instead of an equate (that 
was used in several other places).
This was for 'maxdata' (defined in RFM12B.asm). This sets the payload max size. This was 66
originaly but now can ba a max of 242 (because of 8-bit counts) The complete message length
must fit in an 8-bit value


When in command mode to a remote node, local echo is supplied by the local node so no 'typing blind'
Once the complete line is entered, it is packaged & sent to the remote which then supplies the 
response.




:******************************************************************************************
16/Nov/2010  (V0.8 1st release) 

The demo code is for a transmitter with a DS18B20 1-wire sensor and a receiver that
waits for data and sends it out the serial port.
The command interpreter is listening to the receive side of the serial port. 
This can look a bit messy because if you type a command, the sensor data is being outputted
at the same time so the charcters get mixed on the screen.
When the 'connect' command has been accepted, the sensor processing at the transmit end is
halted, this makes it less distracting.
'connect' times-out in 20 seconds (TX & RX node) and the TX side goes back to its 
sensor processing as usual

Connect the DS18B20 to portD, pin3. Not using parasitic power.
If you dont connect one, the message from the transmitter says CRC ERROR instead of the temp.



A bug somewhere in the returning of the text outputted from a remote node while 'connected'
The commands work,  you can change rates/frequency/channel (typing blind as no remote echo)
but when you type 'help' or '?' to the remote, the command list is not returned.
If you just hit enter, entering an empty line, the node state info is returned.


More comprehensive documentation is required to make it clearer how to set-up everything prior
to a transmission.
The receive-side is much easier to deal with




*/
