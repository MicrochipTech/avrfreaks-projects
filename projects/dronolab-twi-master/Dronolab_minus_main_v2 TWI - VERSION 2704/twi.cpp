/*******************************
* Projet Cortex  -  Dronolab  *
* http://dronolab.etsmtl.ca/  *
*******************************/

#include "twi.h"

#include <core/console/console.h>
#include <singleton.h>

void twi0_interrupt_handler();
//void twi1_interrupt_handler(); /** Dronolab doesnt use twi1 on Cortex V4 **/
void twi2_interrupt_handler();

Twi::Twi(volatile avr32_twim_t* avr_twi_reg) :
started(false), current(buffer), tosend(buffer) , avr_twi_reg(avr_twi_reg)
{}

void Twi::init_master(unsigned int speed, unsigned long bus_hz)
{
	// Disable all interrupts
	Disable_global_interrupt();

	//Disable all interrupt for twi
	avr_twi_reg->idr = ~0UL;
	
	// Enable master transfer
	avr_twi_reg->cr = AVR32_TWIM_CR_MEN_MASK;

	// Reset TWI
	avr_twi_reg->cr = AVR32_TWIM_CR_SWRST_MASK;

	// Clear SR
	avr_twi_reg->scr = ~0UL;
	
	if((uintptr_t)avr_twi_reg == (uintptr_t)&AVR32_TWIM0)
	{
		// Register TWI handler on level 2
		INTC_register_interrupt(&twi0_interrupt_handler, AVR32_TWIM0_IRQ, AVR32_INTC_INT2);
	}
	else if((uintptr_t)avr_twi_reg == (uintptr_t)&AVR32_TWIM1)
	{
		// Register TWI handler on level 2
		//INTC_register_interrupt(&twi1_interrupt_handler, AVR32_TWIM1_IRQ, AVR32_INTC_INT2);
		
		Singleton::get()->console.print("Dronolab doesnt use twi1 on Cortex V4 \n\r");
		
	}
	else if((uintptr_t)avr_twi_reg == (uintptr_t)&AVR32_TWIM2)
	{
		// Register TWI handler on level 2
		//INTC_register_interrupt(&twi2_interrupt_handler, AVR32_TWIM2_IRQ, AVR32_INTC_INT2);
		
		Singleton::get()->console.print("Dronolab doesnt use twi2 on Cortex V4 yet \n\r");
	}
	else
	{
		Singleton::get()->console.print("AVR32 TWI pointer not valid \n\r");
	}
	
	//Make change active
	avr_twi_reg->sr;

	// Select the speed
	setSpeed(speed , bus_hz);
}	

//Copy Directly From AVR32 Framework
inline void Twi::setSpeed(unsigned int speed, unsigned long bus_hz)
{
	uint32_t f_prescaled;
	uint8_t cwgr_exp = 0;
	
	f_prescaled = (bus_hz / (speed * 2));
	
	// f_prescaled must fit in 8 bits, cwgr_exp must fit in 3 bits
	while ((f_prescaled > 0xFF) && (cwgr_exp <= 0x7)) {
		// increase clock divider
		cwgr_exp++;
		// divide f_prescaled value
		f_prescaled /= 2;
	}
	
	// set clock waveform generator register
	avr_twi_reg->cwgr = ((f_prescaled/2) << AVR32_TWIM_CWGR_LOW_OFFSET)
	| ((f_prescaled - f_prescaled/2) << AVR32_TWIM_CWGR_HIGH_OFFSET)
	| (cwgr_exp << AVR32_TWIM_CWGR_EXP_OFFSET)
	| (0     << AVR32_TWIM_CWGR_DATA_OFFSET)
	| (f_prescaled << AVR32_TWIM_CWGR_STASTO_OFFSET);
}

TwiMsg * Twi::build_new_message(bool mode)
{
	current->reset(mode);
	return current;
}

void Twi::message_builded()
{
	AVR32_ENTER_CRITICAL_REGION();
	
	//Depasse la grosseur du tableau, on loop back
	if ((current+1) > (buffer + (DRONOLAB_TWI_BUFFER_SIZE - 1)))
		current = buffer;
	else
		current +=1;
		
	
	AVR32_LEAVE_CRITICAL_REGION();
	
	run_if_inactive();
}

bool Twi::next_message()
{
	
	bool flag = true;
	
	AVR32_ENTER_CRITICAL_REGION();
	
	//Loop back
	if ((tosend+1) > (buffer + DRONOLAB_TWI_BUFFER_SIZE - 1))
		tosend = buffer;
	else
		tosend += 1;
	
		
	//All msg have been sent
	if(tosend == current) flag = false;
	
	AVR32_LEAVE_CRITICAL_REGION();
	
	return flag;
}

void Twi::run_if_inactive()
{	
	if (!started)
	{
		started = true;
		this->initiate_communication();
	}
}


void Twi::initiate_communication()
{
	
	AVR32_ENTER_CRITICAL_REGION();
	
	//If in write, set slave twi-address , internal adress = 0
	//Send first byte of message

	// Reset the TWIM module to clear the THR register
	avr_twi_reg->cr = AVR32_TWIM_CR_MEN_MASK;
	avr_twi_reg->cr = AVR32_TWIM_CR_SWRST_MASK;
	avr_twi_reg->cr = AVR32_TWIM_CR_MDIS_MASK;

	
	//********** Get Twi message information **************************//
	uint8_t addr; // dest chip address
	tosend->get_addr(addr);
	// La passe de FREDOU , l'adresse en TWI est de 7 bit et non 8!
	addr = addr >> 1;

	//Set the commande to start the transfer
	int nbytes = tosend->size - 1;
	//*****************************************************************//
	
	if (tosend->get_mode() == TwiMsg::READ)
	{
		//Master in mode read
		//Add slave twi-address
		//No slave internal-address
		
		avr_twi_reg->cmdr = (addr << AVR32_TWIM_CMDR_SADR_OFFSET)	//Twi slave adress on 7bits
		| (nbytes << AVR32_TWIM_CMDR_NBYTES_OFFSET)					//Nb of bytes to be transfered
		| (AVR32_TWIM_CMDR_VALID_MASK)								//Current transfert is valid
		| (AVR32_TWIM_CMDR_START_MASK)								//Transmit start condition
		| (AVR32_TWIM_CMDR_STOP_MASK)								//Transmit stop condition after sending Nbytes
		| (AVR32_TWIM_CMDR_READ_MASK);								//This is a read transfert
				
		//Clear all previous interrupts
		avr_twi_reg->idr = ~0UL;
		
		//Set interrupts to be catch by the twi
		twi_it_mask = AVR32_TWIM_IER_ANAK_MASK | AVR32_TWIM_IER_DNAK_MASK | AVR32_TWIM_IER_ARBLST_MASK |  //NACK flags (NACK on adress and NACK on data transfert)
					  AVR32_TWIM_IER_RXRDY_MASK;														 //Communication flags
		avr_twi_reg->ier = twi_it_mask;
		
	}
	else // write
	{
		avr_twi_reg->cmdr = (addr << AVR32_TWIM_CMDR_SADR_OFFSET)	//Twi slave adress on 7bits
		| (nbytes << AVR32_TWIM_CMDR_NBYTES_OFFSET)					//Nb of bytes to be transfered
		| (AVR32_TWIM_CMDR_VALID_MASK)								//Current transfert is valid
		| (AVR32_TWIM_CMDR_START_MASK)								//Transmit start condition
		| (AVR32_TWIM_CMDR_STOP_MASK);								//Transmit stop condition after sending Nbytes
		//This is a write transfert
		//Clear all previous interrupts
		avr_twi_reg->idr = ~0UL;
		
		//Set interrupts to be catch by the twi
		twi_it_mask =  AVR32_TWIM_IER_ANAK_MASK | AVR32_TWIM_IER_DNAK_MASK | AVR32_TWIM_IER_ARBLST_MASK |	//NACK flags (NACK on adress and NACK on data transfert)
					   AVR32_TWIM_IER_TXRDY_MASK;															//Communication flags
		avr_twi_reg->ier = twi_it_mask;		
	}

	//Master enable
	avr_twi_reg->cr = AVR32_TWIM_CR_MEN_MASK;
	
	AVR32_LEAVE_CRITICAL_REGION();
}

void Twi::next_transaction()
{	
	if (next_message())
	{	
		this->initiate_communication();
	}
	else
	{	
		twi_disable_interrupt();
		started = false;		
	}
}

void Twi::twi_disable_interrupt()
{
	
	AVR32_ENTER_CRITICAL_REGION();

	// Clear the interrupt flags
	avr_twi_reg->idr = ~0UL;
	
	// Clear the status flags
	avr_twi_reg->scr = ~0UL;
	
	AVR32_LEAVE_CRITICAL_REGION();
}

__attribute__((__interrupt__))
void twi0_interrupt_handler()
{
	//Should replace Singleton::get()->twi_device by your Twi object
	global_twi_interrupt_handler(&Singleton::get()->twi_device);
}

__attribute__((__interrupt__))
void twi2_interrupt_handler()
{
	//TODO
	//global_twi_interrupt_handler(Twi* twi_instance);
}



inline void global_twi_interrupt_handler(Twi* twi_instance)
{	
	//Current msg to send	
	TwiMsg*					tosend		= twi_instance->tosend;
	volatile avr32_twim_t*	avr_twi_reg = twi_instance->avr_twi_reg;
			
	uint32_t status = avr_twi_reg->sr & twi_instance->twi_it_mask;
	
	//NACK on TWI lines
	if (status & (AVR32_TWIM_SR_ANAK_MASK | AVR32_TWIM_SR_DNAK_MASK | AVR32_TWIM_SR_ARBLST_MASK))
	{
		avr_twi_reg->CMDR.valid	   = 0;		//Invalid communication, discard
		avr_twi_reg->scr		   = ~0UL;	//Clear twi status
		avr_twi_reg->idr		   = ~0UL;	//Disable all interrupts
		
		twi_instance->next_transaction();
	}
	else if (status & AVR32_TWIM_SR_IDLE_MASK) //Wait for bus to finish transmission
	{		
		twi_instance->next_transaction();
	}
	else if(status & AVR32_TWIM_SR_TXRDY_MASK) //TWI WRITE
	{	
		if (tosend->is_empty())
		{
			//Wait for a the bus to generate stop condition, wait for the bus to be idle or in error
			//Clear all previous interrupts
			avr_twi_reg->idr = ~0UL;
			twi_instance->twi_it_mask     = AVR32_TWIM_IER_IDLE_MASK | AVR32_TWIM_IER_ANAK_MASK | AVR32_TWIM_IER_DNAK_MASK | AVR32_TWIM_IER_ARBLST_MASK | AVR32_TWIM_IER_TXRDY_MASK;
			avr_twi_reg->ier = twi_instance->twi_it_mask;
		}
		else	//still have data to transfert
		{
			
			uint8_t data;
			tosend->get_write(data);
			avr_twi_reg->thr = data;
		}
	}
	else if (status & AVR32_TWIM_SR_RXRDY_MASK) // TWI READ
	{		
		if (tosend->is_empty())
		{
			//Wait for a the bus to generate stop condition, wait for the bus to be idle or in error
			//Clear all previous interrupts
			avr_twi_reg->idr = ~0UL;
			twi_instance->twi_it_mask = AVR32_TWIM_IER_IDLE_MASK | AVR32_TWIM_IER_ANAK_MASK | AVR32_TWIM_IER_DNAK_MASK | AVR32_TWIM_IER_ARBLST_MASK | AVR32_TWIM_IER_RXRDY_MASK;
			avr_twi_reg->ier = twi_instance->twi_it_mask;
		}
		else
		{
			//Get pointer to value to be read
			//True => Last value to read
			uint8_t* pdata;
			if(tosend->get_read(&pdata))
			{
				
				*pdata = avr_twi_reg->rhr;
				
				//Wait for a the bus to generate stop condition, wait for the bus to be idle or in error
				//Clear all previous interrupts
				avr_twi_reg->idr = ~0UL;
				twi_instance->twi_it_mask = AVR32_TWIM_IER_IDLE_MASK | AVR32_TWIM_IER_ANAK_MASK | AVR32_TWIM_IER_DNAK_MASK | AVR32_TWIM_IER_ARBLST_MASK | AVR32_TWIM_IER_RXRDY_MASK;
				avr_twi_reg->ier = twi_instance->twi_it_mask;
				
			}
			else
			{	
				*pdata = avr_twi_reg->rhr;
			}							
		}
	}
	else
	{
		twi_instance->next_transaction();
	}

}

