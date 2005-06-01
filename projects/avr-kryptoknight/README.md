# AVR-Kryptoknight

Uploaded by LieBtrau on 2005-06-01 16:41:00 (rating 0 out of 5)

## Summary

This project makes it possible to authenticate the microcontroller to a PC and vice versa. So both will know that there other is whos he/she says he/she is.  

This protocol is based on the IBM-Kryptoknight protocol. It makes use of HMAC (hashed message authentication), SHA-1 (secure hashing standard), and a hardware random number generator.  

The communication between the microcontroller and the PC or another microcontroller proceeds via RS232. An idle RQ-stop & wait protocol with CRC16-checksum has been written for that purpose.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
