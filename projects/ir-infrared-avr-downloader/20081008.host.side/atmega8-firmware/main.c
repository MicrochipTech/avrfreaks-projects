/* Name: main.c
 * Project: avrirdownloader based on AVR USB Driver
 * Author: Christian Starkjohann (AVR USB Driver), Hendy Eka H and Arief Mardhi B (avrirdownloader)
 * Creation Date: 2007 - 2008
 * Copyright: AVR USB Driver (c) 2005 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt) or proprietary (CommercialLicense.txt)
 * Copyright: avrirdownloader (c) 2008 by UBU-4001 Electrical Engineering Brawijaya University Of Malang, Indonesia
 * License: GNU GPL v2 (see Copying.txt)
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#ifndef F_CPU
	#define F_CPU 12000000UL
#endif

#include "usbdrv.h"
#include "timer.h"
#include "usart.h"
#include "inframerah.h"

// Perintah dari Host
#define CMD_ECHO					0
#define CMD_KIRIM_PERINTAH_TULIS	1
#define CMD_KIRIM_PERINTAH_BACA		2
#define CMD_ISI_BUFFER				3
#define CMD_BACA_BUFFER				4
#define CMD_KIRIM_AKHIR				5
#define CMD_INISIALISASI			6
#define CMD_CEK_STATUS_TRANSMISI	7

// Untuk USBDRV
static uchar bufferBalasanUSB[8];
static uchar jmlDataKirimanUSB;
static unsigned int indeksKirimanUSB;

void enumerateUSB(void)
{
	uchar i, j;
	
	USB_CFG_IOPORT= ~((1<<USB_CFG_DMINUS_BIT) | (1<<USB_CFG_DPLUS_BIT));
	USBDDR= (1<<USB_CFG_DMINUS_BIT) | (1<<USB_CFG_DPLUS_BIT);
	j= 0;
    while(--j)
	{
        i= 0;
        while(--i);
    };
	USBDDR= 0;
	usbInit();
}

void inisialisasi(void)
{
	enumerateUSB();
	
	// Inisialisasi pin dan port
	// Led indikator
	DDRC= 0xFF;
	PORTC= 0xFD;
	// Sinyal 38 kHz
	DDRB|= (1<<3);
	// Tx sebagai output, Rx sebagai input dengan pull up internal Rx USART diaktifkan
	DDRD|= (1<<1);
	PORTD|= (1<<0);
	
	// Inisialisasi fasilitas
	inisialisasiSinyal38kHz();
	aktifkanSinyal38kHz();
	
	inisialisasiUSART();
	matikanReceiveInterrupt();
	matikanReceiver();
	
	// Inisialisasi protokol bersama
	inisialisasiProtokol();
}

USB_PUBLIC uchar usbFunctionWrite(uchar *dataOutUSB, uchar panjangDataOutUSB)
{
    unsigned int i;
	
	if(statusSisiPemrogram == KIRIM_PAKET_PERINTAH_BACA)
	{
		if(panjangDataOutUSB > jmlDataKirimanUSB)
			panjangDataOutUSB= jmlDataKirimanUSB;
		
		jmlDataKirimanUSB-= panjangDataOutUSB;
		
		for(i= 0; i < panjangDataOutUSB; i++)
		{
			if(indeksKirimanUSB < 3)
				deviceSignature[indeksKirimanUSB++]= dataOutUSB[i];
		};
	}
	else
	{
		if(panjangDataOutUSB > jmlDataKirimanUSB)
			panjangDataOutUSB= jmlDataKirimanUSB;
		
		jmlDataKirimanUSB-= panjangDataOutUSB;
		
		for(i= 0; i < panjangDataOutUSB; i++)
		{
			if(indeksKirimanUSB < PANJANG_BUFFER_PAKET)
				dataPaket[indeksKirimanUSB++]= dataOutUSB[i];
		};
		
		if(indeksKirimanUSB >= PANJANG_BUFFER_PAKET)
			statusSisiPemrogram= TUNGGU_PROSES_TRANSMISI;
	}
	
    return jmlDataKirimanUSB == 0;
}


USB_PUBLIC uchar usbFunctionSetup(uchar dataUSB[8])
{
	uchar panjangBalasanUSB= 0;
	
	usbRequest_t *dataRequest= (void *)dataUSB;
	
	if(dataRequest->bRequest == CMD_ECHO)// Cek device, mengembalikan data ke host
	{
		PORTC&= ~(1<<3);
		
		bufferBalasanUSB[0]= dataUSB[2];
        bufferBalasanUSB[1]= dataUSB[3];
		
        panjangBalasanUSB= 2;
		
		PORTC|= (1<<3);
	}
	else if(dataRequest->bRequest == CMD_KIRIM_PERINTAH_TULIS)// Kirimkan paket perintah untuk operasi tulis
	{
		if((statusSisiPemrogram != KIRIM_PAKET_PERINTAH_TULIS) && (statusSisiPemrogram == IDLE))
		{
			statusSisiPemrogram= KIRIM_PAKET_PERINTAH_TULIS;
			
			IDPerintah= dataUSB[2];
			jmlPaket= dataUSB[3];
			
			awalTransmisi();
		}
		else
		{
			indeksKirimanUSB= 0;
			
			bufferBalasanUSB[0]= KIRIM_PAKET_PERINTAH_SELESAI;
			statusSisiPemrogram= PROSES_TRANSMISI;
			
			panjangBalasanUSB= 1;
		}
	}
	else if(dataRequest->bRequest == CMD_KIRIM_PERINTAH_BACA)// Kirimkan paket perintah untuk operasi baca
	{
		if((statusSisiPemrogram != KIRIM_PAKET_PERINTAH_BACA) && (statusSisiPemrogram == IDLE))
		{
			statusSisiPemrogram= KIRIM_PAKET_PERINTAH_BACA;
			
			indeksKirimanUSB= 0;
			jmlDataKirimanUSB= dataRequest->wLength.word;
			
			if(jmlDataKirimanUSB > 3)// Panjang data device signature (dalam byte)
				jmlDataKirimanUSB= 3;
			
			return -1;// Untuk data dari host yang panjang ditangani oleh usbFunctionWrite
		}
		else if(statusSisiPemrogram == KIRIM_PAKET_PERINTAH_BACA)
		{
			IDPerintah= dataUSB[2];
			
			awalTransmisi();
			
			statusSisiPemrogram= KIRIM_PAKET_PERINTAH_SELESAI;
		}
		else
		{
			bufferBalasanUSB[0]= KIRIM_PAKET_PERINTAH_SELESAI;
			statusSisiPemrogram= TERIMA_TRANSMISI;
			
			panjangBalasanUSB= 1;
		}
	}
	else if(dataRequest->bRequest == CMD_ISI_BUFFER)// Isi buffer data ke device, jika penuh lalu tunggu proses kiriman
	{
		if(statusSisiPemrogram == PROSES_TRANSMISI)
		{
			if(dataUSB[2] == 0x01 && dataUSB[3] == 0xFF)// End of file
			{
				// Buffer telah penuh, siap transfer
				statusSisiPemrogram= TUNGGU_PROSES_TRANSMISI;
				bufferBalasanUSB[0]= KIRIM_PAKET_DATA;
				
				panjangBalasanUSB= 1;
			}
			else
			{
				if(indeksKirimanUSB < PANJANG_BUFFER_PAKET)
				{
					jmlDataKirimanUSB= dataRequest->wLength.word;
					
					if(jmlDataKirimanUSB > PANJANG_DATA)// Panjang data maksimal dari host adalah 128 byte
						jmlDataKirimanUSB= PANJANG_DATA;
					
					return -1;// Untuk data dari host yang panjang ditangani oleh usbFunctionWrite
				}
			}
		}
	}
	else if(dataRequest->bRequest == CMD_BACA_BUFFER)// Baca buffer data, kirim ke host
	{
		if(statusSisiPemrogram == TUNGGU_TERIMA_TRANSMISI)
		{
			jmlDataKirimanUSB= PANJANG_DATA;
			
			if(jmlDataKirimanUSB > dataRequest->wLength.word)
				jmlDataKirimanUSB= dataRequest->wLength.word;
			
			usbMsgPtr= dataPaket;
			
			statusSisiPemrogram= TUNGGU_KIRIM_HOST;
			
			return jmlDataKirimanUSB;
		}
	}
	else if(dataRequest->bRequest == CMD_KIRIM_AKHIR)// Kirimkan paket akhir
	{
		if((statusSisiPemrogram != KIRIM_PAKET_AKHIR) && (statusSisiPemrogram == KIRIM_PAKET_DATA_SELESAI))
		{
			statusSisiPemrogram= KIRIM_PAKET_AKHIR;
			akhirTransmisi();
		}
		else
		{
			statusSisiPemrogram= KIRIM_PAKET_AKHIR_SELESAI;
			bufferBalasanUSB[0]= statusSisiPemrogram;
			
			panjangBalasanUSB= 1;
		}
	}
	else if(dataRequest->bRequest == CMD_INISIALISASI)// Re-inisialisasi untuk mengembalikan ke keadaan idle dan siap melakukan proses transmisi
	{
		// Inisialisasi pin dan port
		// Led indikator
		DDRC= 0xFF;
		PORTC= 0xFD;
		// Sinyal 38 kHz
		DDRB|= (1<<3);
		// Tx sebagai output, Rx sebagai input dengan pull up internal Rx USART diaktifkan
		DDRD|= (1<<1);
		PORTD|= (1<<0);
		
		// Inisialisasi fasilitas
		inisialisasiSinyal38kHz();
		aktifkanSinyal38kHz();
		
		inisialisasiUSART();
		matikanReceiveInterrupt();
		matikanReceiver();
		
		inisialisasiProtokol();
		
		statusSisiPemrogram= IDLE;
		
		bufferBalasanUSB[0]= statusSisiPemrogram;
		
        panjangBalasanUSB= 1;
	}
	else if(dataRequest->bRequest == CMD_CEK_STATUS_TRANSMISI)// Untuk data dari host ke device atau sebaliknya > 256 byte (transmisi kontinyu)
	{
		if(statusSisiPemrogram == PROSES_TRANSMISI)
		{
			bufferBalasanUSB[0]= statusSisiPemrogram;
			panjangBalasanUSB= 1;
		}
		else if((statusSisiPemrogram == TUNGGU_TERIMA_TRANSMISI) || (statusSisiPemrogram == SELESAI_TERIMA_TRANSMISI))
		{
			bufferBalasanUSB[0]= statusSisiPemrogram;
			panjangBalasanUSB= 1;
		}
		else if(statusSisiPemrogram == TUNGGU_KIRIM_HOST)
		{
			bufferBalasanUSB[0]= statusSisiPemrogram;
			panjangBalasanUSB= 1;
			
			statusSisiPemrogram= TERIMA_TRANSMISI;
		}
	}
	
	usbMsgPtr= bufferBalasanUSB;
	
	return panjangBalasanUSB;
}

int main(void)
{
	unsigned int m;
	
	inisialisasi();
	
	// Aktifkan global interrupt
	sei();
	
	for(;;)
	{
		usbPoll();
		
		if(statusSisiPemrogram == TUNGGU_PROSES_TRANSMISI)
		{
			PORTC|= (1<<1);
			
			eksekusiTransmisi();
			
			if(IDPaket < jmlPaket)
			{
				indeksKirimanUSB= 0;
				IDProsesTransmisi++;
				IDPaket= JUMLAH_PAKET_PER_BUFFER*IDProsesTransmisi;
				IDPaketGagal= IDPaket+1;
				
				statusPaketData= 1;
				
				for(m= 0; m < PANJANG_BUFFER_PAKET; m++)
					dataPaket[m]= 0xFF;
				
				statusSisiPemrogram= PROSES_TRANSMISI;
			}
		}
		else if(statusSisiPemrogram == TERIMA_TRANSMISI)
		{
			PORTC|= (1<<1);
			
			terimaTransmisi();
			
			PORTC&= ~(1<<1);
		}
	};
	
	return 0;
}
