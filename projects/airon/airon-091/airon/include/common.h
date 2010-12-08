#ifndef common_H
#define common_H

#define CPU_CLOCK  8000000

/** O PB2 CTL_POWER
 ** rest to input
 **/
#define GLOBAL_DIRB	0b00000100

/** I PCO V_VOLTAGE
	 I PC1 V_CURRENT
 **/
#define GLOBAL_DIRC  0b11111100

/** all to output
 **/
#define GLOBAL_DIRD	0x11111111

#endif // common_H
