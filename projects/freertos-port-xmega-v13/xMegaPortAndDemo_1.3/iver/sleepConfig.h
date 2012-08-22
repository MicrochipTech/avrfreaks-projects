
#ifndef SLEEPCONFIG_H
#define SLEEPCONFIG_H

/*============================ DEFINITIONS ===================================*/

//! Sleep mode control register.
#define SLEEPMGR_CTRL_REG SLEEP.CTRL
//! Bitmask for Sleep Enable bit.
#define SLEEPMGR_ENABLE_MASK SLEEP_SEN_bm
//! Bitmask for all sleep mode bits.
#define SLEEPMGR_MODE_MASK SLEEP_SMODE_gm

/*============================ MACROS ========================================*/

//! Prepare sleep configuration. Used before actually entering sleep mode.
#define SLEEPMGR_PREPARE_SLEEP( sleepMode ) \
{ \
	SLEEPMGR_CTRL_REG = \
		(SLEEPMGR_CTRL_REG & ~SLEEPMGR_MODE_MASK) | \
		(sleepMode) | SLEEPMGR_ENABLE_MASK; \
}

//! Disable sleep, so that subsequent sleep attempts will fail, until reenabled.
#define SLEEPMGR_DISABLE_SLEEP() \
{ \
	SLEEPMGR_CTRL_REG &= ~SLEEPMGR_ENABLE_MASK; \
}

#endif
// EOF 

