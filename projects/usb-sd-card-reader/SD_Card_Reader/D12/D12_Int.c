#include "D12_INT.h"
#include "USB_MassStorage.h"

#include "MMC_SD.h"


CBW cbw;
CSW csw;

unsigned char DISK_INF[36]=                   //Disk infomation                   //磁盘信息
{
 0x00,
 0x00,//最高位表示是否是可移动设备  //MSB of this byte indicate that this is removable disk
 0x00,
 0x01,
 0x1F,
 0x00,0x00,0x00,

 '*',0xd5,0xc2,0xc6,0xe4,0xb2,0xa8,'*',//这一段可以自己添加内容，但长度不可变，编码为ascii 也可是gbk
//You can add infomation here, a ASCII string needed, but the length must be 8 
 
 0xd6,0xc6,0xd7,0xf7,0xb5,0xc4,0xbf,0xc9,0xd2,0xc6,0xb6,0xaf,0xb4,0xc5,0xc5,0xcc,//这一段可以自己添加内容，但长度不可变，编码为ascii 也可是gbk
//You can add infomation here, a ASCII string needed, but the length must be 16
 
 
 
 0x31,0x2E,0x30,0x31
};

//Dis capacity. The first four bytes is the max LBA address
//It will be reinitialized when SD card is ready
unsigned char DISK_CAPACITY[8]=                                  //磁盘容量//在SD初始化后会修改
{
 0x00,0x0F,0x1C,0xF0,
 0x00,0x00,0x02,0x00
};

//the requied data of MODE SENSE
unsigned char SENSE[0x12]=                                      //模式探测返回数据
{
0x70, 0x00, 0x05, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00
};

//返回CSW包
void Send_CSW(U32 DataResidue,U8 status)
{
	csw.dCSWSignature = 0x53425355; //csw包标志
	csw.dCSWTag=cbw.dCBWTag;		//主机发过来的tag
 	csw.dCSWDataResidue=DataResidue;
 	csw.bCSWStatus=status;
 	while(D12_Select_Endpoint(5));
 	D12_Write_Endpoint(5, (U8 *)&csw, 13);//定长度 13字节
}

//SCSI读操作
void read_10()
{
	U32 Byte_Count;//总字节数
	U32 LBA;		//LBA地址
	U8 j=0;


//得到LBA地址	
	LBA = 0;
	LBA += cbw.CBWCB[2];
	LBA <<= 8;
	LBA += cbw.CBWCB[3];
	LBA <<= 8;
	LBA += cbw.CBWCB[4];
	LBA <<= 8;
	LBA += cbw.CBWCB[5];

//得到传输字节数	
	Byte_Count = 0;
	Byte_Count+=cbw.CBWCB[7];
	Byte_Count<<=8;
	Byte_Count+=cbw.CBWCB[8];
	Byte_Count<<=9;
 
//传输数据   //一个while循环为读一个扇区 
   while(1)	
   {
   		//以下为SD读一扇区的前导操作
		if(MMC_SD_SendCommand(17, LBA<<9)!=0x00){return;} 
		SPI_CS_Assert();
		while(SPI_WriteByte(0xff) != 0xfe);//等数据的开始	//wait to start recieve data
		LBA++;
		for(j=0;j<8;j++)
		{
			D12_Write_Endpoint_SD(5);//写端点，与SD卡同时操作//每次64字节
			Byte_Count-=64;
			if(Byte_Count==0)
			{
				//读一扇区结尾的伪crc
				SPI_WriteByte(0xff);//伪crc
				SPI_WriteByte(0xff);
	
				SPI_CS_Deassert();

				return;//结束返回
			}
		}
		//读一扇区结尾的伪crc
		SPI_WriteByte(0xff);//伪crc
		SPI_WriteByte(0xff);
	
		SPI_CS_Deassert();
	}
}

//SCSI写操作
void write_10()
{
	U32 Byte_Count;//写字节数
	U32 LBA;		//写LBA地址
	U8 j=0;

	U8 buffer[512];	//实践证明写SD卡操作不可像读操作那样操作，否则会出错 所以用缓冲区来做 等收到512字节再写

//LBA地址
	LBA = 0;
	LBA += cbw.CBWCB[2];
	LBA <<= 8;
	LBA += cbw.CBWCB[3];
	LBA <<= 8;
	LBA += cbw.CBWCB[4];
	LBA <<= 8;
	LBA += cbw.CBWCB[5];

//总字节数	
	Byte_Count = 0;
	Byte_Count+=cbw.CBWCB[7];
	Byte_Count<<=8;
	Byte_Count+=cbw.CBWCB[8];
	Byte_Count<<=9;

//写操作 1个while循环写1扇区   
	while(1)	
	{
		D12_Read_Endpoint(4, &buffer[j*64], 64);//正常端点读操作
		j++;
		Byte_Count-=64;
		if(j==8)//计满512字节后一起写到sd卡内
		{
			j=0;
			MMC_SD_WriteSingleBlock(LBA++, buffer);
		}
		if(Byte_Count==0)return;//结束返回
	}
}

//标准请求的函数指针
U8 (*Stand_Device_Request[])(U8 *SetupPacket) = 
{
	Get_Status,
	Clear_Feature,
	Reserved,
	Set_Feature,
	Reserved,
	Set_Address,
	Get_Descriptor,
	Reserved,
	Get_Configuration,
	Set_Configuration,
	Get_Interface,
	Set_Interface,
	Reserved,
	Reserved,
	Reserved,
	Reserved
}; 


/**************************************/
void USB_Delay(U16 Time)
{
	while(Time--)asm("nop");
}



/*****************初始化USB********************/
U8 USB_Init()
{
//	D12_PORT_INI();
	USB_Delay(20000);

	if(D12_Read_Chip_ID()!=0x1210 )
		return 0;

	D12_Set_DMA(MyD12DmaCfg);
	if(D12_Get_DMA()!=MyD12DmaCfg)
		return 0;

	D12_Set_Mode(MyD12EpCfgOff, D12Pll24M);
	USB_Delay(20000);
	USB_Delay(20000);
	D12_Set_Mode(MyD12EpCfgOn, D12Pll24M);

	return 1;
}


/********************** D12 interrupt process ******************/
void D12Ep0IntProc()
{
	U8 SetupPacket[8];
	U8 i = 0;
	
	Clear_Remain_Descriptor_Flag();
	//printf("\nEndpoint0 Process!");
	if(D12_Read_Last_Transaction_Status(0)&0x20)
	{	

		if(D12_Read_Endpoint(0, SetupPacket, 8)==8)
		{
			D12_Ack_Endpoint(0);
			D12_Ack_Endpoint(1);
			
			//for(i=0; i<8; i++)
				//printf("%x,", SetupPacket[i]);
			if(SetupPacket[0] == 0xa1 && SetupPacket[1] == 0xfe)
			{
				D12_Write_Endpoint(1,&i,1);
			}
			if(!Stand_Device_Request[SetupPacket[1]&0xf](SetupPacket));//调用相应的标准请求响应函数
				return;
		}
	}
					
	D12_Set_Endpoint_Status(0, D12EpStall);
	D12_Set_Endpoint_Status(1, D12EpStall);		
}

void D12Ep1IntProc()
{
	U8 i;
	//printf("\nEndpoint1 Process!\n");
	i = D12_Read_Last_Transaction_Status(1);
	if(Remain_Descriptor_Flag())
		Send_Descriptor();
		
}

void D12Ep2IntProc()
{
//	printf("\nEndpoint2 Process!\n");
	D12_Set_Endpoint_Status(2, D12EpStall);
}

void D12Ep3IntProc()
{
//	printf("\nEndpoint3 Process!\n");
	D12_Set_Endpoint_Status(3, D12EpStall);
}

void D12Ep4IntProc()
{
	//printf("\nEndpoint4 Process!\n");
	D12_Read_Last_Transaction_Status(4);	
	D12_Read_Endpoint(4, (U8 *)&cbw, Endpoint2_Packet_Size);
	if(cbw.dCBWSignature != 0x43425355)return;			//cbw包标志

	if(cbw.bmCBWFlags&0x80)								//写操作
	{
		switch(cbw.CBWCB[0])
		{
			case              Read_10: read_10();Send_CSW(0x00,SUCCESS);break;
			case              Inquiry: D12_Write_Endpoint(5,DISK_INF,36);Send_CSW(0x00,SUCCESS); break;
  		  	case        Read_Capacity: D12_Write_Endpoint(5,DISK_CAPACITY,0x08);Send_CSW(0x00,SUCCESS);break;
  	 	 	case Read_Format_capacity: D12_Write_Endpoint(5,0x00,0x00);Send_CSW(cbw.dCBWDataTransgerLength,FAIL);break;
   		 	case        Request_Sense: D12_Write_Endpoint(5,SENSE,0x12);Send_CSW(0x00,SUCCESS);break;
		 	case                 0x1a: D12_Write_Endpoint(5,0x00,0x00);Send_CSW(cbw.dCBWDataTransgerLength,FAIL);break;
   		 	default                  : D12_Write_Endpoint(5,0x00,0x00);Send_CSW(cbw.dCBWDataTransgerLength,FAIL);break;
		}
	}
	else												//读操作
	{
		switch(cbw.CBWCB[0])
		{
			case        Write_10: write_10();Send_CSW(0x00,SUCCESS);break;
			case Test_Unit_Ready: Send_CSW(0x00,SUCCESS);break; 
			case          Verify: Send_CSW(0x00,SUCCESS);break;
			default             : Send_CSW(cbw.dCBWDataTransgerLength,FAIL);break;
		}
	}
}

void D12Ep5IntProc()
{
	//printf("\nEndpoint5 Process!\n");
	D12_Read_Last_Transaction_Status(5);//所有的数据传输都已在ep4内完成，该程序没有采用中断机制
}

void D12BusRstProc()
{
	//printf("\nBus Reset!");
	D12_Clear_Buffer();
	D12_Enable_Buffer();
	D12_Set_Endpoint_Enable(1);
}

void D12SuspChgProc()
{
}
