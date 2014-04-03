# Nano quadrocopter TX hack

Uploaded by schwobaseggl on 2014-04-03 12:00:00 (rating 0 out of 5)

## Summary

Have you seen the tiny ready-to-fly quadcopters lately? They come with a very tiny, fiddly 2G4-Transmitter which is way too small for an adultÂ´s hand. This project shows a very dirty hack on how to control the quad through a normal PPM-output RC transmitter (PPM sum signal from trainer port or separate sum signal receiver). Instead of understanding the internal workings of the digital side of the original baby transmitter, i just generate 4 analog signals and loop them into the baby tx instead of its own poti readings. The ppm readout library is NOT mine, it comes from an unknown source and i found it used on several other RC projects, so i dared to use it here. The rest is terribly simple, bad style software PWM to generate 4 analog voltages pieced together in 15 minutes. The main job is separating the poti lines and install a suitable connector (i used a simple 5x2 header) to feed supply and signal from the PPM- dongle to the baby tx. Use the low voltage (3,3V compatible) version of an Atmega8 and internal RC oscillator 8MHz for this project. The baby tx can still be used standalone by plugging in a bypass jumper instead of the dongle connector, thus reconnecting its own stick potis. Quick, dirty, and working very nicely. The little, aggressive Quadcopter can be controlled much more accurately and becomes a precise, tame little bird. *ERROR* The output channels are wired to Port D, not Port C as shown in the schematic. Please modify source or schematic accordingly, i will upload a corrected version shortly.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
