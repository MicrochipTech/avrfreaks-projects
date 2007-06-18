#ifndef _QTI_
#define _QTI_

// qti10 returns a 10-bit value representing the reflectance the QTI
// sensor sees.

int qti10(unsigned char channel);

// qti8 returns an 8-bit value representing the reflectance the QTI
// sensor sees.

int qti8(unsigned char channel);

#endif // _QTI_
