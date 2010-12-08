#ifndef timing_H
#define timing_H

struct time {
	unsigned char second;
	unsigned char minute;
	unsigned char hour;
	unsigned char day;
	unsigned char month;
	unsigned int  year;
};

void usleep(int usec);
void msleep(int msec);

void InitTimer2(void);

#endif // timing_H
