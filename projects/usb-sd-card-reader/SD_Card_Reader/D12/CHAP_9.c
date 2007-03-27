#include "CHAP_9.h"


static U8 USB_Remote_Wake;
static U8 USB_Config;

static U8  USB_Send_Descriptor_Flag;
static int send_remain;
static U8 *send_ptr;

//Language ID
unsigned char LANGUAGE_ID[4]={0x04,0x03,0x09,0x04};

//可以添加自己的信息，如果长度不是18字节，需修改第一字节的值 //内容必须是unicode码
//You can add your own infomation here, the length of the array should be the same value of the first data
//The second data is not allowed modified
//The rest is yours
//The string must unicode string, not ASCII or others.
//Here is a example
//unsigned char sign name[42]=
//{42,0x03,'T',0x00,'h',0x00,'i',0x00,'s',0x00,' ','0x00,'i',0x00,'s',0x00,' ',0x00,'J',0x00,'a',0x00,'c',0x00,'k',0x00,''',0x00,'s',0x00,' ',0x00,'U',0x00,'d',0x00,'i',0x00,'s',0x00,'k',0x00}
//The infomation said: This is Jack's Udisk
unsigned char sign_name[18]=
{18,0x03,0xe0,0x7a,0x76,0x51,0xe2,0x6c,0x84,0x76,0xfb,0x79,0xa8,0x52,0xc1,0x78,0xd8,0x76};


static struct
{
	USB_Device_Descriptor       DevDesc;
	USB_Configuraton_Descriptor CfgDesc;
	USB_Interface_Descriptor    ItfDesc;
	USB_Endpoint_Descriptor     Endp4Desc;
	USB_Endpoint_Descriptor     Endp5Desc;
}ThisDevDesc =
				{
					{
						USB_Device_Descriptor_Size,
						USB_Device_Descriptor_Type,
						USB_11,
						Default_DeviceClass,
						Default_DeviceSubClass,
						Default_DeviceProtocol,
						Endpoint0_Packet_Size,
						Philip_IdVendor,
						Default_IdProduct,
						Default_BcdDevice,
						0,
						2,
						No_Index,
						1
					},//设备描述符 device discriptor
					{
						USB_Configuration_Descriptor_Size,
						USB_Configuration_Descriptor_Type,
						USB_Total_Configuration_Descriptor_Size,
						1,
						1,
						0,
						Bus_Power,
						PowerConsumption_Nma(400),
					},//配置描述符 configuation discriptor
					{
						USB_Interface_Descriptor_Size,
						USB_Interface_Descriptor_Type,
						0,
						0,
						Endpoint_Count,
						UsbClassMassStorage,
						UsbSubClassScsi,
						UsbProtocolBulk,
						0
					},//接口描述符 interface discriptor
					{
						USB_Endpoint_Descriptor_Size,
						USB_Endpoint_Descriptor_Type,
						Endpoint_In(2),
						UsbEndpBulk,
						Endpoint2_Packet_Size,
						10
					},//端点2描述符 endpoint 2 discriptor
					{
						USB_Endpoint_Descriptor_Size,
						USB_Endpoint_Descriptor_Type,
						Endpoint_Out(2),
						UsbEndpBulk,
						Endpoint2_Packet_Size,
						10
					}//端点2描述符endpoint 2 discriptor					
				};


/****************** standard device request ****************/
U8 Get_Status(U8 *SetupPacket)
{
	U8 temp;
	//printf("\nGet Status Request\n");
	if(SetupPacket[2]|SetupPacket[3]|SetupPacket[5]|SetupPacket[7])
		return 1;
	if(SetupPacket[6]!=2)
		return 1;
	if(SetupPacket[0]==0x80)
	{
		if(SetupPacket[4])
			return 1;
		SetupPacket[0] = USB_Remote_Wake?1:0;
		SetupPacket[1] = 0;
	}
	else
	if(SetupPacket[0]==0x81)
	{
		if(SetupPacket[4])
			return 1;
		SetupPacket[0] = 0;
		SetupPacket[1] = 0;		
	}
	else
	if(SetupPacket[0]==0x82)
	{
		temp=D12_Select_Endpoint((SetupPacket[4]*2)|((SetupPacket[4]&0x80)?1:0));
		SetupPacket[1] = (temp&2)?1:0;
		SetupPacket[0] = 0;
	}
	else
		return 1;

	D12_Write_Endpoint(1, SetupPacket, 2);
	return 0;		
}

U8 Clear_Feature(U8 *SetupPacket)
{
	//printf("\nClear Feature Request\n");
	if(SetupPacket[0]&0x80)
		return 0;
	if(SetupPacket[3]|SetupPacket[5]|SetupPacket[6]|SetupPacket[7])	
		return 0;
		
	if(SetupPacket[0]&0x1f)
	{
		if((SetupPacket[0]&0x1f)!=2)
			return 0;
		D12_Set_Endpoint_Status((SetupPacket[4]*2)|((SetupPacket[4]&0x80)?1:0),0);
	}
	else
	{
		if(SetupPacket[4])
			return 0;
		USB_Remote_Wake = 0;
	}
	
	D12_Write_Endpoint(1, SetupPacket, 0);
	return 1;
}

U8 Set_Feature(U8 *SetupPacket)
{
	//printf("\nSet Feature Request\n");
	if(!(SetupPacket[0]&0x80))
		return 0;
	if(SetupPacket[3]|SetupPacket[5]|SetupPacket[6]|SetupPacket[7])
		return 0;
		
	if(SetupPacket[0]&0x1f)
	{
		if((SetupPacket[0]&0x1f)!=2)
			return 0;
		D12_Set_Endpoint_Status((SetupPacket[4]*2)|((SetupPacket[4]&0x80)?1:0),1);
	}
	else
	{
		if(SetupPacket[4])
			return 0;
		USB_Remote_Wake = 1;
	}
	
	D12_Write_Endpoint(1, SetupPacket, 0);
	return 1;
}

U8 Set_Address(U8 *SetupPacket)
{
	//printf("\nSet Address Request\n");
	if(SetupPacket[0]|SetupPacket[3]|SetupPacket[4]|SetupPacket[5]|SetupPacket[6]|SetupPacket[7])
		return 0;
		
	D12_Set_Address_Enable(SetupPacket[2],1);
	D12_Write_Endpoint(1, SetupPacket, 0);
	return 1;	
}

void Send_Descriptor(void)
{
	//U8 i;
	//printf("\nSend Descriptor:");
	USB_Send_Descriptor_Flag = send_remain>=Endpoint0_Packet_Size;	
	D12_Write_Endpoint(1, send_ptr, (send_remain>Endpoint0_Packet_Size)?Endpoint0_Packet_Size:send_remain);
	
	//for(i=0; i<((send_remain>Endpoint0_Packet_Size)?Endpoint0_Packet_Size:send_remain); i++)
		//printf("%x,", *(U8 *)(send_ptr+i));	
		
	send_remain -= Endpoint0_Packet_Size;
	send_ptr    += Endpoint0_Packet_Size;
	
}

U8 Remain_Descriptor_Flag(void)
{
	return USB_Send_Descriptor_Flag;
}

void Clear_Remain_Descriptor_Flag(void)
{
	USB_Send_Descriptor_Flag=0;
}

U8 Get_Descriptor(U8 *SetupPacket)
{
	//printf("\nGet Descriptor Request\n");
	if(SetupPacket[0]!=0x80)
		return 0;
		
	switch	(SetupPacket[3])
	{
	case USB_Device_Descriptor_Type:
		send_ptr = (U8 *)&ThisDevDesc;
		send_remain = USB_Device_Descriptor_Size;
		break;
	case USB_Configuration_Descriptor_Type:
		send_ptr = (U8 *)&ThisDevDesc.CfgDesc;
		send_remain = USB_Total_Configuration_Descriptor_Size;
		break;
	case USB_String_Descriptor_Type:
		if(SetupPacket[2] == 0)
		{
			send_ptr = LANGUAGE_ID;
			send_remain = LANGUAGE_ID[0];
		}
		else if(SetupPacket[2] == 2)
		{
			send_ptr = sign_name;
			send_remain = sign_name[0];
		}		
		break;	
	default:
		return 0;	
	}

	if(!SetupPacket[7]&&(SetupPacket[6]<send_remain))
		send_remain = SetupPacket[6];

	Send_Descriptor();	
	return 1;	
}

U8 Get_Configuration(U8 *SetupPacket)
{
	//printf("\nGet Configuration Request\n");
	if(SetupPacket[0]!=0x80)
		return 0;
	if(SetupPacket[2]|SetupPacket[3]|SetupPacket[4]|SetupPacket[5]|SetupPacket[7])
		return 0;
	if(SetupPacket[6]!=1)
		return 0;
		
	SetupPacket[0] = USB_Config?1:0;
	D12_Write_Endpoint(1, SetupPacket, 1);	
	return 1;	
}

U8 Set_Configuration(U8 *SetupPacket)
{
	//printf("\nSet Configuration Request\n");
	if(SetupPacket[0]|SetupPacket[3]|SetupPacket[4]|SetupPacket[5]|SetupPacket[6]|SetupPacket[7])
		return 0;
	if(SetupPacket[2]&0xfe)
		return 0;
		
	USB_Config = SetupPacket[2]&1;
	D12_Set_Endpoint_Enable(USB_Config);
	D12_Set_Endpoint_Status(4, USB_Config^1);
	D12_Set_Endpoint_Status(5, USB_Config^1);
	D12_Write_Endpoint(1, SetupPacket, 0);
	return 1;	
}

U8 Get_Interface(U8 *SetupPacket)
{
	//printf("\nGet Interface Request\n");
	if(SetupPacket[0]!=0x81)
		return 0;
	if(SetupPacket[2]|SetupPacket[3]|SetupPacket[4]|SetupPacket[5]|SetupPacket[7])
		return 0;
	if(SetupPacket[6]!=1)
		return 0;
		
	SetupPacket[0] = 0;
	D12_Write_Endpoint(1, SetupPacket, 1);
	return 1;
}

U8 Set_Interface(U8 *SetupPacket)
{
	return 0;
}

U8 Reserved(U8 *SetupPacket)
{
	return 0;	
}
