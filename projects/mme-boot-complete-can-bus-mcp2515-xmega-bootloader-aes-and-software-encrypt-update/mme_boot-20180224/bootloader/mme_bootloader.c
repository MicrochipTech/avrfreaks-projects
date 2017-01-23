/*
 * MME-BOOT - Xmega bootloader with CAN BUS (MCP2515) and AES support. 
 * See config.h for configuration options.
 * 
 * Created: 14.1.2017 15:36:53
 * Author: Marko Mlakar (marko@mlakar.si)
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "spiX.h"
#include "mcp2515.h"
#include "sp_driver.h"
#include "aes.h"
#include "config.h"

uint8_t can_send(CanMessage *msg)
{
	uint8_t res, txbuf_n;
	res = mcp2515_getNextFreeTXBuf(&txbuf_n);
	if (res != MCP_ALLTXBUSY)
	{
		mcp2515_write_canMsg(txbuf_n, msg);
		mcp2515_start_transmit(txbuf_n);		
		return 0;
	}
	else
		return 1;
}
uint8_t can_read(CanMessage *msg)
{
	uint8_t res;
	res = mcp2515_readStatus();
	if (res & MCP_STAT_RXIF_MASK )
	{
		if (res & MCP_STAT_RX0IF)
		{
			mcp2515_read_canMsg( MCP_RXBUF_0, msg);
			mcp2515_modifyRegister(MCP_CANINTF, MCP_RX0IF, 0);
			res = 0;
		}
		else if (res & MCP_STAT_RX1IF)
		{
			mcp2515_read_canMsg( MCP_RXBUF_1, msg);
			mcp2515_modifyRegister(MCP_CANINTF, MCP_RX1IF, 0);
			res = 0;
		}
		else
		{
			res = 1;
		}
		
		if (res == 0)
			return 1;
	}
	
	return 0;
}
uint8_t firmware_read(uint8_t *firmware)
{
	if (SP_ReadUserSignatureByte(0) == 0xF0 && SP_ReadUserSignatureByte(5) == 0x0F)
	{
		firmware[0] = SP_ReadUserSignatureByte(1);
		firmware[1] = SP_ReadUserSignatureByte(2);
		firmware[2] = SP_ReadUserSignatureByte(3);
		firmware[3] = SP_ReadUserSignatureByte(4);
		
		return 1;
	}
	else
		return 0;
}

volatile uint64_t ticks_100us = 0;
volatile uint64_t ticks_1ms = 0;
volatile uint8_t aes_key[] = { AES_KEY };
volatile uint8_t aes_iv[] = { AES_IV };
volatile uint8_t buffer[SPM_PAGESIZE];
volatile uint8_t buffer_decrypted[SPM_PAGESIZE];

// provide 100us ticks
ISR(TCC0_OVF_vect)
{
	static uint8_t c = 0;
	++ticks_100us;
	++c;
	if (c == 10)
	{
		++ticks_1ms;
		c = 0;
	}
}

int main(void)
{		
	
	// Set 32 mhz clock
	CCP = CCP_IOREG_gc;
	OSC.CTRL = OSC_RC32MEN_bm;
	while(!(OSC.STATUS & OSC_RC32MRDY_bm));
	CCP = CCP_IOREG_gc;
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;	

	// Setup timer that triggers overflow every 100us@32 Mhz
	TCC0.CTRLA = 0x05;
	TCC0.PER = 0x32;
	TCC0.INTCTRLA = 0b11;
	CCP = CCP_IOREG_gc;
	PMIC.CTRL = PMIC_IVSEL_bm | PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
	sei();
			
	uint8_t t;
				
	spi_init();
	
	volatile uint8_t crc = 0;
	volatile uint16_t page = 0;
	uint8_t data_sequence = 0;
	uint16_t buffer_index;
	volatile uint8_t res, stat;
	CanMessage msg;
	memset(&msg, 0, sizeof(CanMessage));
	
	uint8_t firmware[4];
	uint8_t firmware_valid = 0;
	
	uint8_t authorized = 0;
	
	uint8_t success;
		
	unsigned long timeout_ticks = 50;
	
	unsigned int speed = 500;
	
	volatile uint16_t lastPage = -1;
	
	res = mcp2515_init(500);
	if (res == MCP2515_OK) 
	{		
		// Set mask & filters to filter out any traffic that does not concern us
		mcp2515_setCANCTRL_Mode(MODE_CONFIG);
		mcp2515_write_can_id(MCP_RXM0SIDH, 0, 0x7FF);
		mcp2515_write_can_id(MCP_RXM1SIDH, 0, 0x7FF);
		mcp2515_write_can_id(MCP_RXF0SIDH, 0, PC_AVR_CAN_ID);
		
		if (mcp2515_setCANCTRL_Mode(MODE_NORMAL) == MCP2515_OK)
		{			
			ticks_1ms = 0;
			while(1)
			{
				// if device is not authorized within first 50ms, try different speed (100). Exit at 100ms.
				if (!authorized && ticks_1ms >= timeout_ticks)
				{
					if (speed == 500)
					{
						speed = 1000;
						res = mcp2515_init(speed);
						// Set mask & filters to filter out any traffic that does not concern us
						mcp2515_setCANCTRL_Mode(MODE_CONFIG);
						mcp2515_write_can_id(MCP_RXM0SIDH, 0, 0x7FF);
						mcp2515_write_can_id(MCP_RXM1SIDH, 0, 0x7FF);
						mcp2515_write_can_id(MCP_RXF0SIDH, 0, PC_AVR_CAN_ID);
						mcp2515_setCANCTRL_Mode(MODE_NORMAL);
						timeout_ticks = ticks_1ms + 50;
					}
					else // all speeds tested
						break;
				}

				if (can_read(&msg))
				{
					if (msg.identifier == PC_AVR_CAN_ID)
					{
						switch(msg.dta[0])
						{
							case 0x99: // LOGIN <device_id> <code>, returns: 0x99 <authorized[1/0]> <device_id> <device_firmware_valid[1/0]> <device_firmware1> <device_firmware2> <device_firmware3> <device_firmware4>
								if (msg.dta[1] == DEVICE_ID && msg.dta[2] == DEVICE_CODE)
								{
									authorized = 1;								
									firmware_valid = firmware_read(&firmware);
								}
								
								if (msg.dta[1] == DEVICE_ID)
								{
									msg.identifier = AVR_PC_CAN_ID;
									msg.dlc = 8;
									msg.dta[0] = 0x99;
									msg.dta[1] = authorized ? 1: 0;
									msg.dta[2] = DEVICE_ID;
									msg.dta[3] = firmware_valid;
									msg.dta[4] = firmware[0];
									msg.dta[5] = firmware[1];
									msg.dta[6] = firmware[2];
									msg.dta[7] = firmware[3];
									can_send(&msg);
								}
								break;
							
							case 0x1: // ERASE_FIRMWARE, returns 0x1 <success[1]>
								if (authorized)
								{
									SP_EraseApplicationSection();
									SP_WaitForSPM();
							
									msg.identifier = AVR_PC_CAN_ID;
									msg.dlc = 2;
									msg.dta[0] = 1;
									msg.dta[1] = 1;
									can_send(&msg);
								}
								break;
							
							case 0x2: // SET_PAGE <pageH> <pageL> (first page = 0), returns 0x2 <success[1]> <pageH> <pageL>
								if (authorized)
								{
									page = ((int)msg.dta[1] << 8) + msg.dta[2];
								
									data_sequence = 0;
									buffer_index = 0;
									memset(&buffer, 0, sizeof(buffer));
									
									msg.identifier = AVR_PC_CAN_ID;
									msg.dlc = 4;
									msg.dta[0] = 0x2;
									msg.dta[1] = 1;
									msg.dta[2] = (page >> 8);
									msg.dta[3] = page;
									can_send(&msg);
								}
								break;
							
							case 0x3: // ADD_PAGE_DATA <sequence> <b1> [<b2>] [<b3>] [<b4>] [<b5>] [<b6>], return 0x3 <success[1/0]> <buf_indexH> <buf_indexL> <sequence + 1>
								if (authorized && msg.dlc >= 3)
								{
									uint8_t seq = msg.dta[1];
								
									success = 0;								
									if (seq == data_sequence && (buffer_index + msg.dlc - 2 <= sizeof(buffer)))
									{
										++data_sequence;
									
										buffer[buffer_index++] = msg.dta[2];
										if (msg.dlc >= 4) buffer[buffer_index++] = msg.dta[3];
										if (msg.dlc >= 5) buffer[buffer_index++] = msg.dta[4];
										if (msg.dlc >= 6) buffer[buffer_index++] = msg.dta[5];
										if (msg.dlc >= 7) buffer[buffer_index++] = msg.dta[6];
										if (msg.dlc == 8) buffer[buffer_index++] = msg.dta[7];
									
										success = 1;
									}
																
									msg.identifier = AVR_PC_CAN_ID;
									msg.dlc = 5;
									msg.dta[0] = 0x3;
									msg.dta[1] = success;
									msg.dta[2] = (buffer_index >> 8);
									msg.dta[3] = buffer_index;
									msg.dta[4] = data_sequence;
									can_send(&msg);
								}
								break;
								
							case 0x33: // ADD_PAGE_DATA_BULK <b1> [<b2>] [<b3>] [<b4>] [<b5>] [<b6>] [b7], return nothing
								if (authorized && msg.dlc >= 2)
								{								
									if (buffer_index + msg.dlc - 2 <= sizeof(buffer))
									{
										buffer[buffer_index++] = msg.dta[1];
										if (msg.dlc >= 3) buffer[buffer_index++] = msg.dta[2];
										if (msg.dlc >= 4) buffer[buffer_index++] = msg.dta[3];
										if (msg.dlc >= 5) buffer[buffer_index++] = msg.dta[4];
										if (msg.dlc >= 6) buffer[buffer_index++] = msg.dta[5];
										if (msg.dlc >= 7) buffer[buffer_index++] = msg.dta[6];
										if (msg.dlc == 8) buffer[buffer_index++] = msg.dta[7];
									}								
								}
								break;
							
							case 0x4: // FLASH_LOADED_PAGE <crc>, returns 0x4 <success[1/0]> <pageH> <pageL> 
								if (authorized)
								{
									crc = 0;
									for (uint16_t i=0; i < sizeof(buffer); ++i) crc += buffer[i];
									success = 0;
									
									uint32_t addr = (uint32_t)APP_SECTION_START + (uint32_t)((uint32_t)page * (uint32_t)SPM_PAGESIZE);
									
									if (msg.dta[1] == crc)
									{				
										if (lastPage == page) // erase if re-sending the flash after timeout
										{
											SP_EraseApplicationPage(addr);
											SP_WaitForSPM();
										}
											
										lastPage = page;
										
										// decrypt using AES
										for (int i=0; i < SPM_PAGESIZE; i+=16)
										{
											AES128_CBC_decrypt_buffer(&buffer_decrypted[i], &buffer[i], 16, (i==0) ? aes_key : 0, (i==0) ? aes_iv : 0);
										}
										
										SP_LoadFlashPage(&buffer_decrypted);
										
										SP_WriteApplicationPage(addr);
										SP_WaitForSPM();
									
										success = 1;
									}
							
									msg.identifier = AVR_PC_CAN_ID;
									msg.dlc = 8;
									msg.dta[0] = 0x4;
									msg.dta[1] = success ? 1 : 0;
									msg.dta[2] = (page >> 8);
									msg.dta[3] = page;
									msg.dta[4] = addr >> 24;
									msg.dta[5] = addr >> 16;
									msg.dta[6] = addr >> 8;
									msg.dta[7] = addr;
									can_send(&msg);
								}
								break;
							
							case 0x10: // NEW_FIRMWARE_VERSION <firmware1> <firmware2> <firmware3> <firmware4>, RETURNS 0x10 <success[0/1]> <firmware1> <firmware2> <firmware3> <firmware4>
								if (authorized && msg.dlc == 5)
								{
									memset(&buffer, 0, sizeof(buffer));
									SP_EraseUserSignatureRow(); SP_WaitForSPM();
							
									firmware[0] = msg.dta[1];
									firmware[1] = msg.dta[2];
									firmware[2] = msg.dta[3];
									firmware[3] = msg.dta[4];
							
									// save new version into user signature area with start (0xF0) and end (0x0F) bytes so we can check if data is valid.
									buffer[0] = 0xF0;
									buffer[1] = firmware[0];
									buffer[2] = firmware[1];
									buffer[3] = firmware[2];
									buffer[4] = firmware[3];
									buffer[5] = 0x0F;
									SP_LoadFlashPage(&buffer);
									SP_WriteUserSignatureRow();
									SP_WaitForSPM();
								
									// read the signature back to verify write was successful
									uint8_t f[4];
								
									success = 0;
									if (firmware_read(&f))
									{
										if (f[0] == firmware[0] && f[1] == firmware[1] && f[2] == firmware[2] &&  f[3] == firmware[3])
											success = 1;
									}
									else
										success = 0;
									
							
									msg.identifier = AVR_PC_CAN_ID;
									msg.dlc = 4;
									msg.dta[0] = 0x10;
									msg.dta[1] = success ? 1 : 0;
									msg.dta[2] = firmware[0];
									msg.dta[3] = firmware[1];
									msg.dta[4] = firmware[2];
									msg.dta[5] = firmware[3];
									can_send(&msg);
								}
								break;
							
							case 0x20:	// RUN_APPLICATION 
								if (authorized)
								{
									goto run_app;
								}
								break;
						}
					}
				}			
			}
		}
	}
	
run_app:
	cli();
	CCP = CCP_IOREG_gc;
	RST.STATUS = 0;
	CCP = CCP_IOREG_gc;
	PMIC.CTRL = 0;
	CCP = CCP_IOREG_gc;	
	SP_LockSPM();
	asm("jmp 0x0");
}