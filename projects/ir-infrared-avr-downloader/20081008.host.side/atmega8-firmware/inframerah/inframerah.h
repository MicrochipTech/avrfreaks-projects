#ifndef __inframerah_h_included__
#define __inframerah_h_included__

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "timer.h"
#include "usart.h"
#include "crc.h"

#ifndef uchar
	#define uchar						unsigned char
#endif

#define KIRIM_PAKET_PERINTAH_TULIS		1
#define KIRIM_PAKET_PERINTAH_BACA		2
#define KIRIM_PAKET_PERINTAH_SELESAI	3
#define ISI_BUFFER						4
#define ISI_BUFFER_SELESAI				5
#define KIRIM_PAKET_DATA				6
#define KIRIM_PAKET_DATA_SELESAI		7
#define KIRIM_PAKET_AKHIR				8
#define KIRIM_PAKET_AKHIR_SELESAI		9
#define PROSES_TRANSMISI				10
#define TUNGGU_PROSES_TRANSMISI			11
#define TERIMA_TRANSMISI				12
#define TUNGGU_TERIMA_TRANSMISI			13
#define TUNGGU_KIRIM_HOST				14
#define SELESAI_TERIMA_TRANSMISI		15
#define IDLE							16

#define KONFIRMASI_OK					1
#define KONFIRMASI_GAGAL				2
#define KONFIRMASI_PAKET_OK				3
#define KONFIRMASI_IDLE					4

#define PANJANG_DATA					128
#define PANJANG_BUFFER_KONFIRMASI		4
#define PANJANG_BUFFER_PAKET			256
#define JUMLAH_PAKET_PER_BUFFER			PANJANG_BUFFER_PAKET/PANJANG_DATA

uchar statusSisiPemrogram;
uchar IDPerintah;
uchar deviceSignature[3];

unsigned int hasilCRC;
uchar indekKonfirmasi;
uchar IDPaket, IDPaketOK;
uchar idx_buffer_paket;
uchar IDProsesTransmisi;
uchar IDPaketGagal;
uchar jmlPaket;

uchar statusKonfirmasi;
uchar statusPaketPerintah;
uchar statusPaketData;
uchar statusPaketAkhir;
uchar tungguKonfirmasi;

uchar dataLoad[PANJANG_DATA];
uchar bufferPaketData[PANJANG_DATA+10];
uchar bufferKonfirmasi[PANJANG_BUFFER_KONFIRMASI];
uchar dataPaket[PANJANG_BUFFER_PAKET];

volatile uchar statusPaketDiterima;
uchar mulaiTerima, selesaiTerima;

uchar bacaDataSerial(void);

void inisialisasiProtokol(void);

void ambilDataBaru(uchar indeksPaket);
void kirimPaketPerintah(void);
void kirimPaketData(void);
void kirimPaketAkhir(void);
//void olahKonfirmasiPaketPerintahDanAkhir(void);
void olahKonfirmasiPaketPerintah(void);
void olahKonfirmasiPaketAkhir(void);
void olahKonfirmasiPaketData(void);

void kirimKonfirmasiIdle(void);
void kirimKonfirmasiOk(void);
void kirimKonfirmasiGagal(void);
void kirimKonfirmasiPaketOK(void);

void awalTransmisi(void);
void akhirTransmisi(void);
void eksekusiTransmisi(void);

void terimaTransmisi(void);

#endif
