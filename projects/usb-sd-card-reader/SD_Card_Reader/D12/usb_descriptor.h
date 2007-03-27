#ifndef __USB_Descriptor_H__
#define __USB_Descriptor_H__

/*     USB描述符的定义     */
/*编写者：章其波 2006-10-10*/

#define  U8 unsigned char
#define U16 unsigned int
#define U32 unsigned long

/*设备描述符*/
typedef struct
{
	U8  bLength;			//描述符大小Descriptor Size =12H
#define USB_Device_Descriptor_Size	0x12
	U8  bDescriptorType;	//设备描述符类型Device Descriptor Type =01H
#define USB_Device_Descriptor_Type	0x01
	U16 bcdUSB;				//USB规划发布号BCD码
#define USB_11						0x0110
#define USB_20						0x0200
	U8  bDeviceClass;		//类型代码（由USB指定）Device Class
#define Default_DeviceClass			0x00
	U8  bDeviceSubClass;	//子类型代码（由USB分配）Device SubClass
#define Default_DeviceSubClass		0x00
	U8  bDeviceProtocol;	//协议代码（由USB分配）Device protocol
#define Default_DeviceProtocol		0x00
	U8  bMaxPacketSize0;	//端点0的最大分组大小（8，16，32或64）Max Packet Size of End Point 0
#define	Endpoint0_Packet_Size		0x10
	U16 idVendor;			//供应商ID（由USB分配）Vendor ID
#define Philip_IdVendor				0x0471
	U16 idProduct;			//产品ID（由厂商分配）Product ID
#define Default_IdProduct			0x0001
	U16 bcdDevice;			//设备出厂编码BCD码Device code BCD
#define Default_BcdDevice			0x0001
	U8  iManufacturer;		//厂商描述符字符串索引 Index of Manufacturer Descriptor
#define No_Index					0x00
	U8  iProduct;			//产品描述符字符串索引 Index of Product Descriptor
	U8  iSerialNumber;		//设备序列号字符串索引 index of Serial Number Descriptor
	U8  bNumConfigurations;	//可能的配置数 The Number of Configuations
}USB_Device_Descriptor;


/*接口描述符*/
typedef struct
{
	U8  bLength;			//描述符的大小=09H
#define USB_Interface_Descriptor_Size	0x09
	U8  bDescriptorType;	//接口描述符类型=04H
#define USB_Interface_Descriptor_Type	0x04
	U8  bInterfaceNumber;	//接口的编号
	U8  bAlternateSetting;	//用于为上一个字段选择可供替换的设置
	U8  bNumEndpoint;		//使用的端点数目（端点0除外）
#define Endpoint_Count					0x02
	U8  bInterfaceClass;	//类型代码（由USB分配）
#define	UsbClassMassStorage				0x08
	U8  bInterfaceSubClass;	//子类型代码（由USB分配）
#define	UsbSubClassRbc					0x01
#define	UsbSubClassSff8020i				0x02
#define	UsbSubClassQic157				0x03
#define	UsbSubClassUfi					0x04
#define	UsbSubClassSff8070i				0x05
#define	UsbSubClassScsi					0x06
	U8  bInterfaceProtocol;	//协议代码（由USB分配）
#define	UsbProtocolCbi0					0x00
#define	UsbProtocolCbi1					0x01
#define	UsbProtocolBulk					0x50
	U8  iInterface;			//字符串描述符的索引
}USB_Interface_Descriptor;

/*端点描述符*/
typedef struct
{
	U8  bLength;			//描述符的大小=07H
#define	USB_Endpoint_Descriptor_Size        0x07
	U8  bDescriptorType;	//端点描述符类型=05H
#define	USB_Endpoint_Descriptor_Type        0x05
	U8  bEndpointAddress;	//USB设备的端点地址: Bit0~3:端点号; Bit4~6:保留 复位为0; Bit7:方向，对于控制端点可忽略 0:Out 1:In
#define Endpoint_In(n)						0x80|n
#define Endpoint_Out(n)						0x00|n
	U8  bmAttributes;		//端点属性 Bit0~1: 0:控制;1:同步;2:批量;3:中断.其他位保留
#define	UsbEndpControl						0x00
#define	UsbEndpIsochronous					0x01
#define	UsbEndpBulk							0x02
#define	UsbEndpInterrupt					0x03 
	U16 wMaxPacketSize;		//本端点接收或发送的最大信息包的大小
#define	Endpoint2_Packet_Size				0x0040
	U8  bInterval;			//轮询数据传送端点的时间间隔
#define Default_Interval					0x00
}USB_Endpoint_Descriptor;

/*字符串描述符*/ //可选描述符

typedef struct
{
	U8  bLength;			//描述符的大小
	U8  bDescriptorType;	//字符串描述符类型
#define USB_String_Descriptor_Type          0x03
	U16 * bString;			//Unicode编码字符串
}USB_String_Descriptor;


/*配置描述符*/
typedef struct
{
	U8  bLength;			//描述符的大小=09H
#define USB_Configuration_Descriptor_Size	0x09
	U8  bDescriptorType;	//配置描述符类型=02H
#define USB_Configuration_Descriptor_Type	0x02
	U16 wTotalLength;		//返回整个数据的长度，包括该配置所返回的所有描述符的联合长度
#define USB_Total_Configuration_Descriptor_Size	USB_Configuration_Descriptor_Size+USB_Interface_Descriptor_Size+Endpoint_Count*USB_Endpoint_Descriptor_Size
	U8  bNumInterfaces;		//配置所支持的接口数
	U8  bConfigurationValue;//作为Set Configuration 的一个参数选择配置值
	U8  iConfiguration;		//用于描述该配置字符串描述符的索引
	U8  bmAttributes;		//配置特性:D7--Bus Power; D6--Self Power; D5--Remote Awake; D4-D0 Reserved 0 at reset.
#define Bus_Power							0x80
#define Self_Power							0x40
#define Remote_Awake						0x20
	U8  MaxPower;			//当设备完全可操作时，在此配置下的最大消耗电流，以2mA为单位
#define PowerConsumption_Nma(n)				(U8)(n/2)
}USB_Configuraton_Descriptor;

#endif
