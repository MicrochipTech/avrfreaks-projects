/*
 * config.h
 *
 * Created: 22.1.2017 4:11:27
 *  Author: Marko Mlakar (marko@mlakar.si)
 */ 


#ifndef CONFIG_H_

#define CONFIG_H_

#define PC_AVR_CAN_ID	0x07EF
#define AVR_PC_CAN_ID	0x07FF

#define DEVICE_ID		80
#define DEVICE_CODE		35

#define CAN_SPEED		500 /* 125, 250, 500, 1000 */

#define AES_KEY			160, 232, 92, 72, 242, 106, 188, 57, 232, 214, 158, 164, 215, 157, 8, 9
#define AES_IV			173, 2, 166, 235, 16, 198, 93, 161, 134, 117, 241, 175, 25, 127, 181, 79
	
#define MCP2515_DDR		PORTE_DIR
#define MCP2515_PORT	PORTE_OUT
#define MCP2515_PIN		3

#define CONFIG_H_

#endif /* CONFIG_H_ */