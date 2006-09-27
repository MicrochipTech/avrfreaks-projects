/*
 * MICRO-BASIC:
 *
 * This is a small INTEGER BASIC interpreter that I wrote a number
 * of years ago, and subsequently ported to MICRO-C. While not a great
 * example of coding style (it was a quick and dirty hack job), It is
 * quite instructive, as a simple but fairly complete interpreter.
 *
 * Variables:
 *	260 Numeric	  variables	:	A0-A9 ... Z0-Z9
 *	260 Character variables	:	A0$-A9$ ... Z0$-Z9$
 *	260 Numeric arrays		:	A0()-A9() ... Z0()-Z9()
 *
 *	For convenience the '0' variables can be referenced by letter
 *	only. IE: A is equivalent to A0 ... Z$ is equivalent to Z0$
 *
 * Statements:
 *	BEEP freq,ms			- Generate a BEEP on the PC speaker
 *	CLEAR					- Erase variables only
 *	CLOSE#n					- Close file (0-9) opened with OPEN
 *  CLS						- Clear screen and return to position 0
 *	DATA					- Enter "inline" data statements
 *	DDRSET "PORTx", valeu	- Configures DDR settings for PORTx
 *	DELAY ms				- Stops for the indicated time
 *	DIM var(size)[, ... ]	- Dimension an array
 *	DOS "comand"			- Execute a DOS program
 *	END						- Terminate program with no message
 *	EXIT					- Terminate MICRO-BASIC
 *	FOR v=init TO limit [STEP increment] - Perform a counted loop
 *	GOSUB line				- Call a subroutine
 *	GOTO  line				- Jump to line
 *	IF test THEN line		- Conditional goto
 *	IF test THEN statement	- Conditional statement (next statement only)
 *	INPUT var				- Get value for variable
 *	INPUT "prompt",var		- Get value of variable with prompt
 *		prompt must be a constant string, however you can use a char variable
 *		in prompt by concatinating it to such a string: INPUT ""+a$,b$
 *	INPUT#n,var				- Get value for variable from file (0-9)
 *	LET (default)			- variable = expression
 *	LIF test THEN statements- LONG IF (all statements to end of line)
 *	LIST [start,[end]]		- List program lines
 *	LIST#n ...				- List program to file (0-9)
 *	LOAD "name"				- Load program from disk file
 *		When LOAD is used within a program, execution continues with the
 *		first line of the newly loaded program. In this case, the user
 *		variables are NOT cleared. This provides a means of chaining
 *		to a new program, and passing information to it.
 *		Also note that LOAD must be the LAST statement on a line.
 *	NEW						- Erase program and variables
 *	NEXT [v]				- End counted loop
 *	OPEN#n,"name","opts"	- Open file (0-9), opts are same as "fopen()"
 *	ORDER line				- Position data read pointer
 *	OUT port,expr			- Write to I/O port
 *	PRINT expr[,expr ...]	- Print to console
 *	PRINT#n,...				- Print to file (0-9)
 *	READ var[,var ...]		- Read data from program statements
 *		You MUST issue an "ORDER" statement targeting a line
 *		containing a valid DATA statement before using READ
 *	RETURN					- Return from subroutine
 *	REM						- Comment... reminder of line is ignored
 *	RUN [line]				- Run program
 *	SAVE ["name"]			- Save program to disk file
 *	STOP					- Terminate program & issue message
 *
 * Operators:
 *	+						- Addition, string concatination
 *	-						- Unary minus, subtraction
 *	*, /, %,				- multiplication, division, modulus
 *	&, |, ^					- AND, OR, Exclusive OR
 *	=, <>					- Assign/test equal, test NOTequal (num or string)
 *	<, <=, >, >=			- LT, LE, GT, GE (numbers only)
 *	!						- Unary NOT
 *		The "test" operators (=, <>, <, <=, >, >=) can be used in any
 *		expression, and evaluate to 1 of the test is TRUE, and 0 if it
 *		is FALSE. The IF and LIF commands accept any non-zero value to
 *		indicate a TRUE condition.
 *
 * Functions:
 *	CHR$(value)				- Returns character of passed value
 *	STR$(value)				- Returns ASCII string of value's digits
 *	ASC(char)				- Returns value of passed character
 *	NUM(string)				- Convert string to number
 *	ABS(value)				- Returns absolute value of argument
 *	RND(value)				- Returns random number from 0 to (value-1)
 *	KEY()					- Test for key from keyboard
 *	IN(port)				- Read an I/O port
 *
 * Copyright 1982-2000 Dave Dunfield
 * All rights reserved.
 *
 * WinAVR Port by Max Oliver 2006
 *
 * Permission granted for personal (non-commercial) use only.
 *
 * Compile command: cc basic -fop
 */
#include <stdio.h>
#include <setjmp.h>
#include <ctype.h>
#include <stdlib.h> 
#include <string.h> 

// AVR Libraries INCLUDES
#include <avr/io.h>			// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <avr/portpins.h>
#include <avr/version.h>   // 'libc'

#if defined (__AVR_ATmega128__)
#include "uart2.h"
#else
#include "uart.h"
#endif

#include "a2d.h"		// include A/D converter function library
#include "rprintf.h"
#include "vt100.h"
#include "param.h"


// AVR Utilities Libraries INCLUDES for Timing DELAYs
#include <util/delay.h>
#define DelayMicroseconds(us) _delay_us(us)
#define DelayMilliseconds(ms) _delay_ms(ms)

#define SetPortBit(port,bit)     (port |=  (1<<bit))  // set 'bit' in 'port'
#define ClearPortBit(port,bit)   (port &= ~(1<<bit))  // clear 'bit' in 'port'

u08 *eeprom_store = 0;

//static int uart_putchar(char c, FILE *stream);
//char uart_getchar(FILE *stream);

static int put_char(char c, FILE *stream)
{

#if defined (__AVR_ATmega128__)
   uart1SendByte(c);
#else
   uartSendByte(c);
#endif
   return 0;
} 

static int get_char(char c, FILE *stream) {

}

static FILE mystdout = FDEV_SETUP_STREAM(put_char, get_char,
                                             _FDEV_SETUP_RW);

void init_uart(unsigned char baudrate) {
	uartInit();

#if defined (__AVR_ATmega128__)
	uartSetBaudRate(0,9600);
	uartSetBaudRate(1,9600);
#else
	uartSetBaudRate(9600);
#endif

}

/* Fixed parameters */
#define BUFFER_SIZE 100		/* input buffer size */
#define NUM_VAR 	64		/* number of variables */
#define SA_SIZE 	64		/* string accumulator size */

/* Control stack constant identifiers */
#define _FOR		1000	/* indicate FOR statement */
#define _GOSUB		_FOR+1	/* indicate GOSUB statement */

/* Primary keywords */
#define	LET		1
#define	EXIT	2
#define	LIST	3
#define	NEW		4
#define	RUN		5
#define	CLEAR	6
#define	GOSUB	7
#define	GOTO	8
#define	RETURN	9
#define	PRINT	10
#define	FOR		11
#define	NEXT	12
#define	IF		13
#define	LIF		14
#define	REM		15
#define	STOP	16
#define	END		17
#define	INPUT	18
#define	OPEN	19
#define	CLOSE	20
#define	DIM		21
#define	ORDER	22
#define	READ	23
#define	DATA	24
#define	SAVE	25
#define	LOAD	26
#define	DELAY	27
#define	BEEP	28
#define	DOS		29
#define	OUT		30

/* Secondary keywords */
#define	TO		31	/* Also used as marker */
#define	STEP	32
#define	THEN	33
#define DDRSET	34
#define CLS		35
#define ADCSET	36

/* Operators and functions */
#define	ADD		37	/* Also used as marker */
#define	SUB		38
#define	MUL		39
#define	DIV		40
#define	MOD		41
#define	AND		42
#define	OR		43
#define	XOR		44
#define	EQ		45
#define	NE		46
#define	LE		47
#define	LT		48
#define	GE		49
#define	GT		50
#define	CHR		51
#define	STR		52
#define	ASC		53
#define	FABS	54
#define	NUM		55
#define	RND		56
#define	KEY		57
#define	IN		58
#define ADCREAD	59

#define	RUN1	99

int chk_file(char flag);
int do_arith(char opr, unsigned op1, unsigned op2);

char		skip_blank(void);
char		get_next(void);
void		expect(int token);
void		error(unsigned int en);
unsigned	get_num(void);
void		delete_line(unsigned lino);
void		insert_line(unsigned lino);
int			edit_program(void);
unsigned int *address(void);
int			get_var( void );
int			eval_sub(void);
int			eval(void);
void		disp_pgm(FILE *fp, unsigned i, unsigned j);
void		clear_vars(void);
void		clear_pgm(void);
void		direct_only(void);
int			eval_num(void);
void		pgm_only(void);
void		skip_stmt(void);
void		num_string(unsigned value, char *ptr);
void		eval_char(void);
int			get_value(void);
void		get_char_value(char *ptr);

char beep( int i, int t );
char delay( int d );
char *concat(char *filename, char *sa1, char *suf);
char kbtst( void );
void cls(void);
void adcset(char prescaler, char reference);

int fopen(char *c, char *p);

int fopen(char *c, char *p){ 
	return 0;
	}

char beep( int i, int t ){


	long j;

//	printf("i = %u   t = %u\r\n",i,t);
	DDRE |= 1<<PE4;			//make PB0 an output
	DDRE |= 1<<PE5;			//make PB0 an output

	while (i > 0) {
		
		PORTE |= 1<<PE4;		// buzzer pin high
		PORTE &= !(1<<PE5);		// buzzer pin low
		
		for(j=0;j<2000;j++);
	
		PORTE |= 1<<PE5;		// buzzer pin high
		PORTE &= !(1<<PE4);		// buzzer pin low
		
		for(j=0;j<2000;j++);
		i--;
	}

	return 0;

	}

void adcset(char prescaler, char reference) {
	
	a2dSetPrescaler(prescaler);
	a2dSetReference(reference);

}

char readadc(char channel) {
//	printf("channel = %u\r\n", channel);
	return a2dConvert10bit(channel);
}

void cls(void){
	vt100ClearScreen();
	vt100SetCursorPos(0,0);
}

char delay_bas(int d){

//	DelayMilliseconds(d);

	return 0;
}

char *concat(char *filename, char *sa1, char *suf){
	return 0;
}

char kbtst(){
	return 0;
}



/* Make sure this token table matches the above definitions */
static char *reserved_words[] = {
	"LET", "EXIT", "LIST", "NEW", "RUN", "CLEAR", "GOSUB", "GOTO",
	"RETURN", "PRINT", "FOR", "NEXT", "IF", "LIF", "REM", "STOP",
	"END", "INPUT", "OPEN", "CLOSE", "DIM", "ORDER", "READ", "DATA",
	"SAVE", "LOAD", "DELAY", "BEEP", "DOS", "OUT",
	"TO", "STEP", "THEN", "DDRSET", "CLS", "ADCSET",
	"+", "-", "*", "/", "%", "&", "|", "^",
	"=", "<>", "<=", "<", ">=", ">",
	"CHR$(", "STR$(", "ASC(", "ABS(", "NUM(", "RND(", "KEY(", "IN(", "ADCREAD(", 0 };

static char *PORTS[] = {
	"PORTA", "PORTB", "PORTC", "PORTD", "PORTE", "PORTF", "PORTG", 0
};

static char *DDRS[] = {
	"DDRA", "DDRB", "DDRC", "DDRD", "DDRE", "DDRF", "DDRG", 0
};

static char *PINS[] = {
	"PINA", "PINB", "PINC", "PIND", "PINE", "PINF", "PING", 0
};
/* Table of operator priorities */
static char priority[] = { 0, 1, 1, 2, 2, 2, 3, 3, 3, 1, 1, 1, 1, 1, 1 };

/* Table of error messages */
static char *error_messages[] = {
	"Syntax",
	"Illegal program",
	"Illegal direct",
	"Line number",
	"Wrong type",
	"Divide by zero",
	"Nesting",
	"File not open",
	"File already open",
	"Input",
	"Dimension",
	"Data",
	"Out of memory"
	};


struct line_record {
	unsigned Lnumber;
	struct line_record *Llink;
	char Ltext[]; };

char sa1[SA_SIZE], sa2[SA_SIZE];		/* String accumulators */
struct line_record *pgm_start,			/* Indicates start of program */
	*runptr,							/* Line we are RUNnning */
	*readptr;							/* Line we are READing */

unsigned dim_check[NUM_VAR];			/* Check dim sizes for arrays */

FILE *filein, *fileout;					/* File I/O active pointers */

jmp_buf savjmp;							/* Save area for set/longjmp */

/* Misc. global variables */
char *cmdptr,							/* Command line parse pointer */
	*dataptr,							/* Read data pointer */
	buffer[BUFFER_SIZE],				/* General input buffer */
	mode = 0,							/* 0=Stopped, !0=Running */
	expr_type,							/* Type of last expression */
	nest;								/* Nest level of expr. parser */
unsigned line,							/* Current line number */
	ctl_ptr = 0,						/* Control stack pointer */
	ctl_stk[50];						/* Control stack */

/*
 * The following variables must be iniitialized to zero. If your
 * compiler does not automatically zero uninitialized global
 * variables, modify these declarations to initialize them.
 */
char filename[10];						/* Name of program file */
FILE *files[2];							/* File unit numbers */
int num_vars[NUM_VAR];					/* Numeric variables */
int *dim_vars[NUM_VAR];					/* Dimensioned arrays */
char *char_vars[NUM_VAR];				/* Character variables */

unsigned char in(unsigned char *p);
char out(unsigned char *p, unsigned char data);
char ddrset(unsigned char *p, unsigned char data);

unsigned char in(unsigned char *p){
	
int i;
unsigned char data;

	data = 0;

	for(i = 0; i<=6; i++) {
		if(!strcmp(p,PINS[i]))
			break;
	}

//	printf("p = %s  data = %u\r\n",p,data);

	switch(i) {

	#ifdef PINA
	case 0:
		data = PINA;
	#endif 

	#ifdef PINB
	case 1:
		data = PINB;
	#endif 

	#ifdef PINC
	case 2:
		data = PINC;
	#endif 

	#ifdef PIND
	case 3:
		data = PIND;
	#endif 

	#ifdef PINE
	case 4:
		data = PINE;
	#endif 

	#ifdef PINF
	case 5:
		data = PINF;
	#endif 

	#ifdef PING
	case 6:
		data = PING;
	#endif 

	}

	return data;
}

char out(unsigned char *p, unsigned char data){
int i;

	for(i = 0; i<=6; i++) {
		if(!strcmp(p,PORTS[i]))
			break;
	}

//	printf("p = %s  data = %u\r\n",p,data);

	switch(i) {

	#ifdef PORTA
	case 0:
		PORTA = data;
	#endif 

	#ifdef PORTB
	case 1:
		PORTB = data;
	#endif 

	#ifdef PORTC
	case 2:
		PORTC = data;
	#endif 

	#ifdef PORTD
	case 3:
		PORTD = data;
	#endif 

	#ifdef PORTE
	case 4:
		PORTE = data;
	#endif 

	#ifdef PORTF
	case 5:
		PORTF = data;
	#endif 

	#ifdef PORTG
	case 6:
		PORTG = data;
	#endif 

	}


	return 0;
}

char ddrset(unsigned char *p, unsigned char data){
int i;

	for(i = 0; i<=6; i++) {
		if(!strcmp(p,DDRS[i]))
			break;
	}

//	printf("p = %s  data = %u\r\n",p,data);

	switch(i) {
	
	#ifdef DDRA
	case 0:
		DDRA = data;
	#endif

	#ifdef DDRB
	case 1:
		DDRB = data;
	#endif

	#ifdef DDRC
	case 2:
		DDRC = data;
	#endif

	#ifdef DDRD
	case 3:
		DDRD = data;
	#endif

	#ifdef DDRE
	case 4:
		DDRE = data;
	#endif

	#ifdef DDRF
	case 5:
		DDRF = data;
	#endif

	#ifdef DDRG
	case 6:
		DDRG = data;
	#endif

	}

	return 0;
}

/*
 * Test for end of expression
 */
int is_e_end(char c)
{
	if((c >= (-128+TO)) && (c < (-128+ADD)))
		return(1);
	return (c == '\0') || (c == ':') || (c == ')') || (c == ',');
}

/*
 * Test for end of statement
 */
int is_l_end(char c)
{
	return (c == '\0') || (c == ':');
}

/*
 * Test for terminator character
 */
int isterm(char c)
{
	return (c == ' ') || (c == '\t');
}

/*
 * Advance to next non-blank & retreive data
 */
char skip_blank()
{
	while(isterm(*cmdptr))
		++cmdptr;
	return *cmdptr;
}

/*
 * Advance to., retrieve and skip next non blank
 */
char get_next()
{
	char c;

	while(isterm(c=*cmdptr))
		++cmdptr;
	if(c)
		++cmdptr;
	return c;
}

/*
 * Test for a specific character occuring next & remove if found
 */
int test_next(int token)
{
	if(skip_blank() == token) {
		++cmdptr;
		return -1; }
	return 0;
}

/*
 * Expect a specific token - syntax error if not found
 */
void expect(int token)
{
	if(get_next() != token)
		error(0);
}

/*
 * Lookup up word from command line in table
 */
unsigned lookup(char *table[])
{
	unsigned i;
	char *cptr, *optr;

	optr = cmdptr;
	for(i=0; (cptr = table[i]); ++i) {
		while((*cptr) && (*cptr == toupper(*cmdptr))) {
			++cptr;
			++cmdptr; }
		if(!*cptr) {
			if(! (isalnum(*(cptr-1)) && isalnum(*cmdptr)) ) {
				skip_blank();
				return i+1; } }
		cmdptr = optr; }
	return 0;
}

/*
 * Get a number from the input buffer
 */
unsigned get_num()
{
	unsigned value;
	char c;

	value = 0;
	while(isdigit(c=*cmdptr)) {
		++cmdptr;
		value = (value * 10) + (c - '0'); }
	return value;
}

/*
 * Allocate memory and zero it
 */
char *allocate(unsigned size)
{
	char *ptr;
	if(!(ptr = malloc(size)))
		error(12);
	memset(ptr, 0, size);
	return ptr;
}

/*
 * Delete a line from the program
 */
void delete_line(unsigned lino)
{
	struct line_record *cptr, *bptr;
	
	bptr = (struct line_record *)malloc(sizeof(struct line_record));

	{
	};
	if(!(cptr = pgm_start))					/* no lines in pgm */
		return;
	do {
		if(lino == cptr->Lnumber) {			/* we have line to delete */
			if(cptr == pgm_start) {			/* first line in pgm */	
				pgm_start = cptr->Llink;
				return; }
			else {
				bptr->Llink = cptr->Llink;	/* skip it in linked list */
				free((void*)cptr); } }				/* let it go */
		bptr = cptr; }
	while((cptr = cptr->Llink));
}

/*
 * Insert a line into the program
 */
void insert_line(unsigned lino)
{
	unsigned i;
	struct line_record *cptr, *bptr, *optr;
	char *ptr;

	ptr = cmdptr;
	for(i=5; *ptr; ++i)
		++ptr;
	bptr = (struct line_record *)allocate(i);
	bptr->Lnumber = lino;
	for(i=0; *cmdptr; ++i)
		bptr->Ltext[i] = *cmdptr++;
	bptr->Ltext[i] = 0;
	if((!(cptr = pgm_start)) || (lino < cptr->Lnumber)) {	/* at start */
		bptr->Llink = pgm_start;
		pgm_start = bptr; }
	else {				/* inserting into main part of pgm */
		for(;;) {
			optr = cptr;
			if((!(cptr = cptr->Llink)) || (lino < cptr->Lnumber)) {
				bptr->Llink = optr->Llink;
				optr->Llink = bptr;
				break; } } }
}

/*
 * Tokenize input line and Add/Replace a source line if suitable
 */
int edit_program(void)
{
	unsigned value;
	char *ptr, c;

	cmdptr = ptr = buffer;
	/* Translate special tokens into codes */
	while((c = *cmdptr)) {
		if((value = lookup(reserved_words)))
			*ptr++ = value | 0x80;
		else {
			*ptr++ = c;
			++cmdptr;
			if(c == '"') {		/* double quote */
				while((c = *cmdptr) && (c != '"')) {
					++cmdptr;
					*ptr++ = c; }
				*ptr++ = *cmdptr++; } } }
	*ptr = 0;
	cmdptr = buffer;

	if(isdigit(skip_blank())) {	/* Valid source line */
		value = get_num();		/* Get line number */
		delete_line(value);		/* Delete the old */
		if(skip_blank())
			insert_line(value);
		return -1; }			/* Insert the new */
	return 0;
}

/*
 * Locate given line in source
 */
struct line_record *find_line(unsigned line)
{
	struct line_record *cptr;

	for(cptr = pgm_start; cptr; cptr = cptr->Llink)
		if(cptr->Lnumber == line)
			return cptr;
	error(3);

	return (struct line_record*)0;
}

/*
 * Compute variable address for assignment
 */
unsigned int *address(void)
{
	unsigned int i, j, *dptr;
	i = get_var();
	if(expr_type)
		return (unsigned int *)&char_vars[i];
	else {
		if(test_next('(')) {	/* Array */
			if(expr_type) error(0);
			if(!(dptr = dim_vars[i]))
				error(10);
			nest = 0;
			if((j = eval_sub()) >= dim_check[i])
				error(10);
			return &dptr[j]; } }
	return &num_vars[i];
}

/*
 * Execute a BASIC commands
 */
struct line_record *execute(char cmd)
{
	unsigned i, j, k, *dptr;
	int ii, jj;
	struct line_record *cptr;
	char c;
	char *tmpbuffer;

	int kb_c;

	switch(cmd & 0x7F) {
		case LET :
			dptr = address();
			j = expr_type;

			expect(-128+EQ);

			k = eval();

			if(j != expr_type)
				error(0);
			if(!expr_type)		/* numeric assignment */
				*dptr = k;
			else {				/* character assignment */
				if(*dptr)
					free((void*)*dptr);
				if(*sa1) {
					*dptr = (unsigned )allocate(strlen(sa1)+1);
					strcpy((char *)*dptr, sa1);
				}
				else
					*dptr = 0; }
			break;
		case EXIT :
			exit(0);
		case LIST :
			chk_file(1);
			if(!isdigit(skip_blank())) {
				i=0; j=-1; }
			else {
				i = get_num();
				if(get_next() == ',') {
					if(isdigit(skip_blank()))
						j=get_num();
					else
						j = -1; }
				else
					j=i; }
			disp_pgm(fileout,i,j);
			break;
		case NEW :
			clear_vars();
			clear_pgm();
			longjmp(savjmp, 1);
		case RUN :
			direct_only();
			clear_vars();
		case RUN1 :		/* No clearing */
			if(is_e_end(skip_blank()))
				runptr = pgm_start;
			else
				runptr = find_line(eval_num());
			--mode;			/* indicate running */
newline:
			while(runptr) {
				cmdptr = runptr->Ltext;
				line = runptr->Lnumber;
				do {
					#if defined (__AVR_ATmega128__)
					kb_c = uart1GetByte();
					#else
					kb_c = uartGetByte();
					#endif
				
					if(kb_c == 3){
				
						goto stop;
					}

					if((cmd = skip_blank()) < 0) {
						++cmdptr;
						if((dptr=(unsigned *)execute(cmd))) {
							runptr = (struct line_record *)dptr;
							goto newline; } }
					else
						execute(1); }
				while((c = get_next()) == ':');
				if(c)
					error(0);
				runptr = runptr->Llink; 
			}//%%%
			mode = 0;
			break;
		case CLEAR :
			clear_vars();
			break;
		case CLS :
			cls();
			break;
		case GOSUB :
			ctl_stk[ctl_ptr++] = (unsigned)runptr;
			ctl_stk[ctl_ptr++] = (unsigned)cmdptr;
			ctl_stk[ctl_ptr++] = (unsigned)_GOSUB;
		case GOTO :
			pgm_only();
			return find_line(eval_num());
		case RETURN :
			pgm_only();
			if(ctl_stk[--ctl_ptr] != _GOSUB)
				error(6);
			cmdptr = (char *)ctl_stk[--ctl_ptr];
			runptr = (struct line_record *)ctl_stk[--ctl_ptr];
			line = runptr->Lnumber;
			skip_stmt();
			break;
		case PRINT :
			chk_file(1);
			j = 0;
			do {
				if(is_l_end(skip_blank()))
					--j;
				else {
					i = eval();
					if(!expr_type) {
						num_string(i, sa1);
						putc(' ',fileout); 
					}
					fputs(sa1, fileout); 
				} 
			}
			while(test_next(','));
			if(!j)
				fputs("\r\n", fileout)
			;
			break;
		case FOR :
			pgm_only();
			ii = 1;			/* default step value */
			i = get_var();
			if(expr_type) error(0);
			expect(-128+EQ);
			num_vars[i] = eval();
			if(expr_type) error(0);
			expect(-128+TO);
			jj = eval();
			if(test_next(-128+STEP))
				ii = eval();
			skip_stmt();
			ctl_stk[ctl_ptr++] = (unsigned)runptr;	/* line */
			ctl_stk[ctl_ptr++] = (unsigned)cmdptr;	/* command pointer */
			ctl_stk[ctl_ptr++] = (unsigned)ii;		/* step value */
			ctl_stk[ctl_ptr++] = (unsigned)jj;		/* limit value */
			ctl_stk[ctl_ptr++] = (unsigned)i;			/* variable number */
			ctl_stk[ctl_ptr++] = (unsigned)_FOR;
			break;
		case NEXT :
			pgm_only();
			if(ctl_stk[ctl_ptr-1] != _FOR)
				error(6);
			i = ctl_stk[ctl_ptr-2];
			if(!is_l_end(skip_blank()))
				if(get_var() != i) error(6);
			jj = ctl_stk[ctl_ptr-3];	/* get limit */
			ii = ctl_stk[ctl_ptr-4];	/* get step */
			num_vars[i] += ii;
			if((ii < 0) ? num_vars[i] >= jj : num_vars[i] <= jj) {
				cmdptr = (char *)ctl_stk[ctl_ptr-5];
				runptr = (struct line_record *)ctl_stk[ctl_ptr-6];
				line = runptr->Lnumber; }
			else
				ctl_ptr -= 6;
			break;
		case IF :
			i = eval_num();
			expect(-128+THEN);
			if(i) {
				if(isdigit(cmd = skip_blank()))
					return find_line(eval_num());
				else if(cmd < 0) {
					++cmdptr;
					return execute(cmd); }
				else
					execute(1); }
			else
				skip_stmt();
			break;
		case LIF :
			i = eval_num();
			expect(-128+THEN);
			if(i) {
				if((cmd = skip_blank()) < 0) {
					++cmdptr;
					return execute(cmd); }
				else
					execute(1);
				break; }
		case DATA :
			pgm_only();
		case REM :
			if(mode) {
				if((cptr = runptr->Llink))
					return cptr;
				longjmp(savjmp, 1); }
			break;
		case STOP :
stop:
			pgm_only();
			printf("STOP in line %u\r\n",line);
		case END :
			pgm_only();
			longjmp(savjmp, 1);
		case INPUT :
			ii = chk_file(1);
			if(skip_blank() == '"') {		/* special prompt */
				eval();
				expect(','); }
			else
				strcpy(sa1, "? ");
			dptr = address();
			cptr = (struct line_record *)cmdptr;
input:		if(ii == -1)
				fputs(sa1, stdout);
//			cmdptr = fgets(buffer, sizeof(buffer)-1 ,(FILE *)filein);
			tmpbuffer = buffer;
			do
			{
			#if defined (__AVR_ATmega128__)
				c = uart1GetByte();
			#else
				c = uartGetByte();
			#endif

				switch(c) {
					case 8:
						tmpbuffer--;
						break;
					case -1:
						break;
					case 13:
						break;
					case 10:
						break;
					default:
						*tmpbuffer++ = (char)c;

				}
			}
			while (c != 13);
			*tmpbuffer = 0;
			cmdptr = buffer;

			if(expr_type) {
				if(*dptr) free((void*)*dptr);
				*dptr = (unsigned )allocate(strlen(buffer)+1);
				strcpy((char*)*dptr, buffer); }
			else {
				k = 0;
				if(test_next('-'))
					--k;
				if(!isdigit(*cmdptr)) {
					if(ii != -1) error(9);
					fputs("Input error\r\n",stdout);
					goto input; }
				j = get_num();
				*dptr = (k) ? 0-j: j; }
			cmdptr = (char *)cptr;
			break;
		case OPEN :
			if(skip_blank() != '#') error(0);
			if(files[i = chk_file(0)]) error(8);
			eval_char();
			strcpy(buffer, sa1);
			expect(',');
			eval_char();
//			files[i] = fopen(buffer, sa1);
			break;
		case CLOSE :
			if((i = chk_file(1)) == -1) error(0);
			if(!filein) error(8);
			fclose(files[i]);
			files[i] = 0;
			break;
		case DIM :
			do {
				if((dptr = dim_vars[(i = get_var())]))
					free((void*)dptr);
				dim_vars[i] = (int *)allocate((dim_check[i] = eval_num()+1) * 2); }
			while(test_next(','));
			break;
		case ORDER :
			readptr = find_line(eval_num());
			dptr = (unsigned *)cmdptr;
			cmdptr = readptr->Ltext;
			if(get_next() != -128+DATA)
				error(11);
			dataptr = cmdptr;
			cmdptr = (char *)dptr;
			break;
		case READ :
			do {
				dptr = address();
				j = expr_type;
				cptr = (struct line_record *)cmdptr;
				cmdptr = dataptr;
				ii = line;
				if(!skip_blank()) {		/* End of line */
					readptr = readptr->Llink;
					cmdptr = readptr->Ltext;
					if(get_next() != -128+DATA)
						error(11); }
				line = readptr->Lnumber;
				k = eval();
				test_next(',');
				dataptr = cmdptr;
				cmdptr = (char *)cptr;
				line = ii;
				if(j != expr_type)
					error(11);
				if(!expr_type)		/* numeric assignment */
					*dptr = k;
				else {				/* character assignment */
					if(*dptr)
						free((void*)*dptr);
					if(*sa1){
						*dptr = (unsigned )allocate(strlen(sa1)+1);
						strcpy((char*)*dptr, sa1);
					}
					else
						*dptr = 0; } }
			while(test_next(','));
			break;
		case DELAY :
			delay_bas(eval_num());
			break;
		case BEEP :
			i = eval_num();
			expect(',');
			beep(i, eval_num());
			break;
		case DOS :
			/* Not necessary for AVR
			eval_char();
			system(sa1);
			*/
			break;
		case OUT :
			eval_char();
			expect(',');
			i = eval_num();
			out(sa1,(char)i);
			break;
		case DDRSET :
			eval_char();
			expect(',');
			ddrset(sa1,eval_num());
			break;
		case ADCSET :
			i = eval_num();
			expect(',');
			adcset(i, eval_num());
			break;

		case SAVE :
			direct_only();
			if(skip_blank()) {
				eval_char();
				concat(filename, sa1, ".BAS"); }
//			if((fileout = fopen((char *)filename, (char *)"wv"))) {
//				disp_pgm(fileout, 0, -1);
//				fclose(fileout); }
			break;
		case LOAD :
			eval_char();
			concat(filename, sa1, ".BAS");
//			if((filein = fopen(filename, "rv"))) {
//				if(!mode) clear_vars();
//				clear_pgm();
//				while(fgets(buffer, sizeof(buffer)-1, filein))
//					edit_program();
//				fclose(filein);
//				return pgm_start; }
			longjmp(savjmp, 1);
		default :			/* unknown */
			error(0); }
		return 0;
}

/*
 * Test for file operator, and set up pointers
 */
int chk_file(char flag)
{
	unsigned i;

	i = -1;
	if(test_next('#')) {
		if(9 < (i = eval_num())) error(7);
		test_next(',');
		filein = fileout = files[i];
		if(flag && (!filein))
			error(7); }
	else {
		filein = stdin;
		fileout = stdout; }
	return i;
}

/*
 * Display program listing
 */
void disp_pgm(FILE *fp, unsigned i, unsigned j)
{
	unsigned k;
	struct line_record *cptr;
	char c;

	for(cptr = pgm_start; cptr; cptr = cptr->Llink) {
		k = cptr->Lnumber;
		if((k >= i) && (k <= j)) {
			fprintf(fp,"%u ",k);
			for(k=0; (c = cptr->Ltext[k]); ++k)
				if(c < 0) {
					c = c & 127;
					fputs(reserved_words[c - 1], fp);
					if(c < ADD)
						putc(' ',fp); }
				else
					putc(c,fp);
			fputs("\r\n", fp); 
		} 
	}
}

/*
 * Test for program only, and error if interactive
 */
void pgm_only(void)
{
	if(!mode) error(2);
}

/*
 * Test for direct only, and error if running
 */
void direct_only(void)
{
	if(mode) error(1);
}

/*
 * Skip rest of statement
 */
void skip_stmt(void)
{
	char c;

	while((c=*cmdptr) && (c != ':')) {
		++cmdptr;
		if(c == '"') {
			while((c=*cmdptr) && (c != '"'))
				++cmdptr;
			if(c) ++cmdptr; } }
}

/*
 * Dislay error message
 */
void error(unsigned int en)
{
	printf("%s error", error_messages[en]);
	if(mode)
		printf(" in line %u", line);
	fputs("\r\n",stdout);
	longjmp(savjmp, 1);
}

/*
 * Evaluate number only (no character)
 */
int eval_num(void)
{
	unsigned value;

	value = eval();
	if(expr_type)
		error(4);
	return value;
}

/*
 * Evaluate character only (no numeric)
 */
void eval_char(void)
{
	eval();
	if(!expr_type)
		error(4);
}

/*
 * Evaluate an expression (numeric or character)
 */
int eval(void)
{
	unsigned value;

	nest = 0;
	value = eval_sub();
	if(nest != 1) error(0);
	return value;
}

/*
 * Evaluate a sub expression
 */
int eval_sub(void)	
{
	int value = 0;
	unsigned nstack[10], nptr, optr;
	char c, ostack[10];

	++nest;								/* indicate we went down */

/* establish first entry on number and operator stacks */
	ostack[optr = nptr = 0] = 0;		/* add zero to init */

	nstack[++nptr] = get_value();		/* get next value */
/* string operations */
	if(expr_type) {						/* string operations */
		while(!is_e_end(c = skip_blank())) {
			++cmdptr;
			c &= 0x7F;
			get_char_value(sa2);
			if(c == ADD)				/* String concatination */
				strcat(sa1, sa2);
			else {
				if(c == EQ)				/* String EQUALS */
					value = !strcmp(sa1, sa2);
				else if(c == NE)		/* String NOT EQUALS */
					value = strcmp(sa1, sa2) != 0;
				else
					error(0);
				nstack[nptr] = value;
				expr_type = 0; } } }

/* numeric operations */
	else {
		while(!is_e_end(c = skip_blank())) {
			++cmdptr;
			c = (c & 0x7F) - (ADD-1);	/* 0 based priority table */
			if(priority[(int)c] <= priority[(int)ostack[optr]]) {	/* execute operand */
				value = nstack[nptr--];
				nstack[nptr] = do_arith(ostack[optr--], nstack[nptr], value); }
			nstack[++nptr] = get_value();		/* stack next operand */
			if(expr_type) error(0);
			ostack[++optr] = c; }
		while(optr) {				/* clean up all pending operations */
			value = nstack[nptr--];
			nstack[nptr] = do_arith(ostack[optr--], nstack[nptr], value); } }
	if(c == ')') {
		--nest;
		++cmdptr; }
	return nstack[nptr];
}

/*
 * Get a value element for an expression
 */
int get_value(void)
{
	int value = 0;
	unsigned v, *dptr;
	char c, *ptr;

	expr_type = 0;
	if(isdigit(c = skip_blank()))
		value = get_num();
	else {
		++cmdptr;
		switch(c) {
			case '(' :			/* nesting */
				value = eval_sub();
				break;
			case '!' :			/* not */
				value = ~get_value();
				break;
			case -128+SUB :	/* negate */
				value = -get_value();
				break;
			case -128+ASC :	/* Convert character to number */
				eval_sub();
				if(!expr_type) error(4);
				value = *sa1 & 255;
				expr_type = 0;
				break;
			case -128+NUM :	/* Convert string to number */
				eval_sub();
				if(!expr_type) error(4);
				value = atoi(sa1);
				expr_type = 0;
				break;
			case -128+FABS :	/* Absolute value */
				if((value = eval_sub()) > 32767)
					value = -value;
				goto number_only;
			case -128+RND :	/* Random number */
				value = random();
				expect(')');
//				value = random(eval_sub());
				goto number_only;
			case -128+KEY :		/* Keyboard test */
				eval_sub();
				expect(')');
				value = kbtst();
				break;
			case -128+ADCREAD :	/* Read from port */
				value = readadc(get_value());
				expect(')');
				expr_type = 0;
				goto number_only;
			case -128+IN :	/* Read from port */
				eval_sub();
				value = in(sa1);
				expr_type = 0;
			number_only:
				if(expr_type) error(4);
				break;

			default:			/* test for character expression */
				--cmdptr;
				if(isalpha(c)) {		/* variable */
					value = get_var();
					if(expr_type) {		/* char */
						if((ptr = char_vars[value]))
							strcpy(sa1, ptr);
						else
							strcpy(sa1, ""); }
					else {
						if(test_next('(')) {	/* Array */
							if(!(dptr = dim_vars[value]))
								error(10);
							if((v = eval_sub()) >= dim_check[value])
								error(10);
							value = dptr[v]; }
						else						/* Std variable */
							value = num_vars[value]; } }
				else
					get_char_value(sa1); } }

//	printf("value = %u\r\n", value);
	return value;
}

/*
 * Get character value
 */
void get_char_value(char *ptr)
{
	unsigned i;
	char c, *st;

	if((c = get_next()) == '"') {	/* character value */
		while((c = *cmdptr++) != '"') {
			if(!c) error(0);
			*ptr++ = c; }
		*ptr = 0; }
	else if(isalpha(c)) {			/* variable */
		--cmdptr;
		i = get_var();
		if(!expr_type)
			error(0);
		if((st = char_vars[i]))
			strcpy(ptr,st);
		else
			strcpy(ptr,""); }
	else if(c == -128+CHR) {		/* Convert number to character */
		*ptr++ = eval_sub();
		if(expr_type)
			error(4);
		*ptr = 0; }
	else if(c == -128+STR) {		/* Convert number to string */
		num_string(eval_sub(), ptr);
		if(expr_type)
			error(4); }
	else
		error(0);
	expr_type = 1;
}

/*
 * Perform an arithmetic operation
 */
int do_arith(char opr, unsigned op1, unsigned op2)
{
	int value = 0;

	switch(opr) {
		case ADD-(ADD-1) :		/* addition */
			value = op1 + op2;
			break;
		case SUB-(ADD-1) :		/* subtraction */
			value = op1 - op2;
			break;
		case MUL-(ADD-1) :		/* multiplication */
			value = op1 * op2;
			break;
		case DIV-(ADD-1) :		/* division */
			value = op1 / op2;
			break;
		case MOD-(ADD-1) :		/* modulus */
			value = op1 % op2;
			break;
		case AND-(ADD-1) :		/* logical and */
			value = op1 & op2;
			break;
		case OR-(ADD-1) :		/* logical or */
			value = op1 | op2;
			break;
		case XOR-(ADD-1) :		/* exclusive or */
			value = op1 ^ op2;
			break;
		case EQ-(ADD-1) :		/* equals */
			value = op1 == op2;
			break;
		case NE-(ADD-1) :		/* not-equals */
			value = op1 != op2;
			break;
		case LE-(ADD-1) :		/* less than or equal to */
			value = op1 <= op2;
			break;
		case LT-(ADD-1) :		/* less than */
			value = op1 < op2;
			break;
		case GE-(ADD-1) :		/* greater than or equal to */
			value = op1 >= op2;
			break;
		case GT-(ADD-1) :		/* greater than */
			value = op1 > op2;
			break;
		default:
			error(0); }
	return value;
}

/*
 * Convert a number to a string, and place in memory
 */
void num_string(unsigned value, char *ptr)
{
	char cstack[5], cptr;

	cptr = 0;

	if(value > 32767) {
		*ptr++ = '-';
		value = -value; }
	do
		cstack[(int)cptr++] = (value % 10) + '0';
	while(value /= 10);
	while(cptr)
		*ptr++ = cstack[(int)--cptr];
	*ptr = 0;
}

/*
 * Clear program completely
 */
void clear_pgm(void)
{
	for(runptr = pgm_start; runptr; runptr = runptr->Llink)
		free(runptr);
	pgm_start = 0;
}

/*
 * Clear all variables to zero
 */
void clear_vars(void)
{
	unsigned i;
	char *ptr;

	for(i=0; i < NUM_VAR; ++i) {
		num_vars[i] = 0;
		if((ptr = char_vars[i])) {	/* Character variables */
			free(ptr);
			char_vars[i] = 0; }
		if((ptr = (char *)dim_vars[i])) {		/* Dimensioned arrays */
			free(ptr);
			dim_vars[i] = 0; } }
}

/*
 * Get index for variable from its name
 */
int get_var( void )
{
	unsigned index;
	char c;

	if(!isalpha(c = get_next()))
		error(0);
	index = ((c - 'A') & 0x1F) * 10;
	if(isdigit(c = *cmdptr)) {
		index += (c - '0');
		c = *++cmdptr; }
	if(c == '$') {
		++cmdptr;
		expr_type = 1; }
	else
		expr_type = 0;

	return index;
}

/*
 * Main program
 */
int main(int argc, char *argv[])
{
	int i, j;
	int c;
	char *tmpbuffer;

	init_uart(1);
	stdout = &mystdout;

	// make all rprintf statements use uart for output
	#if defined (__AVR_ATmega128__)
		rprintfInit(uart1SendByte);
	#else
		rprintfInit(uartSendByte);
	#endif
	// turn on and initialize A/D converter
	a2dInit();

	// initialize vt100 terminal
	vt100Init();


/*
 * If arguments are given, copy them into the A0$, A1$, A2$ ... variables
 */
	pgm_start = (struct line_record *)0;
	j = 0;

/*
 * If a name is given on the command line, load it as a program and
 * run immediately. If the program does not explicitly EXIT, we will
 * then proceed to an interactive session
 */
//	if(j) {
//		concat(filename, char_vars[0], ".BAS");
//		if(filein = fopen(filename, "rv")) {
//			while(fgets(buffer, sizeof(buffer)-1, filein))
//				edit_program();
//			fclose(filein);
//			if(!setjmp(savjmp))
//				execute(RUN1); } }

/*
 * Display header AFTER running command line, so that programs run as
 * "batch" commands terminating with EXIT do not get "noise" output.
 */
	fputs("MICRO-BASIC 2.1 - Copyright 1982-2000 Dave Dunfield.\r\n",stdout);
	fputs("AVR Version by Max Oliver 0.3a - 2006.\r\n",stdout);

	setjmp(savjmp);
	for(;;) {						/* Main interactive loop */
		fputs("Ready\r\n", stdout);
//		memset(buffer,0,sizeof(buffer));

	noprompt: 
		mode = ctl_ptr = 0;
		tmpbuffer = buffer;
		do
		{
		#if defined (__AVR_ATmega128__)
			c = uart1GetByte();
		#else
			c = uartGetByte();
		#endif

			switch(c) {
				case 8:
					tmpbuffer--;
					break;

				case -1:
					break;

				case 13:
					break;

				case 10:
					break;

				default:
					*tmpbuffer++ = (char)c;

			}

//			if( (c != -1) && (c != 13) && (c != 10)){
//				*tmpbuffer++ = (char )c;

//			}
		}
		while (c != 13);
		
		*tmpbuffer = 0;

		if(edit_program())			/* Tokenize & edit if OK */
			goto noprompt;
		if((i = *cmdptr) < 0) {		/* Keyword, execute command */
			++cmdptr;
			execute(i); }
		else if(i)					/* Unknown, assume LET */
			execute(LET); }

	return 0;
}
