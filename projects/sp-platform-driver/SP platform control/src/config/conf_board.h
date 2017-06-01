/**
 * \file
 *
 * \brief User board configuration template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H

// local or extern?
#ifdef _CONF_BOARD_
#define GLOBAL
#define _I(x) x
#else // end #ifdef CONF_BOARD_H
#define GLOBAL extern
#define _I(x)
#endif // end #ifndef CONF_BOARD_H

// cleanup
#undef _I
#undef GLOBAL

#endif // CONF_BOARD_H
