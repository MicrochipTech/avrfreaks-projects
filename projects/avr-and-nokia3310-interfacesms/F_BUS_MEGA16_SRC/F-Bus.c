#include "F-Bus.h"
#include "timer.h"

unsigned char InitFrames[] ={
	0x1E, 0x00, 0x0C, 0x40, 0x00, 0x06, 0x00, 0x01, 0x64, 0x01, 0x01, 0x41
	, 0x1E, 0x00, 0x0C, 0x40, 0x00, 0x05, 0x00, 0x01, 0x66, 0x01, 0x42, 0x00
	, 0x1E, 0x00, 0x0C, 0x40, 0x00, 0x06, 0x00, 0x01, 0xCA, 0x01, 0x01, 0x43
	, 0x1E, 0x00, 0x0C, 0x40, 0x00, 0x06, 0x00, 0x01, 0xC8, 0x05, 0x01, 0x44
	, 0x1E, 0x00, 0x0C, 0x03, 0x00, 0x07, 0x00, 0x01, 0x00, 0x07, 0x02, 0x01
	, 0x1E, 0x00, 0x0C, 0x03, 0x00, 0x07, 0x00, 0x01, 0x00, 0x07, 0x03, 0x01
	, 0x1E, 0x00, 0x0C, 0x04, 0x00, 0x06, 0x00, 0x01, 0x00, 0x01, 0x01, 0x47};
	unsigned char LedStatus[] = { OFF ,OFF ,OFF ,OFF }; 
	char MyString[60];
	unsigned char ACKseq = 0x02,SeqNo=0x41 /* 0x41 */
		,CMD=0xD2 , seqIndex = 0 , ACKrecieved = NO;

	unsigned char SMSC[] = {0x08, 0x91, 0x69, 0x74, 0x07, 0x11, 0x44, 0x11, 0xF0, 0x00, 0x00, 0x00};
	unsigned char RecipientNo[] ={0x0d ,0x91 ,0x69 , 0x74 , 0x07 , 0x73 , 0x21 ,0x36 , 0xF5};
	unsigned char FBusFrame[200],decodedSMS[100],index ,InitMode=1,InitFrameIndex=0;
	unsigned char SendAns = NO ;
	unsigned char ch[6],chr=0,ReciveMode=0,size=0,ptr=0,eePtr=0 , PinNumber;
	extern unsigned char _digit0,_digit1,_digit2,_digit3;
	ISR(SIG_USART_RECV)
	{
		chr=UDR; //read the recived char
		if(ReciveMode == 0)
		{
			ch[5]=ch[4];
			ch[4]=ch[3];
			ch[3]=ch[2];
			ch[2]=ch[1];
			ch[1]=ch[0];
			ch[0]=chr;
			// check whether to start recieve the SMS header
			if ((ch[1] == 0x00) && (ch[3] == 0x00) && (ch[4] == 0x0c) && (ch[5] == 0x1e))
			{
				CMD = ch [2];
				if ((ch[0] & 0x01))//odd size then add 1 + two checksum bytes to size(padding)
				{
					size = ch[0]+2;
					seqIndex = size - 3;
				}
				else
				{
					size = ch[0]+1;//even
					seqIndex = size - 2;
				}
				ptr=0;
				ReciveMode =1;
			}
		}
		else
		{
			FBusFrame[ptr]=chr;
			if(ptr++==(size))
			{
				//printf("%d Bytes Recieved\n",size);
				ReciveMode = 0;
				ACKseq = FBusFrame[seqIndex] & 0x07;
				if(CMD != 0x7F)
					sendACK();
				else
					ACKrecieved = YES;
				if(CMD == 0x02) // sms
				{
					//printf("Decoding SMS....\n");
					decodeSMS (FBusFrame,decodedSMS ,size-3);
				}
			}
		}
	}

	unsigned char decodeSMS(unsigned char *src,unsigned char *out,unsigned char size)
	{
		//Get the SMSC (SMS Center) number

		// Get 
		for(unsigned char h =0;h<9 ; h++)
		{
			RecipientNo [h] = src [h + 23];
		}
		memset(decodedSMS , 0 , sizeof(decodedSMS) );
		memset(MyString , 0 , sizeof(MyString) );
		unsigned char c=0,w=0,index=0,i=1,j=1;
		unsigned char Mask[] = { 0x7F , 0x3F , 0x1F , 0x0F , 0x07 , 0x03 , 0x01 , 0x00 };
		out[0] = src[0 + SMSstart] & 0x7F;
		while( 1)
		{
			c = src[i + index + SMSstart] & Mask[i];
			c <<= i;
			w = src[i + index -1 + SMSstart];
			w >>= (8 - i);
			out[j] = c | w;
			//putchar(out[j]);
			i++;
			j++;
			if(i==8)
			{
				i = 0;
				index += 7;
			}
			if((index + i + SMSstart) >= size)
				goto exit;
		}
exit:
		//printf ("%d bytes extracted from %d bytes\r\n\n", j , size-48);
		//for(unsigned char K=0;K<j;K++)
		//{
		//	__EEPUT (K,out[K]);
		//}
		//eePtr += (j + 15);
		//putstream(out,j);
		ParseSMS(out);
		//addChkSumAndSend(sms2send);
		return j;
	}

	unsigned char sendACK(void)
	{
		unsigned char ACK[11];
		ACK[0] = 0x1E ;
		ACK[1] = 0x00 ;
		ACK[2] = 0x0C ;
		ACK[3] = 0x7F ;
		ACK[4] = 0x00 ;
		ACK[5] = 0x02 ;
		ACK[6] = CMD  ;
		ACK[7] = ACKseq ;
		unsigned char oddCeckSum = 0,evenCheckSum = 0;
		//printf ("\nThe size of the frame is %d bytes\n\n",size);
		for (unsigned char i =0; i < 8 ; i += 2)
		{
			oddCeckSum ^= ACK [i];
			evenCheckSum ^= ACK [i+1];
		}
		ACK [8] = oddCeckSum;
		ACK [9] = evenCheckSum;
		for (unsigned char j=0 ; j<10 ; j++)
		{
			putchar (ACK [j]);
		}
		if(CMD == 0x02)
			InitMode = 0;//No more Initialization frames
		if(InitMode==1)
			SendNextFrame();
	}

	void addChkSumAndSend(unsigned char *Frame,char con)
	{ 
		if (con == '-')
			memset(FBusFrame ,0,sizeof(FBusFrame));
		unsigned char __isodd =0;//even by default
		unsigned char oddCeckSum = 0,evenCheckSum = 0;
		unsigned char FrmSize = Frame [ 5 ];
		// if FrmSize is odd 
		if (FrmSize & 0x01)
		{
			FrmSize += 6;
			__isodd = 1;
		}
		else
			FrmSize += 5;
		if(con=='-')
			memcpy(FBusFrame,Frame,FrmSize);
		if(__isodd)
		{
			FBusFrame [FrmSize-1] = SeqNo;
			FBusFrame [FrmSize] = 0;
		}
		else
		{
			FBusFrame [FrmSize-1] = SeqNo;
		}
		for (unsigned char i =0; i < FrmSize ; i += 2)
		{
			oddCeckSum ^= FBusFrame [i];
			evenCheckSum ^= FBusFrame [i+1];
		}
		SeqNo++;
		SeqNo &= ~(1<<3);

		FBusFrame [FrmSize] = oddCeckSum;
		FBusFrame [FrmSize +1] = evenCheckSum;
		for (unsigned char j=0 ; j<(FrmSize+2) ; j++)
		{
			putchar (FBusFrame [j]);
		}
	}

	unsigned char initPhone(void)
	{
		InitFrameIndex = 0;
		InitMode = YES;
		SeqNo = 0x41;
		ACKseq = 0x02;
		unsigned char i=0 ;
		unsigned char GetSwHwCmd[] ={0x1E, 0x00, 0x0C, 0xD1, 0x00, 0x07, 0x00, 0x01, 
			0x00, 0x03, 0x00, 0x01, 0x60, 0x00, 0x72, 0xD5};
		//send 'U' (0x55) 128 times
		for(i=0 ; i<128 ; i++)
		{
			putchar(0x55);
		}
		//delay some time
		_delay_ms (50);
		//send 'Get SW&HW command'
		for(i=0 ; i<sizeof(GetSwHwCmd) ; i++)
		{
			putchar(GetSwHwCmd[i]);
		}
		sei();
	}

	unsigned char SendSMS(const char *Message,unsigned char *PhoneNumber)
	{
		//clear buffer
		unsigned char j=0;
		memset(FBusFrame,0,sizeof(FBusFrame));
		//Compress the string

		unsigned char MsgLen = strlen(Message) , FrameSize=0;
		unsigned char c , w , n , shift=0 , frameIndex=0;

		/* Decode into 7 bit characters */
		for (n=0; n<MsgLen; n++)
		{
			c = Message[n] & 0x7f;
			c >>= shift;
			w = Message[n+1] & 0x7f;
			w <<= (7-shift);
			shift +=1;
			c = c | w;
			if (shift == 7)
			{
				shift = 0x00;
				n++;
			}
			FBusFrame[frameIndex + MsgStartIndex] = c;
			frameIndex++ ;
		}
		FBusFrame[frameIndex + MsgStartIndex] = 0x01;
		//The size of the frame is frameIndex+48 !!!!
		FrameSize = frameIndex + 44;// frameIndex + 48 + 1 - 5
		FBusFrame[0]  = 0x1E ;
		FBusFrame[1]  = 0x00 ;
		FBusFrame[2]  = 0x0C ;
		FBusFrame[3]  = 0x02 ;
		FBusFrame[4]  = 0x00 ;
		FBusFrame[5]  = FrameSize ;
		FBusFrame[6]  = 0x00 ;
		FBusFrame[7]  = 0x01 ;
		FBusFrame[8]  = 0x00 ;
		FBusFrame[9]  = 0x01 ;
		FBusFrame[10] = 0x02 ;
		FBusFrame[11] = 0x00 ;
		// copy the SMSC number
		for (j=0 ; j<sizeof(SMSC) ;j++)
			FBusFrame[12 + j] =SMSC[j] ;
		FBusFrame[24] = 0x35 ; //Message type??
		FBusFrame[28] = MsgLen ; //Message length (uncompressed)
		// copy the Recipient number
		for (j=0 ; j<sizeof(RecipientNo) ;j++)
			FBusFrame[j + 29] = RecipientNo[j] ;
		FBusFrame[41] = 0xA7 ; // Validity period
		frameIndex ++; //advance pointer
		addChkSumAndSend(FBusFrame,'+'/* '+' means Don't clear FBusFrame[]*/);
	}

	void SendNextFrame(void)
	{
		//_delay_ms (1);
		addChkSumAndSend(&InitFrames[InitFrameIndex],'-');
		InitFrameIndex += 12;
		if(InitFrameIndex == 84)
		{
			InitMode = 0;
			_delay_ms (500);
			if (SendAns)/*wether to reply or not ?*/
			{
				SendSMS(MyString,RecipientNo);
				SendAns = NO ;
			}
		}

	}

	void ParseSMS(unsigned char *msg)
	{
		_makelower (msg);
		memset(MyString,0,sizeof(MyString));
		if(strncmp ("led ",msg,4)== 0)
		{
			PinNumber = msg[4] - '0' ;
			if((PinNumber > 0) && (PinNumber < 3))
			{
				// check the command is ON or OFF
				if(strncmp (" on",msg+5,3)== 0)
				{
					/*Turn on*/
					if (LedStatus[PinNumber - 1] == OFF )
					{
						PORTC &= ~(1 << (PinNumber-1));
						LedStatus[PinNumber - 1] = ON;
						sprintf(MyString,"LED %d is turned on!!!.\n",PinNumber);
						__start_timout ();
					}
					else
					{
						sprintf(MyString,"LED %d is already turned on\n",PinNumber);
						__start_timout ();
					}
				}
				else if(strncmp (" off",msg+5,4)== 0)
				{
					/*Turn off*/
					if (LedStatus[PinNumber - 1] == ON )
					{
						PORTC |= (1 << (PinNumber-1));
						LedStatus[PinNumber - 1] = OFF;
						sprintf(MyString,"LED %d is turned off!!!.\n",PinNumber);
						__start_timout ();
					}
					else
					{
						sprintf(MyString,"LED %d is already turned off\n",PinNumber);
						__start_timout ();
					}
				}
			}
		}
		else if(strncmp ("send temp.",msg,10) == 0)
		{
			sprintf(MyString,"The temperature is now %d%d.%d °C.",_digit3,_digit2,_digit1);
			__start_timout ();		
		}
	}

	void _makelower (unsigned char *src)/* Convert string to lowercase*/
	{
		unsigned char i =0;
		while(src[i])
		{
			_EEPUT(i,src[i]);
			if (isupper(src[i]))
				src[i] = tolower(src[i]);
			i++;
		}
	}
