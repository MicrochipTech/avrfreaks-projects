/****************************************************************************
  Описание основных структур программы
****************************************************************************/

struct ETH_status
{	
	unsigned char	phase;			// состояние работы с модулем ethernet
	unsigned char	initRequest;	// запрос инициализации утановок модуля ethernet 1-FLASH, 2-USB, 3-USER IP destination
};

struct Request
{	
	unsigned char		req;		// признак запроса передачи/приема пакета к модулю ethernet
	unsigned char		leng;		// длина запрошенного к передаче пакета
};

//вспомогательная структура для передачи данных модулю WIZnet
struct TD
{
	unsigned char* pIData;	//адресный указатель памяти внутри модуля WIZnet
	unsigned char* pEData;	//адресный указатель памяти процессора
	unsigned char lengs;		//длина передаваемых байтов
};
