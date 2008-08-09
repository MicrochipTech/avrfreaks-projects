#ifndef __fuse_bits_h_included__
#define __fuse_bits_h_included__

//Deklarasi Prototipe Fungsi Operasi-Operasi Fuse Bits

/*** Read Signature Byte ***/
void ISPReadDevaisSignature(void);

/*** Fuse Low Byte Operation ***/
unsigned char ISPReadLowFuseBits(void); 
void ISPWriteLowFuseBits(unsigned char fuse_low_byte);    

/*** Fuse High Byte Operation ***/
unsigned char ISPReadHighFuseBits(void);  
void ISPWriteHighFuseBits(unsigned char fuse_high_byte); 

/*** Fuse Extended Byte Operation ***/
unsigned char ISPReadExtendedFuseBits(void); 
void ISPWriteExtendedBits(unsigned char fuse_extended_byte); 

#endif