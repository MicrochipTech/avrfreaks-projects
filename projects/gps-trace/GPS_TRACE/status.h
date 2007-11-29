extern void setFixstate (char c);
/**
 @brief    Sets the status of GPS fix (fixstate)
 @param    char c --> this is the result of getfield (5,2);                                 
 @return   none
*/

extern int getFixstate();
/**
 @brief    Returns the value of GPS fix (fixstate)
 @param    none                                    
 @return   1 or 0 --> 1 means fix, 0 means no fix
*/

extern void showFixstate();
/**
 @brief    Displays the fix state on LCD
 @param    none                                    
 @return   none
*/

extern void setNS_indicator (char c);
/**
 @brief    Sets the North/South inidicator 
 @param    char c                                    
 @return   none
*/

char getNS_indicator();
/**
 @brief    Gets the North/South inidicator 
 @param    none                                    
 @return   NSindicator (char)
*/

char getEW_indicator();
/**
 @brief    Gets the East/West inidicator 
 @param    none                                    
 @return   EWindicator (char)
*/

extern void setEW_indicator (char c);
/**
 @brief    Sets the East/West inidicator 
 @param    char c                                    
 @return   none
*/

extern char*  getLatitude();
/**
 @brief    Returns a char pointer the the N degree
 @param    none                                    
 @return   char* Latitude
*/

extern void setLatitude(char *s);
/**
 @brief    Sets GPS Latitude
 @param    char *s (result)                                   
 @return   none
*/

extern char*  getLongitude();
/**
 @brief    Gets GPS Longitude
 @param    none                                   
 @return   char* Longitude
*/

extern void setLongitude(char *s);
/**
 @brief    Sets GPS Longitude
 @param    char *s (result)                                   
 @return   none
*/

extern void FormatLongitude();
/**
 @brief    Formats GPS Latitude van ddmm.mmmm naar  DD°MM.mmm
 @param    none                                  
 @return   none
*/

extern void FormatLatitude();
/**
 @brief    Formats GPS Longitude van dddmm.mmmm naar  DD°MM.mmm
 @param    none                                  
 @return   none
*/

extern void showLatitudeFormatted();
/**
 @brief    Shows GPS Latitude in DD°MM.mmm format on LCD
 @param    none                                  
 @return   none
*/

extern void showLongitudeFormatted();
/**
 @brief    Shows GPS Longitude in DD°MM.mmm format on LCD
 @param    none                                  
 @return   none
*/

extern char* getLatitudeFormatted();
extern char* getLongitudeFormatted();
extern void ReportPosition();

int fixstate;
/**
 @brief    Represents the fixstate of the GPS-module
*/

char NSindicator;
/**
 @brief    Represents the Latitude North/South indicator
*/

char EWindicator;
/**
 @brief    Represents the Longitude East/West indicator
*/

char Latitude[10];
/**
 @brief    Represents the Latitude in ddmm.mmmm format 
 */

char LatitudeFormatted[15];
/**
 @brief    Represents the Latitude in  DD°MM.mmm format 
*/


char Longitude[10];
/**
 @brief    Represents the Longitude in dddmm.mmmm format 
*/

char LongitudeFormatted[15];
/**
 @brief    Represents the Longitude in DD°MM.mmm format 
 */
