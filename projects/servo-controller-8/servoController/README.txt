ServoController

System Clock is 18.432 MHz

Controlls upto eight servos at a time

255 steps of resolution (0-254, 255 not allowed)

Two byte packets:
***first byte is the servo number (0-7)
***second byte is the position value (0-254)

Uses spi to communicate
***Sample rising, setup falling (cpol = 0, cpha = 0)
***Data order msb transmitted first
***Fosc/4



   