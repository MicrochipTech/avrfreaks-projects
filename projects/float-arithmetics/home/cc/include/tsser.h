// /home/cc/include/tsser.h

#ifdef AVR
#include <avr/io.h>
#include <avr/pgmspace.h>
#endif


// The device depend base functions
extern void tsseropen( void );		// Prepare the interface
extern void tsserclose( void );		// Switch the interface off
extern bool tsserisicha( void );	// check if a character is typed in
extern uint8_t tsserichaw( void ); // wait until a character is typed in,reutn it.
extern void tsserochaw( char c );		// Output a character

// the device independ composed functions


#ifdef AVR
extern void tsserputs_P( PGM_P p ); // Output / send a string out of program memory
#define tsserputs(x) tsserputs_P( PSTR(x) )
#else
extern void tsserputs( const char *p ); // Output / send a string
#endif

extern void tssernl( void );		// Output / send new line


// read an unsigned char, in hex format, no prompt, no end character
extern char tsseruchar_hex2( uint8_t *p );



// read an unsigned word, in decimale format, e.g. 7FA0
#ifdef AVR
extern char tsseruword_hex_P(
		PGM_P prompt,	// send / display this before reading characters
		uint16_t *p );		// Save the converted hex number here
#define tsserulong_hex(x,y) tsserulong_hex_P(PSTR(x),y)
#else
extern char tsseruword_hex(
		const char *prompt,		// send / display this before reading characters
		uint16_t *p );		// Save the converted hex number here
#endif


// Decimale input and output
// read an unsigned char, in decimale format e.g. 226
#ifdef AVR
extern char tsseruchar_dec_P(
		PGM_P prompt,	// send / display this before reading characters
		uint8_t *p );		// Save the converted hex number here
#define tsseruchar_dec(x,y) tsseruchar_dec_P(PSTR(x),y)
#else
extern char tsseruchar_dec(
		const char *prompt,		// send / display this before reading characters
		uint8_t *p );		// Save the converted hex number here
#endif

// read an unsigned word, in decimale format, e.g. 40200
#ifdef AVR
extern char tsseruword_dec_P(
		PGM_P prompt,	// send / display this before reading characters
		uint16_t *p );		// Save the converted hex number here
#define tsserulong_dec(x,y) tsserulong_dec_P(PSTR(x),y)
#else
extern char tsseruword_dec(
		const char *prompt,		// send / display this before reading characters
		uint16_t *p );		// Save the converted hex number here
#endif


// read an unsigned long, in decimale format, e.g. 1000240
#ifdef AVR
extern char tsserulong_dec_P(
		PGM_P prompt,	// send / display this before reading characters
		uint32_t *p );		// Save the converted hex number here
#define tsseruword_dec(x,y) tsseruword_dec_P(PSTR(x),y)
#else
extern char tsserulong_dec(
		const char *prompt,		// send / display this before reading characters
		uint32_t *p );		// Save the converted hex number here
#endif




// read an ssigned char, in decimale format e.g. -100
#ifdef AVR
extern char tsserschar_dec_P(
		PGM_P prompt,	// send / display this before reading characters
		int8_t *p );		// Save the converted hex number here
#define tsserschar_dec(x,y) tsserschar_dec_P(PSTR(x),y)
#else
extern char tsserschar_dec(
		const char *prompt,		// send / display this before reading characters
		int8_t *p );		// Save the converted hex number here
#endif

// read an signed word, in decimale format, e.g. -16000
#ifdef AVR
extern char tssersword_dec_P(
		PGM_P prompt,	// send / display this before reading characters
		int16_t *p );		// Save the converted hex number here
#define tsserslong_dec(x,y) tsserslong_dec_P(PSTR(x),y)
#else
extern char tssersword_dec(
		const char *prompt,		// send / display this before reading characters
		int16_t *p );		// Save the converted hex number here
#endif


// read an signed long, in decimale format, e.g. -1000240
#ifdef AVR
extern char tsserslong_dec_P(
		PGM_P prompt,	// send / display this before reading characters
		int32_t *p );		// Save the converted hex number here
#define tssersword_dec(x,y) tssersword_dec_P(PSTR(x),y)
#else
extern char tsserslong_dec(
		const char *prompt,		// send / display this before reading characters
		int32_t *p );		// Save the converted hex number here
#endif


// read a float (float), in decimale format e.g. -1.23e-3 or 24.33
#ifdef AVR
extern char tsserfloat_dec_P(
		PGM_P prompt,	// send / display this before reading characters
		float *p );		// Save the converted hex number here
#define tsserfloat_dec(x,y) tsserfloat_dec_P(PSTR(x),y)
#else
extern char tsserfloat_dec(
		const char *prompt,		// send / display this before reading characters
		float *p );		// Save the converted hex number here
#endif





// read a line of text
extern char tssergetline( void );		// Save the string in tsser_charbuffer

extern void tsserhx1_out( uint8_t h );	// Output a single hex digit
extern void tsserhx2_out( uint8_t h );	// Output two hex digits
extern void tsserhx4_out( uint16_t h );	// Output four hex digits

// Display several number data types
extern void tsserdec_fl_out( float h ); // Output float number
extern void tsserdec_uc_out( uint8_t h ); // Output 1 to 3 decimale digits (0...255)
extern void tsserdec_uw_out( uint16_t h ); // Output 1 to 5 decimale digits (0...65535)
extern void tsserdec_ul_out( uint32_t h ); // Output 1 to 10 decimale digits (0...4294967295)

extern void tsserdec_sc_out( int8_t h ); // Output 1 to 3 decimale digits (-128..127)
extern void tsserdec_sw_out( int16_t h ); // Output 1 to 5 decimale digits (-32768...32767)
extern void tsserdec_sl_out( int32_t h ); // Output 1 to 10 decimale digits (-2147483647...2147483648)
#define TSSER_NCHARBUFFER 20
extern char tsser_charbuffer[TSSER_NCHARBUFFER] ;
