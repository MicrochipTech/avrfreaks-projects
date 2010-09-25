#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 32000000L // x4 !!!
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <util/delay.h>
#include <string.h>
#include "uart.h"
#include "flash.h"

const uint8_t blocksize=128;
const uint32_t flash_size=(uint32_t)256*1024;

volatile uint8_t state=0;

uint8_t *d;
uint8_t *buf2;

void init_ports(void)
{
    DDRC=0xFF;
    PORTC=0x00;
    DDRB=0x07;
    PORTB&=~0x07;
    DDRD=0xF2;
    PORTD=0x70;
    led_off;
}

void rec_cb(uint16_t len, uint8_t *buf)
{
    if (state==0)
    {
        buf[len]=0;
        if (!strcmp((char *)buf,"Hello Flasher\n"))
        {
            printf("Hello Computer\n");
            state=1;
        }
        return;
    }
    if (state==1)
    {
        buf[len]=0;
        if (!strcmp((char *)buf,"Bye Flasher\n"))
        {
            printf("Bye Computer\n");
            state=0;
            return;
        } 
        if (!strcmp((char *)buf,"write\n"))
        {
            state=2;
            return;
        }
        if (!strcmp((char *)buf,"read\n"))
        {
            state=5;
            return;
        }
    }
    if (state==4)
    {
        d=buf;
        state=3;
        return;
    }
    if (state==7)
    {
        if (!strcmp((char *)buf,"ready\n"))
        {
            state=6;
            return;
        }
    }
    
}

void init_devices(void)
{
    cli();
    init_ports();
    uart_init(rec_cb);
    sei();
}

uint8_t *request_block_data(uint32_t adr,uint16_t len)
{
    state=4;
    set_limit(blocksize+1,255);
    printf("req ");
    print_adr(adr);
    printf(" %d\n",len);
    while(state==4);
    set_limit(255,'\n');
    
    return d;
}

uint8_t check_block(uint8_t *b,uint16_t len)
{
    int i;
    uint8_t c=0;
    for(i=0;i<len;i++)
    {
        c=c ^ b[i];
    }
    return c!=b[len];
}

void write(void)
{
    uint32_t N, last_block_l,i;
    uint8_t *block;
//    printf("state=2\n");
    state=3;
    N=flash_size/blocksize;
    last_block_l=(flash_size % blocksize);
    ce_down;
    chip_erase();
    ce_up;
    for(i=0; i<N; i++)
    {
        block=request_block_data(blocksize*i,blocksize);
        if (check_block(block,blocksize))
        {
            printf("Error\n");
            state=1;
        }
        if (programm_block(blocksize*i,blocksize,block))
        {
            printf("Error\n");
            state=1;
        }
    }
    if (last_block_l>0)
    {
        block=request_block_data(blocksize*N,last_block_l);
        if (check_block(block,last_block_l))
        {
            printf("Error\n");
            state=1;
        }
        if (programm_block(blocksize*N,last_block_l,block))
        {
            printf("Error\n");
            state=1;
        }
    }
    printf("done\n");
    state=1;
}

void send_block(uint32_t adr, uint16_t len)
{
    uint32_t i;
    uint8_t crc=0,c;
    printf("send ");
    print_adr(adr);
    printf(" %d\n",len);

    for(i=0;i<len;i++)
    {
        c=read_byte(i+adr);
        crc=c^crc;
        buf2[i]=c;
    }
    buf2[len]=crc;
    for(i=0;(i<len+1);i++)
    {
        putch(buf2[i]);
    }
    putch(crc);
    
    state=7;
    while(state==7);
    
}


void read(void)
{
    uint32_t i;
    printf("send ");
    print_adr(flash_size);
    printf("\n");
    ce_down;
    for (i=0; i< flash_size; i++)
    {
        putch(read_byte(i));
    }
    ce_up;
    state=1;
}


int main(void)
{
//    uint32_t i;
    init_devices();
//    printf("\n-----------\n");
//    ce_down;
//    for(i=0;i<20;i++)
//    {
//        printf("0x%0X\n",read_byte(i));
//    }
//    ce_up;
    while(1)
    {
        if(state==2) write();
        if(state==5) read();
    }
}
