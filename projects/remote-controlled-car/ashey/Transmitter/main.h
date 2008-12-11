#define IR          PB2
#define RED_LED     PB1
#define GREEN_LED   PB0 
#define S3          (!( PINC & _BV(PC3) ))
#define S2          (!( PINC & _BV(PC2) ))
#define S1          (!( PINC & _BV(PC1) )) 
#define S0          (!( PINC & _BV(PC0) ))

void SetIR(unsigned char width , unsigned char low , unsigned char OCR_value);
void delay(unsigned char value , unsigned char low , unsigned char OCR_value); 
void initDDRs(void);
void setupTimer1(void);
void setupTimer2(void);
void StartTransmit( int *array);

