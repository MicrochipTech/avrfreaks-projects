/********************************************************************************/
/*  sd-atm, John Kim, john-k.home@rcn.com,  October 2007                        */
/*   Stack depth analyzer, works for avr atm128.                                */
/*   Free code.                                                                 */
/*   Compiles on Windows, MSYS, MinGW, and "gcc (GCC) 3.4.2 (mingw-special)"    */
/*                                                                              */
/********************************************************************************/


#ifndef __COMMON_H__
#define __COMMON_H__
//=====================================================================================================

#define ___str_eq_1(a, b, ...) (!strcmp((a), (b)))


#define ___str_eq_4(a, b0, b1, b2, b3, ...)\
	(___str_eq_1(a, b0) || ___str_eq_1(a, b1) || ___str_eq_1(a, b2) || ___str_eq_1(a, b3))

#define ___str_eq_8(a, b0, b1, b2, b3, ...)\
	(___str_eq_1(a, b0) || ___str_eq_1(a, b1) || ___str_eq_1(a, b2) || ___str_eq_1(a, b3) ||\
	___str_eq_4(a, __VA_ARGS__, b0, b0, b0, b0))

#define ___str_eq_12(a, b0, b1, b2, b3, ...)\
	(___str_eq_1(a, b0) || ___str_eq_1(a, b1) || ___str_eq_1(a, b2) || ___str_eq_1(a, b3) ||\
	___str_eq_8(a, __VA_ARGS__, b0, b0, b0, b0))

#define ___str_eq_16(a, b0, b1, b2, b3, ...)\
	(___str_eq_1(a, b0) || ___str_eq_1(a, b1) || ___str_eq_1(a, b2) || ___str_eq_1(a, b3) ||\
	___str_eq_12(a, __VA_ARGS__, b0, b0, b0, b0))

#define ___str_eq_20(a, b0, b1, b2, b3, ...)\
	(___str_eq_1(a, b0) || ___str_eq_1(a, b1) || ___str_eq_1(a, b2) || ___str_eq_1(a, b3) ||\
	___str_eq_16(a, __VA_ARGS__, b0, b0, b0, b0))

#define ___str_eq_24(a, b0, b1, b2, b3, ...)\
	(___str_eq_1(a, b0) || ___str_eq_1(a, b1) || ___str_eq_1(a, b2) || ___str_eq_1(a, b3) ||\
	___str_eq_20(a, __VA_ARGS__, b0, b0, b0, b0))

#define ___str_eq_28(a, b0, b1, b2, b3, ...)\
	(___str_eq_1(a, b0) || ___str_eq_1(a, b1) || ___str_eq_1(a, b2) || ___str_eq_1(a, b3) ||\
	___str_eq_24(a, __VA_ARGS__, b0, b0, b0, b0))

#define ___str_eq_32(a, b0, b1, b2, b3, ...)\
	(___str_eq_1(a, b0) || ___str_eq_1(a, b1) || ___str_eq_1(a, b2) || ___str_eq_1(a, b3) ||\
	___str_eq_28(a, __VA_ARGS__, b0, b0, b0, b0))

#define ___str_eq_start(a, b0, b1, b2, b3, ...)\
	(___str_eq_1(a, b0) || ___str_eq_1(a, b1) || ___str_eq_1(a, b2) || ___str_eq_1(a, b3) ||\
	___str_eq_32(a, __VA_ARGS__, b0, b0, b0, b0))

#define ___str_eq(a, b, ...) (___str_eq_start(a, b, b, b, b, b, ##__VA_ARGS__))

//-----------------------------
#define __eq_1(a, b, ...) ((a) == (b))


#define __eq_4(a, b0, b1, b2, b3, ...)\
	(__eq_1(a, b0) || __eq_1(a, b1) || __eq_1(a, b2) || __eq_1(a, b3))

#define __eq_8(a, b0, b1, b2, b3, ...)\
	(__eq_1(a, b0) || __eq_1(a, b1) || __eq_1(a, b2) || __eq_1(a, b3) ||\
	__eq_4(a, __VA_ARGS__, b0, b0, b0, b0))

#define __eq_12(a, b0, b1, b2, b3, ...)\
	(__eq_1(a, b0) || __eq_1(a, b1) || __eq_1(a, b2) || __eq_1(a, b3) ||\
	__eq_8(a, __VA_ARGS__, b0, b0, b0, b0))

#define __eq_16(a, b0, b1, b2, b3, ...)\
	(__eq_1(a, b0) || __eq_1(a, b1) || __eq_1(a, b2) || __eq_1(a, b3) ||\
	__eq_12(a, __VA_ARGS__, b0, b0, b0, b0))

#define __eq_20(a, b0, b1, b2, b3, ...)\
	(__eq_1(a, b0) || __eq_1(a, b1) || __eq_1(a, b2) || __eq_1(a, b3) ||\
	__eq_16(a, __VA_ARGS__, b0, b0, b0, b0))

#define __eq_24(a, b0, b1, b2, b3, ...)\
	(__eq_1(a, b0) || __eq_1(a, b1) || __eq_1(a, b2) || __eq_1(a, b3) ||\
	__eq_20(a, __VA_ARGS__, b0, b0, b0, b0))

#define __eq_28(a, b0, b1, b2, b3, ...)\
	(__eq_1(a, b0) || __eq_1(a, b1) || __eq_1(a, b2) || __eq_1(a, b3) ||\
	__eq_24(a, __VA_ARGS__, b0, b0, b0, b0))

#define __eq_32(a, b0, b1, b2, b3, ...)\
	(__eq_1(a, b0) || __eq_1(a, b1) || __eq_1(a, b2) || __eq_1(a, b3) ||\
	__eq_28(a, __VA_ARGS__, b0, b0, b0, b0))

#define __eq_start(a, b0, b1, b2, b3, ...)\
	(__eq_1(a, b0) || __eq_1(a, b1) || __eq_1(a, b2) || __eq_1(a, b3) ||\
	__eq_32(a, __VA_ARGS__, b0, b0, b0, b0))

#define __eq(a, b, ...) (__eq_start(a, b, b, b, b, b, ##__VA_ARGS__))
//-----------------------------




#define __max(a, b) ((a > b) ? a : b)
#define __min(a, b) ((a < b) ? a : b)

//=====================================================================================================
typedef enum {opr_NONE, opr_HEX, opr_INT}opr_types_t;

#define __SREG__ 0x3f
#define __SP_H__ 0x3e
#define __SP_L__ 0x3d

#define DEPTH_NONE -1000

//--------------------------------------------------
typedef unsigned int addr_t;

typedef struct
{
	char inst[MAX_INST_LEN];
	int operand;
	opr_types_t opr_type;
	int delta;
	int depth_forward;
	int depth_backward;
	int idx_label;
}code_t;

typedef struct
{
	addr_t addr;
	char label[MAX_LABEL_LEN];
}label_t;



#endif //__COMMON_H__
