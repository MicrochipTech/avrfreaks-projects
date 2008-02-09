/**************************************************************************************************
---------------------------------------------------------------------------------------------------
	Copyright (c) 2007, Jonathan Bagg
	All rights reserved.

	 Redistribution and use in source and binary forms, with or without modification, are permitted 
	 provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of 
	  conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of 
	  conditions and the following disclaimer in the documentation and/or other materials provided 
	  with the distribution.
    * Neither the name of Jonathan Bagg nor the names of its contributors may be used to 
	  endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
  AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
  POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------------------------------
   Project name : Infidigm AVR Drivers
   Processor	: AT90CAN32/64/128
   File name    : can.h
---------------------------------------------------------------------------------------------------
   Modifications: 

   Revision 1.0  2007/05/19 	Bagg
   - Initial Release
   
---------------------------------------------------------------------------------------------------
   Created      : 10 March 2007                Author(s) : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   CAN bus Interrupt Based Driver 
---------------------------------------------------------------------------------------------------
**************************************************************************************************/

/**************************************************************************************************
*   Select CAN version
**************************************************************************************************/
#define CAN2A	0
#define CAN2B	1
#define CAN_V	CAN2A			//Only edit this line to be CAN2A or CAN2B

/**************************************************************************************************
*   Pre-Processor ONLY! - Do not edit!
**************************************************************************************************/
#define	CAN_FRAME_DATA_LENGTH	8
#if CAN_V==CAN2A
	#define	CAN_FRAME_MSG_LENGTH	3
	#define	CAN_IDE					0
#elif CAN_V==CAN2B
	#define	CAN_FRAME_MSG_LENGTH	5
	#define	CAN_IDE					_BV(IDE)
#endif
#define CAN_FRAME_SIZE			(CAN_FRAME_DATA_LENGTH + CAN_FRAME_MSG_LENGTH)

/**************************************************************************************************
*   CAN Frame Message Structure
**************************************************************************************************/
union can_frame {
	struct {
		#if CAN_V==CAN2A
		unsigned int	rb0					: 1;	// RB0 bit
		unsigned int	filler				: 1;	// filler bit
		unsigned int	rbr					: 1;	// RBR bit
		unsigned int	filler1				: 1;	// filler bit
		unsigned int	filler2				: 1;	// filler bit
		unsigned int	id					:11;
		#elif CAN_V==CAN2B
		unsigned int	rb0					: 1;	// RB0 bit
		unsigned int	rb1					: 1;	// RB1 bit
		unsigned int	rbr					: 1;	// RBR bit
		unsigned long	id					:29;
		#endif
		unsigned int	length 				: 4;	// data length in bytes
		unsigned int	filler3				: 4;	// filler bits
		unsigned char	data[CAN_FRAME_DATA_LENGTH];
	};
	unsigned char		array[CAN_FRAME_MSG_LENGTH + CAN_FRAME_DATA_LENGTH];
};

/**************************************************************************************************
*
*   Function          : init_can
*
*   Description       : Sets up the CAN controller.  Inits Mob 0 as TX and Mob 1 as RX.
*
*   Parameters        : None
*
*   Functions Called  : None
*
*   Returns           : None
*
**************************************************************************************************/
void init_can(void);

/**************************************************************************************************
*
*   Signal	          : SIG_CAN_INTERRUPT1
*
*   Description       : CAN Interrupt Service Routine.  Loads the next can frame to transmit from
*						the tx buffer to the MoB.  Unloads the received can frame from the
*						receiver Mob to the the rx buffer.
*
*   Parameters        : None
*
*   Functions Called  : None
*
*   Returns           : None
*
**************************************************************************************************/

/**************************************************************************************************
*
*   Function          : send_can_frame
*
*   Description       : Adds a can frame to be transmitted.  If send_can_frame() returns true, the
*						added can frame will be transmitted by the can controller when the
*						can bus is free.  After send_can_frame() returns, the added can_frame
*						is on the tx buffer (if returned true), and the originating can_frame in
*						the application can be modified / destroyed.
*
*   Parameters        : pointer to the CAN message
*
*   Functions Called  : None
*
*   Returns           : 1 if successfully added can frame to send, 0 if buffer is full
*
*   Example           :	union can_frame can_engin_rpm;
*						can_engin_rpm.id = 0x0123;
*						can_engin_rpm.data[0] = rpm;
*						can_engin_rpm.length = 1;
*						send_can_frame(&can_engin_rpm);
*
**************************************************************************************************/
unsigned char send_can_frame(union can_frame *frame);

/**************************************************************************************************
*
*   Function          : read_can_frame
*
*   Description       : Used for reading data from received CAN messages.  See below RX example.
*
*   Parameters        : None
*
*   Functions Called  : None
*
*   Returns           : Pointer to current can frame in RX buffer
*
**************************************************************************************************/
union can_frame *read_can_frame(void);

/**************************************************************************************************
*
*   Function          : next_can_frame
*
*   Description       : Used to remove the current received CAN frame from the RX buffer and select
*						the next received CAN message, if one exists.  See below RX example.
*
*   Parameters        : None
*
*   Functions Called  : None
*
*   Returns           : None
*
**************************************************************************************************/
void next_can_frame(void);

/**************************************************************************************************
*
*   Function          : new_can_frame
*
*   Description       : Used to check if there is one or more new received CAN frames on the RX 
*						buffer.  See below RX example.
*
*   Parameters        : None
*
*   Functions Called  : None
*
*   Returns           : 1 if there is one or more new received CAN frames, 0 if RX buffer is empty.
*
**************************************************************************************************/
unsigned char new_can_frame(void);

/**************************************************************************************************
*
*   RX Example
*
*   unsigned char rpm;
*
*   for (;;) {
*		if (new_can_frame()) {
*		union can_frame *can_engin_rpm;		//temporarily declare space on stack
*	   	can_engin_rpm = read_can_frame();
*			if (can_engin_rpm->id == 0x0123) {
*				rpm = can_engin_rpm->data[0];
*			}
*			next_can_frame();
*		}
*   }
*
**************************************************************************************************/
