//prototypes
unsigned char	OWReadBit	(unsigned int OWPin);
void			OWWriteData	(unsigned int OWPin, unsigned char* data, unsigned char numbytes);
void			OWWriteByte	(unsigned int OWPin, unsigned char ByteVal);
void			OWWriteBit	(unsigned int OWPin, unsigned char BitVal);
void			OWWrite0	(unsigned int OWPin);
void			OWWrite1	(unsigned int OWPin);
unsigned char	OWReset		(unsigned int OWPin);
unsigned char	OWReadByte	(unsigned int OWPin);
void 			OWReadData	(unsigned int OWPin, unsigned char* data, unsigned char numbytes);

