/* 
NICOLAUS ADI NUGROHO
WWW.ANTARBLOG.COM
UCUPZ_LC@YAHOO.CO.ID
*/

//INISIALISASI ATMEGA8535
#include <mega8535.h>

//LCD PADA PORTC
#asm
.equ __lcd_port=0x15 ;PORTC
#endasm

//MEMASUKAN RUTIN-RUTIN LCD DAN DELAY
#include <lcd.h>
#include <delay.h>

int j,k;

typedef unsigned char byte;

//GAMBAR PADA 1 KOLOM
flash byte char0[8]={
0b00000000,
0b00000000,
0b00000000,
0b00000000,
0b00000000,
0b00000000,
0b00000000,
0b00000000,};

flash byte char1[8]={
0b11110000,
0b11110000,
0b11110000,
0b11110000,
0b11110000,
0b11110000,
0b11110000,
0b11110000};

flash byte char2[8]={
0b11111000,
0b11111000,
0b11111000,
0b11111000,
0b11111000,
0b11111000,
0b11111000,
0b11111000};

flash byte char3[8]={
0b11111100,
0b11111100,
0b11111100,
0b11111100,
0b11111100,
0b11111100,
0b11111100,
0b11111100};

flash byte char4[8]={
0b11111110,
0b11111110,
0b11111110,
0b11111110,
0b11111110,
0b11111110,
0b11111110,
0b11111110};

flash byte char5[8]={
0b11111111,
0b11111111,
0b11111111,
0b11111111,
0b11111111,
0b11111111,
0b11111111,
0b11111111};

//FUNGSI UNTUK DEFINISI KARAKTER
void define_char(byte flash *pc,byte char_code)
{
byte i,a;
a=(char_code<<3)|0x40;
for (i=0; i<8; i++) lcd_write_byte(a++,*pc++);
}

void main(void)
{
//INISIALISASI LCD
lcd_init(16);

//MEMBERI NAMA KARAKTER (0,1,2,3,4,5)
define_char(char0,0);
define_char(char1,1);
define_char(char2,2);
define_char(char3,3);
define_char(char4,4);
define_char(char5,5);

//KURSOR PADA 0,0
lcd_gotoxy(0,0);
lcd_putsf("KARAKTER :");

while (1)
{
//16 KOLOM PADA LCD-->MAJU
for (j=0;j<=16;j++)
{
lcd_gotoxy(j,1);
    //5 BARIS TIAP KOLOM
    for (k=0;k<=5;k++)
    {lcd_gotoxy(j-1,1);lcd_putchar(5);
    lcd_gotoxy(j,1);lcd_putchar(k);delay_ms(2);
    };
}
//MUNDUR
for (j=16;j>-1;j--)
{
lcd_gotoxy(j,1);
    for (k=5;k>-1;k--)
    {
    lcd_gotoxy(j,1);lcd_putchar(k);delay_ms(2);
    };
}

}
}
