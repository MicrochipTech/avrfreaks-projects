/*
,-----------------------------------------------------------------------------------------.
| io/mca25
|-----------------------------------------------------------------------------------------
| this file implements the communcation protocol used by sony ericcsons mca25 handy camera
| - based on my mca25 code shipped with the rtl8019 webservercode
| - enhanced by some ideas from jesper
| - mca25 packetsize can be freely configured -> use full ethernet buffer -> faster
|
| BUGS:
| - sometimes the camera hangs during mux7 etc -> but cam is then restartet
| - retransfer if there was a lost packet is NOT TESTED !!
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
#include "mca25.h"
#include "led.h"

//ACTIVATE DEBUG by editing this file:
#include "../debug.h"

//start JPG magic
PROGMEM char MCA25_START_JPG[] = {
0xF9,0x81,0xEF,0x3F,0x83,0x00,0x82,0x71,0x00,0x58,0x3C,0x6D,0x6F,0x6E,0x69,0x74,0x6F,0x72,0x69,0x6E,0x67,0x2D,0x63,0x6F,
0x6D,0x6D,0x61,0x6E,0x64,0x20,0x76,0x65,0x72,0x73,0x69,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x6F,0x6E,0x3D,0x22,0x31,0x2E,0x30,0x22,0x20,0x74,0x61,0x6B,0x65,0x2D,0x70,0x69,0x63,0x3D,0x22,0x4E,
0x4F,0x22,0x20,0x73,0x65,0x6E,0x64,0x2D,0x70,0x69,0x78,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x65,0x6C,0x2D,0x73,0x69,0x7A,0x65,0x3D,0x22,0x36,0x34,0x30,0x2A,0x34,0x38,0x30,0x22,0x20,0x7A,0x6F,
0x6F,0x6D,0x3D,0x22,0x31,0x30,0x22,0x2F,0x3E,0x42,0x00,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x21,0x78,0x2D,0x62,0x74,0x2F,0x69,0x6D,0x61,0x67,0x69,0x6E,0x67,0x2D,0x6D,0x6F,0x6E,0x69,0x74,0x6F,
0x72,0x69,0x6E,0x67,0x2D,0x69,0x6D,0x61,0x67,0x65,0x00,0x8C,0xF9,
0xF9,0x81,0xEF,0x0D,0x4C,0x00,0x06,0x06,0x01,0x80,0x4B,0xF9};


//start capturing magic
PROGMEM char MCA25_START_CAPTURING[] = {
//part 1
0xF9,0x81,0xEF,0x3F,0x83,0x00,0x69,0x71,0x00,0x3F,0x3C,0x6D,0x6F,0x6E,0x69,0x74,0x6F,0x72,0x69,
0x6E,0x67,0x2D,0x63,0x6F,0x6D,0x6D,0x61,0x6E,0x64,0x20,0x76,0x65,0x72,0x73,0x69,0x8C,0xF9,
//part 2
0xF9,0x81,0xEF,0x3F,0x6F,0x6E,0x3D,0x22,0x31,0x2E,0x30,0x22,0x20,0x74,0x61,0x6B,0x65,0x2D,0x70,
0x69,0x63,0x3D,0x22,0x59,0x45,0x53,0x22,0x20,0x7A,0x6F,0x6F,0x6D,0x3D,0x22,0x31,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x30,0x22,0x2F,0x3E,0x42,0x00,0x21,0x78,0x2D,0x62,0x74,0x2F,0x69,0x6D,0x61,
0x67,0x69,0x6E,0x67,0x2D,0x6D,0x6F,0x6E,0x69,0x74,0x6F,0x72,0x69,0x6E,0x67,0x2D,0x8C,0xF9,
//part 3
0xF9,0x81,0xEF,0x19,0x69,0x6D,0x61,0x67,0x65,0x00,0x4C,0x00,0x06,0x06,0x01,0x80,0x50,0xF9};

//configure cam magic
PROGMEM char MCA25_CONFIG_640x480[] = {
0xF9,0x81,0xEF,0x3F,0x82,0x01,0x3B,0x01,0x00,0x03,0x49,0x01,0x35,0x3C,0x63,0x61,0x6D,0x65,
0x72,0x61,0x2D,0x73,0x65,0x74,0x74,0x69,0x6E,0x67,0x73,0x20,0x76,0x65,0x72,0x73,0x69,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x6F,0x6E,0x3D,0x22,0x31,0x2E,0x30,0x22,0x20,0x77,0x68,0x69,0x74,0x65,
0x2D,0x62,0x61,0x6C,0x61,0x6E,0x63,0x65,0x3D,0x22,0x4F,0x46,0x46,0x22,0x20,0x63,0x6F,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x6C,0x6F,0x72,0x2D,0x63,0x6F,0x6D,0x70,0x65,0x6E,0x73,0x61,0x74,0x69,
0x6F,0x6E,0x3D,0x22,0x31,0x33,0x22,0x20,0x66,0x75,0x6E,0x2D,0x6C,0x61,0x79,0x65,0x72,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x3D,0x22,0x30,0x22,0x3E,0x3C,0x6D,0x6F,0x6E,0x69,0x74,0x6F,0x72,0x69,
0x6E,0x67,0x2D,0x66,0x6F,0x72,0x6D,0x61,0x74,0x20,0x65,0x6E,0x63,0x6F,0x64,0x69,0x6E,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x67,0x3D,0x22,0x45,0x42,0x4D,0x50,0x22,0x20,0x70,0x69,0x78,0x65,0x6C,
0x2D,0x73,0x69,0x7A,0x65,0x3D,0x22,0x38,0x30,0x2A,0x36,0x30,0x22,0x20,0x63,0x6F,0x6C,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x6F,0x72,0x2D,0x64,0x65,0x70,0x74,0x68,0x3D,0x22,0x38,0x22,0x2F,0x3E,
0x0D,0x0A,0x3C,0x74,0x68,0x75,0x6D,0x62,0x6E,0x61,0x69,0x6C,0x2D,0x66,0x6F,0x72,0x6D,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x61,0x74,0x20,0x65,0x6E,0x63,0x6F,0x64,0x69,0x6E,0x67,0x3D,0x22,0x45,
0x42,0x4D,0x50,0x22,0x20,0x70,0x69,0x78,0x65,0x6C,0x2D,0x73,0x69,0x7A,0x65,0x3D,0x22,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x31,0x30,0x31,0x2A,0x38,0x30,0x22,0x20,0x63,0x6F,0x6C,0x6F,0x72,0x2D,
0x64,0x65,0x70,0x74,0x68,0x3D,0x22,0x38,0x22,0x2F,0x3E,0x0D,0x0A,0x3C,0x6E,0x61,0x74,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x69,0x76,0x65,0x2D,0x66,0x6F,0x72,0x6D,0x61,0x74,0x20,0x65,0x6E,0x63,
0x6F,0x64,0x69,0x6E,0x67,0x3D,0x22,0x22,0x20,0x70,0x69,0x78,0x65,0x6C,0x2D,0x73,0x69,0x8C,0xF9,
0xF9,0x81,0xEF,0x3F,0x7A,0x65,0x3D,0x22,0x36,0x34,0x30,0x2A,0x34,0x38,0x30,0x22,0x2F,0x3E,
0x0D,0x0A,0x3C,0x2F,0x63,0x61,0x6D,0x65,0x72,0x61,0x2D,0x73,0x65,0x74,0x74,0x69,0x6E,0x8C,0xF9,
0xF9,0x81,0xEF,0x0B,0x67,0x73,0x3E,0x0D,0x0A,0xAF,0xF9};


//global vars
unsigned char mca25_pic_state;

unsigned char mca25_allocated_for_socket;
unsigned char mca25_allocated_timeout;

unsigned char mca25_buffer[MCA25_COMM_BUFFER_LEN];
unsigned char mca25_ready;
unsigned char mca25_mux_buffer[6];

unsigned long mca25_datapos;
unsigned char mca25_jpg_state;
unsigned char mca25_is_rev017;

#if 0
unsigned char mca25_brightness;
//estimate picture brightness, might be used to switch led light only if necessary
void mca25_estimate_brightness(unsigned char val){
	//pixel is rgb332, we use rgb222 only!
	unsigned char tmp = (((val&0xC0)>>6) + ((val&0x18)>>3) + (val&0x3));
	mca25_brightness += tmp;
}
#endif

//send data ack
void mca25_send_data_ack(void){
	mca25_puts_progmem("\xF9\x81\xEF\x07\x83\x00\x03\xA6\xF9");
}

//handle channel 0x23 data
void mca25_handle_channel_x23(void){
	if (mca25_mux_buffer[1] == 0xEF){
		//UIH
		if (string_compare_progmem_plen("AT*ECUR=", mca25_buffer)){
			//ack current request packets:
			// -> send mux "\r\nOK\r\n" packet:
			mca25_puts_progmem("\xF9\x21\xEF\x0D\x0D\x0A\x4F\x4B\x0D\x0A\x48\xF9");
		}else{
			#if MCA25_DEBUG
			softuart_putc('u');
			#endif
		}
	}else if (mca25_mux_buffer[1] == 0x3F){
		//SABM
		mca25_puts_progmem("\xF9\x23\x73\x01\x02\xF9");
	}
}

//handle channel 0x03 data
void mca25_handle_channel_x03(void){
	if (mca25_mux_buffer[1] == 0xEF){
		//UIH
		if (string_compare_progmem_plen("\xE1\x07\x23\x0C\x01", mca25_buffer)){
			mca25_puts_progmem("\xF9\x01\xEF\x0B\xE3\x07\x23\x0C\x01\x79\xF9");
		}else{
			#if MCA25_DEBUG
			softuart_putc('v');
			#endif
		}
	}else if (mca25_mux_buffer[1] == 0x3F){
		//SABM
		mca25_puts_progmem("\xF9\x03\x73\x01\xD7\xF9");
	}
}

//cleanup dead connection -> run this every second !
void mca25_cleanup(void){
	if (mca25_pic_state != MCA25_PIC_STATE_IDLE){
		if (mca25_allocated_timeout == 0){
			#if MCA25_DEBUG
			softuart_puts_progmem("CAM : allocation to socket ");
			softuart_put_uint8(mca25_allocated_for_socket);
			softuart_puts_progmem(" removed");
			softuart_putnewline();
			#endif	
			mca25_pic_state = MCA25_PIC_STATE_IDLE;
		}else{
			mca25_allocated_timeout--;
		}
	}
}

//grab jpg data & store in buffer
unsigned int mca25_grab_data(char *buffer, unsigned int datalen, unsigned long pos, unsigned char socketnum){
	int skip_count=0;
	unsigned char state=0;
	
	mca25_allocated_timeout = 5; //5sec timeout

	//cam is used by another user -> abort
	if ((mca25_pic_state != MCA25_PIC_STATE_IDLE) && (mca25_allocated_for_socket != socketnum)){
		return 0xFFFE;
	}

	//cam is no longer working -> reset !
	if ((mca25_pic_state == MCA25_PIC_STATE_CAM_ERROR) || (mca25_ready == 0)){
		//cam not initialised or missing!
		#if MCA25_DEBUG
		softuart_puts_progmem("CAM : grab -> cam error -> retry init!");
		softuart_putnewline();
		#endif
		mca25_init_cam();
		mca25_configure();
		//return 0xFFFF; //-> abort this image !
	}

	//we need to do a preview image:
	if (mca25_pic_state == MCA25_PIC_STATE_IDLE){
		mca25_start_image_grab(socketnum);
	}
		
#if 1
	softuart_puts_progmem("CAM : req byte ");
	softuart_put_uint16(pos);
	softuart_puts_progmem(" cur pos is: ");
	softuart_put_uint16(mca25_datapos);
	softuart_putnewline();
	//tcp stack requested data bytes we do no longer have...
	//hack: dont touch ethernetbuffer & hope that data is still available.
	//      if buffer was used during last transfer -> picture will have errors...
	if (pos<mca25_datapos){
		int adjust = (mca25_datapos-pos)-512;
		if (adjust == 0){
			//perfect
			return 512;
		}else if(adjust > 0){
			//we must shift the data:
			for(unsigned int a=0; a<512; a++){
				if (a+adjust <= 512)
					buffer[a] = buffer[a+adjust];
			}
			return adjust; //#adjusted bytes data
		}else{
			//>512 bytes of data seems to be lost -> no way to fix it...
			//-> dont touch the buffer, maybe we can use the data later ...
			return (adjust%512);
		}
	}else if(pos>mca25_datapos){
		//?! how could that happen ?
		//FIXME:
		//1) ignore the missing data & continue
		//2) skip camera data until we have the requestet data packet...
		//-> use (1), change to 2 later ?!
	}
#endif

	#if MCA25_DEBUG
	/*softuart_puts_progmem("CAM : grab data starting at byte ");
	softuart_put_uint16(pos>>16);
	softuart_put_uint16(pos&0xFFFF);
	softuart_puts_progmem(", current pos is ");
	softuart_put_uint16(mca25_datapos>>16);
	softuart_put_uint16(mca25_datapos&0xFFFF);
	softuart_putnewline();*/
	#endif

	unsigned int dcnt=0;
	unsigned int res=0;
	unsigned int bufpos=0;

	//we have received data -> send an ack !
	if ((mca25_pic_state == MCA25_PIC_STATE_JPG_DATA) || (mca25_pic_state ==  MCA25_PIC_STATE_LAST_DATA)){
		mca25_send_data_ack();
	}

	//do the statemachine
	while (state != 100){
		if (state == 0)
			res = mca25_read_mux_packet(&buffer[bufpos],6);
		else
			res = mca25_read_mux_packet(&buffer[bufpos],0);
		if (res == 0){
			#if MCA25_DEBUG 
			softuart_puts_progmem("TIMEOUT! (MUX J)");
			softuart_putnewline();
			softuart_puts_progmem("dcount=");
			softuart_put_uint16(dcnt);
			softuart_putnewline();
			#endif

			//cam error, disable !
			mca25_ready = 0;
			return 0xFFFF; //read failed -> exit!; 
		}
	
		if (bufpos>datalen){
			#if MCA25_DEBUG 
			softuart_puts_progmem("CAM : buf overflow! ");
			softuart_put_uint16(skip_count);
			softuart_put_uint16(bufpos);
			softuart_putnewline();
			#endif
			return 0xFFFF;
		}

		//new version, thanks to jesper ;)
		//softuart_put_uint8(mca25_buffer[1]);
		if ((mca25_mux_buffer[0] == 0x83) && (mca25_mux_buffer[1] == 0xEF) ){ //data channel & UIH packet
				dcnt++;
				
				switch (state){
					case 0: //check how many bytes cam want to send
						skip_count = ((int)mca25_mux_buffer[4]<<8) + mca25_mux_buffer[5];

						if (skip_count > datalen){
							skip_count = datalen;
						}
						
						if (skip_count > 3){
							//only handle data packets -> ignore packets smaller 3 bytes (=acks)
							skip_count    -= ((mca25_mux_buffer[2])>>1);
							bufpos        += (mca25_mux_buffer[2]>>1)-6;

							//last data packet ?!
							if (mca25_mux_buffer[3] == 0xA0){
								mca25_pic_state = MCA25_PIC_STATE_LAST_DATA; //no more data
							}else{
								mca25_pic_state = MCA25_PIC_STATE_JPG_DATA; //middle of pic
							}
							state = 1;
							
							if (skip_count <= 0){
								//softuart_putc('.');
								if (mca25_pic_state == MCA25_PIC_STATE_LAST_DATA){
									mca25_send_data_ack();

									//reconfigure mux channel (?!)
									mca25_puts_progmem("\xF9\x01\xEF\x0B\xE3\x07\x23\x0C\x01\x79\xF9");

									mca25_pic_state = MCA25_PIC_STATE_IDLE;
									mca25_datapos += bufpos;
									return bufpos;
								}else{
									mca25_datapos += bufpos;
							    return 0;
								}
							}
						}
						break;
					case 2:
					case 1:
						skip_count    -= ((mca25_mux_buffer[2])>>1);
						bufpos        += (mca25_mux_buffer[2]>>1)-0;
	
						if (skip_count <= 0){
							//softuart_putc('.');
							if (mca25_pic_state == MCA25_PIC_STATE_LAST_DATA){
								mca25_send_data_ack();

								//reconfigure mux channel (?!)
								mca25_puts_progmem("\xF9\x01\xEF\x0B\xE3\x07\x23\x0C\x01\x79\xF9");

								mca25_pic_state = MCA25_PIC_STATE_IDLE;
								mca25_datapos += bufpos;
								return bufpos; //last data packet !
							}else{
								mca25_datapos += bufpos;
								return 0; //state = 100;
							}
						}
						break;

					default:
						break;
				} //case
				//softuart_put_uint16(skip_count);
			}else if (mca25_mux_buffer[0] == 0x23){
				//channel 0x23
				mca25_handle_channel_x23();
			}else if (mca25_mux_buffer[0] == 0x03){ 
				//channel 0x03
				mca25_handle_channel_x03();
			}else{
				/*for(unsigned char f=0; f<40; f++){
					softuart_put_uint8(f);
					softuart_putc('>');
					softuart_put_uint8(mca25_buffer[f]);
					softuart_putnewline();
				}softuart_putnewline();softuart_putnewline();*/
			}
		}	
		//softuart_puts_progmem("count=");
		//softuart_put_uint16(dcnt);
		//preview image #i has been grabbed.
		#if MCA25_DEBUG
		softuart_putc(' ');
		#endif
	
	return 0xFFFF;
}

//start image grab, grab preview pictures and send jpg request
unsigned char mca25_start_image_grab(unsigned char socketnum){
	unsigned char state;
	unsigned char datapos;
	int skip_count=0;

	//switch light on (slow fade in !)
	//mpf10_fade(MPF10_FADE_IN);
	//without soft fade in:
	MPF10_ENABLE();

	//reset global data byte counter	
	mca25_datapos = 0;

	//allocate cam to current socket
	mca25_allocated_for_socket = socketnum;

	//reset state
	mca25_pic_state = MCA25_PIC_STATE_IDLE; //not ready

	if (mca25_ready == 0){
		//cam not initialised or missing!
		#if MCA25_DEBUG
		softuart_puts_progmem("CAM : cam not initialised or missing -> abort.");
		softuart_putnewline();
		#endif
		MPF10_DISABLE();
		return 0;
	}
		
	#if MCA25_DEBUG
	softuart_puts_progmem("CAM : starting image grab...");
	softuart_putnewline();
	softuart_puts_progmem("CAM : grabbing preview ");
	#endif
	
	//grab 5 preview pictures:
	for (char i=0; i<5; i++){
		#if MCA25_DEBUG
		softuart_put_uint8(i);
		#endif

		//softuart_put_uint8(mca25_brightness);
		//mca25_brightness = 0;

		if ((i==0) && (mca25_pic_state == MCA25_PIC_STATE_JPG_DATA)){
			// do not send start cmd ! an jpg request is already running 
			// -> use first "preview" to ack jpg data!
		}else{
			if (mca25_pic_state == MCA25_PIC_STATE_LAST_DATA){
				//last data -> send an ack !
				mca25_send_data_ack();
			}
		
			// send capture start cmd:
			mca25_puts_progmem2(MCA25_START_CAPTURING, sizeof(MCA25_START_CAPTURING)); 
		}

		state = 0;
		datapos = 0;
	
		// grab actual image and tell cam to keep it in RAM
		// we acknowledge the _preview_ image and download
		// the full size image later...
		while (state != 100){
			
			if( ! mca25_read_mux_packet(mca25_buffer,0)){
				#if MCA25_DEBUG 
				softuart_puts_progmem("TIMEOUT! (MUX4) / previewpic");
				softuart_put_uint8(i);
				softuart_putnewline();
				#endif

				//cam error, disable !
				mca25_ready = 0;
				MPF10_DISABLE();
				return 0; //read failed -> exit!; 
			}

			//new version, thanks to jesper ;)
			//softuart_put_uint8(mca25_buffer[1]);
			if ((mca25_mux_buffer[0] == 0x83) && (mca25_mux_buffer[1] == 0xEF) ){ //data channel & UIH packet
				//softuart_put_uint16(skip_count);
				switch (state){
					case 0: //check how many bytes cam want to send
						skip_count = ((int)mca25_mux_buffer[4]<<8) + mca25_mux_buffer[5];
						
						if (skip_count > 3){
							//only handle data packets -> ignore packets smaller 3 bytes (=acks)
							skip_count -= ((mca25_mux_buffer[2])>>1);

							//last data packet ?!
							if (mca25_buffer[0] == 0xA0) 
								state = 2;
							else 
								state = 1;

							//estimate picture brightness by grabbing some bmp preview pixels
							///mca25_estimate_brightness(mca25_buffer[28]);

							if (skip_count <= 0){
								//softuart_putc('.');
								mca25_send_data_ack();
								if (state == 2)
									state = 100;
								else
							    state = 0;
							}
						}
						break;
					case 2:
					case 1:
						skip_count -= ((mca25_mux_buffer[2])>>1);
						
						//estimate picture brightness by grabbing some bmp preview pixels
						///mca25_estimate_brightness(mca25_buffer[28]);

						if (skip_count <= 0){
							softuart_putc('.');
							mca25_send_data_ack();
							if (state == 2)
								state = 100;
							else
								state = 0;
						}
						break;

					default:
						break;
				} //case
				//softuart_put_uint16(skip_count);
			}else if (mca25_mux_buffer[0] == 0x23){
				//channel 0x23
				mca25_handle_channel_x23();
			}else if (mca25_mux_buffer[0] == 0x03){ 
				//channel 0x03
				mca25_handle_channel_x03();
			}else{
				for(unsigned char f=0; f<40; f++){
					softuart_put_uint8(f);
					softuart_putc('=');
					softuart_put_uint8(mca25_buffer[f]);
					softuart_putnewline();

				}softuart_putnewline();softuart_putnewline();//softuart_putc('.');
				/*softuart_put_uint8(mca25_buffer[1]);
				softuart_put_uint8(mca25_buffer[2]);
				softuart_put_uint8(mca25_buffer[3]);
				softuart_put_uint8(mca25_buffer[4]);
				softuart_putnewline();*/
			}
		}
		//preview image #i has been grabbed.
		#if MCA25_DEBUG
		softuart_putc('-');
		#endif
	}
	#if MCA25_DEBUG
	softuart_putnewline();
	softuart_puts_progmem("CAM : preview images done!");
	softuart_putnewline();
	softuart_puts_progmem("CAM : sending JPG grab request... ");
	softuart_putnewline();
	#endif

	// send capture start cmd:
	mca25_puts_progmem2(MCA25_START_JPG, sizeof(MCA25_START_JPG));
	#if MCA25_DEBUG
	//softuart_puts_progmem("done!");
	//softuart_putnewline();
	#endif

	//switch light off:
	MPF10_DISABLE();

	//cam has transferred no image byte yet -> 0
	mca25_datapos=0;
	mca25_pic_state = MCA25_PIC_STATE_PREVIEW_DONE; //preview done!
	return 1;
}

//configure mca25
unsigned char mca25_configure(void){
	unsigned char state=0;
	//unsigned char res = 0;
	int skip_count=0;

	mca25_pic_state = MCA25_PIC_STATE_IDLE;

	if (mca25_ready == 0){
		//cam not initialised or missing!
		#if MCA25_DEBUG
		softuart_puts_progmem("CAM : cam not initialised or missing -> abort.");
		softuart_putnewline();
		#endif
		return 0;
	}
		
	#if MCA25_DEBUG
	softuart_puts_progmem("CAM : configure...");
	#endif
	
	//config cam:
	mca25_puts_progmem2(MCA25_CONFIG_640x480, sizeof(MCA25_CONFIG_640x480)); 

	while (state != 100){
		if( ! mca25_read_mux_packet(mca25_buffer,0)){
				#if MCA25_DEBUG 
				softuart_puts_progmem("TIMEOUT! (MUX2)");
				softuart_putnewline();
				#endif

				//cam error, disable !
				mca25_ready = 0;
				return 0; //read failed -> exit!; 
		}

		if (mca25_mux_buffer[0] == 0x83){
			if (mca25_mux_buffer[1] == 0xEF){//UIH packet
				switch (state){
					case 0:
					case 1:
						// wait for cam ACK:
						// [F9 83 EF 07 A0 00 03 C7 F9
						//if (string_compare_progmem_plen("\xF9\x83\xEF\x07\xA0\x00\x03\xC7\xF9", mca25_buffer)){
						if (string_compare_progmem_plen("\xA0\x00\x03", mca25_buffer)){
							// request camera info:
							// [F9 81 EF 2F 83 00 17 42 00 14 78 2D 62 74 2F 63 
							//  61 6D 65 72 61 2D 69 6E 66 6F 00 90 F9]
							mca25_puts_progmem("\xF9\x81\xEF\x2F\x83\x00\x17\x42\x00\x14\x78\x2D\x62\x74\x2F\x63\x61\x6D\x65\x72\x61\x2D\x69\x6E\x66\x6F\x00\x90\xF9");	
							state = 2;
						}
						break;
					case 2:
						// ignore camera info ...
						// new version: read how many config bytes cam wants to send
						// in order to decide when we can continue!
						// thanks to jesper for this idea ;)  [now works with brand new, unused cams!]
						skip_count = ((int)mca25_mux_buffer[3]<<8) + mca25_mux_buffer[4];
						skip_count -= (mca25_mux_buffer[2]>>1);
						state = 3;
						
						break;
		
					case 3:
						//now skip the necessary amount of bytes:
						skip_count -= (mca25_mux_buffer[2]>>1);
		
						//finished !
						if (skip_count <= 0)
							state = 100;
						break;
		
					default:
						break;
				}
			}
		}else if (mca25_mux_buffer[0] == 0x23){
			//channel 0x23
			mca25_handle_channel_x23();
		}else if (mca25_mux_buffer[0] == 0x03){ 
			//channel 0x03
			mca25_handle_channel_x03();
		}else{
			softuart_putc('i');
		}
	}
	#if MCA25_DEBUG
	softuart_puts_progmem("done!");
	softuart_putnewline();
	#endif
	return 1;
}


//initialise
void mca25_init(void){
	//start with rev016
	mca25_is_rev017 = 0;
	mca25_allocated_timeout = 0;
	mca25_pic_state = MCA25_PIC_STATE_IDLE;

	mca25_init_cam();
}

//initialise webcam, return 0 if failed!
unsigned char mca25_init_cam(void){
	unsigned char state=0;
	#if MCA25_DEBUG 
	//unsigned char old_state=0xFF;
	#endif	

	//mark cam as not ready:
	mca25_ready	= 0;

	//make camreset pin output
	MCA25_RESET_PORT_DIR |=  (1<<MCA25_RESET_PIN); 
	PORTD |= (1<<1);  //UART TX = output
	PORTD &= ~(1<<0); //UART RX =  input

	MCA25_RESET_LO();

	//init uart
	if (mca25_is_rev017) 
		mca25_uart_init_19200();
	else
		mca25_uart_init_9600();

	//reset cam:
	MCA25_RESET_LO();
	_delay_ms(50);
	MCA25_RESET_HI();

	#if MCA25_DEBUG 
	softuart_puts_progmem("CAM : reset");
	softuart_putnewline();
	#endif
/*
	//WARNING: will block whole server if no cam connected !! -> DO NOT USE!

	//detect mca25 revision
	//KTY 105 016 sends always at 9600 baud
	//KTY 105 017 sends AT at 9600 and then switches to 19200 !!!
	unsigned int count;
	for (state = 0; state <4; state++){
		count=0;
		while((PIND & (1<<0))){}
		while(!(PIND & (1<<0))){ count++; }
		softuart_put_uint16(count);
		softuart_putnewline();
	}


	#if MCA25_DEBUG 
	softuart_puts_progmem("CAM : detected rev");
	if (count > 50)
		softuart_puts_progmem("016");
	else
		softuart_puts_progmem("017");
	softuart_putnewline();
	#endif
	
	//revision 017 !
	if (count < 50){
		mca25_uart_init_19200();
		//reset cam again:
		MCA25_RESET_LO();
		_delay_ms(50);
		MCA25_RESET_HI();
	}
*/

	#if MCA25_DEBUG 
	softuart_puts_progmem("CAM : init (takes ~5sec) ");
	#endif

	while (state != 100){
		//read cam data:
		if (state<10){
			//read AT command
			if ( ! mca25_read_at_command(mca25_buffer)){
				#if MCA25_DEBUG 
				softuart_puts_progmem("TIMEOUT! (AT) ");
				#endif
				if (state == 0){
					//maybe this is a bad/new (?) mca25 which wants to communicate at 19200 baud!
					//retry!
					mca25_uart_init_19200();	
					#if MCA25_DEBUG
					softuart_puts_progmem(" trying 19200 baud ");
					#endif
					if ( ! mca25_read_at_command(mca25_buffer)){
						#if MCA25_DEBUG 
						softuart_puts_progmem("TIMEOUT! (AT) ");
						softuart_putnewline();
						#endif
						return 0;
					}else{
						mca25_is_rev017 = 1;
					}
				}else{
					//state != 0 -> always exit
					#if MCA25_DEBUG
					softuart_putnewline();
					#endif
					return 0; //read failed -> exit! 
				}
			}
		}else{
			//read MUX packet
			if( ! mca25_read_mux_packet(mca25_buffer,0)){
				#if MCA25_DEBUG 
				softuart_puts_progmem("TIMEOUT! (MUX)");
				softuart_putnewline();
				#endif
				return 0; //read failed -> exit!; 
			}
		}
		#if MCA25_DEBUG 
		/*if (old_state != state){
			softuart_put_uint8(state);
			softuart_putc('-');
			old_state = state;
		}*/
		#endif
		switch (state){
			case 0:
				//we have to wait for AT&F:
				if (string_compare_progmem_plen("AT&F", mca25_buffer) || string_compare_progmem_plen("AT&F", &mca25_buffer[2])){
					mca25_send_ok();
					state = 1;
				}
				break;

			case 1:
				//wait for AT+IPR
				if (string_compare_progmem_plen("AT+IPR=?", mca25_buffer)){
					mca25_puts_progmem("+IPR: (),(1200,2400,4800,9600,19200,38400,57600,460800)\r\n\r\nOK\r\n");
					state = 2;
				}
				break;
			
			case 2:
				//wait for AT+IPR SET command
				if (string_compare_progmem_plen("AT+IPR=460800", mca25_buffer)){
					mca25_puts_progmem("\r\nOK\r\n"); //bubug: here only 1 \r before OK!
					//set higher baudrate:
					mca25_uart_init_460800();
					//wait ...
					state = 3;
				}
				break;

			case 3:
				//wait for mux info req
				if (string_compare_progmem_plen("AT+CMUX=?", mca25_buffer)){
					mca25_puts_progmem("\r\r\n+CMUX: (0),(0),(1-7),(31),(10),(3),(30),(10),(1-7)\r\n");
					state = 4;
				}
				break;

			case 4:
				//wait for mux enable request
				if (string_compare_progmem_plen("AT+CMUX=0,0,7,31", mca25_buffer)){
					#if MCA25_DEBUG 
					softuart_puts_progmem("at done, ");
					#endif
					mca25_send_ok();
					state = 10;
				}
				break;

			case 10:
					// wait for mux ch0 request:
					// [F9 03 3F 01 1C F9]
					//if (string_compare_progmem_plen("\xF9\x03\x3F\x01\x1C\xF9", mca25_buffer)){
					if ((mca25_mux_buffer[0] == 0x03) && (mca25_mux_buffer[2] == 0x01)){
						// send mux ch0 ack/open packet:
						// [F9 03 73  01 D7 F9]
						mca25_puts_progmem("\xF9\x03\x73\x01\xD7\xF9");
					  state = 11;
					}
					break;

				case 11:
					// wait for mux ch3 request:
					// [F9 23 3F 01 C9 F9]
					//if (string_compare_progmem_plen("\xF9\x23\x3F\x01\xC9\xF9", mca25_buffer)){
					if ((mca25_mux_buffer[0] == 0x23) && (mca25_mux_buffer[2] == 0x01)){
						// send mux ch3 ack/open packet:
						// [F9 23 73 01 02 F9]
						mca25_puts_progmem("\xF9\x23\x73\x01\x02\xF9");
					  state = 12;
					}
					break;

				case 12:
					// wait for config mux ch0 request:
					// [F9 03 EF 09 E3 05 23 8D FB F9]
					//if (string_compare_progmem_plen("\xF9\x03\xEF\x09\xE3\x05\x23\x8D\xFB\xF9", mca25_buffer)){
					if (string_compare_progmem_plen("\xE3\x05\x23\x8D", mca25_buffer)){
						// send mux ch0 config ack/open packet:
						// [F9 01 EF 0B E3 07 23 0C 01 79 F9]
						mca25_puts_progmem("\xF9\x01\xEF\x0B\xE3\x07\x23\x0C\x01\x79\xF9");
					  state = 13;
					}
					break;

				case 13:
					// wait for config mux ch3 request:
					// [F9 03 EF 09 E1 07 23 0C 01 FB F9]
					//if (string_compare_progmem_plen("\xF9\x03\xEF\x09\xE1\x07\x23\x0C\x01\xFB\xF9", mca25_buffer)){
					if (string_compare_progmem_plen("\xE1\x07\x23\x0C\x01", mca25_buffer)){
						// send mux ch3 config ack/open packet:
						// [F9 01 EF 09 E1 05 23 8D 9A F9]
						mca25_puts_progmem("\xF9\x01\xEF\x09\xE1\x05\x23\x8D\x9A\xF9");
					  state = 14;
					}
					break;

				case 14:
					// wait for AT*EACS.17.1.r:
					// [F9 23 EF 1B 41 54 2A 45 41 43 53 3D 31 37 2C 31 0D D1 F9]
					//if (memcmp(buf,"\xF9\x23\xEF\x1BAT*EACS=17,1\r\xD1\xF9",19) == 0){
					//if (string_compare_progmem_plen("\xF9\x23\xEF\x1B\x41\x54\x2A\x45\x41\x43\x53\x3D\x31\x37\x2C\x31\x0D\xD1\xF9", mca25_buffer)){
					if (string_compare_progmem_plen("\x41\x54\x2A\x45\x41\x43\x53\x3D\x31\x37\x2C\x31\x0D", mca25_buffer)){
						// send mux "\r\nOK\r\n" packet:
						// [F9 21 EF 0D 0D 0A 4F 4B 0D 0A 48  F9]
						mca25_puts_progmem("\xF9\x21\xEF\x0D\x0D\x0A\x4F\x4B\x0D\x0A\x48\xF9");
					  state = 15;
					}
					break;
					
				case 15:
					// wait for AT+CSCC=1,199\r5 peripheral AUTH req:
					// [F9 23 EF 1D 41 54 2B 43 53 43 43 3D 31 2C 31 39 39 0D 35 F9]
					//if (string_compare_progmem_plen("\xF9\x23\xEF\x1D\x41\x54\x2B\x43\x53\x43\x43\x3D\x31\x2C\x31\x39\x39\x0D\x35\xF9", mca25_buffer)){
					if (string_compare_progmem_plen("\x41\x54\x2B\x43\x53\x43\x43\x3D\x31\x2C\x31\x39\x39\x0D", mca25_buffer)){
						// send response token:
						//DONT// [F9 21 EF 1D 41 54 2B 43 53 43 43 3D 31 2C 31 39 39 0D 54 F9]
						// [F9 21 EF 1B 0D 0A 2B 43 53 43 43 3A 20 45 33 0D 0A B0 F9 ]
						//printf("\xF9\x21\xEF\x1D\x41\x54\x2B\x43\x53\x43"
						//			 "\x43\x3D\x31\x2C\x31\x39\x39\x0D\x54\xF9"); 
						mca25_puts_progmem("\xF9\x21\xEF\x1B\x0D\x0A\x2B\x43\x53\x43"
									 "\x43\x3A\x20\x45\x33\x0D\x0A\xB0\xF9");
									 
						//append ok:
						mca25_puts_progmem("\xF9\x21\xEF\x0D\x0D\x0A\x4F\x4B\x0D\x0A\x48\xF9");
					  state = 16;
					}
					break;
					
				case 16:
					// wait for AT+CSCC=2,199.B9\r AUTH2 req:
					// AT+CSCC.2.199.B9.r|
					// [F9 23 EF 23 41 54 2B 43 53 43 43 3D 32 2C 31 39 39 2C 42 39 0D FB F9]
					//if (string_compare_progmem_plen("\xF9\x23\xEF\x23\x41\x54\x2B\x43\x53\x43\x43\x3D\x32\x2C\x31\x39\x39\x2C\x42\x39\x0D\xFB\xF9", mca25_buffer)){
					if (string_compare_progmem_plen("\x41\x54\x2B\x43\x53\x43\x43\x3D\x32\x2C\x31\x39\x39\x2C\x42\x39\x0D", mca25_buffer)){
						// send response token: (OK)
						// [F9 21 EF 0D 0D 0A 4F 4B 0D 0A 48  F9]
						mca25_puts_progmem("\xF9\x21\xEF\x0D\x0D\x0A\x4F\x4B\x0D\x0A\x48\xF9");
						
						// now request data mux channel (ch1):
						// [F9 81 3F 01 AB F9]
						mca25_puts_progmem("\xF9\x81\x3F\x01\xAB\xF9");
					  state = 17;
					}
					break;
					
				case 17:
					// wait for mux ch1 ack:
					// [F9 81 73 01 60 F9]
					//if (string_compare_progmem_plen("\xF9\x81\x73\x01\x60\xF9", mca25_buffer)){
					if ((mca25_mux_buffer[0] == 0x81) && (mca25_mux_buffer[2] == 0x01)){
						// channel1 is now open!
					  state = 18;
					}
					break;
					
				case 18:
					// wait for ch1 mux config:
					// [F9 03 EF 09 E3 05 83 8D FB F9]
					//if (string_compare_progmem_plen("\xF9\x03\xEF\x09\xE3\x05\x83\x8D\xFB\xF9", mca25_buffer)){
					if (string_compare_progmem_plen("\xE3\x05\x83\x8D", mca25_buffer)){
						// send config response:
						// [F9 01 EF 09 E1 05 83 8D 9A F9]
						mca25_puts_progmem("\xF9\x01\xEF\x09\xE1\x05\x83\x8D\x9A\xF9");
						
						// now configure cam mode:
						// [ F9 81 EF 37 80 00 1A 10 00 01 00 46 00 13 E3 3D
						//   95 45 83 74 4A D7 9E C5 C1 6B E3 1E DE 8E 61 82 F9 ]
						// F9 81 EF 37 			= mux header
						// 80 00 1A 10 00 	= ???
						// 01 00 						= (256-6)Byte data, then wait for ACK
						// 46  00 13 E3 3D 95 45 83 74 4A D7 9E C5 C1 6B E3 1E DE 8E 61 ???
						mca25_puts_progmem("\xF9\x81\xEF\x37\x80\x00\x1A\x10\x00");
						//mca25_puts_progmem("\x02\x00");//buffsize 512 byte
						mca25_uart_putc(hi8(NIC_BUFFERSIZE-TCP_POS_TXDATA+6));
						mca25_uart_putc(lo8(NIC_BUFFERSIZE-TCP_POS_TXDATA+6));
						//"\x02\x28");//buffsize 546 data byte (but we have 6 dont care bytes) -> 546+6 = 552
						mca25_puts_progmem("\x46\x00\x13\xE3\x3D\x95\x45\x83\x74\x4A\xD7\x9E");
						mca25_puts_progmem("\xC5\xC1\x6B\xE3\x1E\xDE\x8E\x61\x82\xF9");
						mca25_puts_progmem("\xF9\x21\xEF\x0D\x0D\x0A\x4F\x4B\x0D\x0A\x48\xF9");
						
						state = 19;
					}
					break;
					
				case 19:
					// cam should now accept our settings:
					// [F9 83 EF 3F A0 00 1F 10 00 20 00 CB 00 00 00 01 4A 00
					//  13 E3 3D 95 45 83 74 4A D7 9E C5 C1 6B E3 1E DE 8E ED F9
					//if (string_compare_progmem_plen("\xF9\x83\xEF\x3F\xA0\x00\x1F\x10\x00\x20\x00\xCB\x00\x00\x00\x01\x4A\x00\x13\xE3\x3D\x95\x45\x83\x74\x4A\xD7\x9E\xC5\xC1\x6B\xE3\x1E\xDE\x8E\xED\xF9", mca25_buffer)){
					if (string_compare_progmem_plen("\xA0\x00\x1F\x10\x00\x20\x00\xCB\x00\x00\x00\x01\x4A\x00\x13\xE3\x3D\x95\x45\x83\x74\x4A\xD7\x9E\xC5\xC1\x6B\xE3\x1E\xDE\x8E", mca25_buffer)){
						state = 100; //-> exit init loop.
					#if MCA25_DEBUG 
					softuart_puts_progmem("mux. ");
					#endif
					
					}
					break;			

			default:
				break;
		}
	}
	#if MCA25_DEBUG 
	softuart_puts_progmem("ready"); //finished! CAM found & ready!");
	softuart_putnewline();
	#endif

	//cam is initialised:
	mca25_ready	= 1;

	return 0;
}

//send ok
void mca25_send_ok(void){
	mca25_puts_progmem("\r\r\nOK\r\n");
}

//read at command
unsigned char mca25_read_at_command(unsigned char *buffer){
	unsigned int cnt;

	//set timeout to t+8 seconds:
	clock_timeout_timer = 8;
	

	#if MCA25_DEBUG
	//softuart_puts_progmem("CAM : rx [");
	#endif
	for(cnt=0;cnt<MCA25_COMM_BUFFER_LEN;cnt++){
 		if (clock_timeout_timer == 0)
 				return 0;

		//wait for data in
		while (!(UCSRA & (1<<RXC))){
 			if (clock_timeout_timer == 0)
 				return 0;
		}
		
		buffer[cnt] = UDR;
		if (buffer[cnt] == '\r' || buffer[cnt] == '\n'){
			buffer[cnt] = '\0';
			break; //we have finished read.
		}
	}
	#if MCA25_DEBUG
	/*for(cnt=0;cnt<MCA25_COMM_BUFFER_LEN;cnt++){
		if (buffer[cnt] == 0)
			break;
		softuart_putc(buffer[cnt]);
	}
	softuart_puts_progmem("]\r\n");*/
	#endif
	return 1;
}

//read mux packet
unsigned char mca25_read_mux_packet(unsigned char *buffer, unsigned char skipcount){
	unsigned char data;
	unsigned char state = 0;
	unsigned char len = 0;
	unsigned char crc = 0;
	unsigned char bufpos = 0;
	unsigned char muxbufpos = 0;

	//set timeout to t+3 seconds:
	clock_timeout_timer = 3;

	while (clock_timeout_timer > 0){
		//wait for incoming data: (abort if timeout!)
		while (!(UCSRA & (1<<RXC))){
			if (clock_timeout_timer == 0){
				#if MCA25_DEBUG
				softuart_put_uint8(state);
				softuart_put_uint8(len);
				#endif
				return 0;
			}
		}
	
		data = UDR;
	
		if ((state == 4) || (state ==5 )){
			if (skipcount == 0){
				buffer[bufpos++] = data;
			}else{
				skipcount--;
			}
		}

		LED_STATUS_ON();

		switch (state){
			case 0:	// waiting for start flag
				if (data == 0xF9){
					bufpos = 0;
					state = 1;
				}
				break;

			case 1: // address
				if (data == 0xF9){
					bufpos = 0;
				}else{
					muxbufpos = 0;
					mca25_mux_buffer[0] = data;
					state = 2;
				}
				break;
	
			case 2:	// frame type
				if (data != 0xF9){
					state = 3;
					mca25_mux_buffer[1] = data;
					muxbufpos = 2;
				}else{
					//strange ... happens sometime!
					state = 1;
					bufpos = 0;
				}
				break;
	
			case 3:	// frame length
				len = (data >> 1);

				//limt length
				if (len>31)
					len = 31;

				mca25_mux_buffer[muxbufpos++] = data;

				if (len > 0)
					state = 4;
				else
					state = 5;
				break;
	
			case 4:	// package data
				if (muxbufpos<6)
					mca25_mux_buffer[muxbufpos++] = data;
				len--;
				if (len == 0)
					state = 5;
				break;
	
			case 5: // checksum
				crc = 0xFF;
				crc = crc8_calc_byte_rev0x07(crc, mca25_mux_buffer[0]);
				crc = crc8_calc_byte_rev0x07(crc, mca25_mux_buffer[1]);
				crc = crc8_calc_byte_rev0x07(crc, mca25_mux_buffer[2]);
				crc = 0xFF - crc;
				if (crc != data){
					// -> bad crc !

					// special handling of the "buggy" config message
					// \xF9\x03\xEF\x09\xE1\x07\x23\x0C\x01\xFB\xF9
					if ((buffer[0] == 0xE1) && (buffer[1] == 0x07) && (buffer[3] == 0x0C)){
						//softuart_puts_progmem("CAM : buggy cfg detected");
						//read extra char for buggy config message:
						while (!(UCSRA & (1<<RXC))){
							if (clock_timeout_timer == 0)
								return 0;
						}
						data = UDR;
						//softuart_puts_progmem("skipped extra data.\r\n");
						state = 6;
					}else{
						#if MCA25_DEBUG
						softuart_puts_progmem("CAM : bad crc ");
						softuart_put_uint8(crc);
						softuart_putc(':');
						softuart_put_uint8(data);
						softuart_putnewline();
						#endif
						state = 0;
					}
				}else{
						state = 6;
				}
				break;
	
			case 6: // flag
				if (data != 0xF9){
					#if MCA25_DEBUG
					softuart_puts_progmem("CAM : bad packet ?! ");
					softuart_put_uint8(data);
					softuart_putnewline();
					#endif
					state = 0;
				}else{
					LED_STATUS_OFF();
					return 1;
				}
				break;

			default:
				break;
		}
	}
	LED_STATUS_OFF();
	return 0;
}


//put progmem data with given length,
//see mca25.h for the mca25_puts_progmem(s) macro !!
void mca25_puts_progmem2(PGM_P pointer, unsigned int len){
	//softuart_put_uint16(len);
	while (len--){
		if (pgm_read_byte(pointer) == 0xF9)
			_delay_ms(1);
		mca25_uart_putc(pgm_read_byte(pointer++));
	
	}
}

//initialise 460kbaud
void mca25_uart_init_460800(void){
	_delay_ms(2);
	UBRRH=(F_CPU / (460800L * 16L) - 1)>>8;
	UBRRL=(F_CPU / (460800L * 16L) - 1)&0xFF;
}

//initialise uart
void mca25_uart_init_9600(void){
	UBRRH = (F_CPU / (9600 * 16L) - 1)>>8;
	UBRRL = (F_CPU / (9600 * 16L) - 1)&0xFF;

	//activate uart TX+RX
	UCSRB |= (1<<TXEN)|(1<<RXEN);
	//async 8N1
	UCSRC |= (1<<URSEL)|(3<<UCSZ0);
}

//initialise uart
void mca25_uart_init_19200(void){
	UBRRH = (F_CPU / (19200 * 16L) - 1)>>8;
	UBRRL = (F_CPU / (19200 * 16L) - 1)&0xFF;
}
