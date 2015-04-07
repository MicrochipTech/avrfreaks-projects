/*
 * tc.h
 *
 * Created: 07/04/2015 23:51:09
 *  Author: Augusto
 */ 


#ifndef TC_H_
#define TC_H_

#include <compiler.h>
#include <parts.h>
#include "status_codes.h"
#include <sysclk.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup tc_group Timer Counter 0 (TC0)
 *
 * See \ref mega_tc_quickstart
 *
 * This is a driver for the AVR MEGA Timer Counter (TC0). It provides functions
 * for enabling, disabling and configuring TC0.
 *
 * @{
 */


/**
 * \brief Interrupt event callback function type
 *
 * The interrupt handler can be configured to do a function callback,
 * the callback function must match the tc_callback_t type.
 *
 */
typedef void (*tc_callback_t) (void);

//! Timer02 Counter Waveform Generator modes
enum tc02_wg_mode_t { // WGM02:0, WGM22:0
	//! TC in normal Mode
	TC02_WG_NORMAL = 0x00, // TC in normal Mode - WGM01:0 = 0x00 b000
	//! TC in Phase Correct PWM Mode 1
	TC02_WG_PC1_PWD = 0x01, // TC in Phase Correct PWM Mode 1 - WGM02:0 = 0x01 b001
	//! TC in Clear Timer on Compare Match (CTC) Mode
	TC02_WG_CTC = 0x02, // TC in Clear Timer on Compare Match (CTC) Mode - WGM02:0 = 0x02 b010
	//! TC in Fast PWM Mode 1
	TC02_WG_FS1_PWM = 0x03, // TC in Fast PWM Mode - WGM02:0 = 0x03 b011
	//! TC in Phase Correct PWM Mode 2
	TC02_WG_PC2_PWD = 0x05, // TC in Phase Correct PWM Mode 2 - WGM02:0 = 0x05 b101
	//! TC in Fast PWM Mode 2
	TC02_WG_FS2_PWM = 0x07, // TC in Fast PWM Mode - WGM02:0 = 0x07 b111
};

//! Timer1 Counter Waveform Generator modes
enum tc1_wg_mode_t { // WGM13:0
	//! TC in normal Mode
	TC1_WG_NORMAL = 0x00, // TC in normal Mode - WGM13:0 = 0x00 b0000
	//! TC in Phase Correct PWM Mode 1
	TC1_WG_PC1_PWD = 0x01,
	//! TC in Phase Correct PWM Mode 2
	TC1_WG_PC2_PWD = 0x02,
	//! TC in Phase Correct PWM Mode 3
	TC1_WG_PC3_PWD = 0x03,
	//! TC in Clear Timer on Compare Match (CTC) Mode match OCR1A
	TC1_WG_CTC_OCR1A = 0x04,
	//! TC in Fast PWM Mode 1
	TC1_WG_FS1_PWM = 0x05,
	//! TC in Fast PWM Mode 2
	TC1_WG_FS2_PWM = 0x06,
	//! TC in Fast PWM Mode 3
	TC1_WG_FS3_PWM = 0x07,
	//! TC in Phase Correct PWM Mode 4
	TC1_WG_PC4_PWD = 0x0A,
	//! TC in Phase Correct PWM Mode 5
	TC1_WG_PC5_PWD = 0x0B,
	//! TC in Clear Timer on Compare Match (CTC) Mode match ICR1
	TC1_WG_CTC_ICR1 = 0x0C,
	//! TC in Fast PWM Mode 4
	TC1_WG_FS4_PWM = 0x0D,
	//! TC in Fast PWM Mode 5
	TC1_WG_FS5_PWM = 0x0E,
};

//! Timer Counter Clock selection
enum tc01_clksel_t {
	TC01_CLKSEL_NOCLK = 0x00,		// No clock source (Timer/Counter stopped)
	TC01_CLKSEL_PS_1 = 0x01,		// clkI/O/(No prescaling)
	TC01_CLKSEL_PS_8 = 0x02,		// clkI/O/8 (From prescaler)
	TC01_CLKSEL_PS_64 = 0x03,		// clkI/O/64 (From prescaler)
	TC01_CLKSEL_PS_256 = 0x04,		// clkI/O/256 (From prescaler)
	TC01_CLKSEL_PS_1024 = 0x05,		// clkI/O/1024 (From prescaler)
	TC01_CLKSEL_EXT_FALLING = 0x06,	// External clock source on T0 pin. Clock on falling edge.
	TC01_CLKSEL_EXT_RISING = 0x07	// External clock source on T0 pin. Clock on rising edge.
};

enum tc2_clksel_t {
	TC2_CLKSEL_NOCLK = 0x00,		// No clock source (Timer/Counter stopped)
	TC2_CLKSEL_PS_1 = 0x01,			// clkI/O/(No prescaling)
	TC2_CLKSEL_PS_8 = 0x02,			// clkI/O/8 (From prescaler)
	TC2_CLKSEL_PS_32 = 0x03,		// clkI/O/32 (From prescaler)
	TC2_CLKSEL_PS_64 = 0x04,		// clkI/O/64 (From prescaler)
	TC2_CLKSEL_PS_128 = 0x05,		// clkI/O/128 (From prescaler)
	TC2_CLKSEL_PS_256 = 0x06,		// clkI/O/256 (From prescaler)
	TC2_CLKSEL_PS_1024 = 0x07		// clkI/O/1024 (From prescaler)
};

//! Timer Counter Modes of Operation
enum tc_ocom_mode_t {
	TC_OCOM_NORMAL = 0x00,
	TC_OCOM_TOGGLE,
	TC_OCOM_CLEAR,
	TC_OCOM_SET
};

/**
 * \brief Enable TC
 *
 * Enables the TC.
 *
 * \note
 * unmask TC clock (sysclk), but does not configure the TC clock source.
 */
void tc0_enable(void);
void tc1_enable(void);
void tc2_enable(void);

/**
 * \brief Disable TC
 *
 * Disables the TC.
 *
 * \note
 * mask TC clock (sysclk).
 */
void tc0_disable(void);
void tc1_disable(void);
void tc2_disable(void);

/**
 * \brief Set the interrupt callback
 * \param  callback : interrupt callback
 */
void tc0_set_ovf_interrupt_callback(tc_callback_t callback); // Overflow Flag
void tc1_set_ovf_interrupt_callback(tc_callback_t callback); // Overflow Flag
void tc2_set_ovf_interrupt_callback(tc_callback_t callback); // Overflow Flag

/**
 * \brief Unset the interrupt callback
 */
void tc0_unset_ovf_interrupt_callback(void);
void tc1_unset_ovf_interrupt_callback(void);
void tc2_unset_ovf_interrupt_callback(void);

/**
 * \brief Set the interrupt callback
 * \param  callback : interrupt callback
 */
void tc0_set_oca_interrupt_callback(tc_callback_t callback); // Output Compare A Match Flag
void tc1_set_oca_interrupt_callback(tc_callback_t callback); // Output Compare A Match Flag
void tc2_set_oca_interrupt_callback(tc_callback_t callback); // Output Compare A Match Flag

/**
 * \brief Unset the interrupt callback
 */
void tc0_unset_oca_interrupt_callback(void);
void tc1_unset_oca_interrupt_callback(void);
void tc2_unset_oca_interrupt_callback(void);
/**
 * \brief Set the interrupt callback
 * \param  callback : interrupt callback
 */
void tc0_set_ocb_interrupt_callback(tc_callback_t callback); // Output Compare B Match Flag
void tc1_set_ocb_interrupt_callback(tc_callback_t callback); // Output Compare B Match Flag
void tc2_set_ocb_interrupt_callback(tc_callback_t callback); // Output Compare B Match Flag

/**
 * \brief Unset the interrupt callback
 */
void tc0_unset_ocb_interrupt_callback(void);
void tc1_unset_ocb_interrupt_callback(void);
void tc2_unset_ocb_interrupt_callback(void);

void tc1_set_ic_interrupt_callback(tc_callback_t callback);
void tc1_unset_ic_interrupt_callback(void);

/**
 * \brief Write the TC clock source
 * \param  tc_clksel_enum : clock source
 */
static inline void tc0_write_clock_source(enum tc01_clksel_t tc_clksel_enum)
{
	// TODO - write FOC0A FOC0B to 0 if pwm mode or set clock before setting pwm mode
	TCCR0B = (TCCR0B & ~((1<<CS02)|(1<<CS01)|(1<<CS00))) | tc_clksel_enum;
}

static inline void tc1_write_clock_source(enum tc01_clksel_t tc_clksel_enum)
{
	// TODO - write FOC1A FOC1B to 0 if pwm mode or set clock before setting pwm mode
	TCCR1B = (TCCR1B & ~((1<<CS12)|(1<<CS11)|(1<<CS10))) | tc_clksel_enum;
}

static inline void tc2_write_clock_source(enum tc2_clksel_t tc_clksel_enum)
{
	// TODO - write FOC2A FOC2B to 0 if pwm mode or set clock before setting pwm mode
	while((ASSR >> TCR2BUB) & 0x01); // Check update busy flag
	TCCR2B = (TCCR2B & ~((1<<CS22)|(1<<CS21)|(1<<CS20))) | tc_clksel_enum;
}

/**
 * \brief Read the TC clock source
 */
static inline enum tc01_clksel_t tc0_read_clock_source(void)
{
	return (TCCR0B & ((1<<CS02)|(1<<CS01)|(1<<CS00)));
}

static inline enum tc01_clksel_t tc1_read_clock_source(void)
{
	return (TCCR1B & ((1<<CS12)|(1<<CS11)|(1<<CS10)));
}

static inline enum tc2_clksel_t tc2_read_clock_source(void)
{
	while((ASSR >> TCR2BUB) & 0x01); // Check update busy flag
	return (TCCR2B & ((1<<CS22)|(1<<CS21)|(1<<CS20)));
}

/**
 * \brief Write the TC timer counter register
 * \param  cnt_value : timer counter value
 */
static inline void tc0_write_count(uint8_t cnt_value)
{
	TCNT0 = cnt_value;
}

static inline void tc1_write_count(uint16_t cnt_value)
{
	irqflags_t iflags = cpu_irq_save();
	
	TCNT1 = cnt_value;
	cpu_irq_restore(iflags);
}

static inline void tc2_write_count(uint8_t cnt_value)
{
	while((ASSR >> TCN2UB) & 0x01); // Check update busy flag
	TCNT2 = cnt_value;
}

/**
 * \brief Read the TC timer counter register
 */
static inline uint8_t tc0_read_count(void)
{
	return TCNT0;
}

static inline uint16_t tc1_read_count(void)
{
	uint16_t tmpcnt;
	irqflags_t iflags = cpu_irq_save();
	
	tmpcnt = TCNT1;
	cpu_irq_restore(iflags);
	return tmpcnt;
}

static inline uint8_t tc2_read_count(void)
{
	while((ASSR >> TCN2UB) & 0x01); // Check update busy flag
	return TCNT2;
}

/**
 * \brief Write the TC output compare A register
 * \param  cnt_value : output compare A value
 */
static inline void tc0_write_oca(uint8_t cnt_value)
{
	OCR0A = cnt_value;
}

static inline void tc1_write_oca(uint16_t cnt_value)
{
	irqflags_t iflags = cpu_irq_save();
	
	OCR1A = cnt_value;
	cpu_irq_restore(iflags);
}

static inline void tc2_write_oca(uint8_t cnt_value)
{
	while((ASSR >> OCR2AUB) & 0x01); // Check update busy flag
	OCR2A = cnt_value;
}

/**
 * \brief Read the TC timer output compare A register
 */
static inline uint8_t tc0_read_oca(void)
{
	return OCR0A;
}

static inline uint16_t tc1_read_oca(void)
{
	uint16_t tmpcnt;
	irqflags_t iflags = cpu_irq_save();
	
	tmpcnt = OCR1A;
	cpu_irq_restore(iflags);
	return tmpcnt;
}

static inline uint8_t tc2_read_oca(void)
{
	while((ASSR >> OCR2AUB) & 0x01); // Check update busy flag
	return OCR2A;
}

/**
 * \brief Write the TC output compare B register
 * \param  cnt_value : output compare B value
 */
static inline void tc0_write_ocb(uint8_t cnt_value)
{
	OCR0B = cnt_value;
}

static inline void tc1_write_ocb(uint16_t cnt_value)
{
	irqflags_t iflags = cpu_irq_save();
	
	OCR1B = cnt_value;
	cpu_irq_restore(iflags);
}

static inline void tc2_write_ocb(uint8_t cnt_value)
{
	while((ASSR >> OCR2BUB) & 0x01); // Check update busy flag
	OCR2B = cnt_value;
}

/**
 * \brief Read the TC timer output compare B register
 */
static inline uint8_t tc0_read_ocb(void)
{
	return OCR0B;
}

static inline uint16_t tc1_read_ocb(void)
{
	uint16_t tmpcnt;
	irqflags_t iflags = cpu_irq_save();
	
	tmpcnt = OCR1B;
	cpu_irq_restore(iflags);
	return tmpcnt;
}

static inline uint8_t tc2_read_ocb(void)
{
	while((ASSR >> OCR2BUB) & 0x01); // Check update busy flag
	return OCR2B;
}

static inline void tc1_write_ic(uint16_t cnt_value)
{
	irqflags_t iflags = cpu_irq_save();
	
	ICR1 = cnt_value;
	cpu_irq_restore(iflags);
}

static inline uint16_t tc1_read_ic(void)
{
	uint16_t tmpcnt;
	irqflags_t iflags = cpu_irq_save();
	
	tmpcnt = ICR1;
	cpu_irq_restore(iflags);
	return tmpcnt;
}

static inline void tc2_enable_async_clock(void)
{
	ASSR = ASSR | (1 << EXCLK); // Enable external clock input
	ASSR = ASSR | (1 << AS2); // Enable async clock mode
}

static inline void tc2_disable_async_clock(void)
{
	ASSR = ASSR & ~(1 << AS2); // Disable async clock mode
	ASSR = ASSR & ~(1 << EXCLK); // Disable external clock input
}

/**
 * \brief Return if the TC overflow flag is set
 */
static inline bool tc0_is_overflow(void)
{
	return (TIFR0&(1<<TOV0))&&(1<<TOV0);
}

static inline bool tc1_is_overflow(void)
{
	return (TIFR1&(1<<TOV1))&&(1<<TOV1);
}

static inline bool tc2_is_overflow(void)
{
	return (TIFR2&(1<<TOV2))&&(1<<TOV2);
}

/**
 * \brief Clear TC overflow flag
 */
static inline void tc0_clear_overflow(void)
{
	TIFR0 = (1<<TOV0);
}

static inline void tc1_clear_overflow(void)
{
	TIFR1 = (1<<TOV1);
}

static inline void tc2_clear_overflow(void)
{
	TIFR2 = (1<<TOV2);
}

/**
 * \brief Return if the TC output compare A flag is set
 */
static inline bool tc0_is_oca_interrupt(void)
{
	return (TIFR0&(1<<OCF0A))&&(1<<OCF0A);
}

static inline bool tc1_is_oca_interrupt(void)
{
	return (TIFR1&(1<<OCF1A))&&(1<<OCF1A);
}

static inline bool tc2_is_oca_interrupt(void)
{
	return (TIFR2&(1<<OCF2A))&&(1<<OCF2A);
}

/**
 * \brief Clear TC output compare A flag
 */
static inline void tc0_clear_oca_interrupt(void)
{
	TIFR0 = (1<<OCF0A);
}

static inline void tc1_clear_oca_interrupt(void)
{
	TIFR1 = (1<<OCF1A);
}

static inline void tc2_clear_oca_interrupt(void)
{
	TIFR2 = (1<<OCF2A);
}

/**
 * \brief Return if the TC output compare B flag is set
 */
static inline bool tc0_is_ocb_interrupt(void)
{
	return (TIFR0&(1<<OCF0B))&&(1<<OCF0B);
}

static inline bool tc1_is_ocb_interrupt(void)
{
	return (TIFR1&(1<<OCF1B))&&(1<<OCF1B);
}

static inline bool tc2_is_ocb_interrupt(void)
{
	return (TIFR2&(1<<OCF2B))&&(1<<OCF2B);
}

/**
 * \brief Clear TC output compare B flag
 */
static inline void tc0_clear_ocb_interrupt(void)
{
	TIFR0 = (1<<OCF0B);
}

static inline void tc1_clear_ocb_interrupt(void)
{
	TIFR1 = (1<<OCF1B);
}

static inline void tc2_clear_ocb_interrupt(void)
{
	TIFR2 = (1<<OCF2B);
}

static inline bool tc1_is_ic_interrupt(void)
{
	return (TIFR1&(1<<ICF1))&&(1<<ICF1);
}

static inline void tc1_clear_ic_interrupt(void)
{
	TIFR1 = (1<<ICF1);
}

/**
 * \brief Configures TC0 in the specified Waveform generator mode
 *
 * \param  wg_mode_enum : waveform generator
 */
static inline void tc0_set_wgm(enum tc02_wg_mode_t wg_mode_enum)
{
	TCCR0A = (TCCR0A & ~((1<<WGM01)|(1<<WGM00))) | (wg_mode_enum & 0x03); // Mask then set WGM01, WGM00
	TCCR0B = (TCCR0B & ~(1<<WGM02)) | (((wg_mode_enum >> 2) & 0x01) << WGM02); // Mask then set WGM02
}

static inline void tc1_set_wgm(enum tc1_wg_mode_t wg_mode_enum)
{
	TCCR1A = (TCCR1A & ~((1<<WGM11)|(1<<WGM10))) | (wg_mode_enum & 0x03); // Mask then set WGM11, WGM10
	TCCR1B = (TCCR1B & ~((1<<WGM13)|(1<<WGM12))) | (((wg_mode_enum >> 2) & 0x03) << WGM12); // Mask then set WGM13, WGM12
}

static inline void tc2_set_wgm(enum tc02_wg_mode_t wg_mode_enum)
{
	while((ASSR >> TCR2AUB) & 0x01); // Check update busy flag
	TCCR2A = (TCCR2A & ~((1<<WGM21)|(1<<WGM20))) | (wg_mode_enum & 0x03); // Mask then set WGM21, WGM20
	while((ASSR >> TCR2BUB) & 0x01); // Check update busy flag
	TCCR2B = (TCCR2B & ~(1<<WGM22)) | (((wg_mode_enum >> 2) & 0x01) << WGM22); // Mask then set WGM22
}

static inline void tc1_ic_setup(bool noise_cancel, bool rising_edge)
{
	TCCR1B = (TCCR1B & ~((1<<ICNC1)|(1<<ICES1)))|((noise_cancel<<ICNC1)|(rising_edge<<ICES1));
}

/**
 * \brief Enables Output compare A mode
 *
 * \param ocom_mode_enum Select Output compare A mode
 */
static inline void tc0_set_oca_mode(enum tc_ocom_mode_t ocom_mode_enum)
{
	TCCR0A = (TCCR0A & ~((1<<COM0A1)|(1<<COM0A0)))|((ocom_mode_enum & 0x03)<<COM0A0);
};

static inline void tc0_force_oca(void)
{
	TCCR0B = TCCR0B | (1<<FOC0A);
}

static inline void tc1_set_oca_mode(enum tc_ocom_mode_t ocom_mode_enum)
{
	TCCR1A = (TCCR1A & ~((1<<COM1A1)|(1<<COM1A0)))| ((ocom_mode_enum & 0x03)<<COM1A0);
};

static inline void tc1_force_oca(void)
{
	TCCR1C = TCCR1C | (1<<FOC1A);
}

static inline void tc2_set_oca_mode(enum tc_ocom_mode_t ocom_mode_enum)
{
	while((ASSR >> TCR2AUB) & 0x01); // Check update busy flag
	TCCR2A = (TCCR2A & ~((1<<COM2A1)|(1<<COM2A0)))| ((ocom_mode_enum & 0x03)<<COM2A0);
};

static inline void tc2_force_oca(void)
{
	while((ASSR >> TCR2BUB) & 0x01); // Check update busy flag
	TCCR2B = TCCR2B | (1<<FOC2A);
}

/**
 * \brief Enables Output compare B mode
 *
 * \param ocom_mode_enum Select Output compare B mode
 */
static inline void tc0_set_ocb_mode(enum tc_ocom_mode_t ocom_mode_enum)
{
	TCCR0A = (TCCR0A & ~((1<<COM0B1)|(1<<COM0B0)))| ((ocom_mode_enum & 0x03)<<COM0B0);
};

static inline void tc0_force_ocb(void)
{
	TCCR0B = TCCR0B | (1<<FOC0B);
}

static inline void tc1_set_ocb_mode(enum tc_ocom_mode_t ocom_mode_enum)
{
	TCCR1A = (TCCR1A & ~((1<<COM1B1)|(1<<COM1B0)))| ((ocom_mode_enum & 0x03)<<COM1B0);
};

static inline void tc1_force_ocb(void)
{
	TCCR1C = TCCR1C | (1<<FOC1B);
}

static inline void tc2_set_ocb_mode(enum tc_ocom_mode_t ocom_mode_enum)
{
	while((ASSR >> TCR2AUB) & 0x01); // Check update busy flag
	TCCR2A = (TCCR2A & ~((1<<COM2B1)|(1<<COM2B0)))| ((ocom_mode_enum & 0x03)<<COM2B0);
};

static inline void tc2_force_ocb(void)
{
	while((ASSR >> TCR2BUB) & 0x01); // Check update busy flag
	TCCR2B = TCCR2B | (1<<FOC2B);
}

static inline void tc0_set_OC0A_direction(enum ioport_direction dir)
{
	if(dir == IOPORT_DIR_INPUT) DDRD &= DDRD & ~(1 << DDD6);
	else DDRD |= (1 << DDD6);
}

static inline void tc1_set_OC1A_direction(enum ioport_direction dir)
{
	if(dir == IOPORT_DIR_INPUT) DDRB &= DDRB & ~(1 << DDB1);
	else DDRB |= (1 << DDB1);
}

static inline void tc2_set_OC2A_direction(enum ioport_direction dir)
{
	if(dir == IOPORT_DIR_INPUT) DDRB &= DDRB & ~(1 << DDB3);
	else DDRB |= (1 << DDB3);
}

static inline void tc0_set_OC0B_direction(enum ioport_direction dir)
{
	if(dir == IOPORT_DIR_INPUT) DDRD &= DDRD & ~(1 << DDD5);
	else DDRD |= (1 << DDD5);
}

static inline void tc1_set_OC1B_direction(enum ioport_direction dir)
{
	if(dir == IOPORT_DIR_INPUT) DDRB &= DDRB & ~(1 << DDB2);
	else DDRB |= (1 << DDB2);
}

static inline void tc2_set_OC2B_direction(enum ioport_direction dir)
{
	if(dir == IOPORT_DIR_INPUT) DDRD &= DDRD & ~(1 << DDD3);
	else DDRD |= (1 << DDD3);
}

//@}

/** @} */


#ifdef __cplusplus
}
#endif

#endif /* TC_H_ */