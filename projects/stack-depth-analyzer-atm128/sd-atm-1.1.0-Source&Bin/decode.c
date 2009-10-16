/********************************************************************************/
/*  sd-atm, John Kim, john-k.home@rcn.com,  October 2007                        */
/*   Stack depth analyzer, works for avr atm128.                                */
/*   Free code.                                                                 */
/*   Compiles on Windows, MSYS, MinGW, and "gcc (GCC) 3.4.2 (mingw-special)"    */
/*                                                                              */
/********************************************************************************/


#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "settings.h"
#include "common.h"
#include "sd-atm.h"
#include "instructions.h"

//=====================================================================================================

static int dump;
static long line_num;
static long n_instructions;
static long n_data_lines;
static int n_labels;
static addr_t max_code_addr;
static char src_line[MAX_SRC_LINE_LEN + 2];

static struct
{
	addr_t last_addr;
	int started;
}ellipsis;

//=====================================================================================================

#define __printf if(dump) printf

//=====================================================================================================
static const char SOURCE_TYPES_STR[] =
	"\t" "\n"
	"\t" " -------------------------- Soruce line [types] ---------------------------\n"
	"\t" " [1]:\n"
	"\t" " \t 2290: \t 0e 94 e6 5d \t call \t 0xbbcc <fsys_read> \n"
	"\t" " \t 200a: \t 0c 94 d0 31 \t jmp \t 0x63a0 <__vector_TEST_vect> \n"
	"\t" " \t 20d2: \t 80 93 88 00 \t sts \t 0x0088, r24 \n"
	"\t" "\n"
	"\t" " [2]:\n"
	"\t" " \t 63b2: \t 20 91 89 00 \t lds \t r18, 0x0089 \n"
	"\t" "\n"
	"\t" " [3]:\n"
	"\t" " \t 2062: \t 00 98 \t cbi	0x00, 0	; 0 \n"
	"\t" " \t 21fe: \t de bf \t out	0x3e, r29	; 62\n"
	"\t" "\n"
	"\t" " [4]:\n"
	"\t" " \t 207e:	98 de       	rcall	.-720    	; 0x1db0 <_end_sys+0xc34> \n"
	"\t" " \t 2096:	98 f4       	brcc	.+38     	; 0x20be <_app_start+0x5e> \n"
	"\t" "\n"
	"\t" " [5]:\n"
	"\t" " \t b6da:	26 96       	adiw	r28, 0x06	; 6 \n"
	"\t" " \t 21f8:	24 97       	sbiw	r28, 0x04	; 4 \n"
	"\t" " \t 20e6:	4f b6       	in	r4, 0x3f	; 63 \n"
	"\t" "\n"
	"\t" " [6]:\n"
	"\t" " \t 2064:	74 07       	cpc	r23, r20 \n"
	"\t" " \t 206c:	98 ae       	std	Y+56, r9	; 0x38 \n"
	"\t" " \t 20c8:	4f 92       	push	r4 \n"
	"\t" " \t 20c6:	f8 94       	cli \n"
	"\t" " \t 2060:	00 00       	nop \n"
	"\t" "\n"
	"\t" " [L]abel:\n"
	"\t" " \t 00002000 <_data_crc>: \n"
	"\t" "\n"
	"\t" " [D]ata:\n"
	"\t" " \t 205e:	ff ff       	.word	0xffff	; ???? \n"
	"\t" " \t 2006:	a6 0f                                               [..] \n"
	"\t" " \t 2008:	34 11                                               [4.] \n"
	"\t" " \t 2004:	0a 10 \n"
	"\t" "\n"
	"\t" " [E]llipsis:\n"
    "\t" " \t 4254:	09 9a       	sbi	0x01, 1	; 1\n"
	"\t" " \t ...\n"
    "\t" " \t 426e:	00 00       	nop\n"
//	"\t" "\n"
//	"\t" " [ ]:\n"
//	"\t" " \t Don't care\n"
	"\t" " --------------------------------------------------------------------------\n";

//=====================================================================================================

static int is_known_inst(char *inst)
{
	return(___str_eq(inst, BRANCH_INSTRUCTIONS) ||
			___str_eq(inst, SKIP_INSTRUCTIONS) ||
			___str_eq(inst, JUMP_INSTRUCTIONS) ||
			___str_eq(inst, INDIRECT_JUMP_INSTRUCTIONS) ||
			___str_eq(inst, CALL_INSTRUCTIONS) ||
			___str_eq(inst, INDIRECT_CALL_INSTRUCTIONS) ||
			___str_eq(inst, LONG_INSTRUCTIONS) ||
			___str_eq(inst, INSTRUCTIONS_A) ||
			___str_eq(inst, INSTRUCTIONS_F) ||
			___str_eq(inst, INSTRUCTIONS_S));
}


static void record_code(addr_t addr, char *inst, int opr, char method)
{
	if(codes[addr].inst[0] != '\0')
	{
		error_exit("Line %d, [%s]\n	tries to overwrite addr %#.4x\n"
					"\t %#.4x %s \t %4x",
					line_num, src_line, addr,
					addr, codes[addr].inst, codes[addr].operand);
	}

	strcpy(codes[addr].inst, inst);
	codes[addr].operand = opr;

	if(method == '4')
	{
		codes[addr].opr_type = opr_INT;
	}
	else if(method == '6')
	{
		codes[addr].opr_type = opr_NONE;
	}
	else
	{
		codes[addr].opr_type = opr_HEX;
	}

	n_instructions++;
}


static void _decode(void)
{
	char inst[MAX_SRC_LINE_LEN], op1[MAX_SRC_LINE_LEN], op2[MAX_SRC_LINE_LEN];
	char label[MAX_LABEL_LEN];

	int n_match;
	addr_t addr, ad;
	int opr;
	unsigned int c1, c2, c3, c4;

	char method;

	int i, j;

	//----------------------------------------------------------------


	for(line_num = 1; ; line_num++)
	{
		if(fgets(src_line, sizeof(src_line), fp_source) == NULL) //gets line until end
		{
			if(feof(fp_source))
			{
				fclose(fp_source);
				return;
			}
			else
			{
				error_exit("Cannot read source file\n");
			}
		}

		addr = 0;
		opr = 0;

		i = strlen(src_line);

		if((i > MAX_SRC_LINE_LEN) || (src_line[i - 1] != '\n'))
		{
			error_exit("Line %d is too long\n", line_num);
			exit(0);
		}

		for(j = 0; (j < 4) && (i >= 0); j++, i--)
		{
			src_line[i] = __eq(src_line[i], '\n', '\r') ? '\0' : src_line[i];
		}

		method = ' '; //unknown method

		inst[0] = '\0';
		op1[0] = '\0';
		op2[0] = '\0';

		n_match = sscanf(src_line, "%x: %x %x %x %x %s %s %s", &addr, &c1, &c2, &c3, &c4, inst, op1, op2);

		if(n_match >= 7)
		{
			if(sscanf(op1, "0x%x", &opr))
			{
				//	2290:	0e 94 e6 5d 	call	0xbbcc <fsys_read>
				//	200a:	0c 94 d0 31 	jmp	0x63a0 <__vector_SAFETY_vect>
				//	20d2:	80 93 88 00 	sts	0x0088, r24
				method = '1';
			}
			else if(sscanf(op2, "0x%x", &opr))
			{
				//    63b2:	20 91 89 00 	lds	r18, 0x0089
				method = '2';
			}
			else
			{
				method = 'D'; //data
			}
		}
		else if(n_match >= 3)
		{
			inst[0] = '\0';
			op1[0] = '\0';
			op2[0] = '\0';

			n_match = sscanf(src_line, "%x: %x %x %s %s %s", &addr, &c1, &c2, inst, op1, op2);

			if(sscanf(op1, "0x%x", &opr) > 0) //the first argument is needed to detect SP write
			{
				//	2062:	00 98       	cbi	0x00, 0	; 0
				//	21fe:	de bf       	out	0x3e, r29	; 62 <=== SP write
				method = '3';
			}
			else if(sscanf(op1, ".%d", &opr) > 0) //the first argument is needed for rcall and branch
			{
				//	207e:	98 de       	rcall	.-720    	; 0x1db0 <_rom_end_sys+0xc34>
				//	2096:	98 f4       	brcc	.+38     	; 0x20be <_app_dir_start+0x5e>
				method = '4';
			}
			else if(sscanf(op2, "0x%x", &opr) > 0) //the second argument is needed to calculate SP write value
			{
				//	b6da:	26 96       	adiw	r28, 0x06	; 6 <== related to SP write
				//	21f8:	24 97       	sbiw	r28, 0x04	; 4 <== related to SP write
				//	20e6:	4f b6       	in	r4, 0x3f	; 63
				method = '5';
			}
			else
			{
				//	20c6:	f8 94       	cli
				//	2060:	00 00       	nop
				//	2064:	74 07       	cpc	r23, r20
				//	206c:	98 ae       	std	Y+56, r9	; 0x38
				//	20c8:	4f 92       	push	r4
				// --------------------------------
				// These will become method 'D' later
				//	205e:	ff ff       	.word	0xffff	; ????
				//  2006:	a6 0f                                               [..]
				//  2008:	34 11                                               [4.]
				//	2004:	0a 10
				// --------------------------------
				method = '6'; //whatever
			}

		}
		else
		{
			n_match = sscanf(src_line, "%x <%s>:", &addr, label); //sscanf matches '<' but not after '%s' (">:")

			if((n_match == 2) && (label[strlen(label) -2] == '>'))
			{
				//00002000 <_app_crc>:
				label[strlen(label) -2] = '\0';
				method = 'L';
			}
			else if(___str_eq(src_line, "	...")) //ellipsis
			{
				method = 'E';
			}
		}


		if(!__eq(method, ' ', 'L', 'E')) //expecting instruction
		{
			if(!is_known_inst(inst))
			{
				method = 'D'; //data
			}
		}


		if(method == 'D') //data
		{
			n_data_lines++;

			ellipsis.last_addr = addr;
			ellipsis.started = 0;
		}
		else if(method == 'L') //label
		{
			if(codes[addr].idx_label >= 0) //there already is label assigned to this address
			{
				error_exit(" Line %d, %s tries to re-assign label [%s] to addr %#.4x\n", line_num, src_line, label, addr);
			}

			codes[addr].idx_label = n_labels;

			labels[n_labels].addr = addr;

			strcpy(labels[n_labels].label, label);

			n_labels++;

			ellipsis.last_addr = addr;
			ellipsis.started = 0;
		}
		else if(method == 'E') //Ellipsis
		{
			ellipsis.started = ~0;
		}
		else if(method != ' ')
		{
			record_code(addr, inst, opr, method);

			if((ellipsis.started) && (ellipsis.last_addr))
			{
				for(ad = addr - 2; ad > ellipsis.last_addr; ad -= 2)
				{
					record_code(ad, inst, opr, method);
				}
			}

			ellipsis.last_addr = addr;
			ellipsis.started = 0;
		}


		if(method != ' ')
		{
			__printf("%6ld: [%c] \t %s\n", line_num, method, src_line);
		}
		else
		{
			__printf("%6ld:      \t %s\n", line_num, src_line);
		}

		max_code_addr = __max(max_code_addr, addr);

	}
}

static void decode_init(void)
{
	unsigned int i;

	for(i = 0; i < MAX_INSTRUCTIONS; i++)
	{
		codes[i].inst[0] = '\0';
		codes[i].idx_label = -1;
	}

	for(i = 0; i < MAX_NUM_LABELS; i++)
	{
		labels[i].label[0] = '\0';
	}


	n_labels = 0;
	n_instructions = 0;
	n_data_lines = 0;
	max_code_addr = 0;
}

static void dump_decoded(void)
{
	addr_t addr;

	printf("\n\n\t ---------------- Decoded ------------------------------\n");

	for(addr = 0; addr <= max_code_addr; addr += 2)
	{
		if(codes[addr].inst[0] != '\0')
		{
			printf("\t\t %#6.4x: [%-5s]", addr, codes[addr].inst);

			if(codes[addr].opr_type == opr_NONE)
			{
				printf(" ______\n");
			}
			else if(codes[addr].opr_type == opr_HEX)
			{
				printf(" %#.4x\n", codes[addr].operand);
			}
			else
			{
				printf(" %+.5d\n", codes[addr].operand);
			}
		}
	}
	printf("\t --------------------------------------------------------\n");
}

void decode(void)
{
	printf("\n________ Decoding ...");

	decode_init();

	dump = request.trace_decoding;

	__printf(SOURCE_TYPES_STR);

	_decode();

	decoding.n_src_lines = line_num;
	decoding.n_instructions = n_instructions;
	decoding.n_data_lines = n_data_lines;
	decoding.n_labels = n_labels;
	ellipsis.started = 0;

	printf("\n\t.... done Decoding; ");

	if(request.dump_decoded)
	{
		dump_decoded();
	}

    printf("\n\t %ld lines, %ld instructions, %ld lines of data, %d labels found\n",
    	decoding.n_src_lines, decoding.n_instructions, decoding.n_data_lines, decoding.n_labels);
}
