//Timer de 16 bits

#define Timer_16bit_Clk_1				0x01
#define Timer_16bit_Clk_8				0x02
#define Timer_16bit_Clk_64				0x03
#define Timer_16bit_Clk_256				0x04
#define Timer_16bit_Clk_1024				0x05
#define Timer_16bit_Clk_T1_FallingEdge			0x06
#define Timer_16bit_Clk_T1_RisingEdge			0x07

#define Timer_16bit_Mode_Normal				0x01
#define Timer_16bit_Mode_CTC				0x02 // Clear Timer On Compare
#define Timer_16bit_Mode_Fast_PWM			0x03
#define Timer_16bit_Mode_Phase_Correct_PWM		0x04
#define Timer_16bit_Mode_Phase_and_Frecuency_Correct_PWM 0x05


#define Timer_16bit_Interrupt_Input_Capture           0x01
#define Timer_16bit_Interrupt_Output_Compare_Match_A  0x02
#define Timer_16bit_Interrupt_Output_Compare_Match_B  0x03
#define Timer_16bit_Interrupt_Overflow                0x04

#define Timer_16bit_Input_Capture_Flag     0x01
#define Timer_16bit_Output_Compare_A_Flag  0x02
#define Timer_16bit_Output_Compare_B_Flag  0x03
#define Timer_16bit_Overflow_Flag          0x04

#define Timer_16bit_OCR1A  0x01
#define Timer_16bit_OCR1B  0x02

//Define placa externa de 4 botones, 1 potenciometro, 1 led y una ldr

#define Port_KeyBoard DDRC
#define Data_Port_KeyBoard PORTC
#define Pin_Port_Keyboard PINC

#define Led_Tarjeta_Externa 0 //Led de la Tarjeta Externa
#define Potenciometro_Tarjeta_Externa_Canal_ADC 0x02
#define LDR_Tarjeta_Externa_Canal_ADC 0x04
#define Temperatura_LM7805_Motores_LM35DZ_ADC 0x09

//Define las teclas de la tarjeta externa

#define Boton_1  1
#define Boton_2  2
#define Boton_3  3
#define Boton_4  4

#define Tarjeta_Externa_Led_On  Data_Port_KeyBoard |=  _BV( Led_Tarjeta_Externa);
#define Tarjeta_Externa_Led_Off Data_Port_KeyBoard &= ~_BV( Led_Tarjeta_Externa);


//Variables

float cte_LM35DZ= 0.09759;// Para entrada sin amplificar.
char result[20];

uint16_t Tiempo_Bajo= 32768;
uint16_t Tiempo_Alto= 0;


//Variables RTC

int anno=2008;
int mes= 1;
int dia=0;
int segundos=00;
int minutos=00;
int horas=00;
int a=0, b=0, c=0, d=0, e=0;
int meses[12]={6,2,2,5,0,3,5,1,4,6,2,4};
int dias_mes[13]={0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
char dia_semana[][4]={ "Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab"};
char nombre_meses[][13]={"", "Ene","Feb","Mar","Abr","May","Jun","Jul","Ago","Sep","Oct","Nov","Dic"};
int RTC_Paso=1;
int paso;
int Temperatura_Baja=0;
void RTC();

//Variables de control de la tarjeta de control de motores PaP

uint8_t Comando_Placa_74HC595= 0;
float Pot;
int Giro_Motor;
//Define nombres de variables para manejar el motor PaP

int Giro_Izda     = 0;
int Giro_Dcha     = 1;
int Motor_Frenado = 2;

/* Identificación del puerto que da servicio a la interfase ISP */

#define SPI_DDR            DDRB
#define SPI_Select_Slave   3
#define SPI_SS             4
#define SPI_MOSI           5
#define SPI_MISO           6
#define SPI_CLCK           7


/* Establece la línea SPI como esclava */

//SPI_DDR = _BV( SPI_MISO);

