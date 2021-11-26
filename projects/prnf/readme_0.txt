 # PRNF
 A lightweight printf implementation.
 With some reasonable limitations, and non-standard behaviour suited to microcontrollers.

  If you need a full featured printf(), I strongly reccomend checking out eyalroz's fork
 of mpaland's printf(), which can be found [here](https://github.com/eyalroz/printf).
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


 [width]
 
 	The minimum number of characters to output.
	Dynamic width using %* is NOT supported. A non-standard method of centering text is provided (see below).


 [.precision]

 	For float %f, this is the number of fractional digits after the '.', valid range is .0 - .8
	For decimal integers, this will prepend 0's (if needed) until the total number of digits equals .precision
	For binary and hex, this specifies the *exact* number of digits to print, default is based on the argument size.
	For strings %s %S, This is the maximum number of characters to read from the source.

	Centering strings: If [width] is specified and precision is .0 %s arguments will centered.
	**Caution - If you are generating formatting strings at runtime, and generate a %[width].0s, you will NOT get 0 characters.
	Dynamic precision using %.* is NOT supported.

 Supported [length]:
 
 	Used to specify the size of the argument. The following is supported:
	hh 		Expect an int-sized argument which was promoted from a char.
	h 		Expect an int-sized argument which was promoted from a short.
	l		Expect a long-sized argument.
	z		Expect a size_t sized argument (size_t must not be larger than long).
	t		Expect a ptrdiff_t sized argument (ptrdiff_t must not be larger than long).

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

.

 # AVR's PROGMEM support:
The symbol PLATFORM_AVR must be defined, either before including prnf.h, or preferably by adding -DPLATFORM_AVR to compiler options.
prnf.c will #include itself for a second pass, to build the _P versions of the functions.
If you wish to write code which is cross compatible with AVR and non-AVR, use the _SL (String Literal) macros provided,
these will place string literals in PROGMEM for AVR targets and produce normal string literals for 'normal' von-newman targets.


On AVR, both the the format sring, and string arguments, may be in either ram or program memory. See the header for more detailed useage.

.

# Replacing printf()

If you intend to override printf() this can be done with macros. Be aware that after you define 'printf' you will break anything which tries to add the format checking attribute __attribute__((format(printf, 1, 2)))

Another approach is to use GCC's --wrap feature in the compiler flags, which is probably better.

.

# Example debug macro:
The following is useful for debug/diagnostic and cross platform friendly with AVR:

	#define DBG(_fmtarg, ...) prnf_SL("%S:%.4i - "_fmtarg , PRNF_ARG_SL(__FILE__), __LINE__ ,##__VA_ARGS__)

Example usage:

	DBG("value is %i\n", 51);
The above will output something like "main.c:0113 - value is 51"

Note that even if you use this on many lines, the __FILE__ string literal will only occur once in the string pool.

.

# Adding prnf() functionality to other IO modules:

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
.

# snappf()
Safely APPEND to a string in a buffer of known size.

	int snappf(char* dst, size_t dst_size, const char* fmtstr, ...);

In addition to the regular snprnf(), which is used for safely printing to a buffer of n bytes. snappf() can safely append to the buffer.
This is useful in situations where you may need to iterate through a number of fields in a loop. The return value is the number of characters appended (ignoring truncation).


