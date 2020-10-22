// ******************************************************************************
// *
// *  FBOOT-alternative program loader for 64-bit / 32-bit Windows
// *  Author : Taketoshi Iyota  ( iyota@soka.ac.jp ) in Tokyo, Japan
// *
// ******************************************************************************

#include <windows.h>
#include   <stdio.h>

HANDLE      comPort ;
const char* ComName[10] = { "",     "COM1", "COM2", "COM3", "COM4",
							"COM5", "COM6", "COM7", "COM8", "COM9" } ;

DCB  dcb ;	// Structure that contains the configuration information of the serial port


int  find_comport()
  {
	int  i ;

	for(i=1; i<10; i++)
	  {
		comPort = CreateFileA( ComName[i], GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL) ;
		if(comPort != INVALID_HANDLE_VALUE)
		  {  break ; }
		else
		  {  if(i >= 9)
			   {  i = 0 ;  break ;  }
			 else
			   CloseHandle(comPort) ;
		  }
	  }
	CloseHandle(comPort) ;
	return i ;
  }


int  com_rstat()
  {
	DWORD errors ;
	COMSTAT comStat ;
	int  rvchnbr ;

	ClearCommError(comPort, &errors, &comStat) ;
	rvchnbr = comStat.cbInQue ; 

	return rvchnbr ;
  }


int  com_putch(unsigned char ch)
  {
	DWORD  putchnbr ;

	WriteFile(comPort, &ch, 1, &putchnbr, NULL) ;

	return  putchnbr ;
  }


int  com_getch()
  {
	DWORD  getchnbr ;
	unsigned char rvdata[2] ;

	ReadFile(comPort, rvdata, 1, &getchnbr, NULL) ;

	if(getchnbr < 1)  return -1 ;
	else  return  (int)(rvdata[0]) ;
  }


int  com_putstr(unsigned char* cstrg, int  strchnbr)
  {
	DWORD  putchnbr ;

	WriteFile(comPort, cstrg, strchnbr, &putchnbr, NULL) ;

	return  (int)(putchnbr) ;
  }


int  com_getstr(unsigned char* cstrg, int rvchnbr)
  {
	DWORD  getchnbr ;

	ReadFile(comPort, cstrg, rvchnbr, &getchnbr, NULL) ;

	return  (int)(getchnbr) ;
  }


int  read_hexval(unsigned char *cp, int cnbr)
  {
	int i, ch ;
	int val = 0 ;
	
	for(i=0; i<cnbr; i++)
	  {
		ch = cp[i] ;
		if(ch > '9')  ch = ch - 0x07 ;
		ch = ch - '0' ;
		val = val * 0x0010 + ch ;
	  }

	return  val ;
  }


int  com_getch_wto(int waitms)	// com_getch with time out
  {
	DWORD  getchnbr ;
	unsigned char rvdata[2] ;
	int  i ;

	waitms = waitms / 10 ;
	if(waitms <= 0)  waitms = 1 ;
	
	for(i=0; i<waitms; i++)
	  {
		Sleep(10) ;
		ReadFile(comPort, rvdata, 1, &getchnbr, NULL) ;
		if(getchnbr > 0)  break ;
	  }

	if(getchnbr > 0)  return (int)(rvdata[0]) ;
	else			  return  -1 ;
  }


void main(int argc, char *argv[])
  {
	char  sendstr[5] = { 'P', 'e', 'd', 'a', 0xFF } ;
	int   sendchnbr  = 5 ;

	unsigned char   recvstr[10] ;
	int  i, j, rvchnbr, archnbr ;
	int  compt_nbr ;

	FILE *fp ;
	char *fname ;
	char fstrbuf[80] ;
	unsigned char prog_data[0x10000] ;
	char *sp  ;
	char *cp  ;
	char *cpc ;
	int  rescode, buffsize, flashsize, prog_adrs, rcd_type ;
	int  byte_count, lp_cnt, ck_sum, end_adrs, last_adrs, pdat ;


	if(argc==3)
	  {
	    compt_nbr = (argv[1])[3] - '0' ;
		fname = argv[2] ;
	  }
	else if(argc==2)
	  {
		compt_nbr = find_comport() ;
		fname = argv[1] ;
		if(compt_nbr==0)
		  {
			printf("There is No Existing ComPort !! \n") ;
			return ;
		  }
	  }
	else
	  {
		printf("Usage examples :\n") ;
		printf("> prog_loader COM4 led_blink.hex  // ComPort is COM4, Hex file name is led_blink.hex \n") ;
		printf("> prog_loader led_blink.hex       // Minimum number ComPort of Existing ComPorts is  \n") ;
		printf("                                  // selected automatically, Hex file is the same.   \n") ;
		printf("                                  // ComPort number is from 1 to 9, Not over 9 !     \n") ;
		return ;
	  }


	comPort = CreateFileA(ComName[compt_nbr], GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL) ;

	GetCommState(comPort, &dcb) ;			// read the current setting values

	dcb.BaudRate = 38400 ;					// baud rate
	dcb.ByteSize = 8 ;						// data bit length
	dcb.Parity = NOPARITY ;					// parity
	dcb.StopBits = ONESTOPBIT ; 			// stop bit length
	dcb.fOutxCtsFlow = FALSE ; 				// CTS flow control desable at the time of transmission
	dcb.fRtsControl = RTS_CONTROL_ENABLE ; 	// RTS flow control enable

	SetCommState(comPort, &dcb);			// write the changed setting values

	Sleep(500); 
	
	printf("Waiting for Response *** \n") ;

	for(i=0; i<10; i++)
	  {
		com_putstr(sendstr, sendchnbr) ;
		Sleep(50) ;
		rvchnbr = com_rstat() ;
		archnbr = com_getstr(recvstr, rvchnbr) ;
	  }

	for(i=0; i<40; i++)
	  {
		com_putstr(sendstr, sendchnbr) ;
		Sleep(50) ;
		rvchnbr = com_rstat() ;
		archnbr = com_getstr(recvstr, rvchnbr) ;
		rescode = recvstr[archnbr-1] ;
		if(rescode==0xA6)  break ;
	  }

	if(rescode!=0xA6)
	  {
		printf("Connection Error ! \n") ;
		return ;
	  }


	printf("Connection Valid !! \n") ;

	rvchnbr = com_rstat() ;
	if(rvchnbr > 0)  com_getstr(recvstr, rvchnbr) ;

	sendstr[0] = 0xA5 ;  sendstr[1] = 0xA5 ;
	com_putstr(sendstr, sendchnbr) ;
	Sleep(20) ;
	rvchnbr = com_rstat() ;
	com_getstr(recvstr, rvchnbr) ;
	if(recvstr[0] != 0xAA)
	  {
		printf("Protocol Start Error ! \n") ;
		return ;
	  }

	sendstr[1] = 0x00 ;  sendchnbr = 2 ;
	com_putstr(sendstr, sendchnbr) ;
	Sleep(20) ;
	rvchnbr = com_rstat() ;
	com_getstr(recvstr, rvchnbr) ;
	if(recvstr[0]!=0xA8 || recvstr[1]!=0x03 || recvstr[4]!=0xAA)
	  {
		printf("Version Response Error ! \n") ;
		return ;
	  }
	printf("BootLoader: Ver. %1d.%1d \n", recvstr[2], recvstr[3]) ;

	sendstr[1] = 0x02 ;  sendchnbr = 2 ;
	com_putstr(sendstr, sendchnbr) ;
	Sleep(20) ;
	rvchnbr = com_rstat() ;
	com_getstr(recvstr, rvchnbr) ;
	if(recvstr[0]!=0xA8 || recvstr[1]!=0x04 || recvstr[5]!=0xAA)
	  {
		printf("Signeture Response Error ! \n") ;
		return ;
	  }
	printf("Signeture: %02X%02X%02X \n", recvstr[2], recvstr[3], recvstr[4]) ;

	sendstr[1] = 0x01 ;  sendchnbr = 2 ;
	com_putstr(sendstr, sendchnbr) ;
	Sleep(20) ;
	rvchnbr = com_rstat() ;
	com_getstr(recvstr, rvchnbr) ;
	if(recvstr[0]!=0xA8 || recvstr[1]!=0x03 || recvstr[4]!=0xAA)
	  {
		printf("BufferSize Response Error ! \n") ;
		return ;
	  }
	buffsize = (unsigned int)(recvstr[2]) * 0x0100 + (unsigned int)(recvstr[3]) ;
	printf("BufferSize: 0x%04X \n", buffsize) ;

	sendstr[1] = 0x03 ;  sendchnbr = 2 ;
	com_putstr(sendstr, sendchnbr) ;
	Sleep(20) ;
	rvchnbr = com_rstat() ;
	com_getstr(recvstr, rvchnbr) ;
	if(recvstr[0]!=0xA8 || recvstr[1]!=0x04 || recvstr[5]!=0xAA)
	  {
		printf("Flash MemSize Error ! \n") ;
		return ;
	  }
	flashsize =   (unsigned int)(recvstr[2]) * 0x010000
				+ (unsigned int)(recvstr[3]) * 0x000100
				+ (unsigned int)(recvstr[4]) ;
	printf("Flash MemSize: 0x%06X \n", flashsize) ;


	for(i=0; i<0x10000; i++)
	  prog_data[i] = 0xFF ;

	fp = fopen(fname, "rb") ;
	if(fp==NULL)
	  {
		printf("File Open Error ! \n") ;
		return ;
	  }

	last_adrs = 0x000000 ;

	while(1)
	  {
		sp = fgets(fstrbuf, 78, fp) ;
//		printf("%s", fstrbuf) ;
		if(sp==NULL)   break ;
		if(fstrbuf[0]!=':')
		  {
			printf("Line Read Error ! \n") ;
		  }
		else
		  {
			cp = fstrbuf + 1 ;
			byte_count = read_hexval(cp, 2) ;
			cp = cp + 2 ;
			prog_adrs  = read_hexval(cp, 4) ;
			cp = cp + 4 ;
			rcd_type   = read_hexval(cp, 2) ;
			cp = cp + 2 ;
			
			if(rcd_type==0x01)  break ;
			
			cpc = fstrbuf + 1 ;
			lp_cnt = byte_count + 5 ;
			ck_sum = 0 ;
			
			for(i=0; i<lp_cnt; i++)
			  {
				ck_sum = ck_sum + read_hexval(cpc, 2) ;
				cpc = cpc + 2 ;
			  }
			if((ck_sum & 0x0000FF) != 0)
			  {
				printf("Check Sum Error ! \n") ;
				return ;
			  }

			for(i=0; i<byte_count; i++)
			  {
				prog_data[prog_adrs+i] = read_hexval(cp, 2) ;
				cp = cp + 2 ;
			  }

			end_adrs = prog_adrs + byte_count -1 ;
			if(last_adrs < end_adrs)  last_adrs = end_adrs ;
		  }
	  }

	fclose(fp) ;
	
	printf("Program %s: 00000 - 00000", fname) ;

	i = buffsize ;  prog_adrs = 0x000000 ;
	
	sendstr[1] = 0x04 ;  sendchnbr = 2 ;
	com_putstr(sendstr, sendchnbr) ;
	Sleep(20) ;

	
	while(1)
	  {
		pdat = prog_data[prog_adrs] ;
		if(pdat==0xA5 || pdat==0x13)
		  {
			com_putch(0xA5) ;
			pdat = pdat + 0x80 ;
		  }
		com_putch(pdat) ;
		i-- ;
		
		if(i==0)
		  {
			printf("\b\b\b\b\b%05X", prog_adrs+1) ;
			if((rescode=com_getch_wto(1000)) != 0xA9)
			  {
				printf("\nrescode=%02X: Transfer Failed \n", rescode) ;
				return ;
			  }
			i = buffsize ;
		  }

		if(prog_adrs==last_adrs)
		  {
			com_putch(0xA5) ;   com_putch(0x80) ;
			printf("\b\b\b\b\b%05X", prog_adrs) ;
			if((rescode=com_getch_wto(1000)) == 0xAA)
			  printf("\nTransfer Success ! \n") ;
			else
			  {
				printf("\nrescode=%02X: Transfer Failed \n", rescode) ;
				return ;
			  }
			break ;
		  }

		prog_adrs++ ;
	  }

	sendstr[0] = 0xA5 ;  sendstr[1] = 0x80 ;  sendstr[2] = 0xA5 ;
	sendstr[3] = 0xA5 ;  sendstr[4] = 0xA5 ;  sendstr[5] = 0x05 ;
	sendchnbr = 6 ;
	com_putstr(sendstr, sendchnbr) ;
	Sleep(100) ;
  }

