Directions:
Code writen assuming ATMEGA88PA clocked at 3.69 MHz. Compiles
with avr-gcc.

PORTB PIN0 of AVR connects to SIG pin of PING)))

Returns a raw 16 bit binary number for distance
(other micros/computers have no concept of meters/inches)
The distance per count is giving by the follow formula:
(Fclk^-1)*V*4
Fclk = AVR system clock
V = speed of sound (about 340 m/s in air at STP)

As written the code allows a max range of about 2.9 meters. The 
max range can be extend by increasing the value of TIMEOUT in the 
source code. 7885 was chosen based on the specs given in the Ping)))
datasheets. I have increased the value and found the code to still 
function properly.

Uses SPI in Slave mode
