/********************************************************************************/
/*  sd-atm, John Kim, john-k.home@rcn.com,  October 2007                        */
/*   Stack depth analyzer, works for avr atm128.                                */
/*   Free code.                                                                 */
/*   Compiles on Windows, MSYS, MinGW, and "gcc (GCC) 3.4.2 (mingw-special)"    */
/*                                                                              */
/********************************************************************************/

#ifndef __SD_ATM_H__
#define __SD_ATM_H__

//=====================================================================================================

extern code_t codes[MAX_INSTRUCTIONS];
extern label_t labels[MAX_NUM_LABELS];

extern struct
{
	long n_src_lines;
	long n_instructions;
	long n_data_lines;
	int n_labels;
}decoding;

extern FILE *fp_source;

extern struct
{
	int trace_decoding;
	int trace_proc;
	int dump_decoded;
	int dump_processed;
	int dump_label;
	int backward_update;
}request;

//=====================================================================================================

void error_exit(const char *message, ...);

#endif //__SD_ATM_H__

