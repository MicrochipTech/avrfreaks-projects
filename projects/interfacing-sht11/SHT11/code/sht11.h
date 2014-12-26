/************************************************************/
/**********            SHT11 Command    *********************/
/************************************************************/

#define SHT11_Temp_Measure 		0x03   	// Temperature read
#define SHT11_Humidity_Measure 	0x05 	// Humidity read
#define Sht11_Reset 			0x1e    // reset

// Externally used  functions
extern char SHT11_Read_Raw_Data(unsigned int *sensor_data, unsigned char *chk1, unsigned char *chk2);;
extern int Temp_Calc(unsigned int resolution);
extern char SHT11_Start_Measuremnt(unsigned char measure_entity);
extern  char  SHT11_Check_Conversion(void);

