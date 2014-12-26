/************************************************************/
/**********File Related to usage of SHT11********************/
/************************************************************/
#include <avr/io.h>
#include <util/delay.h>
#include "sht11.h"

// CRC Polynomial being used by SHT11
#define CRC8POLY	0x18  //0X18 = X^8+X^5+X^4+X^0

// Functions used
void Short_Pulse(void);
void Long_Pulse(void);
void Set_SCK_0(void);
void Set_SCK_1(void);
void SCK_Port(void);
char Check_Data(void);  
void Data_Pull_Up_1(void);
void Data_Out_Mode(void);
void Data_In_Mode(void);
void Set_Data_0(void);
void Set_Data_1(void);
unsigned char  SHT11_Read_Data(unsigned char  data_ackk);
char SHT11_Write_Data(unsigned char value);
void SHT11_Transmission_Start(void);
void SHT11_Comm_Reset(void);
char SHT11_Reset(void);
extern unsigned char  swapbits(unsigned char data_in);
extern unsigned char crc8(unsigned char crc, unsigned char data_in);

// Sensor Port Declaration
static unsigned char  Sensor_Data_DDR = DDD0;
static unsigned char  Sensor_Data_Port = PORTD0;
static unsigned char  Sensor_Data_Pin = PIND0; 

// the current measurement mode, temperature or humidity
static unsigned char   sensor_mode; 




// Read Data from sensor
unsigned char  SHT11_Read_Data(unsigned char  data_ackk)
{
        unsigned char   x, a,value=0, i=0x80;
        Data_In_Mode();  // change mode
		asm("nop");
        asm("nop");
		Data_Pull_Up_1();
        asm("nop");
		Short_Pulse(); 
        for (a=1; a<=8; a++)
		{
			Set_SCK_1(); 
        	Short_Pulse();
            x = Check_Data(); // check whether input serial data has 1 value or 0
			if (x)
			{
            	value = value | i; 
        
		 	}
            i= i>>1 ;  // change masking bit location
			Set_SCK_0();
            Short_Pulse();
            
        }

        Data_Out_Mode(); 

        asm("nop");
		if (!data_ackk)
		{ 
         	Set_Data_0();
        }
		else
		{
            Set_Data_1();
        }
        
		Set_SCK_1(); //9 clock to check for acknolwedgement

		Long_Pulse();
        Set_SCK_0();
        Short_Pulse();
        Data_In_Mode(); // set in input mode
        Data_Pull_Up_1();

        return (value);
}

// writes a byte to SHT11
// and also check for acknolwedgement
char SHT11_Write_Data(unsigned char value)
{
        unsigned char  a,x, i=0x80, error=0;
        
		Data_Out_Mode();  // set the data pin mode
        // Convert the 8 bit data into serial data for Tx to SHT11
		for (a=1; a<=8; a++)
		{
			x = i & value;
	       	if (x)  // check if bit is 1 or 0
			{
               Set_Data_1();
             }
			 else
			 { 
             	Set_Data_0();
             }
             
			 Short_Pulse(); 
             Set_SCK_1(); 
             Long_Pulse();
             i= i>>1 ;   // change masking bit
			 Set_SCK_0();
			 Short_Pulse();
        }
        Data_In_Mode();   // change data pin mode 
        Data_Pull_Up_1() ;
        Set_SCK_1(); 
        Long_Pulse();

		x = Check_Data(); // check for data 

        if (x)
		{ 
         	error=1;
        }

        Short_Pulse();
        Set_SCK_0();

        return(error); 
}

// Generate SHT11 transmission sequence 
void SHT11_Transmission_Start(void)
{
        SCK_Port();
        Set_SCK_0();
		asm("nop");
        asm("nop");

		Data_Out_Mode();
		asm("nop");
        asm("nop"); 

        Set_Data_1();
        Short_Pulse();

        Set_SCK_1();
        Short_Pulse();

        Set_Data_0();
        Short_Pulse();
        Set_SCK_0();
        Long_Pulse();

        Set_SCK_1();
        Short_Pulse();

        Set_Data_1();
        Short_Pulse();
        Set_SCK_0();
        Short_Pulse();
        
        Data_In_Mode(); //Data input mode

		asm("nop");
        asm("nop");
		Data_Pull_Up_1();
}

/// Reset communication with SHT11
void SHT11_Comm_Reset(void)
{
        unsigned char  i;
        SCK_Port();
		Set_SCK_0();
        Data_Out_Mode(); 
        Set_Data_1();
        
		Short_Pulse();
        for(i=0;i<9;i++)  // Generate 9 clock pulses
		{ 
        	Set_SCK_1();
            Long_Pulse();
            Set_SCK_0();
            Long_Pulse();
        }
        SHT11_Transmission_Start(); //Start Transmission  Cycle.
}


// resets the sensor by a softreset
char SHT11_Reset(void)
{
        //Reset communication with SHT11
		SHT11_Comm_Reset(); 
        //send reset command 
        return (SHT11_Write_Data(Sht11_Reset)); 
}

// Start Measurement 
char SHT11_Start_Measuremnt(unsigned char measure_entity)
{
        char i;
		//transmission start
		SHT11_Transmission_Start(); 

        sensor_mode = measure_entity; // Temp or Humidity, global value to be used in other functions
        i = SHT11_Write_Data(measure_entity);
		if (i)
		{
                return(0); // retrun error
        }
        return(1);
}

// Check whether daya is avilable for reading and processing purpose 
// Has to be polled 
char  SHT11_Check_Conversion(void)
{
        char i = 0;
		i = Check_Data();
		if (i !=0)
		{
        	return(0);  // data not avialable after conversion
        }
        return(1);  // data available after conversion
}

// Read data , first MSB and then LSB Byte
// Read CRC checksum
// Compute check sum and then generate error if any

char SHT11_Read_Raw_Data(unsigned int *sensor_data, unsigned char *chk1, unsigned char *chk2)
{
        unsigned char  i, msb_byte,lsb_byte, checksum, crc_state=0; 
        // Compute  CRC8 of the data recived 
		// intial value is 0
		i = swapbits(sensor_mode);
        crc_state=crc8(crc_state, i);

        msb_byte=SHT11_Read_Data(0); //// Read MSB data


        i= swapbits(msb_byte);
		crc_state=crc8(crc_state, i);

        lsb_byte=SHT11_Read_Data(0); // Read LSB data
        
		i = swapbits(lsb_byte);
        crc_state=crc8(crc_state, i);

        checksum =SHT11_Read_Data(1) ; //read checksum

        *chk1 = crc_state   ; /// return checksum and state value to check for crc errors
        *chk2 = checksum ;
		if (crc_state != checksum ) 
		{
        
		        return(0);
        }
		
		// Raw Data available from SHT11, converted to  sensor data
		*sensor_data = (msb_byte<<8) + lsb_byte;
        return(1);
}

//Convert  disgital data into temperature
// 14 bits at 5v 
// Refer manual  for any other bits and voltage operations
int Temp_Calc(unsigned int resolution)
{
        resolution = ((resolution/10) - 401);
        return(resolution);
}

// Generate a delay for short pulse width
void Short_Pulse(void)
{
	_delay_loop_1(5);
}

// Generate a delay for long pulse width
void Long_Pulse(void)
{
	_delay_loop_1(15);
}

// Set clock to 1
void Set_SCK_1(void)
{
	PORTB |= (1<<PORTB0);
}

// Set clock to 0
void Set_SCK_0(void)
{
	PORTB &= ~(1<<PORTB0);
}

// Set SCk Port
void SCK_Port(void)
{
	DDRB |= (1<<DDB0);
}

// Check Data 
char Check_Data(void)
{
	 char i;
	 i = ( PIND & (1<<Sensor_Data_Pin) );
	return(i);
}

// Internally Pull up Data pin
void Data_Pull_Up_1(void)
{
	PORTD |=  (1<<Sensor_Data_Pin);
}


// Set Data in Out Mode
void Data_Out_Mode(void)
{
	DDRD  |=  (1<<Sensor_Data_DDR);
}

// Set Data in In Mode
void Data_In_Mode(void)
{
	DDRD  = DDRD & ~(1<<Sensor_Data_DDR);
}



// Set Data line  to 0
void Set_Data_0(void)
{
	PORTD = PORTD  &   ~(1<<Sensor_Data_Port);
}

// Set Data line  to 1
void Set_Data_1(void)
{
	PORTD = PORTD | (1<<Sensor_Data_Port);
}




