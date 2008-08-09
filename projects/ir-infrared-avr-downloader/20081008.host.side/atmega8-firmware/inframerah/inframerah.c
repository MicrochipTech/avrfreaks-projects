#include "inframerah.h"

uchar bacaDataSerial(void)
{	
	uchar data_terima= 0;
	
	while(counter_buffer == 0);
	
	data_terima= buffer_rx[idx_baca_buffer];
	if(++idx_baca_buffer == PANJANG_BUFFER_RX)
		idx_baca_buffer= 0;
	
	cli();
	--counter_buffer;
	sei();
	
	return data_terima;
}

void inisialisasiProtokol(void)
{
	unsigned int i;
	
	for(i= 0; i < PANJANG_BUFFER_PAKET; i++)
		dataPaket[i]= 0xFF;
	
	// Inisialisasi untuk buffer rx
	idx_tulis_buffer= 0;
	idx_baca_buffer= 0;
	counter_buffer= 0;
	
	// IDPaket mulai dari 1 sampai dengan jumlah paket maksimal
    IDPaket= 0;
	IDProsesTransmisi= 0;
    IDPaketGagal= 1;
	
    // Indek buffer konfirmasi
    indekKonfirmasi= 0;
    
    // Status konfirmasi
    // OK= 1, GAGAL= 0
    statusKonfirmasi= 0;
    tungguKonfirmasi= 0;
	
    statusPaketPerintah= 1;
    statusPaketData= 0;
    statusPaketAkhir= 0;
	
	// Untuk penerimaan transmisi dari sisi target
	mulaiTerima= 0;
	selesaiTerima= 0;
	statusPaketDiterima= KONFIRMASI_IDLE;
	
	statusSisiPemrogram= IDLE;
}

void ambilDataBaru(uchar indeksPaket)
{ 
	unsigned int i, indeksData;
	
	indeksPaket= indeksPaket-1;
	
	// Pengambilan data array mulai dari indeks 0 sampai 127 (1 paket 128 byte data)
	indeksData= (unsigned int) indeksPaket*PANJANG_DATA;
	
	for(i= 0; i < PANJANG_DATA; i++)
		dataLoad[i]= dataPaket[indeksData++];
}

void kirimPaketPerintah(void)
{
	uchar i;
	
	cli();
	
	if(statusSisiPemrogram == KIRIM_PAKET_PERINTAH_TULIS)
	{
		bufferPaketData[0]= '<';
		bufferPaketData[1]= 0x45;
		bufferPaketData[2]= '>';
		bufferPaketData[3]= IDPerintah;
		
		// Informasi jumlah paket yang dikirimkan
		bufferPaketData[4]= '|';
		bufferPaketData[5]= 0xFF;
		bufferPaketData[6]= 0xFF;
		bufferPaketData[7]= jmlPaket;// Jumlah Paket yang akan dikirimkan
		bufferPaketData[8]= '|';
		hasilCRC= hitungCRC16(bufferPaketData, 9);
		bufferPaketData[9]= (uchar) hasilCRC;
		bufferPaketData[10]= (uchar) (hasilCRC>>8);
		
		// Persiapan
		kirimDataSerial(0x00);
		kirimDataSerial(0x00);
		kirimDataSerial(0x00);
		kirimDataSerial(0x00);
		kirimDataSerial(0x00);
		
		for(i= 0; i < 11; i++)
			kirimDataSerial(bufferPaketData[i]);
	}
	else if(statusSisiPemrogram == KIRIM_PAKET_PERINTAH_BACA)
	{
		bufferPaketData[0]= '<';
		bufferPaketData[1]= 0x45;
		bufferPaketData[2]= '>';
		bufferPaketData[3]= IDPerintah;
		
		// Informasi device signature yang dikirimkan
		bufferPaketData[4]= '|';
		// Device signature mikrokontroler yang diinginkan
		bufferPaketData[5]= deviceSignature[0];
		bufferPaketData[6]= deviceSignature[1];
		bufferPaketData[7]= deviceSignature[2];
		bufferPaketData[8]= '|';
		hasilCRC= hitungCRC16(bufferPaketData, 9);
		bufferPaketData[9]= (uchar) hasilCRC;
		bufferPaketData[10]= (uchar) (hasilCRC>>8);
		
		// Persiapan
		kirimDataSerial(0x00);
		kirimDataSerial(0x00);
		kirimDataSerial(0x00);
		kirimDataSerial(0x00);
		kirimDataSerial(0x00);
		
		for(i= 0; i < 11; i++)
			kirimDataSerial(bufferPaketData[i]);
	}
	
	sei();
}

void kirimPaketData(void)
{
	uchar i, j;
	
	//cli();
	
    if(!statusKonfirmasi)
		IDPaket= IDPaketGagal;
    else
		IDPaket++;
	
    ambilDataBaru(IDPaket-(IDProsesTransmisi*JUMLAH_PAKET_PER_BUFFER));
	
    // Header Paket Data
    bufferPaketData[0]= '<';
    bufferPaketData[1]= 0x45;
    bufferPaketData[2]= '>';
    bufferPaketData[3]= IDPerintah; // Berisi informasi ID Perintah
    bufferPaketData[4]= '|';
    bufferPaketData[5]= IDPaket; // Berisi informasi ID Paket atau Indek Paket
    bufferPaketData[6]= '|';
	
    // Data yang dikirim
	for(i= 0; i < PANJANG_DATA; i++)
	{
		j= i+7;
		bufferPaketData[j]= dataLoad[i];
	};
    
    hasilCRC= hitungCRC16(bufferPaketData, PANJANG_DATA+7);
    bufferPaketData[PANJANG_DATA+7]= (uchar) hasilCRC;
    bufferPaketData[PANJANG_DATA+8]= (uchar) (hasilCRC>>8);
	
	// Persiapan
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	
    for(i= 0; i < PANJANG_DATA+9; i++)
        kirimDataSerial(bufferPaketData[i]);
	
	//sei();
}

void kirimPaketAkhir(void)
{
	uchar i;
	
	cli();
	
    bufferPaketData[0]= '<';
    bufferPaketData[1]= 0x45;
    bufferPaketData[2]= '>';
	
    // Informasi penanda paket akhir
    bufferPaketData[3]= '\n';
    bufferPaketData[4]= '|';
    bufferPaketData[5]= '\r';
    bufferPaketData[6]= '|';
    hasilCRC= hitungCRC16(bufferPaketData, 7);
    bufferPaketData[7]= (uchar) hasilCRC;
    bufferPaketData[8]= (uchar) (hasilCRC>>8);
	
	// Persiapan
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	
    for(i= 0; i < 9; i++)
        kirimDataSerial(bufferPaketData[i]);
	
	sei();
}

void olahKonfirmasiPaketPerintah(void)
{
	uchar dataKonfirmasi= 0;
	
	PORTC&= ~(1<<2);
	
	while(tungguKonfirmasi)
	{
		dataKonfirmasi= bacaDataSerial();  
		bufferKonfirmasi[indekKonfirmasi++]= dataKonfirmasi;
		
		if((indekKonfirmasi == 1 && dataKonfirmasi != '|') || (indekKonfirmasi == 3 && dataKonfirmasi != '|'))
			indekKonfirmasi= 0;
		else if(indekKonfirmasi == 4)
		{
			if((bufferKonfirmasi[1] == 0xAA) && (dataKonfirmasi == 0xAA))
			{
				statusKonfirmasi= 1;
				statusPaketPerintah= 0;
				statusPaketData= 1;
				
				if(statusPaketPerintah)
					statusPaketPerintah= 0;
					
				if(statusPaketAkhir)
					statusPaketAkhir= 0;
				
				tungguKonfirmasi= 0;
			}
			else
				tungguKonfirmasi= 0;
			
			indekKonfirmasi= 0;
		}
	};
	
	PORTC|= (1<<2);
}

void olahKonfirmasiPaketAkhir(void)
{
	uchar dataKonfirmasi= 0;
	
	PORTC&= ~(1<<2);
	
	while(tungguKonfirmasi)
	{
		dataKonfirmasi= bacaDataSerial();  
		bufferKonfirmasi[indekKonfirmasi++]= dataKonfirmasi;
		
		if((indekKonfirmasi == 1 && dataKonfirmasi != '|') || (indekKonfirmasi == 3 && dataKonfirmasi != '|'))
			indekKonfirmasi= 0;
		else if(indekKonfirmasi == 4)
		{
			if((bufferKonfirmasi[1] == '\n') && (bufferKonfirmasi[3] == '\r'))
			{
				statusKonfirmasi= 1;
				statusPaketPerintah= 0;
				statusPaketData= 1;
				
				if(statusPaketPerintah)
					statusPaketPerintah= 0;
					
				if(statusPaketAkhir)
					statusPaketAkhir= 0;
				
				tungguKonfirmasi= 0;
			}
			else
				tungguKonfirmasi= 0;
			
			indekKonfirmasi= 0;
		}
	};
	
	PORTC|= (1<<2);
}

void olahKonfirmasiPaketData(void)
{
	uchar dataKonfirmasi= 0;
	
	PORTC&= ~(1<<3);
	
	PORTC|= (1<<1);
	
	while(tungguKonfirmasi)
	{       
		dataKonfirmasi= bacaDataSerial();
		bufferKonfirmasi[indekKonfirmasi++]= dataKonfirmasi;
		
		if((indekKonfirmasi == 1 && dataKonfirmasi != '|') || (indekKonfirmasi == 3 && dataKonfirmasi != '|'))
			indekKonfirmasi= 0;
		else if(indekKonfirmasi == 4)
		{
			if((bufferKonfirmasi[1] == 0xAA) && (bufferKonfirmasi[3] == IDPaket))
			{
				statusKonfirmasi= 1;
				tungguKonfirmasi= 0;
			}
			else
			{
				if(bufferKonfirmasi[1] == 0x55)
				{
					statusKonfirmasi= 0;
					tungguKonfirmasi= 0;
					
					IDPaketGagal= dataKonfirmasi;
				}
				else
				{
					PORTC&= ~(1<<1);
					
					statusKonfirmasi= 0;
					tungguKonfirmasi= 0;
				}
			}
			
			indekKonfirmasi= 0;
		}
	};

	PORTC|= (1<<3);
}

void kirimKonfirmasiIdle(void)
{
	PORTC&= ~(1<<2);
	
	cli();
	
	// Persiapan
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	
	kirimDataSerial('|');
	kirimDataSerial('\n');
	kirimDataSerial('|');
	kirimDataSerial('\r');
	
	sei();
	
	PORTC|= (1<<2);
}

void kirimKonfirmasiOk(void)
{
	PORTC&= ~(1<<3);
	
	cli();
	
	// Persiapan
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	
	kirimDataSerial('|');
	kirimDataSerial(0xAA);
	kirimDataSerial('|');
	kirimDataSerial(0xAA);
	
	sei();
	
	PORTC|= (1<<3);
}

void kirimKonfirmasiGagal(void)
{
	PORTC&= ~(1<<2);
	
	cli();
	
	// Persiapan
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	
	kirimDataSerial('|');
	kirimDataSerial(0x55);
	kirimDataSerial('|');
	kirimDataSerial(IDPaket);
	
	sei();
	
	PORTC|= (1<<2);
}

void kirimKonfirmasiPaketOK(void)
{
	PORTC&= ~(1<<3);
	
	cli();
	
	// Persiapan
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	kirimDataSerial(0x00);
	
	kirimDataSerial('|');
	kirimDataSerial(0xAA);
	kirimDataSerial('|');
	kirimDataSerial(IDPaketOK);
	
	sei();
	
	PORTC|= (1<<3);
}

void awalTransmisi(void)
{
	// Inisialisasi fasilitas USART
	inisialisasiUSART();
	matikanReceiveInterrupt();
	matikanReceiver();
	
	// Pengiriman Paket Awal
	while(statusPaketPerintah)
	{
		kirimPaketPerintah();
		
		tungguKonfirmasi= 1;
		
		aktifkanReceiver();
		aktifkanReceiveInterrupt();
		
		olahKonfirmasiPaketPerintah();
		
		matikanReceiveInterrupt();
		matikanReceiver();
	};
	
	if(statusSisiPemrogram == KIRIM_PAKET_PERINTAH_BACA)
	{
		mulaiTerima= 0;
		selesaiTerima= 0;
		statusPaketDiterima= KONFIRMASI_IDLE;
	}
}

void akhirTransmisi(void)
{
	// Inisialisasi fasilitas USART
	inisialisasiUSART();
	matikanReceiveInterrupt();
	matikanReceiver();
	
	// Inisialisasi untuk buffer rx
	idx_tulis_buffer= 0;
	idx_baca_buffer= 0;
	counter_buffer= 0;
	
	// Pengiriman Paket Akhir
	while(statusPaketAkhir)
    {
		kirimPaketAkhir();
		
		tungguKonfirmasi= 1;
		
		aktifkanReceiver();
		aktifkanReceiveInterrupt();
		
		olahKonfirmasiPaketAkhir();
		
		matikanReceiveInterrupt();
		matikanReceiver();
    };
	
	PORTC= 0x00;
}

void eksekusiTransmisi(void)
{
	// Inisialisasi fasilitas USART
	inisialisasiUSART();
	matikanReceiveInterrupt();
	matikanReceiver();
	
	// Pengiriman Paket Data
    while(statusPaketData)
    {
		kirimPaketData();
		
		tungguKonfirmasi= 1;
		
		aktifkanReceiver();
		aktifkanReceiveInterrupt();
		
		olahKonfirmasiPaketData();
		
		matikanReceiveInterrupt();
		matikanReceiver();
		
		if((IDPaket == JUMLAH_PAKET_PER_BUFFER*(IDProsesTransmisi+1)) && statusKonfirmasi)
			statusPaketData= 0;
		
		if((IDPaket == jmlPaket) && statusKonfirmasi)
		{
			statusPaketData= 0;
			statusPaketAkhir= 1;
			statusSisiPemrogram= KIRIM_PAKET_DATA_SELESAI;
		}
    };
}

void terimaTransmisi(void)
{
	uchar dataDiterima= 0;
	unsigned int crc= 0;
	uchar i= 0;
	
	// Inisialisasi variabel untuk penerimaan paket
	idx_buffer_terima= 0;
	idx_buffer_paket= 0;
	
	for(i= 0; i < PANJANG_DATA; i++)
		dataPaket[i]= 0xFF;
	
	i= 0;
	
	// Inisialisasi fasilitas USART
	inisialisasiUSART();
	
	// Inisialisasi timer timeout
	inisialisasiTimerTimeout();
	
	while(statusSisiPemrogram == TERIMA_TRANSMISI)
	{
		dataDiterima= bacaDataSerial();
		bufferPaketData[idx_buffer_terima++]= dataDiterima;
		
		if((idx_buffer_terima == 1 && dataDiterima != '<') || (idx_buffer_terima == 2 && dataDiterima != 0x46) || (idx_buffer_terima == 3 && dataDiterima != '>'))
			idx_buffer_terima= 0;
		else if((idx_buffer_terima == 5 && dataDiterima != '|') || (idx_buffer_terima == 7 && dataDiterima != '|'))
			idx_buffer_terima= 0;
		else if(idx_buffer_terima == 1)// Tanda mulai paket valid, set ulang timeout
			counterTimeout= TIMEOUT_USART;
		else if(idx_buffer_terima == 9 && mulaiTerima == 0 && selesaiTerima == 0)// Deteksi awal kiriman paket
		{
			crc= hitungCRC16(bufferPaketData, 7);
			
			if(((unsigned char) crc == bufferPaketData[7]) && ((unsigned char) (crc>>8) == bufferPaketData[8]))
			{
				if(bufferPaketData[3] == IDPerintah)
				{
					statusPaketDiterima= KONFIRMASI_OK;
					
					jmlPaket= bufferPaketData[5];
					mulaiTerima= 1;
					IDPaket= 1;
					
					kirimKonfirmasiOk();
				}
			}
			
			idx_buffer_terima= 0;
		}
		else if(idx_buffer_terima == 9 && selesaiTerima == 1)// Deteksi akhir kiriman
		{
			crc= hitungCRC16(bufferPaketData, 7);
			
			if(((unsigned char) crc == bufferPaketData[7]) && ((unsigned char) (crc>>8) == bufferPaketData[8]))
			{
				if((bufferPaketData[3] == '\n') && (bufferPaketData[5] == '\r'))
				{
					// Persiapan
					kirimDataSerial(0x00);
					kirimDataSerial(0x00);
					kirimDataSerial(0x00);
					kirimDataSerial(0x00);
					kirimDataSerial(0x00);
					
					statusPaketDiterima= KONFIRMASI_OK;
					
					kirimKonfirmasiOk();
					
					statusSisiPemrogram= SELESAI_TERIMA_TRANSMISI;
				}
				else
				{
					statusPaketDiterima= KONFIRMASI_GAGAL;
					
					kirimKonfirmasiGagal();
				}
			}
			
			idx_buffer_terima= 0;
		}
		else if(idx_buffer_terima == (PANJANG_DATA+9) && mulaiTerima == 1 && IDPaket <= jmlPaket)// Penerimaan Data
		{
			crc= hitungCRC16(bufferPaketData, PANJANG_DATA+7);
			
			if(((unsigned char) crc == bufferPaketData[PANJANG_DATA+7]) && ((unsigned char) (crc>>8) == bufferPaketData[PANJANG_DATA+8]))
			{
				if(IDPaket == bufferPaketData[5])
				{
					IDPaketOK= IDPaket;
					statusPaketDiterima= KONFIRMASI_PAKET_OK;
					
					for(i= 7; i<= PANJANG_DATA+6; i++)
						dataPaket[idx_buffer_paket++]= bufferPaketData[i];
					IDPaket++;
					kirimKonfirmasiPaketOK();
					
					statusSisiPemrogram= TUNGGU_TERIMA_TRANSMISI;
					
					if(IDPaket == (jmlPaket+1))
					{
						mulaiTerima= 0;
						selesaiTerima= 1;
					}
				}
				else
				{
					statusPaketDiterima= KONFIRMASI_GAGAL;
					
					kirimKonfirmasiGagal();
				}
			}
			else
			{
				statusPaketDiterima= KONFIRMASI_GAGAL;
				
				kirimKonfirmasiGagal();
			}
			
			idx_buffer_terima= 0;
		}
	};
	
	// Matikan timer timeout
	matikanTimerTimeout();
	
	// Matikan fasilitas USART
	matikanReceiveInterrupt();
	matikanReceiver();
}
