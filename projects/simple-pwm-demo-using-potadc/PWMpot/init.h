#ifndef INIT_H
#define INIT_H

#define INTR_OFF() asm("CLI")
#define INTR_ON()  asm("SEI")

/* function prototype */
void init_devices(void);

#endif
