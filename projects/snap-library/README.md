# Snap Library

Uploaded by mportobello on 2003-08-16 06:02:00 (rating 0 out of 5)

## Summary

SNAP Protocol Library


Simple library for sending and receiving Snap packets.


The library has the following features:  

 - easy to use  

 - collision avoidance (e.g. RS486 half-duplex networks)  

 - crc-16 error detection  

 - interrupt-based receiving  

 - blocking transmitting


The Snap protocol is very flexible. See <http://www.hth.com> for more information. The library has the following restrictions to keep it simple:  

 - 3 byte addresses (16.7 million nodes)  

 - crc-16 error detection  

 - command-mode bits and protocol specific flags not supported  

 - 2 preamble bytes to improve packet start detection


Example: the device will echo back any data received


 #include "snap.h"  

 int main(void)  

 {  

    while (1)  

     {  

       if (SnapPacketReceived())  

       {  

          SnapSend(SnapPacketSource, SnapPacketData, SnapPacketDataLength);  

          SnapReset();  

       }  

    }  

 }


[mportobello@hotmail.com](mailto:mportobello@hotmail.com)

## Compilers

- AVR32 GNU Toolchain

## Tags

- General Lib.functions
- AVR32 GNU Toolchain
