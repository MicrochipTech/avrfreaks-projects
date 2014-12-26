/************************************************************/
/**********              Main File      *********************/
/**********            WINAVR20090313   *********************/
/**********         				  *********************/
/************************************************************/


// Resources 
#include <avr/io.h>
#include <stdlib.h>
#include <string.h> 
#include <util/delay.h>
#include "sht11.h"

#include "lcd.h"

// Constants
#define True 1
#define False 0

// Functions declartion
void Init_Ports(void);					
void Display_Error_Message(char err_msg);
void Data_Temp_Conversion(void);
void Measure_Temperature(void);
void Data_Temp_Conversion(void);
void Display_Data(void);
void Data_Temp_Conversion(void);
void Measure_Humidity(void);
void Check_Diagnoctic_Button(void);

// Variables used
unsigned char   sensor_error = 0;
unsigned char sensor;
static char temp_buff[12], humi_buff[12];
unsigned char  ch1, ch2;
unsigned int Sensor_Humidity_Data,Sensor_Temperature_Data;
unsigned char  diagnostic ;


//**************** MAIN START HERE ****************************
int main(void)

{
	// To disable  JTAG to use PortC for LCD interface
	MCUCSR = (1 <<JTD);				
	MCUCSR = (1 <<JTD);

	Init_Ports(); 	//Intialize Ports
	lcd_init(); 	// LCD Intialize
	_delay_ms(500);
		
	sensor = 0;
	diagnostic = False;	  // 0 will   display measured values, 1 will show diagnostic data
	while(1)
	{  	
		 
		  PORTB = PORTB | (0x08);
 		 // Blink Led to indicate activity
		//Measue Temperature 
		Measure_Temperature();
		_delay_ms(350);
		 PORTB = PORTB & (0x80);
		 _delay_ms(350);

		PORTB = PORTB | (0x08);

		// Measure Humididty     DISPLAYED as RAW DATA
		Measure_Humidity();
		_delay_ms(350);
		 PORTB = PORTB & (0x80);
		 _delay_ms(350);

		// check Diagnostic  Button only when no error is deducted
		
		if (sensor_error == 1)
			Check_Diagnoctic_Button();	
		
		if ( diagnostic == False && sensor_error == 1)
			Display_Data();   // display the data on lcd,  i.e. in non-diagnostic mode 
		

	
	}/********************* WHILE END *********************/
} //********************* MAIN END *********************/

// Convert Temp data which is in 14bit to Centigrade
// Convert to string for display
// Adds decimal to the data also
void Data_Temp_Conversion(void)
{
        int  i, di, val;
        val = Temp_Calc(Sensor_Temperature_Data); 
		di = val/10;
		itoa(di,temp_buff,10); 
        i=strlen(temp_buff);
        temp_buff[i]='.';
        di = abs(val)%10;
		itoa(di,&(temp_buff[i+1]),10); 
}
// Display Data
void Display_Data(void)
{	
		// Convertt the Temp Data of 14 bit to Centigrade and Humidity
		// data into a string for display
		Data_Temp_Conversion();
		// Display Temprature Data								
		lcd_gotoxy(0,1);
		printf("Temp:%d" ,Sensor_Temperature_Data);
		lcd_gotoxy(9,1);lcd_puts("     ");
		lcd_gotoxy(12,1);lcd_puts(temp_buff);
		
		// Display HumidityData
		lcd_gotoxy(0,2);
		lcd_gotoxy(0,2);printf("Humidity:  %d ", Sensor_Humidity_Data);
		
}
// Measure Temperature
void Measure_Temperature(void)
{
		unsigned char   data_avail=10;
// Check if sensor is attached
		sensor_error = SHT11_Start_Measuremnt(SHT11_Temp_Measure); 
		// Display Error Message, if no sensor is detected
		
		if( sensor_error == 0)		
		{
			Display_Error_Message(1);
			goto out_exit;  // exit the   condition and  reset of the code
		}	
 		// Start Temperature    measurement                  
		// Check the conversion status i.e. whether  requested Data is available or not
		_delay_ms(50);
		
		
		// Poll to see if data is available for conversion
	
		data_avail = SHT11_Check_Conversion();
		
		while( (sensor_error == 1 )  && (data_avail != 1))
		{
			data_avail = SHT11_Check_Conversion();
		}

		// at exit of this loop data will be avialable for processing     
		// hcek for crc error , if any
		sensor_error = SHT11_Read_Raw_Data( &Sensor_Temperature_Data, &ch1, &ch2);

		if (sensor_error == 0)
		{
			Display_Error_Message(2);  // display crc error message
			goto out_exit;  // exit the   condition and  reset of the code
		}
		// Used for displaying  error, raw temp data crc calculated and checksum 
		// Used for   diagnostics
		if (sensor_error == 1 && diagnostic == 1)
		{
			lcd_gotoxy(0,1);
			printf("Temp:%d        ",Sensor_Temperature_Data);
		    lcd_gotoxy(0,2);
			printf("CRC %d  %d        ", ch1, ch2);
		}   
out_exit:	
		;   // exit  from the code 
}

// Measure Humidity
void Measure_Humidity(void)
{
		unsigned char   data_avail=10;
	// Check if sensor is attached
		sensor_error = SHT11_Start_Measuremnt(SHT11_Humidity_Measure); 
		// Display Error Message, if no sensor is detected
		
		if( sensor_error == 0)		
		{
			Display_Error_Message(1);
			goto out_exit;  // exit the   condition and  reset of the code
		}	
 		// Start Temperature    measurement                  
		// Check the conversion status i.e. whether  requested Data is available or not
		_delay_ms(10);
		
		
		// Poll to see if data is available for conversion
	
		data_avail = SHT11_Check_Conversion();
		
		while( (sensor_error == 1 )  && (data_avail != 1))
		{
			data_avail = SHT11_Check_Conversion();
		}

		// at exit of this loop data will be avialable for processing     
		// hcek for crc error , if any
		sensor_error = SHT11_Read_Raw_Data( &Sensor_Humidity_Data, &ch1, &ch2);
		if (sensor_error == 0)
		{
			Display_Error_Message(2);  // display crc error message
			goto out_exit;  // exit the   condition and  reset of the code
		}
		// Used for displaying error, raw temp data crc calculated and checksum 
		// Used for   diagnostics
		if (sensor_error == 1  && diagnostic == 1)
		{
			
			lcd_gotoxy(0,1);
			printf("Humi:%d        ",Sensor_Humidity_Data);
		    lcd_gotoxy(0,2);
			printf("CRC %d  %d      ", ch1, ch2);
		}   
out_exit:
		;		// exit the rest of code 
}
// Display error , when no sensor or crc error  is detected
void Display_Error_Message(char err_msg)
{
		//no sensor found error

		if ( err_msg == 1)
		{
		//	lcdcls();
			lcd_gotoxy(5,1);
			printf("ERROR");
			lcd_gotoxy(1,2);
			printf("Connect Sensor");
		//	_delay_ms(1000);
			goto out;		// exit on error 
		}

		// crc error
		if ( err_msg == 2)
		{
		//	lcdcls();
			lcd_gotoxy(3,1);
			printf("CRC ERROR");
			lcd_gotoxy(5,2);
			printf("Reset");
		//	_delay_ms(1000);
		}
out :
	; // exit   om error
}

// Check the status of the  Diagnostic button 
void Check_Diagnoctic_Button(void)
{
		char res = 0;
		DDRA = DDRA & 0xFE;   // set PA0 in input mode
		res = PINA & 0x01 ;	// get value of Diagnoctic button
		// Button pushed, take action
		if (res == 0x01)	
		{	
			_delay_ms(50);
			res = PINA & 0x01 ;
			if (res == 0x01)	
				diagnostic = True;  // set the diagnostic variable so CRC values can be displayed
		}
		// Button not pressed
		if (res == 0x00)
		{
			diagnostic = False;		// normal data displayed.  NO CRC values
		}
}

// Intialize Ports
void Init_Ports(void)
{
	DDRC = 0xFF;		// LCD DATA PORT
	DDRA = 0xFE;		// LCD CONTROL PORT   and PA0 as input
   	DDRB = 0x09;		// Sensor Ports- SCK
	DDRD = 0x01;		// Sensor Data Port
}



