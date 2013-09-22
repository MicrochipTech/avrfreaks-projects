#ifndef P_DEFINES_H
#define P_DEFINES_H

#include <util/delay.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef ON
#define ON 1
#endif

#ifndef OFF
#define OFF 0
#endif



// Max amounts of bitAddresses the modbus master can come up with
#define MAX_BitAddresses		32
// Max amounts of bitAddresses the modbus master can come up with
//#define MAX_INPUTS			145

// Max amounts of WordAddresses the modbus master can come up with
#define MAX_WordAddress			50

//#define MAX_INPUT_REGS		96

#define	MaxActionBytes		((uint8_t)(MAX_BitAddresses / 8) + ((MAX_BitAddresses % 8)?1:0))
//#define	MaxProfileWords		30

#define getByteForSysBit(a) ((uint8_t)(a / 8))
#define getBitForSysBit(a) ((uint8_t)(a % 8))


// poichè i registri sono rappresentati in una matrice,
// diamo i nomi alle loro posizioni nell'array
#define IDX_DW_fwd			0
#define IDX_DW_bwd			1
#define IDX_DW_cPCount		2
#define IDX_DW_cStepCount	3
#define IDX_DW_minVel		4
#define IDX_DW_maxVel		5
#define IDX_DW_accel		6
#define IDX_DW_sepMotSpeed	7
#define IDX_DW_runPNo		8
#define IDX_DW_speedPNo		9
#define IDX_DW_SetPage		10
#define IDX_DW_PageCounter	11
#define IDX_DW_VRef_Set		12
#define IDX_DW_VRef			13
#define IDX_DW_AsepMotSpeed	14

#define IDX_DW_ActStep		15
#define IDX_DW_RampState	16
#define IDX_DW_ProfState	17
#define IDX_DW_ActVel		18
#define IDX_DW_EndUp		19
#define IDX_DW_BegDown		20

#define IDX_DW_STGF			21
#define IDX_DW_STGB			22

#define IDX_DW_EncSt_L		23
#define IDX_DW_EncSt_H		24
#define IDX_DW_MantBegin	25
#define IDX_DW_MantFree		26
#define IDX_DW_RPM			27
#define IDX_DW_EncSteps		28
#define IDX_DW_H2O_Turns	29

#define IDX_DW_ActProfNo	30
#define IDX_DW_ActSpeedNo	31
#define IDX_DW_ActDspMode	32
#define IDX_DW_EncHyst		33

#define IDX_DW_Debug01		0 //35 - Original value
#define IDX_DW_Debug02		1 //36 - Original value
#define IDX_DW_Debug03		2 //37 - Original value
#define IDX_DW_Debug04		3 //Added for debug (MiBu)
#define IDX_DW_Debug05		4 //Added for debug (MiBu)
#define IDX_DW_Debug06		5 //Added for debug (MiBu)

// since coils are represented by bit's in an array of Byte,
// we give names to each Byte in array same for Bits

#define	ActProfileByte		0
#define	TestActionByte		1
#define	RunActionByte		2
#define	AddActionByte		3

#define IDX_PB_Dir			0
#define IDX_PB_Mode			1
#define IDX_PB_Relay1		2
#define IDX_PB_Relay2		3
#define IDX_PB_RunProfFPart	4
#define IDX_PB_RunProfBPart	5
#define IDX_PB_SetManBeg	6
#define IDX_PB_SetManFree	7

#define IDX_TAB_DigiP_Up	8
#define IDX_TAB_DigiP_Down	9
#define IDX_TAB_Sleep		10
#define IDX_TAB_PWRDown		11
#define IDX_TAB_RstVRef		12
#define IDX_TAB_SaveVref	13
#define IDX_TAB_TestRunMot	14
#define IDX_TAB_SaveProfile	15

#define IDX_RAB_Enable		16
#define IDX_RAB_IREnc1		17
#define IDX_RAB_IREnc2		18
#define IDX_RAB_FProf		19
#define IDX_RAB_BProf		20
#define IDX_RAB_ResetPage	21
#define IDX_RAB_RunSepMot	22
#define IDX_RAB_RunProd		23

#define IDX_AB_RH2O			24
#define IDX_AB_RTinte		25
#define IDX_AB_RMant		26
#define IDX_AB_RPrint		27
#define IDX_AB_EncSync		28
#define IDX_AB_Free29		29
#define IDX_AB_Free30		30
#define IDX_AB_Free31		31

#define RS485_DirPort	DDRD
#define RS485_Port	PORTD
#define RS485_Pin	PIND
#define RS485_Send	PD2
#define RS485_LRx	PD3
#define RS485_LTx	PD4
#define RS485_LErr	PD5
#define RS485_LSc1	PD6
#define RS485_LSc2	PD7
#define	Led_DirPort	DDRA
#define Led_Port	PORTA
#define Led_Pin		PINA
#define LedOFF		0xFF
//#define Led_DB_CPort DDRB
//#define Led_DB_Port	 PORTB
//#define Led_DB_Pin	 PINB


#ifndef nop
#define nop() asm volatile("nop")
#endif


typedef struct {
//volatile	tRampState	rampState;
volatile	uint16_t	act_Step;
volatile	uint16_t	act_Vel;
volatile	uint16_t	endUp;
volatile	uint16_t	begDown;
volatile	uint8_t		cmdLineMode;
//volatile	tProfState	ProfState;
			uint8_t		act_RProfile;
			uint8_t		act_SProfile;
			uint8_t		act_SProfileIDX;
volatile	uint8_t		TC_running:1;
volatile	uint8_t		act_Mode:1;
volatile	uint8_t		act_Dir:1;
volatile	uint8_t		rs232_cmd:1;
volatile	uint8_t		cmdIsNumber:1;
			uint8_t		dummy:3;
} tGLOBAL_FLAGS ;


static inline void longDelay(uint32_t l){
	while (l--){
		_delay_ms(1);
	}
}

#endif