# Swipe Card (Credit Card) Reader

Uploaded by AMKhan on 2009-06-05 17:03:00 (rating 0 out of 5)

## Summary

![](http://www.digisoft.com.pk/Projects/swipe-card-reader-interface/image002.gif)


**Swipe Cards** are used nowadays at many locations that include Point of Sale Terminals, Access Control, ATM Machines, Motorway Toll, Airline Tickets Verification, Vending Machines, etc. They connect to the host Microprocessor through Direct I/O interface, RS-232 serial ports or USB.


Swipe Cards used in Financial Transaction have a defined size, Track Encoding and card Data Format. Debit Card and Credit Card are example of Financial Transaction Card.


As the Financial Transaction Cards and information related to them are available easily, they are used in this project. Details of FTC are provided as under.


Card Dimensions, Encoding and Data Format for credit cards are as per ISO7813


Swipe Card Reader Connector


Magtek is world renowned swipe card manufacturer. Card reader used in this project is Magtek Part number 2105004 which is 101mm compatible, Dual Track (Track 1 & 2) Swipe card reader with cover. The Card has TTL compatible direct I/O interface, as given in the card Reader pinout Diagram.


Connecting Card Reader to the AVR Project Board


Below is the connection for the wiring of the card to the AVR project board to capture the track 1 of the card.


Card Present <-> PD7 / LED1  

Strobe TK1 <-> PD2  

Data TK1 <-> PD0


Note: Remove the MAX 232 from the project board, or use any other port pin for DATA signal as the PD0 and PD1 are directly connected to the MAX 232.


TRACK 1 DATA FORMAT


The following is typical for Track 1 data.


The Track 1 data format includes the following items:


1. Approximately 62 leading clocking zeros  

2. The Start Sentinel "%"  

3. Up to 76 alphanumeric characters selected by the user  

4. The end sentinel "?"  

5. The LRC (longitudinal redundancy check character)  

6. Trailing clocking zeros (minimum of 62) to the end of the card


Each data character is in a 7-bit (6 plus parity) alphanumeric format. The data is in the format "parity, 32, 16, 8, 4, 2, 1". Parity is odd (an odd number of "one's" in each character).


The character is written "backwards" on the card starting with the least significant digit and ending with the parity bit. The card data format is "1, 2, 4, 8, 16, 32, parity".


Decoding Reader Output


The usual way to gather card data from a TTL (Transistor-Transistor Logic) swipe magnetic card reader as described in the Magtek Application Note is as follows:


1. Connect the strobe line to an interrupt port on the CPU that is programmed to interrupt on the falling edge of the strobe.  

2. Connect the data line to an ordinary CPU port pin.  

3. Connect the card present line to another ordinary CPU port pin.


All three of the above signals are quiescently high (VCC). When a card is swiped, the first signal to go low (active) is Card Present. It will go low after 14 to 15 head flux reversals have been read. These initial strobes are not output on the strobe line. After Card Present goes low, the strobes are output on the strobe line. Storing data should not begun until the data line goes low (indicating a one) and the strobe line falls. When this occurs, the CPU starts to store the data in RAM (Random Access Memory) until the strobes stop because the card has cleared the head. Approximately 150ms later, Card Present returns to VCC. When this occurs, the CPU analyzes the data in RAM and converts it to the ASCII.


Software for Decoding Track 1


The Software that does this is provided on the website. Note that the code listed here has some limitation as below


1. It decodes Track 1 only  

2. It does not calculate the parity check  

3. It does not calculate the LRC  

4. Card only can be swiped in one way (one that is shown on the card reader) to get valid output.  

5. The listed code does not output data to any console or somewhere so that it can be seen by the user. In order to see the data, user has to connect JTAG Board and Host computer to the AVR Project Board and use the debugging features of the AVR Project Board. After a card is swiped, he has to halt the code and see the appropriate variables in the watch window in order to check the output. Alternatively user may write his own code to send the data to some display.  

6. It does not check for "start sentinel" to begin data collection, which is more reliable way of gathering data.


Pictures and source code can be seen on  

<http://www.digisoft.com.pk/Projects/swipe-card-reader-interface>

## Links

- [Card Reader pinout](http://www.digisoft.com.pk/Projects/swipe-card-reader-interface/image010.gif)
- [Pictures and Code](http://www.digisoft.com.pk/Projects/swipe-card-reader-interface)
- [Link to AVR project Book Page](http://www.digisoft.com.pk/products/avr-project-book)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
