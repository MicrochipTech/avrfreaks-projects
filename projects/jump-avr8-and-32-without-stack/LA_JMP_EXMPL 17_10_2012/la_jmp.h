/*
 * la_jmp.h
 *
 * Created: 07.08.2011 12:03:36
 * Release: 16.10.2012
 *  Author: Григорий Ваулин/ Grigory Vaulin
 *  ООО "Лаборатория автоматики"/ OOO Laboratory automation
 *  www.lab-automat.ru
 *
 *  Макросы la_jmp предназначены для создания переходом без участия стека,
 *  простого программирования выдержек времени без остановки микроконтроллера и 
 *  использования адресов меток как переменных. Только для AVR8 и AVR32 микроконтроллеров и
 *  программ на GCC 
 * 
 *	la_jmp macroses easy method for jump without stack, delay without break MCU and   
 *  use addresses labels as variable. For AVR8 and AVR32 only GCC compiler
 *
 */ 

#ifndef LA_JMP_H_
#define LA_JMP_H_

#define AVR32 32
#define AVR8 8
// Выбор ядра / Select AVR kernel here
#define AVR AVR8 

// Устанавливает метку / Set label 
#define la_set(lab) {asm volatile(#lab ": "::); }
#if (AVR == AVR32)
// Сохраняет адрес метки lab в переменной var / Save address the lab in var
// var должна быть типа U32  
	#define la_save(var, lab) { asm volatile("lda.w   %[VAL], "#lab "  \n\t" \
											" ":[VAL]"=r" (var): "[VAL]" (var)); }
	
// Устанавливает метку lab и сохраняет адрес метки lab в переменной var /
// Set label lab and save address the lab in var
	#define la_save_set(var, lab) { asm volatile("lda.w   %[VAL], "#lab "  \n\t" \
												" ":[VAL]"=r" (var): "[VAL]" (var)); \
												asm volatile(#lab ": "::); }
// Переходит на адресс указанные в переменной var / Jump to address saved in var												
	#define la_jmp(var) { asm volatile("mov  pc, %[VAL]":: [VAL]"r"(var)); } // lddpc
// Структура данных для выдержек времени / Define the type for delay macros
	typedef struct la_str_jmp {
		U32 *timer;
		U32 jmp;
	} la_str_jmp;											
#elif (AVR == AVR8)
// Сохраняет адрес метки lab в переменной var / Save address the lab in var
// var должна быть типа unsigned int
	#define la_save(var, lab) { asm volatile ("ldi r16, hi8("#lab ") " "\n\t" \
	"lsr r16 \n\t mov %B[retval], r16" "\n\t" \
	"ldi r16, lo8("#lab ") " "\n\t" \
	"ror r16 \n\t mov %A[retval], r16" "\n\t" \
	" " :[retval] "=r" (var): "[retval]" (var) : "r16"); }
// Устанавливает метку lab и сохраняет адрес метки lab в переменной var /
// Set label lab and save address the lab in var	
	#define la_save_set(var, lab) { la_save(var, lab); la_set(lab); }
// Переходит на адресс указанные в переменной var / Jump to address saved in var	
	#define la_jmp(var) { asm volatile("ijmp ":: [VAL]"z"(var)); } // lddpc	
// Структура данных для выдержек времени / Define the type for delay macros											
	typedef struct la_str_jmp {
		unsigned int *timer;
		unsigned int jmp;
	} la_str_jmp;	
#else 
	#error "UNKNOWN AVR kernel"
#endif


// Переход при условии наличия не нулевого адреса / Jump if address is not null
#define la_c_jmp(i_jmp) \
	if(i_jmp->jmp) \
		{ la_jmp(i_jmp->jmp); }
// Выдержка времени. Если время не прошло то выполняется переход на long_exit /
// Delay. If delay is not full then program do long_exit 	
#define la_jmp_time(lab, time, i_jmp, long_exit)  \
	{ *i_jmp->timer = 0; la_save(i_jmp->jmp, lab); \
	la_set(lab); while(*i_jmp->timer < time) \
		{ long_exit; } \
	i_jmp->jmp = 0; }
// Макрос устанавлеваемые перед функцией, где используются макросы la_jmp /
// Macros is settings before functions if it have internal la_jmp macroses
#define la_jmp_before_func(lab, i_jmp) {la_save(i_jmp->jmp, lab); la_set(lab);}
// Макрос устанавлеваемый после функциии, где используются макросы la_jmp и
// функция зада внутри la_c_jmp - label конструкции/
// Macros is settings after functions if it have internal la_jmp macroses and
// function calls in la_c_jmp - label structure	
#define la_jmp_after_func(i_jmp, j_jmp, long_exit)  \
	if (j_jmp->jmp) \
		{ long_exit; } \
	else  { i_jmp->jmp = 0; }
			
#endif /* LA_JMP_H_ */