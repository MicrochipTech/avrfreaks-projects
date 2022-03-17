
//*****************************************************
// File Name  : cashRegisterMainBoard.c
// Title      : ������������ ������� ������ (Mainboard)
// Target MCU : Atmel ATmega32 MCU at 8MHz XTAL
//*****************************************************

 
//----- Include Files ---------------------------------
#include "CashRegisterMainBoard.h"		// 


//----- Begin Code ------------------------------------

int main(void)
{
   
	DDRA = 0X0F;     // PA0-3 output, PA4-7 inputs

	DDRD = 0b11111000; // Make Port D output(PD0=RxD, PD1=TxD)
	PORTD = 0b00000000;
	sbi(DDRC, eeWP);

    
	for (i=0; i<3; i++)
	{
		PORTD |= _BV(Led1);
		timerPause(300000);
	    PORTD &= ~(_BV(Led1));

		PORTD |= _BV(Led2);
		timerPause(300000);
	    PORTD &= ~(_BV(Led2));

		PORTD |= _BV(Led3);
		timerPause(300000);
	    PORTD &= ~(_BV(Led3));

		PORTD |= _BV(Led2);
		timerPause(300000);
	    PORTD &= ~(_BV(Led2));
	}


	// initialize i2c bus and Real Time Clock (RTC).
	i2cInit();
    rtc_init();
	i2cTest();


}


//---------------------------------------------------
void PrintfProgStr_P(const char *data)
{
    unsigned char size = 0;
    
    while (pgm_read_byte(data) != '!')
    {
        // put data into buffer, sends the data
        printerBuffer[size++] = pgm_read_byte(data++);
    }

    i2cMasterSend(PRINTER_1_ADDR, size, printerBuffer);
} 

//---------------------------------------------------
void PrintLogo (void)
{

    if (newReceiption == 1)
    {
        PrintfProgStr_P(Logo1); //���� ��������� �� ��������.
        newReceiption = 0;     // ��� �� ������������.
    }

}

//---------------------------------------------------
void PrintTime (void)
{

    unsigned char highByte;
    unsigned char lowByte;
    unsigned char j = 0;

    if (Price != 0) // �� o Price ��� ����� ������, �������� ��� ��� 
    {

        r = 0;
        i = 0;
        timerPause(20000);
        PrintfProgStr_P(emptyLine); // �������� ��� ��������� 1 ����.
        i2cMasterSend(PRINTER_1_ADDR, 15, printerBuffer); //


        rtc_get_time(rtcBuffer); // ������� ��� ��/��� ��� �� DS1302.

        printerBuffer[r++] = ' ';

		convertRTCbyte2ASCII();
        printerBuffer[r++] = ':';

		convertRTCbyte2ASCII();
        printerBuffer[r++] = ' ';

		i++;

		convertRTCbyte2ASCII();
        printerBuffer[r++] = '/';

		convertRTCbyte2ASCII();
        printerBuffer[r++] = '/';

		i++;

		convertRTCbyte2ASCII();
                                 
    	timerPause(1000000);

		Price = 0; // ����� ��������� �������� ��� Price
    }

    i2cMasterSend(PRINTER_1_ADDR, 15, printerBuffer); //

        timerPause(20000);
        PrintfProgStr_P(scrollTape); // �������� ��������� 2 �����
        printerBuffer[14] = '*';     // ���������� ������ ���������
        i2cMasterSend(PRINTER_1_ADDR, 15, printerBuffer); //
 
}


//==================================================================
//
//==================================================================
void convertRTCbyte2ASCII (void)
{
    unsigned char highByte;
    unsigned char lowByte;

	highByte = rtcBuffer[i];
	highByte >>= 4;
	lowByte = rtcBuffer[i++] & 0b00001111;
	printerBuffer[r++] = (highByte + '0');
	printerBuffer[r++] = (lowByte + '0');

}

//==================================================================
// � �������-������ ��� ����������� � ���� ������ �� ������� ����.
// ������, �� ������ �� �������� ��� ���, �������������� ����� ���
// ���������� (�� 100207 ��� 10/02/2007), ������� �� ������� ��� 
// ����������� ��� ���� �������������� ��� ����� ���
// (�� 055100 ��� 05:51:00) ��� ������� �� ������� ��� ����
//==================================================================
void setTime (unsigned char dateTime)
{
    unsigned char data = 0;
    unsigned char highByte;
    unsigned char lowByte, u = 0;

    CheckForPressedButton();

	if (dateTime == TIME)
	{
		u = 2;
		for (i=0; i<6;)
		{
			highByte = localBuffer[i++];
			highByte <<= 4;
			lowByte = localBuffer[i++];						
			lowByte &= 0b00001111;
			rtcBuffer2[u--] = lowByte + highByte;
		}
	}

	if (dateTime == DATE)
	{
		u = 3;
		for (i=0; i<6;)
		{
			if (u == 5)
				u++;
			highByte = (localBuffer[i++] - '0');
			highByte <<= 4;	
			rtcBuffer2[u++] = (localBuffer[i++] - '0') + highByte;
		}
	}
// ��������� ��� DS1302 ��� ���/���������� ��� ����������� ����
// ������ ������ ��� ������ <rtcBuffer2.
    rtc_comms(0x00, 0x0E, WRITE); // Remove write protection.

    for (i=0; i<7; i++)
    {
        data = rtcBuffer2[i];
        rtc_comms(&data, i, WRITE);
    }

}

//---------------------------------------------------
void i2cTest (void)
{

	timerPause(1000);

	while(1)
	{

        timerPause(1000);
        CheckKeys();
		checkForPower();

    }
}

//=================================================================
// ��������� ��� �������� ��� ������ LED �� ���� 32����� ����������
//=================================================================
long Convert_ASCII2Num ()
{

        NoOfPrintNumbers = localBufferLength;
        unsigned long TempPrice = 0;
        unsigned long TempPrice2 = 1;

        unsigned char Temp = 0;
        CheckForPressedButton ();

        unsigned char DispNumber = localBufferLength;
        localBufferLength--;


       for (;DispNumber > 0 ;DispNumber--)
          {
            Temp = ((localBuffer[localBufferLength--])-'0');
            for (;Temp > 0; Temp--)
                {
                    TempPrice += TempPrice2;
                }
            TempPrice2 *= 10;
          }

// �� �� ��������� ���� ����� ���������� ��� �� �������� ����
// �������� �� ���� �� ��������� � ������� ����.
// ���� �� ���� �� ����� �� ����� (TotalPrice).
            if (TempPrice > Price)
                TotalPrice = TempPrice - Price;
            else TotalPrice = 0;


// �������� ���� �������� ����, ��� ���� ��� ���������� ���������.
            if (command != 2 && temp == 0) 
                Price += TempPrice;  

            localBufferLength = 0;

            return TempPrice;

}


//=================================================================
// ��������� ���� 32������ ���������� �� ������� ���� ����� LED 
//=================================================================
void Convert_Num2ASCII (unsigned long TempPrice)
{

        unsigned char  temp = 0; 
        unsigned char  ByteCounter = 0;
        unsigned char  i;
        unsigned char  firstNumber = 0;
        unsigned long  Comparator = 10000000;
        
        for (i=0; i<8; i++)
        {
            temp = 0;
            if (TempPrice >= Comparator)
            {
                for (;TempPrice >= Comparator; TempPrice -= Comparator)
                {
                    temp++;
                }
                firstNumber = 1;
            }
            if (firstNumber == 1)
            {
                localBuffer[ByteCounter++] = (temp + '0');
            }
            Comparator /= 10;
        }

            i2cMasterSend(LCD_DISP_ADDR, ByteCounter, localBuffer);
            NoOfPrintNumbers2 = ByteCounter;
            NoOfPrintNumbers = ByteCounter;
            
}

//=================================================================
/* � ����������� ��� ��� ���������� ��� ��� �������� �������:

    �� �� �������� ���� Price = 125.00 � 
    ��� �� ��� �����  fpaType = 19%
    �� 125.00 (������ ������ �����) ���� �������������� ����� � 
	������� 12500 (������ �������� ����������)
    � ��������� fpa ������� ��� ���� ��� Price (�������� ����).


    totalFpa = Price - (fpa*100 / fpaType+100) = 
             = 12500 - (12500*100 / 19+100) =
             = 12500 - (1250000 / 119) =
             = 12500 - 10504 =
             = 1996 ���� ����� �� ����������� 19.96 (�).
*/

void Apoforologisi ()
{

	unsigned char crc = 0;
	unsigned char temp = 0;
	unsigned long tempPrice = 0;
	unsigned long pr = 0;	//��������� 4 bytes ��� ������ �� ���� 
	unsigned long treg;     //���� 32bit ���������� (pr).

	tempPrice = PriceA;
	tempPrice *= 100;
	tempPrice /= 109;
	PriceA -=  tempPrice;
	
	Price = tempPrice;

	tempPrice = PriceB;
	tempPrice *= 100;
	tempPrice /= 119;
	PriceB -= tempPrice;

	Price += tempPrice;

	// Price: ������� ��������� ���������� (����� ���).
	// PriceA: ���� ����� ��� ����������� ���� ���������� �� 9% ���.
	// PriceB: ���� ����� ��� ����������� ���� ���������� �� 19% ���.
	
	rtc_get_time(rtcBuffer); // ������� ��� ���������� ��� �� DS1302.
	
	eepromBuffer[2] = rtcBuffer[3]; // �����
	eepromBuffer[3] = rtcBuffer[4]; // �����
	eepromBuffer[4] = rtcBuffer[6]; // ����

	Zcounter = eeprom_read_byte (0); // ������� ����� ����� �
	     // ���������� ��������� ������� � ��� ���� ���������
	Zcounter <<= 8 ;
	Zcounter += eeprom_read_byte (1);
	eepromBuffer[5] = eeprom_read_byte (0);
	eepromBuffer[6] = eeprom_read_byte (1);

	treg = eeprom_read_byte (2); // ������� ���� ���� � ���������
	              // ���� ������ ��� ���������� ������ ��� ��������
	extMemAddress = (treg << 24);
	treg = eeprom_read_byte (3);
	extMemAddress += (treg << 16);
	treg = eeprom_read_byte (4);
	extMemAddress += (treg << 8);
	extMemAddress += eeprom_read_byte (5);
	
	union {		// ���������� ��� 32bit PriceA ���� eeprom 24c256
	long i;
 	char c[4];
	} both2;
	both2.i = PriceA; 
    eepromBuffer[11] = both2.c[3]; // 
	eepromBuffer[12] = both2.c[2]; // 
	eepromBuffer[13] = both2.c[1]; // 
	eepromBuffer[14] = both2.c[0]; // 

	union {		// ���������� ��� 32bit PriceB ���� eeprom 24c256
	long i;
 	char c[4];
	} both3;
	both3.i = PriceB; 
    eepromBuffer[15] = both3.c[3]; // 
	eepromBuffer[16] = both3.c[2]; // 
	eepromBuffer[17] = both3.c[1]; // 
	eepromBuffer[18] = both3.c[0]; // 

	union {		// ���������� ��� 32bit Price ���� eeprom 24c256
	long i;
 	char c[4];
	} both4;
	both4.i = Price; 
    eepromBuffer[23] = both4.c[3]; // 
	eepromBuffer[24] = both4.c[2]; // 
	eepromBuffer[25] = both4.c[1]; // 
	eepromBuffer[26] = both4.c[0]; // 

	crc = CRC8calc(eepromBuffer,31); // ����������� CRC

	eepromBuffer[31] = crc; // ���������� ��� CRC ���� ���������
	                         // ���� ��� ������ eeprom (���� 32�) 

	PORTC &= ~(_BV(eeWP)); // Disable eeprom's write protect 
	i2ceepromWritePage(EXT_EPROM_ADDR, extMemAddress, 32, eepromBuffer);
	 // ����� ��� �� �������� ���� ��������� eeprom.
	
	newReceiption = 1;
	PrintLogo();
	timerPause(1000000);

	timerPause(20000);
    PrintfProgStr_P(emptyLine); // �������� ��� ��������� 1 ����.
    i2cMasterSend(PRINTER_1_ADDR, 15, printerBuffer); //

	PrintfProgStr_P(message3); //������: "������ �����"...
	timerPause(1000000);
	PrintfProgStr_P(message4); //..."���"

	treg = eepromBuffer[23]; // ��������� ��� 4 bytes �� ���� 32bit pr
	pr = (treg << 24);
	treg = eepromBuffer[24];
	pr += (treg << 16);
	treg = eepromBuffer[25];
	pr += (treg << 8);
	pr += eepromBuffer[26];                                                                                       
    Convert_Num2ASCII(pr);        
    addDot(0);      
	timerPause(1000000);

	timerPause(20000);
    PrintfProgStr_P(emptyLine); // �������� ��� ��������� 1 ����.
    i2cMasterSend(PRINTER_1_ADDR, 15, printerBuffer); //

	PrintfProgStr_P(message5); //FPA 9%
	treg = eepromBuffer[11];
	pr = (treg << 24);
	treg = eepromBuffer[12];
	pr += (treg << 16);
	treg = eepromBuffer[13];
	pr += (treg << 8);
	pr += eepromBuffer[14];
    Convert_Num2ASCII(pr);        
    addDot(0);      
	timerPause(1000000);

	timerPause(20000);
    PrintfProgStr_P(emptyLine); // �������� ��� ��������� 1 ����.
    i2cMasterSend(PRINTER_1_ADDR, 15, printerBuffer); //

	PrintfProgStr_P(message6); //FPA 19%
	treg = eepromBuffer[15];
	pr = (treg << 24);
	treg = eepromBuffer[16];
	pr += (treg << 16);
	treg = eepromBuffer[17];
	pr += (treg << 8);
	pr += eepromBuffer[18];
    Convert_Num2ASCII(pr);        
    addDot(0);      

	timerPause(20000);
    PrintfProgStr_P(emptyLine); // �������� ��� ��������� 1 ����.
    i2cMasterSend(PRINTER_1_ADDR, 15, printerBuffer); //

 		r = 0;
        i = 3;

        rtc_get_time(rtcBuffer); // ������� ��� ��/��� ��� �� DS1302.

		temp = '0';				// ��������� ��� 16bit Zcounter
		                              // �� 4 ASCII ����������
		for (;Zcounter >= 1000;)
		{
			Zcounter -= 1000;
			temp++;
		}
		printerBuffer[r++] = temp;

		temp = '0';
		for (;Zcounter >= 100;)
		{
			Zcounter -= 100;
			temp++;
		}
		printerBuffer[r++] = temp;

		temp = '0';
		for (;Zcounter >= 10;)
		{
			Zcounter -= 10;
			temp++;
		}
		printerBuffer[r++] = temp;

		temp = '0';
		for (;Zcounter >= 1;)
		{
			Zcounter--;
			temp++;
		}
		printerBuffer[r++] = temp;
		// ����� ���������� ��� Zcounter �� 4 ASCII ����������.

        printerBuffer[r++] = ' '; // �������� ��� �����������

		convertRTCbyte2ASCII();
        printerBuffer[r++] = '/';

		convertRTCbyte2ASCII();
        printerBuffer[r++] = '/';
		i++;

		convertRTCbyte2ASCII();
		printerBuffer[r++] = ' ';
		printerBuffer[r++] = ' ';
                                 
    	timerPause(1000000);

		i2cMasterSend(PRINTER_1_ADDR, 15, printerBuffer); //

		Zcounter = eeprom_read_byte (0); // ����������� ����� �����
		      // � ���������� ��������� ������� � ��� ���� ���������
		Zcounter << 8 ;
		Zcounter += eeprom_read_byte (1);
		Zcounter++;
		extMemAddress += 32; //�������� ���� 32 bytes ���� ������
		                        // ��������� ��� ���������� eeprom

		union {		// 
		long i;
	 	char c[4];
		} both5;
		both5.i = extMemAddress; 
	    eepromBuffer[2] = both5.c[3]; // 
		eepromBuffer[3] = both5.c[2]; // 
		eepromBuffer[4] = both5.c[1]; // 
		eepromBuffer[5] = both5.c[0]; // 
	
		eeprom_write_byte(2, eepromBuffer[2]);
		eeprom_write_byte(3, eepromBuffer[3]);
		eeprom_write_byte(4, eepromBuffer[4]);
		eeprom_write_byte(5, eepromBuffer[5]);

		extMemAddress = Zcounter;
		extMemAddress >>= 8;
		eeprom_write_byte(0, extMemAddress);
		Zcounter &= 0b11111111;
		eeprom_write_byte(1, Zcounter);

		timerPause(20000);
	    PrintfProgStr_P(emptyLine); // �������� ��� ��������� 1 ����.
	    i2cMasterSend(PRINTER_1_ADDR, 15, printerBuffer); //

		timerPause(1000000);
	    PrintfProgStr_P(message9); // �������� ��������� "������ ��������".
	    i2cMasterSend(PRINTER_1_ADDR, 15, printerBuffer); //

	    timerPause(1000000);
	    PrintfProgStr_P(scrollTape); // �������� ��� ��������� 2 �����.

		timerPause(1000000);
	    printerBuffer[14] = '*';   // ���������� ������ ���������
	    i2cMasterSend(PRINTER_1_ADDR, 15, printerBuffer); //

		newReceiption = 1; // ���� �������� �� ������������� �� ��������.
		Price = 0;
		PriceA = 0;
		PriceB = 0;

}

//====================================================================
// ������� �� ����� ����� �������� ������ �������.
// �� �����, �������� ����� �� ��������� ���� ������ ��������� (OFF).
//====================================================================

void CheckForPressedButton (void)
{
    unsigned char ValueOfPortA; // ��������� ���������.

    do
    {
        PORTA |= (1<<3);        // E������o���� �� buzzer  
        timerPause(100000);
        ValueOfPortA = PINA;    // ������� ��� ����� �
        ValueOfPortA &= 0xF0;   // �������� �� bit ��0-��3
    }
    while (ValueOfPortA > 15);  // ��������� ��� ������� ���
	                            // ���� ���������� � �������

    PORTA &= ~(1<<3);           // ���������o���� �� buzzer
}
//====================================================================
void CheckKeys (void)
{

    unsigned char colSel = 0;
    unsigned char inputs = 0;

    for (colSel=0; colSel<8; colSel++)
    {
        PORTA = colSel;
        timerPause(10000);
        inputs = PINA;
        inputs &= ~(1<<3); 
        if (inputs > 7)
        {   
			switch(inputs)
			{
				case 23: // �������� ����������
				functionSelect = functionLeitourgia;
				break;

				case 39: // ���������� �� �
				functionSelect = functionX;
				break;

				case 71: // ���������� �� �
				functionSelect = functionZ;
				break;

				case 135: // ���������� �� �������������� (��� - ����������)
				functionSelect = functionProg;
				break;

				default:
				Send_disp_number(inputs); // �� ��� ����� ������ ��� ���
				break;  //  ����, ���� �� ��������� ��� ������� ��������

			}
        }
    }
}

//====================================================================
//
//====================================================================

ISR(INT0_vect)
{
	sleep_disable();
	GIMSK = 0b00000000; // Disable INT0 interrupt
		
}
//====================================================================
//
//====================================================================
void checkForPower (void)
{
	unsigned char btnState; 
	unsigned char portState;
	unsigned char pinState; 

	PORTA = 0x00;	//���� ��� ���� ���� ������ �0-�2 ��� 74HC238

	btnState = ~PIND & (1<<PD2);
	if(!btnState)  
	{
		portState = DDRC;  
		pinState = PORTC; // Make backup port C status.

		DDRC = 0XFF;  // Make Port C output.
		PORTC = 0X00;

		GIMSK |= 0b01000000; // Enable INT0 interrupt
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_enable();
		sei();				// Enable interrupts.
		sleep_cpu();		// Go to sleep.
		sleep_disable();
		DDRC = portState;
		PORTC = pinState;

		timerPause(200000);
		i2cMasterSend(LCD_DISP_ADDR, localBufferLength, localBuffer); 
		// ������������ �� ���� ���� ����� ���� ��� ��������� �����������.

	}

}
//====================================================================

void Send_disp_number(int a) 
{   

    unsigned char TypeOfNumber = 0;
    CheckForPressedButton ();

    if (No_of_Pressed_key < 8 )
    {
        //  ������� �� �� 0 ����� �� ����� ������� ��� ��������.
        if (!(No_of_Pressed_key == 0 )); //�� ��� ��� �������� ��� �������
        {
       		switch(a)   // Scan for Number 0-9
            {
                case 129:  // 
                localBuffer[localBufferLength++] = '0';
                break;

                case 64: 
                localBuffer[localBufferLength++] = '1';
                break;

                case 65:  // 
                localBuffer[localBufferLength++] = '2';
                break;

                case 66:  // 
                localBuffer[localBufferLength++] = '3';
                break;

                case 32:  // 
                localBuffer[localBufferLength++] = '4';
                break;

                case 33:  // 
                localBuffer[localBufferLength++] = '5';
                break;

                case 34:  // 
                localBuffer[localBufferLength++] = '6';
                break;

                case 16:  // 
                localBuffer[localBufferLength++] = '7';
                break;

                case 17:  // 
                localBuffer[localBufferLength++] = '8';
                break;

                case 18:  // 
                localBuffer[localBufferLength++] = '9';
                break;

                case 21:  // 
				if(functionSelect == functionProg) // �������� ���
				// �������� ������ ���� ��� � ���������� ���������
				// ����� ���� ���� "����." (���������������).
                	setTime(TIME);    // ���� ���� ������� ��������
					                  // ����/�����������.
                break;

                case 37:  //
				if(functionSelect == functionProg) // �������� ���
				// �������� ������ ���� ��� � ���������� ���������
				// ����� ���� ���� "����." (���������������).
					setTime(DATE);    // ���� ���� ������� ��������
					                  // ����/�����������.
                break;
            }
        }
                No_of_Pressed_key++;
        
    }

       		switch(a) // Scan for command
            {

//----------------------------------
                case 36:  // TM7
                break;
//----------------------------------
                case 128:  // ������� '*'. ��������������� �����
                if (temp == 0)
                {
                    temp = 1;
                    backupPrice = Convert_ASCII2Num();
                }
                break;

//----------------------------------
                case 130:  // 
                localBuffer[0] = 'x';  // Send <Clear display> command.
                break;
//----------------------------------
                case 133:  // 
	    		timerPause(10000);
	    		PrintfProgStr_P(scrollTape); // �������� ��������� 2 �����
                break;
//----------------------------------
                case 131: // ����������� �� ��� 9% (����� 1)
                PrintLogo();
                multiply();
                command = 0;
                PriceA += Convert_ASCII2Num(); //PriceA;
                addDot(9);

                No_of_Pressed_key = 0;

                Convert_Num2ASCII(Price);
                break;
//----------------------------------
                case 67: // ����������� �� ��� 19% (����� 2) 
                PrintLogo();
                multiply();
                command = 0;
                PriceB += Convert_ASCII2Num(); //PriceA;
                addDot(19);

                No_of_Pressed_key = 0;

                Convert_Num2ASCII(Price);
                break;
//----------------------------------
				case 69: // ������� ����������� ��� ����������� PriceA
				         // ��� PriceB ���� ��������� EEPROM.
				if(functionSelect == functionZ) // �������� ��� ��������
				// ������ ���� ��� � ���������� ��������� ����� ���� ���� �
                	Apoforologisi();   				
				break;
//----------------------------------
// ������� ��� �� ����� (������� <�������>).

                case 70:
                if (Price != 0)
                    command = 2;
                No_of_Pressed_key = 0;
                localBufferLength = 0;
                a = 100;
                break;

//----------------------------------
// ������� �������� (������� <�������>).

                case 134: 
                if (Price != 0) // �� o Price ��� ����� ������ �������� ��� ������� 
                {
                    PrintfProgStr_P(message1); // ������       
                    backupPrice = Convert_ASCII2Num(); // ���� ���������
					                           // �� ���� ��� �������� ��������.                                                                                    
                    Convert_Num2ASCII(Price);        
                    addDot(0);     
                    i2cMasterSend(PRINTER_1_ADDR, 15, printerBuffer); //  

                    if (command == 2)
                    {
                        if (TotalPrice != 0)
                        {   
                            timerPause(20000);
                            PrintfProgStr_P(message2); // �������
                            Convert_Num2ASCII(backupPrice); // �������� �� ����
                            addDot(0);     

                            timerPause(20000);
                            PrintfProgStr_P(message7); // �����
                            Convert_Num2ASCII(TotalPrice);
                            addDot(0);     

                            a = 100;         
                            PrintTime();
                            Price = 0;
                        }
                        command = 0;                     
                    }
                    
                    newReceiption = 1; // ���� �������� �� ������������� ��������
                    a = 100;
                    PrintTime();
                    Price = 0;
                    No_of_Pressed_key = 0;
                    localBufferLength = 0;

                }
                break;

//----------------------------------
                default:
                break;
            }
 
 
            if (localBuffer[0] == 'x')
            {
                No_of_Pressed_key = 0;
                localBufferLength = 1;
                i2cMasterSend(LCD_DISP_ADDR, localBufferLength--, localBuffer);
            }

            else if (a != 100)
            {
                i2cMasterSend(LCD_DISP_ADDR, localBufferLength, localBuffer);
                a = 0;
            }
           
}

void multiply (void)
/* �������������� �� ���� ��� �������� �� ���� ���� ������.
   ����� ��� ��� ��������� ��� ������� �� �������������
    ��� ����������, 5 ���� ��������. */
{
    if (temp == 1)
    {
        backupPrice *= Convert_ASCII2Num();
        Convert_Num2ASCII(backupPrice);
        temp = 0;
        localBufferLength = NoOfPrintNumbers;
    }
}


//====================================================================
//
//====================================================================
void addDot (unsigned char percent)
{

    unsigned char temp = 1;

    if (NoOfPrintNumbers>0)
    {
        if (NoOfPrintNumbers>1)
        {
            temp = ++NoOfPrintNumbers;
            localBuffer[NoOfPrintNumbers] = localBuffer[NoOfPrintNumbers - 1];
            localBuffer[--NoOfPrintNumbers] = localBuffer[NoOfPrintNumbers - 1];
            localBuffer[--NoOfPrintNumbers] = localBuffer[NoOfPrintNumbers - 1];
            localBuffer[--NoOfPrintNumbers] = ',';
        }

        for(r=0; r<15; r++)
            printerBuffer[r] = ' '; // ����� ��� ��� buffer ��������.

        NoOfPrintNumbers = temp;
        r = 11 - NoOfPrintNumbers;

        for(i=0; i<r; i++)
            printerBuffer[i] = ' ';      

        if (r == 10)
        {
            printerBuffer[7] = '0';
            printerBuffer[8] = ',';
            printerBuffer[9] = '0';
        }
        if (r == 9)
        {
            printerBuffer[7] = '0';
            printerBuffer[8] = ',';
        }
        if (r == 8)
        {
            printerBuffer[7] = '0';
        }              

        r = 0;
        for(; i<11; i++)
        {
            printerBuffer[i] = localBuffer[r++];                           
        }

        if (percent == 9)
        {
            printerBuffer[11]=' ';
            printerBuffer[12]=' ';
            printerBuffer[13]='9';
            printerBuffer[14]='%';
        }
        else if (percent == 19)
        {
            printerBuffer[11]=' ';
            printerBuffer[12]='1';
            printerBuffer[13]='9';
            printerBuffer[14]='%';
        }

        i2cMasterSend(PRINTER_1_ADDR, 15, printerBuffer);
    }
}
/*----------------------------------------------------------------
--------FUNCTION TO GENERATE DELAY IN MILLISECONDS----------------
-----------------------------------------------------------------*/

void timerPause(u32 count) // ����������� 2.5 ms
{
    while(count--);
}

