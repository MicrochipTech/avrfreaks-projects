/*****************************************************************************
*
	Обработчик TWI прерывания. Прерывания вырабатываются при фиксации события на шине.
	TWSR - регистр статусной информации интерфейса, используется для анализа причины прерывания.
	TWCR - регист установки режима работы интерфейса, используется для задания следующей команды контроллеру интерфейса.
	Внешняя функция должна сделать следующее: 
	1. Проверить признак окончания предыдущей транзакции по шине TWI в TWI_stat.TWI_state.
	   Если значение 0xFF, то интерфейс временно занят и ничего делать с ним нельзя.
	2. Если интерфейс свободен, то  TWI_stat.TWI_state=oxff - занять интерфейс, TWI_stat.TWI_msgSize = количество байт для передачи,
	   TWI_buf[TWI_BUFFER_SIZE] - заполнить данными для передачи.
	3. Разрешить прерывания от TWI контроллера, посылкой start условия.
	
	Обработчик прерывания работает как машина состояний. Следующая команда для интерфейса зависит от кода TWSR
	и текущей позиции в буфере TWI_buf.
	
	После окончания транзакции, контроллер TWI установит TWI_stat.TWI_state, указывая, что операция завершена.
	Для операции записи выдвинется информация из буфера на указанную длину, причем для модуля Ethernet
	3и первых байта будут означать следующее: адрес девайса на линии с кодом операции записи, старшиая часть адреса назначения,
	младшая часть адреса назначения, далее идут данные, соответственно длины буфера. Минимальный пакет для передачи
	- 3и байта для установки адреса.
	Для операции чтения первый байт буфера будет содержать код адреса девайса с битом операции, остальная часть буфера,
	соответственно длине, будет содержать прочитанные данные по окончанию выполнения.
	Для понимания этого порядка вещей необходимо обратиться к описанию контроллера WizNet в части работы с I2C интерфейсом.
*
****************************************************************************/

#include <avr/io.h>
#include <compat/twi.h>
#include <avr/interrupt.h>            
#include "headers/defConstant.h"
#include "headers/defStruct.h"
#include "headers/MACRO.H"

#define TWI_TR			0x85	//Передача очередного байта в интерфейс.
#define TWI_STOP		0x94	//Генерация стор состояния, запрет прерываний от TWI.
#define TWI_RC_ACK		0xC5	//Прием очередного байта с интерфейса, требуется ACK.
#define TWI_RC_NACK		0x85	//Прием последнего байта с интерфейса. Не требуется ACK.
#define TWI_ARB			0xA5	//Генерация старт состояния.
/*
	TWI_BUSY		0xFF	// TWI Bus busy
	TWI_FREE		0x00	// TWI Bus free
	TWI_WRITE		0x01	// TWI Bus write compleat and free
	TWI_READ		0x02	// TWI Bus read compleat and free
	TWI_ERROR		0x03	// TWI Bus error and free
*/
extern struct TWI_status	 TWI_stat;
extern unsigned char time_out[NT];	// массив переменных для организации задержек
/*
struct TWI_status		// Структура описания состояния TWI интерфейса.
{
	volatile unsigned char		TWI_state;					// Код состояния. Default set to TWI_NO_STATE.
	unsigned char				TWI_msgSize;				// Number of bytes to be transmitted. Количество байт в буфере для передачи или приема.
	unsigned char				TWI_buf[TWI_BUFFER_SIZE];	// Transceiver buffer. Буфер данных для передачи или приема.
}
*/

SIGNAL(SIG_2WIRE_SERIAL)
{
	static unsigned char indexTWIbuf;			//текущий индекс в буфере обмена TWI интерфейса

	time_out[3] = BLOCK_TWI_TIME;					// установка таймера ожидания нового прерывания в исходное состояние
	switch (TWSR)
	{
		case TW_START:								// START has been transmitted 
		case TW_REP_START:							// Repeated START has been transmitted
		indexTWIbuf = 0;							// Set buffer pointer to the TWI Address location
		case TW_MT_SLA_ACK:						// SLA+W has been tramsmitted and ACK received
		case TW_MT_DATA_ACK:						// Data byte has been tramsmitted and ACK received
		if (indexTWIbuf < TWI_stat.TWI_msgSize)
		{
		/*
		TWI Interface enabled (1<<TWEN)|(1<<TWIE)|(1<<TWINT)
		Enable TWI Interupt and clear the flag to send byte
		*/
			TWDR = TWI_stat.TWI_buf[indexTWIbuf++];//взять очередной байт из буфера TWI.
			TWCR = TWI_TR;							//передача очередного байта в интерфейс.
		}
		else										// Send STOP after last byte
		{
		/*
		Set status bits to completed successfully.
		TWI Interface enabled (1<<TWEN)|(1<<TWINT)|(1<<TWSTO)
		Disable TWI Interrupt and clear the flag
		Initiate a STOP condition.
		*/
			TWCR = TWI_STOP;						//Генерация стор состояния, запрет прерываний от TWI.
			TWI_stat.TWI_state = TWI_WRITE;			//запись окончена
		}
		break;
		case TW_MR_DATA_ACK:						// Data byte has been received and ACK tramsmitted
		TWI_stat.TWI_buf[indexTWIbuf++] = TWDR;
		case TW_MR_SLA_ACK:						// SLA+R has been tramsmitted and ACK received. Прием очередного байта с интерфейса, требуется ACK.
		if (indexTWIbuf < (TWI_stat.TWI_msgSize-1))// Detect the last byte to NACK it. Проверка условия ожидания приема
		/*
		TWI Interface enabled (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA)
		Enable TWI Interupt and clear the flag to read next byte
		Send ACK after reception
		*/
		TWCR = TWI_RC_ACK;							//Прием очередного байта с интерфейса, требуется ACK.
		else										// Send NACK after next reception
		/*
		TWI Interface enabled (1<<TWEN)|(1<<TWIE)|(1<<TWINT)
		Enable TWI Interupt and clear the flag to read next byte
		Send NACK after reception
		*/
		TWCR = TWI_RC_NACK;							//прием последнего байта с интерфейса.
		break; 
		case TW_MR_DATA_NACK:						// Data byte has been received and NACK tramsmitted
		TWI_stat.TWI_buf[indexTWIbuf] = TWDR;
		/*
		Set status bits to completed successfully. 
		TWI Interface enabled
		Disable TWI Interrupt and clear the flag
		Initiate a STOP condition.
		*/
		TWCR = TWI_STOP;							//Генерация стор состояния, запрет прерываний от TWI.
		TWI_stat.TWI_state = TWI_READ;				//чтение завершено	
		break;      
		case TW_MT_ARB_LOST:						// Arbitration lost
		/*
		TWI Interface enabled (1<<TWEN)|(1<<TWSTA)|(1<<TWIE)|(1<<TWINT)
		Enable TWI Interupt and clear the flag
		Initiate a (RE)START condition.
		*/
		TWCR = TWI_ARB;								//Генерация старт состояния.
		break;
		case TW_MT_SLA_NACK:						// SLA+W has been tramsmitted and NACK received
		case TW_MR_SLA_NACK:    					// SLA+R has been tramsmitted and NACK received    
		case TW_MT_DATA_NACK:						// Data byte has been tramsmitted and NACK received
		case TW_BUS_ERROR:							// Bus error due to an illegal START or STOP condition
		default:
		/*
		Reset TWI Interface
		Enable TWI-interface and release TWI pins
		Store TWSR and automatically sets clears noErrors bit.
		Disable Interupt
		No Signal requests
		*/     
		TWCR = TWI_STOP;							//Генерация стор состояния, запрет прерываний от TWI.
		TWI_stat.TWI_state = TWI_ERROR;				//ошибка шины
	}
}

