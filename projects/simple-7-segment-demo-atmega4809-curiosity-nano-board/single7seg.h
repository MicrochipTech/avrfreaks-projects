/*
 * single7seg.h
 *
 * Created: 12/23/2020 8:32:47 PM
 *  Author: Chip
 */ 


#ifndef SINGLE7SEG_H_
#define SINGLE7SEG_H_

#include <stdbool.h>

void init_7seg();
void setDisplay(uint8_t val, bool dp);

#endif /* SINGLE7SEG_H_ */