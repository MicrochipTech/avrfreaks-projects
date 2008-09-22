// gcc -std=gnu99 -g -Wall -Os -o disk_reader disk_reader.c -lm -DF_CPU=16000000

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "config.h"
#include "rms64.h"
#include "pcf-rtc.h"

#define SECTOR_LEN 512
#define ENTRY_LEN 0x35

typedef struct {
	uint32_t block_idx;
	calib_t calib;
} header_t;

void fatal(const char *msg) {
	fprintf(stderr, "error: %s\n", msg);
	exit(1);
}

void read_block(FILE *fh, void *buf) {
	if(1 != fread(buf, SECTOR_LEN, 1, fh))
		fatal("read error");
}

void rtc_print_pretty(pcf_time_t *time_buf) {
	uint8_t month = (time_buf->month & 0xf) + (time_buf->month & 0x10 ? 10 : 0);
	if(month < 1 || month > 12) {
		printf("???");
	} else {
		const char *months = 
			"Jan\0Feb\0Mar\0Apr\0May\0Jun\0Jul\0Aug\0Sep\0Oct\0Nov\0Dec";
		printf(months + (month-1)*4);
	}
	printf("/%02x/%02x:%02x:%02x.%02x",
		time_buf->date & 0x3f,
		time_buf->hour,
		time_buf->minute,
		time_buf->second,
		time_buf->hundredths);
}

double crank(int64_t q, int32_t a, int32_t b, int32_t nconv) {
	int64_t diff = q * nconv - (int64_t)a * (int64_t)b;
	return (double)diff / nconv / nconv;
}

int main() {
	FILE *fh = fopen("/dev/mmcblk0", "rb");
	if(!fh) fatal("could not open device");

	uint8_t buf[SECTOR_LEN];
	int entries_per_block = MMC_BUF_LEN / ENTRY_LEN;

	read_block(fh, buf);
	header_t *header = (header_t *)buf;
	uint32_t nblocks = header->block_idx;
	double calib[NUM_I_CHANNELS+1];
	for(int i=0; i<NUM_I_CHANNELS+1; i++) {
//		printf("calib[%d] = %d, %d\n", i, 
//			header->calib.A[i], header->calib.B[i]);
		calib[i] = 
			(double)header->calib.A[i] / 
			(double)header->calib.B[i];
	}

	//for(int i=0; i<4; i++) {
	//	printf("calib[%d] = %d / %d\n", i, calib.A[i], calib.B[i]);
	//}

	int samps_per_second = ADC_CONV_PER_SEC;
	double energy = 0;

	for(int block_idx=1; block_idx<=nblocks; block_idx++) {
		//printf("block %d\n", block_idx);
		read_block(fh, buf);
		for(int log_idx=0; log_idx<entries_per_block; log_idx++) {
			pcf_time_t *t = (pcf_time_t *)(buf + log_idx*ENTRY_LEN);
			power_t *p = (power_t *)(buf + log_idx*ENTRY_LEN + 7);

			if(p->range < 0 || p->range >= NUM_I_CHANNELS+1) {
				printf("error\n");
				continue;
			}

			//printf("%8.3f\n", (double)p->sum_V / p->nsamps);
			
			double V = sqrt(crank(p->sum_V2, p->sum_V, p->sum_V, p->nsamps)) 
				* calib[0];
			double I = sqrt(crank(p->sum_I2, p->sum_I, p->sum_I, p->nsamps)) 
				* calib[p->range] / 10.0;
			double P = crank(p->sum_P, p->sum_V, p->sum_I, p->nsamps)
				* calib[0] * calib[p->range] / 10.0;

			// because of the inverting amplifiers:
			if(p->range == 2) P = -P;

			double dt = (double)p->nsamps / samps_per_second;
			energy += P * dt;

			rtc_print_pretty(t);
			printf("  ");

			//printf("r=%d n=%d %7.3lfV %5.3lfA %9.3lfW %9.3lfkWh\n", 
			//	p->range, p->nsamps, V, I, P,
			//	energy / 1000.0 / 3600.0);
			printf("%g %g %g\n", V, I, P);
		}
	}
}
