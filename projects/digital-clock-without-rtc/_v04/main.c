/*
Judul : jam digital

deskripsi : 
program jam digital dengan penampil 7 segmen sebanyak 4 buah dengan driver 7446
menggunakan kristal 11059200mhz dengan timer overflow setiap 5ms sehingga 1 detik = 200x
menggunakan mikrokontroler atmega8535

versi : 03

lanjutan dari versi 1. 
penyempurnaan tindaan agar mendapat hasil proses yang tepat selama 1 detik pada saat
time overflow

perhatikan lama tundaan keadaan atau nilai yang akan di capai oleh pencacah mc_count...

mc_count -->> menentukan lama tidaknya tundaan, tepat tidaknya tundaan dalam setiap detik
	akibatnya jika terlambat maka perubahan detik akan tidak tepat sehingga jam akan tidak tepat.....
	
lanjutan dari versi 2.
pada versi 3 di tambahkan komunikasi dengan serial agar perubahan waktu dapat di lihat melalui
teminal serial.

lanjutan dari versi 3.
pada versi 4 ada perubahan waktu intrupsi yaitu dari 5ms menjadi 2.5ms dengan mengubah
nilai overflow
sehingga mengubah banyak pencacahan dari 200 menjadi 400 cacahan.

henry R
16-09-08



*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>




///definisi waktu
// waktu tik yaitu setiap 5ms
// menggunakan timer overflow pada timer 1 mode normal
// dengan kristal sebesar 11059200hz
// menggunakan priskaler 1024
//

//#define time_tick 54735//65481
#define time_tick 65508
//#define time_tick 65319

// definisi tepe data bentukan 
typedef unsigned char u8;
typedef unsigned int u16;


//definisi tempat untuk meletakkan data sebagai acuan mengubah angka menjadi string
prog_char Angka[] = "0123456789ABCDEF";


// variabel global penunjuk waktu



static volatile u16 ms_count = 0;
static volatile u8 detik = 0;
static volatile u8 menit = 0;
static volatile u8 jam = 0;
static volatile u8 loudnes = 0;
static volatile u8 puluhan = 0;
static volatile u8 satuan = 0;

// fungsi inisialisasi perangkat keras
void init_hw(void)
{
	// inisialisasi timer
	// priskalar clock/1024
	TCCR1B = 0x05;
	// inisialisasi intrupsi
	TIFR = 1<<TOV1;
	TIMSK |= 1<<TOIE1;
	TCNT1 = time_tick;
	
	// inisialisasi port
	DDRA = 0xff; // port a sebagai keluaran
	PORTA = 0xff;
	
}

// fungsi inisialisasi uart
void init_uart(u16 baud)
{
	UBRRH = (u8) (baud>>8);
	UBRRL = (u8) baud;
	UCSRB = (1<<RXEN)|(1<<TXEN);
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
}

void kirim_byte(u8 data)
{
	while (!( UCSRA & (1<<UDRE)));
	UDR = data;
}

void dec2str(u8 data)
{
	puluhan = data/10;
	satuan = data - (puluhan * 10);
}

void konversi_kirim2monitor(void)
{
	u8 temp;
	
	// menampilkan jam
	dec2str(jam);
	temp = pgm_read_byte(&Angka[puluhan]);
	kirim_byte(temp);
	temp = pgm_read_byte(&Angka[satuan]);
	kirim_byte(temp);
	kirim_byte(':');
	
	//menampilkan menit
	dec2str(menit);
	temp = pgm_read_byte(&Angka[puluhan]);
	kirim_byte(temp);
	temp = pgm_read_byte(&Angka[satuan]);
	kirim_byte(temp);
	kirim_byte(':');
	
	//menampilkan detik
	dec2str(detik);
	temp = pgm_read_byte(&Angka[puluhan]);
	kirim_byte(temp);
	temp = pgm_read_byte(&Angka[satuan]);
	kirim_byte(temp);
	kirim_byte('\r');
}
	

SIGNAL(SIG_OVERFLOW1)
{

	TCNT1 = time_tick;
	if (++ms_count == 384){  // 196 -- mendekati kenyataan  
							  //lama tundaan ini yang harus di perhatikan
	proses_jam();
	ms_count=0;
	}

	
}


// fungsi jam dan kenaikan nya
void proses_jam(void)
{
	detik++;
	if (detik > 59 ) {
		detik = 0;
		menit++;
		if (menit > 59 ) {
			menit = 0;
			jam++;
			if (jam == 24) {
			jam = 0;
			}
		}
	}
	
//		if (detik == 5)
//		{
//		PORTA = 0x00;
		konversi_kirim2monitor();
//		}
	
}

int main(void)
{
	init_hw();
	init_uart(71);
	
	sei();
	
	
	while(1)
	{

	}
	return 0;
}
		