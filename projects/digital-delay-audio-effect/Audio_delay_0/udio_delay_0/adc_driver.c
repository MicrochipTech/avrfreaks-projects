/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *      XMEGA ADC driver source file.
 *
 *      This file contains the function implementations the XMEGA ADC driver.
 *
 *      The driver is not intended for size and/or speed critical code, since
 *      most functions are just a few lines of code, and the function call
 *      overhead would decrease code performance. The driver is intended for
 *      rapid prototyping and documentation purposes for getting started with
 *      the XMEGA ADC module.
 *
 *      For size and/or speed critical code, it is recommended to copy the
 *      function contents directly into your application instead of making
 *      a function call.
 *
 *      Several functions use the following construct:
 *          "some_register = ... | (some_parameter ? SOME_BIT_bm : 0) | ..."
 *      Although the use of the ternary operator ( if ? then : else ) is discouraged,
 *      in some occasions the operator makes it possible to write pretty clean and
 *      neat code. In this driver, the construct is used to set or not set a
 *      configuration bit based on a boolean input parameter, such as
 *      the "some_parameter" in the example above.
 *
 * \par Application note:
 *      AVR1300: Using the XMEGA ADC
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 2793 $
 * $Date: 2009-09-21 11:12:00 +0200 (ma, 21 sep 2009) $  \n
 *
 * Copyright (c) 2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/
#include "adc_driver.h"


/*! \brief This function get the calibration data from the production calibration.
 *
 *  The calibration data is loaded from flash and stored in the calibration
 *  register. The calibration data reduces the non-linearity error in the adc.
 *
 *  \param  adc          Pointer to ADC module register section.
 */
void ADC_CalibrationValues_Load(ADC_t * adc)
{
	if(&ADCA == adc){
		 /* Get ADCACAL0 from production signature . */
		adc->CALL = SP_ReadCalibrationByte( PROD_SIGNATURES_START + ADCACAL0_offset );
		adc->CALH = SP_ReadCalibrationByte( PROD_SIGNATURES_START + ADCACAL1_offset );
	}else {
		/* Get ADCBCAL0 from production signature  */
		adc->CALL = SP_ReadCalibrationByte( PROD_SIGNATURES_START + ADCBCAL0_offset );
		adc->CALH = SP_ReadCalibrationByte( PROD_SIGNATURES_START + ADCBCAL1_offset );
	}
}


/*! \brief This function clears the interrupt flag and returns the unsigned coversion result.
 *
 *	This function should be used together with the ADC_Ch_Conversion_Complete.
 *      When the conversion result is ready this funciton reads out the result.
 *
 *  \param  adc_ch  Pointer to ADC channel register section.
 *  \param  offset  Unsigned offset value to subtract.
 *  \return  The unsigned Conversion result with the offset substracted.
 */
uint16_t ADC_ResultCh_GetWord_Unsigned(ADC_CH_t * adc_ch, uint8_t offset)
{
  	uint16_t answer;

	/* Clear interrupt flag.*/
	adc_ch->INTFLAGS = ADC_CH_CHIF_bm;

	/* Return result register contents*/
	answer = adc_ch->RES - offset;

	return answer;
}


/*! \brief This function clears the interrupt flag and returns the signed coversion result.
 *
 *	This function should be used together with the ADC_Ch_Conversion_Complete.
 *      When the conversion result is ready this funciton reads out the result.
 *
 *  \param  adc_ch  Pointer to ADC channel register section.
 *  \param  signedOffset  Offset value to subtract.
 *  \return  The signed Conversion result with the offset substracted.
 */
int16_t ADC_ResultCh_GetWord_Signed(ADC_CH_t * adc_ch, int8_t signedOffset)
{
  	int16_t answer;

	/* Clear interrupt flag.*/
	adc_ch->INTFLAGS = ADC_CH_CHIF_bm;

	/* Return result register contents*/
	answer = adc_ch->RES - signedOffset;

	return answer;
}

/*! \brief This function clears the interrupt flag and returns the coversion result without compensating for offset.
 *
 *	This function should be used together with the ADC_Ch_Conversion_Complete.
 *      When the conversion result is ready this funciton reads out the result.
 *
 *  \param  adc_ch  Pointer to ADC channel register section.
 *  \return  Signed conversion result.
 */
uint16_t ADC_ResultCh_GetWord(ADC_CH_t * adc_ch)
{
	/* Clear interrupt flag.*/
	adc_ch->INTFLAGS = ADC_CH_CHIF_bm;

	/* Return result register contents*/
	return adc_ch->RES;;
}


/*! \brief This function clears the interrupt flag and returns the low byte of the coversion result.
 *
 *	This funtion should be used together with the ADC_Ch_Conversion_Complete.
 *      When the conversion result is ready this funciton reads out the result.
 *
 *  \note  If this function is used with 12-bit right adjusted results, it
 *         returns the 8 LSB only. Offset is not compensated.
 *
 *  \param  adc_ch  Pointer to ADC channel register section.
 *
 *  \return  Low byte of conversion result.
 */
uint8_t ADC_ResultCh_GetLowByte(ADC_CH_t * adc_ch)
{
	/* Clear interrupt flag.*/
	adc_ch->INTFLAGS = ADC_CH_CHIF_bm;
	/* Return result register contents*/
	return adc_ch->RESL;
}

/*! \brief This function clears the interrupt flag and returns the high byte of the coversion result.
 *
 *	This funtion should be used together with the ADC_ResultCh_ConversionComplete.
 *      When the conversion result is ready this function reads out the result.
 *
 *  \note  If this function is used with 12-bit right adjusted results, it
 *         returns the 8 LSB only. Offset is not compensated.
 *
 *  \param  adc_ch  Pointer to ADC channel register section.
 *
 *  \return  High byte of conversion result.
 */
uint8_t ADC_ResultCh_GetHighByte(ADC_CH_t * adc_ch)
{
	/* Clear interrupt flag.*/
	adc_ch->INTFLAGS = ADC_CH_CHIF_bm;

	/* Return low byte result register contents.*/
	return adc_ch->RESH;
}

/*! \brief This function waits until the adc common mode is settled.
 *
 *  After the ADC clock has been turned on, the common mode voltage in the ADC
 *  need some time to settle. The time it takes equals one dummy conversion.
 *  Instead of doing a dummy conversion this function waits until the common
 *  mode is settled.
 *
 *  \note The function sets the prescaler to the minimum value to minimize the
 *        time it takes the common mode to settle. If the clock speed is higher
 *        than 8 MHz use the ADC_wait_32MHz function.
 *
 *  \param  adc Pointer to ADC module register section.
 */
void ADC_Wait_8MHz(ADC_t * adc)
{
  	/* Store old prescaler value. */
  	uint8_t prescaler_val = adc->PRESCALER;

	/* Set prescaler value to minimum value. */
	adc->PRESCALER = ADC_PRESCALER_DIV4_gc;

	/* Wait 4*COMMON_MODE_CYCLES for common mode to settle. */
	delay_us(4*COMMON_MODE_CYCLES);

	/* Set prescaler to old value*/
	adc->PRESCALER = prescaler_val;
}


/*! \brief This function waits until the adc common mode is settled.
 *
 *  After the ADC clock has been turned on, the common mode voltage in the ADC
 *  need some time to settle. The time it takes equals one dummy conversion.
 *  Instead of doing a dummy conversion this function waits until the common
 *  mode is settled.
 *
 *  \note The function sets the prescaler to the minimum value possible when the
 *        clock speed is larger than 8 MHz to minimize the time it takes the
 *        common mode to settle.
 *
 *  \note The ADC clock is turned off every time the ADC i disabled or the
 *        device goes into sleep (not Idle sleep mode).
 *
 *  \param  adc Pointer to ADC module register section.
 */
void ADC_Wait_32MHz(ADC_t * adc)
{
  	/* Store old prescaler value. */
  	uint8_t prescaler_val = adc->PRESCALER;

	/* Set prescaler value to minimum value. */
	adc->PRESCALER = ADC_PRESCALER_DIV8_gc;

	/* wait 8*COMMON_MODE_CYCLES for common mode to settle*/
	delay_us(8*COMMON_MODE_CYCLES);

	/* Set prescaler to old value*/
	adc->PRESCALER = prescaler_val;
}

/*! \brief This function gets the offset of the ADC when it is configured in unsigned mode
 *
 *   This function does one or several measurements to determine the offset of
 *   the ADC. 
 *
 *  \note The ADC must be configured and enabled before this function is run.
 *
 *  \note This function only return the low byte of the 12-bit convertion,
 *        because the offset should never be more than +-8 LSB off.
 *
 *  \param adc Pointer to the ADC to calculate offset from.
 *  \param ch Pointer to the ADC channel to measure on.
 *  \param oversampling false for one measurement. true for averaging several measurements.
 *
 *  \return Offset on the selected ADC
 */
uint8_t ADC_Offset_Get_Unsigned(ADC_t * adc, ADC_CH_t *ch, bool oversampling)
{
    if (oversampling)
    {
      uint16_t offset=0;
      for (int i=0; i<4; i++)
      {
        /* Do one conversion to find offset. */
        ADC_Ch_Conversion_Start(ch);
    
        do{
        }while(!ADC_Ch_Conversion_Complete(ch));
        offset += ADC_ResultCh_GetWord_Unsigned(ch, 0x00);
      }
      return ((uint8_t)(offset>>2));
    }
    else
    {        
      uint8_t offset=0;
      
      /* Do one conversion to find offset. */
      ADC_Ch_Conversion_Start(ch);
  
      do{
      }while(!ADC_Ch_Conversion_Complete(ch));
      offset = (uint8_t)ADC_ResultCh_GetWord(ch);
      
      return offset;
    }
}

/*! \brief This function gets the offset of the ADC when it is configured in signed mode
 *
 *   This function does one or several measurements to determine the offset of
 *   the ADC. 
 *
 *  \note The ADC must be configured and enabled before this function is run.
 *
 *  \note This function only return the low byte of the 12-bit convertion,
 *        because the offset should never be more than +-8 LSB off.
 *
 *  \param adc Pointer to the ADC to calculate offset from.
 *  \param ch Pointer to the ADC channel to measure on.
 *  \param oversampling false for one measurement. true for averaging several measurements.
 *
 *  \return Offset on the selected ADC
 */
int8_t ADC_Offset_Get_Signed(ADC_t * adc, ADC_CH_t *ch, bool oversampling)
{
    if (oversampling)
    {
      int16_t offset=0;
      for (int i=0; i<4; i++)
      {
        /* Do one conversion to find offset. */
        ADC_Ch_Conversion_Start(ch);
    
        do{
        }while(!ADC_Ch_Conversion_Complete(ch));
        offset += ADC_ResultCh_GetWord_Signed(ch, 0x00);
      }
      return ((int8_t)(offset/4));
    }
    else
    {        
      int8_t offset=0;
      
      /* Do one conversion to find offset. */
      ADC_Ch_Conversion_Start(ch);
  
      do{
      }while(!ADC_Ch_Conversion_Complete(ch));
      offset = (uint8_t)ADC_ResultCh_GetWord_Signed(ch, 0x00);
      
      return offset;
    }
}

               
#ifdef __GNUC__

/*! \brief Function for GCC to read out calibration byte.
 *
 *  \note For IAR support, include the adc_driver_asm.S90 file in your project.
 *
 *  \param index The index to the calibration byte.
 *
 *  \return Calibration byte.
 */
uint8_t SP_ReadCalibrationByte( uint8_t index )
{
	uint8_t result;

	/* Load the NVM Command register to read the calibration row. */
	NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
 	result = pgm_read_byte(index);

	/* Clean up NVM Command register. */
 	NVM_CMD = NVM_CMD_NO_OPERATION_gc;

	return result;
}

#endif
