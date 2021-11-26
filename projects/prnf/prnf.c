/*
*/

#ifndef SECOND_PASS
	#define FIRST_PASS
	#define IS_SECOND_PASS false

	#include <stdbool.h>
	#include <stddef.h>
	#include <stdarg.h>
	#include <stdint.h>
	#include <limits.h>

	#ifdef PLATFORM_AVR
		#include <avr/pgmspace.h>
	#endif

	#include "prnf.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

	#define SUPPORT_FLOAT

	#define ENG_PREC_DEFAULT 	0

	#define FLOAT_PREC_DEFAULT 	3

//********************************************************************************************************
// Local defines
//********************************************************************************************************

	#ifndef SIZE_MAX
		#define SIZE_MAX ((size_t)(ssize_t)(-1))
	#endif

	#ifdef SUPPORT_FLOAT
		#include <float.h>
	#endif

//	Should =< the number of decimal digits which can be represented by an un signed long
	#define FLOAT_PREC_MAX		8

	#ifndef WARN
		#define WARN(arg) ((void)(0))
	#endif

	#ifndef ASSERT
		#define ASSERT(arg) ((void)(0))
	#endif

	#ifdef PLATFORM_AVR
		#define FMTRD(_fmt) 	fmt_rd_either(_fmt, is_pgm)
	#else
		#define FMTRD(_fmt) 	(*(_fmt))
	#endif

	enum {TYPE_NONE, TYPE_BIN, TYPE_INT, TYPE_UINT, TYPE_HEX, TYPE_STR, TYPE_PSTR, TYPE_CHAR, TYPE_FLOAT, TYPE_ENG};	// di u xX s S c fF eE

	struct placeholder_struct
	{
		bool	flag_minus;
		bool	flag_zero;
		char 	sign_pad;			//'+' or ' ' if a prepend character for positive numeric types is specified. Otherwise 0
		bool 	prec_specified;
		int 	width;
		int 	prec;
		uint_least8_t size_modifier;	//equal to size of int, or size of specified type (l h hh etc)
		uint_least8_t type;				//TYPE_x
	};

	struct out_struct
	{
		bool	terminate;
		size_t	size_limit;
		int		char_cnt;
		void* 	dst_fptr_vars;
		void(*dst_fptr)(void*, char);
	};

	struct eng_struct
	{
		float value;
		char prefix;
	};

#endif	//FIRST PASS

#ifdef FIRST_PASS
	#define prnf_PX 		prnf
	#define sprnf_PX 		sprnf
	#define snprnf_PX 		snprnf
	#define snappf_PX 		snappf
	#define vprnf_PX 		vprnf
	#define vsprnf_PX 		vsprnf
	#define vsnprnf_PX 		vsnprnf
	#define fptrprnf_PX 	fptrprnf
	#define vfptrprnf_PX 	vfptrprnf
#else
	#undef prnf_PX
	#undef sprnf_PX
	#undef snprnf_PX
	#undef snappf_PX
	#undef vprnf_PX
	#undef vsprnf_PX
	#undef vsnprnf_PX
	#undef fptrprnf_PX
	#undef vfptrprnf_PX
	#define prnf_PX 		prnf_P
	#define sprnf_PX 		sprnf_P
	#define snprnf_PX 		snprnf_P
	#define snappf_PX 		snappf_P
	#define vprnf_PX 		vprnf_P
	#define vsprnf_PX 		vsprnf_P
	#define vsnprnf_PX 		vsnprnf_P
	#define fptrprnf_PX 	fptrprnf_P
	#define vfptrprnf_PX 	vfptrprnf_P
#endif

//********************************************************************************************************
// Public variables
//********************************************************************************************************
	
#ifdef FIRST_PASS
//	Default character output, may be pointed at an application provided putchar(void*, charx)
//	The void* is passed a NULL
	void(*prnf_out_fptr)(void*, char) = NULL;
#endif

//********************************************************************************************************
// Private variables
//********************************************************************************************************

#ifdef FIRST_PASS
#ifdef SUPPORT_FLOAT
	static float pow10_tbl[FLOAT_PREC_MAX+1] = {1E0F, 1E1F, 1E2F, 1E3F, 1E4F, 1E5F, 1E6F, 1E7F, 1E8F};
#endif
#endif

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

#ifdef FIRST_PASS
	static const char* parse_placeholder(struct placeholder_struct* placeholder, const char* fmtstr, bool is_pgm);
	static int core_prnf(struct out_struct* out_info, const char* fmtstr, bool is_pgm, va_list va);

	static void print_bin(struct out_struct *out_info, struct placeholder_struct* placeholder, unsigned long uvalue);
	static void print_hex(struct out_struct *out_info, struct placeholder_struct* placeholder, unsigned long uvalue);
	static void print_dec(struct out_struct *out_info, struct placeholder_struct* placeholder, long value);

#ifdef SUPPORT_FLOAT
	static void print_float(struct out_struct *out_info, struct placeholder_struct* placeholder, float value, char postpend);
	static const char* determine_float_msg(struct placeholder_struct* placeholder, float value);
	static char determine_sign_char_of_float(struct placeholder_struct* placeholder, float value);
	static void print_float_normal(struct out_struct *out_info, struct placeholder_struct* placeholder, float value, char postpend);
	static void print_float_special(struct out_struct *out_info, struct placeholder_struct* placeholder, const char* out_msg, float value);
	static struct eng_struct get_eng(float value);
	static unsigned long round_float_to_ulong(float x);
	static uint_least8_t get_prec(struct placeholder_struct* placeholder);
#endif

	static void prepad(struct out_struct *out_info, struct placeholder_struct* placeholder, size_t source_len);
	static void postpad(struct out_struct *out_info, struct placeholder_struct* placeholder, size_t source_len);
	static bool is_type_int(uint_least8_t type);
	static bool is_type_unsigned(uint_least8_t type);
	static bool is_type_numeric(uint_least8_t type);
	static bool is_centered_string(struct placeholder_struct* placeholder);
	static uint_least8_t sizeof_sizemod(uint_least8_t size_modifier);

	static bool prnf_is_digit(char x);
	static char ascii_hex_digit(uint_least8_t x);
	
	static void out(struct out_struct *out_info, char x);
	static void out_buf(void* out_buf_vars, char x);

	static void print_str(struct out_struct* out_info, struct placeholder_struct* placeholder, const char* str, bool is_pgm);
	static int prnf_strlen(const char* str, bool is_pgm);

	#ifdef PLATFORM_AVR
		static char fmt_rd_either(const char* fmt, bool is_pgm);
	#endif

#endif

//********************************************************************************************************
// Public functions
//********************************************************************************************************

int prnf_PX(const char* fmtstr, ...)
{
  va_list va;
  va_start(va, fmtstr);

  const int ret = vprnf_PX(fmtstr, va);

  va_end(va);
  return ret;
}

int sprnf_PX(char* dst, const char* fmtstr, ...)
{
  va_list va;
  va_start(va, fmtstr);

  const int ret = vsprnf_PX(dst, fmtstr, va);

  va_end(va);
  return ret;
}

int snprnf_PX(char* dst, size_t dst_size, const char* fmtstr, ...)
{
  va_list va;
  va_start(va, fmtstr);

  const int ret = vsnprnf_PX(dst, dst_size, fmtstr, va);

  va_end(va);
  return ret;
}

int snappf_PX(char* dst, size_t dst_size, const char* fmtstr, ...)
{
	va_list va;
	va_start(va, fmtstr);
	int chars_written = 0;
	size_t org_len;

	if(dst_size)
	{
		org_len = prnf_strlen(dst, false);
		if(org_len > dst_size-1)
			org_len = dst_size-1;
	}
	else
		org_len = 0;

	chars_written = vsnprnf_PX(&dst[org_len], dst_size-org_len, fmtstr, va);

	va_end(va);
	return chars_written;
}

int fptrprnf_PX(void(*out_fptr)(void*, char), void* out_vars, const char* fmtstr, ...)
{
	va_list va;
	va_start(va, fmtstr);

	const int ret = vfptrprnf_PX(out_fptr, out_vars, fmtstr, va);

	va_end(va);
	return ret;
}

int vprnf_PX(const char* fmtstr, va_list va)
{
	struct out_struct out_info = {.terminate=false, .size_limit=SIZE_MAX, .char_cnt=0, .dst_fptr_vars=NULL, .dst_fptr=prnf_out_fptr};
	return core_prnf(&out_info, fmtstr, IS_SECOND_PASS, va);
}

int vsprnf_PX(char* dst, const char* fmtstr, va_list va)
{
	struct out_struct out_info = {.terminate=true, .size_limit=SIZE_MAX, .char_cnt=0, .dst_fptr_vars=&dst, .dst_fptr=out_buf};
	return core_prnf(&out_info, fmtstr, IS_SECOND_PASS, va);
}

int vsnprnf_PX(char* dst, size_t dst_size, const char* fmtstr, va_list va)
{
	struct out_struct out_info = {.terminate=true, .size_limit=dst_size, .char_cnt=0, .dst_fptr_vars=&dst, .dst_fptr=out_buf};
	return core_prnf(&out_info, fmtstr, IS_SECOND_PASS, va);
}

int vfptrprnf_PX(void(*out_fptr)(void*, char), void* out_vars, const char* fmtstr, va_list va)
{
	struct out_struct out_info = {.terminate=false, .size_limit=SIZE_MAX, .char_cnt=0, .dst_fptr_vars=out_vars, .dst_fptr=out_fptr};
	return core_prnf(&out_info, fmtstr, IS_SECOND_PASS, va);
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

// wrappers for PGM or non-PGM access

#ifdef FIRST_PASS

#ifdef PLATFORM_AVR
static char fmt_rd_either(const char* fmt, bool is_pgm)
{
	return is_pgm? 	pgm_read_byte(fmt):(*fmt);
}
#endif

static int core_prnf(struct out_struct* out_info, const char* fmtstr, bool is_pgm, va_list va)
{
	struct placeholder_struct placeholder;

	union
	{
		long l;
		unsigned long ul;
		int i;
		unsigned int ui;
	} value;

	#ifdef SUPPORT_FLOAT
		struct eng_struct eng;
	#endif

	while(FMTRD(fmtstr))
	{
		// placeholder? %[flags][width][.precision][length]type
		if(FMTRD(fmtstr) == '%')
		{
			fmtstr++;
			if(FMTRD(fmtstr) == '%')
			{
				out(out_info, '%');
				fmtstr++;
			}
			else
			{
				fmtstr = parse_placeholder(&placeholder, fmtstr, is_pgm);

				// This was initially in a print_placeholder() function, but passing va_list to a function did not work on gcc-avr
				// each call accessed the first argument. Attempting to pass it by reference did not work on PC, as on the PC va_list is
				// an array type which was broken by pointer decay (although this did work on AVR).

				//integer type? (int uint bin hex size_t ptrdiff_t)
				if(is_type_int(placeholder.type))
				{
					if(placeholder.size_modifier == sizeof(long))
						value.ul = va_arg(va, unsigned long);	
					else
					{
						value.ui = va_arg(va, unsigned int);	
						if(is_type_unsigned(placeholder.type))
							value.ul = value.ui;
						else
							value.l = value.i;
					};
				};

				if(placeholder.type == TYPE_INT || placeholder.type == TYPE_UINT)
					print_dec(out_info, &placeholder, value.l);
				
				else if(placeholder.type == TYPE_BIN)
					print_bin(out_info, &placeholder, value.ul);
							
				else if(placeholder.type == TYPE_HEX)
					print_hex(out_info, &placeholder, value.ul);

#ifdef SUPPORT_FLOAT
				else if(placeholder.type == TYPE_FLOAT)
					print_float(out_info, &placeholder, (float)va_arg(va, double), 0);

				else if(placeholder.type == TYPE_ENG)
				{	
					eng = get_eng((float)va_arg(va, double));
					print_float(out_info, &placeholder, eng.value, eng.prefix);
				}
#endif
				else if(placeholder.type == TYPE_CHAR)
					out(out_info, (char)va_arg(va, int));

				else if(placeholder.type == TYPE_STR)
					print_str(out_info, &placeholder, va_arg(va, char*), false);

				#ifdef PLATFORM_AVR
				else if(placeholder.type == TYPE_PSTR)
					print_str(out_info, &placeholder, va_arg(va, char*), true);
				#endif
			};
		}
		else
		{
			out(out_info, FMTRD(fmtstr));
			fmtstr++;
		};
	};

	// Terminate
	out(out_info, 0);

	return out_info->char_cnt;
}

// parse textual placeholder information into a placeholder_struct
static const char* parse_placeholder(struct placeholder_struct* dst, const char* fmtstr, bool is_pgm)
{
	struct placeholder_struct placeholder = {false, false, 0, false, 0, 0, sizeof(int), TYPE_NONE};
	bool finished;

	//Get flags
	do
	{
		finished = false;
		switch(FMTRD(fmtstr))
		{
			case '0': placeholder.flag_zero = true;		break;
			case '-': placeholder.flag_minus = true;	break;
			case '+': placeholder.sign_pad = '+';  		break;
			case ' ': placeholder.sign_pad = ' ';  		break;
			case '#': WARN(false);						break;	//unsupported flag
			case '\'': WARN(false);						break;	//unsupported flag
			default : finished = true;        			break;
		};
		if(!finished)
			fmtstr++;
	}while(!finished);

	//Get width
	while(prnf_is_digit(FMTRD(fmtstr)))
	{
		placeholder.width *=10;
		placeholder.width += FMTRD(fmtstr)&0x0F;
		fmtstr++;
	};

	//Get precision
	if(FMTRD(fmtstr) == '.')
	{
		placeholder.prec_specified = true;
		fmtstr++;
		while(prnf_is_digit(FMTRD(fmtstr)))
		{
			placeholder.prec *=10;
			placeholder.prec += FMTRD(fmtstr)&0x0F;
			fmtstr++;
		};
	};

	//Get size modifier
	switch(FMTRD(fmtstr))
	{
		case 'h' :
			fmtstr++;
			if(FMTRD(fmtstr) == 'h')
			{
				placeholder.size_modifier = sizeof(char);
				fmtstr++;
			}
			else
				placeholder.size_modifier = sizeof(short);
			break;

		case 'l' :
			fmtstr++;
			ASSERT(FMTRD(fmtstr) != 'l');	//unsupported long long
			placeholder.size_modifier = sizeof(long);
			break;

		case 't' :
			fmtstr++;
			placeholder.size_modifier = sizeof(ptrdiff_t);
			break;

		case 'z' :
			fmtstr++;
			placeholder.size_modifier = sizeof(size_t);
			break;

		case 'L' :	//unsupported long double
		case 'j' :	//unsupported intmax_t
			ASSERT(false);
	};

	//Get type
	switch(FMTRD(fmtstr))
	{
		case 'd' :
		case 'i' :
			placeholder.type = TYPE_INT;
			break;
		
		case 'o' :
			placeholder.type = TYPE_BIN;
			break;

		case 'u' :
			placeholder.type = TYPE_UINT;
			break;

		case 'x' :
		case 'X' :
			placeholder.type = TYPE_HEX;
			break;

#ifdef SUPPORT_FLOAT
		case 'f' :
		case 'F' :
			placeholder.type = TYPE_FLOAT;
			break;

		case 'e' :
		case 'E' :
			placeholder.type = TYPE_ENG;
			break;
#endif

		case 'S' :
		#ifdef PLATFORM_AVR
			placeholder.type = TYPE_PSTR;
			break;
		#endif
		case 's' :
			placeholder.type = TYPE_STR;
			break;

		case 'c' :
			placeholder.type = TYPE_CHAR;
			break;
		
		default :
			ASSERT(false);	//unsupported type
			break;
	};
	fmtstr++;

	*dst = placeholder;
	return fmtstr;
}

//Handles both signed and unsigned decimal integers
static void print_dec(struct out_struct *out_info, struct placeholder_struct* placeholder, long value)
{
	unsigned long uvalue;
	unsigned long remainder;
	int number_len = 1;
	int zero_pad_len = 0;
	unsigned long weight = 1;
	char sign_char = 0;
	uint_least8_t digit;

	if(is_type_unsigned(placeholder->type))
		uvalue = (unsigned long)value;
	else
	{
		uvalue = value;
		if(value < 0.0)
		{
			sign_char = '-';
			uvalue = -value;
		}
		else if(placeholder->sign_pad)
			sign_char = placeholder->sign_pad;
	};

	//determine number of digits
	remainder = uvalue;
	while(uvalue >= 10)
 	{
		uvalue /=10;
		weight *=10;
		number_len++;
 	};

	//if more digits required, determine amount of zero padding
	if(placeholder->prec_specified && placeholder->prec > number_len)
		zero_pad_len = placeholder->prec - number_len;
 
	//number length is now digits needed + zero padding
	number_len += zero_pad_len;

	//number length is now digits needed + zero padding + sign char (if any)
	if(sign_char)
		number_len++;

	//prepad number length to satisfy width  (if specified)
	prepad(out_info, placeholder, number_len);

	if(sign_char)
		out(out_info, sign_char);
	while(zero_pad_len--)
		out(out_info, '0');
	do
	{
		digit = remainder / weight;
		remainder = remainder % weight;
		out(out_info, (char)(0x30+digit));
		weight /=10;
	}while(weight);

	//postpad number length to satisfy width  (if specified)
	postpad(out_info, placeholder, number_len);
}

static void print_bin(struct out_struct *out_info, struct placeholder_struct* placeholder, unsigned long uvalue)
{
	int number_len;
	unsigned long bit;

	//determine number of digits
	if(placeholder->prec_specified)
		number_len = placeholder->prec;
	else
		number_len = placeholder->size_modifier * 8;

	//prepad number length to satisfy width  (if specified)
	prepad(out_info, placeholder, number_len);

	bit = 1UL<<(number_len-1);
	while(bit)
	{
		out(out_info, (uvalue & bit)? '1':'0');
		bit >>= 1;
	};

	//postpad number length to satisfy width  (if specified)
	postpad(out_info, placeholder, number_len);
}

static void print_hex(struct out_struct *out_info, struct placeholder_struct* placeholder, unsigned long uvalue)
{
	int number_len;
	uint_least8_t offset;

	//determine number of digits
	if(placeholder->prec_specified)
		number_len = placeholder->prec;
	else
		number_len = placeholder->size_modifier * 2;

	//prepad number length to satisfy width  (if specified)
	prepad(out_info, placeholder, number_len);

	offset = number_len*4;

	do
	{
		offset -= 4;
		out(out_info, ascii_hex_digit((uint_least8_t)(uvalue >> offset)));
	}while(offset);

	//postpad number length to satisfy width  (if specified)
	postpad(out_info, placeholder, number_len);
}

#ifdef SUPPORT_FLOAT
//	With precision of 3 printable range is +/- 4294967.295
static void print_float(struct out_struct *out_info, struct placeholder_struct* placeholder, float value, char postpend)
{
	const char* out_msg;

	out_msg = determine_float_msg(placeholder, value);
	if(out_msg)
		print_float_special(out_info, placeholder, out_msg, value);
	else
		print_float_normal(out_info, placeholder, value, postpend);
}

// Return FLOAT_CASE_x
static const char* determine_float_msg(struct placeholder_struct* placeholder, float value)
{
	char* retval = NULL;
	int prec;

	if(value < 0.0F)
		value = -value;

	// Determine special case messages
	if(value != value)
		retval = "NAN";
	else if(value > FLT_MAX)
		retval = "INF";

	// If not NAN or INF
	if(!retval)
	{
		// Multiply by 10^prec to move fractional digits into the integral digits
		prec = get_prec(placeholder);
		value *= pow10_tbl[prec];

		// Check within printable range
		if(value >= (float)ULONG_MAX)
			retval = "OVER";
	};

	return retval;
}

static void print_float_normal(struct out_struct *out_info, struct placeholder_struct* placeholder, float value, char postpend)
{
	uint_least8_t prec;
	uint_least8_t number_len = 1;
	uint_least8_t radix_pos = -1;
	unsigned long uvalue;
	unsigned long weight = 1;
	unsigned long remainder;
	uint_least8_t digit;
	char sign_char;

	sign_char = determine_sign_char_of_float(placeholder, value);

	// Multiply by 10^prec to move fractional digits into the integral digits
	prec = get_prec(placeholder);

	if(value < 0.0F)
		value = -value;
	value *= pow10_tbl[prec];
	uvalue = round_float_to_ulong(value);
	remainder = uvalue;

	//determine number of digits
	while(uvalue >= 10 || (number_len < prec+1))
	{
		uvalue /=10;
		weight *=10;
		number_len++;
	};

	if(prec)
	{
		number_len++;	//+1 for decimal point
		radix_pos = number_len - prec - 1;
	};

	if(sign_char)		//+1 for sign character
		number_len++;

	if(postpend)		//+1 for engineering notaion?
		number_len++;

	//prepad number length to satisfy width  (if specified)
	prepad(out_info, placeholder, number_len);

	if(sign_char)
		out(out_info, sign_char);
	
	do
	{
		if(radix_pos-- == 0)
			out(out_info, '.');
		digit = remainder / weight;
		remainder = remainder % weight;
		out(out_info, (char)(0x30+digit));
		weight /=10;
	}while(weight);

	if(postpend)
		out(out_info, postpend);

	//postpad number length to satisfy width  (if specified)
	postpad(out_info, placeholder, number_len);
}

// Floating point special cases
// "NAN" , "-INF" , "INF" , "+INF" , " INF" , "-OVER" , "OVER" , "+OVER" , " OVER"
static void print_float_special(struct out_struct *out_info, struct placeholder_struct* placeholder, const char* out_msg, float value)
{
	struct placeholder_struct ph = *placeholder;
	int msg_len;
	char sign_char;

	sign_char = determine_sign_char_of_float(placeholder, value);

	ph.type = TYPE_STR;
	ph.prec = 1;	// Must be non-0 or prepad/postpad may center the msg which is not what we want
	msg_len = prnf_strlen(out_msg, false);
	if(sign_char)
		msg_len++;

	//prepad length to satisfy width (if specified)
	prepad(out_info, &ph, msg_len);
	if(sign_char)
		out(out_info, sign_char);
	while(*out_msg)
		out(out_info, *out_msg++);
	postpad(out_info, &ph, msg_len);
}

// +, - , <space> or 0
static char determine_sign_char_of_float(struct placeholder_struct* placeholder, float value)
{
	char sign_char = 0;
	bool neg = (value < 0.0F);
	bool nan = (value != value);

	// Determine sign character
	if(neg)
		sign_char = '-';
	else if(!nan)
	{
		if(placeholder->sign_pad)
			sign_char = placeholder->sign_pad;
	};

	return sign_char;
}
#endif	//^SUPPORT_FLOAT^

static void print_str(struct out_struct *out_info, struct placeholder_struct* placeholder, const char* str, bool is_pgm)
{
	int	source_len;
	int cnt;

	source_len = prnf_strlen(str, is_pgm);

	//limit source length to precision, if precision is non0
	if(placeholder->prec_specified && placeholder->prec && placeholder->prec < source_len)
		source_len = placeholder->prec;

	prepad(out_info, placeholder, source_len);

	cnt = source_len;
	while(cnt--)
	{
		out(out_info, FMTRD(str));
		str++;
	};

	postpad(out_info, placeholder, source_len);
}

static int prnf_strlen(const char* str, bool is_pgm)
{
	int retval = 0;

	if(str)
	{
		while(FMTRD(str))
		{
			str++;
			retval++;
		};
	};
	return retval;
}

// prepad the output to achieve width, if needed
static void prepad(struct out_struct *out_info, struct placeholder_struct* placeholder, size_t source_len)
{
	int pad_len = 0;
	char pad_char = ' ';

	if(is_centered_string(placeholder) && source_len < placeholder->width)
		pad_len = (placeholder->width - source_len)/2;

	// if not left aligned, and the required width is longer than the source length
	else if(!placeholder->flag_minus && placeholder->width > source_len)
		pad_len = placeholder->width - source_len;

	// prepad character of '0' is specified for a numeric type
	if(is_type_numeric(placeholder->type) && placeholder->flag_zero)
		//(the zero flag is ignored if precision is specified for an integer type).
		if(!(placeholder->prec_specified && is_type_int(placeholder->type)))
			pad_char = '0';

	while(pad_len--)
		out(out_info, pad_char);
}

// postpad the output to achieve width, if needed
static void postpad(struct out_struct *out_info, struct placeholder_struct* placeholder, size_t source_len)
{
	int pad_len = 0;

	if(is_centered_string(placeholder) && source_len < placeholder->width)
	{
		pad_len = (placeholder->width - source_len);
		if(pad_len & 1)
			pad_len++;	// round up, prefer larger postpad if unable to center
		pad_len >>=1;
	}

	// if left aligned, and the required width is longer than the source length
	else if(placeholder->flag_minus && placeholder->width > source_len)
		pad_len = placeholder->width - source_len;
	
	while(pad_len--)
		out(out_info, ' ');
}

//string type with .precision of 0?
static bool is_centered_string(struct placeholder_struct* placeholder)
{
	return ((placeholder->type == TYPE_STR || placeholder->type == TYPE_PSTR) && placeholder->width && placeholder->prec_specified && placeholder->prec==0);
}

//return true for only unsigned integer types
static bool is_type_unsigned(uint_least8_t type)
{
	return (type==TYPE_UINT || type==TYPE_HEX || type==TYPE_BIN);
}

//return true for both signed and unsigned integer types
static bool is_type_int(uint_least8_t type)
{
	return (type==TYPE_UINT || type==TYPE_INT || type==TYPE_HEX || type==TYPE_BIN);
}

//return true for any numeric type
static bool is_type_numeric(uint_least8_t type)
{
	return 	(type==TYPE_BIN || type==TYPE_INT || type==TYPE_UINT || type==TYPE_HEX || type==TYPE_FLOAT || type==TYPE_ENG);
}

static bool prnf_is_digit(char x)
{
	return ('0' <=x && x <= '9');
}

static char ascii_hex_digit(uint_least8_t x)
{
	char retval;

	x &= 0x0F;
	if(x < 0x0A)
		retval = '0'+x;
	else
		retval = 'A'+(x-0x0A);
	
	return retval;
}

#ifdef SUPPORT_FLOAT
static struct eng_struct get_eng(float value)
{
	struct eng_struct retval;
	uint8_t i = 8;
	static const char tbl[17] = {'y', 'z', 'a', 'f', 'p', 'n', 'u', 'm', 0, 'k', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y'};

	while(!(-1000.0F < value && value < 1000.0F) && i < 16)
	{
		i++;
		value /= 1000.0F;
	};

	while((-1.0F < value && value < 1.0F) && i)
	{
		i--;
		value *= 1000.0F;
	};

	retval.value = value;
	retval.prefix = tbl[i];
	return retval;
}

static uint_least8_t get_prec(struct placeholder_struct* placeholder)
{
	uint_least8_t prec;

	if(placeholder->prec_specified)
		prec = placeholder->prec;
	else if(placeholder->type == TYPE_ENG)
		prec = ENG_PREC_DEFAULT;
	else
	 	prec = FLOAT_PREC_DEFAULT;

	ASSERT(prec <= FLOAT_PREC_MAX);

	return prec;
}

static unsigned long round_float_to_ulong(float x)
{
	unsigned long retval;

	retval = (unsigned long)x;

	if(x - (float)retval >= 0.5)
		retval++;

	return retval;
}
#endif  //^SUPPORT_FLOAT^

// Per-character output processing
// Counts output characters (regardless of truncation)
// Calls function pointer to destinations character output.
// Truncates output
// Terminates output, (depending on out_info->terminate)
static void out(struct out_struct *out_info, char x)
{
	if(x)
	{
		if(out_info->char_cnt+1 < out_info->size_limit && out_info->dst_fptr)
			out_info->dst_fptr(out_info->dst_fptr_vars, x);
		out_info->char_cnt++;
	}
	else if(out_info->terminate && out_info->dst_fptr && out_info->size_limit)
		out_info->dst_fptr(out_info->dst_fptr_vars, 0);
}

static void out_buf(void* out_buf_vars, char x)
{
	char** dst = (char**)out_buf_vars;
	**dst = x;
	(*dst)++;
}

#endif


// Compile _P version for PROGMEM access
#undef FMTRD
#ifdef PLATFORM_AVR
	#ifdef FIRST_PASS
		#undef FIRST_PASS
		#undef IS_SECOND_PASS
		#define SECOND_PASS
		#define IS_SECOND_PASS true
		#include "prnf.c"
	#endif
#endif
