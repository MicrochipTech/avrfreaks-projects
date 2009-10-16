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


//=====================================================================================================

#define __printf if(request.trace_proc) printf
#define __show_process if(request.trace_proc) show_process


//=====================================================================================================

//--------------------------------------------------
/*
    21f4:	cd b7       	in	r28, 0x3d	; 61
    21f6:	de b7       	in	r29, 0x3e	; 62
    21f8:	24 97       	sbiw	r28, 0x04	; 4
    21fa:	0f b6       	in	r0, 0x3f	; 63
    21fc:	f8 94       	cli
    21fe:	de bf       	out	0x3e, r29	; 62
    2200:	0f be       	out	0x3f, r0	; 63
    2202:	cd bf       	out	0x3d, r28	; 61
*/


typedef struct
{
	code_t	in_spl;
	code_t	in_sph;
	code_t	sbiw;
	code_t	in_sreg;
	code_t	cli;
	code_t	out_sph;
	code_t	out_sreg;
	code_t	out_spl;
}push_auto_sp_t;


/*
    b6dc:	0f b6       	in	r0, 0x3f	; 63
    b6de:	f8 94       	cli
    b6e0:	de bf       	out	0x3e, r29	; 62
    b6e2:	0f be       	out	0x3f, r0	; 63
    b6e4:	cd bf       	out	0x3d, r28	; 61
*/

typedef struct
{
	code_t	adiw;
	code_t	in_sreg;
	code_t	cli;
	code_t	out_sph;
	code_t	out_sreg;
	code_t	out_spl;
}pop_auto_sp_t;

static code_t sp_write_MODEL =
{"out", __SP_L__, opr_HEX, 0, 0, 0, -1};

static push_auto_sp_t push_auto_MODEL =
{
	{"in", __SP_L__, opr_HEX, 0, 0, 0, -1},
	{"in", __SP_H__, opr_HEX, 0, 0, 0, -1},
	{"sbiw", 0, opr_HEX, 0, 0, 0, -1},
	{"in", __SREG__, opr_HEX, 0, 0, 0, -1},
	{"cli", 0, opr_NONE, 0, 0, 0, -1},
	{"out", __SP_H__, opr_HEX, 0, 0, 0, -1},
	{"out", __SREG__, opr_HEX, 0, 0, 0, -1},
	{"out", __SP_L__, opr_HEX, 0, 0, 0, -1}
};

static pop_auto_sp_t pop_auto_MODEL =
{
	{"adiw", 0, opr_HEX, 0, 0, 0, -1},
	{"in", __SREG__, opr_HEX, 0, 0, 0, -1},
	{"cli", 0, opr_NONE, 0, 0, 0, -1},
	{"out", __SP_H__, opr_HEX, 0, 0, 0, -1},
	{"out", __SREG__, opr_HEX, 0, 0, 0, -1},
	{"out", __SP_L__, opr_HEX, 0, 0, 0, -1}
};

//--------------------------------------------------
static addr_t path[MAX_STEP]; //record of path

//automatic variables creation and destroy history
static struct
{
	int delta;
	int depth_forward;
}auto_stack[MAX_CALL_LEVEL];

//global max values detection
static struct
{
	int auto_level;
	int call_depth;
	int step;
	int for_depth;
	long proc_iter;
}maxs;

static struct
{
	unsigned int addr;
	unsigned int step;
	int depth_forward;
}proc_track[MAX_STEP];


//=====================================================================================================

static int insn_size(addr_t addr)
{
    if(___str_eq(codes[addr].inst, LONG_INSTRUCTIONS))
    {
		return(4);
	}

    return 2;
}

static int is_branch(addr_t addr)
{
	return(___str_eq(codes[addr].inst, BRANCH_INSTRUCTIONS));
}

static int is_skip(addr_t addr)
{
	return(___str_eq(codes[addr].inst, SKIP_INSTRUCTIONS));
}

static int is_ijmp(addr_t addr)
{
	if(___str_eq(codes[addr].inst, INDIRECT_JUMP_INSTRUCTIONS))
	{
		return(~0);
	}

	return(0);
}

static int is_call(addr_t addr)
{
	return(___str_eq(codes[addr].inst, CALL_INSTRUCTIONS));
}

static int is_icall(addr_t addr)
{
	if(___str_eq(codes[addr].inst, INDIRECT_CALL_INSTRUCTIONS))
	{
		return(~0);
	}

	return(0);
}

static int get_rel(addr_t addr)
{
    return(2 + codes[addr].operand);
}

static addr_t get_next(addr_t addr)
{

	if(___str_eq(codes[addr].inst, "rjmp"))
	{
		return((unsigned int)(addr + get_rel(addr)));
	}
	else if(___str_eq(codes[addr].inst, "jmp"))
	{
		return((unsigned int) codes[addr].operand);
	}

	return(addr + insn_size(addr));
}

static addr_t get_target(addr_t addr)
{
	addr_t next;

	if(___str_eq(codes[addr].inst, "rcall"))
	{
	    return((unsigned int)(addr + get_rel(addr)));
	}
	else if(___str_eq(codes[addr].inst, "call"))
	{

	    return((unsigned int) codes[addr].operand);
	}
    else if(is_branch(addr))
    {
	    return((unsigned int)(addr + get_rel(addr)));
    }
    else if(is_skip(addr)) //skip size depends on size of subsequent insn
    {
		next = addr + insn_size(addr);
		return(next + insn_size(next));
    }

	return(addr + insn_size(addr));
}

static int is_ret(addr_t addr)
{
	return((___str_eq(codes[addr].inst, "ret", "reti")) ? ~0 : 0);
}


//--------------------------------------------------
static int is_eq_inst(code_t *ps, code_t *pm)
{
	if(___str_eq(ps->inst, pm->inst) && (ps->opr_type == pm->opr_type) && (ps->idx_label == pm->idx_label))
	{
		return(~0);
	}

	return(0);
}

static int is_eq_code(code_t *ps, code_t *pm)
{
	if(is_eq_inst(ps, pm) && (ps->operand == pm->operand))
	{
		return(~0);
	}

	return(0);
}

static int is_sp_write(addr_t addr)
{
	return(is_eq_code(&codes[addr], &sp_write_MODEL));
}

enum {AUTO_NOT, AUTO_YES, AUTO_ERROR, AUTO_ERROR_CRITICAL};

static int auto_push(addr_t addr, int depth_forward, int *p_auto_level, int *p_depth)
{
	int nd;
	int auto_level = *p_auto_level;
	code_t *ps = &codes[addr];

	if(is_eq_code(ps - 1 * 2, &push_auto_MODEL.out_sreg))
	{
		if(is_eq_code(ps - 2 * 2, &push_auto_MODEL.out_sph))
		{
			if(is_eq_code(ps - 3 * 2, &push_auto_MODEL.cli))
			{
				if(is_eq_code(ps - 4 * 2, &push_auto_MODEL.in_sreg))
				{
					if(is_eq_code(ps - 6 * 2, &push_auto_MODEL.in_sph))
					{
						if(is_eq_code(ps - 7 * 2, &push_auto_MODEL.in_spl))
						{
							if(is_eq_inst(ps - 5 * 2, &push_auto_MODEL.sbiw))
							{

								nd = (ps - 5 * 2)->operand;

								auto_stack[auto_level].depth_forward = depth_forward; //before push
								auto_stack[auto_level].delta = nd; //difference
								auto_level++;

								if(auto_level >= MAX_CALL_LEVEL)
								{
									//printf("\n\n Internal ERROR, auto_push() @ %#.4x, auto_level too deep:%d >= %d\n", addr, auto_level, MAX_CALL_LEVEL);
									//exit(0);

									printf("\n\n ERROR, auto_push() @ %#.4x, auto_level too deep:%d >= %d\n", addr, auto_level, MAX_CALL_LEVEL);
									printf("  Guesses: 1. Not compatible target code. 2. Started with wrong address, 3. Internal ERROR\n");
									return(AUTO_ERROR_CRITICAL);
								}

								maxs.auto_level = __max(auto_level, maxs.auto_level);

								__printf(" * APUSH(%+d)", nd);
								*p_auto_level = auto_level;

								*p_depth = nd;
								return(AUTO_YES);
							}
						}
					}
				}
			}
		}
	}

	*p_depth = 0;
	return(AUTO_NOT);
}


static int auto_pop(addr_t addr, int depth_forward, int *p_auto_level, int *p_depth)
{
	int nd;
	int auto_level = *p_auto_level;
	code_t *ps = &codes[addr];

	if(is_eq_code(ps - 1 * 2, &pop_auto_MODEL.out_sreg))
	{
		if(is_eq_code(ps - 2 * 2, &pop_auto_MODEL.out_sph))
		{
			if(is_eq_code(ps - 3 * 2, &pop_auto_MODEL.cli))
			{
				if(is_eq_code(ps - 4 * 2, &pop_auto_MODEL.in_sreg))
				{
					if(is_eq_inst(ps - 5 * 2, &pop_auto_MODEL.adiw))
					{
						auto_level--;

						if(auto_level < 0)
						{
							//printf("\n\n Internal ERROR, auto_pop() @ %#.4x, auto_level NEGATIVE:%d\n", addr, auto_level);
							//exit(0);
							printf("\n\n ERROR, auto_pop() @ %#.4x, auto_level NEGATIVE:%d\n", addr, auto_level);
							printf("  Guesses: 1. Not compatible target code. 2. Started with wrong address, 3. Internal ERROR\n");
							return(AUTO_ERROR_CRITICAL);
						}

						nd = (ps - 5 * 2)->operand;

						if(auto_stack[auto_level].delta != nd) //difference
						{
							printf("auto_pop() @ %#.4x, delta different: pushed=%d, popping=%d\n", addr, auto_stack[auto_level].delta, nd);

							*p_depth = 0;
							return(AUTO_ERROR);
						}
						else if(auto_stack[auto_level].depth_forward != (depth_forward - nd)) //before push
						{
							printf("auto_pop() @ %#.4x, depth error: pushed=%d, popping=%d-%d\n", addr, auto_stack[auto_level].delta, depth_forward, nd);

							*p_depth = 0;
							return(AUTO_ERROR);
						}
						else
						{
							__printf(" * APOP(%+d)", nd * -1);

							*p_auto_level = auto_level;
							*p_depth = nd * -1;
							return(AUTO_YES);
						}
					}
				}
			}
		}
	}

	*p_depth = 0;
	return(AUTO_NOT);
}


static int stack_change(addr_t addr)
{
	if(___str_eq(codes[addr].inst, "push"))
	{
		return(1);
	}
	else if(___str_eq(codes[addr].inst, "pop"))
	{
		return(-1);
	}
	else if(is_call(addr) || is_icall(addr)) //'call' does not (should not) affect the current sp, but 'ret' does
	{
		return(0);
	}
	else if(is_ret(addr))
	{
		return(-2);
	}

	return(0);
}


#define HEADER \
     "    	                      		delta	d-old	d-new	call_d	proc_d	auto_l"
//    0001	[0x00ca: ldi   0x00a5]		+00  	____	+002   	+000  	+000  	+000


static void show_process(int proc_depth, addr_t addr, int depth, int call_depth, int auto_level, int step)
{
	char s_operand[20], s_depth_fore[20], s_depth[20];

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

	printf("    %.4d	[%#6.4x: %-5s %s]", step, addr, codes[addr].inst, s_operand);



	if(codes[addr].depth_forward == DEPTH_NONE)
	{
		sprintf(s_depth_fore, "____");
	}
	else
	{
		sprintf(s_depth_fore, "%.4d", codes[addr].depth_forward);
	}



	if(depth == DEPTH_NONE)
	{
		sprintf(s_depth, "_____");
	}
	else
	{
		sprintf(s_depth, "%+.4d", depth);
	}

	printf("		%+.2d  	%s	%s", codes[addr].delta, s_depth_fore, s_depth);


	printf("	%+.3d ", call_depth);
	printf("	%+.3d  ", proc_depth);
	printf("	%+.3d", auto_level);
}


static void print_track(int proc_depth)
{
	int i;
	addr_t addr;
	char s_operand[20];

	printf("\n\n\t ---------------- Process track ------------------------------\n");

	for(i = 0; i <= proc_depth; i++)
	{
		addr = proc_track[i].addr;

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

		printf("		%.4d step(%.4d)  [%#.4x: %-5s %s]\n", i, proc_track[i].step, addr, codes[addr].inst, s_operand);
	}
	printf("\t ---------------------------------------------------------------\n");
}

static int compare_track(int former, int later)
{
	addr_t addr = proc_track[later].addr;

	former--;
	later--;

	for( ; former >= 0; former--, later--)
	{
		if(proc_track[former].addr == addr) //ending with the origin addr, found duplication
		{
			return(former);
		}

		if(proc_track[former].addr != proc_track[later].addr) //difference
		{
			return(-1);
		}
	}

	return(-1);
}

static int check_recursive(int proc_depth)
{
	int i, j;
	addr_t addr;

	addr = proc_track[proc_depth].addr;

	for(i = proc_depth - 1; i >= 0; i--)
	{
		if(proc_track[i].addr == addr)
		{
			if((j = compare_track(i, proc_depth)) > 0)
			{
				if(compare_track(j, proc_depth)) //two consequtive
				{
					return(~0);
				}
			}
		}
	}

	return(0);
}

static void log_process(int proc_depth, addr_t addr, int depth_forward, int call_depth, int auto_level, int step)
{
	if(proc_depth >= MAX_STEP)
	{
		error_exit("processing too deep %d\n", proc_depth);
	}

	proc_track[proc_depth].addr = addr;
	proc_track[proc_depth].step = step;
	proc_track[proc_depth].depth_forward = depth_forward;

	if(check_recursive(proc_depth))
	{
		printf("\n\nCannot handle recusive process (call or loop) @%#.4x\n", addr);
		printf("    %s\n", HEADER);
		show_process(proc_depth, addr, depth_forward, call_depth, auto_level, step);
		print_track(proc_depth);
		error_exit("\nAborting process, recursive process detected\n");
	}
}


static int process(int proc_depth, addr_t addr, int depth_forward, int call_depth, int auto_level, int step)
{
	int delta, tmp;
	int step_begin;

	step_begin = step;

	__printf("\n\n--->");
	__printf(HEADER);
	__printf("		Forward scan begin, step=%d @%#6.4x\n", step_begin, addr);

	if(codes[addr].inst[0] == '\0')
	{
		//error_exit("\nCannot start without good instruction code @%#6.4x.\n", addr);
		printf("\nCannot start without good instruction code @%#6.4x.\n", addr);

		if(proc_depth == 0)
		{
			return(0);
		}
		else
		{
			printf("\t Terminating this path, depth_forward=%d\n", depth_forward);
			return(~0);
		}
	}

	//forward pass, get all the delta and trace path
	for(;;)
	{
		maxs.proc_iter++;

		//--------------------------------------------------------------------------------
		//show the progress

		__show_process(proc_depth, addr, depth_forward, call_depth, auto_level, step);
		path[step] = addr;

		//-------------------------------------------------------------
		// termination conditions #1
		if(addr >= MAX_INSTRUCTIONS)
		{
			error_exit("processing beyond MAX_INSTRUCTIONS %#.4x\n", addr);
		}

		//watch recursive calls.
		if(codes[addr].depth_forward >= depth_forward)
		{
			__printf("\t BEEN");
			return(~0);
		}
		else if(codes[addr].depth_forward != DEPTH_NONE)
		{
			__printf("\t F++");
		}

		codes[addr].depth_forward = depth_forward; //forward depth when code reached here
		//-------------------------------------------------------------
		// SP change

		delta = 0;


		if(is_sp_write(addr))
		{
			tmp = auto_push(addr, depth_forward, &auto_level, &delta);


			if(tmp == AUTO_NOT)
			{
				tmp = auto_pop(addr, depth_forward, &auto_level, &delta);
			}

			if(tmp == AUTO_ERROR_CRITICAL)
			{
				return(0); //message has been posted from the funcions.
			}

			if((tmp == AUTO_NOT) || (tmp == AUTO_ERROR))
			{
				printf("\n Warning: Writing SP @ %#.4x, not AUTO-VARIABLE model.\n", addr);

				if(call_depth > 0)
				{
					printf("\t Terminating this path, stack depth=%d\n", depth_forward);
					return(~0);
				}
				else
				{
					printf("\t Assuming this is the first SP setting,\n");
				}
			}
		}
		else
		{
			delta = stack_change(addr);
		}

		codes[addr].delta = delta;
		depth_forward += delta;

		maxs.for_depth = __max(depth_forward, maxs.for_depth);


		//-------------------------------------------------------------
		// termination conditions #2
		if(is_ijmp(addr))
		{
			printf("\n Warning: Indirect Jump '%s' from Depth=%d, terminating the path\n", codes[addr].inst, depth_forward);

			printf("\t Terminating this path, stack depth=%d\n", depth_forward);
			return(~0);
		}
		else if(is_ret(addr))
		{
			call_depth--;

			if(call_depth < -1) //tolerate one return for vector was called
			{
				printf("call depth negative @ %#.4x, call_depth = %d\n", addr, call_depth);
			}

			return(~0);
		}
		else if(get_target(addr) == 0) //jump to origin resets the program
		{
			__printf("\t RESET at stack depth=%d\n", depth_forward);

			return(~0);
		}

		//-------------------------------------------------------------
		//warn
		if(codes[addr].inst[0] == '\0')
		{
			//ellipsis is processed in decode()
			printf("\nNo good instruction code @%#6.4x.\n", addr);
			return(0);

		}

		//-------------------------------------------------------------

		//depth from any sub path
		if(is_call(addr))
		{
			call_depth++;
			maxs.call_depth = __max(call_depth, maxs.call_depth);

			log_process(proc_depth, addr, depth_forward, call_depth, auto_level, step);

			if(!process(proc_depth+1, get_target(addr), depth_forward + 2, call_depth, auto_level, step+1)) //caller's depth is not calee's stack depth, except..
			{
				return(0); //terminate
			}

			__printf("\n===>");
			__printf(HEADER);
			__printf("		Forward continue, after CALL (step=%d @%#6.4x)\n", step, addr);

			if(codes[addr].depth_forward == DEPTH_NONE) //this wouldn't happen
			{
				error_exit("\t RET to CALL did not converge, depth_forward = %d\n", codes[addr].depth_forward);
			}
		}
		else if(is_branch(addr) || is_skip(addr))
		{

			log_process(proc_depth, addr, depth_forward, call_depth, auto_level, step);

			if(!process(proc_depth+1, get_target(addr), depth_forward, call_depth, auto_level, step+1))
			{
				return(0);
			}

			__printf("\n===>");
			__printf(HEADER);
			__printf("		Forward continue, after BRANCH (step=%d @%#6.4x)\n", step, addr);
		}
		else
		{
			__printf("\n");

			if(is_icall(addr))
			{
				printf("\n Warning: Indirect Call,[%#6.4x: %s], at stack depth=%d, Sub path processing not done\n\t", addr, codes[addr].inst, depth_forward);
				printf("\n");
			}
		}


		//--------------------------------------------------------------------------------
		//Next line


		step++;
		maxs.step = __max(maxs.step, step);

		if(step >= MAX_STEP)
		{
			error_exit("\nStep through too far, %d @%#.4x\n", step, addr);
		}

		addr = get_next(addr);
	}
}


int process_one(addr_t start_addr, int initial_depth)
{
	int i;

	maxs.auto_level = 0;
	maxs.call_depth = 0;
	maxs.step = 0;
	maxs.for_depth = -1000;
	maxs.proc_iter = 0;

	for(i = 0; i < MAX_INSTRUCTIONS; i++)
	{
		codes[i].delta = 0;
		codes[i].depth_forward = DEPTH_NONE;
	}

	for(i = 0; i < MAX_CALL_LEVEL; i++)
	{
		auto_stack[i].delta = 0;
	}

	if(codes[start_addr].idx_label >= 0)
	{
		printf("\n________ Processing \"%s\" @ %#.4x\n", labels[codes[start_addr].idx_label].label, start_addr);
	}
	else
	{
		printf("\n________ Processing %#.4x\n", start_addr);
	}

	if(!process(0, start_addr, initial_depth, 0, 0, 0))
	{
		printf("\n\t.... terminated Processing with error\n");
		return(0);
	}

	printf("\n\t.... done Processing; ");

	if(codes[start_addr].idx_label >= 0)
	{
		printf("\t <%s> ", labels[codes[start_addr].idx_label].label);
	}
	else
	{
		printf("\t ");
	}

	printf("@ %#.4x\n", start_addr);
	printf("\t stack_depth = %d\n", maxs.for_depth);
	printf("\t call_depth=%d\n", maxs.call_depth);
	printf("\t max step=%d\n", maxs.step);
	printf("\t proc iteration=%ld\n", maxs.proc_iter);
	printf("\t -----------------------------\n\n");

	return(~0);
}

