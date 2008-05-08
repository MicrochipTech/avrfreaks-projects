#include "process.h"

struct pulse_data p[MAX_PULSES];	// Storing pulses
struct pulse_data *s[MAX_PULSES];	// Sorted order of  pulse array
struct type_data t[MAX_TYPES];		// Storing pulse types
struct decode_data dec;	// Storing decode data
int pulses = 0;			// Number of pulses captured
int types = 0; 			// Number of pulse types found
char decode_str[MAX_PULSES+20]; // For showing and storing decoded data
int undo_code=-1;	// ID for latest user change


void sort(void) {		// Sort the pulse array (using pointers)
	int i, pass, done;
	struct pulse_data *temp;	// Temporary adress holding variable
	for (i=0;i<pulses;i++) s[i] = &p[i];  	// Store the adresses for sorting
	for (pass=1;pass<pulses;pass++) {
		done = 1;
		for (i=0;i<pulses-1;i++) {
			if (s[i]->len > s[i+1]->len) { // Swap the adresses
				temp = s[i];
				s[i] = s[i+1];
				s[i+1] = temp;
				done = 0;
			}
		}
		if (done) return;  // Quit as soon as all elements are in order
	}
}

void group(void) {	// Find the pulse types that match within limits
	int i,lim;
	types = 0;	// Note: global variable
	t[types].min = s[0]->len;	// Store the shortest pulse value
	for (i=0;i<MAX_TYPES;i++) t[i].num=0;	// Clear counters
	for (i=0;i<pulses-1;i++) {
		lim = s[i]->len + s[i]->len/LEN_TOL;  // Upper limit when comparing to next pulse
		s[i]->type = types;	// Assign the current type being tested
		t[types].num++;	// Increase number of pulses for this type
		if (s[i+1]->len > lim) {  // Is the next pulse a new type?
			t[types].max = s[i]->len; // Store longest length of current pulse type
			if (types<MAX_TYPES-1) types++;// else { printf("Too many pulse types\n"); return;} // Change to next pulse type ORDNE FEILMELDING FRA DENNE!
			t[types].min = s[i+1]->len; // Store the shortest length for next pulse type
		}
	}
	t[types].max = s[pulses-1]->len; // Store the longest pulse value
	s[pulses-1]->type = types;	// Store the last (or single) pulse type
	t[types].num++;	// Increase number of pulses for last (or single) type
}

void quantize(void) {		// Convert pulse types to bits (if within limits)
	int i,temp, min = types; // Min = position of the smallest pulse type concidered a code bit
	int lim1 = pulses/NUM_TOL;	// For testing if a pulse type is frequent enough to concider it part of a code
	for (i=types;i>=0;i--) {
		if (t[i].num > lim1) min = i;	// Find the position of the smallest "code bit"
		temp = t[i].max-t[i].min; // Calc pulse type range
		t[i].min+=temp/2;	// Calc the average lenght of the pulse type
		//if (temp > (t[i].min/RAN_TOL)) {printf("Range error\n"); return; }// The range is greater than [1/tol] of the value itself
	}
	int lim2 = t[min].min/BIT_TOL; // For testing if a pulse type is within the "bit multiples"
	int lim3 = lim2/2;
	for (i=0;i<=types;i++) {
		t[i].bit=0;	// Init (none found yet)
		temp = t[i].min+lim3;
		if ((temp % t[min].min) <= lim2) t[i].bit = temp/t[min].min;  // Må også sjekke at lim1 oppfylles for den som matcher!?
		
	}
}

int find(int *pulse, int n, unsigned int key[]) {	// Check for pattern
	int i,match=0;
	for (i=0;i<dec.num[n];i++) if (i+dec.num[n]<=pulses) 
			if (key[i]==p[i+*pulse].type) match++;
	if (match==dec.num[n]) {*pulse+=dec.num[n]-1; return 1;}	// Jump if found
	return 0;
}

void decode(void) {		// Convert the pulses to decoded text
	int i,j,col=1,pos=0;
	char out, bi_char='-';
	for (i=0;i<pulses;i++) {
		if (p[i].type>9) out=p[i].type+55; 	// Letter symbols (default)
			else out=p[i].type+48;			// Number symbols (default)	
		if (t[p[i].type].repl!=0) out =	t[p[i].type].repl; 	// Char replace
		if (out==10) {bi_char='-'; col=0;}		// New row
		if ((dec.mod==2)&&(col==1)) i++; 	// Offset for biphase mode 2
		if (dec.num[0]) if (find(&i,0,dec.zero)) out='-'; 	// Normal PWM
		if ((dec.num[1])&&(out!='-')) if (find(&i,1,dec.one)) out='*'; 
		if (dec.mod) {	// Bi Phase decoding
			if (out=='*') {				// Toggle output
				if (bi_char=='-') bi_char='*'; else bi_char='-';	
				out=bi_char;										
			}
			if (out=='-') out=bi_char;	// Repeat value
		}
		for (j=1;j<=dec.num[2];j++) if (col==dec.word[j-1]) {
			decode_str[pos]=',';	// User defined word separator
			pos++;
		}
		decode_str[pos]=out;
		pos++;
		col++;
	}
	decode_str[pos]=0;	// Stop character
}

void mdecode(int ed_type, char repl) { // Manually try decode data
	if ((ed_type>-1)&&(ed_type<MAX_TYPES)) {
		t[ed_type].repl=repl;
	} else {
		if (ed_type==200) dec.num[0]=0;	// Undo zero pattern
		if (ed_type==201) dec.num[1]=0;	// Undo one pattern
		if (ed_type==202) dec.num[2]=0;	// Undo group pos
		if (ed_type==250) { 
			if (repl) {
				dec.mod++;		// Change modulation
				if (dec.mod>2) dec.mod=0;
			} else dec.mod=0;	// Undo modulation
		}
	}
	decode();			// To show the result in main
	undo_code=ed_type;	// For undo
}

void clr_dec(void) {	// Clear all the decode data
	int i;
	for (i=0;i<4;i++) dec.num[i]=0;			// Clear decode data
	for (i=0;i<MAX_TYPES;i++) t[i].repl=0; 	// Clear type replacement
	dec.mod=0;	// Clear modulation type
	dec.bas=0;	// Clear number base
	undo_code=-1;
	decode();	// To show the original code again
}


void setgroup(char input) {	// Set the spacings between custom word
	if (dec.num[2]) {
		if ((input=='+')&&(dec.word[dec.num[2]-1]<80))
			dec.word[dec.num[2]-1]++;
		if ((input=='-')&&(dec.word[dec.num[2]-1]>0))
			dec.word[dec.num[2]-1]--;
	}
	if ((input=='g')&&(dec.num[2]<8)) {
		dec.num[2]++;
		dec.word[dec.num[2]-1]=0;	// Clear the new pos
		if (dec.num[2]>1)		// Start at the previous position
			dec.word[dec.num[2]-1]=dec.word[dec.num[2]-2];
	}
	undo_code=202;
}


