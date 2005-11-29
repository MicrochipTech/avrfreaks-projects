# NeoRS232

Uploaded by LieBtrau on 2005-11-28 18:39:00 (rating 0 out of 5)

## Summary

This project implements an Idle RQ Stop & Wait protocol. This is a complicated manner to say that a transmitter must wait for an answer of the receiver before it can send something again. The receiver checks the incoming frame using the CRC16-checksum. If the checksum is correct, then an acknowledge is sent back. In the meanwhile, the sender had started a timer after sending his frame. It waits for the acknowledge of the receiver. If that acknowledge doesn't arrive in five seconds, then it tries to resend the data. The sender tries a maximum number of five times to resend data. This implementation takes about 1400 bytes of code on your microcontroller's flash. See for yourself if you can live with this.  

This project also includes an implementation in Java to make it possible to have communication between PC and microcontroller.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
