// DesignStellaris2006 Entry - Demonstration Program
//
// Entry: LM1770
// Chuck Baird, http://www.cbaird.net
// This code may be free used
//
// This program randomly builds and destroys nodes in a binary tree
// and displays the results on the LM3S811's display. It uses the
// binary tree node garbage collection and allocation algorithm
// outlined in the LM1770 entry documentation.
//
// Note: most of the hardware initialization code and ADC interrupt
// handler were copied from the LMI quick start application for the
// LM3S811 Evaluation Board, as was the random number generation
// support. This code is Copyright (c) 2006 Luminary Micro, Inc.

#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_types.h"
#include "adc.h"
#include "debug.h"
#include "gpio.h"
#include "interrupt.h"
#include "sysctl.h"
#include "timer.h"
#include "support/osram96x16.h"
#include "LM1770_globals.h"
#include "random.h"
#include "stdlib.h"

unsigned char bpress;			// whether the button was pressed (0/1)
unsigned char bnow;				// current debounced button state (0/1)
unsigned char bcnt;				// button debounce counter
unsigned char systick;		// clock tick indicator
unsigned long pwheel;			// potentiometer value
unsigned char gbuf[192];	// graphics display buffer
unsigned char ss[3];			// tree & node counts string buffer
int node_grab;						// number of nodes to malloc() at a time
struct bt_node *garb;			// head of garbage linked list
struct bt_node *tptr;			// head of tree (points to root node)

// ====================================================================
// main - mainline
//
//	we generate 63 nodes and put them into the available pool
//	for allocation. starting with an empty tree, we enter an
//	endless loop:
//
//		pick a leaf (out of 32 possible), and generate all the
//		nodes necessary (including endpoints) to connect the root
//		to that leaf. display the resulting tree, then pause for
//		a period of time determined by the potentiometer wheel.
// 	
//		if the user is not pressing the button, pick a node (out of
//		63 possible nodes). if that node exists, delete it and its
//		children. if it is an only child, recursively delete its
//		parent, thus trimming off any one-child branches above it.
//		display the resulting tree, and pause.
//
//	rinse and repeat.
//
//	the number of nodes in the tree and the number of nodes in
//	the garbage pool are counted and displayed with the tree.
//	these two numbers should always add up to 63.
//
//	having the option of suppressing the node destruction phase
//	allows the user to build a more robust tree prior to calling
//	in the whack-a-node guys.

int main(void)
{
	sys_init();								// initialize the system stuff
	bcnt = bpress = bnow = 0;	// button press & debouncing
	garb = NULL;							// no garbage pool yet
	garb_init();							// make the nodes & throw them away
	tptr = NULL;							// nothing in the tree
	draw_tree();							// display the tree & counts

  while(1) {
	  construct_leaf();				// maybe add a leaf
		draw_tree();						// draw that
		wait_some();						// wait a while
		if (bnow) {							// if they're not pushing the button...
			destruct_node();			// maybe knock off a node
			draw_tree();					// draw that
			wait_some();					// hang out
		}
	}
}

// ====================================================================
//	the famous algorithm: toss_me() and gimme()
//
//	these two routines implement the algorithm to allocate nodes
//	and reclaim nodes and/or subtrees.
//
//	the variable garb is the head of a linked list (possibly
//	empty) of available nodes, and each node can have subtrees
//	attached to it through the child pointers. if a request
//	for node allocation occurs when the garbage pool is empty,
//	malloc() is called to obtain node_grab nodes, and they are
//	linked into the available node list.
//
//	for this application, the allocator dies with a distinctive
//	display screen if malloc() fails or if node_grab is zero
//	at the time of allocation. either of these is a fatal
//	error here, although in other settings would simply return
//	an error flag.
//
// -------------------------------------------
// toss_me - put a node into the garbage pool
//
//	this links a node (with or without children) into the
//	linked list of available nodes. the newly added node
//	goes at the front of the list.
//
//	the variable garb is the head of the linked list.
//
void toss_me(struct bt_node *p) {
  (*p).u.nxt = garb;			// we point to old head
  garb = p;								// head points to us
}

// -------------------------------------------
// gimme - get a node from the garbage pool
//
//	this routine returns a pointer to the first node in
//	the garbage list, and relinks its children (if any)
//	and the remaining nodes. the returned nodes has
//	undefined data in all fields.
//
//	if there are no available nodes, some will be dynamically
//	created and all but the returned one put in the garbage pool.
//	the variable node_grab tells how many to create, and can
//	be altered by the caller to fit the circumstances.
//
//	for this application, having node_grab == 0 or being
//	unsuccessful in obtaining heap memory causes ungraceful death
//	if new node creation is attempted.
//
struct bt_node *gimme(void) {
	struct bt_node *rtn, *p, *ww[3];
	int i, n, who[3];

	rtn = garb;								// the node we'll return
	if (rtn) {
	  n = 0;									// count the # of non-null pointers
		for (i = 0; i < 2; i++) {		// child pointers
			p = (*rtn).ptr[i];
		  if (p) {
				ww[n] = p;
				who[n++] = i;
			}
		}
		if ((*rtn).u.nxt) {				// linked list pointer
			ww[n] = (*rtn).u.nxt;
			who[n++] = 2;
		}

//	the relinking strategy depends upon how many non-null
//	pointers there were in the node we're returning

		switch (n) {
		  case 1:
				garb = ww[0];							// head --> our target
				if (who[0] < 2)
					(*garb).u.nxt = NULL;	// if child, no next pointer
				break;

			case 2:
				garb = ww[0];							// head --> first target
				p = ww[1];								// 2nd target
				(*garb).u.nxt = p;				// 1st --> 2nd
				if (who[1] == 1)					// if 2nd is a child, define
				  (*p).u.nxt = NULL;			// its nxt pointer
				break;

			case 3:
				garb = ww[0];								// head --> first child
				p = ww[1];									// second child
				(*garb).u.nxt = p;					// ch1.next --> second child
				(*p).u.nxt = ww[2];					// ch2.next --> orig next
				break;

			default:
				garb = NULL;					// last node in the pantry
				break;
		}	

//	the following code handles the allocation of new memory

  } else {												// no available nodes
		if (node_grab) {							// should we make some?
		  p = malloc(node_grab * sizeof(struct bt_node));
			if (p) {										// we got our nodes
 				for (i = 0; i < node_grab; i++) {
				  if (i) {
					  (*p).ptr[0] = NULL;		// left child pointer
						(*p).ptr[1] = NULL;		// right child pointer
						toss_me(p);						// put it in the pool
					} else {
					  rtn = p;							// return the first node
					}
					p++;										// ready for next one
				}
			} else {										// malloc() failed
				for (i = 0; i < 192; i += 4) 
		  		*(unsigned long *) (gbuf + i) = 0x90909090;	// a pattern
				OSRAMImageDraw(gbuf, 0, 0, 96, 2);						// display
				while (1) ;																		// & die
			}
		} else {									// no - don't make any
				// this condition is an error and we'll simply
				// put a nastiness on the display and die.
			for (i = 0; i < 192; i += 4) 
	  		*(unsigned long *) (gbuf + i) = 0x5b5b5b5b;	// some pattern
			OSRAMImageDraw(gbuf, 0, 0, 96, 2);						// displayed
			while (1) ;																		// as a last gasp
		}
	}
	(*rtn).ptr[0] = NULL;				// be discrete (not required)
	(*rtn).ptr[1] = NULL;				// and help prevent accidents
	(*rtn).u.nxt = NULL;
	return (rtn);
}
 
// ====================================================================
// wait_some - kill some time
//
//	this uses the current potentiometer setting to delay from
//	about 1/60th of a second to about 3 seconds
//
//	there are CLOCK_RATE interrupts per second, each of which
//	sets systick = 1. we just hang around waiting for the
//	interrupts and count them off; sort of a government job.
//
void wait_some(void) {
	unsigned long wt, i;

	wt = pwheel * CLOCK_RATE * 3 / 1024	+ 2;
	for (i = 0; i < wt; i++) {
	  while (!systick);
		systick = 0;
	}
}

// ====================================================================
// construct_leaf - choose a leaf and add it to the tree
//
//	if the nodes are considered to be numbered 1 to 63, the
//	bit pattern designates the path to the node. ignoring the
//	leading zeroes, the leftmost 1 indicates the root node.
//	then, proceeding to the right, a 0 indicates the left child,
//	and a 1 indicates the right child. when the bits are exhausted
//	the target node has been reached.
//
//	a binary tree with 63 nodes is 5 levels deep. the leaf nodes
//	are at the 5th level, which means they are the binary numbers
//	10000 through 11111, or 32 to 63 decimal. therefore this
//	routine generates a random number from 32 to 63 inclusive,
//	then traverses the tree from the (possibly non-existent) root
//	to it, creating any nodes that do not already exist.
//
void construct_leaf(void) {
  int r, who, fail;
	struct bt_node *p, *q;

	r = randrange(1);					// get a leaf node number (32 - 63)

	who = 0;									// returned # of actual node found
	p = traverse(r, &who, &fail);		// try to find the node
	while (r != who) {				// wanted r, found who: same?
		q = gimme();						// no - get a new node
		(*q).ptr[0] = NULL;			// no minors in the bar area
		(*q).ptr[1] = NULL;

		if (who) {							// do we have any tree at all?
			(*p).ptr[fail] = q;		// yes - new node is a kid
			(*q).u.n.nnum = ((*p).u.n.nnum << 1) | fail;	// node number
		} else {
			tptr = q;							// no - this is the new root
			(*q).u.n.nnum = 1;		// root is node #1			
		}
	  p = traverse(r, &who, &fail);	// and try again
	}
}

// ====================================================================
// destruct_node - choose a node and delete it if it exists
//
//	in this function a random number between 1 and 63 inclusive
//	is generated. the tree is traversed (see discussion in the
//	function construct_leaf()) to see if that node exists. if it
//	does, the node and its subtrees, if any, are deleted.
//
//	the deleted node's parent is checked to see if it is an only
//	child, and, if it is, the parent is also deleted. this process
//	repeats recursively.
//
//	if the nodes are numbered 1 to 63, the parent node's number is
//	found by right shifting (dividing by 2) the child node number.
//
void destruct_node(void) {
  int r, who, fail, br, nbr;
	struct bt_node *p, *q;

	r = randrange(0);					// get any node number (1 - 63)

	who = 0;									// returned # of actual node found
	p = traverse(r, &who, &fail);		// try to find the node
	if (r == who) {						// if not there, we're done
		while (1) {							// check up the ladder
			toss_me(p);						// give it up
			if (r < 2) {
				tptr = NULL;				// we tossed the root
				return;
			} else {

//	we have to delete the parent's pointer to us, but if
//	it turns out we were an only child, we'll just delete
//	the parent instead. "have mercy, your honor, I'm an
//	orphan!" "well, duh - you killed your parents."

				br = r & 0x01;				// bit 0: L or R branch to get here
				nbr = br ^ 1;					// the road less travelled
				r >>= 1;							// parent# = child# / 2
				p = traverse(r, &who, &fail);	// find the parent
				(*p).ptr[br] = NULL;	// kill pointer to us
				q = (*p).ptr[nbr];		// sibling pointer
				if (q) return;				// if there's a sib, we're done
			}		
		}
	}
}

// ====================================================================
// randrange - return random number within a range, inclusive
//
//	r = 0, return 1 to 63
//	r = 1, return 32 to 63
//
int randrange(int r) {
  unsigned long rr;

	rr = RandomNumber();
	if (r) {
	  r = 0x20 + (rr & 0x1f);			// 32 to 63
	} else {
	  while (!(rr & 0x3f)) {			// don't allow 0
		  rr = NEXT_RAND(rr);
		}
		r = rr & 0x3f;							// 1 to 63
	}
	return (r);
}

// ====================================================================
// plot_point - light up a point on the gbuf grid
//
//	x - 0 to 95
//	y - 0 to 15
//
//	upper left hand corner is (0,0), lower right is (95,15)
//
void plot_point(int x, int y) {
	if (x >= 0 && x < 96) {
  	y &= 0x0f;								// crude, but effective
		if (y > 7) {							// top or bottom half?
		  x += 96;								// bottom follows top
			y -= 8;									// and is numbered 0 - 7
		}
		gbuf[x] |= (1 << y);			// light up that bit
	}
}

// ====================================================================
// plot_hline - plot a horizontal line on the gbuf grid
//
//	x1, x2 - 0 to 95 (either order)
//	y - 0 to 15
//
void plot_hline(int x1, int x2, int y) {
  int i;

	if (x1 > x2) {
	  i = x1;
		x1 = x2;
		x2 = i;
	}
	if (x1 >= 0 && x2 < 96) {
	  for (i = x1; i <= x2; i++) plot_point(i, y);
	}
}
 
// ====================================================================
// traverse - attempt to locate a node by node number
//
//	nodenum - the number of the node to locate (1 to 63)
//	*who - returned number of last seen existing node
//	*fail - the child (0 or 1) that couldn't be found
//	return - pointer to node, or NULL
//
//	if *who == nodenum on return, then the node was located
//		and the returned pointer indicates the target.
//	if *who != nodenum on return, then the pointer and *who
//		indicate the last existing node in the path to the target.
//	if *who == 0 and the pointer is NULL, the tree is empty
//		or the nodenum was invalid.

struct bt_node *traverse(int nodenum, int *who, int *fail) {
  struct bt_node *p, *q;
	unsigned int mask;

	p = NULL;										// haven't found root yet
	*who = 0;										// build node #s here as we find them
	*fail = 0;									// which child failed us
	if (!tptr) return (p);			// make like a tree and leaf

	mask = 0x20;								// bit 4 set (5th level leaves)

	while (mask) {
		if (nodenum & mask) {			// is the masked bit is set?
			if (p) {								// if p not NULL, we're rolling...
			  q = (*p).ptr[1];			// is there a right child?
			  if (q) {							// if so, continue
					*who = ((*who) << 1) + 1;	// update node number
					p = q;							// jump to right child
				} else {							// if not, we're done
					*fail = 1;					// mom liked you the best
					return (p);
				}
			} else {								// p is NULL - it's the 1st set bit
			  p = tptr;							// first set bit - start at root
				*who = 1;							// root is node #1
			}
		} else {									// the masked bit is cleared
		  if (p) {								// if p is NULL, ignore it
			  q = (*p).ptr[0];			// otherwise, left child?
			  if (q) {							// if so, continue
					*who = (*who) << 1;	// update node number
				  p = q;							// jump to left child
				} else {							// if not, we're done
				  return (p);
				}
			}
		}
	  mask >>= 1;								// next bit to the right
	}
	return (p);
}

// ====================================================================
// tdraw - draw the tree
//
//	this blanks out the display buffer, then traverses the
//	tree. for each node found, the node itself (a single pixel)
//	is lit. for all but the root node, a horizontal line is
//	drawn in the row above the node, where the line runs from
//	above the child node to under the parent node (which is two
//	rows above the child). a picture is worth 0x03e8 words:
//
//                            P
//                     xxxxxxxx
//                     C
//
//	where P is the parent node and C is the child node.
//
//	after the tree is traversed, the image is displayed. then
//	the tree nodes and garbage pool nodes are counted and the
//	counts written to the display. these two numbers should
//	always add to 63.
//
void draw_tree(void){
	int i;

	for (i = 0; i < 192; i += 4) {	// blank the display buffer
	  *(unsigned long *)(gbuf + i) = 0;
	}
	ndraw(tptr);								// draw a node and traverse
	OSRAMImageDraw(gbuf, 0, 0, 96, 2);

	i = count_tree();						// how big is the tree?
	ss[0] = '0' + ((i / 10) % 10);
	ss[1] = '0' + (i % 10);
	ss[2] = '\0';
	OSRAMStringDraw((const char *) ss, 1, 0);

	i = count_garb();						// how many available nodes?
	ss[0] = '0' + ((i / 10) % 10);
	ss[1] = '0' + (i % 10);
	ss[2] = '\0';
	OSRAMStringDraw((const char *) ss, 83, 0);
}

// ====================================================================
// ndraw - draw a node & traverse
//
//	recursively traverse the tree
//
void ndraw(struct bt_node *p) {
  if (p) {
		plotnode(p);							// plot the node and line
	  ndraw((*p).ptr[0]);				// left subtree
		ndraw((*p).ptr[1]);				// right subtree
	}
}

// ====================================================================
// plotnode - draw a node and connecting line
//
//	each node has a node number (1 - 63) in its data
//	structure. that number is converted to a (x,y) coordinate
//	and the length of line segment above it to connect to its
//	parent. that length depends upon the depth of the node,
//	that is, how far away from the root it is.
//
void plotnode(struct bt_node *p) {
  int x, y, nn, lng;

	nn = (*p).u.n.nnum;								// node number
	getnodexy(nn, &x, &y, &lng);			// get (x,y) of node
	plot_point(x, y);									// plot the node
	if (nn > 1) {											// root has no line above it
		if (nn & 1) lng = -lng;					// on even nodes, line goes left
  	plot_hline(x, x + lng, y - 1);	// draw the line
	}
}

// ====================================================================
// getnodexy - locate a node on the grid
//
//	n - node number (1 to 63)
//	*x, *y - returned coordinates on grid
//	*lng - returned length of line segment above node
//
//	the root node is in a defined location (ROOTX, ROOTY).
//	we'll start there, then traverse to our target root,
//	adding or subtracting an offset to x at each level,
//	depending on whether we're taking the right or left
//	child branch. the offset starts at 16 for level 2, and
//	halves each succeeding level.
//
void getnodexy(int n, int *x, int *y, int *lng) {
  int lev, nn, offset, mask;

	n &= 0x3f;								// paranoia strikes deep
	lev = 0;									// get level (1-5) of this node
	nn = n;
	while (nn) {							// the depth (lev) is the bit#
	  nn >>= 1;								// of the leftmost set bit, plus 1
		lev++;
	}
	*lng = (1 << (6 - lev)) + 1;		// lng: 33,17,9,5,3 for lev 1-5

	*x = ROOTX;								// predefined (x,y) for root
	*y = ROOTY;

//	now we add or subtract x offsets at each level, depending
//	on whether we're taking the left or right child branch
//	to get to the next level. this branch info is encoded as
//	the bit pattern of the node number.

	if (lev > 1) {
		offset = 16;							// offset from level 1 to level 2
		mask = 1 << (lev - 2);		// position after 1st set bit

		while (lev > 1) {
			if (n & mask) {					// the bit for this level
			  *x += offset;					// 1 = right child
			} else {
				*x -= offset;					// 0 = left child
			}
			offset >>= 1;						// offset halves each level
			mask >>= 1;							// bit we want is next to the right
			*y += 2;								// nodes are on every other row
			lev--;									// tells us when to quit
		}
	}
}

// ====================================================================
// count_tree - count the nodes in the tree
//
int count_tree(void) {
	int cnt;

	cnt = 0;							// start with nothing
	ncount(tptr, &cnt);		// it's all one tree
	return (cnt);
}

// ====================================================================
// count_garb - count the nodes in the garbage pool
//
//	we need to count any subtrees for each node in the
//	linked list.
//
int count_garb(void) {
  int cnt;
	struct bt_node *p;

	cnt = 0;							// none yet
	p = garb;							// head of garbage linked list
	while (p) {						// walk the linked list
	  ncount(p, &cnt);		// count each node & its subtrees
		p = (*p).u.nxt;			// next in linked list
	}
	return (cnt);
}

// ====================================================================
// ncount - recursively counts nodes in a subtree
//
//	this traverses the tree, counting nodes as it finds them.
//
void ncount(struct bt_node *p, int *cnt) {
	if (p) {
		(*cnt)++;										// count this node
		ncount((*p).ptr[0], cnt);		// count left subtree
		ncount((*p).ptr[1], cnt);		// count right subtree
	}
}

// ====================================================================
// garb_init - initialize all nodes and make them available
//
//	we're going to force the node allocation routine to make 63
//	single nodes (no children) and put them in the garbage pool.
//	we'll then make running out of nodes a fatal error.

void garb_init(void) {
	struct bt_node *p;

	node_grab = 63;					// tell gimme() to make 63 nodes at a time
	p = gimme();						// create them and give us one
	(*p).ptr[0] = NULL;			// kill off its children
	(*p).ptr[1] = NULL;
	toss_me(p);							// give it back
	node_grab = 0;					// make it a fatal error to run out
}

// ====================================================================
// sys_init - various system initialization thingies
//
//	note: this is all borrowed, very slightly modified, from the
//	evaluation board sample application code
//
void sys_init(void) {
    // Set the clocking to run at 20MHz from the PLL.
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL |
			SYSCTL_OSC_MAIN | SYSCTL_XTAL_6MHZ);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);  // Enable peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);  // ???
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    // Configure the ADC to sample the potentiometer when the
		// timer expires. After sampling, the ADC will interrupt the
		// processor; this is used for debouncing and timing.
    //
    ADCSequenceConfigure(ADC_BASE, 0, ADC_TRIGGER_TIMER, 0);
    ADCSequenceStepConfigure(ADC_BASE, 0, 0,
                             ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC_BASE, 0);
    ADCIntEnable(ADC_BASE, 0);
    IntEnable(INT_ADC0);

    // Configure the first timer to generate a 10 kHz PWM signal for driving
    // the user LED.

    TimerConfigure(TIMER0_BASE, TIMER_CFG_16_BIT_PAIR | TIMER_CFG_B_PWM);
    TimerLoadSet(TIMER0_BASE, TIMER_B, (SysCtlClockGet() / 10000) - 1);
    TimerMatchSet(TIMER0_BASE, TIMER_B, 0);
    TimerControlLevel(TIMER0_BASE, TIMER_B, true);
    TimerEnable(TIMER0_BASE, TIMER_B);
    //
    // Configure the second timer to generate triggers to the ADC to sample the
    // potentiometer.
    //
    TimerConfigure(TIMER1_BASE, TIMER_CFG_32_BIT_PER);
    TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet() / 120);
    TimerControlStall(TIMER1_BASE, TIMER_A, true);
    TimerControlTrigger(TIMER1_BASE, TIMER_A, true);
    TimerEnable(TIMER1_BASE, TIMER_A);

    // Configure the LED and push button
    GPIODirModeSet(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_DIR_MODE_HW);
    GPIODirModeSet(GPIO_PORTC_BASE, PUSH_BUTTON, GPIO_DIR_MODE_IN);
    GPIODirModeSet(GPIO_PORTC_BASE, USER_LED, GPIO_DIR_MODE_OUT);
    GPIOPinWrite(GPIO_PORTC_BASE, USER_LED, 0);

    OSRAMInit(true);	// Initialize the OSRAM OLED display
}

// ====================================================================
// ADCIntHandler - ADC interrupt handler
//
// the first few lines of this were lifted from the evaluation
// board distribution code
//
void ADCIntHandler(void)
{
	unsigned long ulData;

	ADCIntClear(ADC_BASE, 0);	    // Clear the ADC interrupt
	ADCSequenceDataGet(ADC_BASE, 0, &ulData);	// Read the data from the ADC
	RandomAddEntropy(ulData);	// Add ADC data to random number entropy pool

	// Pass the ADC data through the low pass filter (with a coefficient of
	// 0.9) to update the position of the potentiometer.
	pwheel = ((pwheel * 58982) + (ulData * 6554)) / 65536;

	// Read the push button.
	ulData = GPIOPinRead(GPIO_PORTC_BASE, PUSH_BUTTON) ? 1 : 0;

	if (ulData != bnow) {
		bcnt += 1;								// increment debounce counter
		if (bcnt > 3) {
			bnow ^= 1;							// toggle the current state
			bcnt = 0;								// not really necessary...
			if (bnow) bpress = 1;		// just pressed?
		}
	} else {
		bcnt = 0;									// start over counting
	}
	systick = 1;								// interrupts happen
}
