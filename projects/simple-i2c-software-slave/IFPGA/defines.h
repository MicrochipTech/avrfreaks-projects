#ifndef __IFAGC_DEFS__
#define __IFAGC_DEFS__

#define F_CPU 8000000UL  // 8 MHz
#include <util/delay.h>

#define SET_BIT(reg,bit)	reg |= (uint8_t)(1<<bit)
#define TGL_BIT(reg,bit)	reg ^= (uint8_t)(1<<bit)
#define CLR_BIT(reg,bit)	reg &= (uint8_t)~(1<<bit)
#define OVR_BIT(reg,bit,state)	reg &= (uint8_t)~(1<<bit); reg |= ((state & 0x01)<<bit)

#define SET_MSK(reg,mask)	reg |= (uint8_t)(mask)
#define CLR_MSK(reg,mask)	reg &= (uint8_t)~(mask)


#define AMPL_DATA	PORTB2	/* pin 5, out */
#define AMPL_SCK 	PORTA7	/* pin 6, out */
#define AMPL_STORE	PORTA3	/* pin 10, out */
#define AMPL_nRESET	PORTA2	/* pin 11, out */

#define Q7	0x80
#define Q6	0x40
#define Q5	0x20
#define Q4	0x10
#define Q3	0x08
#define Q2	0x04
#define Q1	0x02
#define Q0	0x01

#define FGAPWUP	Q7
#define VGAPWUP	Q6
#define VGA_B0	Q5
#define VGA_B1	Q4
#define VGA_B2	Q3
#define VGA_B3	Q2
#define nVGAENB	Q1
#define LOGPWUP	Q0

#define GAIN_0	0
#define GAIN_1	VGA_B0
#define GAIN_2	VGA_B1
#define GAIN_3	( VGA_B0 | VGA_B1)
#define GAIN_4	VGA_B2
#define GAIN_5	(VGA_B2 | VGA_B0)
#define GAIN_6	(VGA_B2 | VGA_B1)
#define GAIN_7	(VGA_B2 | VGA_B1 | VGA_B0)
#define GAIN_8	VGA_B3
#define GAIN_9	(VGA_B3 | VGA_B0)
#define GAIN_10	(VGA_B3 | VGA_B1)
#define GAIN_11	(VGA_B3 | VGA_B0 | VGA_B1)
#define GAIN_12	(VGA_B3 | VGA_B2)
#define GAIN_13	(VGA_B3 | VGA_B2 | VGA_B0)
#define GAIN_14	(VGA_B3 | VGA_B2 | VGA_B1)
#define GAIN_15	(VGA_B3 | VGA_B2 | VGA_B1 | VGA_B0)

#define SCLpin	PORTB0 /* PCINT8 */
#define SDApin	PORTB1 /* PCINT9 */
#define I2Cport	PORTB
#define I2Cpin	PINB
#define I2Cddr	DDRB
#define SDAint	PCINT9
#define SCLint	PCINT8

#define PULL_SCL()		SET_BIT(I2Cddr, SCLpin);
#define RELEASE_SCL()	CLR_BIT(I2Cddr, SCLpin);

#define SDA_1()			SET_BIT(I2Cport, SDApin);
#define SDA_0()			CLR_BIT(I2Cport, SDApin);

#define DRIVE_SDA()		SET_BIT(I2Cddr, SDApin);
#define PULL_SDA()		SET_BIT(I2Cddr, SDApin); CLR_BIT(I2Cport, SDApin);
#define RELEASE_SDA()	CLR_BIT(I2Cddr, SDApin);

#define SDA_INT_OFF()	CLR_BIT(PCMSK1, SDAint)
#define SCL_INT_OFF()	CLR_BIT(PCMSK1, SCLint)

#define SDA_INT_ON()	SET_BIT(PCMSK1, SDAint)
#define SCL_INT_ON()	SET_BIT(PCMSK1, SCLint)



#endif /* __IFAGC_DEFS__ */
