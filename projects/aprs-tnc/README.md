# APRS TNC

Uploaded by georges80 on 2012-04-25 06:10:00 (rating 0 out of 5)

## Summary

ATMEGA324 based APRS TNC. Interfaces with a SIRF III GPS module and an LCD panel for status display. Operates on a single li-ion cell and has on board li-ion charger chip.


The hardware will generate a PTT (push to talk) signal and also the modulated audio. These 2 signals can be connected to a 2M handheld or various 2M mobile transceivers to send APRS packets via the radios to the APRS digipeater system.


The firmware encodes data in MIC-E format and also implements a smart beaconing algorithm to minimize transmitted packets.


The SIRF III GPS is intefaced via a TTL connection and the NMEA sentences are decoded in the software to acquire the current position for use with the smart beaconing algorithm and also to be encoded for transmission along with the user's call sign.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
