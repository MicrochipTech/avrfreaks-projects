#define MAX_PULSES 256	// Number of pulses allowed in the bitstream
#define MAX_TYPES 16	// Number of pulse types allowed in bitstream
#define MAX_FRAMES 16	// Number of frame parameters (for check) Not used yet
 
#define LEN_TOL 4		// 1/tol = deviation for pulses of same type
#define RAN_TOL 3		// 1/tol = total deviation for pulses of same type (not used yet?)
#define BIT_TOL 3		// 1/tol = deviation for comparing bit multiples
						// example: shortest type/next type = 2 within 10% -> two bit pulse
#define NUM_TOL 10		// 1/tol = number of same pulse type for considering it a code bit
						// example: 10% of the pulses must be same type if they are part of a code

struct pulse_data {
	unsigned int len;	// Measured pulse length
	unsigned int type;	// Assigned pulse type (after processing) Store timer status before procesing.
};

struct type_data {
	unsigned int min;	// Shortest length for this pulse type 
	unsigned int max;	// Longest length for this pulse type 
	unsigned int num;	// How many of this pulse type
	unsigned int bit;	// How many multiples of the shortest pulse (after processing)
	char repl;			// Text replacement for long pulse types, (space, CR etc);
};

struct decode_data {
	unsigned int zero[8];	// Pulse type sequence for a zero
	unsigned int one[8];	// Pulse type sequence for a one
	unsigned int word[8];	// User defined space positions in the sequence (nibble, byte etc)
	unsigned int num[4];	// Count for the above members
	unsigned int mod;		// Modulation type
	unsigned int bas;		// Convert to number base
};

extern struct pulse_data p[MAX_PULSES];	// Storing pulses
extern struct pulse_data *s[MAX_PULSES];	// Sorted order of  pulse array
extern struct type_data t[MAX_TYPES];		// Storing pulse types
extern struct decode_data dec;	// Storing decode data
extern int pulses;			// Number of pulses captured
extern int types; 			// Number of pulse types found
extern char decode_str[MAX_PULSES+20]; // For showing and storing decoded data
extern int undo_code;	// ID for latest user change

extern void sort(void); // Sort the pulse array (using pointers)
	
extern void group(void);		// Find the pulse types in the pulse array

extern void quantize(void);	// Convert pulse types to bits (if within limits)

extern void decode(void);	// Convert the pulses to decoded text

extern void mdecode(int ed_type, char repl); // Manually try decode data

extern void clr_dec(void); 	// Clear all the decode data

void setgroup(char input); 	// Set the spacings between custom word



