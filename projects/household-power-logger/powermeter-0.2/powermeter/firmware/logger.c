#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <avr/wdt.h>
#include <avr/pgmspace.h>

#include "config.h"
#include "serial.h"
#include "rms64.h"
#include "mmc.h"
#include "pcf-rtc.h"

#define DEBUG 0
#define MAX_SAMPS (ADC_CONV_PER_SEC * 2L)

typedef struct {
	uint32_t block_idx;
	calib_t calib;
} header_t;

typedef struct {
	pcf_time_t t;
	power_t p;
} logentry_t;

#define power_log ((logentry_t *)mmc_buf)
#define log_len (MMC_BUF_LEN / sizeof(logentry_t))

static uint8_t have_disk = 0;
static uint8_t log_idx = 0;
static uint32_t block_idx = 0;

static uint8_t init_disk() {
	wdt_reset();
	DSTATUS disk_stat = disk_initialize();
	wdt_reset();
#if DEBUG
print_keyP_val32("disk_initialize=", disk_stat);
#endif
	if(disk_stat) return 1;

	wdt_reset();
	DRESULT disk_rstat = disk_read(0);
	wdt_reset();
#if DEBUG
print_keyP_val32("disk_rstat=", disk_rstat);
#endif
	if(disk_rstat != RES_OK) return 1;

	block_idx = ((header_t *)mmc_buf)->block_idx + 1;
#if DEBUG
print_keyP_val32("init block_idx=", block_idx);
#endif
	log_idx = 0;
	memset(power_log, 0, log_len*sizeof(logentry_t));

	have_disk = 1;

	return 0;
}

static uint8_t flush_log() {
	wdt_reset();
	DRESULT disk_wstat = disk_write(block_idx);
	wdt_reset();
#if DEBUG
print_keyP_val32("disk_wstat=", disk_wstat);
#endif
	if(disk_wstat != RES_OK) return 1;

	((header_t *)mmc_buf)->block_idx = block_idx;
	((header_t *)mmc_buf)->calib = calib;

	wdt_reset();
	disk_write(0);
	wdt_reset();

	memset(power_log, 0, log_len*sizeof(logentry_t));

	block_idx++;
#if DEBUG
print_keyP_val32("new block_idx=", block_idx);
#endif
	log_idx = 0;

	return 0;
}

void logger_erase_disk() {
	if(!have_disk) init_disk();
	if(!have_disk) {
		printP("No disk.\n");
		return;
	}
	printP("Erasing...\n");
	block_idx = 0;
	flush_log();
	have_disk = 0;
	init_disk();
	if(!have_disk) {
		printP("Could not reinit.\n");
	} else if(block_idx != 1) {
		printP("Could not erase.\n");
	} else {
		printP("Card erased.\n");
	}
}

void accum_powerlog(pcf_time_t *time) {
	if(!have_disk) {
		if(init_disk()) return;
	}

	power_t *p = &((power_log + log_idx)->p);

	if(p->nsamps == 0) {
		p->range = adc_result.range;
		(power_log + log_idx)->t = *time;
	}

	if(
		p->range != adc_result.range ||
		p->nsamps + adc_result.nsamps > MAX_SAMPS
	) {
#if DEBUG
if(p->range != adc_result.range) {
print_keyP_val32("range changed to ", adc_result.range);
}
#endif
		log_idx++;

		if(log_idx == log_len) {
			uint8_t err = flush_log();
			if(err) {
				have_disk = 0;
				return;
			}
			log_idx = 0;
		}

		p = &((power_log + log_idx)->p);
		p->range = adc_result.range;
		(power_log + log_idx)->t = *time;
	}

	if(!p->nsamps || adc_result.min_I < p->min_I) p->min_I = adc_result.min_I;
	if(!p->nsamps || adc_result.max_I < p->max_I) p->max_I = adc_result.max_I;
	if(!p->nsamps || adc_result.min_V < p->min_V) p->min_V = adc_result.min_V;
	if(!p->nsamps || adc_result.max_V < p->max_V) p->max_V = adc_result.max_V;

	p->nsamps += adc_result.nsamps;
	p->sum_I  += adc_result.sum_I;
	p->sum_V  += adc_result.sum_V;
	//acc64(&(p->sum_P ), &adc_result.sum_P );
	//acc64(&(p->sum_I2), &adc_result.sum_I2);
	//acc64(&(p->sum_V2), &adc_result.sum_V2);
	p->sum_P  += adc_result.sum_P;
	p->sum_I2 += adc_result.sum_I2;
	p->sum_V2 += adc_result.sum_V2;
}

void print_powerlog_idx() {
	if(!have_disk) {
		printP("no disk\n");
	} else {
		power_t *p = &((power_log + log_idx)->p);
		printP("log ");
		print_int32(block_idx);
		putchr(',');
		print_int32(log_idx);
		putchr(',');
		print_int32(p->nsamps);
		putchr('\n');

		wdt_reset();
	}
}
