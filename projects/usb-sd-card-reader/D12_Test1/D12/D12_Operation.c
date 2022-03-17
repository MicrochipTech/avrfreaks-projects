#include "D12_Operation.h"
#include"MMC_SD.h"

#define NOP8() asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop")
#define NOP7() asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop")
#define NOP6() asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop")
#define NOP5() asm("nop");asm("nop");asm("nop");asm("nop");asm("nop")
#define NOP4() asm("nop");asm("nop");asm("nop");asm("nop")
#define NOP3() asm("nop");asm("nop");asm("nop")
#define NOP2() asm("nop");asm("nop")
#define NOP1() asm("nop")

void D12_Delay(U8 n)
{
	while(n--);
}

void D12_PORT_INI()
{
	D12_CS_PORT |= 1<<D12_CS_BIT;
	D12_CS_DDR |= 1<<D12_CS_BIT;

	D12_INT_PORT |= 1<<D12_INT_BIT;
	D12_INT_DDR &= ~(1<<D12_INT_BIT);

	D12_WR_PORT |= 1<<D12_WR_BIT;
	D12_WR_DDR |= 1<<D12_WR_BIT;
	
	D12_RD_PORT |= 1<<D12_RD_BIT;
	D12_RD_DDR |= 1<<D12_RD_BIT;

	D12_A0_PORT |= 1<<D12_A0_BIT;
	D12_A0_DDR |= 1<<D12_A0_BIT;

}

/**************************************************************/
/* ������: U8 D12_Select_Endpoint(U8 Endpoint)                */
/* ����  : ѡ��˵�                                           */
/* ����ֵ: bit0: 1 �������� 0 empty ; bit1: 1 �˵�ֹͣ        */
/* ����  : �˵�ֵ 00H~05H                                     */
/**************************************************************/
U8 D12_Select_Endpoint(U8 Endpoint)
{
	U8 temp;

	NOP4();

	D12_OUTPUT_MODE();
	D12_OUTPUT = Endpoint;
	D12_CMD_WR_L();
	D12_CMD_WR_H();

	NOP7();

	D12_INPUT_MODE();
	D12_DAT_RD_L();	
	NOP3();
	temp = D12_INPUT;
	D12_DAT_RD_H();

	NOP4();

	return temp;
}

/***************************************************************/
/* ������  : void D12_Set_Endpoint_Status(U8 Endpoint,U8 Status);*/
/* ����    : ���ö˵�״̬                                      */
/* ����ֵ  : ��                                                */
/* ����    : �˵��+״̬                                       */
/* Endpoint:                                                   */
/*          0:�������  1:��������   2:�˵�1���               */
/*          3:�˵�1���� 4:�˵�2���  5:�˵�2����               */
/* Status:                                                     */
/*          0:��ֹͣ 1:ֹͣ                                    */
/***************************************************************/
void D12_Set_Endpoint_Status(U8 Endpoint,U8 Status)
{
	NOP4();

	D12_OUTPUT_MODE();
	D12_OUTPUT = Endpoint+0x40;
	D12_CMD_WR_L();
	D12_CMD_WR_H();

	NOP7();

	D12_OUTPUT = Status;
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	NOP4();
}


/***************************************************************/
/* ������: void D12_Set_Address(U8 Addr,U8 Enable)             */
/* ����  : ���õ�ַ/ʹ��                                       */
/* ����ֵ: ��                                                  */
/* ����  : Addr Enable                                         */
/* Addr  : ��ֵַ0~127                                         */
/* Enable: ʹ��λ                                              */
/***************************************************************/
void D12_Set_Address_Enable(U8 Addr,U8 Enable)
{

	NOP4();

	D12_OUTPUT_MODE();
	D12_OUTPUT = 0xd0;
	D12_CMD_WR_L();
	D12_CMD_WR_H();

	NOP7();

	D12_OUTPUT = Addr|(Enable<<7);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	NOP4();
}


/***************************************************************/
/* ������: void D12_Set_Endpoint_Enable(U8 Enable)             */
/* ����  : ���ö˵�ʹ��                                        */
/* ����ֵ: ��                                                  */
/* ����  : Enable                                              */
/* Enable :                                                    */
/*         bit0: ��ͨ/ͬ���˵�ʹ��                             */
/***************************************************************/
void D12_Set_Endpoint_Enable(U8 Enable)
{
	NOP4();
	
	D12_OUTPUT_MODE();
	D12_OUTPUT = 0xd8;
	D12_CMD_WR_L();
	D12_CMD_WR_H();

	NOP7();

	D12_OUTPUT = Enable;
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	NOP4();
}

/***************************************************************/
/* ������: void D12_Set_DMA(U8 DMA)                            */
/* ����  : ����DMA                                             */
/* ����ֵ: ��                                                  */
/* ����  : DMA                                                 */
/* DMA   :                                                     */
/*        bit0~1: DMAͻ��������:                               */
/*                00 ������DMA                                 */
/*                01 4 ����DMA                                 */
/*                10 8 ����DMA                                 */
/*                11 16����DMA                                 */
/*        bit2  : DMAʹ��   1:ʹ��     0:ֹͣ                  */
/*        bit3  : DMA����   1:RAM->D12 0:D12->RAM              */
/*        bit4  : �Զ���װ  1:ʹ��     0:��ֹ                  */
/*        bit5  : �жϽ�ģʽ1:SOFģʽ  0:����ģʽ              */
/*        bit6  : �˵�����4�ж�ģʽ                            */
/*        bit7  : �˵�����5�ж�ģʽ                            */
/***************************************************************/
void D12_Set_DMA(U8 DMA)
{
	NOP4();

	D12_OUTPUT_MODE();
	D12_OUTPUT = 0xfb;
	D12_CMD_WR_L();
	D12_CMD_WR_H();

	NOP7();

	D12_OUTPUT = DMA;
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	NOP4();
}

/***************************************************************/
/* ������: U8 D12_Get_DMA(void)                                */
/* ����  : ��DMA����ֵ                                         */
/* ����ֵ: DMAֵ                                               */
/* ����  : ��                                                  */
/***************************************************************/
U8 D12_Get_DMA(void)
{
	U8 temp;
	NOP4();

	D12_OUTPUT_MODE();
	D12_OUTPUT = 0xfb;
	D12_CMD_WR_L();
	D12_CMD_WR_H();

	NOP7();

	D12_INPUT_MODE();
	D12_CMD_RD_L();
	NOP3();
	temp = D12_INPUT;
	D12_CMD_RD_H();

	NOP4();
	return temp;
}

/***************************************************************/
/* ������: void D12_Set_Mode(U8 Val1, U8 Val2)                 */
/* ����  : ����ģʽ                                            */
/* ����ֵ: ��                                                  */
/* ����  : Val1,Val2                                           */
/* Val1  :                                                     */
/*         bit0: ����                                          */
/*         bit1: ��LazyClock                                   */
/*         bit2: ʱ������                                      */
/*         bit3: �ж�ģʽ                                      */
/*         bit4: SoftConect                                    */
/*         bit5: ����                                          */
/*         bit6&7: �˵�����                                    */
/*                00 ��ͬ��ģʽ                                */
/*                01 ͬ�����ģʽ                              */
/*                10 ͬ������ģʽ                              */
/*                11 ͬ������/���ģʽ                         */
/* Val2  :                                                     */
/*         bit0~3: ʱ�ӷ�Ƶϵ��       48MHz/(N-1)              */
/*         bit4~5: ����                                        */
/*         bit6  : SET_TO_ONE                                  */
/*         bit7  : SOF_ONLY�ж�ģʽ                            */
/***************************************************************/
void D12_Set_Mode(U8 Val1, U8 Val2)
{
	NOP4();
	D12_OUTPUT_MODE();
	D12_OUTPUT = 0xf3;
	D12_CMD_WR_L();
	D12_CMD_WR_H();

	NOP7();

	D12_OUTPUT = Val1;
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	NOP6();

	D12_OUTPUT = Val2;
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	NOP4();
}


/***************************************************************/
/* ������: void D12_Send_Resume(void)                          */
/* ����  : ���ͻָ�                                            */
/* ����ֵ: ��                                                  */
/* ����  : ��                                                  */
/***************************************************************/
void D12_Send_Resume(void)
{
	NOP4();
	D12_OUTPUT_MODE();
	D12_OUTPUT = 0xf6;
	D12_CMD_WR_L();
	D12_CMD_WR_H();

	NOP4();
}

/***************************************************************/
/* ������: void D12_Ack_Endpoint(U8 Endpoint)                  */
/* ����  : �˵�Ӧ����                                        */
/* ����ֵ: ��                                                  */
/* ����  : Endpoint �˵��                                     */
/***************************************************************/
void D12_Ack_Endpoint(U8 Endpoint)
{
	NOP4();
	D12_OUTPUT_MODE();
	D12_OUTPUT = Endpoint;
	D12_CMD_WR_L();
	D12_CMD_WR_H();

	NOP7();

	D12_OUTPUT = 0xf1;
	D12_CMD_WR_L();
	D12_CMD_WR_H();
	NOP4();
	if(!Endpoint)
		D12_Clear_Buffer();
}


/***************************************************************/
/* ������: U16 D12_Read_Chip_ID(void)                          */
/* ����  : ��оƬID 1210                                       */
/* ����ֵ: temp id��                                           */
/* ����  : ��                                                  */
/***************************************************************/
U16 D12_Read_Chip_ID(void)
{

	U16 temp;
	NOP4();
	D12_OUTPUT_MODE();
	D12_OUTPUT = 0xfd;
	D12_CMD_WR_L();
	D12_CMD_WR_H();

	NOP7();

	D12_INPUT_MODE();
	D12_DAT_RD_L();
	NOP3();
	temp = D12_INPUT;
	D12_DAT_RD_H();

	NOP6();

	D12_DAT_RD_L();
	NOP3();
	temp = temp<<8|D12_INPUT;
	D12_DAT_RD_H();

	NOP4();
	return temp;	
}


/***************************************************************/
/* ������: U8 D12_Read_Endpoint(U8 Endpoint,U8 * Buffer,U8 Count)*/
/* ����  : ��ȡ�˵�����                                        */
/* ����ֵ: 1���� 0������                                       */
/* ����  :                                                     */
/* Endpoint:�˵��                                             */
/* Buffer  :�ⲿ������ָ��                                     */
/* Count   :���ݳ���                                           */
/***************************************************************/
U8 D12_Read_Endpoint(U8 Endpoint,U8 * Buffer,U8 Count)
{
	U8 temp;
	NOP4();
	while(!(D12_Select_Endpoint(Endpoint)&1));
	
	D12_Read_Last_Transaction_Status(Endpoint);


	D12_OUTPUT_MODE();
	D12_OUTPUT = 0xf0;
	D12_CMD_WR_L();
	D12_CMD_WR_H();

	NOP7();

	D12_INPUT_MODE();
	D12_CMD_RD_L();
	NOP3();
	temp = D12_INPUT;
	D12_CMD_RD_H();

	NOP6();

	D12_CMD_RD_L();
	NOP3();
	temp = D12_INPUT;
	D12_CMD_RD_H();


	if(temp<Count)Count = temp;
	temp = Count;
	while(Count)
	{
		NOP4();
		D12_CMD_RD_L();
		NOP3();
		*Buffer++ = D12_INPUT;
		D12_CMD_RD_H();
		Count--;
	}
	NOP4();
	D12_Clear_Buffer();
	return temp;	
}


/**********************************************************************/
/* ������: void D12_Write_Endpoint(U8 Endpoint, U8 * Buffer, U8 Count)*/
/* ����  : ������д��˵�                                             */
/* ����ֵ: ��                                                         */
/* ����  :                                                            */
/* Endpoint:�˵��                                                    */
/* Buffer  :�ⲿ������ָ��                                            */
/* Count   :���ݳ���                                                  */
/**********************************************************************/
void D12_Write_Endpoint(U8 Endpoint, U8 * Buffer, U8 Count)
{
	NOP4();
	while(1)//�ȴ�������Ϊ��
	{
		if(!(D12_Select_Endpoint(Endpoint)&1))break;
	}
	
	D12_Read_Last_Transaction_Status(Endpoint);

	D12_OUTPUT_MODE();
	D12_OUTPUT = 0xf0;
	D12_CMD_WR_L();
	D12_CMD_WR_H();

	NOP7();
	
	D12_OUTPUT = 0;
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	NOP6();
	
	D12_OUTPUT = Count;
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	while(Count)
	{
		NOP6();
		D12_OUTPUT = *Buffer++;
		D12_DAT_WR_L();
		D12_DAT_WR_H();

		Count--;
	}
	NOP4();
	D12_Enable_Buffer();
}

void D12_Write_Endpoint_SD(U8 Endpoint)
{
	NOP4();
	while(D12_Select_Endpoint(Endpoint)&1)//�ȴ�������Ϊ��
	
	D12_Read_Last_Transaction_Status(Endpoint);

	D12_OUTPUT_MODE();
	D12_OUTPUT = 0xf0;
	D12_CMD_WR_L();
	D12_CMD_WR_H();

	NOP7();
	
	D12_OUTPUT = 0;
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	NOP6();

	D12_OUTPUT = 64;
	D12_DAT_WR_L();
	D12_DAT_WR_H();
	
	
	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();
	
	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();
	
	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();
	
	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();
	
	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();
	
	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();
	
	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();
	
	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();
	
	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();
	
	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();
	
	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();
	
	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();
	
	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();
	
	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();
	
	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();
	
	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	D12_OUTPUT = SPI_WriteByte(0xff);
	D12_DAT_WR_L();
	D12_DAT_WR_H();

	NOP4();
	D12_Enable_Buffer();
}

/***************************************************************/
/* ������: U16 D12_Read_Interrupt_Register(void)               */
/* ����  : ��ȡ�жϼĴ���                                      */
/* ����ֵ: 16λ�жϼĴ���ֵ                                    */
/* ����  : ��                                                  */
/***************************************************************/
U16 D12_Read_Interrupt_Register(void)
{
	U16 temp;
	U8 temp2;
	NOP4();

	D12_OUTPUT_MODE();
	D12_OUTPUT = 0xf4;
	D12_CMD_WR_L();
	D12_CMD_WR_H();

	NOP7();

	D12_INPUT_MODE();
	D12_CMD_RD_L();
	NOP3();
	temp = D12_INPUT;
	D12_CMD_RD_H();

	NOP6();

	D12_CMD_RD_L();
	NOP3();
	temp2 = D12_INPUT;
	D12_CMD_RD_H();
	
	NOP4();

	return temp;
}

/**************************************************************/
/* ������: void D12_Clear_Buffer(void)                        */
/* ����  : �������������                                     */
/* ����ֵ: ��                                                 */
/* ����  : ��                                                 */
/**************************************************************/
void D12_Clear_Buffer(void)
{

	NOP4();

	D12_OUTPUT_MODE();
	D12_OUTPUT = 0xf2;
	D12_CMD_WR_L();
	D12_CMD_WR_H();

	NOP4();
}

/**************************************************************/
/* ������: void D12_Enable_Buffer(void)                       */
/* ����  : ʹ��������Ч                                       */
/* ����ֵ: ��                                                 */
/* ����  : ��                                                 */
/**************************************************************/
void D12_Enable_Buffer(void)
{

	NOP4();

	D12_OUTPUT_MODE();
	D12_OUTPUT = 0xfa;
	D12_CMD_WR_L();
	D12_CMD_WR_H();

	NOP4();
}


/**************************************************************/
/* ������: U16 D12_Read_Current_Frame_Number(void)            */
/* ����  : ��ȡ��ǰ֡��Ŀ                                     */
/* ����ֵ: ֡��Ŀ                                             */
/* ����  : ��                                                 */
/**************************************************************/
U16 D12_Read_Current_Frame_Number(void)
{
	U16 temp;

	NOP4();
	
	D12_OUTPUT_MODE();
	D12_OUTPUT = 0xf5;
	D12_CMD_WR_L();
	D12_CMD_WR_H();

	NOP7();

	D12_INPUT_MODE();
	D12_DAT_RD_L();
	NOP3();
	temp = D12_INPUT;
	D12_DAT_RD_H();

	NOP6();

	D12_DAT_RD_L();
	NOP3();
	temp = temp<<8|D12_INPUT;
	D12_DAT_RD_H();

	NOP4();

	return temp;	
}

/**************************************************************/
/* ������: U8 D12_Read_Last_Transaction_Status(U8 Endpoint)   */
/* ����  : ��ȡ�˵������״̬                               */
/* ����ֵ: ״ֵ̬                                             */
/* ����  : �˵�ֵ                                             */
/**************************************************************/
U8 D12_Read_Last_Transaction_Status(U8 Endpoint)
{
	U8 temp;
	NOP4();

	D12_OUTPUT_MODE();
	D12_OUTPUT = Endpoint+0x40;
	D12_CMD_WR_L();
	D12_CMD_WR_H();

	NOP7();

	D12_INPUT_MODE();
	D12_DAT_RD_L();
	NOP3();
	temp = D12_INPUT;
	D12_DAT_RD_H();

	NOP4();

	return temp;
}
//This is the end of the file





