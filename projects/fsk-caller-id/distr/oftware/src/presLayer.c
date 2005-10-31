/*! \file presLayer.c
 * File takes care of the higher OSI layer of the caller id system
 */

#include "define.h"

//!Types of the presentation layer message.
typedef enum{
	DATE_TIME=1,	//!<Date & Time
	CLI=2			//!<identity of the origin of the call
}PRES_LAYER_TYPE;

//!Position of the data in the "date" presentation layer type.
typedef enum{
	MONTH=2,
	DAY=4,
	HOUR=6,
	MIN=8
}DATE_TYPE;

/*! This function show the presentation layer messages on the LCD.  It
 *  receives a pointer to the parametertype.  This parametertype is 
 *  followed by the presentation layer data.  The paramLength determines
 *  the length of the presentation message belonging to that
 *  parametertype.
 *  Currently only two parametertypes are supported, date & time and 
 *  caller id.  It is possible to add some more parameters later on.
 *  For more info on the possible parameters, see section 5.2.1 of 
 *  ETSI EN 300 659-3 V1.3.1 (2001-01).
 *  \param paramType Pointer to the parameter type of the presentation
 *		layer message.
 *  \param paramLength Length of that presentation layer message.
 */
void presLayer(uint8_t* paramType, uint8_t paramLength){
	uint8_t c=0;
	switch(*paramType){
		case DATE_TIME:
			setSentence(paramType+DAY,2,0);
			c='/';
			setSentence(&c,1,2);
			setSentence(paramType+MONTH,2,3);
			setSentence(paramType+HOUR,2,6);
			c=':';
			setSentence(&c,1,8);
			setSentence(paramType+MIN,2,9);
			break;
		case CLI:
			setSentence(paramType+2,paramLength,16);
			break;
	}
}//presLayer