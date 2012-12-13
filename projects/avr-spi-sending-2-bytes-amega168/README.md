# AVR SPI sending 2 bytes amega168

Uploaded by devtech2015 on 2012-12-13 06:37:00 (rating 0 out of 5)

## Summary

In the document, I provide the C code for setting up the master AVR, sending more than one bytes and also setting up the slave AVR, and then receiving the bytes sent one after the other.  

For my case am using two ATMEGA168 chips and these functions work out correctly.  

The principle is that when u sending and receiving is FIFO (First In, First Out).  

One important thing to note is that what you will read last in the SPDR will be the contents of the last byte sent.  

## Remember that whatever is sent from the master is stored in the SPDR and this register keeps on updating its contents until when the last byte is received.  

If you may want, you may include a small delay between sending consecutive bytes to allow buy some time for reading the data received by the slave .  

Any modifications, questions and compliments are welcome.  

///////////////////////////////////////////////////////////////////////  

FORGIVE MY NOT PERFECT ENGLISH BUT I HOPE THE POINT IS DRIVEN HOME  

//////////////////////////////////////////////////////////////////////

## Compilers

- AVR Studio 5

## Tags

- Complete code
- AVR Studio 5
