#define sbi(port,bit) (port |= 1<<bit) //set bit in IO register
#define cbi(port,bit) (port &= ~(1<<bit)) //clear bit in IO register
