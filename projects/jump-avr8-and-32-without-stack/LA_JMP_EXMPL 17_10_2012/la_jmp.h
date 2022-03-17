/*
 * la_jmp.h
 *
 * Created: 07.08.2011 12:03:36
 * Release: 16.10.2012
 *  Author: �������� ������/ Grigory Vaulin
 *  ��� "����������� ����������"/ OOO Laboratory automation
 *  www.lab-automat.ru
 *
 *  ������� la_jmp ������������� ��� �������� ��������� ��� ������� �����,
 *  �������� ���������������� �������� ������� ��� ��������� ���������������� � 
 *  ������������� ������� ����� ��� ����������. ������ ��� AVR8 � AVR32 ����������������� �
 *  �������� �� GCC 
 * 
 *	la_jmp macroses easy method for jump without stack, delay without break MCU and   
 *  use addresses labels as variable. For AVR8 and AVR32 only GCC compiler
 *
 */ 

#ifndef LA_JMP_H_
#define LA_JMP_H_

#define AVR32 32
#define AVR8 8
// ����� ���� / Select AVR kernel here
#define AVR AVR8 

// ������������� ����� / Set label 
#define la_set(lab) {asm volatile(#lab ": "::); }
#if (AVR == AVR32)
// ��������� ����� ����� lab � ���������� var / Save address the lab in var
// var ������ ���� ���� U32  
	#define la_save(var, lab) { asm volatile("lda.w   %[VAL], "#lab "  \n\t" \
											" ":[VAL]"=r" (var): "[VAL]" (var)); }
	
// ������������� ����� lab � ��������� ����� ����� lab � ���������� var /
// Set label lab and save address the lab in var
	#define la_save_set(var, lab) { asm volatile("lda.w   %[VAL], "#lab "  \n\t" \
												" ":[VAL]"=r" (var): "[VAL]" (var)); \
												asm volatile(#lab ": "::); }
// ��������� �� ������ ��������� � ���������� var / Jump to address saved in var												
	#define la_jmp(var) { asm volatile("mov  pc, %[VAL]":: [VAL]"r"(var)); } // lddpc
// ��������� ������ ��� �������� ������� / Define the type for delay macros
	typedef struct la_str_jmp {
		U32 *timer;
		U32 jmp;
	} la_str_jmp;											
#elif (AVR == AVR8)
// ��������� ����� ����� lab � ���������� var / Save address the lab in var
// var ������ ���� ���� unsigned int
	#define la_save(var, lab) { asm volatile ("ldi r16, hi8("#lab ") " "\n\t" \
	"lsr r16 \n\t mov %B[retval], r16" "\n\t" \
	"ldi r16, lo8("#lab ") " "\n\t" \
	"ror r16 \n\t mov %A[retval], r16" "\n\t" \
	" " :[retval] "=r" (var): "[retval]" (var) : "r16"); }
// ������������� ����� lab � ��������� ����� ����� lab � ���������� var /
// Set label lab and save address the lab in var	
	#define la_save_set(var, lab) { la_save(var, lab); la_set(lab); }
// ��������� �� ������ ��������� � ���������� var / Jump to address saved in var	
	#define la_jmp(var) { asm volatile("ijmp ":: [VAL]"z"(var)); } // lddpc	
// ��������� ������ ��� �������� ������� / Define the type for delay macros											
	typedef struct la_str_jmp {
		unsigned int *timer;
		unsigned int jmp;
	} la_str_jmp;	
#else 
	#error "UNKNOWN AVR kernel"
#endif


// ������� ��� ������� ������� �� �������� ������ / Jump if address is not null
#define la_c_jmp(i_jmp) \
	if(i_jmp->jmp) \
		{ la_jmp(i_jmp->jmp); }
// �������� �������. ���� ����� �� ������ �� ����������� ������� �� long_exit /
// Delay. If delay is not full then program do long_exit 	
#define la_jmp_time(lab, time, i_jmp, long_exit)  \
	{ *i_jmp->timer = 0; la_save(i_jmp->jmp, lab); \
	la_set(lab); while(*i_jmp->timer < time) \
		{ long_exit; } \
	i_jmp->jmp = 0; }
// ������ ��������������� ����� ��������, ��� ������������ ������� la_jmp /
// Macros is settings before functions if it have internal la_jmp macroses
#define la_jmp_before_func(lab, i_jmp) {la_save(i_jmp->jmp, lab); la_set(lab);}
// ������ ��������������� ����� ��������, ��� ������������ ������� la_jmp �
// ������� ���� ������ la_c_jmp - label �����������/
// Macros is settings after functions if it have internal la_jmp macroses and
// function calls in la_c_jmp - label structure	
#define la_jmp_after_func(i_jmp, j_jmp, long_exit)  \
	if (j_jmp->jmp) \
		{ long_exit; } \
	else  { i_jmp->jmp = 0; }
			
#endif /* LA_JMP_H_ */