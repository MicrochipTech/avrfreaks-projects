#include "timer.h"

ISR(TIMER1_COMPA_vect)
{
	// Interrupt setiap 20 milidetik
	
	TCNT1L= 0x00;
	TCNT1H= 0x00;
	
	if(counterTimeout > 0)
		--counterTimeout;
	
	if(counterTimeout == 0)
	{
		counterTimeout= TIMEOUT_USART;
		
		if(statusPaketDiterima == KONFIRMASI_GAGAL)
		{
			idx_buffer_terima= 0;
			kirimKonfirmasiGagal();
		}
		else if(statusPaketDiterima == KONFIRMASI_OK)
			kirimKonfirmasiOk();
		else if(statusPaketDiterima == KONFIRMASI_PAKET_OK)
			kirimKonfirmasiPaketOK();
		else if(statusPaketDiterima == KONFIRMASI_IDLE)
			kirimKonfirmasiIdle();
	}
}

void inisialisasiSinyal38kHz(void)
{
	ASSR= 0x00;
	TCCR2= 0x00;
	TCNT2= 0x00;
	OCR2= 0x9D;
}

void inisialisasiTimerTimeout(void)
{
	TCCR1A= 0x00;
	TCCR1B= 0x04;
	TCNT1H= 0x00;
	TCNT1L= 0x00;
	ICR1H= 0x00;
	ICR1L= 0x00;
	OCR1AH= 0x03;
	OCR1AL= 0xA9;
	OCR1BH= 0x00;
	OCR1BL= 0x00;
	
	TIMSK= 0x10;
	
	counterTimeout= TIMEOUT_USART;
}

void matikanTimerTimeout(void)
{
	TCCR1B= 0x00;
	TIMSK= 0x00;
}
