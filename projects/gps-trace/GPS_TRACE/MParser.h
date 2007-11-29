extern void GSMFeedBuffer (char pdata);
/**
 @brief    Feeds the buffer of the GSMParser
 @param    Incoming Character                                    
 @return   none
*/

extern void InitGSMParser();
/**
 @brief    Initialiseerd de GSM Parser
 @param    none                                    
 @return   none
*/

void ProcessGSM();
/**
 @brief    Analyseerd inkomende data
 @param    none                                    
 @return   none
*/

int ProcessGSMCOM();
/**
 @brief    Verwerkt het initialisatie commando van een binnenkomende gesprek (CLIP).
 @param    none                                    
 @return   none
*/

int ProcessGSMLEX();
/**
 @brief    Ontvangt data die na het CLIP commando komt
 @param    none                                    
 @return   none
*/

//houdt het actueel te verwerken karakter vast
char GSMdata;		

//houd de state vast van de statemachine waarin command verwerkt worden 	
int GSMstate;			

//houdt de positie GSM_COMMAND array
int GSM_COMMAND_count;	

//houdt de positie bij in de GSM_CMD array
int GSM_CMD_count;		

//opslag initcommand
char GSM_CMD[3]; 

//opslag sentence 
char GSM_COMMAND[20];	
