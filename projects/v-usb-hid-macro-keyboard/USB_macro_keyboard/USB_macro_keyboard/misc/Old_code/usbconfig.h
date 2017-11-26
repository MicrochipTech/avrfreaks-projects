#define USB_RESET_HOOK(resetStarts)     if(!resetStarts){hadUsbReset();}
#ifndef __ASSEMBLER__
extern void hadUsbReset(void);      // define the function for usbdrv.c and hid.c
#endif
/* This macro is a hook if you need to know when an USB RESET occurs. It has
 * one parameter which distinguishes between the start of RESET state and its
 * end.
 */