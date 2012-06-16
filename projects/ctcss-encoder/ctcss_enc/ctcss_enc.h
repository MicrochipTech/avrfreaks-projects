//----------------
// Für Drehgeber -
//----------------
extern unsigned char table[4][4];
extern unsigned char position; // zaehlen wir mal die absolute Position
extern volatile unsigned char quadrature_input; // bit 0 und bit 1 sind Quadratureingaenge
extern unsigned char new_quadrature_value, last_quadrature_value;
extern unsigned char rot_has_changed;
extern char T_ACTIVE;
extern unsigned char key;				// welche Taste wurde gedrückt


