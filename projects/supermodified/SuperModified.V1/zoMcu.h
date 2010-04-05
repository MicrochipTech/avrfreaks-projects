#ifndef ZO_MCU_H
#define ZO_MCU_H

#define cli()			__asm__ __volatile__ ("cli" ::)
#define sei()			__asm__ __volatile__ ("sei" ::)

#define enterCritical() __asm__ __volatile__ ("in __tmp_reg__, __SREG__\n\t " \
											  "push __tmp_reg__\n\t" \
											  "cli" ::)

#define exitCritical()  __asm__ __volatile__ ("pop __tmp_reg__ \n\t" \
											  "out __SREG__, __tmp_reg__" ::)  

#endif //ZO_MCU_H