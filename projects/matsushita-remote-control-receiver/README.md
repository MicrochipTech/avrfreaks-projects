# Matsushita remote control receiver

Uploaded by nasi on 2005-10-17 15:58:00 (rating 0 out of 5)

## Summary

This is a small project I used for testing my remote control implementation for Matsushita protocol, which seems to be a quite common one (at least many of my remote controls use it). Input capture is utilized to catch the bitstream. The message content is output through UART.  

The project also uses an interrupt-based UART module, with which you can output a complete buffer content without handling each byte.  

Also system timer routines are contained, these are remainders of an older status of the impementation. They are not really used here, but this module is running very well in many of my other projects.  

All three parts can also be used (nearly) stand-alone. I ran it on MEGA16 and MEGA8.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
