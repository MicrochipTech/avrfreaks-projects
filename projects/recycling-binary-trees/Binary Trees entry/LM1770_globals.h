// LM1770_globals.h - Common stuff for LM1770 demo program

#ifndef __LM1770_GLOBALS_H__
#define __LM1770_GLOBALS_H__

#define NULL 0
#define CLOCK_RATE    120         // potentiometer sample rate
#define PUSH_BUTTON   GPIO_PIN_4  // GPIOs
#define USER_LED      GPIO_PIN_5
#define ROOTX					48					// (x,y) of root node
#define ROOTY					4

// Get a new random number via a fast linear congruence generator
#define NEXT_RAND(x)  (((x) * 1664525) + 1013904223)

//	the following structure is in effect for nodes when they are
//	active and not in the garbage list

struct nodedata {
	int nnum;										// node number
	unsigned long stuff;				// any other node data goes here
	unsigned char other_stuff;
};

struct bt_node {
  struct bt_node *ptr[2];			// left & right child pointers
  union {
  	struct bt_node *nxt;			// link pointer while in the dumps
		struct nodedata n;				// while we're swinging in the trees
	} u;
};

extern unsigned char bpress;    // whether button was pressed
extern unsigned char bnow;      // current button state (debounced)
extern unsigned long pwheel;    // current potentiometer value
extern unsigned char gbuf[];		// graphics frame buffer
extern struct bt_node *garb;		// head, garbage pool
extern struct bt_node *tptr;		// head, main tree
extern int node_grab;						// nodes to allocate at a time

// function declarations
extern struct bt_node *gimme(void);
extern void toss_me(struct bt_node *);
extern void garb_init(void);
extern void sys_init(void);
extern void ncount(struct bt_node *, int *);
extern int count_tree(void);
extern int count_garb(void);
extern void draw_tree(void);
extern void wait_some(void);
extern void construct_leaf(void);
extern void destruct_node(void);
extern struct bt_node *traverse(int, int *, int *);
extern int randrange(int);
extern void plot_point(int, int);
extern void plot_hline(int, int, int);
extern void ndraw(struct bt_node *);
extern void plotnode(struct bt_node *);
extern void getnodexy(int, int *, int *, int *);

#endif
