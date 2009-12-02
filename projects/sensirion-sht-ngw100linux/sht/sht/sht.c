#include <linux/module.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/arch/portmux.h>
#include <asm/arch/gpio.h>
#include <linux/delay.h>
#include "sht.h"
#include "/home/scott/avr32/avr-kernel/linux-2.6.23.atmel.4/arch/avr32/mach-at32ap/pio.h"

//#define DEBUG 1
#define DEVICE_NAME "sht"


//------------------


typedef union
{ unsigned int i;
float f;
} value;

enum {TEMP,HUMI};

#define sht_noACK 0
#define sht_ACK 1
//adr command r/w
#define sht_STATUS_REG_W 0x06 //000 0011 0
#define sht_STATUS_REG_R 0x07 //000 0011 1
#define sht_MEASURE_TEMP 0x03 //000 0001 1
#define sht_MEASURE_HUMI 0x05 //000 0010 1
#define sht_RESET 0x1e //000 1111 0
//-------------------
#define SOFT_RESET 10
#define GET_TEMP 20
#define GET_HUMI 30
#define SHT_OPERATIONS 70
struct sht_operations {
        int action;
        unsigned int data;
};





int sht_ioctl(struct inode *inode, struct file *filp,
        unsigned int cmd, unsigned long val);

static struct file_operations fops = {ioctl:sht_ioctl};
static int major;






int init_module(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if(major < 0) {
            printk("error registering sht module\n");
            return major;
    }
    printk("sht ver1.0\n");
    printk("sht major = %d\n", major);
	halSht11Init();
	udelay(100);
	if(sht11_softreset()==0)
		printk("sht Softreset OK.\n");
	else
		printk("sht No Response to Softreset.\n");
	return 0;

}

int sht_ioctl(struct inode *inode, struct file *filp,
                unsigned int cmd, unsigned long arg)
{
        struct sht_operations *p;
		unsigned char value[2], csum;
		unsigned int value2;
		
		
        #ifdef DEBUG
        printk(KERN_DEBUG "sht.ko: sht-%d ioctl, cmd: 0x%x, arg: %lx.\n",MINOR(inode->i_rdev),cmd, arg);    
        #endif

        p = (struct sht_operations*)arg;
        if(p == 0)
                return -EINVAL;

        if(cmd == SHT_OPERATIONS) {
                switch(p->action) {
                        case SOFT_RESET:
								if(sht11_softreset()==0)
									printk("Softreset OK.\n");
								else
									printk("No Response to Softreset.\n");
                                break;
                        case GET_TEMP:
								if (sht11_measure(value, &csum, TEMP)==0){
									value2=(value[0]<<8)+(value[1]);
									return value2;
                				}
								else
									return -EINVAL;
                                break;
                        case GET_HUMI:
								if (sht11_measure(value, &csum, HUMI)==0){
									value2=(value[0]<<8)+(value[1]);
									return value2;
								}
								else
									return -EINVAL;
                                break;
                        default:
                                printk("error: no such ioctl\n");
                                return -EINVAL;
                }
                return 0;
        }

        return 0;

}


void cleanup_module(void)
{
        printk("Unloading module sht.ko.\n");
        unregister_chrdev(major, "sht");


}


void halSht11Init( void )
{
	u32 a=0;
	
	SETBIT(a,SCK);
        __raw_writel(a,PORTB + PIO_OER);
        __raw_writel(a,PORTB + PIO_PER);
        __raw_writel(a,PORTB + PIO_MDDR);
        __raw_writel(a,PORTB + PIO_CODR);

	a=0;
        SETBIT(a,DATA);
        __raw_writel(a,PORTB + PIO_OER);
        __raw_writel(a,PORTB + PIO_PER);
        __raw_writel(a,PORTB + PIO_MDDR);
        __raw_writel(a,PORTB + PIO_CODR);


}


bool readBit( u32 *port,uint bit )
{
	u32 a;
	//	printk("port %x  bit %x\n", port, bit);
	a=0;
	SETBIT(a,bit);
	__raw_writel(a,PORTB + PIO_IFDR);
	__raw_writel(a,PORTB + PIO_PUER);
	__raw_writel(a,PORTB + PIO_CODR);
	__raw_writel(a,PORTB + PIO_ODR);
	a=__raw_readl(PORTB + PIO_PDSR);
	if(CHECKBIT(a,bit))
 		return TRUE;
	else
		return FALSE;
	
}	
bool writeBit( u32 *port,uint bit,uint value )
{
	u32 a=0;
	SETBIT(a,bit);
    __raw_writel(a,PORTB + PIO_PER);
    __raw_writel(a,PORTB + PIO_MDDR);

        if (value)
        	__raw_writel(a,PORTB + PIO_SODR);
        else
			__raw_writel(a,PORTB + PIO_CODR);

    __raw_writel(a,PORTB + PIO_OER);
    udelay( 50);    
    return TRUE;
}

void showRegs( u32 *port )
{
	u32 a;
        a=__raw_readl(PORTB + PIO_PSR);
        printk("PSR %x\n", a);
        a=__raw_readl(PORTB + PIO_OSR);
        printk("OSR %x\n", a);
        a=__raw_readl(PORTB + PIO_MDSR);
        printk("MDSR %x\n", a);
        a=__raw_readl(PORTB + PIO_ISFR);
        printk("IFSR %x\n", a);
        a=__raw_readl(PORTB + PIO_ODSR);
        printk("ODSR %x\n", a);
        a=__raw_readl(PORTB + PIO_PUSR);
        printk("PUSR %x\n", a);
        a=__raw_readl(PORTB + PIO_PDSR);
        printk("PDSR %x\n", a);
}






//------------------------------------------------------------------------------
char sht11_write_byte(unsigned char value)
//------------------------------------------------------------------------------
// writes a byte on the Sensibus and checks the acknowledge
{
unsigned char i,error=0;

for (i=0x80;i>0;i/=2) //shift bit for masking
{
if (i & value)
writeBit(PORTB,DATA,1); //masking value with i , write to SENSI-BUS
else
writeBit(PORTB,DATA,0);

writeBit(PORTB,SCK,1); //clk for SENSI-BUS
writeBit(PORTB,SCK,0);
}
//writeBit(PORTB,DATA,1); //release DATA-line
readBit(PORTB,DATA); //release DATA-line
writeBit(PORTB,SCK,1); //clk #9 for ack
error=readBit(PORTB,DATA) ; //check ack (DATA will be pulled down by SHT11)
writeBit(PORTB,SCK,0);
return error; //error=1 in case of no acknowledge
}

//------------------------------------------------------------------------------
char sht11_read_byte(unsigned char ack)
//------------------------------------------------------------------------------
// reads a byte form the Sensibus and gives an acknowledge in case of "ack=1"
{
unsigned char i,val=0;

//writeBit(PORTB,DATA,1); //release DATA-line
readBit(PORTB,DATA); //release DATA-line
for (i=0x80;i>0;i/=2) //shift bit for masking
{
writeBit(PORTB,SCK,1); //clk for SENSI-BUS

if (readBit(PORTB,DATA)==1)
val=(val | i); //read bit
writeBit(PORTB,SCK,0);

}
writeBit(PORTB,DATA,!ack); //in case of "ack==1" pull down DATA-Line
writeBit(PORTB,SCK,1); //clk #9 for ack

writeBit(PORTB,SCK,0);

//writeBit(PORTB,DATA,1); //release DATA-line
readBit(PORTB,DATA); //release DATA-line
return val;
}

//------------------------------------------------------------------------------
void sht11_transstart(void)
//------------------------------------------------------------------------------
// generates a transmission start

{
writeBit(PORTB,DATA,1);
writeBit(PORTB,SCK,0); //Initial state
writeBit(PORTB,SCK,1);
writeBit(PORTB,DATA,0);
writeBit(PORTB,SCK,0);
writeBit(PORTB,SCK,1);
writeBit(PORTB,DATA,1);
writeBit(PORTB,SCK,0);
}

//------------------------------------------------------------------------------
void sht11_connectionreset(void)
//------------------------------------------------------------------------------
// communication reset: DATA-line=1 and at least 9 SCK cycles
// followed by transstart

{
unsigned char i;
writeBit(PORTB,DATA,1);
writeBit(PORTB,SCK,0); //Initial state
for(i=0;i<9;i++) //9 SCK cycles
{
writeBit(PORTB,SCK,1);
writeBit(PORTB,SCK,0);
}
sht11_transstart(); //transmission start
}

//------------------------------------------------------------------------------
char sht11_softreset(void)
//------------------------------------------------------------------------------
// resets the sensor by a softreset
{
unsigned char error=0;
sht11_connectionreset(); //reset communication
error+=sht11_write_byte(sht_RESET); //send RESET-command to sensor
return error; //error=1 in case of no response form the sensor
}

//------------------------------------------------------------------------------
char sht11_read_statusreg(unsigned char *p_value, unsigned char *p_checksum)
//------------------------------------------------------------------------------
// reads the status register with checksum (8-bit)
{
unsigned char error=0;
sht11_transstart(); //transmission start
error=sht11_write_byte(sht_STATUS_REG_R); //send command to sensor
*p_value=sht11_read_byte(sht_ACK); //read status register (8-bit)
*p_checksum=sht11_read_byte(sht_noACK); //read checksum (8-bit)
return error; //error=1 in case of no response form the sensor
}

//------------------------------------------------------------------------------
char sht11_write_statusreg(unsigned char *p_value)
//------------------------------------------------------------------------------
// writes the status register with checksum (8-bit)
{
unsigned char error=0;
sht11_transstart(); //transmission start
error+=sht11_write_byte(sht_STATUS_REG_W);//send command to sensor
error+=sht11_write_byte(*p_value); //send value of status register
return error; //error>=1 in case of no response form the sensor
}

//------------------------------------------------------------------------------
char sht11_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode)
//------------------------------------------------------------------------------
// makes a measurement (humidity/temperature) with checksum
{
unsigned error=0;
unsigned int i;

sht11_transstart(); //transmission start
switch(mode)
{ //send command to sensor
case TEMP : error+=sht11_write_byte(sht_MEASURE_TEMP); break;
case HUMI : error+=sht11_write_byte(sht_MEASURE_HUMI); break;
default : break;
}

for (i=0;i<65535;i++){
	udelay(100);
if(readBit(PORTB,DATA)==0)
break; //wait until sensor has finished the measurement
}
if(readBit(PORTB,DATA)==1)
error+=1; // or timeout (~2 sec.) is reached

*(p_value) =sht11_read_byte(sht_ACK); //read the first byte (MSB)
*(p_value+1)=sht11_read_byte(sht_ACK); //read the second byte (LSB)
*p_checksum =sht11_read_byte(sht_noACK); //read checksum
return error;
}


