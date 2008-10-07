



//  prototype declarations
void			OWInit			(unsigned int OWPin);
unsigned char	OWGetROM		(unsigned int OneWirePin, unsigned char* ROM);
int  			OWFirst			(unsigned int OWPin);
unsigned int	OWNext			(unsigned int OWPin);
unsigned int	OWVerify		(unsigned int OWPin, unsigned char *);
void 			OWTargetSetup	(unsigned int OWPin, unsigned char family_code);
void 			OWFamilySkipSetup(unsigned int OWPin);

unsigned int	OWSearch		(unsigned int OWPin, unsigned char*);
unsigned char docrc8			(unsigned char value);
