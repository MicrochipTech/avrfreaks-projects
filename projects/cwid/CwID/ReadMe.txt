This is a small AT90S2313-project for the logic part of a radio beacon.

The attached sorce code enables the CPU to send a message of max. 127 char's in Morse code (CW). The message text is programmed into the CPU's eeprom. Before programming you must decide what CW speed you want and how long the pause (in seconds) before the message is repeated should be. NOTE! the speed is given as characters/min (characters/min. is approx. 5 * Words per minute).

When studying the attched schematics you will notice that the output to the transmitter is via an optocoupler and that there are 2 switches and a jumper. Switch S1 is to stop the transmission and to restart at the beginning of the message when switched to OFF and then ON. Switch S2 is to disable the sound from the buzzer. Jumper JP1 lets you choose between carrier and no carrier during the message pause (JP1 closed means no carrier, open means carrier).

Have fun 
/p-a, SE4A/SM4INV