/*
Interface to console module
*/
	#define CONSOLE_INPUT_FIFO_SIZE 16

	enum console_device_enum {CONSOLE_DEVICE_UART0, CONSOLE_DEVICE_UART1, CONSOLE_DEVICE_OTHER};

	struct console_struct
    {
		struct fifo_control input;
		struct fifo_control *output_ptr;
		char echo;
		enum console_device_enum input_device;
		enum console_device_enum output_device;
	};

//Public variables
	extern struct console_struct console_vars;
	
//Prototypes	
	void console_init(void);
	void console_main(void);
	void console_timer(void);
	
