

#define SetPort(_PORT, _val) asm volatile("out %0, %1" : : "I" (_SFR_IO_ADDR(_PORT)), "a" (_val))
#define SetBitIO(_PORT, _bit) asm volatile("sbi %0, %1" : : "I" (_SFR_IO_ADDR(_PORT)), "M" (_bit))
#define ClrBitIO(_PORT, _bit) asm volatile("cbi %0, %1" : : "I" (_SFR_IO_ADDR(_PORT)), "M" (_bit))
#define NegBitIO(_PORT, _bit) asm volatile("in  __tmp_reg__, %0 \n\t sbrc __tmp_reg__, %1 \n\t  cbi %0, %1 \n\t	sbrs __tmp_reg__, %1 \n\t sbi %0, %1"  : : "I" (_SFR_IO_ADDR(_PORT)), "M" (_bit))

#define SEI asm volatile("sei" : : ) // разрешить все прерывания
#define CLI asm volatile("cli" : : ) // запретить все прерывания
#define NOP asm volatile("nop" : : ) 
#define WDR asm volatile("WDR" : : ) 
