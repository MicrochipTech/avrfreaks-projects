/********************************************************/
/*             USB SD Card Reader V1.0                  */
/* Description:                                         */
/*           A SD Card Reader using Atmega168 PDIUSBD12 */
/* Author:  Qibo Zhang (bozai)                          */
/* Email :  sudazqb@163.com                             */
/* MSN   :  zhangqibo_1985@hotmail.com                  */
/********************************************************/
/* ChangeLog:                                           */
/*           2007-03-10 : Original version              */
/********************************************************/

#include<avr/io.h>
#include"D12/D12_Operation.h"
#include"D12/CHAP_9.h"
#include"D12/D12_Int.h"
#include"MMC_SD/MMC_SD.h"

#define LED_DDR  DDRB
#define LED_PORT PORTB
#define LED_BIT  1

#define LED_ON()  LED_PORT |=  1<<LED_BIT
#define LED_OFF() LED_PORT &= ~(1<<LED_BIT)

extern unsigned char DISK_CAPACITY[8];





int main()
{
	U8 i;
	U32 Cap;
	U8 retry = 0;

	USB_Delay(0xffff);
	
	D12_PORT_INI();
	
	MMC_SD_Init();
	LED_DDR |= 1<<LED_BIT;
	while(MMC_SD_Reset())// retry 100 times
	{
		retry++;
		if(retry>100)break;
	}
	if(retry<100)//if success
	{
		//LED_ON();

		//读SD卡容量
		Cap = (MMC_SD_ReadCapacity()/512)-1;//读到的是总容量，不是能寻址到的扇区地址，所以要减1处理
		DISK_CAPACITY[0] = ((U8 *)(&Cap))[3];//转成大端格式
		DISK_CAPACITY[1] = ((U8 *)(&Cap))[2];
		DISK_CAPACITY[2] = ((U8 *)(&Cap))[1];
		DISK_CAPACITY[3] = ((U8 *)(&Cap))[0];

		if(D12_Read_Chip_ID()==0x1210)
		{
			//puts("Find Pdiusbd12, chip id = 0x1210");
			
			USB_Init();
			while(1)
			{
				while(!D12_INT) //检测中断
				{
					//PORTB |= 0x02;
					i = D12_Read_Interrupt_Register()&0xff;
					UsbIntProc(i);			
				}
			}
		}
		else ;
	}
//	LED_ON();
	while(1);
	return 0;
}
