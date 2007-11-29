int state;			//status van de statemachine
int CMD_count;			//counter voor initcommand 
int COMMAND_count;		//counter voor NMEA sentence 
char data;			//Data te verwerken 
char CMD[4]; 			//opslag initcommand
char COMMAND[200];		//NMEA sentence 
int test; 			///just for debugging
int fora;			//for loop var
char* testch;			//just for debugging
char DisplayBuffer_1[7];	// buffer voor display
char result[10];		


/**
 @brief    Initialiseerd de NMEA Parser
 @param    none                                    
 @return   none
*/
extern void initNMEA ();

/**
 @brief    voert de buffer van de parser & Start data analyse
 @param    none                                    
 @return   none
*/
extern void NMEAFeedBuffer (char pdata);

/**
 @brief    Update de gps positie variablen
 @param    none                                    
 @return   none
*/
extern void UpdateStatus();

/**
 @brief    Statemachine voor analyse van NMEA data
 @param    none                                    
 @return   none
*/
extern void ProcessNMEA();

/**
 @brief    vult de buffer voor het initialisatie commando
 @param    none                                    
 @return   none
*/
extern int ProcessCMD();

/**
 @brief    Bepaald of er al dan niet een GPS-fix is
 @param    none                                    
 @return   Fixstate 0/1
*/
extern int GetFixState();



/**
 @brief    Verkrijgt velden uit de GPGGSA sentence 
 @param    velnummer (int), offset (int)                                
 @return   none
*/
extern void GetField (int fieldnumber, int offset);


