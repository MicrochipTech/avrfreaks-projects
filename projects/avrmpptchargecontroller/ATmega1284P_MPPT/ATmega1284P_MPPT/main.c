/*
 * main.c
 *
 *  Created on: 9 Oct, 2014
 *      Author: prageeth
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <inttypes.h>
#include <stdlib.h>
#include "LCD-AVR-4f.h"
#include "irmp.h"
#include "timer0.h"

void read_and_print_time(void);
void set_pwm_val(volatile uint8_t *port, uint8_t val);
void pwm_init(void);
uint8_t inc_pwm(void);
uint8_t dec_pwm(void);

void adc_init(void);
uint16_t read_adc(uint8_t channel);
void eeprom_write_int16(uint8_t *addr, uint16_t val);
void eeprom_write_int32(uint8_t *addr, uint32_t val);
void eeprom_write_int64(uint8_t *addr, uint64_t val);
uint16_t eeprom_read_int16(uint8_t *addr);
uint32_t eeprom_read_int32(uint8_t *addr);
uint64_t eeprom_read_int64(uint8_t *addr);

enum {
    TIME_REFRESH_DELAY = 50L, INT_POWER_MONITOR_DELAY = 180L, EEPROM_WRITE_DELAY = 600000L, POWER_REFRESH_DELAY = 1000L, SNEP_EEPROM_WRITE_DELAY = 60000L
};

char str[20];
char ir_disp_str[20];
const char line_1_fmt[] = "%ldmV %ldmA %s";
const char line_2_fmt[] = "%ldmW %lumWh";
const char line_3_fmt_int[] = ">%ldmV %ldmWh";
const char line_3_fmt_ext[] = "%ldmV> %ldmWh";
const char line_3_fmt_int_big[] = ">%ldmV %ldWh";
const char line_3_fmt_ext_big[] = "%ldmV> %ldWh";
const char ir_fmt_str[] = "P: %x C: %x F: %x";
IRMP_DATA irmp_data;
uint8_t clk_adj_pos = 0;

long int adc_value_0;
long int adc_value_1;
long int adc_value_2;

long long int _voltage = 0;
long long int _current = 0;
long long int _op_voltage = 0;

unsigned int adc_min = 0x00c;
unsigned int adc_max = 0xffc;
unsigned int adc_preferred = 0x19;

uint8_t min_pwm = 0x00;
uint8_t max_pwm = 0x7f;
volatile uint8_t current_pwm = 0x06;

long int power;
long int power_2;
long int power_1;
long int power_0;

//uint64_t mWh_tot = 0UL;
uint64_t _uWh_tot = 0UL;
uint64_t uWh = 0UL;

uint8_t mode_inc = 1;

int8_t fail = 0;

char buffer[5];

uint64_t now;

uint64_t int_pow_mon_time = 0UL;
uint64_t pow_refr_time = 0UL;
uint64_t eeprom_time = 0UL;
uint64_t time_time = 0UL;
uint64_t snep_eeprom_write_time = 0UL;


uint8_t batt_ext_int_swp = 0;

const uint8_t max_fails = 2;

#define REG_5V_VOLTAGE 4998L

/**
 * Charge controlling for 12 x 80386 cell 3.7v battery pack (recycled from old laptops)
 */
#define BATT_INT_RESISTANCE_MO 30UL

#define MAX_OUTPUT_VOLTAGE_MV 4195UL

#define REDUCE_CHARGE_POWER_AT_VOTAGE_0 3980UL
#define REDUCE_CHARGE_POWER_AT_VOTAGE_1 4050UL
#define REDUCE_CHARGE_POWER_AT_VOTAGE_2 4100UL
#define REDUCE_CHARGE_POWER_AT_VOTAGE_3 4120UL
#define SOON_TO_NOT_ENOUGH_POWER_AT_VOLTAGE 7800UL
#define NOT_ENOUGH_POWER_AT_VOLTAGE 7000UL

#define MAX_INPUT_POWER 12000

#define REDUCED_CHARGE_POWER_0 6000
#define REDUCED_CHARGE_POWER_1 2800
#define REDUCED_CHARGE_POWER_2 1220
#define REDUCED_CHARGE_POWER_3 28

#define MODE_NORMAL_CHARGING 0
#define MODE_REDUCED_CHARGING_0  1
#define MODE_REDUCED_CHARGING_1  2
#define MODE_REDUCED_CHARGING_2  3
#define MODE_REDUCED_CHARGING_3  4
#define MODE_FULLY_CHARGED 5
#define MODE_SOON_TO_NOT_ENOUGH_INPUT_POWER 6
#define MODE_NOT_ENOUGH_INPUT_POWER 7
#define MODE_MAX_INPUT_POWER 8

uint8_t mode = MODE_NORMAL_CHARGING;

int main(void) {

    adc_init();

    OCR0A = 0x00;
    init_lcd_avr_4f();
    lcd_clear();
    irmp_init();
    pwm_init();
    timer_init();
    _delay_ms(10);

    _uWh_tot = eeprom_read_int64((uint8_t*) 0x01);
    if (((long int) _uWh_tot) < 0L) {
        eeprom_write_int64((uint8_t*) 0x01, (uint64_t) 0);
        _delay_ms(10);
        _uWh_tot = eeprom_read_int64((uint8_t*) 0x01);
    }
    for (;;) {

        OCR0A = current_pwm;

        now = millis();

        adc_value_0 = read_adc(0);

        adc_value_1 = read_adc(1);

        adc_value_2 = read_adc(2);
        adc_value_2 += read_adc(2);
        adc_value_2 += read_adc(2);
        adc_value_2 += read_adc(2);
        adc_value_2 += read_adc(2);
        adc_value_2 += read_adc(2);
        adc_value_2 += read_adc(2);
        adc_value_2 += read_adc(2);
        adc_value_2 += read_adc(2);
        adc_value_2 += read_adc(2);
        adc_value_2 /= 10;

        long int in_voltage = ((adc_value_0 * 13072L) / 3266L) * REG_5V_VOLTAGE / 1023L;
        if (adc_value_1 < 0L) {
            adc_value_1 = 0L;
        }

        long int out_voltage = in_voltage - ((adc_value_2) * 14698L) / 4900L * REG_5V_VOLTAGE / 1023L;

        long int current = (adc_value_1 - 512L) * 51098L / 1023L;

        _current *= 30L;
        _current += current;
        _current /= 31L;

        _voltage += in_voltage;
        _voltage /= 2L;

        _op_voltage *= 30L;
        _op_voltage += out_voltage;
        _op_voltage /= 31L;

        long int _out_current = _current * _voltage / _op_voltage;

        if (_out_current < 0L) {
            _out_current = 0L;
        }

        long int _batt_voltage = _op_voltage - (BATT_INT_RESISTANCE_MO * _out_current) / 1000UL;
        if (_batt_voltage < 0L) {
            _batt_voltage = 0L;
        }

        if (now - eeprom_time >= EEPROM_WRITE_DELAY) {
            eeprom_time = now;
            eeprom_write_int64((uint8_t*) 0x01, _uWh_tot);
        }

        if (now - int_pow_mon_time >= INT_POWER_MONITOR_DELAY) {
            int_pow_mon_time = now;
            power_2 = power_1;
            power_1 = power_0;
            power_0 = power;

            if (_voltage < 0L) {
                _voltage = 0L;
            }
            if (_current < 0L) {
                _current = 0L;
            }

            power = _voltage * (long int) _current / 1000L;

            uWh += (power * INT_POWER_MONITOR_DELAY) / 3600UL;
            _uWh_tot += (power * INT_POWER_MONITOR_DELAY) / 3600UL;

            if (power >= MAX_INPUT_POWER) {
                mode = MODE_MAX_INPUT_POWER;
                dec_pwm();
            } else if (_voltage <= NOT_ENOUGH_POWER_AT_VOLTAGE) {
                mode = MODE_NOT_ENOUGH_INPUT_POWER;
                current_pwm = min_pwm;
            } else if (_voltage <= SOON_TO_NOT_ENOUGH_POWER_AT_VOLTAGE) {
                if(now - snep_eeprom_write_time >= SNEP_EEPROM_WRITE_DELAY) {
                    OCR0A = min_pwm;
                    eeprom_write_int64((uint8_t*) 0x01, _uWh_tot);
                    snep_eeprom_write_time = now;
                    current_pwm = min_pwm;
                }
                mode = MODE_SOON_TO_NOT_ENOUGH_INPUT_POWER;
                dec_pwm();
            } else {
                if (_batt_voltage >= REDUCE_CHARGE_POWER_AT_VOTAGE_3) {
                    mode = MODE_REDUCED_CHARGING_3;
                } else if (_batt_voltage >= REDUCE_CHARGE_POWER_AT_VOTAGE_2) {
                    mode = MODE_REDUCED_CHARGING_2;
                } else if (_batt_voltage >= REDUCE_CHARGE_POWER_AT_VOTAGE_1) {
                    mode = MODE_REDUCED_CHARGING_1;
                } else if (_batt_voltage >= REDUCE_CHARGE_POWER_AT_VOTAGE_0) {
                    mode = MODE_REDUCED_CHARGING_0;
                } else {
                    mode = MODE_NORMAL_CHARGING;
                    if (_batt_voltage >= MAX_OUTPUT_VOLTAGE_MV) {
                        mode = MODE_FULLY_CHARGED;
                        dec_pwm();
                    }
                }
            }

            if (mode == MODE_REDUCED_CHARGING_0) {
                if (power >= REDUCED_CHARGE_POWER_0) {
                    dec_pwm();
                } else {
                    inc_pwm();
                }
            } else if (mode == MODE_REDUCED_CHARGING_1) {
                if (power >= REDUCED_CHARGE_POWER_1) {
                    dec_pwm();
                } else {
                    inc_pwm();
                }

            } else if (mode == MODE_REDUCED_CHARGING_2) {
                if (power >= REDUCED_CHARGE_POWER_2) {
                    dec_pwm();
                } else {
                    inc_pwm();
                }

            } else if (mode == MODE_REDUCED_CHARGING_3) {
                if (power >= REDUCED_CHARGE_POWER_3) {
                    dec_pwm();
                } else {
                    inc_pwm();
                }
            } else if (mode == MODE_NORMAL_CHARGING) {
                if (mode_inc) {
                    if (power_1 > power || power_0 > power) {
                        ++fail;
                        if (fail >= max_fails) {
                            fail = 0;
                            mode_inc = 0;
                        }
                    } else {
                        fail = 0;
                        mode_inc = inc_pwm();
                    }
                } else {
                    if (power_1 > power || power_0 > power) {
                        --fail;
                        if (fail <= -max_fails) {
                            fail = 0;
                            mode_inc = 1;
                        }
                    } else {
                        fail = 0;
                        mode_inc = dec_pwm();
                    }
                }
            }

            if (now - pow_refr_time > POWER_REFRESH_DELAY) {
                pow_refr_time = now;
                sprintf(str, line_1_fmt, (long int) _voltage, (long int) _current,
                        mode == MODE_FULLY_CHARGED ?
                                "FULL" :
                                (mode == MODE_NORMAL_CHARGING ?
                                        "NORM" :
                                        (mode == MODE_NOT_ENOUGH_INPUT_POWER ?
                                                "NEPW" : (mode == MODE_REDUCED_CHARGING_0 ? "REL0" : (mode == MODE_REDUCED_CHARGING_1 ? "REL1" : (mode == MODE_REDUCED_CHARGING_2 ? "REL2" : (mode == MODE_SOON_TO_NOT_ENOUGH_INPUT_POWER ? "SNEP" : (mode == MODE_MAX_INPUT_POWER ? "MAXP" : "REL3"))))))));
                lcd_clear_line(lcd_line_one);
                lcd_write_instruction_4f(lcd_SetCursor | lcd_line_one);
                lcd_write_string_4f(str);

                sprintf(str, line_2_fmt, power, (long unsigned int) (uWh / 1000UL));
                lcd_clear_line(lcd_line_two);
                lcd_write_instruction_4f(lcd_SetCursor | lcd_line_two);
                lcd_write_string_4f(str);

                if (_uWh_tot > 999999999UL) {
                    // TODO: divide values so that they can be represented in the limited space on LCD.
                    if (batt_ext_int_swp == 0) {
                        batt_ext_int_swp = 1;
                        sprintf(str, line_3_fmt_int_big, (long int) _batt_voltage, (long int) (_uWh_tot / 1000000UL));
                    } else {
                        batt_ext_int_swp = 0;
                        sprintf(str, line_3_fmt_ext_big, (long int) _op_voltage, (long int) (_uWh_tot / 1000000UL));
                    }
                } else {
                    if (batt_ext_int_swp == 0) {
                        batt_ext_int_swp = 1;
                        sprintf(str, line_3_fmt_int, (long int) _batt_voltage, (long int) (_uWh_tot / 1000UL));
                    } else {
                        batt_ext_int_swp = 0;
                        sprintf(str, line_3_fmt_ext, (long int) _op_voltage, (long int) (_uWh_tot / 1000UL));
                    }
                }
                lcd_clear_line(lcd_line_three);
                lcd_write_instruction_4f(lcd_SetCursor | lcd_line_three);
                lcd_write_string_4f(str);
            }

        }
        read_and_print_time();
        if (irmp_get_data(&irmp_data)) {
            sprintf(ir_disp_str, ir_fmt_str, irmp_data.protocol, irmp_data.command, irmp_data.flags);
            lcd_clear_line(lcd_line_one);
            lcd_write_instruction_4f(lcd_SetCursor | lcd_line_one);
            lcd_write_string_4f(ir_disp_str);

            switch (irmp_data.protocol) {
            case IRMP_APPLE_PROTOCOL:
                switch (irmp_data.command) {
                case 0x3:
                    clk_adj_pos = irmp_data.flags == 0 ? (clk_adj_pos == 3 ? 0 : clk_adj_pos + 1) : clk_adj_pos;
                    break;
                case 0xc:
                    if (adc_preferred > adc_min) {
                        adc_preferred -= 0x0001;
                    }
                    break;
                case 0xa:
                    if (adc_preferred < adc_max) {
                        adc_preferred += 0x0001;
                    }
                    break;
                }
                break;
            }

        }
    }
    return 0;
}

uint8_t inc_pwm(void) {
    if (current_pwm < max_pwm) {
        ++current_pwm;
        return 1;
    } else {
        return 0;
    }
}
uint8_t dec_pwm(void) {
    if (current_pwm > min_pwm) {
        --current_pwm;
        return 0;
    } else {
        return 1;
    }
}

void adc_init(void) {
    ADCSRA |= ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));    //16Mhz/128 = 125Khz the ADC reference clock
    ADMUX |= (1 << REFS0);                //Voltage reference from Avcc (5v)
    ADCSRA |= (1 << ADEN);                //Turn on ADC
    ADCSRA |= (1 << ADSC);                //Do an initial conversion because this one is the slowest and to ensure that everything is up and running
}

uint16_t read_adc(uint8_t channel) {
    ADMUX &= 0xF0;                    //Clear the older channel that was read
    ADMUX |= channel;                //Defines the new ADC channel to be read
    ADCSRA |= (1 << ADSC);                //Starts a new conversion
    while (ADCSRA & (1 << ADSC))
        ;            //Wait until the conversion is done
    return ADCW;                    //Returns the ADC value of the chosen channel
}

void pwm_init(void) {
    /*
     Starting values for red, green and blue
     */

    /*
     Port B Data Direction Register (controls the mode of all pins within port B)
     DDRB is 8 bits: [unused:unused:DDB5:DDB4:DDB3:DDB2:DDB1:DDB0]
     1<<DDB4: sets bit DDB4 (data-direction, port B, pin 4), which puts PB4 (port B, pin 4) in output mode
     1<<DDB1: sets bit DDB1 (data-direction, port B, pin 1), which puts PB1 (port B, pin 1) in output mode
     1<<DDB0: sets bit DDB0 (data-direction, port B, pin 0), which puts PB0 (port B, pin 0) in output mode
     */
    DDRB = 1 << DDB3;

//    TCCR0A = 2 << COM0A0 | 3 << WGM00;
//    TCCR0B = 0 << WGM02 | 1 << CS00;

    // Initial TIMER0 Fast PWM
    // Fast PWM Frequency = fclk / (N * 256), Where N is the Prescaler
    // f_PWM = 11059200 / (64 * 256) = 675 Hz
//    TCCR0A = 0b10100011; // Fast PWM 8 Bit, Clear OCA0/OCB0 on Compare Match, Set on TOP
//    TCCR0B = 0b00000011; // Used 64 Prescaler
//    OCR0A = 0x00;

    // Initial TIMER0 Phase Correct PWM
    // Fast PWM Frequency = fclk / (N * 510), Where N is the Prescaler
    // f_PWM = 16000000 / (1 * 510) = 31.372549 kHz
    TCCR0A = 0b10100001; // Phase Correct PWM 8 Bit, Clear OCA0/OCB0 on Compare Match, Set on TOP
    TCCR0B = 0b00000001; // No Prescaling
    TCNT0 = 0;           // Reset TCNT0
    OCR0A = 0;           // Initial the Output Compare register A & B
}

void read_and_print_time(void) {
    if (now - time_time >= TIME_REFRESH_DELAY) {
        time_time = now;
        lcd_clear_line(lcd_line_four);
        uint64_t ms = millis();
        lcd_write_instruction_4f(lcd_SetCursor | lcd_line_four);

        uint64_t days = ms / 1000UL / 60UL / 60UL / 24UL;
        ultoa(days, str, 10);
        lcd_write_string_4f(str);
        lcd_write_string_4f(" ");
        ms = ms - (days * 24UL * 60UL * 60UL * 1000UL);
        uint64_t hours = ms / 1000UL / 60UL / 60UL;

        if (clk_adj_pos == 3) {
            if ((now / 100) % 5 < 4) {
                ultoa(hours, str, 10);
                if (hours < 10) {
                    lcd_write_string_4f("0");
                }
                lcd_write_string_4f(str);
            } else {
                lcd_write_string_4f("  ");
            }
        } else {
            ultoa(hours, str, 10);
            if (hours < 10) {
                lcd_write_string_4f("0");
            }
            lcd_write_string_4f(str);
        }

        lcd_write_string_4f(":");

        ms = ms - (hours * 60UL * 60UL * 1000UL);
        uint64_t minutes = ms / 1000UL / 60UL;

        if (clk_adj_pos == 2) {
            if ((now / 100) % 5 < 4) {
                ultoa(minutes, str, 10);
                if (minutes < 10) {
                    lcd_write_string_4f("0");
                }
                lcd_write_string_4f(str);
            } else {
                lcd_write_string_4f("  \0");
            }
        } else {
            ultoa(minutes, str, 10);
            if (minutes < 10) {
                lcd_write_string_4f("0");
            }
            lcd_write_string_4f(str);
        }

        lcd_write_string_4f(":");

        ms = ms - (minutes * 60UL * 1000UL);
        uint64_t seconds = ms / 1000UL;

        if (clk_adj_pos == 1) {
            if ((now / 100) % 5 < 4) {
                ultoa(seconds, str, 10);
                if (seconds < 10) {
                    lcd_write_string_4f("0");
                }
                lcd_write_string_4f(str);
            } else {
                lcd_write_string_4f("  ");
            }
        } else {
            ultoa(seconds, str, 10);
            if (seconds < 10) {
                lcd_write_string_4f("0");
            }
            lcd_write_string_4f(str);
        }

        lcd_write_string_4f(" ");
        ms = ms - (seconds * 1000UL);
        ultoa(ms, str, 10);
        if (ms < 100) {
            lcd_write_string_4f("0");
            if (ms < 10) {
                lcd_write_string_4f("0");
            }
        }
        lcd_write_string_4f(str);
        lcd_write_string_4f(mode_inc == 1 ? " H" : " L");
//        itoa(fail, str, 10);
//        lcd_write_string_4f(str);
        itoa(current_pwm, str, 16);
        lcd_write_string_4f(str);
    }
}

void set_pwm_val(volatile uint8_t *port, uint8_t val) {
    *port = val;
}

void eeprom_write_int16(uint8_t *addr, uint16_t val) {
    eeprom_write_byte(addr, (uint8_t) (val & 0xff));
    eeprom_write_byte(addr + 1, (uint8_t) ((val & 0xff00) >> 8));
}

void eeprom_write_int32(uint8_t *addr, uint32_t val) {
    eeprom_write_int16(addr, (uint16_t) (val & 0xffff));
    eeprom_write_int16(addr + 2, (uint16_t) ((val & 0xffff0000) >> 16));
}
void eeprom_write_int64(uint8_t *addr, uint64_t val) {
    eeprom_write_int32(addr, (uint32_t) (val & 0xffffffff));
    eeprom_write_int32(addr + 4, (uint32_t) ((val & 0xffffffff00000000) >> 32));
}

uint16_t eeprom_read_int16(uint8_t *addr) {
    uint16_t val;
    val = eeprom_read_byte(addr);
    val |= ((uint16_t) eeprom_read_byte(addr + 1) << 8);
    return val;
}

uint32_t eeprom_read_int32(uint8_t *addr) {
    uint32_t val;
    val = eeprom_read_int16(addr);
    val |= ((uint32_t) eeprom_read_int16(addr + 2) << 16);
    return val;
}

uint64_t eeprom_read_int64(uint8_t *addr) {
    uint64_t val;
    val = eeprom_read_int32(addr);
    val |= ((uint64_t) eeprom_read_int32(addr + 4) << 32);
    return val;
}
