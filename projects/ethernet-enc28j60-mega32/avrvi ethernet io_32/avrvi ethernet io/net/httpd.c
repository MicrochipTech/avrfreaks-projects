/*
,-----------------------------------------------------------------------------------------.
| net/httpd
|-----------------------------------------------------------------------------------------
| this file implements a very basic http server
| - support for HTTP POST 
| - support for http basic auuth (all /adm/.. file acces must be authorized)
|
| KNOWN PROBLEMS:
| - somehow quick&dirty implementation ! sorry for that ;)
| - uses some dirty hacks (content length detection etc)
| - very big routines without function calls for memory/flash/speed reasons
| - not really documented :-\
| - http authorization does not work if browser request is split into two packets ! (-> always not authorized)
| - httpd_add_prog* only use 16bit as data offset ! -> biggest filesize in avr flash is 65k!!!
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

#include "httpd.h"

//ACTIVATE DEBUG by editing this file:
#include "../debug.h"

///FIXME: include the .c file here !
///       without this sizeof() does not work !! -> is there another way to do this ?!
#include "httpd_data.c"


struct httpd_conn_struct {
	unsigned char state;
	unsigned char substate;
	unsigned int  file_id;
	unsigned long  seq_offset;
	unsigned int  data_ptr;
	unsigned long data_position;
};

struct httpd_conn_struct httpd_conn[TCP_SOCKET_COUNT];

//initialise connection table
void httpd_init(void){
	for(unsigned char c=0; c<TCP_SOCKET_COUNT; c++){
		httpd_cleanup_conn(c);
	}
}

//initialise/cleanup http conn
void httpd_cleanup_conn(unsigned char i){
	if (i<TCP_SOCKET_COUNT){
		httpd_conn[i].state = HTTPD_STATE_IDLE;
		httpd_conn[i].data_position = 0;
	}
}

//return: how many data bytes should be transferred ?
unsigned int httpd_data_in(unsigned char *buffer, unsigned int datapos, unsigned int datalen, unsigned char socketnum, unsigned char *appstate){
	unsigned int pos;
	unsigned char file_name[8+6+1];
	unsigned char file_ext[3+1];
	unsigned char f;
	unsigned int ret;
	unsigned long offset;
	unsigned char authorized;

	pos = 0;

	#if HTTPD_DEBUG2
	softuart_puts_progmem("HTTP: SEQ-SEQ_OFFSET_DATA = ");
	softuart_put_uint16((tcp_sockets[socketnum].seq-httpd_conn[socketnum].seq_offset)&0xFFFF);
	softuart_puts_progmem("\r\n");
	#endif

  //initialise filename:
	for (f=0; f<8+6+1; f++)
		file_name[f] = 0;
	for (f=0; f<3+1; f++)
		file_ext[f] = 0;

	switch(httpd_conn[socketnum].state){
		case(HTTPD_STATE_IDLE):
		//case(HTTPD_STATE_IDLE_r):
		//case(HTTPD_STATE_IDLE_rn):
		//case(HTTPD_STATE_IDLE_rnr):
		//case(HTTPD_STATE_IDLE_rnrn):
			//new request, scan for GET/POST:
			if ((buffer[datapos + 0] == 'G') && 
			    (buffer[datapos + 1] == 'E') && 
			    (buffer[datapos + 2] == 'T')){
				//get request !
				pos = datapos + 3;
				
				f = 0;

				//find start of requested url
				while(buffer[pos] == ' ')
					pos++;

				while((buffer[pos] != ' ')  && (buffer[pos] != '\r') && (buffer[pos] != '\n')  && (buffer[pos] != '?') && (pos<datapos+datalen)){
					//read requested file,
					//read filename:
					file_name[f] = buffer[pos];

					//read file extension:
					/*if (!((file_ext[2] == ' ') || (file_ext[2] == '?'))){
						file_ext[0] = file_ext[1];
						file_ext[1] = file_ext[2];
						file_ext[2] = buffer[pos];
					}*/
					if (buffer[pos] == '.'){
						file_ext[0] = buffer[pos+1];
						file_ext[1] = buffer[pos+2];
						file_ext[2] = buffer[pos+3];
						file_name[f] = 0;
						break;
					}

					if (file_name[f] == '.')
						file_name[f] = 0;
					
					if (f<8+6)
						f++;
	
					//remove first /
					if (file_name[0] == '/')
						f=0;

					pos++;
				}
				//if (file_name[f]=='.')
				//	file_name[f] = 0;

				#if HTTPD_DEBUG
				softuart_puts_progmem("HTTP: GET <");				
				for(f=0;f<8+6 && file_name[f];f++)
					softuart_putc(file_name[f]);
				softuart_putc('.');
				for(f=0;f<3 && file_ext[f];f++)
					softuart_putc(file_ext[f]);
				softuart_puts_progmem(">.\r\n");
				#endif

				//set file id:
				if (file_name[0] == '/' || string_compare_progmem("index", file_name)){
					///show index
					httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_INDEX;
					file_ext[0] = 'h'; //->html
				}else if ( string_compare_progmem("site/cam", file_name)){
					///show webcam pic
					httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_SITE_CAM;
				}else if ( string_compare_progmem("site/temp", file_name)){
					///show templog site
					httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_SITE_TEMP;
				}else if ( string_compare_progmem("site/io", file_name)){
					///show io site
					httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_SITE_IO;
				}else if ( string_compare_progmem("site/ficon", file_name)){
					///show file icon
					httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_SITE_FILE_ICON_PIC;
				}else if ( string_compare_progmem("graph_0", file_name)){
					///show tempgraph (use eeprom data)
					httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_GRAPH0;
				}else if ( string_compare_progmem("fs/ls", file_name)){
					///show dataflash file list
					httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_FS_LS;
				}else if ( string_compare_progmem("cam/pic", file_name)){
					///show webcam image
					httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_CAM_PIC;
				}else if ( string_compare_progmem("cam/busy", file_name)){
					///show webcam busy image
					httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_CAM_BUSY;
				}else if ( string_compare_progmem("site/servo", file_name)){
					///show servo page
					httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_SERVO_OK;
				}else if ( string_compare_progmem_noeof("servo_", file_name)){
					///show/set servo pos
					//parse number & set servopos (use uint16 parser)
					//if you want to show servopos without moving use servo_999.bmp for example
					if (string_buffer_to_uint16(&file_name[6])<256)
						servo_set_pos(string_buffer_to_uint16(&file_name[6])&0xFF);
					httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_SERVO_POS;
				}else if ( string_compare_progmem_noeof("set/", file_name)){
					///show port image
					if(file_name[7] == '1'){
						httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_SET_IO_1_PIC;
						port_set_portbit(file_name[4], file_name[5], 1);
					}else{
						httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_SET_IO_0_PIC;
						port_set_portbit(file_name[4], file_name[5], 0);
					}
				//deprecated
				//}else if ( string_compare_progmem_noeof("fs/f", file_name)){
				//	///show dataflash file with id given (fs/f1234 -> file 1234 !):
				//	httpd_conn[socketnum].file_id = 0xFF + string_buffer_to_uint16(&file_name[4]);
				}else if ( string_compare_progmem_noeof("fs/", file_name)){
					///try to find dataflash file with the given name:
					httpd_conn[socketnum].file_id = 0xFF + filesystem_search_file(&file_name[3], &file_ext[0]);
					httpd_conn[socketnum].data_ptr = 0; 
					if (httpd_conn[socketnum].file_id == 0xFF)
						httpd_conn[socketnum].file_id = HTTPD_STATE_IDLE; ///no file found !
				}else if ( string_compare_progmem("adm/up", file_name)){
					///show upload form
					httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_UP;
				}else if ( string_compare_progmem_noeof("adm/rm/", file_name)){
					///try to find dataflash file with the given name:
					httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_REMOVED;
				}else if ( string_compare_progmem("adm/mkfs", file_name)){
					///format filesystem question
					httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_MKFS;
				}else if ( string_compare_progmem("adm/mkfs2", file_name)){
					///format filesystem exec!
					httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_MKFS2;
				}else{
					///FILE not found -> 404 err
					httpd_conn[socketnum].file_id = HTTPD_STATE_IDLE;
				}
				//initialise substate
				httpd_conn[socketnum].substate = HTTPD_SUBSTATE_NONE;
			}else if ((buffer[datapos + 0] == 'P') && 
					(buffer[datapos + 1] == 'O') && 
					(buffer[datapos + 2] == 'S') && 
					(buffer[datapos + 3] == 'T')){
				//QUICK HACK (TEST!)
				httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_POST;

				string_progmem_to_buffer(PSTR("adm/"), file_name,4);

				//initialise substate
				httpd_conn[socketnum].substate = HTTPD_SUBSTATE_NONE;
			}

			//find the content length!
			//use a quick & dirty method for this !!!
			//-> we search for *th: <number> 
			//   (instead of content-length:)
			//
			// we do this because to minimize the following problem:
			// clen detection DOES NOT work when the header is 
			// split into multiple packets & the packetborder
			// is anywhere between th: <number> !! FIXME!
			if (httpd_conn[socketnum].file_id == 0xFE){
				pos = datapos;
				//pos+4 because len("th: ") is 4 (fixme: number might be outside packetlen!)
				while (pos+4<datapos+datalen){
					if (string_compare_progmem_noeof("th: ", &buffer[pos])){
						//next is the number ! 
						//parse it !
						httpd_conn[socketnum].data_position = string_buffer_to_uint32(&buffer[pos+4]);
						break;
					}
					pos++;
				}
			}

			///search for authorization key
			authorized = 0;
			if (string_compare_progmem_noeof("adm/", file_name)){
				//accessing admin zone, search for auth!
				#if HTTPD_DEBUG_AUTH
				softuart_puts_progmem("HTTPD: auth required! [");
				#endif
				pos = datapos;
				
				while(pos<datapos+datalen){
					//speedup, check first letter
					if (buffer[pos] == 'A'){
						//if match, call whole function match
						if (string_compare_progmem_noeof("Authorization: Basic ", &buffer[pos])){
							//got it ! now buf[pos+21]-... has auth string!
							unsigned char len;
							unsigned char *pwbuff = &buffer[pos+21];

							//maximum pw len 100
							for(len=0; len < 100; len++){ 
								//check buf < '0' is ok because of base64... \r\n & space are smaller ;)
								if (pwbuff[len]<'0')
									break;
								#if HTTPD_DEBUG_AUTH
								softuart_putc(pwbuff[len]);
								#endif
							}
							//len--;
							
							//base64 decode, after this the decoded string is in buffer[pos+21]...
							base64_decode(&pwbuff[0], len);

							#if HTTPD_DEBUG_AUTH
							softuart_puts_progmem("], decoded [");
							for(len=0; len < 100; len++){
								if (pwbuff[len]==0)	
									break;
								softuart_putc(pwbuff[len]);
							}
							softuart_puts_progmem("] auth? ");
							#endif

							if (string_compare_progmem(HTTPD_ADMIN_AUTH_LOGIN":"HTTPD_ADMIN_AUTH_PASS, &pwbuff[0])){
								//auth passed !!
								authorized = 1;
							}
							#if HTTPD_DEBUG_AUTH
							softuart_put_uint8(authorized);
							softuart_putnewline();
							#endif

							break;
						}
					}
					pos++;
				}
				//check if there was a file remove request
				if (authorized && (httpd_conn[socketnum].file_id == HTTPD_STATE_FILE_REMOVED)){
					httpd_conn[socketnum].file_id = 0xFF + string_buffer_to_uint16(&file_name[7]);
					if (httpd_conn[socketnum].file_id != 0xFF){
						//found ! -> remove file !
						filesystem_delete_file(httpd_conn[socketnum].file_id - 0xFF);
						httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_REMOVED;
					}
				}
			}
			
			//detect \r\n\r\n
			unsigned int inpos = datapos;
			unsigned int inpos_old =0;
			while( inpos<(datapos+datalen)){
				if (buffer[inpos] == '\r'){
					if ( ((httpd_conn[socketnum].substate & 0x07) == HTTPD_SUBSTATE_NONE) || 
							 ((httpd_conn[socketnum].substate & 0x07) == HTTPD_SUBSTATE_RN) ){
							httpd_conn[socketnum].substate++;
					}else{
							httpd_conn[socketnum].substate = (httpd_conn[socketnum].substate & 0xF8) | HTTPD_SUBSTATE_NONE;
					}
				}else if (buffer[inpos] == '\n'){
					if ( ((httpd_conn[socketnum].substate & 0x07) == HTTPD_SUBSTATE_R) || 
							 ((httpd_conn[socketnum].substate & 0x07) == HTTPD_SUBSTATE_RNR) ){
							httpd_conn[socketnum].substate++;
					}else{
							httpd_conn[socketnum].substate = (httpd_conn[socketnum].substate & 0xF8) | HTTPD_SUBSTATE_NONE;
					}
				}else{
					httpd_conn[socketnum].substate = (httpd_conn[socketnum].substate & 0xF8) | HTTPD_SUBSTATE_NONE;
				}
				if ((httpd_conn[socketnum].substate & 0x07) == HTTPD_SUBSTATE_RNRN){
					if (httpd_conn[socketnum].file_id == HTTPD_STATE_FILE_POST){
						//file_name + 2nd rnrn found -> ready
						if (httpd_conn[socketnum].substate & HTTPD_SUBSTATE_POST_FN_OK)
							break;

						inpos_old = inpos;
						//we need to grab filename now & wait for another \r\n\r\n
						while(inpos<(datapos+datalen)){
							///WARNING: this only works if the packet with filename is not
							///splitted into two packets !!
							if (string_compare_progmem_noeof("filename=\"", &buffer[inpos])){
								//now the filename starts at inpos+strlen(filename=")
								
								//try to find the filename extension:
								unsigned int fextpos = inpos + 10;
								while((buffer[fextpos] != '.') &&(inpos<(datapos+datalen)))
									fextpos++;
						
								//terminate filename (replace .EXT by \0EXT)
								buffer[fextpos] = '\0';

								#if HTTPD_DEBUG
								softuart_puts_progmem("HTTP: POST ");
								softuart_putnewline();
								#endif 
								//now open the file
								filesystem_open_file_wr(&buffer[inpos+10],&buffer[fextpos+1]);
								break;
							}
							inpos++;
						}
						//now find the next \r\n\r\n
						httpd_conn[socketnum].substate = HTTPD_SUBSTATE_POST_FN_OK | HTTPD_SUBSTATE_NONE;
					}else{
						//normal GET cmd, return data after \r\n\r\n
						break;
					}
				}
				inpos++;
			}


			///handle http authentication for everything in /adm/:
			//we check this only for the first \r\n\r\n otherwise we get problems
			//with the http post stuff. maybe move the second rnrn detection to a seperate
			//post substate!
			if (!authorized && string_compare_progmem_noeof("adm/", file_name) ){
						//authorization required!
						#if HTTPD_DEBUG_AUTH
						softuart_puts_progmem("HTTPD: auth failed!");
						softuart_putnewline();
						#endif

						//not authorzed, send auth request & failure message
						//when this is an http post, this is not correct...
						//but simply ignore this, no normal user would be unauthorized & try to upload (he cant see up.htm)
						//so this should only happens to a hacker ;)
						pos = TCP_POS_TXDATA;
						pos = httpd_add_progmem_data(HTTPD_RESPONSE_401,NULL, buffer, pos, 0, NIC_BUFFERSIZE-pos);
						httpd_conn[socketnum].state = HTTPD_STATE_FINISHED;
						*appstate = TCP_APPSTATE_HAS_TXDATA;
						return (pos-TCP_POS_TXDATA);
			}
			
			if ((httpd_conn[socketnum].substate & 0x07) == HTTPD_SUBSTATE_RNRN){
				//request end, send data !
				pos = TCP_POS_TXDATA;

				//POST test hack
				if (httpd_conn[socketnum].file_id == HTTPD_STATE_FILE_POST){
					//wait for filename detection!
					if (httpd_conn[socketnum].substate  == HTTPD_SUBSTATE_RNRN)
						break; 

					if (httpd_conn[socketnum].substate & HTTPD_SUBSTATE_POST_FN_OK){
						//filename detected !
					}else{
						#if HTTPD_DEBUG
						softuart_puts_progmem("HTTP: POST (unknown filename)");
						softuart_putnewline();
						#endif
						//open default file
						filesystem_open_file_wr("upload","txt");
					}

					inpos++;
					filesystem_write_data(&buffer[inpos], datalen-(inpos-datapos),0);
					
					httpd_conn[socketnum].state = HTTPD_STATE_POST;
					httpd_conn[socketnum].data_position -= datalen-(inpos_old-datapos)-1;

					//set appstate & exit here !!
					*appstate = TCP_APPSTATE_EMPTYACK;
					return (pos-TCP_POS_TXDATA);
				}else{
					//add ok
					pos = httpd_add_progmem_data(HTTPD_RESPONSE_OK,NULL, buffer, pos, 0, NIC_BUFFERSIZE-pos);
					
					//add content type, detects jpg,png,bmp,gif,html -> default is html!
					//we need to check first char only !!
					switch(file_ext[0]){
						case ('j'): //if (string_compare_progmem_noeof("jpg", file_ext))
							pos = httpd_add_progmem_data(HTTPD_CTYPE_JPG,NULL, buffer, pos, 0, NIC_BUFFERSIZE-pos);
							break;

						case ('p'): //else if (string_compare_progmem_noeof("png", file_ext))
							pos = httpd_add_progmem_data(HTTPD_CTYPE_PNG,NULL, buffer, pos, 0, NIC_BUFFERSIZE-pos);
							break;

						case ('b'): //else if (string_compare_progmem_noeof("bmp", file_ext))
							pos = httpd_add_progmem_data(HTTPD_CTYPE_BMP,NULL, buffer, pos, 0, NIC_BUFFERSIZE-pos);
							break;

						case ('g'): //else if (string_compare_progmem_noeof("gif", file_ext))
							pos = httpd_add_progmem_data(HTTPD_CTYPE_GIF,NULL, buffer, pos, 0, NIC_BUFFERSIZE-pos);
							break;

						default: //htm!
							pos = httpd_add_progmem_data(HTTPD_CTYPE_HTML,NULL, buffer, pos, 0, NIC_BUFFERSIZE-pos);
							break;
					}
				
					//add \r\n\r\n
					pos = httpd_add_progmem_data(HTTPD_RNRN,NULL, buffer, pos, 0, NIC_BUFFERSIZE-pos);
					*appstate = TCP_APPSTATE_HAS_TXDATA;

					//http conn active now -> send data
					httpd_conn[socketnum].state      = HTTPD_STATE_ACTIVE;

					//store seq offset!
					httpd_conn[socketnum].seq_offset = pos - datapos + 1;

					return (pos-TCP_POS_TXDATA);
				}
			}

			*appstate = TCP_APPSTATE_EMPTYACK;
			return (0);
			break;

		case(HTTPD_STATE_ACTIVE):
			#if HTTPD_DEBUG2
			softuart_put_uint16(httpd_conn[socketnum].seq_offset);
			#endif
			offset = (tcp_sockets[socketnum].seq - httpd_conn[socketnum].seq_offset);
			*appstate = TCP_APPSTATE_HAS_TXDATA;

			if (httpd_conn[socketnum].file_id <= 0xFF){
				///this file is not stored on dataflash
				switch ((httpd_conn[socketnum].file_id)&0xFF){
					///index.html
					case(HTTPD_STATE_FILE_INDEX):
						ret = httpd_add_progmem_data(HTTPD_FILE_INDEX_HTML,HTTPD_FILE_SITE_START_HTML,buffer,TCP_POS_TXDATA, offset, NIC_BUFFERSIZE-TCP_POS_TXDATA); 
						break;

/*
					///graph0
					case(HTTPD_STATE_FILE_GRAPH0):
						ret = plot_generate_bmp(buffer, TCP_POS_TXDATA, NIC_BUFFERSIZE-TCP_POS_TXDATA, offset, PLOT_SOURCE_EEPROM);
*/						break;

					///upload test
					case(HTTPD_STATE_FILE_UP):
						ret = httpd_add_progmem_data(HTTPD_FILE_INDEX_HTML,HTTPD_FILE_SITE_UPLOAD_HTML,buffer,TCP_POS_TXDATA, offset, NIC_BUFFERSIZE-TCP_POS_TXDATA);
						break;

					///list	fs files
					case(HTTPD_STATE_FILE_FS_LS):
						ret = httpd_add_progmem_data(HTTPD_FILE_INDEX_HTML,HTTPD_FILE_SITE_FSLS_HTML,buffer,TCP_POS_TXDATA, offset, 	NIC_BUFFERSIZE-TCP_POS_TXDATA);
						break;

					///upload finished, up ok
					case(HTTPD_STATE_FILE_UP_DONE):
						ret = httpd_add_progmem_data(HTTPD_FILE_INDEX_HTML,HTTPD_FILE_SITE_UPOK_HTML,buffer,TCP_POS_TXDATA, offset, 	NIC_BUFFERSIZE-TCP_POS_TXDATA);
						break;

					///upload finished, up err
					case(HTTPD_STATE_FILE_UP_ERR):
						ret = httpd_add_progmem_data(HTTPD_FILE_INDEX_HTML,HTTPD_FILE_SITE_UPERR_HTML,buffer,TCP_POS_TXDATA, offset, 	NIC_BUFFERSIZE-TCP_POS_TXDATA);
						break;

					///removed fs file
					case(HTTPD_STATE_FILE_REMOVED):
						ret = httpd_add_progmem_data(HTTPD_FILE_INDEX_HTML,HTTPD_FILE_SITE_RMOK_HTML,buffer,TCP_POS_TXDATA, offset, 	NIC_BUFFERSIZE-TCP_POS_TXDATA);
						break;

					///format filesystem question
					case(HTTPD_STATE_FILE_MKFS):
						ret = httpd_add_progmem_data(HTTPD_FILE_INDEX_HTML, HTTPD_FILE_SITE_MKFS_HTML,buffer,TCP_POS_TXDATA, offset, 	NIC_BUFFERSIZE-TCP_POS_TXDATA);
						break;

					///do format
					case(HTTPD_STATE_FILE_MKFS2):
						ret = httpd_add_progmem_data(HTTPD_FILE_INDEX_HTML, HTTPD_FILE_SITE_MKFS2_HTML,buffer,TCP_POS_TXDATA, offset, 	NIC_BUFFERSIZE-TCP_POS_TXDATA);
						break;

					///cam/pic (webcam picture)
					case(HTTPD_STATE_FILE_CAM_PIC):
						ret = mca25_grab_data(&buffer[TCP_POS_TXDATA], NIC_BUFFERSIZE-TCP_POS_TXDATA, offset, socketnum);
						if (ret == 0xFFFE){
							//cam is busy -> show busy jpg !
							ret = httpd_add_progmem_data_bin(HTTPD_CAM_BUSY_JPG,buffer,TCP_POS_TXDATA, offset, NIC_BUFFERSIZE-TCP_POS_TXDATA, sizeof(HTTPD_CAM_BUSY_JPG));
							httpd_conn[socketnum].file_id = 6;
						}else if (ret == 0xFFFF){
							ret = 1;
						}
						//add offset
						if (ret != 0)
							ret = TCP_POS_TXDATA + ret;
						
						break;

					///cam/busy  ->  cam busy jpg
					case(HTTPD_STATE_FILE_CAM_BUSY):
						ret = httpd_add_progmem_data_bin(HTTPD_CAM_BUSY_JPG,buffer,TCP_POS_TXDATA, offset, NIC_BUFFERSIZE-TCP_POS_TXDATA, sizeof(HTTPD_CAM_BUSY_JPG));
						break;

					///site/cam  ->  include cam html code
					case(HTTPD_STATE_FILE_SITE_CAM):
						ret = httpd_add_progmem_data(HTTPD_FILE_INDEX_HTML,HTTPD_FILE_SITE_CAM_HTML,buffer,TCP_POS_TXDATA, offset, NIC_BUFFERSIZE-TCP_POS_TXDATA);
						break;

					///site/temp  ->  include templog
					case(HTTPD_STATE_FILE_SITE_TEMP):
						ret = httpd_add_progmem_data(HTTPD_FILE_INDEX_HTML,HTTPD_FILE_SITE_TEMP_HTML,buffer,TCP_POS_TXDATA, offset, NIC_BUFFERSIZE-TCP_POS_TXDATA);
						break;

					///site/io  ->  port io site
					case(HTTPD_STATE_FILE_SITE_IO):
						ret = httpd_add_progmem_data(HTTPD_FILE_INDEX_HTML,HTTPD_FILE_SITE_IO_HTML,buffer,TCP_POS_TXDATA, offset, NIC_BUFFERSIZE-TCP_POS_TXDATA);
						break;

					///set/xx_1 -> show green io pic
					case(HTTPD_STATE_FILE_SET_IO_1_PIC):
						ret = httpd_add_progmem_data_bin(HTTPD_BUTTON_GREEN,buffer,TCP_POS_TXDATA, offset, NIC_BUFFERSIZE-TCP_POS_TXDATA, sizeof(HTTPD_BUTTON_GREEN));
						break;

					///set/xx_0 -> show red io pic
					case(HTTPD_STATE_FILE_SET_IO_0_PIC):
						ret = httpd_add_progmem_data_bin(HTTPD_BUTTON_RED,buffer,TCP_POS_TXDATA, offset, NIC_BUFFERSIZE-TCP_POS_TXDATA, sizeof(HTTPD_BUTTON_RED));
						break;

					///servo moving
					case(HTTPD_STATE_FILE_SERVO_OK):
						ret = httpd_add_progmem_data(HTTPD_FILE_INDEX_HTML,HTTPD_FILE_SITE_SERVO_OK,buffer,TCP_POS_TXDATA, offset, NIC_BUFFERSIZE-TCP_POS_TXDATA);
						break;

					///get/set servo pos
					case(HTTPD_STATE_FILE_SERVO_POS):
						ret = servo_generate_bmp(buffer, TCP_POS_TXDATA, NIC_BUFFERSIZE-TCP_POS_TXDATA, offset, PLOT_SOURCE_EEPROM);
						break;

					///add file icon gif
					case(HTTPD_STATE_FILE_SITE_FILE_ICON_PIC):
						ret = httpd_add_progmem_data_bin(HTTPD_FILE_ICON_GIF,buffer,TCP_POS_TXDATA, offset, NIC_BUFFERSIZE-TCP_POS_TXDATA, sizeof(HTTPD_BUTTON_RED));
						break;

					///404 - not found !
					default:
						#if HTTPD_DEBUG
						softuart_puts_progmem("HTTP: 404 file not found!");
						softuart_putnewline();
						#endif
						ret = httpd_add_progmem_data(HTTPD_FILE_NOT_FOUND,NULL,buffer,TCP_POS_TXDATA, offset, NIC_BUFFERSIZE-TCP_POS_TXDATA);
						break;
				}
			}else{
				///this file is stored on dataflash!
				//-> send data from file x:
				if ((httpd_conn[socketnum].data_position != offset) || (offset == 0)){
					//we must (re)seek to position offset!
					ret = filesystem_get_data(buffer, TCP_POS_TXDATA, NIC_BUFFERSIZE-TCP_POS_TXDATA, offset, (httpd_conn[socketnum].file_id - 0xFF),&httpd_conn[socketnum].data_ptr);
					//this is wrong if this is the last packet, 
					//but it does not matter, after the last packet we do not need datapos any more
					httpd_conn[socketnum].data_position = offset + (NIC_BUFFERSIZE-TCP_POS_TXDATA); 
				}else{
					//data_ptr -> flash page
					//data_position % 256 -> position inside page
					unsigned char indpos = httpd_conn[socketnum].data_position & 0xFF; // %256; 
					ret = filesystem_get_data_startat(buffer, TCP_POS_TXDATA, NIC_BUFFERSIZE-TCP_POS_TXDATA, &httpd_conn[socketnum].data_ptr, indpos);
					//this is wrong if this is the last packet, 
					//but it does not matter, after the last packet we do not need datapos any more
					httpd_conn[socketnum].data_position += (NIC_BUFFERSIZE-TCP_POS_TXDATA);
				}
				
			}

			#if HTTPD_DEBUG2
			softuart_puts_progmem("\r\nret=");
			softuart_put_uint8(ret);
			#endif
			if (ret != 0){
				httpd_conn[socketnum].state = HTTPD_STATE_FINISHED;
				return ret-TCP_POS_TXDATA;
			}

			return NIC_BUFFERSIZE-TCP_POS_TXDATA;
			break;

		case(HTTPD_STATE_FINISHED):
			//if this was an filesystem format request, do it !
			//takes a very long time !!!
			if (httpd_conn[socketnum].file_id == HTTPD_STATE_FILE_MKFS2){
				LED_STATUS_OFF();
				filesystem_format();
				LED_STATUS_ON();
			}

			httpd_conn[socketnum].state = HTTPD_STATE_IDLE;
			*appstate = TCP_APPSTATE_FINISHED;
			return 0;

		case(HTTPD_STATE_POST):
			#if HTTPD_DEBUG
			softuart_puts_progmem("HTTP: upload todo (low 16bit only): ");
			softuart_put_uint16(httpd_conn[socketnum].data_position & 0xFFFF);
			softuart_puts_progmem(" datalen=");
			softuart_put_uint16(datalen&0xFFFF);
			softuart_putnewline();
			#endif
			if (httpd_conn[socketnum].data_position <= datalen){
				//find termination:
				unsigned int endpos;
				for(endpos=TCP_POS_TXDATA; endpos<TCP_POS_TXDATA+datalen; endpos++){
					//quick hack, might return false positives !
					if (string_compare_progmem_noeof("---------------", &buffer[endpos])){
						endpos-=2;
						break;
					}
				}
			
				filesystem_write_data(&buffer[TCP_POS_TXDATA], (endpos-TCP_POS_TXDATA),0);

				filesystem_close_file();
				//pos = httpd_add_progmem_data(HTTPD_CTYPE_HTML,NULL, buffer, pos, 0, NIC_BUFFERSIZE-pos);
				//pos = httpd_add_progmem_data(HTTPD_RNRN,NULL, buffer, pos, 0, NIC_BUFFERSIZE-pos);
				//pos = httpd_add_progmem_data(PSTR("HTTP/1.0 200 Document follows\r\nContent-Type: text/html\r\n\r\n<html><body>upload done.<br><a href=\"/fs/ls\">-> file list</a></body></html>"),NULL, buffer, TCP_POS_TXDATA, 0, NIC_BUFFERSIZE-pos);
				//httpd_conn[socketnum].state = 123;
				pos = httpd_add_progmem_data(PSTR("HTTP/1.0 200 Document follows\r\nContent-Type: text/html\r\n\r\n"),NULL, buffer, TCP_POS_TXDATA, 0, NIC_BUFFERSIZE-pos);
				httpd_conn[socketnum].state = HTTPD_STATE_ACTIVE;

				//check if upload successfull
				if (httpd_conn[socketnum].data_position == datalen)
					httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_UP_DONE;
				else
					httpd_conn[socketnum].file_id = HTTPD_STATE_FILE_UP_ERR;

				httpd_conn[socketnum].seq_offset = pos - datapos + 1;
				
				*appstate = TCP_APPSTATE_HAS_TXDATA;

				
				return (pos-TCP_POS_TXDATA);
			}else{
				httpd_conn[socketnum].data_position -= datalen;
				//filesystem_seek_to(tcp_sockets[socketnum].ack);
				filesystem_write_data(&buffer[TCP_POS_TXDATA], datalen,0);

				*appstate = TCP_APPSTATE_EMPTYACK;
			}

			return 0;
	
		case(123):
			*appstate = TCP_APPSTATE_FINISHED;
			return 0;
			break;

	/*	case(4):
		case(5):

				pos = TCP_POS_TXDATA;
				buffer[pos++] = '.';
				*appstate = TCP_APPSTATE_HAS_TXDATA;
				
				

				tcp_sockets[socketnum].misc_state = tcp_sockets[socketnum].misc_state+1;
				return (pos-TCP_POS_TXDATA);

		case(6):
			//finished...
			*appstate = TCP_APPSTATE_FINISHED;
			tcp_sockets[socketnum].misc_state = 0;
			return 0;
	*/
		default:
			*appstate = TCP_APPSTATE_EMPTYACK;
	}

	return 0;
}

//add program data to buffer
//replaces $$ by data pointed to by include ptr,
//while parsing <include> replace $$.. by special data!
//WARNING: very big !! -> rewrite later !
unsigned int httpd_add_progmem_data(PGM_P pointer, PGM_P include,unsigned char *buffer, unsigned int pos, unsigned int offset, unsigned int len){
	unsigned char data;
	unsigned char data_next;
	unsigned char smallbuffer[10];
	unsigned char data_available;
	PGM_P data_ptr;
	PGM_P data_ptr_old;
	unsigned char skip_next_data_byte;

	//start with main data pointer:
	data_ptr       = pointer;
	data_ptr_old   = NULL;
	data_available = 1;
	data           = pgm_read_byte(data_ptr++);
	skip_next_data_byte = 0;

	while (data_available != 0){
		if (skip_next_data_byte){
			skip_next_data_byte = 0;
		}else{
			if (offset == 0){
				//append data:
				buffer[pos++] = data;
				len--;
			}else{
				offset--;
			}
		}
		if (len == 0)
			return 0;
		
		data      = pgm_read_byte(data_ptr++);
		data_next = pgm_read_byte(data_ptr);

		//while parsing the include file we replace 
		//$$TN -> temp now
		//$$T+ -> max temp today
		//$$T- -> min temp today
		//$$CK -> current time
		//$$FF  -> show include file
		//$$Ixy -> show PORTx.y pin as icon
		//$$??  -> show enc28j60 revision
		if ((data == '$') && (data_next == '$')){
			switch (pgm_read_byte(data_ptr + 1)){
				case ('F'):
					//include other file !
					if (include != NULL){
						//reconfigure data pointer
						//+/- 3: see bolow data_ptr+=3 !
						data_ptr_old = data_ptr + 3;
						data_ptr     = include -3; 
						data_available = 2;
					}
					break;

				case ('L'):
					//dataflash file list
					pos = filesystem_generate_index(buffer, pos, &len, &offset);
					if (pos == 0)
						return 0; //eof
					break;

/*
				case ('T'):
					//temperature request!
					//if there is not enough space in buffer, abort & wait for next packet: <0XX,X>
					///if (pos+5 > NIC_BUFFERSIZE)
					///	return 0;
				
					//temperature fits!
					//calculate buffer
					if (pgm_read_byte(data_ptr + 2) == 'N') //TEMP NOW:
						logger_output_temp(&smallbuffer[0], LOGGER_NOW);
					else if (pgm_read_byte(data_ptr + 2) == '-') //TEMP MIN
						logger_output_temp(&smallbuffer[0], LOGGER_MIN);
					else	//TEMP MAX
						logger_output_temp(&smallbuffer[0], LOGGER_MAX);	

					//copy smallbuffer to ethernetbuffer
					for (unsigned char i=0; i<5; i++){
						if (offset == 0){
							buffer[pos++] = smallbuffer[i];
							len--;
							if (len == 0)
								return 0;	
						}else{
							offset--;
						}
					}
					break;
*/
				case ('C'):
					//CLOCK:
					//if there is not enough space in buffer, abort & wait for next packet: <0XX,X>
					///if (pos+8 > NIC_BUFFERSIZE)
					///	return 0;

					//make sure the time is correct
					clock_do();

					//fill small buffer
					string_uint8_to_buffer_02dez(&smallbuffer[0], clock[CLOCK_HOUR]);
					smallbuffer[2] = ':';
					string_uint8_to_buffer_02dez(&smallbuffer[3], clock[CLOCK_MIN]);
					smallbuffer[5] = ':';
					string_uint8_to_buffer_02dez(&smallbuffer[6], clock[CLOCK_SEC]);
					
					//copy small buffer to ethernet buffer
					for (unsigned char i=0; i<8; i++){
						if (offset == 0){
							buffer[pos++] = smallbuffer[i];
							len--;
							if (len == 0)
								return 0;	
						}else{
							offset--;
						}
					}
					break;

				case ('I'):
					smallbuffer[0] = pgm_read_byte(data_ptr + 2);
					smallbuffer[1] = pgm_read_byte(data_ptr + 3);
					smallbuffer[2] = '_';
					smallbuffer[3] = '0' + port_get_portbit(pgm_read_byte(data_ptr + 2), pgm_read_byte(data_ptr + 3));
					string_progmem_to_buffer(PSTR(".gif"), &smallbuffer[4], 4);
					
					//copy small buffer to ethernet buffer
					for (unsigned char i=0; i<8; i++){
						if (offset == 0){
							buffer[pos++] = smallbuffer[i];
							len--;
							if (len == 0)
								return 0;	
						}else{
							offset--;
						}
					}
					data_ptr += 1; //we use one byte more here !
					break;

				case ('?'):
					string_uint8_to_buffer(&smallbuffer[0], enc28j60_revision);
					for (unsigned char i=0; i<3; i++){
						if (offset == 0){
							buffer[pos++] = smallbuffer[i];
							len--;
							if (len == 0)
								return 0;	
						}else{
							offset--;
						}
					}
					break;

				default:
					//unknown
					//do nothing
					break;
			}
			//skip TAG
			data_ptr += 3; //$$..
			skip_next_data_byte = 1; //we have to skip the $ which is inside the data buffer
		}
		if (data == 0){
			//end of pgm string... is this end of include or end of main ?
			// if main    -> data_available = 1
			// if include -> data_available = 2
			if (data_available == 1){
				data_available = 0; //STOP!
			}else{
				//this was the include, switch back to main!
				data_available = 1;
				data_ptr = data_ptr_old;
				skip_next_data_byte = 1;
			}
		}
	}
	return pos;
}


unsigned int httpd_add_progmem_data_bin(PGM_P pointer, unsigned char *buffer, unsigned int pos, unsigned int offset, unsigned int len, unsigned int pgm_datalen){
	unsigned char data;
	//unsigned char data_next;
	
	data = pgm_read_byte(pointer++);
	
	while (pgm_datalen){
		if (offset == 0){
			//append data:
			buffer[pos++] = data;
			len--;
		}else{
			offset--;
		}
		if (len == 0)
			return 0;
	
		data      = pgm_read_byte(pointer++);
		pgm_datalen--;
	}
	return pos;
}

/*unsigned int httpd_add_io_icon(unsigned char *buffer, unsigned int datalen, unsigned int offset, unsigned char socketnum){
	unsigned int datapos = 0;

	//need to add header ?!
	if (offset < sizeof(HTTPD_BUTTON_HEAD)){
		//add gif header:
		datapos = httpd_add_progmem_data_bin(HTTPD_BUTTON_HEAD, buffer, datapos, offset, NIC_BUFFERSIZE-TCP_POS_TXDATA, sizeof(HTTPD_BUTTON_HEAD));
	}

	if (offset >= sizeof(HTTPD_BUTTON_HEAD)){
		offset = offset - sizeof(HTTPD_BUTTON_HEAD);
		if (offset < sizeof(HTTPD_BUTTON_RED_TABLE)){
			//add colortable
			if ((httpd_conn[socketnum].file_id & 0xff) == HTTPD_STATE_FILE_SET_IO_1_PIC){
				//add green
				datapos = httpd_add_progmem_data_bin(HTTPD_BUTTON_GREEN_TABLE, buffer, datapos, offset, NIC_BUFFERSIZE-TCP_POS_TXDATA, sizeof(HTTPD_BUTTON_GREEN_TABLE));
			}else{
				//add red
				datapos = httpd_add_progmem_data_bin(HTTPD_BUTTON_RED_TABLE, buffer, datapos, offset, NIC_BUFFERSIZE-TCP_POS_TXDATA, sizeof(HTTPD_BUTTON_RED_TABLE));
			}
		}
	}
	
	if (offset >= sizeof(HTTPD_BUTTON_RED_TABLE)){
			offset = offset - sizeof(HTTPD_BUTTON_RED_TABLE);
			//add gif data:
			datapos = httpd_add_progmem_data_bin(HTTPD_BUTTON_DATA, buffer, datapos, offset, NIC_BUFFERSIZE-TCP_POS_TXDATA, sizeof(HTTPD_BUTTON_DATA));
	}
	return datapos+TCP_POS_TXDATA;
}*/






