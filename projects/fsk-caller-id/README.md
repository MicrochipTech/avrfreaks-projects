# FSK Caller ID

Uploaded by LieBtrau on 2005-10-31 13:01:00 (rating 0 out of 5)

## Summary

This project consists of some very basic hardware to interface with a telephone line. The software reads the caller identification signal, sent by your telephone company and shows the result on an LCD. This works only for countries that are implementing the ITU-T Rec.V.23. These will mostly be EU-countries.  

The FSK-signal is decoded using an XR-2211. The decoded 1200baud signal is then sent to the USART-RX pin of an ATMEGA8535. This microcontroller runs a state machine. It will check if the received data is correct. If it is, then the number of the caller, including the date & time of the call will be shown on an LCD-display.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
