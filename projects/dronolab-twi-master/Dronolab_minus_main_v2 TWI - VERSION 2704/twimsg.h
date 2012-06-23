/*******************************
 * Projet Cortex  -  Dronolab  *
 * http://dronolab.etsmtl.ca/  *
 *******************************/

#ifndef TWIMSG_H
#define TWIMSG_H

#include <stdint.h>

//Framework
#include <asf.h>

//Foward declaration
class Twi;


class TwiMsg
{
	friend class Twi;											 //Twi objects need to have access to private members (size)
	friend void global_twi_interrupt_handler(Twi* twi_instance); //Twi objects need to have access to private members in interrupt handler

		/** FIFO position cursor **/
	uint8_t size, pos;

	/** Memorize direction (read or write) of the message **/
	bool readmode;

	/** Message data **/
	/** Maximum of 1 address + 5 data **/
	union
	{
		uint8_t  addr;
		uint8_t  write;
		uint8_t* read;
	} buffer[DRONOLAB_TWI_MSG_SIZE];


	/** Get the device address of current message
	 ** @param [out] data address (reference to) of the device
	 */
	void get_addr(uint8_t & data);

	/** Write next data to device
	 ** @param [out] data Data (reference to) to fill with data
	 ** @return true if it's the last data to be written, false otherwise 
	 */
	bool get_write(uint8_t & data);

	/** Read the next data send by the device 
	 ** @note This function assume that the data is ready to be read at the moment this function is called. If called to slow, the data in rhr may change
	 ** @param [out] data Pointer to the data to be read
	 ** @return true if it's the last data to be read, false otherwise
	 */
	bool get_read(uint8_t ** data);
	
	/** Set the direction of the message, read (TwiMsg::READ) or a write (TwiMsg::WRITE)
	 ** @note By default the message is in write mode 
	 ** @note This function must be called before data_to_*
	 ** @param [in] read_nowrite Indicate if the message is a read (TwiMsg::READ) or write (TwiMsg::WRITE)
	 **/
	void mode(bool read_nowrite);
	
	/** Reset message 
	 ** @param [in] read_nowrite Indicate if the message is a read (TwiMsg::READ) or a write (TwiMsg::WRITE)
	 **/
	void reset(bool read_nowrite);
	
	
public:
	static const bool READ;
	static const bool WRITE;

	/** Initialize FIFO position cursor
	 ** 
	 **/
	TwiMsg();

	/** Add a data to be sent 
	 ** @note There is no memory check on the message buffer
	 ** @param [in] data data to be sent
	 **/
	void data_to_send(uint8_t data);

	/** Add data to be received 
	 ** @note There is no memory check on the message buffer
	 ** @param [in] data Data (reference to it) to be filled on reception
	 **/
	void data_to_recv(uint8_t * data);

	/** Define the address of the device 
	 ** @param [in] data Address of slave 
	 **/
	void address(uint8_t data);

	/** Indicate the direction of the message, read (TwiMsg::READ) or a write (TwiMsg::WRITE)
	 ** @return TwiMsg::READ if in mode read, TwiMsg::WRITE if in mode write
	 */
	bool get_mode();

	/** Test that there is only one message remaning in message FIFO **/
	/** @return true if the FIFO contains 1 message, false otherwise **/
	bool is_one_msg_remaning();

	/** Indicate if the buffer msg is emptyIndique si le buffer est vide
	 ** @return true if empty, false otherwisw
	 */
	bool is_empty();

};

#endif

