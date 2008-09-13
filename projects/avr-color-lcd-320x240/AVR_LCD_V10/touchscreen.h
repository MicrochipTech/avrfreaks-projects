/**************************************
        Touchscreen header file
**************************************/
#define TouchPress      PIND.2
#define XR              PORTB.1
#define YT              PORTB.2
#define XL              PORTB.3
#define YB              PORTB.4

#define Touch_Off       (XR = 1) | (YT = 1) | (XL = 0) | (YB = 0)       //All off
#define Touch_Standby   (XR = 0) | (YT = 1) | (XL = 0) | (YB = 0)       //XR on 
#define Touch_Xas       (XR = 0) | (YT = 1) | (XL = 1) | (YB = 0)       //XR and XL are control, measure YB(Y-) 
#define Touch_Yas       (XR = 1) | (YT = 0) | (XL = 0) | (YB = 1)       //YT and YB are control, measure XL(X-)   

#define Touch_Xas_Data  0x01                                            //For ADC channel setting
#define Touch_Yas_Data  0x00                                            //For ADC channel setting
                                                                                                                 
void Touchscreen_Init(void);
unsigned int Touchscreen_Value(unsigned char T_as);
void Touchscreen_XY_positie(void);
void Touchscreen_Calibratie(void);
