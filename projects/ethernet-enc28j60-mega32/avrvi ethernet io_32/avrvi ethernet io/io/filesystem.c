/*
,-----------------------------------------------------------------------------------------.
| io/filesystem
|-----------------------------------------------------------------------------------------
| this file implements a very very basic (custom) filesystem (->dataflash)
| - 600 files max
| - smallest file unit is 256 Byte
| - store file as linked list across the whole dataflash
| - file extension is always lowercase !!
| - see below for filesystem information
|
| Author   : {{removed according to contest rules}}
|            -> circuitcellar.com avr design contest 2006
|            -> Entry #AT2616
|
|-----------------------------------------------------------------------------------------
| License:
| This program is free software; you can redistribute it and/or modify it under
| the terms of the GNU General Public License as published by the Free Software
| Foundation; either version 2 of the License, or (at your option) any later
| version.
| This program is distributed in the hope that it will be useful, but
|
| WITHOUT ANY WARRANTY;
|
| without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
| PURPOSE. See the GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License along with
| this program; if not, write to the Free Software Foundation, Inc., 51
| Franklin St, Fifth Floor, Boston, MA 02110, USA
|
| http://www.gnu.de/gpl-ger.html
`-----------------------------------------------------------------------------------------*/
#include "filesystem.h"
#include "dataflash.h"

//ACTIVATE DEBUG by editing this file:
#include "../debug.h"

#define FILESYSTEM_REVISION 0x01

/*
store filesystem on dataflash

simple quick and dirty filesystem.
store files in multiples of pagesize, every file chunk
stores a pointer to the next data segment (linked list)

-------------------------------------------------------------------------------
every file consists of blocks with 256+8 Byte (=pagesize)
- byte   0-255: DATA
- byte     256: free/used tag -- 0xE0 -> used page, everything else: free page 
- byte     257: filesystem revision (starts with 0x01)
- byte 258+259: file id (16bit uint) 
- byte 260+261: pointer to next file block
- byte     262: block tag: 0x00 = first block, 0xEE = last block -> blocktag != 0xEE -> read more
- byte     262: how many bytes of this block are used

-------------------------------------------------------------------------------*/
#define FILESYSTEM_FILE_FREETAG  0x100
#define FILESYSTEM_FILE_REV      0x101
#define FILESYSTEM_FILE_ID_HI    0x102
#define FILESYSTEM_FILE_ID_LO    0x103
#define FILESYSTEM_FILE_NEXT_HI  0x104
#define FILESYSTEM_FILE_NEXT_LO  0x105
#define FILESYSTEM_FILE_BLOCKTAG 0x106
#define FILESYSTEM_FILE_BLOCK_USAGE 0x107

/*
-------------------------------------------------------------------------------
filesystem info table is stored in block 0-49
- every entry has 24 bytes:
  - byte  0- 1 : 16 bit file id (uint) (valid fileid: 1...549)
  - byte  2- 5 : 32 bit timestamp (last modified)
  - byte  6-16 : 11 byte filename (8+3 chars)
  - byte 17-18 : address of first block
  - byte 19-20 : address of last block
  - byte 21-23 : RESERVED
-------------------------------------------------------------------------------

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
WARNING: we only support one open file (for writing) at one time !!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

*/

///
/// TODO: 
/// - replace writing of 0s with dataflash erase cmd !!!!
#define FILESYSTEM_TABLE_SIZE 50
#define FILESYSTEM_BLOCK_COUNT 2048

#define FILESYSTEM_STATUS_IDLE      0x00
#define FILESYSTEM_STATUS_FILE_OPEN 0x01

#define FILESYSTEM_FTABLE_ID_HI			   0
#define FILESYSTEM_FTABLE_ID_LO			   1
#define FILESYSTEM_FTABLE_TIMESTAMP_3  2
#define FILESYSTEM_FTABLE_TIMESTAMP_2  3
#define FILESYSTEM_FTABLE_TIMESTAMP_1  4
#define FILESYSTEM_FTABLE_TIMESTAMP_0  5
//position of filename start
#define FILESYSTEM_FTABLE_FILENAME_START  6
#define FILESYSTEM_FTABLE_BLOCK_START_HI 17
#define FILESYSTEM_FTABLE_BLOCK_START_LO 18
#define FILESYSTEM_FTABLE_BLOCK_COUNT_HI 19
#define FILESYSTEM_FTABLE_BLOCK_COUNT_LO 20

volatile unsigned char filesystem_status;
volatile unsigned int  filesystem_fileid;

volatile unsigned int  filesystem_first_block;
volatile unsigned int  filesystem_current_block;
volatile unsigned int  filesystem_block_count;


volatile unsigned char filesystem_filename[8];
volatile unsigned char filesystem_filename_ext[3];

unsigned char filesystem_buffer[256];

volatile unsigned char filesystem_file_pos;

//init
void filesystem_init(){
	filesystem_status = FILESYSTEM_STATUS_IDLE;
	filesystem_fileid = 0;
	filesystem_first_block = 0;
	filesystem_current_block = 0;

	filesystem_file_pos = 0;

	//check if we need to format flash:
	dataflash_copy_page_to_buffer(0, 0);
	if ((dataflash_read_buffer(0, 0)==0xFF) && (dataflash_read_buffer(1, 0)==0xFF))
		filesystem_format();
}

void filesystem_delete_file(unsigned int fileid){
	unsigned int tmp;
	unsigned int tmp_old;
	unsigned int pos;

	//invalid fileid ?
	if (fileid == 0)
		return;

	//copy lookuptable page to dataflash buffer 0
	dataflash_copy_page_to_buffer(((fileid-1)/11), 0);
	
	//pos inside page is ?
	pos = 24*((fileid-1) % 11);

	#if FILESYSTEM_DEBUG
	softuart_puts_progmem("FS  : deleting #");
	softuart_put_uint16(fileid);
	softuart_puts_progmem("... ");
	#endif

	//get first block:
	tmp  = dataflash_read_buffer(pos + FILESYSTEM_FTABLE_BLOCK_START_HI, 0)<<8;
	tmp |= dataflash_read_buffer(pos + FILESYSTEM_FTABLE_BLOCK_START_LO, 0);

	//mark lookup table entry as empty:
	dataflash_write_to_page_buffer(pos+FILESYSTEM_FTABLE_ID_HI,0,0);
	dataflash_write_to_page_buffer(pos+FILESYSTEM_FTABLE_ID_LO,0,0);

	//store lookuptable
	dataflash_copy_buffer_to_page(((fileid-1)/11), 0);

	//clear buffer:
	for (unsigned int j=0; j<256+8; j++)
		dataflash_write_to_page_buffer(j,1,0);

	while (tmp != 0x0000){
		#if FILESYSTEM_DEBUG
		softuart_putc('x');
		#endif
		//copy block to buffer0:
		dataflash_copy_page_to_buffer(tmp, 0);

		//remember page to clear
		tmp_old = tmp;

		//calc next block:
		tmp  = dataflash_read_buffer(FILESYSTEM_FILE_NEXT_HI, 0)<<8;
		tmp |= dataflash_read_buffer(FILESYSTEM_FILE_NEXT_LO, 0);
		
		//tmp should be zero if this is the last block. but we have a flag for last block, too:
		if (dataflash_read_buffer(FILESYSTEM_FILE_BLOCKTAG,0) == 0xEE)
			tmp = 0x0000; //last block!
		else
			dataflash_copy_buffer_to_page(tmp_old,1);
	}
	#if FILESYSTEM_DEBUG
	softuart_puts_progmem(" done");
	softuart_putnewline();
	#endif
}

/*
void filesystem_dump_file(unsigned int fileid){
	unsigned int tmp;
	unsigned int count;
	unsigned int pos;

	//invalid fileid ?
	if (fileid == 0)
		return;

	//copy lookuptable page to dataflash buffer 0
	dataflash_copy_page_to_buffer(((fileid-1)/11), 0);
	
	//pos inside page is ?
	pos = 24*((fileid-1) % 11);

	#if FILESYSTEM_DEBUG
	softuart_puts_progmem("FS  : dumping #");
	softuart_put_uint16(fileid);
	softuart_puts_progmem(", loc on block ");
	softuart_put_uint16((fileid-1)/11);
	softuart_puts_progmem(", pos ");
	softuart_put_uint8(pos);
	softuart_putnewline();
	#endif

	//get first block:
	tmp  = dataflash_read_buffer(pos + FILESYSTEM_FTABLE_BLOCK_START_HI, 0)<<8;
	tmp |= dataflash_read_buffer(pos + FILESYSTEM_FTABLE_BLOCK_START_LO, 0);


	while (tmp != 0x0000){
		//copy block to buffer0:
		dataflash_copy_page_to_buffer(tmp, 0);

		//get bytes used inside this block:
		count = dataflash_read_buffer(FILESYSTEM_FILE_BLOCK_USAGE,0)+1;

		//dump block:
		unsigned int i=0;
		while (count > 0){
			softuart_putc(dataflash_read_buffer(i,0));
			count--;
			i++;
		}
		
		//calc next block:
		tmp  = dataflash_read_buffer(FILESYSTEM_FILE_NEXT_HI, 0)<<8;
		tmp |= dataflash_read_buffer(FILESYSTEM_FILE_NEXT_LO, 0);
		
		//tmp should be zero if this is the last block. but we have a flag for last block, too:
		if (dataflash_read_buffer(FILESYSTEM_FILE_BLOCKTAG,0) == 0xEE)
			tmp = 0x0000; //last block!
	}
	#if FILESYSTEM_DEBUG
	softuart_puts_progmem("FS  : dump done.");
	softuart_putnewline();
	#endif

}*/

//search for given filename [8.3]
//return: 0 if not found, otherwise the file id
unsigned int filesystem_search_file(unsigned char *filename, unsigned char *filename_ext){
	unsigned int tmp;
	unsigned char x;
	unsigned char i,j;
	unsigned char found;

	//make file extension lowercase !!
	for(unsigned char i=0; i<3 && filename_ext[i] != 0; i++){
		if ((filename_ext[i] >= 'A') && (filename_ext[i] <= 'Z'))
			filename_ext[i] = filename_ext[i] - 'A' + 'a';
	}


	#if FILESYSTEM_DEBUG
	softuart_puts_progmem("FS  : search file <");
	x=0;
	while((x<8) && filename[x])
		softuart_putc(filename[x++]);
	softuart_putc('>');
	softuart_putc('<');
	x=0;
	while((x<3) && filename_ext[x])
		softuart_putc(filename_ext[x++]);
	softuart_puts_progmem("> ");
	#endif
	
	//search all filesystem table entries:
	for(i=0; i<FILESYSTEM_TABLE_SIZE; i++){
		//copy page i to dataflash buffer 0
		dataflash_copy_page_to_buffer(i, 0);

		//now read 11 filesystem entries:
		for(j=0; j<11; j++){
			//extract id
			tmp  = dataflash_read_buffer(j*24 + FILESYSTEM_FTABLE_ID_HI, 0)<<8;
			tmp |= dataflash_read_buffer(j*24 + FILESYSTEM_FTABLE_ID_LO, 0);

			//only check valid files:
			if (tmp != 0){
				//check if first char equals, if true -> compare the rest
				if (filename[0] == dataflash_read_buffer(j*24+FILESYSTEM_FTABLE_FILENAME_START,0)){
					//yes, first char matches! -> compare
					found = 1;
					//check filename
					for(x=1; x<8; x++){
						//if char does not match -> found = 0	
						if (filename[x] != dataflash_read_buffer(j*24+FILESYSTEM_FTABLE_FILENAME_START + x,0))
							found = 0;
						//end of string -> break
						if (filename[x] == 0)
							break;
					}
					
					//check extension (IS ALWAYS 3 CHARS !!)
					for(x=0; x<3; x++){
						if (filename_ext[x] != dataflash_read_buffer(j*24+FILESYSTEM_FTABLE_FILENAME_START + 8 + x,0))
							found = 0;
					}

					if (found == 1){
						//we got it -> return file id
						#if FILESYSTEM_DEBUG
						softuart_puts_progmem(" found !\r\n");
						#endif
						return tmp;
					}
				}
			}
		}
	}

	//not found -> return 0
	#if FILESYSTEM_DEBUG
	softuart_puts_progmem(" not found !\r\n");
	#endif
	return 0;
}

//erase filesystem  !!! 
//FIXME: use dataflash erase instead !
void filesystem_format(){
	unsigned int i;

	#if FILESYSTEM_DEBUG
	softuart_puts_progmem("FS: formating... please wait (takes a long time) [");
	#endif
	//copy page 0 to dataflash buffer 0
	dataflash_copy_page_to_buffer(0, 0); //??? remove this, not neccessary!

	//clear buffer	
	for(i=0; i<256+8; i++)
			dataflash_write_to_page_buffer(i,0,0);

	//copy buffer to all fs table sectors:
	for(i=0; i<FILESYSTEM_BLOCK_COUNT; i++){
		//store cleared buffer on flash:
		dataflash_copy_buffer_to_page(i,0);
		#if FILESYSTEM_DEBUG
		softuart_putc('.');
		#endif
	}
	#if FILESYSTEM_DEBUG
	softuart_puts_progmem("] done");
	softuart_putnewline();
	#endif
	
}

//list files in filesystem:
void filesystem_list_files(){
	unsigned int tmp,tmp2;
	unsigned char i,j,k;

	#if FILESYSTEM_DEBUG
	softuart_putnewline();
	softuart_puts_progmem(" <fid>  <start>  <size>    <filename>");
	softuart_putnewline();
  softuart_puts_progmem("---------------------------------------");
	softuart_putnewline();	
	#endif

	//search all filesystem table entries:
	for(i=0; i<FILESYSTEM_TABLE_SIZE; i++){
		//copy page i to dataflash buffer 0
		dataflash_copy_page_to_buffer(i, 0);

		//now read 11 filesystem entries:
		for(j=0; j<11; j++){
			//check if there is already a file with our id:
			tmp  = dataflash_read_buffer(j*24 + FILESYSTEM_FTABLE_ID_HI, 0)<<8;
			tmp |= dataflash_read_buffer(j*24 + FILESYSTEM_FTABLE_ID_LO, 0);

			if (tmp != 0){
				//print all:
				//for(tmp2=0; tmp2<24; tmp2++)
				//	rprintf("%x ",dataflash_read_buffer(j*24+tmp2,0));
				//rprintf("\r\n");

				//this is a active file -> print
				#if FILESYSTEM_DEBUG
				softuart_putc(' ');
				softuart_put_uint16(tmp);

				//print start/end sector:
				tmp  = dataflash_read_buffer(j*24 + FILESYSTEM_FTABLE_BLOCK_START_HI, 0);
				tmp  = tmp<<8;
				tmp += dataflash_read_buffer(j*24 + FILESYSTEM_FTABLE_BLOCK_START_LO, 0);
				tmp2  = dataflash_read_buffer(j*24 + FILESYSTEM_FTABLE_BLOCK_COUNT_HI, 0);
				tmp2  = tmp2<<8;
				tmp2 += dataflash_read_buffer(j*24 + FILESYSTEM_FTABLE_BLOCK_COUNT_LO, 0);

				softuart_putc(' ');
				softuart_putc(' ');
				softuart_put_uint16(tmp);
				softuart_puts_progmem("  / ");
				softuart_put_uint16(tmp2);
				
				softuart_puts_progmem("   ./");

				unsigned char c;
				for (k=0; k<8; k++){
					c = dataflash_read_buffer(j*24 + FILESYSTEM_FTABLE_FILENAME_START + k, 0);
					if (c)
						softuart_putc(c);
					else
						break;
				}
				softuart_putc('.');
				for (k=0; k<3; k++){
					c = dataflash_read_buffer(j*24 + FILESYSTEM_FTABLE_FILENAME_START + 8 +k, 0);
					if (c)
						softuart_putc(c);
					else
						break;
				}
				softuart_putnewline();
				#endif

			}
		}
	}
	#if FILESYSTEM_DEBUG
	softuart_putnewline();
	softuart_putnewline();
	#endif
}

unsigned char filesystem_filename_to_buffer(unsigned char *buffer, unsigned int fnpos){
	unsigned int pos = 0;
	unsigned char k;

	//add filename
	for (k=0; k<8; k++){
		buffer[pos] = dataflash_read_buffer(fnpos*24 + FILESYSTEM_FTABLE_FILENAME_START + k, 0);
		
		if (buffer[pos] == 0)
			break;
		
		pos++;
	}

	//add '.'
	buffer[pos++] = '.';

	for (k=0; k<3; k++){
		buffer[pos] = dataflash_read_buffer(fnpos*24 + FILESYSTEM_FTABLE_FILENAME_START + 8 + k, 0);
		
		if (buffer[pos] == 0)
			break;
		
		pos++;
	}
	return pos;
}


//generate filesystem index
unsigned int filesystem_generate_index(unsigned char *buffer, unsigned int datapos, unsigned int *len, unsigned int *offset){
	unsigned int i;
	unsigned char j,k;
	unsigned int tmp;
	unsigned int id;

	//every entry looks like:
	//IIIII_SSSSS_<a href="/fs/FFFFFFFF.EEE">FFFFFFFF.EEE</a><br>
	//-> 5+1+5+1+8+1+3+4 +25 +6= 28+25+6 bytes
	//unsigned int  skip_files = 0;
	//while((*offset) >= 59){
	//	(*offset) = (*offset) - 59;
	//	skip_files++;
	//}

	//file entry buffer:
	unsigned char list_entry[60];
	unsigned char list_pos = 0;

	//search all filesystem table entries:
	//-> slow !! we always walk through the whole list!! -> FIXME
	for(i=0; i<FILESYSTEM_TABLE_SIZE; i++){
		//copy page i to dataflash buffer 0
		dataflash_copy_page_to_buffer(i, 0);

		//now read 11 filesystem entries:
		for(j=0; j<11; j++){
			//check if there is already a file with our id:
			id  = dataflash_read_buffer(j*24 + FILESYSTEM_FTABLE_ID_HI, 0)<<8;
			id |= dataflash_read_buffer(j*24 + FILESYSTEM_FTABLE_ID_LO, 0);

			if (id != 0x0000){
				//if (skip_files == 0){
					#if FILESYSTEM_DEBUG
					softuart_puts_progmem("FS  : found id ");
					softuart_put_uint16(id);
					#endif
					//we expect this file -> print info!

					//intitialise entry buffer pos
					list_pos = 0;

					//add file id
					//string_uint16_to_buffer(&list_entry[list_pos], id);
					//list_pos += 5;
					string_progmem_to_buffer(PSTR("<img src=\"/site/ficon.gif\" align=middle> "), &list_entry[list_pos], 41);
					list_pos += 41;

					//add <a href="/fs/
					string_progmem_to_buffer(PSTR("<a href=\"/fs/"), &list_entry[list_pos], 13);
					list_pos += 13;

					//now copy requested data from list_entry buffer to 
					//tcp buffer:
					//if (list_pos>=60) softuart_puts_progmem("FS: OVERFLOW: ");softuart_put_uint8(list_pos);
					for(k=0; k<list_pos; k++){
						if ((*offset) > 0){
							(*offset)--;
						}else{
							buffer[datapos++] = list_entry[k];
							(*len)--;
							if ((*len) == 0)
								return 0;
						}
					}
					list_pos = 0;

					//add filename
					list_pos += filesystem_filename_to_buffer(&list_entry[list_pos], j);

					//add ">
					string_progmem_to_buffer(PSTR("\">"), &list_entry[list_pos], 2);
					list_pos += 2;

					//add filename again
					list_pos += filesystem_filename_to_buffer(&list_entry[list_pos], j);

					//add </a><br>
 					string_progmem_to_buffer(PSTR("</a>"), &list_entry[list_pos], 4);
					list_pos += 4;

					//add ' '
					list_entry[list_pos++] = ' ';
					
					tmp  = dataflash_read_buffer(j*24 + FILESYSTEM_FTABLE_BLOCK_COUNT_HI, 0)<<8;
					tmp += dataflash_read_buffer(j*24 + FILESYSTEM_FTABLE_BLOCK_COUNT_LO, 0);

					//convert to kbyte
					tmp  = tmp / 4; //*256/1024 = /4
					if (tmp == 0)
						tmp = 1;
					//add file size
					string_uint16_to_buffer(&list_entry[list_pos], tmp); 
					list_pos += 5;
				
					//add ' '
					string_progmem_to_buffer(PSTR("KB "), &list_entry[list_pos], 3);
					list_pos += 3;

					//now copy requested data from list_entry buffer to 
					//tcp buffer:
					//if (list_pos>=60) softuart_puts_progmem("FS: OVERFLOW: ");softuart_put_uint8(list_pos);
					for(k=0; k<list_pos; k++){
						if ((*offset) > 0){
							(*offset)--;
						}else{
							buffer[datapos++] = list_entry[k];
							(*len)--;
							if ((*len) == 0)
								return 0;
						}
					}

					//now add next data, reuse buffer, we do not want to use another 59byte buf !
 					list_pos = 0;

					//add a href part of remove file link:
					string_progmem_to_buffer(PSTR(" <a href=\"/adm/rm/"), &list_entry[list_pos], 18);
					list_pos += 18;

					//add filename
					//list_pos += filesystem_filename_to_buffer(&list_entry[list_pos], j);
					//add file id:
					string_uint16_to_buffer(&list_entry[list_pos], id);
					list_pos += 5;

					//add rest of href:
					string_progmem_to_buffer(PSTR("\">remove</a><br>"), &list_entry[list_pos], 16);
					list_pos += 16;

					//now copy requested data from list_entry buffer to 
					//tcp buffer:
					//if (list_pos>=60) softuart_puts_progmem("FS: OVERFLOW: ");softuart_put_uint8(list_pos);
					for(k=0; k<list_pos; k++){
						if ((*offset) > 0){
							(*offset)--;
						}else{
							buffer[datapos++] = list_entry[k];
							(*len)--;
							if ((*len) == 0)
								return 0;
						}
					}

					#if FILESYSTEM_DEBUG
					softuart_puts_progmem(" data created.\r\n");
					#endif
				//}else{
				//	skip_files--;
				//}
			}
		}
	}
	return datapos;
}

//read next byte to data ptr !
//return: 0 if eof file
//        1 otherwise
unsigned char filesystem_get_next_byte(unsigned char *data){
	//unsigned int tmp;

	//copy flash to buffer
	if (filesystem_file_pos == 0){
		dataflash_copy_page_to_buffer(filesystem_current_block, 0);
		for(unsigned char p=0; p<255; p++)
			filesystem_buffer[p] = dataflash_read_buffer(p,0);
	}

	*data = filesystem_buffer[filesystem_file_pos];
	filesystem_file_pos++;

	//block end or end of file ?!
	if ((filesystem_file_pos == 255)){
		filesystem_file_pos = 0;

		//load next block:		
		filesystem_current_block  = dataflash_read_buffer(FILESYSTEM_FILE_NEXT_HI, 0)<<8;
		filesystem_current_block |= dataflash_read_buffer(FILESYSTEM_FILE_NEXT_LO, 0);

		#if FILESYSTEM_DEBUG
		//softuart_puts_progmem("FS  : next block read: ");
		//softuart_put_uint16(filesystem_current_block);
		//softuart_putnewline();
		#endif
		if (filesystem_current_block == 0x0000)
			return 0; //eof file
	}
	return 1;
}


unsigned int filesystem_get_data_startat(unsigned char *buffer, unsigned int datapos, unsigned int len, unsigned int *data_ptr, unsigned int inside_block_pos){
// 	sunsigned int tmp;
	unsigned int count;
	//unsigned int pos;
	unsigned int i;
	while (*data_ptr != 0x0000){
		//copy block to buffer0:
		dataflash_copy_page_to_buffer(*data_ptr, 0);

		//get bytes used inside this block:
		count = dataflash_read_buffer(FILESYSTEM_FILE_BLOCK_USAGE,0)+1;

		//dump block:
		i=0;

		//calc first startbyte:
		if (inside_block_pos != 0){
			count = count - inside_block_pos;
			i = inside_block_pos;
			inside_block_pos = 0;
		}

		if ((count == 256) && (i == 0) && (len >=256)){
			dataflash_read_n_to_buffer(&buffer[datapos], 0);
			datapos += 256;
			len -= 256;
			//i+=...
			if (len == 0)
				return 0;
		}else{
			while (count > 0){
				buffer[datapos++] = dataflash_read_buffer(i,0);
				len--;
				//no more data allowed
				if (len == 0)
					return 0;
				count--;
				i++;
			}
		}
		//calc next block:
		*data_ptr  = dataflash_read_buffer(FILESYSTEM_FILE_NEXT_HI, 0)<<8;
		*data_ptr  = (*data_ptr) | dataflash_read_buffer(FILESYSTEM_FILE_NEXT_LO, 0);
		
		//tmp should be zero if this is the last block. but we have a flag for last block, too:
		if (dataflash_read_buffer(FILESYSTEM_FILE_BLOCKTAG,0) == 0xEE)
			*data_ptr = 0x0000; //last block!
	}
	return datapos;
}










unsigned int filesystem_get_data(unsigned char *buffer, unsigned int datapos, unsigned int len, unsigned long streampos, unsigned int fileid, unsigned int *data_ptr){
	//file was requested, search file:
	//unsigned int tmp;
	unsigned int count;
	unsigned int pos;


	//invalid fileid ?
	if (fileid == 0){
		#if FILESYSTEM_DEBUG
		softuart_puts_progmem("FS  : file not found.");
		softuart_putnewline();
		#endif
		return datapos;
	}

	//copy lookuptable page to dataflash buffer 0
	dataflash_copy_page_to_buffer(((fileid-1)/11), 0);
	
	//pos inside page is ?
	pos = 24*((fileid-1) % 11);

	#if FILESYSTEM_DEBUG
	softuart_puts_progmem("FS  : requested fid ");
	softuart_put_uint16(fileid);
	softuart_puts_progmem(", loc on block ");
	softuart_put_uint16((fileid-1)/11);
	softuart_puts_progmem(", pos ");
	softuart_put_uint8(pos);
	softuart_putnewline();
	#endif

	//get first block:
	*data_ptr  = dataflash_read_buffer(pos + FILESYSTEM_FTABLE_BLOCK_START_HI, 0)<<8;
	*data_ptr  = (*data_ptr) | dataflash_read_buffer(pos + FILESYSTEM_FTABLE_BLOCK_START_LO, 0);

	if (*data_ptr == 0x0000){
		buffer[datapos++] = '4';
		buffer[datapos++] = '0';
		buffer[datapos++] = '4';
		return datapos;
	}

	while (*data_ptr != 0x0000){
		//copy block to buffer0:
		dataflash_copy_page_to_buffer(*data_ptr, 0);

		//is this block requested ?
		if (streampos < 255){
			//get bytes used inside this block:
			count = dataflash_read_buffer(FILESYSTEM_FILE_BLOCK_USAGE,0)+1;
	
			//dump block:
			unsigned int i=0;

			//calc first startbyte:
			if (streampos != 0){
				count = count - streampos;
				i += streampos;
				streampos = 0;
			}
			
			while (count > 0){
				//if (streampos > 0){
				//	//FIXME: could be done muuuch faster (calculate skip count etc...) FIXME
				//	streampos--;
				//}else{
					buffer[datapos++] = dataflash_read_buffer(i,0);
					len--;
					//no more data allowed
					if (len == 0)
						return 0;
				//}
				count--;
				i++;
			}
		}else{
			//we skip this block -> decrease streampos:
			streampos -= 256;
		}

		//calc next block:
		*data_ptr  = dataflash_read_buffer(FILESYSTEM_FILE_NEXT_HI, 0)<<8;
		*data_ptr  = (*data_ptr) | dataflash_read_buffer(FILESYSTEM_FILE_NEXT_LO, 0);
		
		//tmp should be zero if this is the last block. but we have a flag for last block, too:
		if (dataflash_read_buffer(FILESYSTEM_FILE_BLOCKTAG,0) == 0xEE)
			*data_ptr = 0x0000; //last block!
	}
	#if FILESYSTEM_DEBUG
	softuart_puts_progmem("FS  : file sent.");
	softuart_putnewline();
	#endif


	return datapos;
}

//search file table for the next free file id:
unsigned int filesystem_allocate_fileid(){
	unsigned int id;
	unsigned int tmp;
	unsigned char i,j;

	//start with invalid file id 
	id = 0;

	#if FILESYSTEM_DEBUG
	softuart_puts_progmem("FS: searching free id...");
	#endif

	//search all filesystem table entries:
	for(i=0; i<FILESYSTEM_TABLE_SIZE; i++){
		//copy page i to dataflash buffer 0
		dataflash_copy_page_to_buffer(i, 0);

		//now read 11 filesystem entries:
		for(j=0; j<11; j++){
			//check if there is already a file with our id:
			tmp  = dataflash_read_buffer(j*24 + FILESYSTEM_FTABLE_ID_HI, 0)<<8;
			tmp |= dataflash_read_buffer(j*24 + FILESYSTEM_FTABLE_ID_LO, 0);
			
			//if current file has an id of 0 -> this slot is empty -> use this id !
			if (tmp == 0){
				id = 1 + i*11 + j; //fileid valid from 1...50*11-1 = 1...549
				#if FILESYSTEM_DEBUG
				softuart_puts_progmem(" found id ");
				softuart_put_uint16(id);
				softuart_putnewline();
				#endif
				return id;
			}
		}	
	}

	#if FILESYSTEM_DEBUG
	softuart_puts_progmem(" failed. FS FULL!");
	softuart_putnewline();
	#endif
	return 0; //FAILED
}

//close file & story lookup info
void filesystem_close_file(){
	unsigned int pos;
	unsigned char i;

	#if FILESYSTEM_DEBUG
	softuart_puts_progmem("FS  : closing file ");
	softuart_put_uint16(filesystem_fileid);
	softuart_putnewline();
	#endif

	//check for invalid fileid:
	if (filesystem_fileid == 0)
		return;

	//store active buffer:
	dataflash_write_n_to_page_buffer(0,0,filesystem_buffer, filesystem_file_pos);
	filesystem_store_current_block(0);

	//store file info in lookup table!
	
	//copy lookuptable page to dataflash buffer 0
	dataflash_copy_page_to_buffer(((filesystem_fileid-1)/11), 0);
	
	//pos inside page is ?
	pos = 24*((filesystem_fileid-1) % 11);

	//step1: write file id:
	dataflash_write_to_page_buffer(pos + FILESYSTEM_FTABLE_ID_HI, 0, (filesystem_fileid>>8)&0xFF);
	dataflash_write_to_page_buffer(pos + FILESYSTEM_FTABLE_ID_LO, 0, (filesystem_fileid   )&0xFF);
	
	//step2: write timestamp
	///NOT SUPPORTED RIGHT NOW///
	
	//step3: write filename + ext:
	for(i=0; i<8; i++)
		dataflash_write_to_page_buffer(pos + FILESYSTEM_FTABLE_FILENAME_START + i, 0, filesystem_filename[i]);
	for(i=0; i<3; i++)
		dataflash_write_to_page_buffer(pos + FILESYSTEM_FTABLE_FILENAME_START + i + 8, 0, filesystem_filename_ext[i]);

	//step4: write start block address:
	dataflash_write_to_page_buffer(pos + FILESYSTEM_FTABLE_BLOCK_START_HI, 0, (filesystem_first_block>>8)&0xFF);
	dataflash_write_to_page_buffer(pos + FILESYSTEM_FTABLE_BLOCK_START_LO, 0, (filesystem_first_block   )&0xFF);

	//step5: write block count:
	dataflash_write_to_page_buffer(pos + FILESYSTEM_FTABLE_BLOCK_COUNT_HI, 0, (filesystem_block_count>>8)&0xFF);
	dataflash_write_to_page_buffer(pos + FILESYSTEM_FTABLE_BLOCK_COUNT_LO, 0, (filesystem_block_count   )&0xFF);

	//store data on flash:
	dataflash_copy_buffer_to_page((filesystem_fileid-1)/11,0);

	//reset vars:
	filesystem_status = FILESYSTEM_STATUS_IDLE;
	filesystem_fileid = 0;
	filesystem_first_block = 0;
	filesystem_current_block = 0;
	filesystem_file_pos = 0;
}


//alocate 264 byte block & store file info header inside:
unsigned int filesystem_allocate_block(){
	//search block
	#if FILESYSTEM_DEBUG
	softuart_puts_progmem("FS  : searching free block...");
	#endif
	for(unsigned int b=FILESYSTEM_TABLE_SIZE; b<FILESYSTEM_BLOCK_COUNT; b++){
		//copy selected page to buffer
		dataflash_copy_page_to_buffer(b, 1);
	
		//rprintf("FS: checking block: [%d].\r\n",b);
		//is it free ? 
		//-> ignore b = current block !
		if ((dataflash_read_buffer(FILESYSTEM_FILE_FREETAG,1) != 0xE0) && (b != filesystem_current_block)){
			//-> free !
			#if FILESYSTEM_DEBUG
			softuart_puts_progmem(" found block ");
			softuart_put_uint16(b);
			softuart_putnewline();
			#endif
			return b;
		}
	}
	//no free block found :(
	#if FILESYSTEM_DEBUG
	softuart_puts_progmem(" FAILED -> FS FULL ?!");
	softuart_putnewline();
	#endif
	return 0;
}

//open a new file:
unsigned int filesystem_open_file_wr(unsigned char *filename, unsigned char *filename_ext){
	//make sure there is no open file !
	if (filesystem_status != FILESYSTEM_STATUS_IDLE)
		filesystem_close_file();
	
	//make file extension lowercase !!
	for(unsigned char i=0; i<3 && filename_ext[i] != 0; i++){
		if ((filename_ext[i] >= 'A') && (filename_ext[i] <= 'Z'))
			filename_ext[i] = filename_ext[i] - 'A' + 'a';
	}

	//protect filename, only allow a-zA-Z0-9_-
	for(unsigned char i=0; i<8; i++){
		if (filename[i] == 0)
			break;
		if (
					!((filename[i] >= 'a') && (filename[i] <= 'z')) &&
					!((filename[i] >= 'A') && (filename[i] <= 'Z')) &&
					!((filename[i] >= '0') && (filename[i] <= '9')) 
				){
			filename[i] = '_';
		}
	}

	#if FILESYSTEM_DEBUG
	softuart_puts_progmem("FS  : new file [");
	for(unsigned char i=0; i<8 && filename[i] != 0; i++)
		softuart_putc(filename[i]);
	softuart_putc('.');
	for(unsigned char i=0; i<3 && filename_ext[i] != 0; i++)
		softuart_putc(filename_ext[i]);
	softuart_putc(']');
	softuart_putnewline();
	#endif

	//check if file exists, returns 0 if file is unknown!
	filesystem_fileid = filesystem_search_file(filename, filename_ext);

	if (filesystem_fileid != 0){
		//file exists !!!
		//->delete file & replace it with the new data:
		#if FILESYSTEM_DEBUG
		softuart_puts_progmem("FS  : WARNING: overwriting existing file !");
		softuart_putnewline();
		#endif
		filesystem_delete_file(filesystem_fileid);
	}else{
		//new file -> get id:
		filesystem_fileid = filesystem_allocate_fileid();
	}

	filesystem_block_count = 1;

	//check for error:
	if (filesystem_fileid  == 0)
		return 0;

	//now search a free block adddress for the first block:
	unsigned int block = filesystem_allocate_block();

	//check for error:
	if (block == 0)
		return 0;

	//store info:
	filesystem_first_block = block;
	filesystem_current_block = block;
	for (unsigned char i=0; i<8; i++)
		filesystem_filename[i] = filename[i];
	for (unsigned char i=0; i<3; i++)
		filesystem_filename_ext[i] = filename_ext[i];

	//return first block:
	return block;
}

/*unsigned int filesystem_open_file_rd(unsigned char *filename, unsigned char *filename_ext){
	unsigned int pos;

	//make sure there is no open file !
	if (filesystem_status != FILESYSTEM_STATUS_IDLE)
		filesystem_close_file();
	
	//check if file exists, returns 0 if file is unknown!
	filesystem_fileid = filesystem_search_file(filename, filename_ext);

	if (filesystem_fileid != 0){
		//file exists !!!
		//->delete file & replace it with the new data:
		#if FILESYSTEM_DEBUG
		softuart_puts_progmem("FS  : open file for reading!");
		softuart_putnewline();
		#endif
	}else{
		//new file -> get id:
		#if FILESYSTEM_DEBUG
		softuart_puts_progmem("FS  : file not found!");
		softuart_putnewline();
		#endif
		return 0;
	}

	//pos inside page is ?
	pos = 24*((filesystem_fileid-1) % 11);

	#if FILESYSTEM_DEBUG
	softuart_puts_progmem("FS  : requested fid ");
	softuart_put_uint16(filesystem_fileid);
	softuart_puts_progmem(", loc on block ");
	softuart_put_uint16((filesystem_fileid-1)/11);
	softuart_puts_progmem(", pos ");
	softuart_put_uint8(pos);
	softuart_putnewline();
	#endif

	//get first block:
	filesystem_current_block  = dataflash_read_buffer(pos + FILESYSTEM_FTABLE_BLOCK_START_HI, 0)<<8;
	filesystem_current_block |= dataflash_read_buffer(pos + FILESYSTEM_FTABLE_BLOCK_START_LO, 0);

	filesystem_first_block = filesystem_current_block;
	
	for (unsigned char i=0; i<8; i++)
		filesystem_filename[i] = filename[i];
	for (unsigned char i=0; i<3; i++)
		filesystem_filename_ext[i] = filename_ext[i];

	filesystem_file_pos = 0;

	//return OK
	return 1;
}*/


//store len bytes of data in current open file. if last==1 close file after write:
void filesystem_write_data(unsigned char* data, unsigned int len, unsigned char last){
	//unsigned int next_block;
	//rprintf("FS: writing %d chars.\r\n",len);

	//check for invalid fileid:
	if (filesystem_fileid == 0)
		return;

	for( ; len>0; len--){
		//save data:
		///dataflash_write_to_page_buffer((unsigned int)filesystem_file_pos, 0, (*data));
		filesystem_buffer[filesystem_file_pos] = (*data);
		//rprintf("FS: saving %c\r\n",(*data));

		//block full or end of file ?!
		if ((filesystem_file_pos == 255) || (len==1 && last==1)){
			//step0: copy buffer to flash:
			dataflash_write_n_to_page_buffer(0,0,filesystem_buffer, 256);

			//store buffer:
			filesystem_store_current_block((filesystem_file_pos == 255) && !(len==1 && last==1));

			#if FILESYSTEM_DEBUG
			softuart_puts_progmem("FS  : storing block ");
			softuart_put_uint16(filesystem_current_block);
			softuart_puts_progmem(" on flash");
			softuart_putnewline();
			#endif

			//filesystem_file_pos = 0xFF; //next is filepos 0
		}

		*data++;
		filesystem_file_pos++;
		
	}
}

void filesystem_store_current_block(unsigned char not_last){
	unsigned int next_block;

	//step1: mark current block as used:
	dataflash_write_to_page_buffer(FILESYSTEM_FILE_FREETAG, 0, 0xE0);
	
	//step2: set filsystem rev
	dataflash_write_to_page_buffer(FILESYSTEM_FILE_REV, 0, FILESYSTEM_REVISION);

	//step3: save file id
	dataflash_write_to_page_buffer(FILESYSTEM_FILE_ID_LO, 0, filesystem_fileid&0xFF);
	dataflash_write_to_page_buffer(FILESYSTEM_FILE_ID_HI, 0, (filesystem_fileid>>8)&0xFF);

	//step4: save block info (first/last block)
	if (filesystem_first_block == filesystem_current_block)
		dataflash_write_to_page_buffer(FILESYSTEM_FILE_BLOCKTAG, 0, 0x00);
	else if (!not_last)
		dataflash_write_to_page_buffer(FILESYSTEM_FILE_BLOCKTAG, 0, 0xEE);
	else
		dataflash_write_to_page_buffer(FILESYSTEM_FILE_BLOCKTAG, 0, 0xFF);

	//step 5: save block usage
	dataflash_write_to_page_buffer(FILESYSTEM_FILE_BLOCK_USAGE, 0, filesystem_file_pos);

	//check if it is the last block
	if (not_last){
		//not the last -> get next free block 

		//need to allocate next block!
		next_block = filesystem_allocate_block();
		filesystem_block_count++;

		//step6: save next block pointer:
		dataflash_write_to_page_buffer(FILESYSTEM_FILE_NEXT_LO, 0, next_block&0xFF);
		dataflash_write_to_page_buffer(FILESYSTEM_FILE_NEXT_HI, 0, (next_block>>8)&0xFF);

		//store in flash:
		dataflash_copy_buffer_to_page(filesystem_current_block,0);

		//no more free blocks -> exit  ///FIXME///
		if (next_block == 0){
			#if FILESYSTEM_DEBUG
			softuart_puts_progmem("FS  : no more blocks ?!");
			softuart_putnewline();
			#endif
			return;
		}

		//update block info
		filesystem_current_block = next_block;
	}else{
		//last block -> we do not need a next pointer
		//step6: save next block pointer:
		dataflash_write_to_page_buffer(FILESYSTEM_FILE_NEXT_LO, 0, 0x00);
		dataflash_write_to_page_buffer(FILESYSTEM_FILE_NEXT_HI, 0, 0x00);
		
		dataflash_write_to_page_buffer(FILESYSTEM_FILE_BLOCK_USAGE, 0, filesystem_file_pos-1);

		//store in flash:
		dataflash_copy_buffer_to_page(filesystem_current_block,0);
	}
}

/// DO NOT USE THIS !!!!!!!!
//seek to given byte position!
//file must be open!
/*unsigned char filesystem_seek_to(unsigned int pos){
	unsigned int tmp_block;
	unsigned int tmp_count=0;
	unsigned int fs_first_byte_of_current_block;
	unsigned char last=0;
	
	#if FILESYSTEM_DEBUG
	softuart_puts_progmem("FS  : DO NOT USE SEEK ! NOT YET FINISHED (buggy!!)\r\n");
	#endif

	//check for invalid fileid:
	if (filesystem_fileid == 0)
		return 0;

	fs_first_byte_of_current_block = filesystem_current_block*256;

	if (pos < fs_first_byte_of_current_block+filesystem_file_pos){
		//requested pos is smaller than current pos !
		if (pos > fs_first_byte_of_current_block){
			//but position is inside current active block!
			//-> set fileposition:
			filesystem_file_pos = pos & 0xFF; //= pos%256
		}else{
			//position is inside a smaller block!

			//store current data:
			dataflash_write_n_to_page_buffer(0,0,filesystem_buffer, 256);
			if (filesystem_block_count > filesystem_current_block)
				filesystem_store_current_block(1); //not the last block!
			else
				filesystem_store_current_block(0); //this is the last block!

			//seek from first block:
			tmp_block = filesystem_first_block;
			
			while ((tmp_count < (pos/256)) && (tmp_block != 0x0000)){
				//copy block to buffer0:
				dataflash_copy_page_to_buffer(tmp_block, 0);

				//calc next block:
				tmp_block  = dataflash_read_buffer(FILESYSTEM_FILE_NEXT_HI, 0)<<8;
				tmp_block |= dataflash_read_buffer(FILESYSTEM_FILE_NEXT_LO, 0);
		
				//tmp should be zero if this is the last block. but we have a flag for last block, too:
				if (dataflash_read_buffer(FILESYSTEM_FILE_BLOCKTAG,0) == 0xEE)
					tmp_block = 0x0000; //last block!

				tmp_count++;
			}
			if (tmp_block == 0x0000)
				return 0; //FS ERROR !! there should be no next block pointer with zero next pointer inside file !

			if (tmp_count > filesystem_block_count)
				filesystem_block_count = tmp_count;
			filesystem_current_block = tmp_block;
			filesystem_file_pos      = pos & 0xFF; //= pos%256
			return 1;
		}
	}else{
		//file position is bigger than current filepos
		//search to filepos & append empty blocks if necessary (= when req pos > file size) 
		tmp_block = filesystem_current_block;
		tmp_count = filesystem_block_count;

		//store current data:
		dataflash_write_n_to_page_buffer(0,0,filesystem_buffer, 256);
		filesystem_store_current_block(1); //not the last block!

		while ((tmp_count < (pos/256)) && (tmp_block != 0x0000)){
			//is this block the last ?!
			if (tmp_count + 256 > (pos/256)){
				//this is the last block:
				last = 1;
				//setup file pos:
				filesystem_file_pos = pos&0xFF;
			}else{
				//not the last -> more data follows
				last = 0;
				filesystem_file_pos = 255;
			}

			//copy block to buffer0:
			dataflash_copy_page_to_buffer(tmp_block, 0);

			//calc next block:
			tmp_block  = dataflash_read_buffer(FILESYSTEM_FILE_NEXT_HI, 0)<<8;
			tmp_block |= dataflash_read_buffer(FILESYSTEM_FILE_NEXT_LO, 0);
		
			//tmp should be zero if this is the last block. but we have a flag for last block, too:
			if ((dataflash_read_buffer(FILESYSTEM_FILE_BLOCKTAG,0) == 0xEE) ||	(tmp_block = 0x0000)){
				//this is the last block of the file !	
				//we need to append empty blocks now !
				filesystem_store_current_block(!(last==1));
			}
			tmp_count++;
		}

		//now we are in the right block
		//setup file pos:
		filesystem_file_pos = pos & 0xFF;
		return 1;
	}
	return 0;
}
*/
