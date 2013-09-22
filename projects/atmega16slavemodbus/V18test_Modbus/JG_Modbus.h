#ifndef _JG_MODBUS_H_
#define _JG_MODBUS_H_

#ifndef FALSE
#define FALSE	0
#endif

#ifndef TRUE
#define TRUE	1
#endif

/* Slave index */
#define HEADER_LENGTH_RTU           1
#define PRESET_QUERY_LENGTH_RTU     6
#define PRESET_RESPONSE_LENGTH_RTU  2

#define CHECKSUM_LENGTH_RTU         2
#define MODBUS_BROADCAST_ADDRESS  255


/* Function codes */
#define FC_READ_COIL_STATUS          0x01  /* discrete inputs */
#define FC_READ_INPUT_STATUS         0x02  /* discrete outputs */
#define FC_READ_HOLDING_REGISTERS    0x03
#define FC_READ_INPUT_REGISTERS      0x04
#define FC_FORCE_SINGLE_COIL         0x05
#define FC_PRESET_SINGLE_REGISTER    0x06
#define FC_READ_EXCEPTION_STATUS     0x07
#define FC_FORCE_MULTIPLE_COILS      0x0F
#define FC_PRESET_MULTIPLE_REGISTERS 0x10
#define FC_REPORT_SLAVE_ID           0x11

/* Protocol exceptions */
#define ILLEGAL_FUNCTION        -0x01
#define ILLEGAL_DATA_ADDRESS    -0x02
#define ILLEGAL_DATA_VALUE      -0x03
#define SLAVE_DEVICE_FAILURE    -0x04
#define SERVER_FAILURE          -0x04
#define ACKNOWLEDGE             -0x05
#define SLAVE_DEVICE_BUSY       -0x06
#define SERVER_BUSY             -0x06
#define NEGATIVE_ACKNOWLEDGE    -0x07
#define MEMORY_PARITY_ERROR     -0x08
#define GATEWAY_PROBLEM_PATH    -0x0A
#define GATEWAY_PROBLEM_TARGET  -0x0B

/* Local */
#define INVALID_DATA            -0x10
#define INVALID_CRC             -0x11
#define INVALID_EXCEPTION_CODE  -0x12

#define SELECT_TIMEOUT          -0x13
#define SELECT_FAILURE          -0x14
#define NOT_FOR_US	        	-0x15
#define CONNECTION_CLOSED       -0x16
#define MB_EXCEPTION            -0x17


/* It's not really the minimal length (the real one is report slave ID
 * in RTU (4 bytes)) but it's a convenient size to use in RTU or TCP
 * communications to read many values or write a single one.
 * Maximum between :
 * - HEADER_LENGTH_TCP (7) + function (1) + address (2) + number (2)
 * - HEADER_LENGTH_RTU (1) + function (1) + address (2) + number (2) + CRC (2)
*/
#define MIN_QUERY_LENGTH           12

/* Modbus_Application_Protocol_V1_1b.pdf Chapter 4 Section 1 Page 5:
 *  - RS232 / RS485 ADU = 253 bytes + slave (1 byte) + CRC (2 bytes) = 256 bytes
 *  - TCP MODBUS ADU = 253 bytes + MBAP (7 bytes) = 260 bytes
 */
#define MAX_PDU_LENGTH            253
#define MAX_ADU_LENGTH_RTU        256

/* Kept for compatibility reasons (deprecated) */
#define MAX_MESSAGE_LENGTH        260

#define EXCEPTION_RESPONSE_LENGTH_RTU  5


/* Internal using */
#define MSG_LENGTH_UNDEFINED -1

/* Slave ID we respond to */
#define SLAVE_ID	1

typedef struct {
	uint8_t		cBit;
	uint8_t		cVal;
} tMBChangedBit ;


//int errorInFrame(uint8_t *frame, uint8_t frameLength);
//int respondFrame(uint8_t *frame, uint8_t frameLength);
// void modbus_slave_manage(const uint8_t *query, int query_length);

void modbus_processSlaveFrame(uint8_t *query, int query_length);
uint8_t getBitCutOut(uint8_t startBit, uint8_t noB, uint8_t *stab, uint8_t *dtab );

#endif

