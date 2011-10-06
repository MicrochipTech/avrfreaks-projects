
#define VERSION_STRING	"V1.0"

#define TIMER_TICK 0.016384

// Debug outputs
/*
	It is advisable to create different levels of debug output.
	You should avoid exposing the end user to anything they do not understand, in order to minimise the time spent on the phone listening to dribble.
	You can do this by writing to different buffer pointers, and simply loading them with NULL or pointing them at a uart depending on the level required.
*/

extern struct fifo_control *debug_fifo_ptr;

// Public Prototypes
void main_fly(void);
int hardware_id(void);
char* hardware_name(char* textbuf);
