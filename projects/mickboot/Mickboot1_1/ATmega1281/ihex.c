/*
ascii characters are passed to the ihex parser one at a time via ihex_char(char)
data and address information becomes available in ihex_address, ihex_byte_count, ihex_data[]

*/

#include <includes.h>

//maximum character length of hex line (must be even)
#define MAXLEN 60

#define IHEX_TYPE_DATA			00
#define IHEX_TYPE_EOF			01
#define IHEX_TYPE_EXT_SEG_ADD	02
#define IHEX_TYPE_START_SEG_ADD	03
#define IHEX_TYPE_EXT_LIN_ADD	04
#define IHEX_TYPE_START_LIN_ADD	05

//Prototypes
char ascii2hex(char* ascii);
void ihex_process_line(void);

//Data types
	struct ihex_struct
	{
		char byte_count;
		unsigned int address;			//big endian
		char type;
		char data[1];
	};

	union hexline_union
	{
		struct ihex_struct ihex;
		char raw[MAXLEN];
		char ascii[MAXLEN];
	};


// interface
	unsigned long ihex_address=0;	//actual data address = (ext_lin_add<<16)+(ext_seg_add<<4)+address;
	unsigned char ihex_byte_count;	//number of data bytes available
	char ihex_data[16];				//actual data bytes
	unsigned char ihex_status;

// global variables
	union hexline_union hexline;
	unsigned long ext_seg_add=0;
	unsigned long ext_lin_add=0;
	char *line_ptr;

//call to initialise ihex parser
void ihex_init()
{
	ext_seg_add=0;
	ext_lin_add=0;
	line_ptr=hexline.ascii;
	*line_ptr=0;
	ihex_status=0;
}

//characters are passed to this function
void ihex_char(char x)
{
	//start of new line?
	if(x==':')
	{
		line_ptr=hexline.ascii;
		*line_ptr=x;
		line_ptr++;
		*line_ptr=0;
	}
	//else if line in progress, append to line
	else if(hexline.ascii[0]==':')
	{
		if(line_ptr != &hexline.ascii[MAXLEN-1])
		{
			*line_ptr++=x;
			*line_ptr=0;
		};
		if(x=='\r')
		{
			ihex_process_line();
			line_ptr=hexline.ascii;
			*line_ptr=0;
		};
	};
}

//called from ihex_char when a hex line is complete
void ihex_process_line(void)
{
	char byte_count, *target, *source, checksum;
	unsigned char index;

	//if valid size
	if(isxdigit(hexline.ascii[1]) && isxdigit(hexline.ascii[2]))
		hexline.ihex.byte_count = ascii2hex(&hexline.ascii[1]);
	else
		hexline.ihex.byte_count=0;

	//convert ascii to hex
	byte_count=hexline.ihex.byte_count+sizeof(struct ihex_struct)-1;
	target = &hexline.raw[1];
	source = &hexline.ascii[3];	//skip : and length field
	if(byte_count > (MAXLEN-6)/2)	//MAXLEN -'CR' -':' -'0terminator' -'size low byte' -'size hi byte'
		byte_count=0;
	while(byte_count)
	{
		*target++=ascii2hex(source);
		source+=2;
		byte_count--;
	};

	//test checksum
	byte_count=hexline.ihex.byte_count+sizeof(struct ihex_struct)-1;
	checksum=0;
	index=0;
	while(byte_count--)
		checksum-=hexline.raw[index++];
	
	if(checksum == hexline.raw[index])
	{
		if(hexline.ihex.type == IHEX_TYPE_DATA)
		{
			//calculate actual address
			ihex_address=(unsigned long)read_bige_uint(hexline.ihex.address);
			ihex_address+=ext_seg_add;
			ihex_address+=ext_lin_add;
			
			//copy data to output
			memcpy(ihex_data, hexline.ihex.data, hexline.ihex.byte_count);
			//copy byte count to output
			ihex_byte_count=hexline.ihex.byte_count;
		}
		else if(hexline.ihex.type == IHEX_TYPE_EXT_SEG_ADD)
		{
			ext_seg_add=hexline.ihex.data[0];
			ext_seg_add<<=8;
			ext_seg_add+=hexline.ihex.data[1];
			ext_seg_add<<=4;
		}
		else if(hexline.ihex.type == IHEX_TYPE_EXT_LIN_ADD)
		{
			ext_lin_add=hexline.ihex.data[0];
			ext_lin_add<<=8;
			ext_lin_add+=hexline.ihex.data[1];
			ext_lin_add<<=16;
		}
		else if(hexline.ihex.type == IHEX_TYPE_EOF)
			ihex_status=IHEX_STATUS_EOF;
	}
	else
		ihex_status=IHEX_STATUS_ERROR;
}

char ascii2hex(char* ascii)
{
	char retval=0;
	if(ascii[0] > '9')
		retval += 0x0A + ascii[0] -'A';
	else
		retval += 0x00 + ascii[0] -'0';
	retval<<=4;
	if(ascii[1] > '9')
		retval += 0x0A + ascii[1] -'A';
	else
		retval += 0x00 + ascii[1] -'0';
	return retval;
}
