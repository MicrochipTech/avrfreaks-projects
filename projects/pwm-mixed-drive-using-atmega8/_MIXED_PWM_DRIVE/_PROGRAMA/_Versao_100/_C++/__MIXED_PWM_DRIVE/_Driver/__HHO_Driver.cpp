/*
/////////////////////////////////////////////////////////////////////////////////////////

		 _   _  _   _ _____  ______      _            ______ _       _ _        _
		| | | || | | |  _  | |  _  \    (_)           |  _  (_)     (_) |      | |
		| |_| || |_| | | | | | | | |_ __ ___   _____  | | | |_  __ _ _| |_ __ _| |
		|  _  ||  _  | | | | | | | | '__| \ \ / / _ \ | | | | |/ _` | | __/ _` | |
		| | | || | | \ \_/ / | |/ /| |  | |\ V /  __/ | |/ /| | (_| | | || (_| | |
		\_| |_/\_| |_/\___/  |___/ |_|  |_| \_/ \___| |___/ |_|\__, |_|\__\__,_|_|
																__/ |
															    |___/
/////////////////////////////////////////////////////////////////////////////////////////
--------------------------------- Main Program (tab 4) C++ ------------------------------

==========================================================================================
@AUTOR   : João D´Artagnan Antunes Oliveira
@DATA    : 19 DE MARÇO DE 2015
@HORÁRIO : 15:38
==========================================================================================
@PROPOSTA: Drive para geração de Hidrogênio + Oxigênio usando a tecnica de Stanley Meyer

@HARDWARE:
@OBSERVAÇÃO:
==========================================================================================
______                                      _
| ___ \                                    | |
| |_/ / __ ___   __ _ _ __ __ _ _ __ ___   | |     ___   __ _
|  __/ '__/ _ \ / _` | '__/ _` | '_ ` _ \  | |    / _ \ / _` |
| |  | | | (_) | (_| | | | (_| | | | | | | | |___| (_) | (_| |
\_|  |_|  \___/ \__, |_|  \__,_|_| |_| |_| \_____/\___/ \__, |
                 __/ |                                   __/ |
                 |___/                                   |___/
==========================================================================================
DATA		HORA		DESCRIÇÃO
__________	_____		__________________________________________________________________________
19/03/2014	15:39	    Projeto do Sofware Iniciado.

*/

/*
+--------------------------------------+
|        AVR/GNU C++ COMPILER	       |
+--------------------------------------+

Symbols		    = F_CPU=8000000L
Optimization	= -Os
Miscellaneous	= -std=gnu++11
*/

/*
+--------------------------------------+
|        GLOBALS INCLUSION		       |
+--------------------------------------+
*/

#include "____GLOBAL/_Global.h"

#define _HHO_VERSION    1
#define _HHO_REVISION	12

/*
+--------------------------------------+
|        HARDWARE DEPENDENT INCLUDES   |
+--------------------------------------+
*/
#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/eeprom.h>
//#include <avr/sleep.h>
#include <avr/pgmspace.h>
/*
+--------------------------------------+
|        C LIBRARY INCLUDES		       |
+--------------------------------------+
*/
//#include <string.h>
//#include <stdlib.h>
#include <stdio.h>


/*
+--------------------------------------+
|        HARDWARE RESOURCES	           |
+--------------------------------------+
*/

/*
+--------------------------------------+
|        LIBRARY INCLUDES	           |
+--------------------------------------+
*/

#include "_Time0_Lib.h"
#include "_HD44780_Drive_Lib.h"
#include "_IO.h"

/*
+--------------------------------------+
|        LOCAL LIBRARY INCLUDES	       |
+--------------------------------------+
*/

/*
+--------------------------------------+
|        GLOBAL FLASH CONSTANTS        |
+--------------------------------------+
*/


/*
+--------------------------------------+
|        GLOBAL EEPROM VARIABLES       |
+--------------------------------------+
*/

/*
+--------------------------------------+
|        GLOBAL CONSTANTS              |
+--------------------------------------+
*/

/*
+--------------------------------------+
|        GLOBAL VARIABLES         	   |
+--------------------------------------+
*/

/*
+--------------------------------------+
|        GLOBAL STRUCTS         	   |
+--------------------------------------+
*/

/*
+--------------------------------------+
|        GLOBAL CLASSES                |
+--------------------------------------+
*/

/*
+--------------------------------------+
|        FUNCTIONS FORWARD      	   |
+--------------------------------------+
*/

/*
+--------------------------------------+
|        GLOBAL FUNCTIONS      	       |
+--------------------------------------+
*/

/*
==================================================================================
=        ___  ___      _        ______                                           =
=        |  \/  |     (_)       | ___ \                                          =
=        | .  . | __ _ _ _ __   | |_/ / __ ___   __ _ _ __ __ _ _ __ ___         =
=        | |\/| |/ _` | | '_ \  |  __/ '__/ _ \ / _` | '__/ _` | '_ ` _ \        =
=        | |  | | (_| | | | | | | |  | | | (_) | (_| | | | (_| | | | | | |       =
=        \_|  |_/\__,_|_|_| |_| \_|  |_|  \___/ \__, |_|  \__,_|_| |_| |_|       =
=                                                __/ |                           =
=                                               |___/                            =
==================================================================================
*/

char bf[80];
#define FILTER_SIZE 5

int16_t	f1SortBuf[FILTER_SIZE];
int16_t f1QueueBuf[FILTER_SIZE];
int16_t	f1FSortBuf[FILTER_SIZE];
int16_t f1FQueueBuf[FILTER_SIZE];
int16_t	f2SortBuf[FILTER_SIZE];
int16_t f2QueueBuf[FILTER_SIZE];
int16_t	f2FSortBuf[FILTER_SIZE];
int16_t f2FQueueBuf[FILTER_SIZE];


class_IO				io;
class_Time				time;
class_HD44780_Drive		lcd(_lcd_INTERFACE::SIZE_4_BITS,2,16);


void    SetFreq(long int freq,uint32_t period)
{
    long int fq2,fs2;

    fq2=freq;
    fq2=F_CPU/fq2;
    fs2=io.Range(50,0,100,0,fq2);
    io.PWMSetTopAndFase((uint16_t)243,(uint16_t)_IO_PWM_OC2_MAX_VALUE,(uint16_t)fq2,(uint16_t)fs2);
    time.DelayMsI(period);
}

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  CallBack function to update internal time                         |
//	| @NAME    :  PumpCallBack			          		                            |
//	| @INPUT   :				  													|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS : variable _seg,_min & _hour is updated								|
//	+-------------------------------------------------------------------------------+
uint8_t _seg,_min,_hour;

void	PumpCallBack(uint8_t hour,uint8_t minute,uint8_t second)
{
	_seg=second;
	_min=minute;
	_hour=hour;
}
int main()
{
	int32_t	fq1,fs1,fq2,fs2;
	uint32_t t,toc1,toc2;
	
	//enable bellow line if use timecallback
	//time.SetTimeClockCallback(PumpCallBack);
	//time.SetClockState(true);
	#define defPrescaler	32L
	#define defPwmDiv		64L
	#define defDiv(freq)	(F_CPU/(defPrescaler*defPwmDiv*freq)-1)
	#define defFreq(div)	(F_CPU/(defPrescaler*defPwmDiv*(div+1)))

	
	sei();

	lcd.SetCursorState(OFF);
	lcd.Locate(1,1);
				  //1234567890123456
	lcd.PutSF(PSTR("HHO Drive V"));
	sprintf(bf,"%d.%03d",_HHO_VERSION,_HHO_REVISION);
	lcd.PutS(bf);
	//hi counter 2 interrupt 250khz reduce delay precision
	time.DelayMsS(2000);
	
	time.SetElapsed(&t);
	time.SetElapsed(&toc1);
	time.SetElapsed(&toc2);
	
//     #define F_MIN 2000
//     #define F_MAX 3000
//     #define F_STEPS 20
//     #define F_STEP ((F_MAX-F_MIN)/F_STEPS)
//     #define F_WAVE_TIME 100
//     #define F_TIME (F_WAVE_TIME/F_STEPS)
//     for(;;)
//     {
//         for(uint16_t f=F_MIN;f<F_MAX;f+=F_STEP)
//         {
//             SetFreq((long int)f,F_TIME);
//         }
//         for(uint16_t f=F_MAX;f>F_MIN;f-=F_STEP)
//         {
//             SetFreq((long int)f,F_TIME);
//         }
//         
//     }

    /////////////////////////////////////////
	for(;;)
	{
		///*
		//get F2 FREQUENCY
		//----------------------------------------------------------------------------------
		io.ADCSetChannel(_IO_ADC_CHANNEL_FQ2);
		//convert to frequency range 500..95000Hz
		fq2=io.Range(io.ADCGetValue(),0,1023,500,95000);
		//apply median filter and convert to pwm divisor
		fq2=io.MedianFilter(F_CPU/fq2,f2SortBuf,f2QueueBuf);
		//check for invalid DIV values
		if (fq2<84) fq2=84;
		if (fq2>16000) fq2=16000;
		//get F2 FASE
		io.ADCSetChannel(_IO_ADC_CHANNEL_FS2);
		fs2=io.MedianFilter(io.Range(io.ADCGetValue(),0,1023,0,fq2),f2FSortBuf,f2FQueueBuf);
		//----------------------------------------------------------------------------------
		//get F1 FREQUENCY
		io.ADCSetChannel(_IO_ADC_CHANNEL_FQ1);
		//convert to frequency range 16..436Hz
		fq1=io.Range(io.ADCGetValue(),0,1023,16,436);
		//apply median filter and convert to pwm divisor
		fq1=io.MedianFilter(defDiv(fq1),f1SortBuf,f1QueueBuf);
		//check for invalid DIV values
		if (fq1<7) fq1=7;
		if (fq1>243) fq1=243;
		//get F1 FASE
		io.ADCSetChannel(_IO_ADC_CHANNEL_FS1);
		fs1=io.MedianFilter(io.Range(io.ADCGetValue(),0,1023,0,_IO_PWM_OC2_MAX_VALUE),f1FSortBuf,f1FQueueBuf);
		//Set Freq and Fase
		io.PWMSetTopAndFase((uint16_t)fq1,(uint16_t)fs1,(uint16_t)fq2,(uint16_t)fs2);
		
		//*/
		
		//show values
		lcd.Locate(1,1);
		sprintf(bf,"F1=%4ld F2=%5ld",defFreq(fq1),F_CPU/fq2);
		lcd.PutS(bf);
		lcd.Locate(2,1);
		sprintf(bf,"D1=%3ld",io.Range(fs1,0,_IO_PWM_OC2_MAX_VALUE,0,100));
		lcd.PutS(bf);
		lcd.PutC('%');
		sprintf(bf," D2=%3ld",io.Range(fs2,0,fq2,0,100));
		lcd.PutS(bf);
		lcd.PutC('%');
	
		io.LEDSwap();
	
		if (io.KeyReadFlash()==KEY_DOWN)
		{
			for(;;)
			{
				io.LEDSet(true);
				lcd.Cls();
				//1234567890123456
				lcd.PutSF(PSTR("HHO Drive V"));
				sprintf(bf,"%d.%03d",_HHO_VERSION,_HHO_REVISION);
				lcd.PutS(bf);				
				time.DelayMsI(500);
				lcd.Cls();
				io.LEDSet(false);
				time.DelayMsI(500);
				if (io.KeyReadFlash()==KEY_NONE) break;
			}
		}
		time.DelayMsI(50);
	}
}
