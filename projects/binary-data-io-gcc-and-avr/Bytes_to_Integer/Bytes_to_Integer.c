//Bytes_to_Integer/////////////////////////
///////////written by David Storek on 2007-09-27///////
//This code:
//a. waits until the read buffer is full (UART1)
//b. converts four consequtive bytes into an integer
//c. puts a header into TX buffer
//d. cuts the integer into four pieces and adds it into the transmission buffer
//e. transmits the TX buffer through UART0
//
//The following functions assume you have setup UART0 and UART1 for buffered operation
//using the AVRlib functions "buffer" and "uart2"


int main(void)
{
	long int IntegerX=0;
	char IntergerX_a[12];						
	
	uartInit();									// initializing serial port
	uartSetBaudRate(1,115200);					// set baud rate for RX channel
	uartSetBaudRate(0,115200);					// set baud rate for TX channel
	
	//optional wait for buffer to fill up
	WaitForRxBufferFull();						
	//merge 4 bytes from the RX buffer into IntegerX 
	IntegerX = readOneIntegerFromBuffer();   	
	//flush receiver buffer UART1
	bufferFlush(uartGetRxBuffer(1)); 			
	
	//.
	//.
	//.
	//using IntegerX for whatever.
	//For example to convert IntegerX to ASCII
	ltoa(IntegerX, IntergerX_a, 10);
	//.
	//.
	//.

	//put a header in TX Buffer
	uart0AddToTxBuffer('I');
	uart0AddToTxBuffer('M');
	uart0AddToTxBuffer('0');
	uart0AddToTxBuffer('3');
	uart0AddToTxBuffer('D');
	//end of header	
	
	//put IntegerX in TX Buffer, after the header
	WaitForTxBufferEmpty();				//first wait for TX buffer to be empty
	SendOneIntegerToBuffer(IntegerX);
	//send whole TX buffer under interrupt control
	uartSendTxBuffer(1);				

	//or one can elect to directly send IntegerX to the UART, unbuffered
	SendOneIntegerToTx(IntegerX);

	//or one can elect to put a header in TX buffer, copy the entire RX buffer into 
	//the TX buffer and prepare the TX buffer for transmission
	CopyRxBufferToTxBuffer(void);
	uartSendTxBuffer(1);

	return 0;
}



//convert four consequtive bytes into one long integer
long int ReadOneIntegerFromBuffer(void)
{
	unsigned char indata1[4];
											
	indata1[0]=uart1GetByte();					//read LSB from buffer (buffer.h function)
	indata1[1]=uart1GetByte();					//read one character from bubffer
	indata1[2]=uart1GetByte();					//read one character from bubffer
	indata1[3]=uart1GetByte();					//read MSB from buffer
	return *((long int *)indata1);				//make an integer from 4 bytes in array
}

//split up IntegerX into 4 bytes that can be put into transmission buffer
void SendOneIntegerToBuffer(long int IntegerX)
{
	unsigned char *convChar;
	
	convChar = (unsigned char *)&IntegerX;
	uart0AddToTxBuffer(convChar[0]);			//add one byte to TX buffer UART0
	uart0AddToTxBuffer(convChar[1]);
	uart0AddToTxBuffer(convChar[2]);
	uart0AddToTxBuffer(convChar[3]);
}


//split IntegerX into 4 bytes and send unbuffered to UART
void SendOneIntegerToTx(long int IntegerX)
{
	unsigned char *convChar;
	
	convChar = (unsigned char *)&IntegerX;
	uartSendByte(0, convChar[0]);
	uartSendByte(0, convChar[1]);
	uartSendByte(0, convChar[2]);
	uartSendByte(0, convChar[3]);	
}


//Copies the whole RX buffer into the TX buffer, includes a header, and
//prepares the TX buffer for transmission
void CopyRxBufferToTxBuffer(void)	/* read data from AVR1 through UART1 */
{		
	int l;
	char *asd, *qwe;		

	WaitForTxBufferEmpty();						//finished transmitting previous frame?								
				
	asd=(char*)(uartGetRxBuffer(1))->dataptr;	//asd now points to the beginning of the RX buffer
	qwe=(char*)(uartGetTxBuffer(0))->dataptr;	//qwe now points to the beginning of the TX buffer

	//put header "IM03D" in TX Buffer
	*(qwe)=73;
	*(qwe+1)=77;
	*(qwe+2)=48;
	*(qwe+3)=51;
	*(qwe+4)=68;

	//copy RX buffer into TX buffer
	BUSY;
	l=-1;							
	while (++l < UART1_RX_BUFFER_SIZE)	//remember to adjust the buffer size to your needs
		*(qwe+l+5)=*(asd+l);									
	
	//reset TX buffer so that TX send-routine knows where to begin
	uartGetTxBuffer(0)->datalength = UART1_RX_BUFFER_SIZE; //remember to adjust the buffer size to your needs
	uartGetTxBuffer(0)->dataindex = 0;
}


void WaitForRxBufferFull(void)
{
	//we will get stuck until RX-buffer is filled with data
	while (((uartGetRxBuffer(1))->datalength) < UART1_RX_BUFFER_SIZE); //remember to adjust the buffer size to your needs
}


void WaitForTxBufferEmpty(void)
{
	while (uartGetTxBuffer(0)->datalength);		//wait until the transmission is completed
}
