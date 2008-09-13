/**************************************
        ADC_LTC1298 header file
**************************************/

/**************************************
Function: Defines
**************************************/
#define ADC_LTC1298_Power_Off   PORTE.2 = 1
#define ADC_LTC1298_Power_On    PORTE.2 = 0         
#define ADC_LTC1298_Channel_0   0x0D                            //StartBit, Channel 0, MSB first, Single Ended
#define ADC_LTC1298_Channel_1   0x0F                            //StartBit, Channel 1, MSB first, Single Ended            

void ADC_LTC1298_On(void);
void ADC_LTC1298_Off(void);
unsigned int ADC_LTC1298_Data(unsigned char Channel);
