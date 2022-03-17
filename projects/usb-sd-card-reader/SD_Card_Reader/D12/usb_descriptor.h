#ifndef __USB_Descriptor_H__
#define __USB_Descriptor_H__

/*     USB�������Ķ���     */
/*��д�ߣ����䲨 2006-10-10*/

#define  U8 unsigned char
#define U16 unsigned int
#define U32 unsigned long

/*�豸������*/
typedef struct
{
	U8  bLength;			//��������СDescriptor Size =12H
#define USB_Device_Descriptor_Size	0x12
	U8  bDescriptorType;	//�豸����������Device Descriptor Type =01H
#define USB_Device_Descriptor_Type	0x01
	U16 bcdUSB;				//USB�滮������BCD��
#define USB_11						0x0110
#define USB_20						0x0200
	U8  bDeviceClass;		//���ʹ��루��USBָ����Device Class
#define Default_DeviceClass			0x00
	U8  bDeviceSubClass;	//�����ʹ��루��USB���䣩Device SubClass
#define Default_DeviceSubClass		0x00
	U8  bDeviceProtocol;	//Э����루��USB���䣩Device protocol
#define Default_DeviceProtocol		0x00
	U8  bMaxPacketSize0;	//�˵�0���������С��8��16��32��64��Max Packet Size of End Point 0
#define	Endpoint0_Packet_Size		0x10
	U16 idVendor;			//��Ӧ��ID����USB���䣩Vendor ID
#define Philip_IdVendor				0x0471
	U16 idProduct;			//��ƷID���ɳ��̷��䣩Product ID
#define Default_IdProduct			0x0001
	U16 bcdDevice;			//�豸��������BCD��Device code BCD
#define Default_BcdDevice			0x0001
	U8  iManufacturer;		//�����������ַ������� Index of Manufacturer Descriptor
#define No_Index					0x00
	U8  iProduct;			//��Ʒ�������ַ������� Index of Product Descriptor
	U8  iSerialNumber;		//�豸���к��ַ������� index of Serial Number Descriptor
	U8  bNumConfigurations;	//���ܵ������� The Number of Configuations
}USB_Device_Descriptor;


/*�ӿ�������*/
typedef struct
{
	U8  bLength;			//�������Ĵ�С=09H
#define USB_Interface_Descriptor_Size	0x09
	U8  bDescriptorType;	//�ӿ�����������=04H
#define USB_Interface_Descriptor_Type	0x04
	U8  bInterfaceNumber;	//�ӿڵı��
	U8  bAlternateSetting;	//����Ϊ��һ���ֶ�ѡ��ɹ��滻������
	U8  bNumEndpoint;		//ʹ�õĶ˵���Ŀ���˵�0���⣩
#define Endpoint_Count					0x02
	U8  bInterfaceClass;	//���ʹ��루��USB���䣩
#define	UsbClassMassStorage				0x08
	U8  bInterfaceSubClass;	//�����ʹ��루��USB���䣩
#define	UsbSubClassRbc					0x01
#define	UsbSubClassSff8020i				0x02
#define	UsbSubClassQic157				0x03
#define	UsbSubClassUfi					0x04
#define	UsbSubClassSff8070i				0x05
#define	UsbSubClassScsi					0x06
	U8  bInterfaceProtocol;	//Э����루��USB���䣩
#define	UsbProtocolCbi0					0x00
#define	UsbProtocolCbi1					0x01
#define	UsbProtocolBulk					0x50
	U8  iInterface;			//�ַ���������������
}USB_Interface_Descriptor;

/*�˵�������*/
typedef struct
{
	U8  bLength;			//�������Ĵ�С=07H
#define	USB_Endpoint_Descriptor_Size        0x07
	U8  bDescriptorType;	//�˵�����������=05H
#define	USB_Endpoint_Descriptor_Type        0x05
	U8  bEndpointAddress;	//USB�豸�Ķ˵��ַ: Bit0~3:�˵��; Bit4~6:���� ��λΪ0; Bit7:���򣬶��ڿ��ƶ˵�ɺ��� 0:Out 1:In
#define Endpoint_In(n)						0x80|n
#define Endpoint_Out(n)						0x00|n
	U8  bmAttributes;		//�˵����� Bit0~1: 0:����;1:ͬ��;2:����;3:�ж�.����λ����
#define	UsbEndpControl						0x00
#define	UsbEndpIsochronous					0x01
#define	UsbEndpBulk							0x02
#define	UsbEndpInterrupt					0x03 
	U16 wMaxPacketSize;		//���˵���ջ��͵������Ϣ���Ĵ�С
#define	Endpoint2_Packet_Size				0x0040
	U8  bInterval;			//��ѯ���ݴ��Ͷ˵��ʱ����
#define Default_Interval					0x00
}USB_Endpoint_Descriptor;

/*�ַ���������*/ //��ѡ������

typedef struct
{
	U8  bLength;			//�������Ĵ�С
	U8  bDescriptorType;	//�ַ�������������
#define USB_String_Descriptor_Type          0x03
	U16 * bString;			//Unicode�����ַ���
}USB_String_Descriptor;


/*����������*/
typedef struct
{
	U8  bLength;			//�������Ĵ�С=09H
#define USB_Configuration_Descriptor_Size	0x09
	U8  bDescriptorType;	//��������������=02H
#define USB_Configuration_Descriptor_Type	0x02
	U16 wTotalLength;		//�����������ݵĳ��ȣ����������������ص����������������ϳ���
#define USB_Total_Configuration_Descriptor_Size	USB_Configuration_Descriptor_Size+USB_Interface_Descriptor_Size+Endpoint_Count*USB_Endpoint_Descriptor_Size
	U8  bNumInterfaces;		//������֧�ֵĽӿ���
	U8  bConfigurationValue;//��ΪSet Configuration ��һ������ѡ������ֵ
	U8  iConfiguration;		//���������������ַ���������������
	U8  bmAttributes;		//��������:D7--Bus Power; D6--Self Power; D5--Remote Awake; D4-D0 Reserved 0 at reset.
#define Bus_Power							0x80
#define Self_Power							0x40
#define Remote_Awake						0x20
	U8  MaxPower;			//���豸��ȫ�ɲ���ʱ���ڴ������µ�������ĵ�������2mAΪ��λ
#define PowerConsumption_Nma(n)				(U8)(n/2)
}USB_Configuraton_Descriptor;

#endif
