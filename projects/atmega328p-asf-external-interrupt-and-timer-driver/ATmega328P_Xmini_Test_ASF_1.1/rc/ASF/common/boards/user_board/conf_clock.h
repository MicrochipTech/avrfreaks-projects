/*
 * conf_clock.h
 *
 * Created: 06/04/2015 10:49:41
 *  Author: Augusto
 */ 

#ifndef CONF_CLOCK_H_
#define CONF_CLOCK_H_

/* ===== System Clock Source Options */
#define SYSCLK_SRC_RC16MHZ    0
#define SYSCLK_SRC_RC128KHZ   1
#define SYSCLK_SRC_TRS16MHZ   2
#define SYSCLK_SRC_RC32KHZ    3
#define SYSCLK_SRC_XOC16MHZ   4
#define SYSCLK_SRC_EXTERNAL   5

#define  SYSCLK_SOURCE         SYSCLK_SRC_RC16MHZ
/* #define SYSCLK_SOURCE        SYSCLK_SRC_RC128KHZ */
/* #define SYSCLK_SOURCE        SYSCLK_SRC_TRS16MHZ */
/* #define SYSCLK_SOURCE        SYSCLK_SRC_XOC16MHZ */

/* ===== System Clock Bus Division Options */

#define CONFIG_SYSCLK_PSDIV         SYSCLK_PSDIV_8

#endif /* CONF_CLOCK_H_ */