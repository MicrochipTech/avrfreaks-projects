# surveillance payload rocket

Uploaded by ezufatrin on 2011-02-18 03:17:00 (rating 0 out of 5)

## Summary

3.1 Mikrokontroller ATMega8535  

3.1.1 Keluarga AVR  

AVR (Alf and Vegardâ€™s Risc Processor) merupakan seri mikrokontroler CMOS 8-bit buatan Atmel, berbasis arsitektur RISC (Reduced Instruction Set Computer). Hampir semua instruksi dieksekusi dalam 1 siklus clock, berbeda dengan instruksi MCS51 yang membutuhkan 12 siklus clock. AVR mempunyai 32 register serbaguna, Timer/Counter fleksibel dengan mode compare, interrupt internal dan eksternal, serial UART, programmable Watchdog Timer, dan mode power saving. Beberapa di antaranya mempunyai ADC dan PWM internal. AVR juga mempunyai In-System Programmable Flash on-chip yang memungkinkan memori program untuk diprogram ulang dalam sistem menggunakan hubungan serial SPI. Arsitektur perangkat keras ATmega 8535 ditunjukkan pada Gambar 1.


Gambar 1 Arsitektur perangkat keras ATmega 8535  

Dari Gambar 1 dapat dilihat bahwa ATmega 8535 memiliki bagian sebagai berikut :


1. Saluran I/O sebanyak 32 buah.  

2. ADC 10 bit sebanyak 8 saluran.  

3. Tiga buah Timer/Counter dengan kemampuan pembanding.  

4. CPU yang terdiri atas 32 buah register.  

5. Watchdog Timer dengan osilator internal.  

6. SRAM sebesar 512 byte.  

7. Memori flash sebesar 8 kb dengan kemampuan Read While Write.  

8. Unit interupsi internal dan eksternal.  

9. Port antarmuka SPI.  

10. EEPROM sebesar 512 byte.  

11. Antarmuka komparator analog.  

12. Port USART untuk komunikasi serial.  

13. Empat kanal PWM.  

14. Tegangan operasi sekitar 4,5 - 5,5 V.


Secara umum, AVR dapat dikelompokkan menjadi 4 kelas, yaitu keluarga ATtiny, keluarga AT90Sxx, keluarga ATmega, dan AT86RFxx. Pada dasarnya yang membedakan masing-masing kelas adalah memori, peripheral, dan fungsinya. Dari segi arsitektur dan instruksi yang digunakan, bisa dikatakan hampir sama. Semua jenis AVR dilengkapi dengan flash memori sebagai memori program. Kapasitas dari flash memori ini berbeda antara chip yang 1 dengan chip yang lainnya tergantung dari jenis IC-nya. Untuk flash memori yang paling kecil adalah 1Kbytes (pada ATtiny11, ATtiny12, dan ATtiny15) dan paling besar adalah 128 Kbytes (ATmega128).  

3.1.2 Fitur-fitur ATMega 8535  

 Beberapa keistimewaan dari AVR ATmega 8535 antara lain:  

1. Sistem mikroprosesor 8 bit berbasis RISC dengan kecepatan maksimal 16 MHz.  

2. Kemampuan memori flash 8KB, SRAM sebesar 512 byte, dan EEPROM (Electrically Erasable Programmable Read OnlyMemory) sebesar 512 byte.  

3. ADC (Analog to Digital Converter) internal dengan resolusi 10 bitsebanyak 8 saluran.  

4. Port komunikasi serial (USART) dengan kecepatan maksimal 2,5Mbps.  

5. Enam pilihan mode sleep menghemat penggunaan daya listrik.


3.1.3 Konfigurasi Pin-Pin Mikrokontroler ATmega 8535  

Mikrokontroler ATmega 8535 memiliki 4 buah port masukan keluaran. Konfigurasi pin-pin ATmega 8535 dapat dilihat pada Gambar 2. 


Gambar 2 Konfigurasi pin-pin mikrokontroler ATmega 8535.


Penjelasan dari masing-masing pin adalah sebagai berikut :  

1. VCC (pin 10) merupakan pin yang berfungsi sebagai masukan catu daya.  

2. GND (pin 31) merupakan pin ground.  

3. Port A (PA7-PA0) (pin 33-40) merupakan port 8 bit I/O duaarah (bidirectional). Port ini dapat berfungsi sebagai ADC(Analog to Digital Converter).  

4. Port B (PB7-PB0) (pin 1-8) merupakan port 8 bit I/O dua arah (bidirectional). Port B mempunyai fungsi khusus untuk berbagai keperluan (multipurpose), yaitu Timer/Counter, komparator analog, dan SPI.  

5. Port C (PC7-PC0) (pin 22-29) merupakan port 8 bit I/O dua arah (bidirectional). Port C mempunyai fungsi khusus, yaitu TWI (Two-Wire Serial Interface), komparator analog, dan Timer Oscillator.  

6. Port D (PD0-PD7) (pin 14-21) merupakan port 8 bit I/O dua arah (bidirectional). Port D mempunyai fungsi khusus, yaitu komparator analog, interupsi eksternal, dan komunikasi serial.  

7. Reset (pin 9) merupakan pin yang digunakan untuk me-reset mikrokontroler.  

8. XTAL1 (pin 13) dan XTAL2 (pin 12) merupakan pin masukan clock eksternal.  

9. AVCC merupakan pin sumber tegangan untuk ADC dan harus terhubung dengan VCC walaupun ADC tidak digunakan.  

10. AREF merupakan tegangan referensi untuk ADC.


3.1.4 Struktur Memori ATmega 8535  

AVR ATmega 8535 memiliki ruang pengalamatan memori data dan memori program yang terpisah. Program ditempatkan di ISP (In-System Programmable/terprogram dalam sistem) Flash Memory. Memori data terbagi menjadi 3 bagian, yaitu 32 x 8 bit register serbaguna, 64 x 8 bit register I/O, dan 512 x 8 bit SRAM internal.


3.1.4.1 Memori Program  

Memori program pada ATmega 8535 dapat dilihat pada Gambar 3. ATmega 8535 mempunyai kapasitas memori program sebesar 8 Kbytes yang terletak dalam flash PEROM.


Gambar 3 Memori program.


Karena semua format instruksi berupa kata (word) yang memiliki lebar 16-32 bit, maka ATmega 8535 memiliki 4 Kbyte x 16 bit flash PEROM dengan alamat mulai dari $000 sampai $FFF. Program Counter (PC)-nya sepanjang 12 bit sehingga mampu mengakses hingga 4096 lokasi memori.


3.1.4.2 Memori Data  

Enam ratus delapan alamat lokasi memori data digunakan untuk file register, memori I/O, dan data SRAM internal. Sembilan puluh enam lokasi pertama dialamatkan untuk file register dan 512 lokasi dialamatkan untuk data SRAM internal. Memori data yang terdiri dari 32 register serbaguna, 64 register I/O, dan SRAM dapat dilihat pada Gambar 2.21.


Gambar 4 Memori data.


3.1.4.3 Register Serbaguna  

Tiga puluh dua bytes general purpose working register atau register serbaguna mendukung adanya konsep register akses cepat. Hal ini berarti waktu akses dari register adalah 1 detak atau 1 operasi ALU (Arithmetic Logic Unit). Peta dari 32 register tersebut dapat dilihat pada Gambar 5.  

Enam dari 32 register dapat digunakan sebagai tiga penunjuk alamat tak langsung 16 bit untuk pengalamatan ruang data. Satu dari tiga penunjuk alamat ini juga digunakan sebagai penunjuk alamat pada konstanta fungsi tabel look-up. Tiga register fungsi tambahan ini disebut register 16 bit X,Y, dan Z.


Gambar 5 Tiga puluh dua register serbaguna.


Register serbaguna juga dapat dianggap sebagai alamat memori data. Register X,Y, dan Z dapat diatur sebagai indeks pada berbagai register. Register X, Y, dan Z tersebut dapat dilihat pada Gambar 6. 


Gambar 6 Register X, Y, dan Z.


3.1.5 Sistem Interupsi


Tabel 1 Sistem interupsi pada mikrokontroler ATmega 8535  

Vector No. Program Address Source Interrupt Definition  

1 0x00 RESET External Pin, Power on Reset, Brown out Reset and Watchdog Rese  

2 0x001 INT0 External Interrupt Request 0  

3 0x002 INT1 External Interrupt Request 1  

4 0x003 TIMER 2 COMP Timer/Counter 2 Compare Match  

5 0x004 TIMER 1 OVF Timer/Counter 2 Overflow  

6 0x005 TIMER 1 CAPT Timer/Counter 1 Capture Event  

7 0x006 TIMER 1 COMPA Timer/Counter 1 Compare Match A  

8 0x007 TIMER 1 COMPB Timer/Counter 1 Compare Match B  

9 0x008 TIMER 1 OVF Timer/Counter 1 Overflow  

10 0x009 TIMER 0 OVF Timer/Counter 0 Overflow  

11 0x00A SPI, STC Serial Transfer Complete  

12 0x00B USART, RXC USART, Rx Complete  

13 0x00C USART, UDRE USART Data Register Empty  

14 0x00D USART, TXC USART, Tx Complete  

15 0x00E ADC ADC Conversion Complete  

16 0x00F EE\_RDY EEPROM Ready  

17 0x010 ANA\_COMP Analog Comparator  

18 0x011 TWI Two-wire Serial Interface  

19 0x012 INT2 External Interrupt Request 2  

20 0x013 TIMER 0 COMP Timer/Counter 0 Compare Match  

21 0x014 SPM\_RDY Store Program Memory Ready


3.1.6 Ruang Memori I/O  

Ruang memori I/O pada ATmega 8535 berisi 64 alamat untuk fungsi peripheral CPU, seperti register kontrol, Timer/Counter, dan fungsi I/O yang lain. Semua I/O dan peripheral ATmega 8535 ditempatkan dalam ruang memori I/O.  

Tabel 2 Ruang alamat input-output ATmega 8535.  

Alamat dalam  

hexadesimal Nama Fungsi  

0x3F(0x5F) SREG Status Register  

0x3E(0x5E) SPH Stack Pointer High  

0x3D(0x5D) SPL Stack Pointer Low  

0x3C(0x5C) OCR0 Output Compare Register Timer/Counter 0  

0x3B(0x5B) GICR General Interrupt Control Register  

0x3A(0x5A) GIFR General Interrupt Flag Register  

0x39(0x59) TIMSK Timer/Counter Interrupt Mask Register  

0x38(0x58) TIFR Timer/Counter Flag Mask Register  

0x37(0x57) SPMCR Store Program Memory Control Register  

0x36(0x56) TWCR TWI Control Register  

0x35(0x55) MCUCR MCU General Control Register  

0x34(0x54) MCUCSR MCU General Control Status Register  

0x33(0x53) TCCR0 Timer/Counter 0 Control Register  

0x32(0x52) TCNT0 Timer/Counter 0 (8 bit)  

0x31(0x51) OSCCAL Oscillator Calibration  

0x30(0x50) SFIOR Special Function IO Register  

0x2F(0x4F) TCCR1A Timer/Counter 1 Control Register A  

0x2E(0x4E) TCCR1B Timer/Counter 1 Control Register B  

0x2D(0x4D) TCNT1H Timer/Counter 1 High Byte  

0x2C(0x4C) TCNT1L Timer/Counter 1 Low Byte  

0x2B(0x4B) OCR1AH Timer/Counter 1 Output Compare Register A High Byte  

0x2A(0x4A) OCR1AL Timer/Counter 1 Output Compare Register A Low Byte  

0x29(0x49) OCR1BH Timer/Counter 1 Output Compare Register B High Byte  

0x28(0x48) OCR1BL Timer/Counter 1 Output Compare Register B Low Byte  

0x25(0x45) ICR1H T/C 1 Input Capture Register High Byte  

0x24(0x44) ICR1L T/C 1 Input Capture Register Low Byte  

0x25(0x45) TCCR2 Timer/Counter 2 Control Register  

0x24(0x44) TCNT2 Timer/Counter 2 (8 bit)  

0x23(0x43) OCR2 Output Compare Register Timer/Counter 2  

0x22(0x42) ASSR Asynchronous Status Register  

0x21(0x41) WDTCR Watchdog Timer Control Register  

0x20(0x40) UBRRH UART Baud Rate Register High  

 UCSRC USART Control and Status Register C  

0x1F(0x3F) EEARH EEPROM Address Register high Byte  

0x1E(0x3E) EEARL EEPROM Address Register Low Byte  

0x1D(0x3D) EEDR EEPROM Data Register  

0x1C(0x3C) EECR EEPROM Control Regsiter  

0x1B(0x3B) PORTA Data Register, Port A  

0x1A(0x3A) DDRA Data Direction Register, Port A  

0x19(0x39) PINA Input PIN, Port A  

0x18(0x38) PORT B Data Register, Port B  

0x17(0x37) DDRB Data Direction Register, Port B  

0x16(0x36) PINB Input PIN, Port B  

0x15(0x35) PORTC Data Register, Port C  

0x14(0x34) DDRC Data Direction Register, Port C  

0x13(0x33) PINC Input PIN, Port C  

0x12(0x32) PORTD Data Register, Port D  

0x11(0x31) DDRD Data Direction Register, Port D  

0x10(0x30) PIND Input PIN, Port D  

0x0F(0x2F) SPDR SPI I/O data Register  

0x0E(0x2E) SPSR SPI Status Register  

0x0D(0x2D) SPCR SPI Control Register  

0x0C(0x2C) UDR UART I/O Data register  

0x0B(0x2B) UCSRA USART Control and Status Register A  

0x0A(0x2A) UCSRB USART Control and Status Register B  

0x09(0x29) UBRRL UART Baud Rate Register Low  

0x08(0x28) ACSR Analog Comparator Status Register  

0x07(0x27) ADMUX ADC Multiplexer Selection Register  

0x06(0x26) ADCSRA ADC Control and Status Register A  

0x05(0x25) ADCH ADC Data Register High Byte  

0x04(0x24) ADCL ADC Data Register Low Byte  

0x03(0x23) TWDR Two Wire Serial Interface Data Register  

0x02(0x22) TWAR Two Wire Serial Interface Address Register  

0x01(0x21) TWSR Two Wire Serial Interface Status Register


3.2 Sensor Percepatan (Accelerometer)  

Accelerometer adalah alat yang digunakan untuk mengukur percepatan, mendeteksi dan mengukur getaran (vibrasi), dan mengukur percepatan akibat gravitasi (inklinasi). Sensor accelerometer mengukur percepatan akibat gerakan benda yang melekat padanya.  

Pada pembuatan payload ini digunakan sensor accelerometer MMA7260Q dengan tiga sumbu pengukuran, yaitu terhadap sumbu x, sumbu y, dan sumbu z. Sensor accelerometer ini digunakan untuk mengukur percepatan benda dalam satuan gravitasi (g). Sensor ini dapat mengukur percepatan dari -1,5 g sampai 6 g.


Gambar 7 Break out accelerometer MMA7260Q.  

Pada sensor accelerometer MMA7260Q ini memiliki fasilitas g-select yang memungkinkan sensor bekerja pada tingkat sensitivitas yang berbeda-beda. Penguatan internal pada sensor akan berubah sesuai dengan tingkat sensitivitas yang dipilih, yaitu 1,5 g, 2 g, 4 g, atau 6 g. Sensivitas accelerometer dapat diubah sewaktu-waktu selama accelerometer beroperasi. Pemilihan tingkat sensitivitas ini dilakukan dengan memberikan input logika pada pin g-select1 dan g-select2. Diskripsi pemilihan tingkat sensitivitas pada sensor accelerometer MMA7260Q dapat diamati pada Tabel 2.1. Pilihan-pilihan tingkat sensitivitas ini diperlukan pada aplikasi yang membutuhkan sensitivitas yang berbeda-beda untuk hasil yang optimal. Sensor accelerometer ini memiliki tingkat sensitivitas yang tinggi pada mode g-Range 1,5 g.


Gambar 8 Blok Gambar 8 diagram fungsional accelerometer MMA7260Q.


Selain fasilitas g-select, sensor accelerometer MMA7260Q juga memiliki pengondisi sinyal internal berupa filter LPF orde 1 dan pengompensasi suhu yang memungkinkan sensor masih dapat bekerja sampai lebih dari 100ÂºC. Sensor ini juga memiliki sleep mode sehingga sensor hanya akan mengonsumsi arus yang sangat kecil, yaitu 3 .  

Tabel 3 Diskripsi tingkat sensitivitas accelerometer MMA7260Q.\  

g-select 1 g-select2 g-range sensitivity  

0 0 1.5 800  

0 1 2 600  

1 0 4 300  

1 1 6 200


Sensor accelerometer MMA7260Q beroperasi pada tegangan 2,2 V sampai 3,6 V dengan tegangan tipikal 3,3 V. Sensor ini memiliki tingkat konsumsi arus yang rendah, yaitu 500 .  

Sensor accelerometer MMA7260Q dapat digunakan untuk mengukur baik percepatan positif maupun percepatan negatif. Ketika sensor dalam keadaan diam, keluaran sensor pada sumbu x akan menghasilkan tegangan offset yang besarnya setengah dari tegangan masukan sensor (Vdd). Tegangan offset accelerometer dipengaruhi oleh orientasi sensor dan percepatan statis tiap sumbu akibat gaya gravitasi bumi. Untuk percepatan positif maka sinyal keluaran akan meningkat di atas tegangan offset, sedangkan untuk percepatan negatif sinyal keluaran akan semakin menurun di bawah tegangan offset. Dengan mode g-range 1,5 g, maka tegangan keluaran maksimal sensor dapat dihitung dengan persamaan di bawah ini.  

Vo maks = Voffset + (percepatan maksimal x sensitivitas)  

Accelerometer MMA7260Q memiliki 16 pin yang terdiri dari pin-pin masukan catu daya, pin-pin keluaran, dan pin-pin untuk mengatur sensitivitas sensor. Gambar 2.4 menunjukkan konfigurasi pin-pin pada accelerometer MMA7260Q.


Gambar 9 Konfigurasi pin-pin accelerometer MMA7260Q.  

Pada intinya, accelerometer MMA7260Q ini bekerja menggunakan prinsip konversi kapasitansi (C) ke tegangan. Alat ini terdiri dari dua permukaan sel mesin mikro yang bersifat kapasitif atau disebut juga g-cell. Struktur mekanik g-cell terbuat dari bahan semikonduktor (polysilicon) dan dapat dimodelkan sebagai sepasang sinar yang terpancarkan pada suatu benda yang bergerak diantara dua sumber sinar tetap. Benda tersebut akan bergerak katika ada percepatan. Karena sinar terpancarkan pada benda yang bergerak tadi, maka jarak antara benda dengan sumber sinar tetap pada 1 sisi akan bertambah sejumlah berkurangnya jarak pada sisi yang lain. Hal ini menyebabkan berubahnya nilai masing-masing kapasitor yang dapat dirumuskan pada persamaan di bawah ini.  

 Îµ  

A adalah luas bidang yang dipancari sinar, Îµ merupakan konstanta dielektrik, dan D adalah jarak antara sinar tetap dengan benda yang bergerak. Gambaran sederhana model transduser g-cell dapat dilihat pada Gambar 2.5.


Gambar 10 Model sederhana transduser g-cell.


3.3 Sensor Kompas  

Pada payload roket ini perlu ditambahkan suatu sistem navigasi yang dapat membantu payload roket teresbut untuk mendarat ke tempat awal roket diluncurkan. Sistem navigasi yang cukup baik, efektif, dan mudah digunakan adalah dengan kompas digital. Banyak jenis kompas digital yang diproduksi khusus untuk keperluan navigasi, salah satunya adalah CMPS03 Magnetic Compass buatan Devantech Ltd. CMPS03 yang berukuran 4 x 4 cm ini menggunakan sensor medan magnet Philips KMZ51 yang cukup sensitif untuk mendeteksi medan magnet bumi.


Gambar 11 Sensor Kompas CMPS03


Adapun spesifikasi dari sensor kompas CMPS03 itu adalah sebagai berikut :  

- Catu daya : +5 VDC,  

- Konsumsi arus : 15 mA,  

- Antarmuka : I2C atau PWM.  

Ada dua cara untuk mendapatkan informasi arah dari modul kompas digital ini yaitu dengan membaca sinyal PWM (Pulse Width Modulation) pada pin 4 atau dengan membaca data interface I2C pada pin 2 dan 3.  

3.3.1 PWM Interface  

Sinyal PWM adalah sebuah sinyal yang telah dimodulasi lebar pulsanya. Pada CMPS03, lebar pulsa positif merepresentasikan sudut arah. Lebar pulsa bervariasi antara 1mS (00) sampai 36.99mS (359.90). Dengan kata lain lebar pulsa berubah sebesar 100uS setiap derajatnya. Sinyal akan low selama 65mS diantara pulsa, sehingga total periodanya adalah 65mS + lebar pulsa positif (antara 66mS sampai 102mS). Pulsa tersebut dihasilkan oleh timer 16 bit di dalam prosesornya, yang memberikan resolusi 1uS.  

3.3.2 I2C Interface  

Selain PWM, CMPS03 juga dilengkapi dengan interface I2C yang dapat digunakan untuk membaca data arah dalam bentuk data serial. Pada mode 8 bit, arah utara ditunjukkan dengan data 255 dengan resolusi 1,40625 derajat/bit. Pada mode 16 bit, arah utara ditunjukkan dengan data 65535 sehingga resolusinya menjadi 0,0055 derajat/bit. Pin 2 dan 3 adalah jalur komunikasi I2C dan dapat digunakan untuk membaca data arah (bearing). Jika jalur I2C tidak digunakan, maka pin ini harus di pull up (ke +5V) melalui resistor yang nilainya sekitar 47K, nilai resistor tidak kritikal.


Gambar 12 I2C Communication Protocol


I2C communication protocol dimulai dengan mengirimkan start bit, address modul digital compass dengan read/write low (0xC0), kemudian nomor register yang akan dibaca. Selanjutnya diikuti dengan start bit lagi, address modul digital compass dengan read/write high (0xC1). Selanjutnya anda bisa membaca satu atau dua register (8 bit atau 16 bit). Untuk register 16 bit, yang pertama kali dibaca adalah high byte. CMPS03 memiliki 16 register sesuai tabel berikut :


Tabel 4 Register pada sensor kompas CMPS03


Register 0 adalah Software revision number. Register 1 adalah data arah yang diubah dalam nilai 0-255. Dalam aplikasinya ini lebih mudah dibandingkan nilai 0-360 karena memerlukan dua byte. Untuk resolusi yang lebih tinggi pada Register 2 dan 3 akan menyimpan data arah 16 bit dengan nilai 0-3599. Ini menunjukkan arah 0-359.9Â°. Register 4 sampai 11 adalah internal test register. Register 12, 13 dan 14 tidak digunakan. Register 15 digunakan untuk melakukan kalibrasi kompas. Pin 7 adalah input untuk memilih mode operasi 50Hz (low) atau 60Hz (high). Pilihan ini diperlukan untuk melakukan sinkronisasi dengan frekuensi jala-jala PLN jika digital compass menggunakan power supply yang terhubung ke jala-jala PLN. Hal ini dapat menurunkan deviasi pembacaan data arah. Proses konversi didalam sensor, selesai dalam waktu 40mS (50Hz) atau 33.3mS (60Hz). Pin ini memiliki resistor pullup on board dan dapat dibiarkan tidak terhubung (unconnected) untuk operasi 60Hz. Pin 6 digunakan untuk kalibrasi digital compass. Pin ini memiliki resistor pullup on board dan dapat dibiarkan tidak terhubung setelah proses kalibrasi.  

- Akurasi : 3-4 derajat,  

- Resolusi : 0,1 derajat,  

- Waktu konversi : 40ms atau 33,3ms dapat dipilih,  

- Telah dikalibrasi pada daerah dengan sudut inklinasi 67 derajat. Karena sudut inklinasi yang ada di Indonesia adalah 23,50 maka sensor ini perlu dikalibrasi ulang


3.4 C3088 Camera Module


C3088 adalah modul kamera warna dengan keluaran digital. Modul kamera ini menggunakan sensor gambar cmos ov6620 produk OmniVision. Menggabungkan teknologi CMO dengan mudah menggunakan antarmuka digital membuat C3088 solusi murah untuk video berkualitas lebih tinggi gambar aplikasi. Port video digital memasok gambar 8 / 16 kontinu bit-wide data stream. fungsi kamera, seperti exposure, gamma, gain, white balance, warna matriks, windowing, dapat diprogram melalui antarmuka I2C. 


Gambar 13 C3088 Camera Module


Fitur:  

- 101.376 piksel, CIF / QCIF format  

- Ukuran kecil: 40 x 28 mm  

- Lensa: f = 4.9mm (Opsional)  

- 8 / 16 bit data video: CCIR601, CCIR656, PP port  

- Format data-YCrCb 04:02:02, GRB 04:02:02, RGB  

- Interface I2C  

- Wide dynamic range, anti blooming, zero smearing  

- Electronic exposure / Gain / White balance control  

- Image enhancement - kecerahan, kontras, gamma,  

 saturasi, ketajaman, Window, dll  

- Internal / external synchronization scheme  

- Frame exposure / line exposure option  

- Single 5V operasi  

- Konsumsi daya rendah (<100mW)  

- Monokrom sinyal video komposit output (50 Hz)


Gambar 14 PCB layout (tampak atas)


Deskripsi PIN


1~8 Y0~Y7 Digital output Y Bus.  

9 PWDN Power down mode  

10 RST Reset  

11 SDA I2C Serial data  

12 FODD Odd Field flag  

13 SCL I2C Serial clock input  

14 HREF Horizontal window reference output  

15 AGND Analog Ground  

16 VSYN Vertical Sync output  

17 AGND Analog Ground  

18 PCLK Pixel clock output  

19 EXCLK External Clock input (remove crystal)  

20 VCC Power Supply 5VDC  

21 AGND Analog Ground  

22 VCC Power Supply 5VDC  

23~30 UV0-UV7 Digital output UV bus.  

31 GND Common ground  

32 VTO Video Analog Output (75W monochrome)


3.5 RF Data Transceiver  

 Radio frekuensi YS-1020 UA ini digunakan sebagai perangkat transfer data secara wireless. Perangkat ini menghubungkan antara ground segment dengan instrumentasi payload. Pada mopdul RF ini terdapat 8 pilihan channel frekuensi 433 MHz. Yang digunakan dalam pembuatan payload ini adalah channel sesuai dengan yang ditentukan panitia saat workshop. Dan baudrate yang digunakan adalah 9600bps-8bitData-NonParity-1StopBit.  

1. Fitur  

- Frekuensi pembawa: 433/450/868MHZ.  

- Antarmuka : RS-232/ RS-485/ TTL.  

- Multi-channels : 8 channels, yang bisa diperluasa untuk 16/32 saluran  

- Baud Rate: 1200/2400/4800/9600/19200/38400bps.  

- Transmisi data transparan : Data yang diterima persis dengan yang telah dikirim, cocok untuk setiap standar atau non-standar protokol.  

- Format antarmuka : 8N1/8E1/801  

- Modulasi: GFSK. Berdasarkan pada modulasi GFSK(Gaussian Frequency Shift Keying), modul ini memiliki tingkat anti gangguan tinggi dan nilai error bit yang rendah.  

- Half duplex.  

- Modul ini hanya memerlukan daya yang rendah dan mempunyai fungsi sleep.  

- Bekerja pada suhu: -35Â°C - 75Â°C (-31-167Â°F)  

- Bekerja pada kelembaban 10%-90%  

- Impedansi : 50 Ohm.  

2. Aplikasi  

- Automatic Meter Reading (AMR) dan home automation.  

- Wireless s smart terminal: POS, PDA.  

- Wireless elektronik display screen, LED display.  

- Wireless remote control, pencatat kondisi lingkungan, sistem telemetri.  

- Sistem pengecekan kehadiran, system manajemen dan letak tambang batu bara.  

- Akusisi data wireless, sensor wireless, SCADA.  

3. Spesifikasi  

- Daya RF : â‰¤10mW/ 10 dBm.  

- Arus penerima : <25Ma.  

- Arus pengirim : â‰¤40mA.  

- Arus sleep : <20ÂµA.  

- Power supply: DC 5v atau 3,3v.  

- Sensitivitas penerimaan:  

- 115 dBm (@9600bps)  

- 120 dBm (@1200bps).  

- Ukuran : 47mm x 26mm x 10mm (tanpa port antena)  

- Rentang:  

- â‰¤ 0,5 m (BER=10-3@9600bps, ketika antena berada 2m diatas tanah pada ruangan terbuka)  

- â‰¤ 0,8 m (BER =[10-3@bps](mailto:10-3@bps), ketika antenna berada 2m diatas tanah pada ruangan terbuka)


Gambar 15 Modul Wireless YS-1020UA RF Data Transceiver dan Koneksi Pin nya


Tabel 5 Penjelasan tiap pin  

No. pin Nama Pin Deskripsi Tingkat Hubungan dengan terminal Pengiriman kembali  

1 GND Ground dari power supply Ground  

2 Vcc Power Supply DC +3,3-5,5V  

3 RXD/TTL Serial data receiving end TTL TxD  

4 TXD/TTL Serial data transmitting end TTL RxD  

5 DGND Digital grounding  

6 A (TXD) A dari RS-485 atau TXD dari RS-232 A(RxD)  

7 B (RXD) B dari RS-485 atau RXD dari RS-232 B(TxD)  

8 Sleep Sleep Control TTL Sleep Signal Low level Sleep  

9 Test Ex-factory testing 


 Untuk perancangan penggunaan wireless YS-1020UA dibedakan dalam 2 kondisi, yaitu sebagai tranceiver yang terhubung dengan port mikrokontroller pada perangkat keras sistem dan transeiver yang terhubung dengan komputer melalui USB downloader (TTL). Untuk dapat bekerja transceiver diberi masukan 5V.  

3.5.1 Transfer data wireless YS-1020UA pada payload  

Pada modul wireless YS-1020UA port RX terhubung dengan port.D1 (TX) pada mikrokontroller sedangkan port TX pada wireless terhubung dengan port.D0 (RX) pada mikrokontroller. Untuk penjelasan lengkapnya dapat dilihat pada gambar 3.11.


Gambar 16 Wireless YS-1020UA pada payload.


3.5.2 Transfer data wireless YS-1020UA pada komputer  

 Pada modul wireless YS-1020UA port RX terhubung dengan port TX(TTL) USB downloader sedangkan port TX pada wireless terhubung dengan port RX(TTL) pada USB downloader. Untuk penjelasan lengkapnya dapat dilihat pada gambar 3.12.


Gambar 17 Wireless YS-1020UA pada komputer.  

Dalam mengkomunikasikan data antara sistem payload dengan sistem transmitter-receiver/TX-RX ( pemancar-penerima ) di bumi ( ground ) Segment diperlukan sebuah alat yaitu RF Data Transceiver dimana pada sistem payload ini yang digunakan adalah modul YS 1020UA. Adapun keterangan-keterangan mengenai modul YS 1020UA bisa dilihat di bawah ini : 


IV. Metodologi yang Digunakan  

4.1 Perancangan Hardware  

Perancangan perangkat keras pada plant payload ini meliputi mikrokontroller AVR ATMega 8535, sensor percepatan (Accelorometer), sensor kompas, kamera mikro dan rangkaian catu daya. Secara umum perancangan yang akan dibuat dapat dijelaskan pada gambar diagram blok, Gambar di bawah ini.


 Gambar 18 Blok diagram perancangan sistem


4.1.1 Sistem Transmitter (Payload)  

Pada payload ini RF YS-1020UA ini terhubung dengan sistem minimum ATMega 128. Di payload, tegangan masukan untuk RF YS-1020UA diperoleh dari bateray DC lithium polymer 2 cell yang kemudian masuk ke dalam regulator tegangan 5 volt dengan IC 7805. Dengan sengaja sumber tegangan untuk modul ini di pisah dengan sumber mikrokontroller karena konsumsi daya pada modul ini cukup besar.  

Level tegangan yang digunakan untuk komunikasi serial adalah level TTL. PIN RXD pada RF YS-1020UA dihubungkan dengan PIN TXD/TTL pada mikrokontroller ATMega 8535 yaitu pada PIND.1 dan untuk PIN TXD dihubungkan dengan PIN RXD/TTL pada mikrokontroller ATMega 8535 yaitu pada PIND.0. Alokasi pin untuk RF YS-1020UA pada payload, bisa dilihat pada Gambar dibawah ini.


Gambar 19 Alokasi pin untuk RF YS-1020UA pada mikrokonntroller  

Payload ini merupakan payload yang diberi kemampuan memonitoring kondisi roket, apakah dalam kondisi pitch, yaw atau roll. Selain akuisis data, payload ini juga mempunyai kemampuan untuk mengirimkan data-data yang telah diambil ke sistem penerima yang ada di ground segment. Dan yang terakhir, payload ini diberi kemampuan untuk kembali ke posisi awal roket tersebut diluncurkan dengan bantuan sensor kompas yang telah di pasang di dalamnya.  

Payload ini dikendalikan oleh Mikrokontroller ATMega 8535. Mikrokontroller ini akan mengakses n buah sensor, yaitu sensor percepatan (accelorometer), sensor kompas (CMPS03) dan untuk menangkap gambar permukaan bumi diikut sertakan juga sebuah kamera digital, dalam hal ini kami menggunakan camera module C3088.  

Sensor pertama adalah sensor percepatan (accelerometer). Sensor ini diakses oleh mikrokontroller melalui PINA.0, PINA.2 dan PINA.4. Masukan dari sumbu x dihubungkan dengan PINA.4, sumbu y dengan PINA.2, dan sumbu z dengan PINA.0.  

Kemudian untuk sensor yang kedua yaitu sensor kompas (CMPS03). Sensor ini diakses eleh mikrokontroller menggunakan metode pwm dimana pin yang digunakan adalah PIN PWM (PIN4 yang terdapat dalam sensor kompas) sedangkan pin yang terdapat pada mikrokontroller adalah PINC.0.  

Kemudian C3088 Camera Module, pengambilan data melalu komunikasi I2C yaitu dengan menggunakan pin SCL (Serial clock input) dan SDA(Serial data). Data yang dikirim dalam bentuk data digital dengan format -YCrCb 4:2:2, GRB 4:2:2, RGB pada mikrokontroler akan diubah menjadi Black and White kemudian dikirm ke ground segment.


4.1.2 Sistem Receiver  

 Pada ground segment ini RF YS-1020UA ini terhubung dengan modul USB yang mempunyai kemampuan komunikasi secara serial pada level TTL dengan desktop/laptop . Di ground segment, tegangan masukan diperoleh dari bateray DC 9 volt yang kemudian masuk ke dalam regulator tegangan 5 volt dengan IC 7805. Level tegangan yang digunakan untuk komunikasi serial adalah level TTL. PIN RXD pada RF YS-1020UA dihubungkan dengan PIN TX pada USB-SERIAL DOWNLOADER dan untuk PIN TXD dihubungkan dengan PIN RX. Alokasi pin untuk RF YS-1020UA pada ground segment, bisa dilihat pada Gambar berikut ini.


Gambar 20 Diagram blok plant receiver


4.1.3 Regulator Tegangan  

 Regulator tegangan digunakan untuk menghasikan tegangan 5volt sebagai tegangan masukan pada Radio Frekuensi YS-1020UA (baik di ground segment ataupun di payload).  

4.1.3.1 Regulator Tegangan 5 volt  

 Regulator 5 volt yang digunakan adalah IC regulator 7805. Adapun rangkaian adalah sebagai berikut.


Gambar 21 Rangkaian regulator tegangan 5 volt.  

Tegangan masukan pada regulator (Vin) dapat dibedakan menjadi 2. Pada ground segment menggunakan bateray DC 9 volt sedangkan pada payload menggunakan bateray DC lithium polymer 2 cell.


4.2 Perancangan Software  

4.2.1 Perancangan Listing program  

Perangkat lunak Mikrokontroller AVR dibuat dengan menggunakan Tools CodevisionAVR yang menggunakan bahasa C. Dengan menggunakan berbagai fasilitas yang sangat membantu bagi penguna, dapat langsung dipilih dari fasilitas yang tersedia pada CodeWizardAVR. P ada tab Chip untuk memilih Chip yang digunakan, dalam hal ini kami menggunakan ATMega 8535. Pada tab ADC untuk enable ADC. Pada tab I2C untuk memilih komunikasi I2C. Pada tab USART untuk mengaktifkan komunikasi serial, dalam hal ini digunakan wireless. Berikut gambar CodeWizardAVR. 


Gambar 22 CodeWizardAVR


Software dirancang dengan menggunakan diagram state seperti yang ditunjukkan gambar berikut.


Gambar 23 Diagram State Software Utama


Diagram state di atas menunjukkan bahwa pemrosesan diawali dari inisialisasi â€œStartâ€. Setelah inisialisasi, pemrosesan dilanjutkan menuju state send\_1 yaitu mengambil data pada sensor akselerometer, dilanjutkan dengan pemrosesan dan pengiriman data melalui wireless hingga penghitungan waktu mencapai 12 detik. Kemudian sistem akan bergerak menuju ke state image\_initial, pada state ini untuk memberikan perbedaan data antara akselerometer dengan data yang akan diambil kamera dan kompas. Setelah 6 byte image initial terkirim, pemrosesan akan menuju state capture + homing.  

Pada state ini payload mengambil gambar garis pantai dengan waktu yang tersedia untuk mengambil dan mengirim hingga diterima di ground segment minimal 1 gambar dalam 60 detik dalam waktu yang sama juga kompas akan mengirim data dengan memanfaatkan Byte 206-208. Sehingga pemrosesan akan meminta servo untuk mengarahkan payload ke arah ground segment lalu brushles akan mendorong payload untuk menuju arah yang telah ditunjukan oleh kompas.  

4.2.2 Perancangan User Interface  

 User Interface dibuat dengan bahasa pemrograman Visual C#.  

Perancangan user interface mengacu pada tampilan di bawah ini :


Gambar 24 Tampilan User Interface


Sensor Interface merupakan tampilan yang berisi data mengenai nilai yang dibaca oleh sensor. Pada blok ini ditampilkan  

a. GRAPHIC berisi grafik nilai yang dibaca pada sensor percepatan.yang akan menampilkan 3 axis data percepatan pada koordinat X,Y dan Z  

b. 200X200 PIX SAMPLE PIC, berisi gambar yang ditangkap oleh payload yang berupa gambar black/white dengan resolusi 200x200 pixel  

c. 3D ATTITUDE MONITOR, berisi penampilan kondisi payload dalam bentuk 3 dimensi  

d. NAVIGATION, berisi mengenai informasi sudut payload agar bias homing

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code
- CodeVisionAVR-Standard
