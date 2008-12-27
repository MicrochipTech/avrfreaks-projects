// gcc -std=gnu99 -Wall -O2 -o avr-gnuplot-threaded avr-gnuplot-threaded.c -lpthread

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>
#include <getopt.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

#define NCHAN 8
#define BUF_LEN 400

#define NBUF 3
int read_buf = 0, write_buf = 1, queue_buf = 2;

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;
int block_queued = 0;
int want_quit = 0;

typedef struct {
	int good[NCHAN];
	int val[NCHAN];
} multisamp_t;

int have_any[NBUF][NCHAN];
multisamp_t sampbuf[NBUF][BUF_LEN];

void trigger_quit() {
	pthread_mutex_lock(&queue_mutex);
	want_quit = 1;
	pthread_cond_signal(&queue_cond);
	pthread_mutex_unlock(&queue_mutex);
}

/////////////// reader thread ////////////////////

typedef struct {
	int buf_len;
	int buf_idx;
	int prev_chan;
} reader_state_t;

void clear_block(reader_state_t *state) {
	memset(sampbuf[read_buf], 0, sizeof(multisamp_t) * state->buf_len);
	memset(have_any[read_buf], 0, sizeof(int) * NCHAN);
}

void push_sample(int channel, int val, reader_state_t *state) {
	if(channel < state->prev_chan) {
		state->buf_idx++;
		if(state->buf_idx == state->buf_len) {
			pthread_mutex_lock(&queue_mutex);
			int tmp = queue_buf; queue_buf = read_buf; read_buf = tmp;
			block_queued = 1;
			pthread_cond_signal(&queue_cond);
			pthread_mutex_unlock(&queue_mutex);

			clear_block(state);
			state->buf_idx = 0;
		}
	}
	state->prev_chan = channel;
	have_any[read_buf][channel] = 1;
	sampbuf[read_buf][state->buf_idx].good[channel] = 1;
	sampbuf[read_buf][state->buf_idx].val[channel] = val;
}

void *reader_thread(void *arg) {
	char *dev_fn = (char *)arg;

	reader_state_t state;
	state.prev_chan = -1;
	state.buf_idx = 0;
	state.buf_len = BUF_LEN;

	clear_block(&state);

	char *cmd = malloc(100+strlen(dev_fn));
	sprintf(cmd, "stty -F %s 2000000 raw cs8", dev_fn);
	fprintf(stderr, "Running '%s'\n", cmd);
	int ret = system(cmd);
	if(ret) {
		fprintf(stderr, "Error: stty failed\n");
		trigger_quit();
		return NULL;
	}

	fprintf(stderr, "Opening %s\n", dev_fn);
	FILE *fh = fopen(dev_fn, "r");
	if(!fh) {
		fprintf(stderr, "Error: could not open serial port\n");
		trigger_quit();
		return NULL;
	}

	#define RBLEN 1024
	uint8_t readbuf[RBLEN];
	int idx_r=0, len_r=0;
	
	uint16_t val = 0;
	int channel = 0;
	int num_overrun = 0;
	int first_loop = 1;
	while(!want_quit) {
		if(idx_r == len_r) {
			idx_r = 0;
			if(first_loop) fprintf(stderr, "Waiting for data\n");
			len_r = fread(readbuf, 1, RBLEN, fh);
			if(!len_r) {
				fprintf(stderr, "Got EOF\n");
				trigger_quit();
				return NULL;
			}
			if(first_loop) fprintf(stderr, "Got data\n");
			first_loop = 0;
		}
		int c = readbuf[idx_r++];

		if(c & 0x80) {
			int overrun = c & 0x40;
			if(overrun) fprintf(stderr, "overrun %d\n", (++num_overrun));
			channel = (c & 0x38) >> 3;
			val = (c & 0x07) << 7;
		} else {
			val += c;
			push_sample(channel, val, &state);
		}
	}

	return NULL;
}

/////////////// plot thread ////////////////////

void *plot_thread(void *arg) {
	FILE *gp = popen("/usr/bin/gnuplot", "w");
	if(!gp) {
		fprintf(stderr, "Error: could not start gnuplot\n");
		trigger_quit();
		return NULL;
	}

	fprintf(gp, "set yrange [0:1023]\n");
	fprintf(gp, "set key off\n");

	while(!want_quit) {
		pthread_mutex_lock(&queue_mutex);
		while(!block_queued && !want_quit) {
			pthread_cond_wait(&queue_cond, &queue_mutex);
		}
		int tmp = queue_buf; queue_buf = write_buf; write_buf = tmp;
		block_queued = 0;
		pthread_mutex_unlock(&queue_mutex);

		if(want_quit) return NULL;

		int num_good_chan = 0;
		for(int i=0; i<NCHAN; i++) {
			if(have_any[write_buf][i]) num_good_chan++;
		}
		if(!num_good_chan) continue;

		fprintf(gp, "plot ");
		for(int i=0; i<num_good_chan; i++) {
			if(i) fprintf(gp, ", ");
			fprintf(gp, "'-' u 1:2 w l");
		}
		fprintf(gp, "\n");

		for(int i=0; i<NCHAN; i++) {
			if(!have_any[write_buf][i]) continue;
			for(int j=0; j<BUF_LEN; j++) {
				if(!sampbuf[write_buf][j].good[i]) continue;
				fprintf(gp, "%d %d\n", j, sampbuf[write_buf][j].val[i]);
			}
			fprintf(gp, "e\n");
		}

		fflush(gp);
	}

	return NULL;
}

int main(int argc, char **argv) {
	if(argc != 2) {
		fprintf(stderr, "Usage: %s /dev/ttyUSB0\n", argv[0]);
		exit(1);
	}
	char *dev_fn = argv[1];

	pthread_t rt;
	int err = pthread_create(&rt, NULL, reader_thread, dev_fn);
	if(err) {
		fprintf(stderr, "Error: could not create reader thread\n");
		exit(1);
	}

	pthread_t pt;
	err = pthread_create(&pt, NULL, plot_thread, NULL);
	if(err) {
		fprintf(stderr, "Error: could not create gnuplot thread\n");
		exit(1);
	}

	pthread_join(rt, NULL);
	pthread_join(pt, NULL);

	return 0;
}
