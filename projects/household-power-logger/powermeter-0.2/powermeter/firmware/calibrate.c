#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include "config.h"
#include "stack-check.h"
#include "serial.h"
#include "rms64.h"
#include "mmc.h"
#include "pcf-rtc.h"

static void write_calibration_to_eeprom() {
    eeprom_write_block(&calib, 0, sizeof(calib_t));
}

// range 0 for V
static void show_calib(uint8_t range) {
	printP("Calibration is ");
	print_int32(calib.A[range]);
	printP(" / ");
	print_int32(calib.B[range]);
	printP("\n");
}

// range 0 for V
static void set_calibration(uint8_t range) {
	if(range) {
		printP("Enter amps times 100:\n");
	} else {
		printP("Enter volts times 10:\n");
	}
	int16_t realval = (int16_t)read_int32();
	print_keyP_val32("You entered ", realval);

	if(range) adc_wanted_range = range;

	// first reading may already be full
	// second could be aborted due to range change
	// third will be valid
	for(uint8_t i=0; i<3; i++) {
		printP("Waiting for result\n");
		while(!adc_result_ready) wdt_reset();
		adc_result_ready = 0;
	}

	uint8_t oob;
	if(range) {
		oob = (adc_result.min_I < ADC_BOUND_MIN || adc_result.max_I > ADC_BOUND_MAX);
	} else {
		oob = (adc_result.min_V < ADC_BOUND_MIN || adc_result.max_V > ADC_BOUND_MAX);
	}
	if(oob) {
		printP("Error: too much signal is being applied to input.\n");
		return;
	}
	if(range && adc_result.range != range) {
		printP("Error: range could not be set.\n");
		return;
	}

	rms_t rms = compute_rms();

	calib.A[range] = realval;
	if(range) {
		calib.B[range] = rms.I;
	} else {
		calib.B[0] = rms.V;
	}

	show_calib(range);

	write_calibration_to_eeprom();
}

void free_running() {
	uint32_t idles = 0;
	for(;;) {
		wdt_reset();

		while(!adc_result_ready) {
			idles++;
			wdt_reset();
			if(peekchr()) {
				char c = getchr();
				if(c >= '0' && c <= '0'+NUM_I_CHANNELS) {
					uint8_t range = c - '0';
					adc_wanted_range = range;
					if(range) {
						print_keyP_val32("\nRange set to ", range);
					} else {
						printP("\nRange set to auto");
					}
					putchr('\n');
				} else return;
			}
		}
		adc_result_ready = 0;

		printP("\n---\n");
		print_keyP_val32("idles: ", idles);

#if USE_MMC
		DSTATUS disk_stat = disk_initialize();
		DRESULT disk_rstat = RES_ERROR;
		if(disk_stat == 0) {
			disk_rstat = disk_read(0);
		}

		print_keyP_val32("disk: ", disk_stat);
		if(disk_rstat == RES_OK) {
			printP("read: ");
			for(uint8_t i=0; i<32; i++)
				print_hex8(mmc_buf[i]);
			putchr('\n');
		}

		wdt_reset();
#endif

		pcf_time_t time_buf;
		pcf_read_time(0, &time_buf);
		printP("time: ");
		rtc_print_pretty_date(&time_buf);
		putchr(' ');
		rtc_print_pretty_time(&time_buf);
		putchr('\n');

		wdt_reset();

		print_keyP_val32("nsamps: ", adc_result.nsamps);
		print_keyP_val32("aborted: ", adc_result.is_aborted);
		print_keyP_val32("range: ", adc_result.range);
		print_keyP_val32("min_I: ", adc_result.min_I);
		print_keyP_val32("max_I: ", adc_result.max_I);
		print_keyP_val32("min_V: ", adc_result.min_V);
		print_keyP_val32("max_V: ", adc_result.max_V);

		if(!adc_result.is_aborted) {
			wdt_reset();
			rms_t rms = compute_rms();
			wdt_reset();

			print_keyP_val32("rms I*10:  ", rms.I);
			print_keyP_val32("rms V*10:  ", rms.V);
			print_keyP_val32("avg P*100: ", rms.P);

			wdt_reset();

			rms_to_decimal(&rms, adc_result.range);

			wdt_reset();

			print_decimal(rms.V, 1);
			printP(" V, ");
			print_decimal(rms.I, 2);
			printP(" A, ");
			print_decimal(rms.P, 2);
			printP(" W\n");
		}

		putchr('\n');

		wdt_reset();

		idles = 0;
	}
}

void do_calibration() {
	for(;;) {
		show_free_stack();
		printP("Calibration:\n");
		printP("	f:  free running\n");
		printP("	cN: voltage/current (N=0..");
		print_int32(NUM_I_CHANNELS);
		printP(")\n");
		printP("	r:  time\n");
		printP("	q:  exit debug\n");
		char c = getchr();
		if(c == 'c') {
			char c = getchr();
			if(c >= '0' && c <= '0'+NUM_I_CHANNELS) {
				uint8_t range = c - '0';
				set_calibration(range);
			}
		} else if(c == 'f') {
			free_running();
		} else if(c == 'r') {
			rtc_set_from_serial(0);
		} else if(c == 'q') {
			return;
		}
	}
}
