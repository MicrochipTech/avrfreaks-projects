/********************************************************************************/
/*  sd-atm, John Kim, john-k.home@rcn.com,  October 2007                        */
/*   Stack depth analyzer, works for avr atm128.                                */
/*   Free code.                                                                 */
/*   Compiles on Windows, MSYS, MinGW, and "gcc (GCC) 3.4.2 (mingw-special)"    */
/*                                                                              */
/********************************************************************************/

//1. Resolve: multiple 'nop' appears as ellipsis
//  ===>. May use elf file, instead
//2. Get rid of odd number codes[]
//3. May use .elf

#define PROG_NAME "sd-atm"
#define VERSION 1.1f
#define My_Message "John Kim"

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "settings.h"
#include "common.h"
#include "decode.h"
#include "process.h"

//=====================================================================================================

code_t codes[MAX_INSTRUCTIONS];

label_t labels[MAX_NUM_LABELS];

FILE *fp_source = NULL;

struct
{
	long n_src_lines;
	long n_instructions;
	long n_data_lines;
	int n_labels;
}decoding;


struct
{
	int trace_decoding;
	int trace_proc;
	int dump_decoded;
	int dump_processed;
	int dump_label;
	int backward_update;
}request;

//=====================================================================================================

//--------------------------------------------------
static char *input_file_name = 0;
static char *program_name = "this program";

static struct
{
	addr_t address;
	char label[MAX_LABEL_LEN];
	int depth;
}initial;

//=====================================================================================================

//Generating list_file.lss from file_name.elf: avr-objdump -d file_name.elf > list-file.lss
static void usage(void)
{
	fprintf(stdout, "\n\nUsage:\n");
	fprintf(stdout, "\n %s list_file.lss [options] \n", program_name);
	fprintf(stdout, "     options are:\n");
	fprintf(stdout, "       -a hex-address(0x1234) | label to process\n");
	fprintf(stdout, "           (default == all labels) \n");
	//fprintf(stdout, "       -i depth : initial stack depth, (default == 0) \n");
	fprintf(stdout, "       -td: trace decoding \n");
	fprintf(stdout, "       -tp: trace processing\n");
	fprintf(stdout, "       -dd: dump decoded \n");
	fprintf(stdout, "       -dp: dump processed \n");
	fprintf(stdout, "       -dl: dump label \n");
	//fprintf(stdout, "       -b : backward update, may speed up for 'all labels'\n");
}

static void usage_exit(void)
{
	usage();
	exit(0);
}

void error_exit(const char *message, ...)
{
	va_list args;

	va_start(args, message);
	fprintf(stderr, "\n Error: ");
	vfprintf(stderr, message, args);
	va_end(args);

	if(fp_source != NULL)
	{
		fclose(fp_source);
	}

	exit(0);
}


//=====================================================================================================

//------------------------------------------------------------------------------
static void parse_args(int argc, char **argv)
{
	char *arg;
	int idx = 0;
	int n;

	program_name = argv[0]; //this is the name used to invoke this code

	if(argc < 2) //nothing but prog name
	{
		usage_exit();
	}

	input_file_name = argv[1];

	idx = 2; //argv[0]: prog name, [1]:input file, [2]:options starts
	argc -= 2;

	initial.address = 0xffff;
	initial.label[0] = '\0';
	initial.depth = 0;

	request.trace_decoding = 0;
	request.trace_proc = 0;
	request.dump_decoded = 0;
	request.dump_processed = 0;
	request.dump_label = 0;
	request.backward_update = 0;

	for(;;)
	{
		if(argc == 0)
		{
			return;
		}

		if(argc < 1) //option and option arg
		{
			usage();
			error_exit("Incorrect option usage");
		}

		arg = argv[idx];

		if(arg[0] != '-') //expected option prefix
		{
			usage();
			error_exit("Option prefix '-' is expected");
		}

		switch(arg[1])
		{
			case 'd':
				switch(arg[2])
				{
					case 'd':
						request.dump_decoded = ~0;
						break;

					case 'p':
						request.dump_processed = ~0;
						break;

					case 'l':
						request.dump_label = ~0;
						break;

					default:
						error_exit("wrong option");
						break;
				}

				break;

			case 't':
				switch(arg[2])
				{
					case 'p':
						request.trace_proc = ~0;
						break;
					case 'd':
						request.trace_decoding = ~0;
						break;

					default:
						error_exit("wrong option");
						break;
				}

				break;

			case 'b':
				request.backward_update = ~0;
				break;

			case 'a':
				if(argc < 2)
				{
					error_exit("Address or label expected");
				}

    			n = sscanf(argv[idx+1], "0x%x", &initial.address);

				if(n != 1) //it is not a number
				{
					//error_exit("Wrong VECTOR address\n");
					strcpy(initial.label, argv[idx+1]);
				}

				idx += 2;
				argc -= 2;
				continue;

			case 'i':
				if(argc < 2)
				{
					error_exit("Number expected");
				}

    			n = sscanf(argv[idx+1], "%d", &initial.depth);

				if(n != 1) //it is not a number
				{
					error_exit("Wrong 'initial depth'\n");
				}

				idx += 2;
				argc -= 2;
				continue;

			default:
				usage();
				error_exit("wrong option");
				break;
		}

		argc--;
		idx++;
	}

}

static void dump_label(void)
{
	unsigned int i;

	printf("\n\t ====== LABELS ==========\n");

	for(i = 0; i < (unsigned int)decoding.n_labels; i++)
	{
		printf("\t %#.4x   %s\n", labels[i].addr, labels[i].label);
	}

	printf("\t ========================\n");
}


static void dump_processed(void)
{
	addr_t addr;
	char s_operand[20], s_depth_fore[20];

	printf("\n\t======= PROCESSED ========================================================\n");

	for(addr = 0; addr < MAX_INSTRUCTIONS; addr++)
	{
		if(codes[addr].inst[0])
		{
			if(codes[addr].depth_forward == DEPTH_NONE) continue;

			if(codes[addr].idx_label >= 0)
			{
				printf("	%#.4x: %s:\n", labels[codes[addr].idx_label].addr, labels[codes[addr].idx_label].label);
			}


			if(codes[addr].opr_type == opr_NONE)
			{
				sprintf(s_operand, " ______");
			}
			else if(codes[addr].opr_type == opr_HEX)
			{
				sprintf(s_operand, " %#6.4x", codes[addr].operand);
			}
			else
			{
				sprintf(s_operand, " %+.5d", codes[addr].operand);
			}

			printf("	  [%#6.4x: %-5s %s]", addr, codes[addr].inst, s_operand);


			if(codes[addr].depth_forward == DEPTH_NONE)
			{
				sprintf(s_depth_fore, "____");
			}
			else
			{
				sprintf(s_depth_fore, "%-4d", codes[addr].depth_forward);
			}


			printf("	delta=%+3d, d-f=%s\n",
					codes[addr].delta, s_depth_fore);
		}
	}
	printf("\t========================================================================\n");
}



int main(int argc, char **argv)
{
	int i;

	printf(" %s version %5.2f,  %s", PROG_NAME, VERSION, My_Message);

	parse_args(argc, argv);

	fp_source = fopen(input_file_name, "r+b");

	if(fp_source == NULL)
	{
		error_exit("Input file '%s' is not readable.\n", input_file_name);
	}

	//rewind(fp_source);

	decode();

	if(request.dump_label)
	{
		dump_label();
	}

	if(initial.label[0])
	{
		for(i = 0; i < decoding.n_labels; i++)
		{
			if(___str_eq(labels[i].label, initial.label))
			{
				initial.address = labels[i].addr;
				break;
			}
		}

		if(i == decoding.n_labels)
		{
			error_exit("Label '%s' is not found.\n", initial.label);
		}
	}

	if(initial.address != 0xffff)
	{
		process_one(initial.address, initial.depth);

		if(request.dump_processed)
		{
			dump_processed();
		}
	}
	else
	{
		for(i = 0; i < decoding.n_labels; i++)
		{
			process_one(labels[i].addr, initial.depth);

			if(request.dump_processed)
			{
				dump_processed();
			}
		}
	}

	return(0);
}

