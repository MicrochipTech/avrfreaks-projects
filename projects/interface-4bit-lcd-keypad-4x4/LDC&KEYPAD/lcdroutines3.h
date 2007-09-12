/*
versi 2 coy

tanggal penulisan 15-08-07
refisi 1. 16-08-07
		penggantian file header dari #include <util/delay.h> menjadi #include <util/delayfortunda.h>
		#include <util/delayfortunda.h> --> di buat sendiri khusus kristal 11.0592MHz untuk mengurangi
		besar program karena penggunaan #include <util/delay.h>

header untuk mengakses LCD 16x2 dengan jalur data 4 bit

konfigurasi:

Pin LCD		-	Pin-µC 
1 Vss			GND 
2 Vcc			5V 
3 Vee			GND oder Poti (siehe oben) 
4 RS			bit 4 portAVR
5 RW			bit 6 portAVR 
6 E				bit 7 portAVR
7 DB0			-     
8 DB1			-     
9 DB2			-     
10 DB3			-     
11 DB4			bit 0 portAVR         
12 DB5			bit 1 portAVR       
13 DB6			bit 2 portAVR          
14 DB7			bit 3 portAVR          


di koreksi oleh Ricky Henry Rawung / USD / 045114010 / henry_ryq@yahoo.com

header lcdroutines.h di peroleh dari AVR-Tutorial LCD - www_mikrocontroller_net.mht dan di koreksi lagi hingga 
menjadi bentuk file header. dengan penyempurnaan ini maka USER di permudah untuk dapat langgsung
mengakses function yang terdapat di dalamnya

seperti 
-	void lcd_data(unsigned char temp1);			-- untuk penulisan karakter
-	void lcd_command(unsigned char temp1);		-- untuk penulisan perintah
-	void lcd_enable(void);						-- untuk hendsheking pada pin enable
-	void lcd_init(void);						-- inisialisasi awal pada LCD
-	void lcd_home(void);						-- miminta kursor kembali ke posisi awal
-	void lcd_clear(void);						-- kosongkan layar
-	void set_cursor(uint8_t x, uint8_t y);		-- set posisi kursor

untuk keterangan lebih lanjut liat DATASHEET LCD 16x2
*/

#include <avr/io.h>
//#include <util/delay.h>
#include <util/delayfortunda.h>


#define CLEAR_DISPLAY 0x01
#define CURSOR_HOME   0x02



void lcd_data(unsigned char temp1);
void lcd_command(unsigned char temp1);
void lcd_enable(void);
void lcd_init(void);
void lcd_home(void);
void lcd_clear(void);
void set_cursor(uint8_t x, uint8_t y); 



#ifndef LCD_PORT
/* prevent compiler error by supplying a default */
# warning "LCD_PORT not defined for <util/lcdroutines.h>"
# define LCD_PORT PORTC
#endif

#ifndef LCD_DDR
/* prevent compiler error by supplying a default */
# warning "LCD_DDR not defined for <util/lcdroutines.h>"
# define LCD_DDR DDRC
#endif




// keadaan awal LCD
 
#define CLEAR_DISPLAY 0x01
#define CURSOR_HOME   0x02
 
// definisi standart pin port pada LCD
 

#define LCD_RS      4
#define LCD_RW      6
#define LCD_EN      7

 
 
// fungsi pengiriman data pada LCD
 
void lcd_data(unsigned char temp1)
{
   unsigned char temp2 = temp1;
 
   LCD_PORT |= (1<<LCD_RS);        // Untuk menulis data Pin RS LCD harus bernilai 1
 
   temp1 = temp1 >> 4;			   // Geser data nibel atas menjadi nibel bawah	
   temp1 = temp1 & 0x0F;		   // Menolkan data nibel atas, data terdapat pada nibel bawa
   LCD_PORT &= 0xF0;			   // menyiapkan LCD_PORT untuk nibel bawa dengan tetep menyimpan data pada LCD_PORT nibel atas
   LCD_PORT |= temp1;              // Kirim data ke LCD_PORT
   lcd_enable();				   // hendseking pada pin enable
 
   temp2 = temp2 & 0x0F;		   // mengambil data nibel bawah
   LCD_PORT &= 0xF0;			   // menyiapkan LCD_PORT
   LCD_PORT |= temp2;              // mengisi data para LCD_PORT
   lcd_enable();				   // handsekong pada pin enable
   
  
	 delay20ms();		   // penundaan ini di perlukan oleh LCD untuk memproses data yang diterima
}
 
// fungsi pengiriman instruksi pada LCD
 
void lcd_command(unsigned char temp1)
{
   unsigned char temp2 = temp1;
 
    LCD_PORT &= ~(1<<LCD_RS);       // Untuk menulis instruksi Pin RS LCD harus bernilai 0
 
   temp1 = temp1 >> 4;              // geser data nibel atas menjadi nibel bawa
   temp1 = temp1 & 0x0F;            // Menolkan data nibel atas, data terdapat pada nibel bawa
   LCD_PORT &= 0xF0;
   LCD_PORT |= temp1;               // mengisi data para LCD_PORT
   lcd_enable();
 
   temp2 = temp2 & 0x0F;            // mengambil data nibel bawah
   LCD_PORT &= 0xF0;
   LCD_PORT |= temp2;               // mengisi data para LCD_PORT
   lcd_enable();
   
   delay20ms();
				// penundaan ini di perlukan oleh LCD untuk memproses data yang diterima
}
 
// fungsi handseking pada pin enable
void lcd_enable(void)
{
   LCD_PORT |= (1<<LCD_EN);
   delay1ms();				// penundaan ini di perlukan oleh LCD untuk memproses data yang diterima

   LCD_PORT &= ~(1<<LCD_EN);
}
 
// fungsi inisialisasi LCD
 
void lcd_init(void)
{
   delay20ms();					// penundaan ini di perlukan untuk memastikan LCD tidak dalam keadaan sibuk
   LCD_PORT &= 0xF0;				// 
   LCD_PORT |= 0x03;            
   LCD_PORT &= ~(1<<LCD_RS);      // Untuk menulis instruksi Pin RS LCD harus bernilai 0
   lcd_enable();
 
   delay5ms();
   lcd_enable();
 
   delay1ms();
   lcd_enable();
 
   // meng set 4 bit transfer data 
   LCD_PORT &= 0xF0;
   LCD_PORT |= 0x02;
   lcd_enable();
 
   // mengatur pergeseran tampilan
    lcd_command(0x28);
    
   // menonaktifkan tampilan kursor
   lcd_command(0x0C); 
 
   // mengatur mode tampilan karakter
   lcd_command(0x06);
 
    lcd_clear();	// kosongkan layar
}
 
// Fungsi pengosingan layar
 
void lcd_clear(void)
{
   lcd_command(CLEAR_DISPLAY);
   delay5ms();

}
 
// fungsi untuk mengset cursor LCD pada posisi Home
 
void lcd_home(void)
{
   lcd_command(CURSOR_HOME);
   delay5ms();

}
 
// fungsi penempatan posisi cursor y (1..4) Spalte x (0..15)
 
void set_cursor(uint8_t x, uint8_t y)
{
  switch (y) {
    case 1: lcd_command(0x80+0x00+x); break;    // baris 1
    case 2: lcd_command(0x80+0x40+x); break;    // baris 2
    case 3: lcd_command(0x80+0x10+x); break;    // baris 3 jika LCD mendukung
    case 4: lcd_command(0x80+0x50+x); break;    // Baris 4 jika LCD mendukung
  }
}
