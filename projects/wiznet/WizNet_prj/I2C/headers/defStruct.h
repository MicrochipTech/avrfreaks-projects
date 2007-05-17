/****************************************************************************
  Описание основных структур программы
****************************************************************************/

struct Request
{	
	unsigned char		req;		// признак запроса передачи/приема пакета к модулю ethernet
	unsigned char		leng;		// длина запрошенного к передаче пакета
};

struct TWI_status											// Status byte holding flags.
{
	volatile unsigned char	TWI_state;					// State byte. Default set to 0.
	unsigned char				TWI_msgSize;				// Number of bytes to be transmitted.
	unsigned char				TWI_buf[TWI_BUFFER_SIZE];	// Transceiver buffer
};
/*-------------------------------------------------------------------------*/
struct Descript
{	
	unsigned char	phase;			// текущая фаза работы с модулем ethernet.
	unsigned char	countError;		// счетчик ошибок транзакций к модулю ethernet через TWI
};
