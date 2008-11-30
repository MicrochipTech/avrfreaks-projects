
#ifndef PS2_H_
#define PS2_H_
/**
 * PS2 avrii-mistral-debugport registers.
 */
/* PS2 Registers */
#define PS2K_DATA_TX	0x0008 /*PS2 KBD transmit data buffer*/
#define PS2K_DATA_RX	0x0004 /*PS2 KBD recieve  data buffer*/
#define PS2K_CTRL		0x0000 /*control register kbd*/

#define PS2M_DATA_TX	0x0108 /*PS2 mouse trans buffer*/
#define PS2M_DATA_RX	0x0104 /*PS2 mouse recv  buffer*/
#define PS2M_CTRL		0x0100 /*control register mouse*/
//Debugport general registers
#define PS2K_INT_EN 	0x0014 /*int enable flags*/
#define PS2K_INT_DIS	0x0018 /*int enable flags*/
#define PS2K_INT_MASK 	0x001C /*int flags*/

#define PS2M_INT_EN 	0x0114 /*int enable flags*/
#define PS2M_INT_DIS	0x0118 /*int enable flags*/
#define PS2M_INT_MASK 	0x011C /*int flags*/
//enable ps2 bit

#define DBP_BASE		0xFFE03C00	//0xFFCA3800
#define TRUE 			1
#define FALSE			0
#define PS2_EN          0x00000101	//Receiver->bit0  &  Transmitter->bit2         last-> (1<<31) 
// Interrupt flags
#define INT_UART_RXC    0
#define INT_UART_TX     1
#define INT_SPI         2
#define INT_PS2K        3
#define INT_PS2M        4
#define INT_PS2K_FLAG   3
#define INT_PS2M_FLAG   4
#define DBG_INT_GRP		5

#endif /*PS2_H_*/

#define ps2_writel(value, reg)		writel(value, DBP_BASE + reg)
#define ps2_writew(value, reg) 		writew(value, DBP_BASE + reg)
#define ps2_writeb(value, reg) 		writeb(value, DBP_BASE + reg)
#define ps2_readl(reg) 				readl(DBP_BASE + reg)
#define ps2_readw(reg) 				readw(DBP_BASE + reg)
#define ps2_readb(reg) 				readb(DBP_BASE + reg)
