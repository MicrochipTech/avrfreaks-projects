/*******************************************************************/
/*          Mega8 MP3 Player (mega8 + VS1003 + SD)  V2.0           */
/*                                                                 */
/* Discription: Can play mp3/wma/wav/mid SF0 files in the root     */
/*              directery on the SD card which file system is FAT16*/
/*              or FAT32.                                          */
/*              It has six keys to control this player             */
/*              MODE: switch between repeat all (default),repeat 1 */
/*                    and shuffle                                  */
/*              PLAY/PAUSE:                                        */
/*              PREV: previous songs                               */
/*              NEXT: next songs                                   */
/*              UP  : volume up                                    */
/*              DOWN: volume down                                  */
/* Platform   : AVRStudio4.13 SP1 + WinAVR20070525                 */
/*              optimize -0s                                       */
/* Author     : bozai(Zhang Qibo)                                  */
/* E-mail     : sudazqb@163.com                                    */
/* MSN        : zhangqibo_1985@hotmail.com                         */
/* Date       : 2007-10-18                                         */
/*******************************************************************/
/*  V2.0                                                           */
/*  2007-11-09                                                     */
/*	now the songs may placed in any directory, but the             */
/*  total number of folder the card has should less than           */
/*  127 for we use 512Bytes EEPROM to stored the info              */

/*  You may set the macro FIX_DIRECTORY in FAT.h to choose whether */
/*  to enable this function                                        */
/*                                                                 */
/*  V1.9                                                           */
/*  2007-10-08                                                     */
/*  fix a bug (but it never works), so stupid I am,take & as &&    */ 
/*                                                                 */
/*  V1.8                                                           */
/*  2007-06-16                                                     */
/*  Still the bug of FAT, it's not resolved totally                */
/*                                                                 */
/*  V1.7                                                           */
/*  2007-05-21                                                     */
/*  Fix a serious bug of FAT                                       */
/*  Set the default mode to "Shuffle"                              */
/*  Because the mode button is inside the box                      */
/*  So I don't want to open the box when I turn on it              */
/*  If the button is easy to press of your mp3 player              */
/*  just change the macro below to:                                */
/*            0 = repet all    1 = repet one  2 = shuffle          */

#define DEFAULT_MODE   0

/*                                                                 */
/*  V1.6                                                           */
/*  2007-05-09                                                     */
/*  And a track function to make the shuffle a real shuffle        */
/*  Due to the limted RAM space of mega8. Maximum no repetition    */
/*  songs is 1024. And songs more than that will use randomly play */
/*  Special thanks to Ronald Ausloos (Belgium)'s advice            */ 
/*                                                                 */
/*  V1.5                                                           */
/*  2007-05-03                                                     */
/*  Change the behavior of the LEDs:                               */
/*                      VS1003 faild: LED1 blink                   */
/*                      SD faild    : LED2 blink                   */
/*                      FAT faild   : LED1 & LED2 blink alternatly */
/*                      File not found: Both LED blink             */
/*  Add read capacity function, fix the bug of FAT init            */
/*  Add slow start up code of VS1003                               */ 
/*                                                                 */                                                              
/*  V1.4                                                           */
/*  2007-05-02                                                     */
/*  Add enough delay when operate VS1003                           */ 
/*                                                                 */                                                              
/*  V1.3                                                           */
/*  2007-04-21                                                     */
/*  Modify the retry time of sd reset                              */
/*  Enable some code incase that when FAT initialize               */
/*  faild program can't jump out the loop                          */
/*  so the error LED can't light up                                */
/*  And light up both leds when FAT_Ini fails                      */
/*                                                                 */
/*  V1.2:                                                          */
/*  2007-04-04:                                                    */
/*  Add a macro of path                                            */
/*                                                                 */
/*  V1.1:                                                          */
/*  2007-02-25                                                     */
/*  Change the directory to C:\music\                              */
/*  A bug fixed                                                    */
/*  Modify some code to adjust new GCC compiler                    */
/*                                                                 */
/*  V1.0:                                                          */
/*  2006-12-03:                                                    */
/*  Original Version                                               */
/*******************************************************************/

#include<avr/io.h>
#include"MMC_SD/MMC_SD.h" //head files
#include"FAT/FAT.h"
#include"VS1003B/VS1003B.h"
#include<avr/pgmspace.h>

#define uint8 unsigned char
#define uint16 unsigned int
#define uint32 unsigned long

#if FIX_DIRECTORY
#define PATH (unsigned char *)("\\music")
#endif
//It's the path where the file placed
//Change it if you want to placed it to anoter foler
//The path should not have blanks, and each foler name's length should less than 8 with no extention

//diagnose and state indicate leds
//at start up this two led indicate error
//at normal they indicate the state of the MODE 
#define LED1_CON() DDRC|=_BV(PC5)
#define LED1_ON()  PORTC|=_BV(PC5)
#define LED1_OFF() PORTC&=~_BV(PC5)

#define LED2_CON() DDRB|=_BV(PB1)
#define LED2_ON()  PORTB|=_BV(PB1)
#define LED2_OFF() PORTB&=~_BV(PB1)

//keys
#define STOP _BV(PD2)
#define MODE _BV(PD3)
#define NEXT _BV(PD7)
#define UP   _BV(PD4)
#define DOWN _BV(PD6)
#define PREV _BV(PD5)

#define MP3 1
#define WMA 2
#define MID 3

//mode
#define REPET_ALL 0
#define REPET_ONE 1
#define RANDOM    2

extern uint16 SectorsPerClust;//??????????
extern uint16 FirstDataSector;//????????????????   //struct of file information
extern uint8  FAT32_Enable;

struct FileInfoStruct FileInfo;//????????

struct direntry MusicInfo;	//????????mp3????????		//the mp3 file item whichi will be played
uint16 totalsongs;			//????????????????			//total songs in the root directery on the SD card
uint8 type;					//????????					//file type

uint8 track[128];			//stroe the information of songs (bit set indicate songs has been played)

void ClearTrackInfo()		//cleare the array track[128]
{
	uint8 i;
	for(i=0;i<128;i++)track[i] = 0;
}

uint8 SetTrack(uint16 songs)//set the track bit, return 1 means the song has been played
{
	uint8 byte_offset;
	uint8 bit_offset;
	songs--;
	byte_offset = songs/8;
	bit_offset = songs%8;
	if(track[byte_offset] & (1<<bit_offset))return 1;
	else
	{
		track[byte_offset] |= 1<<bit_offset;
		return 0;
	}
}

void Delay(uint16 n)//????
{
	while(n--)asm("nop");
}

//Timer initialization offer seed of the srandom()
void Timer1_Initial()
{
 TCNT1H=0x00;
 TCNT1L=0x00;
 TCCR1B=0x01;//system clock;
 ICR1H=0xff;
 ICR1L=0xff;
}

void PlayMusic()//??????????????????????????????
{
	uint16 keylen;			//??????????		//for key processing
	uint16 count;			//????????			//data counting
	uint8 i;				//????????			//loop variable
	uint16 j;				//????????			//loop variable
	uint32 p;				//????????			//cluster
	uint32 totalsect;		//????????????????	//cotain the total sector number of a file
	uint16 leftbytes;		//????????			//cotain the left bytes number of a file //the last cluster usually not fully occupied by the file
	uint8 *buffer;			//????				//buffer
	uint32 sector;			//????				//recor the current sector to judge the end sector
	uint8 flag;				//????/????????		//flag of pause
	uint16 vol=DefaultVolume;//??????????????vs1003????????????????????	//default volume
	uint16 songs=1;			//??????????????	//play the fist songs by default
	uint8 mode=DEFAULT_MODE;//????????			//repet all by default
	uint16 songs_cnt = 0;					//how many songs have been played
	if(totalsongs==0)return;//??????????????????????	//if no music file return
	uint32 rand_val;
	Timer1_Initial();//??????????????????????????????????	//initialize the timer
	ClearTrackInfo();

next://??????????????????						//label for "goto"
	if(mode==RANDOM)//???????????? 				//if the mode is shuffle the songs
	{
		songs_cnt++;
		if(songs_cnt == totalsongs && totalsongs<1025)
		{
			ClearTrackInfo();
			songs_cnt = 0;
		}
		rand_val = TCNT1;
		Delay((random() && 0x00ff));
		rand_val <<= 16;
		rand_val += TCNT1;
		srandom(rand_val);
		if(totalsongs>1024)
		{
			songs = (uint16)(((random()/214749)*(uint32)totalsongs)/10000)+1;//????????????????	//create random song number
		}
		while(totalsongs<1025)
		{
			songs = (uint16)(((random()/214749)*(uint32)totalsongs)/10000)+1;//????????????????	//create random song number
			if(SetTrack(songs) == 0)break;
		}	
	}
	count=0;//??????	//clear count
	flag=1;
	while(count<2048 && (type != MID))//recommand 2048 zeros honoring DREQ befor soft reset
	{									//??????wma????????????????????????????????????//midi??????????
		if((VS1003B_PIN & _BV(VS1003B_DREQ))!=0)
		{
			for(j=0;j<32;j++)
			{
				VS1003B_WriteDAT(0x00);//????0	//fill 0
				count++;
			}
			if(count == 2047)break;
		}
	}
	VS1003B_SoftReset();//soft reset //in case of playing wma files//????????

#if FIX_DIRECTORY
	Search(PATH,&MusicInfo,&songs,&type);		//??????????????		//find the file
#else
	Search(&MusicInfo,&songs,&type);		//??????????????		//find the file
#endif

	p     = MusicInfo.deStartCluster+(((uint32)MusicInfo.deHighClust)<<16);//??????????	//the first cluster of the file
		
	totalsect = MusicInfo.deFileSize/512; //??????????			//calculate the total sectors
	leftbytes = MusicInfo.deFileSize%512; //????????????????	//calculate the left bytes	
	i=0;
	sector=0;
	
	while(1)
	{
		keylen=0;
    	for(;i<SectorsPerClust;i++)		//??????	//a cluster
		{
			buffer=malloc(512);
			FAT_LoadPartCluster(p,i,buffer);//??????????	//read a sector
			count=0;
			while(count<512)
			{
				if(flag==0){if(keylen){Delay(100);keylen--;}}
				else if(type == MID){if(keylen){Delay(100);keylen--;}}
				if((VS1003B_PIN & _BV(VS1003B_DREQ))!=0 && flag)	//??????????????  //send data  honoring DREQ
				{
					for(j=0;j<32;j++)			//??????32??????		//32 Bytes each time
					{
						VS1003B_WriteDAT(buffer[count]);
						count++;
					}
					if(keylen)keylen--;			//??????????			//for key processing
					if(sector == totalsect && count >= leftbytes)		//??????????????		//if this is the end of the file
					{
						if(type == MID)//waiting the midi file was decoded
						{//????mid????????????2048????
							count=0;
							while(count<2048)//recommand 2048 zeros honoring DREQ goto next songs
							{
								if((VS1003B_PIN & _BV(VS1003B_DREQ))!=0 )
								{
									for(j=0;j<32;j++)
									{
										VS1003B_WriteDAT(0x00);
										count++;
									}
									if(count == 2047)break;
								}
							}
						}
						i=SectorsPerClust;
						break;
					}//????????			//file ended
					if(count == 511){break;}//512????????????		//break if a sector was sent
				}
				if((PIND&STOP)==0 /*&& keylen==0*/)//??????????		//key PLAY/PAUSE
			    {
	 			    Delay(100);
					if(!(PIND&STOP))
					{
						while(!(PIND&STOP));
						 if(flag)flag=0;
						 else flag=1;
						 Delay(1000);
					 }
				 }
				else if(!(PIND&DOWN) && keylen==0)  //????-			//Volume down
	 			{
				    Delay(100);
	   			 	if(!(PIND&DOWN))  
					{
					keylen=200;
					   vol=vol+((uint16)(1<<8)+1);
					   if(vol>=0xFEFE) vol=0xFEFE; 
	   			       else VS1003B_WriteCMD(0x0b,vol);
					 }
	 			 }
				 else if(!(PIND&UP) && keylen==0) //????+			//Volume up
	  			 {
					 Delay(100);
				 	 if(!(PIND&UP)) 
					 {
					 keylen=200;
					   vol=vol-((uint16)(1<<8)+1);
					   if(vol<=0x0101) vol=0x0101;
					   else VS1003B_WriteCMD(0x0b,vol);
	   			     }
				  }
				 else if(!(PIND&NEXT)) //??????						next songs
	 			 {
					Delay(0x7fff);		
					if(!(PIND&NEXT))
					{
						Delay(0x7fff);
						if(!(PIND&NEXT))
						{
							while(!(PIND&NEXT));
							songs++;
							if(songs > totalsongs)songs=1;
							{
								free(buffer);
							//	Delay(0xffff);
								goto next;
							}
						}
					}
	  			 }
				 else if(!(PIND&PREV)) //??????						previous songs
	 			 {
					Delay(0x7fff);		
					if(!(PIND&PREV))
					{
						Delay(0x7fff);
						if(!(PIND&PREV))
						{
							while(!(PIND&PREV));
							if(songs == 1)songs=totalsongs;
							else songs--;
							{
								free(buffer);
							//	Delay(0xffff);
								goto next;
							}
						}
					}
	  			 }
				 else if((!(PIND&MODE)) && keylen==0) //????			//mode key
	 			 {
					Delay(100);		
					if(!(PIND&MODE))
					{
						keylen=0xffff;
						if(mode==REPET_ALL)
						{
							mode=REPET_ONE;
							LED1_ON();
							LED2_OFF();
						}
						else if(mode==REPET_ONE)//next mode is shuffle
						{
							mode=RANDOM;
							LED1_OFF();
							LED2_ON();
							srandom(((uint32)TCNT1)<<16);//????????????????
						}
						else 
						{
							mode=REPET_ALL;
							LED1_OFF();
							LED2_OFF();
						}
					}
	  			 }
			}
			sector++;
			free(buffer);
		}
		i=0;
		p=FAT_NextCluster(p);//????????????			//read next cluster
		if(p == 0x0fffffff || p == 0x0ffffff8 || (FAT32_Enable == 0 && p == 0xffff))//????????????????????	//no more cluster
		{
				if(mode==REPET_ALL)songs++;
				if(songs>totalsongs)songs=1;
				goto next;
		}
	}
}

//main function
int main()
{
	uint8 retry = 0;

	DDRD &= 0x03;//??????????
	PORTD |= 0xfc;
	
	LED1_CON();
	LED1_OFF();
	LED2_CON();
	LED2_OFF();


	OSCCAL = 0x00;//????RC????????	//in order to operate some low speed card the initialization should run at lowest speed

	Delay(0xffff);

	MMC_SD_Init();//??????spi??		//SPI initialize

	Delay(0xffff);

	if(VS1003B_Init())
	{
		//LED1_ON();//????VS1003	//config vs1003
		while(1)
		{
			LED1_ON();
			Delay(0xffff);
			LED1_OFF();
			Delay(0xffff);
		}
	}
	Delay(0xffff);//??????????????				//supply enough delay
//	Delay(0xffff);
//	Delay(0xffff);
//	Delay(0xffff);
//	Delay(0xffff);

	while(MMC_SD_Reset())//??????SD??					//sd card initialize
	{
		retry++;
		if(retry>20)
		{
		//	LED2_ON();
			while(1)
			{
				LED2_ON();
				Delay(0xffff);
				LED2_OFF();
				Delay(0xffff);
			}
		}
	}

	OSCCAL = 0xff;//????RC????????				//normal operation maximum the frequency

	Delay(0xffff);								//wait for stable

	if(FAT_Init())//?????????????? ????FAT16??FAT32	//initialize file system  FAT16 and FAT32 are supported
	{
	//	LED1_ON();
	//	LED2_ON();
	//	while(1);
		while(1)
		{
			LED1_ON();LED2_ON();
			Delay(0xffff);
			LED1_OFF();LED2_OFF();
			Delay(0xffff);
		}

	}

#if FIX_DIRECTORY
	Search(PATH,&MusicInfo,&totalsongs,&type);//???????????????? ????????????
#else
	SearchInit();
	Search(&MusicInfo,&totalsongs,&type);//????????????????
#endif

	//???????????????? ??Search("\\new\\mp3",&MusicInfo,&totalsongs,&type);
	//????????????????new??????????mp3????????????????

	//search the songs in the root directery on the SD card
	//You can also specify the directery where the songs are placed
	//eg: Search("\\new\\mp3",&MusicInfo,&totalsongs,&type);
	//		means search the file in the foler C:\new\mp3

	PlayMusic();//????????		//play songs
	while(1)
	{
		while(1)
		{
			LED1_ON();LED2_OFF();
			Delay(0xffff);
			LED2_ON();LED1_OFF();
			Delay(0xffff);
		}
	}
	return 0;
}
