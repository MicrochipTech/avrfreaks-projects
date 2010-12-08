#ifndef io_H
#define io_H

#define KEY_NONE  0
#define KEY_LEFT	1
#define KEY_RIGHT	2

void InitIO (void);
unsigned char GetKey (void);
void IOUpdate (void);

#endif //io_H
