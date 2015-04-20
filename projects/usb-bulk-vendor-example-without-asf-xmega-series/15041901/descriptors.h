/*
*
*	4-18-2015 tcj
*
*	USB descriptors for xmega_bulk_usb_example.c
*
*
*
*/





volatile uint8_t des01[18] = 		// DEVICE DESCRIPTOR
{
0x12, 			// 1 size of descriptor in bytes		
0x01,			// 1 DEVICE descriptor
0x10,			// 2 USB spec rev 1.10
0x01, 
0, 				// 1 device class: device defined at interface level
0,				// 1 device subclass: unused
0,				// 1 device protocol: unused
0x40,			// 1 max packet size: 64 bytes (for endpoint 0)
0x66,			// 2 vendor id
0x66,
0xbb,			// 2 product id
0x89, 
0, 				// 2 device release code
0, 
0x07,			// 1 index of string descriptor containing company name
0x08,			// 1 index of string descriptor containing product name
0,				// 1 index of string descriptor containing serial number
0x01			// 1 number of configurations
};  	


volatile uint8_t des02[9] = 		// CONFIGURATION DESCRIPTOR
{
0x09,			// 1 size of descriptor in bytes
0x02,			// 1 CONFIGURATION descriptor
0x20,			// 2 length of total configuration block, including this descriptor, in bytes
0x00,
0x01,			// 1 number of interfaces
0x00,			// 1 ID of this configuration
0x00,			// 1 index of string descriptor describing this configuration
0x80,			// 1 attributes: bus powered, no remote wakeup
0x0a			// 1 max power in 2 mA increments: 20 mA
};


volatile uint8_t des03[9] =				// BULK VENDOR INTERFACE DESCRIPTOR - INTERFACE 0
{
0x09,			// 1 size of descriptor in bytes
0x04,			// 1 INTERFACE descriptor
0X00,			// 1 index of this interface
0X00,			// 1 index of this alternate setting
0x02,			// 1 number of endpoints
0xff,			// 1 interface class: vendor-defined
0xff,			// 1 interface sub-class: n/a
0xff,			// 1 protocol: n/a
0X00			// 1 index of string descriptor describing this interface
};										





volatile uint8_t des04[7] =				// ENDPOINT DESCRIPTOR, BULK VENDOR - ENDPOINT 1 OUT
{
0x07,			// 1 size of descriptor in bytes
0x05,			// 1 ENDPOINT descriptor
0x01,			// 1 OUT endpoint 1
0x02,			// 1 attributes: BULK
0x40,			// 2 max packet size: 64
0x00,
0x00			// polling interval: 0
};
	
volatile uint8_t des05[7] =				// ENDPOINT DESCRIPTOR, BULK VENDOR - ENDPOINT 1 IN
{
0x07,			// 1 size of descriptor in bytes
0x05,			// 1 ENDPOINT descriptor
0x81,			// 1 IN endpoint 1
0x02,			// 1 attributes: BULK
0x40,			// 2 max packet size: 64
0x00,
0x00			// polling interval: 0
};
				
//===========================================================  strings
	

char *defined_strings[] =			// LIST OF STRING DESCRIPTORS USED
{
"Gargoyle Extension SA",			// company: index 7
"ATXmega USB Bulk Example"			// product: index 8
};

			
uint8_t indexorder[] = {7, 8};		// string descriptor index numbers, in same order as their corresponding strings above


