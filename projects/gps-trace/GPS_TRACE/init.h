extern void InitLCDText();
/**
 @brief    Initialiseert het Display en plaats de welkomstekst etc
 @param    none                                    
 @return   none
*/

extern void InitUarts();
/**
 @brief    Initialiseerd de UARTS (RS232 Poorten)
 @param    none                                    
 @return   none
*/

extern void initGSM();
/**
 @brief   Initialisatie GSM-Module (aanloggen op het netwerk, pincode, etc)
 @param    none                                    
 @return   none
*/

extern void sendTestSMS();
/**
 @brief  [debugger functie] Met deze functie kan indien nodig een SMS worden gestuurd voor test doeleinden
 @param    none                                    
 @return   none
*/

extern void delay_ms(unsigned short ms);
/**
 @brief    Delay functie
 @param    Milliseconds of delay                                    
 @return   none
*/

