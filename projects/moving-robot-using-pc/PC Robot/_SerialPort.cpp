#include <stdio.h>
#include <Windows.h>
#include <tchar.h>

void help();
void manual();
int minimum(int,int);
HRESULT initialSerialComm(HANDLE);
HRESULT Serial_sendData(HANDLE ,const char* ,DWORD*);
HRESULT Serial_readData(HANDLE,char*,DWORD*,DWORD*);
char CorrectScanf(void);

int main(){

	const char* portName = (const char*)malloc(sizeof(char));
	
	int flagSerial = 0;
	HANDLE m_hSerialComm;

	while(1){
		printf("Check your serial port first and enter your serial port\n");
		printf(" Example : COM1 \n");
		printf("If your port is bigger then COM10 you have to input in this way\n Example : \\\\\\\\.\\COM10 \n");
		printf("Input your port  :  ");
		scanf("%s",portName);
		printf("Initialization is happening now! Please WAIT for a moment! \n");
		m_hSerialComm = CreateFile(_T(portName), /* Port Name */ 
							   GENERIC_READ | GENERIC_WRITE, /* Desired Access */ 
							   0, /* Shared Mode */
							   NULL, /* Security */
							   OPEN_EXISTING, /* Creation Disposition */
							   0,
							   NULL); /* Non Overlapped */
		if(initialSerialComm(m_hSerialComm) == E_FAIL)
		{
			printf("initialization ERROR or NOT CONNECTED!\n");
			printf("\n");
			flagSerial = 0;
		}else{
			printf("initialization Success!\n");
			flagSerial = 1;
			break;
		}
	}	
	//Receive & Transmit Buffer
	

	DWORD dwCommModemStatus;
	DWORD dwBytesTransferred;
	DWORD bytesWritten;
	
	char receivedData,sendData;
	char tempStop;

	
	int flag = 0;
	int flagManual = 0;
		
	help();
	
	while(1){
		
		DWORD _dwBytesTransferred;
		
		
		char c = CorrectScanf();
		if(c == 'z') break;
		else if(c =='d'){
			if(flagSerial == 1){
				flagManual = 1 - flagManual;
				if(flagManual == 1){ 
					printf("Manual_Control Start\n");
					manual();
				}
				else if(flagManual == 0) printf("Manual Control Stop\n");
			}
			else{
					printf("Serial Communication FAIL! \n");
			}
		}
		
		if(flagManual == 1 && flagSerial == 1){
			char ManualInput = c;
			Serial_sendData(m_hSerialComm,&ManualInput,&bytesWritten);
		}						
	}

	CloseHandle(m_hSerialComm);
	printf("Handle have closed\n");
	printf("Press any for continue to quit\n");
	getchar();
	getchar();
	return 0;
}

void help(){
	printf("\n");
	printf("=================================\n");
	printf("=	'd'	Manual Control	=\n");
	printf("=	'z'	QUIT		=\n");
	printf("=================================\n");
}

void manual(){
	
	printf("\n");
	printf("=========================================\n");
	printf("=	'k'	forward			=\n");
	printf("=	'j'	anti-clockwise rotate	=\n");
	printf("=	'l'	clockwise rotate	=\n");
	printf("=	's' 'm'	stop			=\n");
	printf("=========================================\n");
}


HRESULT Serial_readData(HANDLE m_hSerialComm,char* receivedData,DWORD* dwCommModemStatus,DWORD* dwBytesTransferred){
	HRESULT hResult = S_OK;

	if(!SetCommMask(m_hSerialComm,EV_RXCHAR)){
		printf("SetCommMask Error!\n");
		hResult = E_FAIL;
	}
		
	if(*dwCommModemStatus & EV_RXCHAR){
		if(!ReadFile(m_hSerialComm,receivedData,1,dwBytesTransferred,0)){
			hResult = E_FAIL;
			printf("ReadFile Error\n");
		}	
	}
	return hResult;
}


HRESULT Serial_sendData(HANDLE m_hSerialComm,const char* data,DWORD *bytesWritten){	
	HRESULT hResult = S_OK;
	bool sendChar = WriteFile(m_hSerialComm,data,1,bytesWritten,NULL);
	if(!sendChar){
		printf("WriteFile ERROR\n");
		hResult = E_FAIL;
	}
	return hResult;
}




HRESULT initialSerialComm(HANDLE m_hSerialComm){
	HRESULT hResult = S_OK;
		
	if(m_hSerialComm == INVALID_HANDLE_VALUE)
	{	
		unsigned long error = ::GetLastError();
		hResult = E_FAIL;
		return hResult;
	}
	
		//Modify CommState
		if(!SetupComm(m_hSerialComm, 1024, 1024)){
			hResult = E_FAIL;
			return hResult;
		}

		DCB dcbConfig;
		DWORD dwTimeOutInSec = 5;
		DWORD dwBaudRate = CBR_9600;

		if(GetCommState(m_hSerialComm, &dcbConfig)) /* Configuring Serial Port Settings */
		{
			dcbConfig.BaudRate = dwBaudRate;
			dcbConfig.ByteSize = 8;
			dcbConfig.Parity = NOPARITY;
			dcbConfig.StopBits = ONESTOPBIT;
			dcbConfig.fBinary = TRUE;
			dcbConfig.fParity = TRUE;
		}else{
			hResult = E_FAIL;
			return hResult;
		}

		if(!SetCommState(m_hSerialComm, &dcbConfig)){
			hResult = E_FAIL;
			return hResult;
		}
	
		COMMTIMEOUTS commTimeout;
				
	if(GetCommTimeouts(m_hSerialComm, &commTimeout)) /* Configuring Read & Write Time Outs */
	{
		commTimeout.ReadIntervalTimeout = 1000*dwTimeOutInSec;
		commTimeout.ReadTotalTimeoutConstant = 1000*dwTimeOutInSec;
		commTimeout.ReadTotalTimeoutMultiplier = 0;
		commTimeout.WriteTotalTimeoutConstant = 1000*dwTimeOutInSec;
		commTimeout.WriteTotalTimeoutMultiplier = 0;
	}else{
		hResult = E_FAIL;	
		return hResult;
	}

	if(!SetCommTimeouts(m_hSerialComm, &commTimeout)){
		hResult = E_FAIL;
		return hResult;
	}

	return hResult;
}

char CorrectScanf(void){
	char data = getchar();
	if(data == '\n'){
			data = getchar();
		}
	return data;
}
