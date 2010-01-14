/*
 * linux/drivers/video/ew24b00yly.c -- Low level console driver
 *
 * 	Created 4 Nov 2009 Marjan Fojkar <marjan@pajkc.eu>
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License.  See the file COPYING in the main directory of this archive for
 *  more details.
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/platform_device.h>

#include <asm/byteorder.h>
#include <asm/io.h>
#include <mach/board.h>
#include <mach/at32ap700x.h>

#include <linux/gpio.h>
#include <linux/atmel_pwm.h>

#include <linux/console.h>
#include <linux/vt_kern.h>
#include <linux/vt_buffer.h>
#include <linux/vt.h>
#include <linux/kd.h>
#include <linux/console_struct.h>
#include <linux/kbd_kern.h>


/* The module name which is mostly used along with printk */
static const char ew24b00yly_name[] = "ew24b00yly";

#ifdef __LITTLE_ENDIAN
/* Extracts the higher part of a 16 bit value */
#define EW24B00YLY_VAL16_HI(value) (((const uint8_t*)&value)[1])
/* Extracts the lower part of a 16 bit value */
#define EW24B00YLY_VAL16_LO(value) (((const uint8_t*)&value)[0])
#endif

#ifdef __BIG_ENDIAN
/* Extracts the higher part of a 16 bit value */
#define EW24B00YLY_VAL16_HI(value) (((const uint8_t*)&value)[0])
/* Extracts the lower part of a 16 bit value */
#define EW24B00YLY_VAL16_LO(value) (((const uint8_t*)&value)[1])
#endif

/* Extracts the higher part of a 16 bit constant */
#define EW24B00YLY_CONST16_HI(value) (value / 256)
/* Extracts the lower part of a 16 bit constant */
#define EW24B00YLY_CONST16_LO(value) (value % 256)


/*
 ******************************************************************************
 * Stuff related to pins configuration and manipulation
 ******************************************************************************
 */


/* Prototypes */
__init static int ew24b00yly_gpio_reserve_pins(void);
static void ew24b00yly_gpio_release_pins(void);
__init static int ew24b00yly_pwm_reserve_channels(void);
static void ew24b00yly_pwm_release_channels(void);

/**
 * enum ew24b00yly_gpio_id - Input/output pins that are used for communication
 *                           with the LCD
 *
 * | ATNGW100          || EW24B00YLY
 * |-------------------||-------------------
 * | NAME | CON. | PIN || NAME   | PIN
 * |======|======|=====||========|=======
 * | PE03 | J7   | 01  || D0     | 11
 * | PE04 | J7   | 02  || D1     | 12
 * | PE05 | J7   | 03  || D2     | 13
 * | PE06 | J7   | 04  || D3     | 14
 * | PE07 | J7   | 05  || D4     | 15
 * | PE08 | J7   | 09  || D5     | 16
 * | PE09 | J7   | 10  || D6     | 17
 * | PE10 | J7   | 11  || D7     | 18
 * |------|------|-----||--------|-------
 * | PC31 | J7   | 06  || WR     | 05
 * | PD00 | J7   | 07  || RD     | 06
 * | PD01 | J7   | 08  || CE     | 07
 * | PE11 | J7   | 12  || CD     | 08
 * | PE12 | J7   | 13  || RESET  | 10
 * |------|------|-----||--------|-------
 * | PA28 | J5   | 20  || VEE    | VEE generator
 * | PA29 | J5   | 21  || BLIGHT | power transistor
 */
enum ew24b00yly_gpio_id {
	EW24B00YLY_GPIO_ID_D0 = GPIO_PIN_PE(3),
	EW24B00YLY_GPIO_ID_D1 = GPIO_PIN_PE(4),
	EW24B00YLY_GPIO_ID_D2 = GPIO_PIN_PE(5),
	EW24B00YLY_GPIO_ID_D3 = GPIO_PIN_PE(6),
	EW24B00YLY_GPIO_ID_D4 = GPIO_PIN_PE(7),
	EW24B00YLY_GPIO_ID_D5 = GPIO_PIN_PE(8),
	EW24B00YLY_GPIO_ID_D6 = GPIO_PIN_PE(9),
	EW24B00YLY_GPIO_ID_D7 = GPIO_PIN_PE(10),
	EW24B00YLY_GPIO_ID_WR = GPIO_PIN_PC(31),
	EW24B00YLY_GPIO_ID_RD = GPIO_PIN_PD(0),
	EW24B00YLY_GPIO_ID_CE = GPIO_PIN_PD(1),
	EW24B00YLY_GPIO_ID_CD = GPIO_PIN_PE(11),
	EW24B00YLY_GPIO_ID_RST = GPIO_PIN_PE(12),
};

/**
 * enum ew24b00yly_gpio_dir - pin directions
 */
enum ew24b00yly_gpio_dir {
	EW24B00YLY_GPIO_DIR_IN, /* input */
	EW24B00YLY_GPIO_DIR_OUT, /* output */
	EW24B00YLY_GPIO_DIR_BI /* bidirectional */
};

/**
 * struct ew24b00yly_gpio_conf - it holds base configuration information for
 *                               a particular pin
 * @name: pin name
 * @gpio: gpio pin number
 * @dir: pin direction (in, out, bi)
 * @value: pin default value (only relevant if its direction is output)
 */
struct ew24b00yly_gpio_conf {
	const char* name;
	const enum ew24b00yly_gpio_id id;
	const enum ew24b00yly_gpio_dir dir;
	const int value;
};

/*
 * Holds information for all pins
 */
static const struct ew24b00yly_gpio_conf ew24b00yly_gpios_conf[] = {
	{
		.name = __stringify(EW24B00YLY_LCD_D0_PIN),
		.id = EW24B00YLY_GPIO_ID_D0,
		.dir = EW24B00YLY_GPIO_DIR_BI,
	},
	{
		.name = __stringify(EW24B00YLY_LCD_D1_PIN),
		.id = EW24B00YLY_GPIO_ID_D1,
		.dir = EW24B00YLY_GPIO_DIR_BI,
	},
	{
		.name = __stringify(EW24B00YLY_LCD_D2_PIN),
		.id = EW24B00YLY_GPIO_ID_D2,
		.dir = EW24B00YLY_GPIO_DIR_BI,
	},
	{
		.name = __stringify(EW24B00YLY_LCD_D3_PIN),
		.id = EW24B00YLY_GPIO_ID_D3,
		.dir = EW24B00YLY_GPIO_DIR_BI,
	},
	{
		.name = __stringify(EW24B00YLY_LCD_D4_PIN),
		.id = EW24B00YLY_GPIO_ID_D4,
		.dir = EW24B00YLY_GPIO_DIR_BI,
	},
	{
		.name = __stringify(EW24B00YLY_LCD_D5_PIN),
		.id = EW24B00YLY_GPIO_ID_D5,
		.dir = EW24B00YLY_GPIO_DIR_BI,
	},
	{
		.name = __stringify(EW24B00YLY_LCD_D6_PIN),
		.id = EW24B00YLY_GPIO_ID_D6,
		.dir = EW24B00YLY_GPIO_DIR_BI,
	},
	{
		.name = __stringify(EW24B00YLY_LCD_D7_PIN),
		.id = EW24B00YLY_GPIO_ID_D7,
		.dir = EW24B00YLY_GPIO_DIR_BI,
	},
	{
		.name = __stringify(EW24B00YLY_LCD_WR_PIN),
		.id = EW24B00YLY_GPIO_ID_WR,
		.dir = EW24B00YLY_GPIO_DIR_OUT,
		.value = 1, /* We are not writing */
	},
	{
		.name = __stringify(EW24B00YLY_LCD_RD_PIN),
		.id = EW24B00YLY_GPIO_ID_RD,
		.dir = EW24B00YLY_GPIO_DIR_OUT,
		.value = 1, /* We are not reading */
	},
	{
		.name = __stringify(EW24B00YLY_LCD_CE_PIN),
		.id = EW24B00YLY_GPIO_ID_CE,
		.dir = EW24B00YLY_GPIO_DIR_OUT,
		.value = 1, /* The LCD (chip enable) is disabled */
	},
	{
		.name = __stringify(EW24B00YLY_LCD_CD_PIN),
		.id = EW24B00YLY_GPIO_ID_CD,
		.dir = EW24B00YLY_GPIO_DIR_OUT,
		.value = 1,
	},
	{
		.name = __stringify(EW24B00YLY_LCD_RST_PIN),
		.id = EW24B00YLY_GPIO_ID_RST,
		.dir = EW24B00YLY_GPIO_DIR_OUT,
		.value = 1, /* The LCD is not in reset state by default */
	},
};

/**
 * ew24b00yly_gpio_reserve_pins() - It checks every pin if it exists and then
 * 	                            reserves it. It also sets pins to their
 *                                  default value.
 * @return: The return value is zero for success, else a negative errno.
 *
 * Note: Bidirectional pins are set as inputs by default.
 * On error all reserved pins get released.
 */
__init static int ew24b00yly_gpio_reserve_pins(void)
{
	unsigned int i;
	int ret;
	/* GPIO reservations */
	for (i = 0; i < ARRAY_SIZE(ew24b00yly_gpios_conf); i++) {
		const struct ew24b00yly_gpio_conf *const gpio_conf =
			&ew24b00yly_gpios_conf[i];

		/* reserve current pin */
		if (0 != (ret = gpio_request(gpio_conf->id, gpio_conf->name)))
			goto error;

		/* set current pin direction */
		switch(gpio_conf->dir) {
		default: /* It is more safe the pin is configured as input */
			printk(KERN_WARNING "%s: The pin direction %d not handled.",
			       ew24b00yly_name, gpio_conf->dir);
		case EW24B00YLY_GPIO_DIR_IN:
		case EW24B00YLY_GPIO_DIR_BI:
			ret = gpio_direction_input(gpio_conf->id);
			break;
		case EW24B00YLY_GPIO_DIR_OUT:
			ret = gpio_direction_output(gpio_conf->id, gpio_conf->value);
			break;
		} /* switch */
		if (0 != ret)
			goto error;
	} /* for ... */

	return 0;

error:
	/* release already reserved pins */
	while (i--)
		gpio_free(ew24b00yly_gpios_conf[i].id);

	printk(KERN_ERR "%s: Necessary pins to drive the lcd are "
	       "already in use by an other driver!\n", ew24b00yly_name);
	return ret;
}

/**
 * ew24b00yly_gpio_release_pins() - It releases previously reserved pins
 */
static void ew24b00yly_gpio_release_pins(void)
{
	unsigned int i;
	for (i = 0; i < ARRAY_SIZE(ew24b00yly_gpios_conf); i++)
		gpio_free(ew24b00yly_gpios_conf[i].id);
}


/**
 * struct ew24b00yly_pwm_conf - it holds base configuration information
 *                              for a particular pwm channel
 * @channel: pwm channel
 * @id: channel id (physical)
 * @polarity: if '1' idle cycle is high and duty cycle is low
 * @center_align: '1' means center aligned, '0' means left aligned
 * @clk_prescaler: clock prescaler 2^pre where the pre is [1..10]
 * @period: period
 * @duty_cycle: duty cycle
 * @enable: if '1' the channel will be enabled at configure time
 */
struct ew24b00yly_pwm_conf
{
	spinlock_t lock;
	struct pwm_channel channel;
	const int id;
	const int polarity;
	const int center_align;
	const uint32_t prescaler;
	const uint32_t period;
	uint32_t duty_cycle;
	int enabled;
};

/**
 * enum ew24b00yly_pwm_id - Defines logical identities which point to a pwm
 *                          channel defined in the ew24b00yly_lcd_pwm_channels[].
 */
enum ew24b00yly_pwm_id {
	/* PMW channel for generating a negative voltage (VEE) for the LCD */
	EW24B00YLY_PWM_ID_VEE = 0,
	/* The LCD back light */
	EW24B00YLY_PWM_ID_BLIGHT
};

/* Here are configuration for the pwm channels */
static struct ew24b00yly_pwm_conf ew24b00yly_pwms_conf[] = {
	[EW24B00YLY_PWM_ID_VEE] = {
		.lock = SPIN_LOCK_UNLOCKED,
		.id = 0, /* PWM0 */
		.polarity = 0,
		.center_align = 0,
		.prescaler = 0, /* MCK / 2^0, 68 kHz */
		.period = 1024,
		.duty_cycle = 150, /* 512 max. */
		.enabled = 0, /* must be disabled, initially */
	},
	[EW24B00YLY_PWM_ID_BLIGHT] = {
		.lock = SPIN_LOCK_UNLOCKED,
		.id = 1, /* PWM1 */
		.polarity = 0,
		.center_align = 0,
		.prescaler = 9, /* MCK / 2^9, 500 Hz */
		.period = 1024,
		.duty_cycle = 512,
		.enabled = 0, /* disabled, initially */
	}
};

/**
 * ew24b00yly_pwm_reserve_channels() - It checks every pwm channel if it exists
 *                                     and then reserves and configures it.
 * @return: The return value is zero for success, else a negative errno.
 */
__init static int ew24b00yly_pwm_reserve_channels(void)
{
	int ret;
	unsigned int i;
	for (i = 0; i < ARRAY_SIZE(ew24b00yly_pwms_conf); i++) {
		uint32_t mode_reg;
		struct ew24b00yly_pwm_conf *const ch =
			&ew24b00yly_pwms_conf[i];
		if ((ret = pwm_channel_alloc(ch->id,
		                             &ch->channel))) {
			printk(KERN_ERR "%s: Failed to allocate PWM channel=%d, "
			       "error=%d!\n", ew24b00yly_name, ch->id, ret);
			goto error;
		}

		mode_reg = 0x00000000;
		if (ch->polarity)
			mode_reg |= PWM_CPR_CPOL;
		if (ch->center_align)
			mode_reg |= PWM_CPR_CALG;
		mode_reg |= (ch->prescaler & PWM_CPR_CPRE);
		pwm_channel_writel(&ch->channel, PWM_CMR, mode_reg);
		pwm_channel_writel(&ch->channel, PWM_CPRD, ch->period);
		pwm_channel_writel(&ch->channel, PWM_CDTY, ch->duty_cycle);
		if (ch->enabled)
			pwm_channel_enable(&ch->channel);
	}

	return 0;

error:
	/* release already reserved pwm channels */
	while (i--)
		pwm_channel_free(&ew24b00yly_pwms_conf[i].channel);
	return ret;
}

/**
 * ew24b00yly_pwm_release_channels() - It releases previously reserved pwm
 *                                     channels
 */
static void ew24b00yly_pwm_release_channels(void)
{
	unsigned int i;
	for (i = 0; i < ARRAY_SIZE(ew24b00yly_pwms_conf); i++) {
		struct ew24b00yly_pwm_conf *const ch =
			&ew24b00yly_pwms_conf[i];
		pwm_channel_free(&ch->channel);
	}
}

/**
 * ew24b00yly_pwm_is_enabled() - Returns 1 whether the PWM channel is enabled
 */
static int ew24b00yly_pwm_is_enabled(enum ew24b00yly_pwm_id channel_id)
{
	return ew24b00yly_pwms_conf[channel_id].enabled;
}

/**
 * ew24b00yly_pwm_enable() - Enables an PWM channel
 */
static void ew24b00yly_pwm_enable(enum ew24b00yly_pwm_id channel_id)
{
	struct ew24b00yly_pwm_conf *const ch =
		&ew24b00yly_pwms_conf[channel_id];

	spin_lock_irq(&ch->lock);

	pwm_channel_enable(&ch->channel);
	ch->enabled = 1;

	spin_unlock_irq(&ch->lock);
}

/**
 * ew24b00yly_pwm_disable() - Disables an PWM channel
 */
static void ew24b00yly_pwm_disable(enum ew24b00yly_pwm_id channel_id)
{
	struct ew24b00yly_pwm_conf *const ch =
		&ew24b00yly_pwms_conf[channel_id];

	spin_lock_irq(&ch->lock);

	pwm_channel_disable(&ch->channel);
	ch->enabled = 0;

	spin_unlock_irq(&ch->lock);
}

/**
 * ew24b00yly_pwm_update_dc() - Updates an PWM channel duty cycle
 */
static int ew24b00yly_pwm_update_dc(enum ew24b00yly_pwm_id channel_id,
                                    uint32_t duty_cycle)
{
	int ret = 0;
	struct ew24b00yly_pwm_conf *const ch =
		&ew24b00yly_pwms_conf[channel_id];

	spin_lock_irq(&ch->lock);

	if (duty_cycle > ch->period)
		ret = -EINVAL;
	else {
		pwm_channel_writel(&ch->channel, PWM_CUPD, duty_cycle);
		ch->duty_cycle = duty_cycle;
	}

	spin_unlock_irq(&ch->lock);
	return ret;
}


/*
 ******************************************************************************
 * Bit level functions for the LCD driving
 ******************************************************************************
 */


/* Prototypes */
static void ew24b00yly_lcd_vee_on(void);
static int ew24b00yly_lcd_vee_status(void);
static void ew24b00yly_lcd_vee_off(void);
static void ew24b00yly_lcd_blight_tmr_handler(unsigned long data);
static void ew24b00yly_lcd_blight_on(void);
static void ew24b00yly_lcd_blight_off(void);
static void ew24b00yly_lcd_reset(void);
static void ew24b00yly_lcd_write_byte(uint8_t value);
static uint8_t ew24b00yly_lcd_read_byte(void);
static void ew24b00yly_lcd_write_cmd(uint8_t cmd);
static void ew24b00yly_lcd_write_data(uint8_t value);
static uint8_t ew24b00yly_lcd_read_status(void);
static void ew24b00yly_lcd_check(void);
static void ew24b00yly_lcd_wait(uint8_t flags);

/**
 * ew24b00yly_lcd_vee_on() - Switches on the voltage VEE generator.
 */
static void ew24b00yly_lcd_vee_on(void)
{
	if (!ew24b00yly_pwm_is_enabled(EW24B00YLY_PWM_ID_VEE)) {
		msleep(50);
		ew24b00yly_pwm_enable(EW24B00YLY_PWM_ID_VEE);
	}
}

/**
 * ew24b00yly_lcd_vee_status() - Returns 1 whether the VEE generator is running.
 */
static int ew24b00yly_lcd_vee_status(void)
{
	return ew24b00yly_pwm_is_enabled(EW24B00YLY_PWM_ID_VEE);
}

/**
 * ew24b00yly_lcd_vee_off() - Switches off the voltage VEE generator.
 */
static void ew24b00yly_lcd_vee_off(void)
{
	if (ew24b00yly_pwm_is_enabled(EW24B00YLY_PWM_ID_VEE)) {
		ew24b00yly_pwm_disable(EW24B00YLY_PWM_ID_VEE);
		msleep(10);
	}
}

/**
 * ew24b00yly_lcd_vee_max_level() - Returns max level of the voltage VEE
 *                                  generator.
 */
static uint32_t ew24b00yly_lcd_vee_max_level(void)
{
	return ew24b00yly_pwms_conf[EW24B00YLY_PWM_ID_VEE].period;
}

/**
 * ew24b00yly_lcd_vee_set_level() - Sets level of the voltage VEE generator.
 */
static int ew24b00yly_lcd_vee_set_level(uint32_t level)
{
	return ew24b00yly_pwm_update_dc(EW24B00YLY_PWM_ID_VEE, level);
}

/**
 * ew24b00yly_lcd_vee_max_level() - Returns current level of the voltage VEE
 *                                  generator.
 */
static uint32_t ew24b00yly_lcd_vee_get_level(void)
{
	return ew24b00yly_pwms_conf[EW24B00YLY_PWM_ID_VEE].duty_cycle;
}

/* Default period of time in seconds to delay back light turning off */
#define EW24B00YLY_LCD_BLIGHT_PERIOD 30
/* Current period of time in seconds to delay back light turning off */
static unsigned long ew24b00yly_lcd_blight_interval =
	EW24B00YLY_LCD_BLIGHT_PERIOD;
/* Timer used from the LCD back light */
static struct timer_list ew24b00yly_lcd_blight_tmr =
	TIMER_INITIALIZER(ew24b00yly_lcd_blight_tmr_handler, 0, 0);
/* The LCD back light access lock */
static spinlock_t ew24b00yly_lcd_blight_lock = SPIN_LOCK_UNLOCKED;
/* The LCD back light modes which say how it is turned on and off */
enum ew24b00yly_lcd_blight_mode {
	EW24B00YLY_LCD_BLIGHT_MODE_OFF = 0, /* Always off */
	EW24B00YLY_LCD_BLIGHT_MODE_ON, /* Always on */
	EW24B00YLY_LCD_BLIGHT_MODE_AUTO, /* Gets turned on for a period of time */
};
/* The names for the enumerator above (they only exist for attributes) */
static const char* ew24b00yly_lcd_blight_mode_names[] = {
	"off", "on", "auto"
};
/* Current the LCD  back light mode */
static enum ew24b00yly_lcd_blight_mode ew24b00yly_lcd_blight_mode =
	EW24B00YLY_LCD_BLIGHT_MODE_AUTO;

/**
 * ew24b00yly_lcd_blight_tmr_handler() - The LCD back light timer handler
 */
static void ew24b00yly_lcd_blight_tmr_handler(unsigned long data)
{
	ew24b00yly_lcd_blight_off();
}

/**
 * ew24b00yly_lcd_blight_on() - It turns the LCD back light on for a given
 *                              period of time (it depends on the mode)
 *
 * This function can also be called from the atomic context so no sleeping
 * here :)
 */
static void ew24b00yly_lcd_blight_on(void)
{
	spin_lock_irq(&ew24b00yly_lcd_blight_lock);

	switch(ew24b00yly_lcd_blight_mode) {
	case EW24B00YLY_LCD_BLIGHT_MODE_OFF: /* do nothing */
		break;
	case EW24B00YLY_LCD_BLIGHT_MODE_AUTO: /* start the timer */
		mod_timer(&ew24b00yly_lcd_blight_tmr,
		          jiffies + (HZ * ew24b00yly_lcd_blight_interval));
	case EW24B00YLY_LCD_BLIGHT_MODE_ON: /* turn off the light */
		if (!ew24b00yly_pwm_is_enabled(EW24B00YLY_PWM_ID_BLIGHT))
			ew24b00yly_pwm_enable(EW24B00YLY_PWM_ID_BLIGHT);
		break;
	}

	spin_unlock_irq(&ew24b00yly_lcd_blight_lock);
}

/**
 * ew24b00yly_lcd_backlight_off() - It turns off the LCD back light
 *
 * This function can also be called from the atomic context so no sleeping
 * here :)
 */
static void ew24b00yly_lcd_blight_off(void)
{
	spin_lock_irq(&ew24b00yly_lcd_blight_lock);

	if (ew24b00yly_pwm_is_enabled(EW24B00YLY_PWM_ID_BLIGHT)) {
		ew24b00yly_pwm_disable(EW24B00YLY_PWM_ID_BLIGHT);
		del_timer(&ew24b00yly_lcd_blight_tmr);
	}

	spin_unlock_irq(&ew24b00yly_lcd_blight_lock);
}

/**
 * ew24b00yly_lcd_backlight_max_level() - Returns the LCD back light brightness
 *                                        max. level
 */
static uint32_t ew24b00yly_lcd_blight_max_level(void)
{
	return ew24b00yly_pwms_conf[EW24B00YLY_PWM_ID_BLIGHT].period;
}

/**
 * ew24b00yly_lcd_backlight_set_level() - Sets the LCD back light brightness
 *                                        level
 */
static int ew24b00yly_lcd_blight_set_level(uint32_t level)
{
	return ew24b00yly_pwm_update_dc(EW24B00YLY_PWM_ID_BLIGHT,
	                               ew24b00yly_lcd_blight_max_level() - level);
}

/**
 * ew24b00yly_lcd_backlight_get_level() - Gets the LCD current back light
 *                                        brightness level
 */
static uint32_t ew24b00yly_lcd_blight_get_level(void)
{
	return ew24b00yly_pwms_conf[EW24B00YLY_PWM_ID_BLIGHT].duty_cycle;
}

/**
 * ew24b00yly_lcd_reset() - It resets the LCD
 */
static void ew24b00yly_lcd_reset(void)
{
	const int vee_status = ew24b00yly_lcd_vee_status();
	if (vee_status)
		ew24b00yly_lcd_vee_off();

	gpio_set_value(EW24B00YLY_GPIO_ID_RST, 0);
	msleep(1);
	gpio_set_value(EW24B00YLY_GPIO_ID_RST, 1);

	if (vee_status)
		ew24b00yly_lcd_vee_on();

	printk(KERN_INFO "%s: The lcd has been reset.\n", ew24b00yly_name);
}

/*
 * It holds list of data pins
 */
static const enum ew24b00yly_gpio_id ew24b00yly_lcd_data_gpio_ids[] = {
	EW24B00YLY_GPIO_ID_D0, EW24B00YLY_GPIO_ID_D1,
	EW24B00YLY_GPIO_ID_D2, EW24B00YLY_GPIO_ID_D3,
	EW24B00YLY_GPIO_ID_D4, EW24B00YLY_GPIO_ID_D5,
	EW24B00YLY_GPIO_ID_D6, EW24B00YLY_GPIO_ID_D7
};

/**
 * ew24b00yly_lcd_write_byte() - Writes a byte to the LCD
 * @value: The value will be written
 *
 * Do not use this function directly!
 */
static void ew24b00yly_lcd_write_byte(uint8_t value)
{
	unsigned int i;
	gpio_set_value(EW24B00YLY_GPIO_ID_CE, 0); /* put down pin CE */
	gpio_set_value(EW24B00YLY_GPIO_ID_WR, 0); /* put down pin WR */
	for (i = 0; i < ARRAY_SIZE(ew24b00yly_lcd_data_gpio_ids); i++) {
		gpio_direction_output(ew24b00yly_lcd_data_gpio_ids[i],
		                      value & 0x01);
		value >>= 1;
	}
	ndelay(80); /* data set up time */
	gpio_set_value(EW24B00YLY_GPIO_ID_WR, 1); /* put up pin WR */
	gpio_set_value(EW24B00YLY_GPIO_ID_CE, 1); /* put up pin CE */
	ndelay(40); /* data hold time */
}

/**
 * ew24b00yly_lcd_read_byte() - Reads a byte from the LCD
 * @return: The read value
 *
 * Do not use this function directly!
 */
static uint8_t ew24b00yly_lcd_read_byte(void)
{
	unsigned int i;
	uint8_t value = 0x00;

	gpio_set_value(EW24B00YLY_GPIO_ID_CE, 0); /* put down pin CE */
	gpio_set_value(EW24B00YLY_GPIO_ID_RD, 0); /* put down pin RD */
	for (i = 0; i < ARRAY_SIZE(ew24b00yly_lcd_data_gpio_ids); i++)
		gpio_direction_input(ew24b00yly_lcd_data_gpio_ids[i]);
	ndelay(150); /* data access time */
	for (i = ARRAY_SIZE(ew24b00yly_lcd_data_gpio_ids) - 1;
	     i < ARRAY_SIZE(ew24b00yly_lcd_data_gpio_ids); i--) {
		value <<= 1;
		value |= ((uint8_t)gpio_get_value(
		                     ew24b00yly_lcd_data_gpio_ids[i]) & 0x01);
	}
	gpio_set_value(EW24B00YLY_GPIO_ID_RD, 1); /* put up pin RD */
	gpio_set_value(EW24B00YLY_GPIO_ID_CE, 1); /* put up pin CE */
	ndelay(50); /* data hold time */
	return value;
}


/*
 ******************************************************************************
 * Byte level functions for the LCD driving
 ******************************************************************************
 */


/* Prevents simultaneous accesses to the LCD */
static spinlock_t ew24b00yly_lcd_lock = SPIN_LOCK_UNLOCKED;
/* Last LCD error reported by one of the functions below */
static int ew24b00yly_lcd_error = -EPERM;

/**
 * ew24b00yly_lcd_write_cmd() - Writes a command to the LCD
 * @value: The value will be processed
 *
 * This function uses spin_lock to lock the LCD.
 */
static void ew24b00yly_lcd_write_cmd(uint8_t cmd)
{
	spin_lock(&ew24b00yly_lcd_lock);
	gpio_set_value(EW24B00YLY_GPIO_ID_CD, 1); /* put put pin CD */
	ew24b00yly_lcd_write_byte(cmd);
	spin_unlock(&ew24b00yly_lcd_lock);
}

/**
 * ew24b00yly_lcd_write_data() - Writes a byte to the LCD
 * @value: The value will be written
 *
 * This function uses spin_lock to lock the LCD.
 */
static void ew24b00yly_lcd_write_data(uint8_t value)
{
	spin_lock(&ew24b00yly_lcd_lock);
	gpio_set_value(EW24B00YLY_GPIO_ID_CD, 0); /* put down pin CD */
	ew24b00yly_lcd_write_byte(value);
	spin_unlock(&ew24b00yly_lcd_lock);
}

/**
 * ew24b00yly_lcd_read_status() - Reads LCD status from the LCD
 * @return: LCD status
 *
 * This function uses spin_lock to lock the LCD.
 */
static uint8_t ew24b00yly_lcd_read_status(void)
{
	uint8_t status = 0x00;

	spin_lock(&ew24b00yly_lcd_lock);
	gpio_set_value(EW24B00YLY_GPIO_ID_CD, 1); /* put up pin CD */
	status = ew24b00yly_lcd_read_byte();
	spin_unlock(&ew24b00yly_lcd_lock);

	return status;
}

/**
 * ew24b00yly_lcd_check() - Checks the LCD status.
 *
 * if it, the LCD, does not respond it gets reset and checked again.
 * The function also clear the lcd error.
 *
 * The following flags need to be set:
 * STA0 Check capability of command execution = 1
 * STA1 Check capability of data read/write = 1
 * STA5 Check capability of controller operation = 1
 */
static void ew24b00yly_lcd_check(void)
{
	int status;

	ew24b00yly_lcd_error = 0;

	printk(KERN_INFO "%s: Checking the lcd.\n", ew24b00yly_name);
	status = ew24b00yly_lcd_read_status();
	if (!(0x23 == status)) {
		printk(KERN_ERR "%s: The lcd has returned unexpected status "
		       "(status=0x%x) \n", ew24b00yly_name, status);
		ew24b00yly_lcd_reset();
		msleep(5);
		status = ew24b00yly_lcd_read_status();
		if (!(0x23 == status)) {
			ew24b00yly_lcd_error = -EIO;
			printk(KERN_CRIT "%s: It seems like the lcd isn't "
			       "working properly or it's unplugged (status=0x%x)!\n",
			       ew24b00yly_name, status);
		}
	}
}

/**
 * ew24b00yly_lcd_wait() - Waits for the LCD until next command or data can
 *                         be accepted by it
 * @flags: Wait until these flags aren't set
 *
 * Implemented by using busy-loop
 */
static void ew24b00yly_lcd_wait(uint8_t flags)
{
	unsigned int timeout;

	if (ew24b00yly_lcd_error && printk_ratelimit()) {
		printk(KERN_ERR "%s: The lcd is in the error state (err: %d)!\n",
		       ew24b00yly_name, ew24b00yly_lcd_error);
		return;
	}

	timeout = 0;
	for(;;) {
		const uint8_t status = ew24b00yly_lcd_read_status();
		if ((status & flags) == flags)
			break;
		else if (0xFF == status) { /* most probably unplugged */
			ew24b00yly_lcd_error = -EIO;
			printk(KERN_ERR "%s: The lcd is probably unplugged "
			       "(status: C=0x%x,E=0x%x)!\n",
			       ew24b00yly_name, status, flags);
			break;
		} else if (timeout >= 10) { /* timeout */
			ew24b00yly_lcd_error = -EBUSY;
			printk(KERN_ERR "%s: The lcd is not responding "
			       "(status: C=0x%x,E=0x%x)!\n",
			       ew24b00yly_name, status, flags);
			break;
		} /* if timeout */

		udelay(10); /* suitable for LCDs which operate at 3MHz system clock */
		timeout++;
	} /* for ever */
}

/*
 ******************************************************************************
 * Functions which communicate with the LCD controller (stream level).
 ******************************************************************************
 */

/* Prototypes */
static void ew24b00yly_lcd_invoke_cmd0(uint8_t cmd);
static void ew24b00yly_lcd_invoke_cmd2(uint8_t arg1, uint8_t arg2, uint8_t cmd);
static void ew24b00yly_lcd_update_mode(void);
static void ew24b00yly_lcd_on(void);
static void ew24b00yly_lcd_off(void);
static void ew24b00yly_lcd_cursor_on(void);
static void ew24b00yly_lcd_cursor_off(void);
static void ew24b00yly_lcd_set_cursor_pos(uint8_t col, uint8_t row);
static void ew24b00yly_lcd_set_cursor_thickness(int thickness);
static void ew24b00yly_lcd_set_ap(uint16_t address);
static void ew24b00yly_lcd_putcs(uint16_t address, const uint16_t* buffer,
                                 size_t count);
static void ew24b00yly_lcd_memset(uint16_t address, uint8_t val, size_t count);
__init static int ew24b00yly_lcd_init(void);
static void ew24b00yly_lcd_cleanup(void);


/**
 * enum ew24b00yly_lcd_cmds - The LCD commands
 *
 * Only fixed (not compounded) are listed here
 */
enum ew24b00yly_lcd_cmds
{
	/* Cursor Pointer Set */
	EW24B00YLY_LCD_CMD_CP_SET = 0x21,
	/* Offset Register Set */
	EW24B00YLY_LCD_CMD_OR_SET = 0x22,
	/* Address Pointer Set */
	EW24B00YLY_LCD_CMD_AP_SET = 0x24,
	/* Text Home Address Set */
	EW24B00YLY_LCD_CMD_TH_ADDR_SET = 0x40,
	/* Text Area Set */
	EW24B00YLY_LCD_CMD_TA_SET = 0x41,
	/* Graphic Home Address Set */
	EW24B00YLY_LCD_CMD_GH_ADDR_SET = 0x42,
	/* Graphic Area Set */
	EW24B00YLY_LCD_CMD_GA_SET = 0x43,
	/* Data Auto Write Set */
	EW24B00YLY_LCD_CMD_DAW_SET = 0xB0,
	/* Data Auto Read Set */
	EW24B00YLY_LCD_CMD_DAR_SET = 0xB1,
	/* Auto Mode Reset (B2H or B3H) */
	EW24B00YLY_LCD_CMD_AM_RESET = 0xB2,
	/* Data Write - Address Pointer Auto Incremented */
	EW24B00YLY_LCD_CMD_DAW_INC_AP = 0xC0,
	/* Data Write - Address Pointer Auto Decremented */
	EW24B00YLY_LCD_CMD_DAW_DEC_AP = 0xC2,
	/* Data Write - Address Pointer Auto Unchanged */
	EW24B00YLY_LCD_CMD_DAW_UNCH_AP = 0xC4,
};

/* Max. number of visible rows */
#define EW24B00YLY_LCD_ROWS 8
/* Max. number of visible columns in a row */
#define EW24B00YLY_LCD_COLS 40

/*
 * 0000H - 0780H: Text RAM area
 * 0780H - 0F00H: Text attribute RAM area
 * 1C00H - 2000H: CG-RAM area
 */

/* Text home address */
#define EW24B00YLY_TH_ADDRESS 0x0000
/* Text Area */
#define EW24B00YLY_T_AREA 0x28
/* Graphic home address */
#define EW24B00YLY_GH_ADDRESS 0x0780
/* Graphic Area */
#define EW24B00YLY_G_AREA EW24B00YLY_T_AREA
/* Offset register */
#define EW24B00YLY_OR 0x03


/**
 * ew24b00yly_lcd_invoke_cmd0() - Invokes a given command on the LCD
 * @cmd: the command
 */
static void ew24b00yly_lcd_invoke_cmd0(uint8_t cmd)
{
	ew24b00yly_lcd_wait(0x03/* LCD busy flags */);
	ew24b00yly_lcd_write_cmd(cmd);
}

/**
 * ew24b00yly_lcd_invoke_cmd2() - Invokes a given two arguments command on the LCD
 * @arg1: the command argument 1
 * @arg2: the command argument 2
 * @cmd: the command
 */
static void ew24b00yly_lcd_invoke_cmd2(uint8_t arg1, uint8_t arg2, uint8_t cmd)
{
	ew24b00yly_lcd_wait(0x03/* LCD busy flags */);
	ew24b00yly_lcd_write_data(arg1);
	ew24b00yly_lcd_write_data(arg2);
	ew24b00yly_lcd_write_cmd(cmd);
}

/* Display mode */
static uint8_t ew24b00yly_lcd_mode = 0x91; /* display off, cursor blinking */

/**
 * ew24b00yly_lcd_update_mode() - It updates current LCD mode to the LCD if
 *                                necessary.
 */
static void ew24b00yly_lcd_update_mode(void)
{
	static int old = -1;
	if (old != ew24b00yly_lcd_mode) {
		ew24b00yly_lcd_invoke_cmd0(ew24b00yly_lcd_mode);
		old = ew24b00yly_lcd_mode;
	}
}

/**
 * ew24b00yly_lcd_on() - It turns the LCD screen on also back light and
 *                       VEE generator.
 */
static void ew24b00yly_lcd_on(void)
{
	ew24b00yly_lcd_mode |= 0x04;
	ew24b00yly_lcd_update_mode();
	ew24b00yly_lcd_vee_on();
	ew24b00yly_lcd_blight_on();
}

/**
 * ew24b00yly_lcd_off() - It turns the LCD screen off and also back light and
 *                        VEE generator.
 */
static void ew24b00yly_lcd_off(void)
{
	if (!ew24b00yly_lcd_error) {
		ew24b00yly_lcd_mode &= ~0x04;
		ew24b00yly_lcd_update_mode();
	}
	ew24b00yly_lcd_vee_off();
	ew24b00yly_lcd_blight_off();
}

/**
 * ew24b00yly_lcd_cursor_on() - It turns the LCD cursor on.
 */
static void ew24b00yly_lcd_cursor_on(void)
{
	ew24b00yly_lcd_mode |= 0x02;
	ew24b00yly_lcd_update_mode();
}

/**
 * ew24b00yly_lcd_cursor_off() - It turns the LCD cursor off
 */
static void ew24b00yly_lcd_cursor_off(void)
{
	ew24b00yly_lcd_mode &= ~0x02;
	ew24b00yly_lcd_update_mode();
}

/**
 * ew24b00yly_lcd_set_cursor_pos - Sets the LCD cursor position
 * @col: coloumn
 * @row: row
 */
static void ew24b00yly_lcd_set_cursor_pos(uint8_t col, uint8_t row)
{
	ew24b00yly_lcd_invoke_cmd2(col, row, EW24B00YLY_LCD_CMD_CP_SET);
}

/**
 * ew24b00yly_lcd_set_cursor_pos - Sets the LCD cursor thickness
 * @thickness: Thickness in pixels. Valid values are from 1 to 8.
 */
static void ew24b00yly_lcd_set_cursor_thickness(int thickness)
{
	if (thickness > 0 && thickness < 9)
		ew24b00yly_lcd_invoke_cmd0(0xA0 + thickness - 1);
}


/**
 * ew24b00yly_lcd_set_ap - Sets the LCD address pointer
 * @address: New address
 */
static void ew24b00yly_lcd_set_ap(uint16_t address)
{
	ew24b00yly_lcd_invoke_cmd2(EW24B00YLY_VAL16_LO(address), /* lo */
	                           EW24B00YLY_VAL16_HI(address), /* hi */
	                           EW24B00YLY_LCD_CMD_AP_SET);
}

/**
 * ew24b00yly_lcd_putcs() - Writes text to the LCD
 * @address: Start writing address
 * @buffer: Screen buffer
 * @count: Number of characters to write into the LCD
 *
 * It writes block of text into the LCD RAM. It filters out all attributes.
 */
static void ew24b00yly_lcd_putcs(uint16_t address, const uint16_t* buffer,
                                 size_t count)
{
	int i;

	/* characters */
	ew24b00yly_lcd_set_ap(address);
	ew24b00yly_lcd_invoke_cmd0(EW24B00YLY_LCD_CMD_DAW_SET);
	ew24b00yly_lcd_wait(0x08/* LCD busy flags */);
	for (i = 0; i < count; i++) {
		uint8_t ch = EW24B00YLY_VAL16_LO(buffer[i]); /*buffer[i] & 0x00FF;*/
		if (ch >= 32)
			ch -= 32; /* see the LCD character set table */
		ew24b00yly_lcd_write_data(ch);
		ew24b00yly_lcd_wait(0x08/* LCD busy flags */);
	}
	ew24b00yly_lcd_invoke_cmd0(EW24B00YLY_LCD_CMD_AM_RESET);

	/* attributes */
	ew24b00yly_lcd_set_ap(EW24B00YLY_GH_ADDRESS + address);
	ew24b00yly_lcd_invoke_cmd0(EW24B00YLY_LCD_CMD_DAW_SET);
	ew24b00yly_lcd_wait(0x08/* LCD busy flags */);
	for (i = 0; i < count; i++) {
		const uint8_t attr = EW24B00YLY_VAL16_HI(buffer[i]);
		ew24b00yly_lcd_write_data(attr);
		ew24b00yly_lcd_wait(0x08/* LCD busy flags */);
	}
	ew24b00yly_lcd_invoke_cmd0(EW24B00YLY_LCD_CMD_AM_RESET);
}

/**
 * ew24b00yly_lcd_memset() - It sets the LCD (RAM)
 * @address: Start writing address
 * @val: The value
 * @count: How many values to write into the LCD
 */
static void ew24b00yly_lcd_memset(uint16_t address, uint8_t val, size_t count)
{
	ew24b00yly_lcd_set_ap(address);
	ew24b00yly_lcd_invoke_cmd0(EW24B00YLY_LCD_CMD_DAW_SET);
	ew24b00yly_lcd_wait(0x08/* LCD busy flags */);
	while (count--) {
		ew24b00yly_lcd_write_data(val);
		ew24b00yly_lcd_wait(0x08/* LCD busy flags */);
	}
	ew24b00yly_lcd_invoke_cmd0(EW24B00YLY_LCD_CMD_AM_RESET);
}

static struct notifier_block ew24b00yly_kbd_nb;

/**
 * ew24b00yly_lcd_init() - Initializes the LCD
 */
__init static int ew24b00yly_lcd_init(void)
{
	int ret;

	/* Prepare IO pins */
	if ((ret = ew24b00yly_gpio_reserve_pins()))
		goto gpio_error;
	if ((ret = ew24b00yly_pwm_reserve_channels()))
		goto pwm_error;

	/* Initialize the LCD */
	ew24b00yly_lcd_reset();
	ew24b00yly_lcd_check();
	if (ew24b00yly_lcd_error)
		goto lcd_init_error;

	/* Mode Set: Text only (with Attribute data in Graphic Area);
	 *           code 00H-7FH; CG-ROM code 80H-FFH; CG-RAM */
	ew24b00yly_lcd_invoke_cmd0(0x84);
	/* Offset register */
	ew24b00yly_lcd_invoke_cmd2(EW24B00YLY_OR, 0x00, EW24B00YLY_LCD_CMD_OR_SET);

	/* Text home address */
	ew24b00yly_lcd_invoke_cmd2(EW24B00YLY_CONST16_LO(EW24B00YLY_TH_ADDRESS),
	                           EW24B00YLY_CONST16_HI(EW24B00YLY_TH_ADDRESS),
	                           EW24B00YLY_LCD_CMD_TH_ADDR_SET);
	/* Text Area */
	ew24b00yly_lcd_invoke_cmd2(EW24B00YLY_T_AREA,
	                           0x00,
	                           EW24B00YLY_LCD_CMD_TA_SET);

	/* Graphic home address */
	ew24b00yly_lcd_invoke_cmd2(EW24B00YLY_CONST16_LO(EW24B00YLY_GH_ADDRESS),
	                           EW24B00YLY_CONST16_HI(EW24B00YLY_GH_ADDRESS),
	                           EW24B00YLY_LCD_CMD_GH_ADDR_SET);
	/* Graphic Area */
	ew24b00yly_lcd_invoke_cmd2(EW24B00YLY_G_AREA,
	                           0x00,
	                           EW24B00YLY_LCD_CMD_GA_SET);

	/* Turn on the display */
	ew24b00yly_lcd_on();
	if (ew24b00yly_lcd_error)
		goto lcd_final_error;

	/* We need keyboard notification in order to turn on the LCD back light */
	register_keyboard_notifier(&ew24b00yly_kbd_nb);

	return 0;

lcd_final_error:
	ret = ew24b00yly_lcd_error;

lcd_init_error:
	ew24b00yly_gpio_release_pins();
	ew24b00yly_pwm_release_channels();
	ret = ew24b00yly_lcd_error;

pwm_error:
gpio_error:
	return ret;

}

/**
 * ew24b00yly_lcd_cleanup() - Cleans up the LCD stuff
 */
static void ew24b00yly_lcd_cleanup(void)
{
	unregister_keyboard_notifier(&ew24b00yly_kbd_nb);

	ew24b00yly_lcd_off();
	ew24b00yly_pwm_release_channels();
	ew24b00yly_gpio_release_pins();
}


/*
 ******************************************************************************
 * Stuff related to keyboard
 ******************************************************************************
 */


/**
 * ew24b00yly_kbd_notifier() - Invoked on keyboard event and it turns on the LCD
 *                             back light.
 *
 * Note: It runs in atomic context!
 */
static int ew24b00yly_kbd_notifier(struct notifier_block* nb,
                                   unsigned long event, void* dummy)
{
	if (KBD_KEYCODE == event)
		ew24b00yly_lcd_blight_on();
	return NOTIFY_OK;
}

/* Keyboard notifier block */
static struct notifier_block ew24b00yly_kbd_nb = {
	.notifier_call = ew24b00yly_kbd_notifier,
};


/*
 ******************************************************************************
 * Stuff related to linux console
 ******************************************************************************
 */


/* Prototypes */
static const char* ew24b00yly_startup(void);
static void ew24b00yly_init(struct vc_data* vc, int init);
static void ew24b00yly_deinit(struct vc_data* vc);
static void ew24b00yly_con_clear(struct vc_data* vc, int sy, int sx, int height,
                          int width);
static void ew24b00yly_con_putc(struct vc_data* vc, int c, int ypos, int xpos);
static void ew24b00yly_con_putcs(struct vc_data* vc, const unsigned short *s,
                          int count, int ypos, int xpos);
static void ew24b00yly_con_cursor(struct vc_data* vc, int mode);
static int ew24b00yly_con_scroll(struct vc_data* vc, int top, int bottom,
                                 int dir, int count);
static void ew24b00yly_con_bmove(struct vc_data* vc, int sy, int sx, int dy,
                                 int dx, int height, int width);
static int ew24b00yly_con_switch(struct vc_data* vc);
static int ew24b00yly_con_blank(struct vc_data* vc, int blank, int mode_switch);
static int ew24b00yly_con_resize(struct vc_data* vc, unsigned int width,
                                 unsigned int height, unsigned int user);
static int ew24b00yly_con_set_palette(struct vc_data* vc, unsigned char *table);
static int ew24b00yly_deltascroll_contents(struct vc_data* vc);
static int ew24b00yly_con_scrolldelta(struct vc_data* vc, int lines);
static int ew24b00yly_con_set_origin(struct vc_data* vc);
static void ew24b00yly_con_save_screen(struct vc_data* vc);
static void ew24b00yly_con_restore_screen(struct vc_data* vc);
static u8 ew24b00yly_con_build_attr(struct vc_data* vc, u8 color, u8 intensity,
                                    u8 blink, u8 underline, u8 reverse, u8 italic);
static void ew24b00yly_con_invert_region(struct vc_data* vc, u16* p, int cnt);
static u16* ew24b00yly_con_screen_pos(struct vc_data* vc, int offset);
static int ew24b00yly_con_dummy(struct vc_data* vc);


/* Start of the range of the overtaken tty-s by this module */
#define EW24B00YLY_FIRST_TTY_NUM 0
/* End of the range of the overtaken tty-s by this module */
#define EW24B00YLY_LAST_TTY_NUM (MAX_NR_CONSOLES - 1)
/* Number of overtaken tty-s */
#define EW24B00YLY_TTY_COUNT \
	(EW24B00YLY_LAST_TTY_NUM - EW24B00YLY_FIRST_TTY_NUM)

/* Number of rows that can be scrolled back */
#define EW24B00YLY_SCROLLDELTA_ROWS (EW24B00YLY_LCD_ROWS * 4)

/**
 * struct ew24b00yly_scrolldelta_buff
 * @buff: Buffer
 * @count: Number of lines present in the buffer
 * @start: Index of the first line in the buffer
 * @end: Index of the first empty line in the buffer
 * @scroll: Number of lines to scroll back.
 */
struct ew24b00yly_scrolldelta_buff {
	uint16_t buff[EW24B00YLY_SCROLLDELTA_ROWS][EW24B00YLY_LCD_COLS];
	int count;
	int end;
	int scroll;
};
/* There is only one scroll delta buffer for all v. consoles. It gets restarted
 * on every console switch */
struct ew24b00yly_scrolldelta_buff ew24b00yly_scrolldelta_buff = {
	.count = 0,
	.end = 0,
	.scroll = 0,
};

/* It holds pointers to saved screens (a screen per v. console) */
static uint16_t* ew24b00yly_saved_screen[EW24B00YLY_TTY_COUNT];


/* Here are bound all the function necessary to serve underlying virtual console */
static struct consw ew24b00yly_consw = {
        .owner = THIS_MODULE,
        .con_startup = ew24b00yly_startup,
        .con_init = ew24b00yly_init,
        .con_deinit = ew24b00yly_deinit,
        .con_clear = ew24b00yly_con_clear,
        .con_putc = ew24b00yly_con_putc,
        .con_putcs = ew24b00yly_con_putcs,
        .con_cursor = ew24b00yly_con_cursor,
        .con_scroll = ew24b00yly_con_scroll,
        .con_bmove = ew24b00yly_con_bmove,
        .con_switch = ew24b00yly_con_switch,
        .con_blank = ew24b00yly_con_blank,
        .con_font_set = (void *)ew24b00yly_con_dummy,
        .con_font_get = (void *)ew24b00yly_con_dummy,
        .con_font_default = (void *)ew24b00yly_con_dummy,
        .con_font_copy = (void *)ew24b00yly_con_dummy,
        .con_resize = ew24b00yly_con_resize,
        .con_set_palette = ew24b00yly_con_set_palette,
        .con_scrolldelta = ew24b00yly_con_scrolldelta,
        .con_set_origin = ew24b00yly_con_set_origin,
        .con_save_screen = ew24b00yly_con_save_screen,
        .con_build_attr = ew24b00yly_con_build_attr,
        .con_invert_region = ew24b00yly_con_invert_region,
        .con_screen_pos = ew24b00yly_con_screen_pos,
};

/**
 * ew24b00yly_startup() - Console startup
 * @return: Module name
 */
static const char* ew24b00yly_startup(void)
{
	return "EW24B00YLY";
}

/**
 * ew24b00yly_init() - ?????
  */
static void ew24b00yly_init(struct vc_data* vc, int init)
{
	vc->vc_can_do_color = 0;
	if (init) {
		vc->vc_rows = EW24B00YLY_LCD_ROWS;
		vc->vc_cols = EW24B00YLY_LCD_COLS;
		vc->vc_top = 0;
		vc->vc_bottom = EW24B00YLY_LCD_ROWS;
	} else
		vc_resize(vc, EW24B00YLY_LCD_COLS, EW24B00YLY_LCD_ROWS);
}

/**
 * ew24b00yly_deinit() - Cleanup the LCD stuff and turns it off
 */
static void ew24b00yly_deinit(struct vc_data* vc)
{
	/* Free up possible saved screen */
	kfree(ew24b00yly_saved_screen[vc->vc_num]);
}

static void ew24b00yly_con_clear(struct vc_data* vc, int sy, int sx, int height,
                                 int width)
{
	while (height--) {
		ew24b00yly_lcd_memset((sy * EW24B00YLY_LCD_COLS) + sx, 0, width);
		sy++;
	}
}

static void ew24b00yly_con_putc(struct vc_data* vc, int c, int ypos, int xpos)
{
	ew24b00yly_lcd_putcs((ypos * EW24B00YLY_LCD_COLS) + xpos,
	                     (uint16_t*)&c,
	                     1);
}

static void ew24b00yly_con_putcs(struct vc_data* vc, const unsigned short *s,
                          int count, int ypos, int xpos)
{
	ew24b00yly_lcd_putcs((ypos * EW24B00YLY_LCD_COLS) + xpos,
	                     s,
	                     count);
}

static void ew24b00yly_con_cursor(struct vc_data* vc, int mode)
{
	switch(mode) {
	case CM_ERASE:
		ew24b00yly_lcd_cursor_off();
		break;
	case CM_MOVE:
		ew24b00yly_lcd_set_cursor_pos(vc->vc_x, vc->vc_y);
		break;
	case CM_DRAW:
		if (ew24b00yly_scrolldelta_buff.scroll) {
			/* get out of the delta scrolling mode */
			ew24b00yly_scrolldelta_buff.scroll = 0;
			ew24b00yly_con_restore_screen(vc);
		}

		ew24b00yly_lcd_set_cursor_pos(vc->vc_x, vc->vc_y);
		switch(vc->vc_cursor_type & 0x0F) {
		default:
		case CUR_UNDERLINE:
			ew24b00yly_lcd_cursor_on();
			ew24b00yly_lcd_set_cursor_thickness(1);
			break;
		case CUR_TWO_THIRDS:
			ew24b00yly_lcd_cursor_on();
			ew24b00yly_lcd_set_cursor_thickness(8);
			break;
		case CUR_LOWER_THIRD:
			ew24b00yly_lcd_cursor_on();
			ew24b00yly_lcd_set_cursor_thickness(2);
			break;
		case CUR_LOWER_HALF:
			ew24b00yly_lcd_cursor_on();
			ew24b00yly_lcd_set_cursor_thickness(4);
			break;
		case CUR_NONE:
			ew24b00yly_lcd_cursor_off();
			break;
		}
		break;
	};
}

static int ew24b00yly_con_scroll(struct vc_data* vc, int top, int bottom,
                                 int dir, int count)
{
	uint16_t* d;
	uint16_t* s;

	switch (dir) {
	case SM_UP:
		/* BUG: If the count is more than one, the first row only gets
		 * saved!! */

		/* add the top line into the scroll delta buffer */
		d = &ew24b00yly_scrolldelta_buff.buff[ew24b00yly_scrolldelta_buff.end][0];
		s = (uint16_t*)(vc->vc_origin + (vc->vc_size_row * top));
		scr_memcpyw(d, s, vc->vc_size_row);
		ew24b00yly_scrolldelta_buff.end++;
		if (ew24b00yly_scrolldelta_buff.end >= EW24B00YLY_SCROLLDELTA_ROWS)
			ew24b00yly_scrolldelta_buff.end = 0;
		if (ew24b00yly_scrolldelta_buff.count < EW24B00YLY_SCROLLDELTA_ROWS)
			ew24b00yly_scrolldelta_buff.count++;

		/* perform scrolling */
		d = (uint16_t*)(vc->vc_origin + (vc->vc_size_row * top));
		s = (uint16_t*)(vc->vc_origin + (vc->vc_size_row * (top + count)));
		scr_memmovew(d, s, (bottom - top - count) * vc->vc_size_row);
		scr_memsetw(d + ((bottom - top - count) * vc->vc_cols),
		            vc->vc_scrl_erase_char,
		            vc->vc_size_row * count);

		break;
	case SM_DOWN:
		s = (uint16_t*)(vc->vc_origin + (vc->vc_size_row * top));
		d = (uint16_t*)(vc->vc_origin + (vc->vc_size_row * (top + count)));
		scr_memmovew(d, s, (bottom - top - count) * vc->vc_size_row);
		scr_memsetw(s,
		            vc->vc_scrl_erase_char,
		            vc->vc_size_row * count);
		break;
	}
	/* We need to update the LCD */
	update_region(vc,
	              vc->vc_origin + (vc->vc_size_row * top),
	              (bottom - top) * vc->vc_cols);

	return 1; /* Scrolling has been done by this function */
}

static void ew24b00yly_con_bmove(struct vc_data* vc, int sy, int sx, int dy,
                                 int dx, int height, int width)
{
	printk(KERN_DEBUG "%s: bmove not implemented\n", ew24b00yly_name);
}

static int ew24b00yly_con_switch(struct vc_data* vc)
{
	/* Reset scroll delta buffer */
	ew24b00yly_scrolldelta_buff.count = 0;
	ew24b00yly_scrolldelta_buff.end = 0;
	ew24b00yly_scrolldelta_buff.scroll = 0;

	ew24b00yly_con_restore_screen(vc);

	/* Refresh the whole screen */
	ew24b00yly_lcd_putcs(0x0000, vc->vc_screenbuf, vc->vc_screenbuf_size / 2);
	return 0; /* Redrawing not needed */
}

static int ew24b00yly_con_blank(struct vc_data* vc, int blank, int mode_switch)
{
	if (blank)
		ew24b00yly_lcd_off();
	else
		ew24b00yly_lcd_on();

	return 0; /* don't update the screen */
}

static int ew24b00yly_con_resize(struct vc_data* vc, unsigned int width,
                                 unsigned int height, unsigned int user)
{
	return 0; /* no error */
}

static int ew24b00yly_con_set_palette(struct vc_data* vc, unsigned char *table)
{
	printk(KERN_DEBUG "%s: set_palette not implemented\n", ew24b00yly_name);
	return 0;
}

/**
 * ew24b00yly_deltascroll_contents() - It copies a part of delta scroll buffer
 *                                     into screen buffer
 * @vc: Current virtual console
 * @return: Actual number of copied rows.
 *
 * The copying is performed from the 'end - scroll' to 'end - scroll + EW24B00YLY_LCD_ROWS'.
 */
static int ew24b00yly_deltascroll_contents(struct vc_data* vc)
{
	int from;
	int copied = 0; /* number of lines copied */
	int rows_to_copy;

	rows_to_copy = min(ew24b00yly_scrolldelta_buff.scroll, EW24B00YLY_LCD_ROWS);
	from = ew24b00yly_scrolldelta_buff.end - ew24b00yly_scrolldelta_buff.scroll;
	if (from < 0) {
		from *= -1;
		copied = min(from, rows_to_copy); /* n to copy */
		scr_memcpyw(vc->vc_screenbuf,
		            &ew24b00yly_scrolldelta_buff.buff[EW24B00YLY_SCROLLDELTA_ROWS - from][0],
		            copied * vc->vc_size_row);
		from = 0;
	}
	if (copied < rows_to_copy) {
		scr_memcpyw(vc->vc_screenbuf + (copied * vc->vc_cols),
		            &ew24b00yly_scrolldelta_buff.buff[from][0],
		            (rows_to_copy - copied) * vc->vc_size_row);
	}
	return rows_to_copy;
}

static int ew24b00yly_con_scrolldelta(struct vc_data* vc, int lines)
{
	int scroll = ew24b00yly_scrolldelta_buff.scroll - lines;
	if (lines > 0 && scroll < 0)
		scroll = 0;
	else if (lines < 0 && scroll > ew24b00yly_scrolldelta_buff.count)
		scroll = ew24b00yly_scrolldelta_buff.count;

	if (0 == scroll)
		ew24b00yly_con_cursor(vc, CM_DRAW);
	else if (scroll > 0) {
		int copied; /* number of rows copied from scroll delta buffer */

		if (!ew24b00yly_saved_screen[vc->vc_num])
			ew24b00yly_con_save_screen(vc);

		ew24b00yly_scrolldelta_buff.scroll = scroll;
		copied = ew24b00yly_deltascroll_contents(vc);
		if (copied < EW24B00YLY_LCD_ROWS) {
			scr_memcpyw(vc->vc_screenbuf + (copied * vc->vc_cols),
			            ew24b00yly_saved_screen[vc->vc_num],
			            (EW24B00YLY_LCD_ROWS - copied) * vc->vc_size_row);
		}

		ew24b00yly_lcd_putcs(0x0000,
		                     vc->vc_screenbuf,
		                     vc->vc_screenbuf_size / 2);

		ew24b00yly_con_cursor(vc, CM_ERASE);
	}

	/*printk(KERN_DEBUG "scrolldelta: lines=%d, scroll=%d, count=%d, end=%d\n",
	       lines, ew24b00yly_scrolldelta_buff.scroll,
	       ew24b00yly_scrolldelta_buff.count,
	       ew24b00yly_scrolldelta_buff.end);*/

	return 0;
}

static int ew24b00yly_con_set_origin(struct vc_data* vc)
{
	return 0;
}

static void ew24b00yly_con_save_screen(struct vc_data* vc)
{
	if (!ew24b00yly_saved_screen[vc->vc_num]) {
		ew24b00yly_saved_screen[vc->vc_num] =
			kmalloc(vc->vc_screenbuf_size, GFP_KERNEL);
	}
	if (ew24b00yly_saved_screen[vc->vc_num]) {
		scr_memcpyw(ew24b00yly_saved_screen[vc->vc_num],
		            vc->vc_screenbuf,
		            vc->vc_screenbuf_size);
	}
}

static void ew24b00yly_con_restore_screen(struct vc_data* vc)
{
	if (ew24b00yly_saved_screen[vc->vc_num]) {
		scr_memcpyw(vc->vc_screenbuf,
		            ew24b00yly_saved_screen[vc->vc_num],
		            vc->vc_screenbuf_size);
		kfree(ew24b00yly_saved_screen[vc->vc_num]);
		ew24b00yly_saved_screen[vc->vc_num] = NULL;

		ew24b00yly_lcd_putcs(0x0000,
		                     vc->vc_screenbuf,
		                     vc->vc_screenbuf_size / 2);
	}
}

static u8 ew24b00yly_con_build_attr(struct vc_data* vc, u8 color, u8 intensity,
                                    u8 blink, u8 underline, u8 reverse, u8 italic)
{
	u8 mask = 0x00;
	if (blink)
		mask |= 0x08;
	if (reverse)
		mask |= 0x05;

	return mask;
}

static void ew24b00yly_con_invert_region(struct vc_data* vc, u16* p, int cnt)
{
	printk(KERN_DEBUG "%s: invert_region not implemented\n", ew24b00yly_name);
}

static u16* ew24b00yly_con_screen_pos(struct vc_data* vc, int offset)
{
	printk(KERN_DEBUG "%s: screen_pos not implemented\n", ew24b00yly_name);
	return 0;
}

static int ew24b00yly_con_dummy(struct vc_data* vc)
{
	return 0;
}


/*
 ******************************************************************************
 * Device attributes
 ******************************************************************************
 */

static ssize_t ew24b00yly_attr_show_blight_mode(struct device* dev,
                                           struct device_attribute* attr,
                                           char* buf)
{
	int i, count = 0;
	for (i = 0; i < ARRAY_SIZE(ew24b00yly_lcd_blight_mode_names); i++)
		count += snprintf(&buf[count], PAGE_SIZE - count,
		                  i == ew24b00yly_lcd_blight_mode ? "[%s] " : "%s ",
		                  ew24b00yly_lcd_blight_mode_names[i]);
	count += snprintf(&buf[count], PAGE_SIZE - count, "\n");
	return count;
}

static ssize_t ew24b00yly_attr_store_blight_mode(struct device* dev,
                                            struct device_attribute* attr,
                                            const char* buf, size_t count)
{
	int ret;
	char mode[5];
	if (sscanf(buf, "%4s", (char *)&mode) == 1)
	{
		int i;
		for (i = 0; i < ARRAY_SIZE(ew24b00yly_lcd_blight_mode_names); i++)
			if (0 == strcmp(mode, ew24b00yly_lcd_blight_mode_names[i]))
				break;
		if (ARRAY_SIZE(ew24b00yly_lcd_blight_mode_names) == i)
			ret = -EINVAL;
		else {
			ew24b00yly_lcd_blight_mode = i;
			if (EW24B00YLY_LCD_BLIGHT_MODE_OFF == i)
				ew24b00yly_lcd_blight_off();
			else
				ew24b00yly_lcd_blight_on();
		}
	} else
		ret = -EINVAL;

	return ret ? ret : count;
}

/* The way how the LCD back light is turned on and off */
static DEVICE_ATTR(backlight_mode, S_IRUGO | S_IWUGO,
                   ew24b00yly_attr_show_blight_mode,
                   ew24b00yly_attr_store_blight_mode);


static ssize_t ew24b00yly_attr_show_blight_interval(struct device* dev,
                                                 struct device_attribute* attr,
                                                 char* buf)
{
	return snprintf(buf, PAGE_SIZE, "%lu\n", ew24b00yly_lcd_blight_interval);
}

static ssize_t ew24b00yly_attr_store_blight_interval(struct device* dev,
                                                  struct device_attribute* attr,
                                                  const char* buf, size_t count)
{
	int ret;
	if (sscanf(buf, "%lu", &ew24b00yly_lcd_blight_interval) == 1)
		ew24b00yly_lcd_blight_on();
	else
		ret = -EINVAL;

	return ret ? ret : count;
}

/* The LCD back light interval of illuminating when in the mode auto */
static DEVICE_ATTR(backlight_interval, S_IRUGO | S_IWUGO,
                   ew24b00yly_attr_show_blight_interval,
                   ew24b00yly_attr_store_blight_interval);


static ssize_t ew24b00yly_attr_show_blight_level(struct device* dev,
                                                 struct device_attribute* attr,
                                                 char* buf)
{
	return snprintf(buf, PAGE_SIZE, "%u\n",
	                ew24b00yly_lcd_blight_get_level());
}

static ssize_t ew24b00yly_attr_store_blight_level(struct device* dev,
                                                  struct device_attribute* attr,
                                                  const char* buf, size_t count)
{
	int ret;
	uint32_t level;
	if (sscanf(buf, "%u", &level) == 1)
	{
		ret = ew24b00yly_lcd_blight_set_level(level);
		ew24b00yly_lcd_blight_on();
	} else
		ret = -EINVAL;

	return ret ? ret : count;
}

/* The LCD back light brightness level */
static DEVICE_ATTR(backlight_level, S_IRUGO | S_IWUGO,
                   ew24b00yly_attr_show_blight_level,
                   ew24b00yly_attr_store_blight_level);


static ssize_t ew24b00yly_attr_show_blight_max(struct device* dev,
                                    struct device_attribute* attr, char* buf)
{
	return snprintf(buf, PAGE_SIZE, "%u\n",
	                ew24b00yly_lcd_blight_max_level());
}

/* The LCD back light brightness max. level */
static DEVICE_ATTR(backlight_max, S_IRUGO, ew24b00yly_attr_show_blight_max,
                   NULL);


static ssize_t ew24b00yly_attr_show_contrast_level(struct device* dev,
                                                   struct device_attribute* attr,
                                                   char* buf)
{
	return snprintf(buf, PAGE_SIZE, "%u\n",
	                ew24b00yly_lcd_vee_get_level());
}

static ssize_t ew24b00yly_attr_store_contrast_level(struct device* dev,
                                                    struct device_attribute* attr,
                                                    const char* buf, size_t count)
{
	int ret;
	uint32_t level;
	if (sscanf(buf, "%u", &level) == 1)
		ret = ew24b00yly_lcd_vee_set_level(level);
	else
		ret = -EINVAL;

	return ret ? ret : count;
}

/* The LCD contrast level */
static DEVICE_ATTR(contrast_level, S_IRUGO | S_IWUGO,
                   ew24b00yly_attr_show_contrast_level,
                   ew24b00yly_attr_store_contrast_level);

static ssize_t ew24b00yly_attr_show_contrast_max(struct device* dev,
                                    struct device_attribute* attr, char* buf)
{
	return snprintf(buf, PAGE_SIZE, "%u\n",
	                ew24b00yly_lcd_vee_max_level());
}

/* The LCD contrast max. level */
static DEVICE_ATTR(contrast_max, S_IRUGO, ew24b00yly_attr_show_contrast_max,
                   NULL);


/* List of all device attributes */
static struct device_attribute *const ew24b00yly_device_attributes[] = {
	&dev_attr_backlight_mode,
	&dev_attr_backlight_interval,
	&dev_attr_backlight_level,
	&dev_attr_backlight_max,
	&dev_attr_contrast_level,
	&dev_attr_contrast_max,
};

/*
 ******************************************************************************
 * Stuff related to the module initialization and cleaning up
 ******************************************************************************
 */


__init static int ew24b00yly_probe(struct platform_device* device)
{
	int ret, attr_id;

	if ((ret = ew24b00yly_lcd_init()))
		goto lcd_init_error;

	if ((ret = take_over_console(&ew24b00yly_consw,
	                             EW24B00YLY_FIRST_TTY_NUM,
	                             EW24B00YLY_LAST_TTY_NUM,
	                             1))) {
		printk(KERN_ERR "%s: Failed to take over the (system) console!\n",
		       ew24b00yly_name);
		goto con_error;
	}
	/* Lcd error might occur during the console over taking */
	if ((ret = ew24b00yly_lcd_error))
		goto lcd_op_error;

	/* Add device attributes */
	for(attr_id = 0; attr_id < ARRAY_SIZE(ew24b00yly_device_attributes); attr_id++) {
		struct device_attribute *const attribute =
			ew24b00yly_device_attributes[attr_id];
		ret = device_create_file(&device->dev, attribute);
		if (ret)
			goto lcd_attr_error;
	}

	printk(KERN_INFO "%s: Probed.\n", ew24b00yly_name);
	return 0;


lcd_attr_error:
	while(attr_id--)
		device_remove_file(&device->dev, ew24b00yly_device_attributes[attr_id]);

lcd_op_error: /* lcd error during operation time */
	unbind_con_driver(&ew24b00yly_consw,
	                  EW24B00YLY_FIRST_TTY_NUM,
	                  EW24B00YLY_LAST_TTY_NUM,
	                  1);
	give_up_console(&ew24b00yly_consw);

con_error: /* console error */
	ew24b00yly_lcd_cleanup();

lcd_init_error: /* lcd error during initialization */

/* general error handling */
	printk(KERN_ERR "%s: Failed to probe the module!\n", ew24b00yly_name);
	return ret;
}

__exit static int ew24b00yly_remove(struct platform_device* device)
{
	int i;

	/* Remove device attributes */
	for(i = 0; i < ARRAY_SIZE(ew24b00yly_device_attributes); i++)
		device_remove_file(&device->dev,
		                   ew24b00yly_device_attributes[i]);

	/* Turn off the lcd */
	ew24b00yly_lcd_cleanup();

	/* Unbind the driver from console*/
	if (con_is_bound(&ew24b00yly_consw))
		unbind_con_driver(&ew24b00yly_consw,
		                  EW24B00YLY_FIRST_TTY_NUM,
		                  EW24B00YLY_LAST_TTY_NUM,
		                  1);
	unregister_con_driver(&ew24b00yly_consw);

	printk(KERN_INFO "%s: The module was successfully removed.\n",
	       ew24b00yly_name);

	return 0;
}

static int ew24b00yly_suspend(struct platform_device* device,
                              pm_message_t state)
{
	ew24b00yly_lcd_off();
	return 0;
}

static int ew24b00yly_resume(struct platform_device* device)
{
	ew24b00yly_lcd_on();
	return 0;
}

static struct platform_driver ew24b00yly_device_driver = {
	.remove         = __exit_p(ew24b00yly_remove),
	.suspend        = ew24b00yly_suspend,
	.resume         = ew24b00yly_resume,
	.driver         = {
		.name   = ew24b00yly_name,
		.owner  = THIS_MODULE,
        }
};

/** This function is called once right after the module is loaded */
__init static int ew24b00yly_mod_init(void)
{
	return platform_driver_probe(&ew24b00yly_device_driver,
	                             ew24b00yly_probe);
}
module_init(ew24b00yly_mod_init);

/** This function is called once right before the module is unloaded */
__exit static void ew24b00yly_mod_exit(void)
{
	platform_driver_unregister(&ew24b00yly_device_driver);
}
module_exit(ew24b00yly_mod_exit);


/* Information about the module goes here */
MODULE_VERSION("0:2.0");
MODULE_DESCRIPTION("Graphic display ew24b00yly");
MODULE_AUTHOR("Marjan Fojkar <marjan@pajkc.eu>");
MODULE_LICENSE("Dual GPL");
