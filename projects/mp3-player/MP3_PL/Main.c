#include "main.h"
#include "timers.h"
#include "lcd.h"
#include "mmc.h"
#include "mmc_file.h"
#include "spi.h"
#include "i2c.h"
#include "stapatch.h"

#include <stdio.h>
#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#define STA_ADRESS 0b1000011
#define STA_BUFFERSIZE 512
#define MUTE_DELAY 500


#define UP 0x0C
#define FUNC 0x0A
#define DOWN 0x06
#define NOKEY 0x0E
#define LONGPRESS 2000  //longpress time in ms
#define DIS_TIMEOUT 3000
#define MENUTIME 5000
#define ID3_SCROLL 750
#define LCD_LINE 24		//length of LCD 

enum modetypes {IDLE,BROWSE,PLAY,BASS,TREBLE,VOLUME,MENU};
enum playmodes {ALL,SINGLE,RANDOM};

const prog_char menutext[][10] ={"Card?","Browse","Play","Bass","Treble","Volume","Menu"};

byte pol,pha;
word bufferindex;

struct {	signed char vol;
			signed char bass;
			signed char treble;
			byte init;
		} audio;
		

struct {
			byte sec;
			byte min;
		} playtime;


void init_io()
{
	DDRA=0xFF;
	DDRB=0b10110001;
	PORTB=0x1E;			//activate pullups for /CS and keys
	DDRC=0b11000000;
	PORTC=0x40;			//PU for /sta_reset
	DDRD=0b11111011;
	PORTD=0x06;			//PU for /sta_req, DAC off
}


void sta_on()
{
	PORTD&=0xEB;
}


void sta_off()
{
	PORTD|=0x10;
}


void aud_on()
{
	PORTD|=0x20;
	PORTD&=~0x02;

}


void aud_off()
{	PORTD|=0x02;
	PORTD&=0xDF;
}
void aud_mute()
{
	PORTD|=0x01;
	i2c_transmit(STA_ADRESS,0x14,1);
}


void aud_demute()
{
	PORTD&=0xFE;
	i2c_transmit(STA_ADRESS,0x14,0);

}



byte init_card ()		//negotiate SPI Mode
{	byte error,eof,mode;		

	pha=0;
	PORTB|=0x01;	//switch off card supply
	spi_off();
	PORTB&=0x0F;	//switch off card data lines
	delay(300);		
	PORTB&=0xFE;	//switch on card
	wdt_reset();
	delay(200);		//wait after switch on
		
	mode=0;
	
	do
	{	pol=0;
		do
		{	error=mmc_init(pol,pha);
			wdt_reset();
			if (!error)
				error=mmc_boot();
		}
		while (error&&(++pol<2));
	}
	while (error&&(++pha<2));
	return error;
}



void set_volume ()
{
	if (audio.vol<0)
		audio.vol=0;
	if (audio.vol>96)
		audio.vol=96;
	i2c_transmit (STA_ADRESS,0x46,audio.vol);
	delay (4);
	i2c_transmit (STA_ADRESS,0x48,audio.vol);

}



void set_sound ()
{	if (audio.bass<-10)
		audio.bass=-10;
	if (audio.bass>10)
		audio.bass=10;
	if (audio.treble<-10)
		audio.treble=-10;
	if (audio.treble>10)
		audio.treble=10;
	i2c_transmit (STA_ADRESS,0x7B,audio.treble);
	delay (2);
	i2c_transmit (STA_ADRESS,0x7C,audio.bass);
}
		
	


byte init_sta()
{	byte error;
	word counter,datapointer;
	
	PORTC&=~0x40;		//assert reset
	sta_on();
	delay (100);
	PORTC|=0x40;		//release reset
	delay (50);
	
	datapointer=(word)&patch[0];
	counter=0;
	do
	{	
		error=i2c_transmit (STA_ADRESS,pgm_read_byte(datapointer),pgm_read_byte(datapointer+1));			//load patch to sta013
		datapointer+=2;
		counter+=2;
	}
	while ((counter<sizeof(patch))&&(!error));
	return error;
}	


void lcd_on()
{
	PORTA|=0x01;	//switch on supply
	delay (50);
	lcd_init();
}



void lcd_off()
{
	PORTA&=~0x01;	//switch off supply for LCD
}


byte wait_card()
{	byte delay_timer,error;
	delay_timer=start_timer(3000);
	do
	{	error=mmc_boot();
		wdt_reset();
		if (error)
			error=init_card();
	}
	while (error&&(!timer_stopped(delay_timer)));
	stop_timer(delay_timer);
	return error;
}				




int main()
{	byte data,error,eof,play,newtrack,id3_disp,mute,menu_timer,end_play;
	byte shortkey,lastkey,currentkey,new_aud,aud_timer,vol_timer,id3_timer,delay_timer,sec_timer;
	byte dis_timer,new_dis,maxmode,menuon,scrolldir;
	signed char id3_index;
	char id3_song[65],output[32],filename[14],idlestring[LCD_LINE+2];
	int entrynumber,startcluster,startentry,counter;
	unsigned long int bytes_sent,filesize;
	enum modetypes mode,oldmode,menumode;
	enum playmodes playmode;
	
	init_io();

	main_timer_init();
	sei();
	
	aud_mute();
	i2c_init();
	lcd_on();
	
	sprintf_P(output,PSTR("INIT..."));
	lcd_write_string (0,0,output,1);
	eeprom_read_block (&audio,0,sizeof (audio));
	if (audio.init!=1)
	{	audio.vol=20;
		audio.bass=0;
		audio.treble=0;
		audio.init=1;
	}
	
	entrynumber=-1;
	shortkey=NOKEY;
	lastkey=NOKEY;
	new_dis=1;
	play=0;
	mute=1;
	mode=IDLE;
	oldmode=PLAY;
	menumode=IDLE;
	newtrack=0;
	playmode=ALL;
	aud_timer=NUM_TIMERS;
	dis_timer=NUM_TIMERS;
	id3_timer=NUM_TIMERS;
	sec_timer=NUM_TIMERS;
	menu_timer=NUM_TIMERS;
	scrolldir=UP;
	id3_disp=0;
	menuon=1;
	
	if (!init_sta())
	{	sprintf_P(output,PSTR("STA OK"));
		lcd_write_string(0,1,output,1);
		set_volume();
		set_sound();
		aud_mute();
		aud_on();
		
		wdt_enable(WDTO_2S);
		
		while (1)
		{	
			//GETTING KEYS
			currentkey=PINB&0x0E;
			if (currentkey==NOKEY)
				shortkey=lastkey;
			lastkey=currentkey;
			
			wdt_reset();
			
			if (play)
			{	
				if (timer_stopped(sec_timer))
				{	sec_timer=start_timer(1000);
					if (++playtime.sec>59)
					{	playtime.sec=0;
						++playtime.min;
					}
					if (mode==PLAY)
					{	sprintf_P(idlestring,PSTR("Play %02u:%02u"),playtime.min,playtime.sec);
						new_dis=1;
					}
				}
			
				if (bufferindex>=STA_BUFFERSIZE)
				{	spi_init (pol,pha,1,0,1,0);	//switch SPI to card 4MHz
					eof=mmc_read_sector();
					bufferindex=0;
				}
				
				if (mute)
				{	if (bytes_sent>4096)
					{	mute=0;
						aud_demute();						
					}
				}
				
				if (eof)
				{	aud_mute();
					mute=1;
					delay(1000);
					error=wait_card();
					if (!error)
					{	switch (playmode)
						{	case ALL:		entrynumber++;
											newtrack=1;
											play=0;
											break;
							case SINGLE:	play=0;
											break;
							case RANDOM:	//tb implemented
											newtrack=1;
											break;
						}
						new_dis=1;
					}
					else
					{	mode=IDLE;
						menuon=1;
						play=0;
					}
				}
			}


			
			if (newtrack)
			{	aud_mute();
				mute=1;
				bytes_sent=0;
				memset (&playtime,0,sizeof(playtime));
				memset (filename,0,sizeof(filename));
				if (!mmc_dir (filename,&startcluster,&filesize,"mp3",&entrynumber))
				{	new_dis=1;
					strcpy_P(idlestring,menutext[mode]);
					error=mmc_open_file (filename,READ);
					if (error)
						play=0;
					else
					{	play=1;
						newtrack=0;
						bufferindex=STA_BUFFERSIZE;	//force reload						
						id3_index=0;
						id3_disp=!get_id3 (output,id3_song);
						if (id3_disp)
						{	strcat_P(id3_song,PSTR(" -- "));
							strcat(id3_song,output);
						}
						else
							strcpy (output,filename);
						mmc_open_file (filename,READ);			//necessary to restore file structure after reading id3
						stop_timer (sec_timer);
						sec_timer=start_timer(1000);
						menuon=1;
					}

				}
				else
				{	newtrack=1;
					entrynumber=0;
				}
			}



	
 
			switch (mode)
			{	case IDLE:	if (menuon)
							{	new_dis=1;
								menuon=0;
								strcpy_P(output,PSTR("                "));
							}
							if (!init_card())
							{	mode=BROWSE;
								entrynumber=-1;
								startentry=0;							
							}
							break;
		
				case PLAY:	play=1;

							
							if (menuon)
							{	if (!id3_disp)
									strcpy (output,filename);
								else 
								{	if (strlen(id3_song)<=LCD_LINE)
										strcpy (output,id3_song);
								}
								menuon=0;
								new_dis=1;							
							}
							if ((id3_disp)&&(strlen(id3_song)>LCD_LINE))
							{	if (timer_stopped(id3_timer))
								{	id3_timer=start_timer(ID3_SCROLL);
									if (scrolldir==UP)
									{	strncpy (output,&id3_song[id3_index++],LCD_LINE);
										if (id3_index>(strlen(id3_song)-LCD_LINE))
										{	scrolldir=DOWN;
											stop_timer(id3_timer);
											id3_timer=start_timer(ID3_SCROLL*2);
											id3_index--;
										}
									}
									else
									{	strncpy (output,&id3_song[id3_index--],LCD_LINE);
										if (id3_index<0)
										{	scrolldir=UP;
											stop_timer(id3_timer);
											id3_timer=start_timer(ID3_SCROLL*10);
											id3_index++;
										}
									}
									new_dis=1;
								}
							}
							if (shortkey==UP)
							{	audio.vol-=2;
								menuon=1;
							}
							if (shortkey==DOWN)
							{	audio.vol+=2;
								menuon=1;
							}
							if (menuon)
							{	set_volume();
								sprintf_P(output,PSTR("-%udB"),audio.vol);
								menuon=0;
								new_dis=1;
								mode=VOLUME;
								stop_timer(dis_timer);
								dis_timer=start_timer(DIS_TIMEOUT);
							}
							break;	
				
				case VOLUME:if (shortkey==UP)
							{	audio.vol-=2;
								menuon=1;
							}
							if (shortkey==DOWN)
							{	audio.vol+=2;
								menuon=1;
							}
							if (menuon)
							{	set_volume();
								sprintf_P(output,PSTR("-%udB"),audio.vol);
								menuon=0;
								new_dis=1;
								stop_timer(dis_timer);
								dis_timer=start_timer(DIS_TIMEOUT);
							}
							if (timer_stopped(dis_timer))
							{	aud_timer=start_timer(2000);
								new_aud=1;
								mode=PLAY;
								menuon=1;
							}
							
							break;
				
				case MENU:	if (play)
								maxmode=TREBLE;
							else
								maxmode=PLAY;
							switch (shortkey)
							{	case UP:	if (++menumode>maxmode)
												menumode=BROWSE;
											menuon=1;
											stop_timer(menu_timer);
											menu_timer=start_timer(MENUTIME);
											break;
								case DOWN:	if (--menumode<BROWSE)
												menumode=maxmode;
											menuon=1;
											stop_timer(menu_timer);
											menu_timer=start_timer(MENUTIME);
											break;
							
							}
							if (menuon)
							{	strcpy_P(output,menutext[menumode]);
								new_dis=1;
								menuon=0;
							}
							if (timer_stopped(menu_timer))
							{	mode=PLAY;
								menuon=1;
							}
							break;
				case BASS:	if (shortkey==UP)
							{	audio.bass+=1;
								menuon=1;
							}
							if (shortkey==DOWN)
							{	audio.bass-=1;
								menuon=1;
							}
							
							if (menuon)
							{	set_sound ();
								sprintf_P(output,PSTR("%i"),audio.bass);
								new_dis=1;
								menuon=0;
							}
							break;
				case TREBLE:if (shortkey==UP)
							{	audio.treble+=1;
								menuon=1;
							}
							if (shortkey==DOWN)
							{	audio.treble-=1;
								menuon=1;
							}
							
							if (menuon)
							{	set_sound ();
								sprintf_P(output,PSTR("%i"),audio.treble);
								new_dis=1;
								menuon=0;
							}
							break;
				case BROWSE:play=0;
							if (!mute)
							{	mute=1;
								aud_mute();
								if(wait_card())
									mode=IDLE;
							}
							startentry=entrynumber;
							error=(entrynumber<0);
							if (shortkey==UP)
							{	entrynumber++;
								memset(filename,0,sizeof(filename));
								error= mmc_dir (filename,&startcluster,&filesize,"mp3",&entrynumber);
								menuon=1;
							}
							if (shortkey==DOWN)
							{	counter=1;
								do
								{	entrynumber-=counter++;
									error=(entrynumber<0);
									memset(filename,0,sizeof(filename));
									if (!error)
										error=mmc_dir (filename,&startcluster,&filesize,"mp3",&entrynumber);								
								}
								while ((!error)&&(entrynumber==startentry));
								menuon=1;						
							
							}
							if (error)
							{	error=wait_card();
								if (!error)
								{	entrynumber=startentry;
									memset(filename,0,sizeof(filename));
									error=mmc_dir (filename,&startcluster,&filesize,"mp3",&entrynumber);
									menuon=1;
								}
								if (error)
								{	mode=IDLE;
									menuon=1;
								}

							}

							if ((menuon)&&(mode!=IDLE))
							{	while(mmc_open_file(filename,READ));
								id3_disp=!get_id3 (output,id3_song);
								if (id3_disp)
								{	strncpy (idlestring,output,LCD_LINE);
									strncpy (output,id3_song,LCD_LINE);
								}
								else
								{	strcpy (output,filename);
									strcpy_P (idlestring,menutext[mode]);
								}
								new_dis=1;
								menuon=0;
							}
						

			}
			
			
			if (shortkey==FUNC)
			{	switch (mode)
				{	case PLAY:		sprintf_P(output,menutext[mode]);
									mode=MENU;
									menumode=PLAY;
									stop_timer(menu_timer);
									menu_timer=start_timer(MENUTIME);
									break;
					case MENU:		mode=menumode;
									menuon=1;
									break;
					case TREBLE:
					case BASS:		stop_timer (aud_timer);
									aud_timer=start_timer (2000);
									new_dis=1;
									new_aud=1;
									menuon=1;
									menumode=mode;
									mode=MENU;
									stop_timer(menu_timer);
									menu_timer=start_timer(MENUTIME);
									break;
					case BROWSE:	newtrack=1;
									mode=PLAY;
									menuon=1;
									break;
					case VOLUME:	stop_timer(dis_timer);
									aud_timer=start_timer(2000);
									new_aud=1;
									mode=PLAY;
									menuon=1;
									break;
									
				
				}
			}
			
			
			
			if (play)
			{	if (!(PIND&0x04))				//STA request?
				{	spi_irq_disable();
					spi_init (0,1,1,0,1,0);	//switch SPI to STA 4MHz
					PORTD|=0x08;				//select STA
					do
					{	spi_master_transfer_byte (sector[bufferindex++]);
						bytes_sent++;
						end_play=((file.filesize-bytes_sent)<129);
					}
					while ((bufferindex<STA_BUFFERSIZE)&&(!(PIND&0x04))&&(!end_play));
					PORTD&=~0x08;				//deselect STA
				}
			}
		
			if ((mode!=oldmode)||newtrack)
			{	strcpy_P(idlestring,menutext[mode]);
				new_dis=1;
				oldmode=mode;
			}
			
			
			
			
			if (new_dis)
			{	
				if ((PIND&0x04)||(mode==BROWSE)||(mode==IDLE))
				{	{	lcd_cls();
						lcd_write_string (0,0,idlestring,1);
						lcd_write_string (0,1,output,1);

						new_dis=0;
					}
				}
			}
			
			if (new_aud)
			{	if(PIND&0x04)
				{	if (timer_stopped(aud_timer))
					{	eeprom_write_block (&audio,0,sizeof (audio));
						new_aud=0;
					}
				}
			}
		}
	}	
	return 0;
}


