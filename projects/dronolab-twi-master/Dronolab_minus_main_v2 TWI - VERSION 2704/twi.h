/*******************************
 * Projet Cortex  -  Dronolab  *
 * http://dronolab.etsmtl.ca/  *
 *******************************/

#ifndef TWI_H_
#define TWI_H_

#include "twimsg.h"

//Framework
#include <asf.h>

//Must be friend of TwiMsg class
extern void global_twi_interrupt_handler();

class Twi
{
	
	friend void global_twi_interrupt_handler(Twi* twi_instance);
	
	/** Buffer for reception and transmission **/
	TwiMsg buffer[DRONOLAB_TWI_BUFFER_SIZE];

	/** Current message for construction **/
	TwiMsg *current, *tosend;

	/** Specify if the twi bus is currently active (true) or disabled (false)**/
	volatile bool started;
	
	/** Pointer to Twi register of UC3C **/
	volatile avr32_twim_t* avr_twi_reg;
	
	/** Specify which interrupt flag we should catch on next interrupt **/ 
	volatile unsigned long twi_it_mask;

	/** Jump to next message to be send/receive
	 ** @return true if there is a next message, false otherwise
	 **/
	bool next_message();

	/** Active the Twi bus and start the communication if the bus is currently disabled **/
	void run_if_inactive();

	/** Start the communication if the bus is idle **/
	void initiate_communication();

	/** Disable Twi bus **/
	void twi_disable_interrupt();
	
	/** Initiate next twi transaction with slave
	 ** if a transaction is already underway, it is discarded
	 ** if there is no more TwiMsg to be treated, then disable the twi bus
	 **/	
	void next_transaction();
	
	/** Set desired twi bus speed
	 ** @param [in] speed Twi bus desired speed in hz
	 ** @param [in] bus_hz Twi clock bus speed (F_PBA, F_PBB or F_PBC)
	 ** @note Copy of fonction of ASF 3.1.3
	 **/
	void setSpeed(unsigned int speed, unsigned long bus_hz);
	
	
public:

	/** Twi class constructor, init buffer pointers 
	**  @param [in] avr_twi_reg AVR32 Twi pointer
	**/
	Twi(volatile avr32_twim_t* avr_twi_reg);
	
    /** Init current twi bus and configure the necessarily register
	 ** @param [in] speed Twi bus desired speed in hz
	 ** @param [in] bus_hz Twi clock bus speed (F_PBA, F_PBB or F_PBC)  	
	 **/
	void init_master(unsigned int speed, unsigned long bus_hz);
	
	/** Start the building of a new message
	 ** @param [in] mode Define the direction of the current message, read (TwiMsg::READ) or write (TwiMsg::WRITE)
	 ** @return The pointer to the message being currently build
	 **/
	TwiMsg * build_new_message(bool mode);

	/** Confirm that the message is finish to be build and is ready to be sent
	 ** Once this function is called, the message is added to the FIFO containing the other TwiMsg
	 ** @note This function must be called AFTER build_new_message
	 **/
	void message_builded();
	
	
};

#endif /* TWI_H_ */
