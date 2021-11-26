/*

 A lightweight printf implementation.
 With some reasonable limitations, and non-standard behaviour suited to microcontrollers.

  If you need a full featured printf(), I strongly reccomend checking out eyalroz's fork
 of mpaland's printf(), which can be found here https://github.com/eyalroz/printf. 
 After some involvement in that project, I realized it would never quite be exactly what I wanted.
 So I wrote this. Some of the ideas used in this are from the authors of that project.
 Particularly the function pointer output from mpaland, and the output wrapping used in this was suggested by eyalroz.
 
 * Thread and re-enterant safe.
 * Low stack & ram usage, zero heap usage, no text buffers needed for numeric output.
 * Full support for AVR's PROGMEM requirements, with almost no cost to non-AVR targets.
 * Compatible enough to make use of GCC's format and argument checking (even for AVR).
 * no double or 64bit arithmetic, (doubles get demoted to float internally).
 
 * NO exponential form, %e provides SI units (y z a f p n u m - k M G T P E Z Y).
 * NO Octal, %o outputs binary instead, (who wants octal?)
 * NO adaptive %g %G
 * NO support for long long 
 
 Build size on avr with -Os is ~5kb with float support, and ~3kb without float

 Standard placeholder syntax:

 	%[flags][width][.precision][length]type


 Supported [flags]:
	- 			left align the output
	+ 			prepends a + for positive numeric types
	(space)		prepends a space for positive numeric type
	0 			prepends 0's instead of spaces to numeric types to satisfy [width]

 Unsupported [flags]:
	# 				If you want 0x it needs to be in your format string.
	' (apostrophe)	No 1000's separator is available

 [width] 		The minimum number of characters to output.
				Dynamic width using %* is NOT supported, but a non-standard method of centering text is provided (see below).
				This has no effect on the single character %c type.

 [.precision]	For float %f, this is the number of fractional digits after the '.', valid range is .0 - .8
				For decimal integers, this will prepend 0's (if needed) until the total number of digits equals .precision
				For binary and hex, this specifies the *exact* number of digits to print, default is based on the argument size.
				For strings %s %S, This is the maximum number of characters to read from the source.
				Centering strings: If [width] is specified and precision is .0 %s arguments will centered.
				**Caution - If you are generating formatting strings at runtime, and generate a %[width].0s, you will NOT get 0 characters.
				Dynamic precision using %.* is NOT supported.
				

 [length]		Used to specify the size of the argument. The following is supported:
	hh 			Expect an int-sized argument which was promoted from a char.
	h 			Expect an int-sized argument which was promoted from a short.
	l			Expect a long-sized argument.
	z			Expect a size_t sized argument (size_t must not be larger than long).
	t			Expect a ptrdiff_t sized argument (ptrdiff_t must not be larger than long).

 Unsupported [length]:
	ll			(long long) not supported
	j			intmax_t not supported

 Supported types:
  	d,i			Signed decimal integer
  	u			Unsigned decimal integer
  	x,X  		Hexadecimal. Always uppercase, .precision defaults to argument size [length]
  	o			NOT Octal. Actually binary, .precision defaults to argument size [length]
  	s			null-terminated string in ram, or NULL. Outputs nothing for NULL.
	S			For AVR targets, read string from PROGMEM, otherwise same as %s
  	c			character 

	f,F			Floating point (not double). NAN & INF are always uppercase.
				Default precision is 3 (not 6).
				Digits printed must be able to be represented by an unsigned long,
				ei. with a precision of 3, maximum range is +/- 4294967.296 
				Values outside this range will produce "OVER".
				A value of 0.0 is always positive. 

	e 			NOT exponential. Floating point with engineering notaion (y z a f p n u m - k M G T P E Z Y).
				Number is postpended with the SI prefix. Default precision is 0.
 
 Unsupported types:
	g,G 		Adaptive floats not available
	a,A			Double in hex notation not available
	p			Pointer not available
	n			not available


 Replacing printf()

 	 If you intend to override printf() this can be done with macros. Be aware that after you define 'printf'
 	you will break anything which tries to add the format checking attribute __attribute__((format(printf, 1, 2)))
 	Another approach is to use GCC's --wrap feature in the compiler flags, which is probably better.


 Example debug macro:
	The following is useful for debug/diagnostic and cross platform friendly with AVR:

	#define DBG(_fmtarg, ...) prnf_SL("%S:%.4i - "_fmtarg , PRNF_ARG_SL(__FILE__), __LINE__ ,##__VA_ARGS__)

	Example usage:
		DBG("value is %i\n", 51);  will output something like "main.c:0113 - value is 51"
	
	Note that even if you use this on many lines, the __FILE__ string literal will only occur once in the string pool.


 Adding prnf() functionality to other IO modules:

	 This can be achieved by writing your own character handler, and variadic function for the module.
	 An example for lcd_prnf() may look something like:

	// LCD prnf character handler
	static void prnf_write_char(void* nothing, char x)
	{
		lcd_data(x);
	}

	// formatted print to LCD
	void lcd_prnf(const char* fmt, ...)
	{
		va_list va;
		va_start(va, fmt);
		vfptrprnf(prnf_write_char, NULL, fmt, va);
		va_end(va);
	}

*/

#ifndef _PRNF_H_
#define _PRNF_H_

//********************************************************************************************************
// Public defines
//********************************************************************************************************

/*
	PLATFORM_AVR should be defined for AVR targets.

	If you have modules that you wish to compile for both AVR and non-AVR targets, you can use the _SL (String Literal) macro wrappers.
	These will put string literals in PROGMEM for AVR targets only.
	The argument list will still be tested using a duiplicate format string in ram,
	 but optimisation (any level) must be enabled to remove the ram duplicate.

	PRNF_ARG_SL() is for passing string literals as arguments to %S (upper case) placeholders.

	Arguments can safely use operators ie. prnf_SL("%i", i++);

	The following two examples will put all string literals in PROGMEM for AVR targets, and ram for non-avr targets.
		prnf_SL("%-50\n", PRNF_ARG_SL("LEFT"));
		prnf_SL("%50\n", PRNF_ARG_SL("RIGHT"));
*/

#ifdef PLATFORM_AVR
//	Compiler will first test argument types based on format string, then remove the empty function during optimization.
	static inline void fmttst_optout(const char* fmt, ...) __attribute__((format(printf, 1, 2)));
	static inline void fmttst_optout(const char* fmt, ...)
	{
	}

//	_SL macros for AVR
	#define prnf_SL(_fmtarg, ...) 						({int _prv; _prv = prnf_P(PSTR(_fmtarg) ,##__VA_ARGS__); while(0) fmttst_optout(_fmtarg ,##__VA_ARGS__); _prv;})
	#define sprnf_SL(_dst, _fmtarg, ...) 				({int _prv; _prv = sprnf_P(_dst, PSTR(_fmtarg) ,##__VA_ARGS__); while(0) fmttst_optout(_fmtarg ,##__VA_ARGS__); _prv;})
	#define snprnf_SL(_dst, _dst_size, _fmtarg, ...) 	({int _prv; _prv = snprnf_P(_dst, _dst_size, PSTR(_fmtarg) ,##__VA_ARGS__); while(0) fmttst_optout(_fmtarg ,##__VA_ARGS__); _prv;})
	#define snappf_SL(_dst, _dst_size, _fmtarg, ...) 	({int _prv; _prv = snappf_P(_dst, _dst_size, PSTR(_fmtarg) ,##__VA_ARGS__); while(0) fmttst_optout(_fmtarg ,##__VA_ARGS__); _prv;})
	#define fptrprnf_SL(_fptr, _fargs, _fmtarg, ...) 	({int _prv; _prv = fctprnf_P(_fptr, _fargs, PSTR(_fmtarg) ,##__VA_ARGS__); while(0) fmttst_optout(_fmtarg ,##__VA_ARGS__); _prv;})
	#define PRNF_ARG_SL(_arg)							((wchar_t*)PSTR(_arg))
#else
	#define prnf_SL(_fmtarg, ...) 						prnf(_fmtarg ,##__VA_ARGS__)
	#define sprnf_SL(_dst, _fmtarg, ...) 				sprnf(_dst, _fmtarg ,##__VA_ARGS__)
	#define snprnf_SL(_dst, _dst_size, _fmtarg, ...) 	snprnf(_dst, _dst_size, _fmtarg ,##__VA_ARGS__)
	#define snappf_SL(_dst, _dst_size, _fmtarg, ...) 	snappf(_dst, _dst_size, _fmtarg ,##__VA_ARGS__)
	#define fptrprnf_SL(_fptr, _fargs, _fmtarg, ...)	fctprnf(_fptr, _fargs, _fmtarg ,##__VA_ARGS__)
	#define PRNF_ARG_SL(_arg)							((wchar_t*)(_arg))
#endif

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//	Point this at applications character handler for prnf(). The void* parameter is always passed NULL.
//	This function pointer is not called for terminating null characters.
//	It may be safely be assigned NULL, if no output from prnf() is required.
	extern void(*prnf_out_fptr)(void*, char);

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

//	Print, sending characters to prnf_out_fptr() (see above).
	int prnf(const char* fmtstr, ...) __attribute__((format(printf, 1, 2)));

//	Print to a char* buffer, with no size limit.
	int sprnf(char* dst, const char* fmtstr, ...) __attribute__((format(printf, 2, 3)));

//	Print safely to a char[] buffer of known size.
	int snprnf(char* dst, size_t dst_size, const char* fmtstr, ...) __attribute__((format(printf, 3, 4)));

//	*Append* safely to a char[] buffer of known size, returns the number of characters appended (ignoring truncation).
	int snappf(char* dst, size_t dst_size, const char* fmtstr, ...) __attribute__((format(printf, 3, 4)));

//	Print. Sending characters to specified character handler, not including a terminating null.
//	The character handler may be NULL if no output is required.
//	void* out_vars is also passed to the void* parameter if the character handler.
	int fptrprnf(void(*out_fptr)(void*, char), void* out_vars, const char* fmtstr, ...) __attribute__((format(printf, 3, 4)));


//	non-variadic versions of the above, accepting va_list
//	The variadic functions above are quite small and call these. 
//	If you want to implement prnf() functionality in other modules,
//	 like lcd_prnf() or uart_prnf() you can write your own variadic functions which call these
	int vprnf(const char* fmtstr, va_list va);
	int vsprnf(char* dst, const char* fmtstr, va_list va);
	int vsnprnf(char* dst, size_t dst_size, const char* fmtstr, va_list va);
    int vsnappf(char* dst, size_t dst_size, const char* fmtstr, va_list va);
	int vfptrprnf(void(*out_fptr)(void*, char), void* out_vars, const char* fmtstr, va_list va);

#ifdef PLATFORM_AVR
	int prnf_P(const char* fmtstr, ...);
	int sprnf_P(char* dst, const char* fmtstr, ...);
	int snprnf_P(char* dst, size_t dst_size, const char* fmtstr, ...);
	int snappf_P(char* dst, size_t dst_size, const char* fmtstr, ...);
	int fptrprnf_P(void(*out_fptr)(void*, char), void* out_vars, const char* fmtstr, ...);
	int vprnf_P(const char* fmtstr, va_list va);
	int vsprnf_P(char* dst, const char* fmtstr, va_list va);
	int vsnprnf_P(char* dst, size_t dst_size, const char* fmtstr, va_list va);
    int vsnappf_P(char* dst, size_t dst_size, const char* fmtstr, va_list va);
	int vfptrprnf_P(void(*out_fptr)(void*, char), void* out_vars, const char* fmtstr, va_list va);
#endif

#endif // _PRNF_H_
