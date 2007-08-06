#ifndef LEDMtrx_H
	#define LEDMtrx_H

	#define LED_CTRL_PORT	PORTB	// PORT for Max7219 control signals
	#define LED_CTRL_DDR	DDRB	// DDR register of LED_CTRL_PORT
	#define LED_CTRL_DATA	PB0		// pin for Max7219 Data line
	#define LED_CTRL_CLK	PB1		// pin for Max7219 Clock line
	#define LED_CTRL_CS0	PB2		// pin for Max7219 Controller 0 Chip Select
	#define LED_CTRL_CS1	PB3		// pin for Max7219 Controller 1 Chip Select
	#define LED_CTRL_CS2	PB4		// pin for Max7219 Controller 2 Chip Select
	#define LED_CTRL_CS3	PB5		// pin for Max7219 Controller 3 Chip Select


	// Max7219 command registers
	#define REG_DECODE        0x09          // "decode mode" register
	#define REG_INTENSITY     0x0a          // "intensity" register
	#define REG_SCAN_LIMIT    0x0b          // "scan limit" register
	#define REG_SHUTDOWN      0x0c          // "shutdown" register
	#define REG_DISPLAY_TEST  0x0f          // "display test" register

	#define INTENSITY_MIN     0x00          // minimum display intensity
	#define INTENSITY_HALF    0x07          // half display intensity
	#define INTENSITY_MAX     0x0f          // maximum display intensity

	// function prototypes
	void LEDInitHW(void);

	void LEDSetXAddress(uint8_t xAddr);
	void LEDSetYAddress(uint8_t yAddr);


	void Clear_Screen(uint8_t screen);
	void Show_Screen(uint8_t screen);
	void Disp_Char(uint8_t screen, uint8_t character, uint8_t padding);


	void Trans_1(void);
	void Trans_2(void);
	void Trans_3(void);
	void Trans_4(void);

	static void MAX7219_Write (uint8_t chip, uint8_t reg_number, uint8_t data);
	static void MAX7219_SendByte (uint8_t dataout);

	//! Initialize the display, clear it, and prepare it for access
	void MAX7219_Init (void);
	void MAX7219_ShutdownOn (uint8_t chip);
	void MAX7219_ShutdownOff (uint8_t chip);
	void MAX7219_DisplayTest (uint8_t chip);
	void MAX7219_DisplayNorm (uint8_t chip);
	void MAX7219_SetBrightness (uint8_t chip, char brightness);
	void MAX7219_Clear (uint8_t chip);

	void Clear_Screen(uint8_t screen);
	void Show_Screen(uint8_t screen);

	void Copy_Screen(uint8_t Source, uint8_t Dest);
	void delay(uint8_t time);
	void set_led(uint8_t screen, uint8_t col, uint8_t row, uint8_t state);

#endif
