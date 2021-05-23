
//UNO slave address has been set to 3


float FPN_number_from_IO(void);
float Float_from_KBD(void);
long Int_number_from_IO(void);
long Int_from_KBD(void);
float float_num_to_display(float);
char int_num_to_display(long);

void blank_dislay(void);
char data_from_IO(void);
char data_from_KBD(void);




char * Char_ptr;                                                                //Used to convert a FPN to its individual bytes                                        
unsigned char num_bytes[4];                                                     //The four bytes of a FPN
unsigned char data_buff[12];                                                    //Used to hold numerical strings

char scroll_control, dp_control, neg_sign, exp_control;                         //Used to control data entry via IO 
volatile char Data_Entry_complete, digit_entry;

volatile char num_type;                                                         //Floating point or integer

volatile char cr_keypress;                                                      //Used to control data transfer over the I2C bus
volatile char active_transaction;