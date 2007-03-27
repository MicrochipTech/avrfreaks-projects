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
/* 函数名: U8 D12_Select_Endpoint(U8 Endpoint)                */
/* 功能  : 选择端点                                           */
/* 返回值: bit0: 1 缓冲区满 0 empty ; bit1: 1 端点停止        */
/* 参数  : 端点值 00H~05H                                     */
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
/* 函数名  : void D12_Set_Endpoint_Status(U8 Endpoint,U8 Status);*/
/* 功能    : 设置端点状态                                      */
/* 返回值  : 无                                                */
/* 参数    : 端点号+状态                                       */
/* Endpoint:                                                   */
/*          0:控制输出  1:控制输入   2:端点1输出               */
/*          3:端点1输入 4:端点2输出  5:端点2输入               */
/* Status:                                                     */
/*          0:不停止 1:停止                                    */
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
/* 函数名: void D12_Set_Address(U8 Addr,U8 Enable)             */
/* 功能  : 设置地址/使能                                       */
/* 返回值: 无                                                  */
/* 参数  : Addr Enable                                         */
/* Addr  : 地址值0~127                                         */
/* Enable: 使能位                                              */
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
/* 函数名: void D12_Set_Endpoint_Enable(U8 Enable)             */
/* 功能  : 设置端点使能                                        */
/* 返回值: 无                                                  */
/* 参数  : Enable                                              */
/* Enable :                                                    */
/*         bit0: 普通/同步端点使能                             */
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
/* 函数名: void D12_Set_DMA(U8 DMA)                            */
/* 功能  : 设置DMA                                             */
/* 返回值: 无                                                  */
/* 参数  : DMA                                                 */
/* DMA   :                                                     */
/*        bit0~1: DMA突发串长度:                               */
/*                00 单周期DMA                                 */
/*                01 4 周期DMA                                 */
/*                10 8 周期DMA                                 */
/*                11 16周期DMA                                 */
/*        bit2  : DMA使能   1:使能     0:停止                  */
/*        bit3  : DMA方向   1:RAM->D12 0:D12->RAM              */
/*        bit4  : 自动重装  1:使能     0:禁止                  */
/*        bit5  : 中断脚模式1:SOF模式  0:正常模式              */
/*        bit6  : 端点索引4中断模式                            */
/*        bit7  : 端点索引5中断模式                            */
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
/* 函数名: U8 D12_Get_DMA(void)                                */
/* 功能  : 读DMA设置值                                         */
/* 返回值: DMA值                                               */
/* 参数  : 无                                                  */
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
/* 函数名: void D12_Set_Mode(U8 Val1, U8 Val2)                 */
/* 功能  : 设置模式                                            */
/* 返回值: 无                                                  */
/* 参数  : Val1,Val2                                           */
/* Val1  :                                                     */
/*         bit0: 保留                                          */
/*         bit1: 无LazyClock                                   */
/*         bit2: 时钟运行                                      */
/*         bit3: 中断模式                                      */
/*         bit4: SoftConect                                    */
/*         bit5: 保留                                          */
/*         bit6&7: 端点配置                                    */
/*                00 非同步模式                                */
/*                01 同步输出模式                              */
/*                10 同步输入模式                              */
/*                11 同步输入/输出模式                         */
/* Val2  :                                                     */
/*         bit0~3: 时钟分频系数       48MHz/(N-1)              */
/*         bit4~5: 保留                                        */
/*         bit6  : SET_TO_ONE                                  */
/*         bit7  : SOF_ONLY中断模式                            */
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
/* 函数名: void D12_Send_Resume(void)                          */
/* 功能  : 发送恢复                                            */
/* 返回值: 无                                                  */
/* 参数  : 无                                                  */
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
/* 函数名: void D12_Ack_Endpoint(U8 Endpoint)                  */
/* 功能  : 端点应答处理                                        */
/* 返回值: 无                                                  */
/* 参数  : Endpoint 端点号                                     */
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
/* 函数名: U16 D12_Read_Chip_ID(void)                          */
/* 功能  : 读芯片ID 1210                                       */
/* 返回值: temp id号                                           */
/* 参数  : 无                                                  */
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
/* 函数名: U8 D12_Read_Endpoint(U8 Endpoint,U8 * Buffer,U8 Count)*/
/* 功能  : 读取端点数据                                        */
/* 返回值: 1正常 0无数据                                       */
/* 参数  :                                                     */
/* Endpoint:端点号                                             */
/* Buffer  :外部缓冲区指针                                     */
/* Count   :数据长度                                           */
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
/* 函数名: void D12_Write_Endpoint(U8 Endpoint, U8 * Buffer, U8 Count)*/
/* 功能  : 把数据写入端点                                             */
/* 返回值: 无                                                         */
/* 参数  :                                                            */
/* Endpoint:端点号                                                    */
/* Buffer  :外部缓冲区指针                                            */
/* Count   :数据长度                                                  */
/**********************************************************************/
void D12_Write_Endpoint(U8 Endpoint, U8 * Buffer, U8 Count)
{
	NOP4();
	while(1)//等待缓冲区为空
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
	while(D12_Select_Endpoint(Endpoint)&1)//等待缓冲区为空
	
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
/* 函数名: U16 D12_Read_Interrupt_Register(void)               */
/* 功能  : 读取中断寄存器                                      */
/* 返回值: 16位中断寄存器值                                    */
/* 参数  : 无                                                  */
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
/* 函数名: void D12_Clear_Buffer(void)                        */
/* 功能  : 清除缓冲区内容                                     */
/* 返回值: 无                                                 */
/* 参数  : 无                                                 */
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
/* 函数名: void D12_Enable_Buffer(void)                       */
/* 功能  : 使缓冲区有效                                       */
/* 返回值: 无                                                 */
/* 参数  : 无                                                 */
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
/* 函数名: U16 D12_Read_Current_Frame_Number(void)            */
/* 功能  : 读取当前帧数目                                     */
/* 返回值: 帧数目                                             */
/* 参数  : 无                                                 */
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
/* 函数名: U8 D12_Read_Last_Transaction_Status(U8 Endpoint)   */
/* 功能  : 读取端点最后处理状态                               */
/* 返回值: 状态值                                             */
/* 参数  : 端点值                                             */
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





