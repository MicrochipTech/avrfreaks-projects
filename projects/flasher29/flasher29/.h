#define ce_down PORTD&=~0x10
#define ce_up PORTD|=0x10
#define oe_down PORTD&=~0x20
#define oe_up PORTD|=0x20
#define we_down PORTD&=~0x40
#define we_up PORTD|=0x40
#define led_on PORTD&=~0x80
#define led_off PORTD|=0x80
void chip_erase(void);
void test_latch(void);
uint8_t read_byte(uint32_t adr);
void program_byte(uint32_t adr, uint8_t c);
uint8_t programm_block(uint32_t adr, uint16_t len, uint8_t *block);

