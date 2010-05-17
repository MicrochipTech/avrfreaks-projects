/*****************************************************************************
*   TLib3.1 Demo showing Low Power Timing for AVRTS2080B/ATtiny88 using IAR
*   Atmel Corporation:  http://www.atmel.com
*   Support Forum:  http://forum.atmel.com -> Touch Technology
*   Support Email:  avr@atmel.com  Attn: PaulR
******************************************************************************
* This is example code only:
* - Diagnostic output style isn't intended for production host interface
* - Coding style isn't production quality
* - This file wasn't fully tested, please report any bugs to Forum or Email
******************************************************************************
* Revision History:
* 2080B_iar_v00.c 2009Nov19 PaulR - Created from TLib3.1 Example
*       GCC + IAR in one File
* 2080B_iar_v01.c 2009Nov27 PaulR - First Release
*       Included in Project [TLib31_2080B_LowPowerTiming_iar_gcc_v01]
*       Control Pin: DIAG/LP-: DIAG:LO=LP Enabled, DIAG:HI=[CPU Fast for ICE + Diagnostics]
*       Output Scope Pins: TIME:LO=measure(), STAT:Lo=Detect, STAT:Pulses=During app()
*       LP Modes for IAR & GCC: DIAG, RUN, WDT, CLK, Y0S, T0R, T1S, T1R
*       PRR - Disables unused modules in Active/Sleep:Idle (Sleep:PowerDown no PRR)
*       Timer1 period affected by qt_measure_sensors() as Qmatrix uses Timer1 and ADC-Comparator
*       Simplified GCC-IAR differences using: M_ISR, M_PIN, M_MAIN
******************************************************************************
* License
* Copyright (c) 2008-2009, Atmel Corporation All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1.Redistributions of source code must retain the above copyright notice,
*   this list of conditions and the following disclaimer.
*
* 2.Redistributions in binary form must reproduce the above copyright notice,
*   this list of conditions and the following disclaimer in the documentation
*   and/or other materials provided with the distribution.
*
* 3.The name of ATMEL may not be used to endorse or promote products derived
*   from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
* SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/
// Build Options: (Option: Define in Project settings to avoid editting C file)
#define D_K_NTHR    10u // Touch Threshold for Keys     (Default: 10u)
#define D_SR_NTHR   16u // Touch Threshold Rotor/Slider (Default: 16u)
#define D_BL        64u // QMatrix Burst Length         (Default: 64u, Check Vcs when long BL)

//==================== LP Options (Low Power Mode) ====================
// Low Power timing (Ensure use DIAG Pin when D_LP_CLK):
// D_LP={ D_LP_RUN D_LP_WDT D_LP_CLK D_LP_T0S D_LP_T0R D_LP_T1S D_LP_T1R D_LP_PIN D_LP_RCV }
#define D_LP_RUN    0   // None:       { } Free Run, No Low Power
#define D_LP_WDT    1   // Watchdog:   { __sleep();} Wakeup upon WDT Interrupt
#define D_LP_CLK    2   // SlowClock:  { CLKPR=Slow; __delay_cycles(); CLKPR=Normal }
                        // *Take care to use Diag pin to avoid SlowClk when using ICE
#define D_LP_T0S    5   // Timer0Sleep:{ Do application while waiting for timer }
#define D_LP_T0R    6   // Timer0Run:  { Do application while waiting for timer }
#define D_LP_T1S    3   // Timer1Sleep:{ __sleep();} Wakeup upon Timer Interrupt
                        // *Timer1 is used by QMatrix, so reset during Measure
#define D_LP_T1R    4   // Timer1Run:  { Do application while waiting for timer }
                        // *Timer1 is used by QMatrix, so reset during Measure
//#define D_LP_PIN    x   // Pin:       { __sleep();} Wakeup upon Interrupt Pin
//#define D_LP_RCV    x   // Serial Rx: { __sleep();} Wakeup upon Receive Data
//***************************************************************************
//***** For Low Power measurements disable DebugWire and disconnect ICE *****
//***** Check PRR disables appropriate items for Application            *****
//***************************************************************************
#define D_LP   D_LP_RUN // Low power Mode [LP only when pin DIAG=PC5=LO]

//==================== LP Timing ====================
// Period between calls to qt_measure_sensors(), used for Drift and ReCalibration Timeouts
// - Estimated values shown here.
//   These should be set to approximately match actual timing measurements
// - Exact values aren't required, reasonable variation will simply alter drift/timeouts proportionally
//   Variations may depend on:
//		= Compiler used: IAR or GCC (Timing Values below measured for 2080B with IAR)
//		= Vcc
//		= Timing Mode (D_LP_RUN D_LP_WDT D_LP_CLK D_LP_T0S D_LP_T0R ...)
//		= Touch Technology: QTouch or QMatrix
//		= Sensor Design (Size, Layers, Materials, Thicknesses ...)
//		= Components (Cs, Rsmp)
//		= Settings (BL ...)
//      = Touched or No Touch
//      = Interface Communications
//      = Temperature
// - If Design changes timing "on the fly" then adjust code to appropriately update [current_time_ms_touch].
//   Application may use slower modes to save power, and faster modes when touch or recent touches.
#define MEASURE_PERIOD_ms_DIAG  16u     // Diagnostic Mode with Diagnostic Output
    #define DELAYCYCLES_DIAG    3000ul  // depends on Vcc // DelayCycles: @8MHz=*125usec, @4MHz=*250nsec, @31250Hz=*32usec
#define MEASURE_PERIOD_ms_RUN   8u      // ~Free Run (~Fastest Sampling)
    #define DELAYCYCLES_RUN     300ul   // depends on Vcc // DelayCycles: @8MHz=*125usec, @4MHz=*250nsec, @31250Hz=*32usec
#define MEASURE_PERIOD_ms_WDT   19u     // depends on Vcc (2080B ~3.3V)
    #define WDT_INTERRUPT_16ms  ((1<<WDIE)|(0<<WDE)|(0<<WDP3)|(0<<WDP2)|(0<<WDP1)|(0<<WDP0))//Period0000=~16ms@5V=~19ms@3.3V
    #define WDT_INTERRUPT_32ms  ((1<<WDIE)|(0<<WDE)|(0<<WDP3)|(0<<WDP2)|(0<<WDP1)|(1<<WDP0))//Period0001=~32ms@5V
    #define WDT_INTERRUPT_64ms  ((1<<WDIE)|(0<<WDE)|(0<<WDP3)|(0<<WDP2)|(1<<WDP1)|(0<<WDP0))//Period0010=~64ms@5V=~76ms@3.3V
    #define WDT_INTERRUPT_125ms ((1<<WDIE)|(0<<WDE)|(0<<WDP3)|(0<<WDP2)|(1<<WDP1)|(1<<WDP0))//Period0011=~125ms@5V
    #define WDT_INTERRUPT_250ms ((1<<WDIE)|(0<<WDE)|(0<<WDP3)|(1<<WDP2)|(0<<WDP1)|(0<<WDP0))//Period0100=~250ms@5V
    #define WDT_INTERRUPT_500ms ((1<<WDIE)|(0<<WDE)|(0<<WDP3)|(1<<WDP2)|(0<<WDP1)|(1<<WDP0))//Period0101=~500ms@5V
    #define WDT_INTERRUPT_1sec  ((1<<WDIE)|(0<<WDE)|(0<<WDP3)|(1<<WDP2)|(1<<WDP1)|(0<<WDP0))//Period0110=~1sec@5V
    #define WDT_INTERRUPT_2sec  ((1<<WDIE)|(0<<WDE)|(0<<WDP3)|(1<<WDP2)|(1<<WDP1)|(1<<WDP0))//Period0111=~2sec@5V
    #define WDT_INTERRUPT_4sec  ((1<<WDIE)|(0<<WDE)|(1<<WDP3)|(0<<WDP2)|(0<<WDP1)|(0<<WDP0))//Period1000=~4sec@5V
    #define WDT_INTERRUPT_8sec  ((1<<WDIE)|(0<<WDE)|(1<<WDP3)|(0<<WDP2)|(0<<WDP1)|(1<<WDP0))//Period1001=~8sec@5V
    #define WDT_INTERRUPT       WDT_INTERRUPT_16ms // Selected period (Approximate)
    //#define WDT_DISABLE        ((0<<WDIE)|(0<<WDE)|(1<<WDP3)|(0<<WDP2)|(0<<WDP1)|(1<<WDP0))//Period1001=~8sec@5V
    //#define WDT_INT_RESET_16ms ((1<<WDIE)|(1<<WDE)|(0<<WDP3)|(0<<WDP2)|(0<<WDP1)|(0<<WDP0))//Period0000=~16ms@5V=~19ms@3.3V
    //#define WDT_RESET_1000ms   ((0<<WDIE)|(1<<WDE)|(0<<WDP3)|(1<<WDP2)|(1<<WDP1)|(0<<WDP0))//Period0110=~1sec@5V
#define MEASURE_PERIOD_ms_CLK   10u
    #define DELAYCYCLES_CLK     58ul    // depends on Vcc // DelayCycles: @8MHz=*125usec, @4MHz=*250nsec, @31250Hz=*32usec
#define MEASURE_PERIOD_ms_T0    25ul
    #define TICKS_PER_ms_T0     4ul     // depends on Timer Prescaler, ~4@4MHz/1024 (2080B ~3.3V)
#define MEASURE_PERIOD_ms_T1 (35ul-7ul) // Subtract ~7ms for QMatrix measure() period since it restarts T1
    #define TICKS_PER_ms_T1     500ul   // depends on Timer Prescaler, ~500@4MHz/8 (2080B ~3.3V)
//#define MEASURE_PERIOD_ms_PIN   25u
//#define MEASURE_PERIOD_ms_RCV   25u

//============================================================================
// Compiler Information
#if defined(__GNUC__)
	#define D_GCC   // Using AVR Studio with WINAVR GCC Compiler
#elif defined (__ICCAVR__)
	#define D_IAR   // Using IAR Workbench for AVR
#else
    #error "Compiler Unsupported"
#endif

//============================================================================
// Compiler specific Include Files and Definitions
// Note: Const typically uses initialled RAM. Data in ROM may need special addressing.
#ifdef D_GCC
    #include <avr/io.h>											//GCC
    #include <avr/interrupt.h>									//GCC
    	#define __delay_cycles(n) __builtin_avr_delay_cycles(n)	//GCC
    	#define __enable_interrupt()  sei() 					//GCC
    	#define __disable_interrupt() cli() 					//GCC
	#include <avr/sleep.h>										//GCC
		#define __sleep()             sleep_cpu()				//GCC
	#include <avr/wdt.h>										//GCC
    	#define __watchdog_reset()    wdt_reset()               //GCC
	#define __no_operation() {asm volatile("nop\n\t");}			//GCC

   	#define M_ISR(vec) ISR(vec)		                        	//GCC
    //#define M_MAIN() void main(void) __attribute__((noreturn)); void main(void) //GCC // main() never exit
    #define M_MAIN() int main(void) 							//GCC // main() never exit

	#include <avr/pgmspace.h>	// PROGMEM, see http://www.nongnu.org/avr-libc/user-manual/pgmspace.html
    //#define FLASH_DECLARE(x) x __attribute__((__progmem__))     //GCC // Put Data in ROM
#endif
#ifdef D_IAR
	#include <ioavr.h>											//IAR
	#include <intrinsics.h>										//IAR
    #define PRAGMA_I(x) _Pragma(#x)                             //IAR
    #define M_ISR(vec) PRAGMA_I(vector=vec) __interrupt void isr_##vec(void) //IAR
    #define M_MAIN() __C_task void main(void)                   //IAR // main() never exit

    // Option: use "__monitor" to disable interrupts for selected functions
    //#define FLASH_DECLARE(x) __flash x // Put Data in ROM     //IAR
#endif

//============================================================================
// TLib Include File
// Definitions for "touch_api.h" listed here don't need to be added to compiler options
#define _QMATRIX_
#define QT_NUM_CHANNELS 8
#define DELAY_CYCLES 4
#define PORT_X   B
#define PORT_YA  D
#define PORT_YB  C
#define PORT_SMP D
#define SMP_BIT  7
#define QT_MAX_NUM_ROTORS_SLIDERS 2
#define _ROTOR_SLIDER_
//#define _DEBUG_INTERFACE_  // Define in Tool Preprocessor options (So change doesn't affect file timestamp)

#include "touch_api.h"	// TLib3.1

//============================================================================
// Types/Structures: see [touch_api.h]
#ifdef D_GCC
    //FLASH_DECLARE( qt_touch_lib_measure_data_t* pqt_measure_data = &qt_measure_data; ); // [pqt_measure_data]: Use to "Watch" TLib Data in AVR Studio Debugger
    const qt_touch_lib_measure_data_t* const pqt_measure_data PROGMEM = &qt_measure_data; // [pqt_measure_data]: Use to "Watch" TLib Data in AVR Studio Debugger
#else
    // For IAR add Watch on [qt_measure_data]
#endif

//============================================================================
// Data
static volatile uint8_t  time_to_measure_touch = 0u;// Flag: Sample Time
static volatile uint16_t current_time_ms_touch = 0u;// Counter: Sample period

//============================================================================
// Constants/Defines

//============================================================================
// Macros (Defines similar to prototypes)
// Macros to Get Sensor Status:
//#define GET_SENSOR_STATE(SENSOR_NUMBER) qt_measure_data.qt_touch_status.sensor_states[(SENSOR_NUMBER/8)] & (1 << (SENSOR_NUMBER % 8))
//#define GET_ROTOR_SLIDER_POSITION(ROTOR_SLIDER_NUMBER) qt_measure_data.qt_touch_status.rotor_slider_values[ROTOR_SLIDER_NUMBER]

#define M_PINp1(portnumber,bitnumber) {REG(PORT,portnumber)|= (1u<<bitnumber);} //Set1
#define M_PINp0(portnumber,bitnumber) {REG(PORT,portnumber)&=~(1u<<bitnumber);} //Set0
#define M_PINdo(portnumber,bitnumber) {REG(DDR,portnumber) |= (1u<<bitnumber);} //SetOutput
#define M_PINdi(portnumber,bitnumber) {REG(DDR,portnumber) &=~(1u<<bitnumber);} //SetInput
#define M_PINi(portnumber,bitnumber)  (REG(PIN,portnumber) &  (1u<<bitnumber)?1:0)//GetPin

//==================== Debug Pin Options ====================
// Define DBG Pins for ICE/LP, Timing, Touch Detect
// DD_ Debug Direction: (DDR):  DD0:Input, DD1: Output
// DP_ Debug Port:      (PORT): DD1:PORT=SetsPinState, [DD0+DP1]:PullupEnabled(UnlessPUD=1)
// DI_ Debug Input:     (PIN):  DD-Read:InputStatus, DD-Write1:TogglePORT (affects any pullup)
// ===== 2080B_J1p9 = tiny88_PC5 = "DIAG": DIAG=Hi=SerialDiagnostic+CPUFastForICE, ICE=Lo=LowPower(ICE may not work)
#define Dp_DIAG C   // DIAG Pin:PORT
#define Db_DIAG 5u  // DIAG Pin:BIT
// ===== 2080B_J1p3 = tiny88_PA2 = "TIME": TIME=LO during qt_measure_sensors()
#define Dp_TIME A   // TIME Pin:PORT
#define Db_TIME 2u  // TIME Pin:BIT
// ===== 2080B_J1p4 = tiny88_PD4 = "STAT": STAT=LO during Detect on any Sensor, and Pulses app()
#define Dp_STAT D   // STAT Pin:PORT
#define Db_STAT 4u  // STAT Pin:BIT
//==================== Diagnostics Options [Diag only when pin DIAG=PC5=HI] ====================
// QTouch Studio and Hawkeye - Assign temporary pins if desired
#ifdef _DEBUG_INTERFACE_
	#define EVK_TYPE (1) //QTouch Studio EVK Display: 0:2080A, 1:2080B, 2:Xplain 3~15:TBD
   	uint8_t sensor_config[QT_NUM_CHANNELS];						// Diagnostics packet format
   	#define SENSOR_CONFIG(from,to,type) ((to<<5)|(from<<2)|type)// Diagnostics packet format
	typedef struct tag_board_info_t
	{
	    uint8_t qt_board_id:4;				// Bits3~0: BoardID
		uint8_t qt_max_num_rotors_sliders:4;// Bits7~4: MaxRotorsSliders
	    uint8_t qt_num_channels;			// MaxChannels
	} board_info_t;
   	const board_info_t board_info = {		// Diagnostics packet format
		(EVK_TYPE & 0x0F), (QT_MAX_NUM_ROTORS_SLIDERS & 0x0F), (QT_NUM_CHANNELS)
	};
	//--------------------
   	#define DBG_DATA_PORT	C
   	#define DBG_DATA_BIT	7u
   	#define DBG_CLK_PORT	A
   	#define DBG_CLK_BIT		1u
	//--------------------
   	static void report_debug_data( void );
   	static void output_to_debugger( uint8_t *p, uint8_t count );
   	static void send_debug_byte( uint8_t data );
#endif//_DEBUG_INTERFACE_

//==================== LP Support Functions ====================
#if (D_LP==D_LP_WDT)
    static void init_wdt( uint8_t wdt_mode_period );// Init: WDT
    //----------
#elif ((D_LP==D_LP_T0S)||(D_LP==D_LP_T0R))
    static void init_timer0( void );			// Init: Timer0
#ifdef D_IAR
    #pragma vector=TIMER0_COMPA_vect		   	//IAR
    __interrupt static void timer0_isr( void );	//IAR
#endif //D_IAR
    //----------
#elif ((D_LP==D_LP_T1S)||(D_LP==D_LP_T1R))
    static void init_timer1( void );			// Init: Timer1
#ifdef D_IAR
    #pragma vector=TIMER1_COMPA_vect		    //IAR
    __interrupt static void timer1_isr( void );	//IAR
#endif //D_IAR
    //----------
#endif //D_LP

//============================================================================
// Application Definitions, Prototypes, etc.
void app( void );

//============================================================================
M_MAIN() // main() //(Declare as: [No Return, No Exits], May save stack)
{
	//========== Init: AVR
    //__disable_interrupt();    // Disable Interrupts while setting up Clocks */
	//wdt_disable();

    CLKPR=0x80; CLKPR=0x01;	  	// CPU:4MHz(8MHz/2^1)    - Second write within 4 cycles
    //CLKPR=0x80; CLKPR=0x00;	// CPU:8MHz(8MHz/2^0)    - Second write within 4 cycles
    //CLKPR=0x80; CLKPR=0x08;	// CPU:31250Hz(8MHz/2^8) - Second write within 4 cycles

	MCUCR |= (1u << PUD); //Disable pull-ups for all Pins == Required for TLib in ATtiny and ATmega

    //========== Init: Debug Pins (To Minimize power in any pullups start as Input or OutputHI)
    M_PINp0(Dp_DIAG,Db_DIAG); M_PINdi(Dp_DIAG,Db_DIAG);   // Set:Input,NoPullup (External Pullup in 2080B)
    M_PINp1(Dp_TIME,Db_TIME); M_PINdo(Dp_TIME,Db_TIME);   // Set:Output,Hi
    M_PINp1(Dp_STAT,Db_STAT); M_PINdo(Dp_STAT,Db_STAT);   // Set:Output,Hi

	//========== Init: Sensors (Using NO_AKS_GROUP for Tuning)
	qt_enable_rotor(  CHANNEL_0, CHANNEL_3, NO_AKS_GROUP, D_SR_NTHR, HYST_6_25, RES_8_BIT, 0u  );
	qt_enable_slider( CHANNEL_4, CHANNEL_7, NO_AKS_GROUP, D_SR_NTHR, HYST_6_25, RES_8_BIT, 0u  );
    //qt_enable_key( CHANNEL_0, NO_AKS_GROUP, D_K_NTHR, HYST_6_25 );
    //qt_enable_key( CHANNEL_1, NO_AKS_GROUP, D_K_NTHR, HYST_6_25 );
    //qt_enable_key( CHANNEL_2, NO_AKS_GROUP, D_K_NTHR, HYST_6_25 );
    //qt_enable_key( CHANNEL_3, NO_AKS_GROUP, D_K_NTHR, HYST_6_25 );
    //qt_enable_key( CHANNEL_4, NO_AKS_GROUP, D_K_NTHR, HYST_6_25 );
    //qt_enable_key( CHANNEL_5, NO_AKS_GROUP, D_K_NTHR, HYST_6_25 );
    //qt_enable_key( CHANNEL_6, NO_AKS_GROUP, D_K_NTHR, HYST_6_25 );
    //qt_enable_key( CHANNEL_7, NO_AKS_GROUP, D_K_NTHR, HYST_6_25 );

    qt_init_sensing(); // Initialise Sensing for Sensors defined above

    // QMatrix Specific Settings: BL (Check Vcs when big BL)
    //memset(qt_burst_lengths, D_BL, sizeof(qt_burst_lengths)); // Default=64pulses

    // Set Sensor Global Configuration:
    qt_config_data.qt_di              = DEF_QT_DI;
    qt_config_data.qt_neg_drift_rate  = DEF_QT_NEG_DRIFT_RATE;
    qt_config_data.qt_pos_drift_rate  = DEF_QT_POS_DRIFT_RATE;
    qt_config_data.qt_max_on_duration = DEF_QT_MAX_ON_DURATION;
    qt_config_data.qt_drift_hold_time = DEF_QT_DRIFT_HOLD_TIME;
    qt_config_data.qt_recal_threshold = DEF_QT_RECAL_THRESHOLD;

    qt_filter_callback = 0; // TLib Filter Callback Address: No Filter

#ifdef _DEBUG_INTERFACE_
	//========== Init: Diagnostics to QTouch Studio
   	//REG( DDR, DBG_DATA_PORT ) |= (1u << DBG_DATA_BIT ); // Diagnostics Data
   	//REG( DDR, DBG_CLK_PORT  ) |= (1u << DBG_CLK_BIT  );	// Diagnostics Clock
    M_PINdo(DBG_DATA_PORT,DBG_DATA_BIT);    // Set:Output
    M_PINdo(DBG_CLK_PORT,DBG_CLK_BIT);      // Set:Output
    sensor_config[0] = SENSOR_CONFIG( CHANNEL_0, CHANNEL_3, SENSOR_TYPE_ROTOR );
	sensor_config[1] = SENSOR_CONFIG( CHANNEL_4, CHANNEL_7, SENSOR_TYPE_SLIDER );
   	//sensor_config[0] = SENSOR_CONFIG( CHANNEL_0, CHANNEL_0, SENSOR_TYPE_KEY );
   	//sensor_config[1] = SENSOR_CONFIG( CHANNEL_1, CHANNEL_1, SENSOR_TYPE_KEY );
   	//sensor_config[2] = SENSOR_CONFIG( CHANNEL_2, CHANNEL_2, SENSOR_TYPE_KEY );
   	//sensor_config[3] = SENSOR_CONFIG( CHANNEL_3, CHANNEL_3, SENSOR_TYPE_KEY );
   	//sensor_config[4] = SENSOR_CONFIG( CHANNEL_4, CHANNEL_4, SENSOR_TYPE_KEY );
   	//sensor_config[5] = SENSOR_CONFIG( CHANNEL_5, CHANNEL_5, SENSOR_TYPE_KEY );
   	//sensor_config[6] = SENSOR_CONFIG( CHANNEL_6, CHANNEL_6, SENSOR_TYPE_KEY );
   	//sensor_config[7] = SENSOR_CONFIG( CHANNEL_7, CHANNEL_7, SENSOR_TYPE_KEY );
#endif/* _DEBUG_INTERFACE_ */

	//========== Init: Touch Sampling Timing Control
#if   (D_LP==D_LP_WDT)
    init_wdt( WDT_INTERRUPT );// Init: WDT Interrupts
#elif ((D_LP==D_LP_T0S)||(D_LP==D_LP_T0R))
   	init_timer0();		      // Init: Timer0 Interrupts
#elif ((D_LP==D_LP_T1S)||(D_LP==D_LP_T1R))
   	init_timer1();		      // Init: Timer1 Interrupts
#endif

    //========== Main Loop:
   	for(;;)
   	{
  	   	//========== Measure Period Timing
        if( M_PINi(Dp_DIAG,Db_DIAG) ) // DIAG:Hi=Fast+Diag, DIAG:Lo=LowPower (EVK2080B Jumper on J1:pin9-pin10)

        {   // Diag Mode - CPU Fast so can use ICE, Diagnostic Serial, Not for Power Measurements
            // Diagnostioc Output
            PRR = ((0<<PRTWI)|(0<<PRTIM0)|(0<<PRTIM1)|(0<<PRSPI)|(0<<PRADC));//Diag: Enable All (QMatrix uses ADC,TIM1)
#ifdef _DEBUG_INTERFACE_
			report_debug_data();                // Diagnostic Data to QTouch Studio (or Hawkeye)
            __delay_cycles(DELAYCYCLES_DIAG);   // Data Sync
            current_time_ms_touch += MEASURE_PERIOD_ms_DIAG;
#else // same as D_LP_RUN
            __delay_cycles(DELAYCYCLES_RUN);    // Min Timing Setting (For Oscilloscope and Drift/Timeouts)
            current_time_ms_touch += MEASURE_PERIOD_ms_RUN;
#endif
#if (D_LP==D_LP_WDT)
            __watchdog_reset();                 // restart WDT so no WDT Interrupt or WDT Reset
#endif
        }
        else
        {   // LP Mode (ICE May not work depending on LP Mode and CPU frequency)
#if   (D_LP==D_LP_RUN)
            PRR = ((1<<PRTWI)|(1<<PRTIM0)|(0<<PRTIM1)|(1<<PRSPI)|(0<<PRADC));//Disable Unused Modules (QMatrix uses ADC,TIM1)
            __delay_cycles(DELAYCYCLES_RUN);  // Min Timing Setting (For Oscilloscope and Drift/Timeouts)
            current_time_ms_touch += MEASURE_PERIOD_ms_RUN;
#elif (D_LP==D_LP_WDT)
            __disable_interrupt();  // disable interrupts while setting up sleep
            SMCR = 0x05u;           // set lowest power sleep mode (SM10:PowerDown, WDT Enabled)
            __enable_interrupt();   // re-enable interrupts for wake oncomms, pin change, etc.
    		WDTCSR |= (1<<WDIF);   	// Clear the WDT Interrupt Flag for next event (Write 1 to WDIF)
            __sleep();              // go to sleep (will wake on watchdog)
            SMCR = 0u;              // disable sleep (To prevent any false sleep elsewhere in code)
            __watchdog_reset();     // restart the WDT Timer
            PRR = ((1<<PRTWI)|(1<<PRTIM0)|(0<<PRTIM1)|(1<<PRSPI)|(0<<PRADC));//Disable Unused Modules (QMatrix uses ADC.TIM0)
            current_time_ms_touch += MEASURE_PERIOD_ms_WDT;
#elif (D_LP==D_LP_CLK)
            //**********************************************************************
            //***** Use DIAG Pin to avoid SlowClk when using ICE for Debugging *****
            //**********************************************************************
            CLKPR=0x80; CLKPR=0x08;	// CPU:31250Hz(8MHz/2^8) =  - Second write within 4 cycles
            __delay_cycles(DELAYCYCLES_CLK);  // DelayCycles: @8MHz=*125usec, @4MHz=*250nsec, @31250Hz=*32usec
            CLKPR=0x80; CLKPR=0x01;	// CPU:4MHz(8MHz/2^1) - Second write within 4 cycles
            PRR = ((1<<PRTWI)|(1<<PRTIM0)|(0<<PRTIM1)|(1<<PRSPI)|(0<<PRADC));//Disable Unused Modules (QMatrix uses ADC.TIM0)
            current_time_ms_touch += MEASURE_PERIOD_ms_CLK;
#elif (D_LP==D_LP_T0S)
            // *Timer1 is used by QMatrix, so reset during Measure
            PRR = ((1<<PRTWI)|(0<<PRTIM0)|(1<<PRTIM1)|(1<<PRSPI)|(1<<PRADC));//Disable Unused Modules
            __disable_interrupt();  // disable interrupts while setting up sleep
            SMCR = 0x01u;           // set sleep mode (SM00:IDLE, CLKio:Enabled)
            __enable_interrupt();   // re-enable interrupts for wake oncomms, pin change, etc.
            __sleep();              // go to sleep (will wake on Timer)
            SMCR = 0u;              // disable sleep (To prevent any false sleep elsewhere in code)
            PRR = ((1<<PRTWI)|(0<<PRTIM0)|(0<<PRTIM1)|(1<<PRSPI)|(0<<PRADC));//Disable Unused Modules (QMatrix uses ADC.TIM0)
            current_time_ms_touch += MEASURE_PERIOD_ms_T0;
#elif (D_LP==D_LP_T0R)
            // *Timer1 is used by QMatrix, so reset during Measure
            PRR = ((1<<PRTWI)|(0<<PRTIM0)|(1<<PRTIM1)|(1<<PRSPI)|(1<<PRADC));//Disable Unused Modules
      	    while( !time_to_measure_touch ){app();}// Do Application while waiting for Timer
            time_to_measure_touch = 0u;// Reset Timing Flag
            PRR = ((1<<PRTWI)|(0<<PRTIM0)|(0<<PRTIM1)|(1<<PRSPI)|(0<<PRADC));//Disable Unused Modules (QMatrix uses ADC.TIM0)
            current_time_ms_touch += MEASURE_PERIOD_ms_T0;
#elif (D_LP==D_LP_T1S)
            // *Timer1 is used by QMatrix, so reset during Measure
            PRR = ((1<<PRTWI)|(1<<PRTIM0)|(0<<PRTIM1)|(1<<PRSPI)|(1<<PRADC));//Disable Unused Modules
            __disable_interrupt();  // disable interrupts while setting up sleep
            SMCR = 0x01u;           // set sleep mode (SM00:IDLE, CLKio:Enabled)
            __enable_interrupt();   // re-enable interrupts for wake oncomms, pin change, etc.
            __sleep();              // go to sleep (will wake on Timer)
            SMCR = 0u;              // disable sleep (To prevent any false sleep elsewhere in code)
            PRR = ((1<<PRTWI)|(1<<PRTIM0)|(0<<PRTIM1)|(1<<PRSPI)|(0<<PRADC));//Disable Unused Modules (QMatrix uses ADC.TIM0)
            current_time_ms_touch += MEASURE_PERIOD_ms_T1;
#elif (D_LP==D_LP_T1R)
            // *Timer1 is used by QMatrix, so reset during Measure
            PRR = ((1<<PRTWI)|(1<<PRTIM0)|(0<<PRTIM1)|(1<<PRSPI)|(1<<PRADC));//Disable Unused Modules
      	    while( !time_to_measure_touch ){app();}// Do Application while waiting for Timer
            time_to_measure_touch = 0u;         // Reset Timing Flag
            PRR = ((1<<PRTWI)|(1<<PRTIM0)|(0<<PRTIM1)|(1<<PRSPI)|(0<<PRADC));//Disable Unused Modules (QMatrix uses ADC.TIM0)
            current_time_ms_touch += MEASURE_PERIOD_ms_T1;
//#elif (D_LP==D_LP_PIN)
//            current_time_ms_touch += MEASURE_PERIOD_ms_PIN;
//#elif (D_LP==D_LP_RCV)
//            current_time_ms_touch += MEASURE_PERIOD_ms_RCV;
#else
#error "D_LP: Unsupported Low Power Mode"
#endif
		}

   	   	//========== Measure sensors
        M_PINp0(Dp_TIME,Db_TIME);                   // Measure Touch Sensors - Start
        qt_measure_sensors( current_time_ms_touch );// Sample All Touch Sensors, with Chattering/Debounce
        // QMatrix uses ADC-Comparator and Timer1, so Timer1 Period is affected by qt_measure_sensors()
        M_PINp1(Dp_TIME,Db_TIME);                   // Measure Touch Sensors - Done

		//========== Application Code
        app();// Allow Application Code at least once per Sample Period

  	   	//========== Measure Period Complete
   	}
}
//============================================================================
// Application Code, May be called one or more times per Sample Period depending on D_LP
void app( void )
{
    __no_operation();   // Insert Custom Application Code Here

    M_PINp0(Dp_STAT,Db_STAT);M_PINp1(Dp_STAT,Db_STAT);// Optional Marker at end of Application Block

    if(0==(qt_measure_data.qt_touch_status.sensor_states[0])){ M_PINp1(Dp_STAT,Db_STAT);}//Show NoTouch
    else                                                     { M_PINp0(Dp_STAT,Db_STAT);}//Show Touched
}

//============================================================================
#if (D_LP==D_LP_WDT)
// WDT Interrupt: Init
void init_wdt( uint8_t wdt_mode_period )
{
    __disable_interrupt();  // disable interrupts while setting up WDT
    __watchdog_reset();     // restart the WDT Timer to ensure no false timeout during changes
    WDTCSR |= 0x18U; WDTCSR = wdt_mode_period; // WDCE=WDE=1 then Mode+Prescaler (second write within 4 cycles)
    __watchdog_reset();     // restart the WDT Timer with new Timeout
    WDTCSR |= (1<<WDIF);   	// Clear the WDT Interrupt Flag for next event (Write 1 to WDIF)
    __enable_interrupt();   // re-enable interrupts for wake on WDT, etc.
}
//====================
// WDT Interrupt: Handler
M_ISR(WDT_vect)	// Required by GCC, Optional in IAR
{
   	__no_operation();
}
#endif //D_LP

//============================================================================
#if ((D_LP==D_LP_T0S) || (D_LP==D_LP_T0R))
// Timer0 Interrupt: Init
static void init_timer0( void )
{
    __disable_interrupt();      // disable interrupts while setting up Timer
#if (( TICKS_PER_ms_T0 * MEASURE_PERIOD_ms_T0 )>255)
    #error "Timer0 period overflow, Select Different Timer Prescaler"
#endif
     OCR0A = ( TICKS_PER_ms_T0 * MEASURE_PERIOD_ms_T0 );// Timer0 Period
     TIMSK0 |= (1u << OCIE0A);  // Timer0 Enable Interrupt on OCR0A
     TCCR0A |= (0x5);           // Timer0 PreScale=SystemClk/N, N:1:/1,2:/8,3:/64,4:/256,5:/1024.
     TCCR0A |= (1u << CTC0);    // Timer0 Mode: CountUp, ClearOn OCR0A
     __enable_interrupt();      // re-enable interrupts for wake on Timer, etc.
}
//====================
// Timer0 Interrupt: Handler
M_ISR(TIMER0_COMPA_vect)
{
    time_to_measure_touch = 1u;	// Flag: Touch sample Time
}
#endif //D_LP

//============================================================================
#if ((D_LP==D_LP_T1S) || (D_LP==D_LP_T1R))
// Timer1 Interrupt: Init
// *Timer1 is used by QMatrix, so reset during Measure
static void init_timer1( void )
{
    __disable_interrupt();      // disable interrupts while setting up Timer
#if (( TICKS_PER_ms_T1 * MEASURE_PERIOD_ms_T1 )>65535)
    #error "Timer1 period overflow, Select Different Timer Prescaler"
#endif
    OCR1A = ( TICKS_PER_ms_T1 * MEASURE_PERIOD_ms_T1 );// Timer1 Period
    TIMSK1 |= (1u << OCIE1A);   // Timer1 Enable Interrupt on OCR1A
    TCCR1B |= (1u << CS11);		// Timer1 PreScale=SystemClk/8
    TCCR1B |= (1u << WGM12);	// Timer1 Mode: Count up to OCR1A, then Restart
    __enable_interrupt();       // re-enable interrupts for wake on Timer, etc.
}
//====================
// Timer1 Interrupt: Handler
// *Timer1 is used by QMatrix, so reset during Measure
M_ISR(TIMER1_COMPA_vect)
{
    time_to_measure_touch = 1u;	// Flag: Touch sample Time
}
#endif //D_LP

//============================================================================
#ifdef _DEBUG_INTERFACE_
static void report_debug_data( void )
{
    uint8_t i;
    int16_t sensor_delta;
	static uint8_t u8_cycle;

    output_to_debugger( (uint8_t *) &board_info, (uint8_t) sizeof( board_info ) );
    output_to_debugger( (uint8_t *) &qt_measure_data.channel_signals[0], (uint8_t) sizeof( qt_measure_data.channel_signals ) );
    output_to_debugger( (uint8_t *) &qt_measure_data.channel_references[0], (uint8_t) sizeof( qt_measure_data.channel_references ) );
    for( i = 0u; i < QT_NUM_CHANNELS; i++ )
    {
        sensor_delta = qt_get_sensor_delta( i );
        output_to_debugger( (uint8_t *) &sensor_delta, sizeof( int16_t ) );
    }
    output_to_debugger( (uint8_t *) &qt_measure_data.qt_touch_status, (uint8_t) sizeof( qt_measure_data.qt_touch_status ) );
    output_to_debugger( (uint8_t *) &sensor_config[0], (uint8_t) sizeof( sensor_config ) );
	send_debug_byte(u8_cycle++);// optional Application Data
#ifdef D_IAR
	send_debug_byte(111);		// optional Application Data
#endif
#ifdef D_GCC
	send_debug_byte(121);		// optional Application Data
#endif
	//send_debug_byte(102);		// optional Application Data
	//send_debug_byte(103);		// optional Application Data
	//send_debug_byte(104);		// optional Application Data
	//send_debug_byte(105);		// optional Application Data
	//send_debug_byte(106);		// optional Application Data
	//send_debug_byte(107);		// optional Application Data
}
//====================
// Diagnostic Output Block (Data pointer, Number of Bytes)
static void output_to_debugger( uint8_t *p, uint8_t count )
{
    uint8_t data, i;

    for( i = 0u; i < count; i++ )
    {
        data = *p; /* get next byte to transmit */
        send_debug_byte( data );/* transmit byte over the debug interface  */
        p++;/* point to next byte to transmit */
    }
}
//====================
// Diagnostic Output Bype (Data)
static void send_debug_byte( uint8_t data )
{
    uint8_t i;

    for( i = 0u; i < 8u; i++ )
    {
        /*  set data    */
        //if(data & 0x80u){ REG(PORT,DBG_DATA_PORT) |=  (1u<<DBG_DATA_BIT); }
        //else            { REG(PORT,DBG_DATA_PORT) &= ~(1u<<DBG_DATA_BIT); }
        if(data & 0x80u){  M_PINp1(DBG_DATA_PORT,DBG_DATA_BIT); } //Data=1
        else            {  M_PINp0(DBG_DATA_PORT,DBG_DATA_BIT); } //Data=0
        __delay_cycles( 10UL );/*  data set up time before clock pulse */
        data = (uint8_t)( data << 1u );/* shift next bit up, ready for output */
        //REG( PORT, DBG_CLK_PORT ) |= (1u << DBG_CLK_BIT );/*  clock pulse */
        M_PINp1(DBG_CLK_PORT,DBG_CLK_BIT);  // Clock=1 (Pulse)
        __delay_cycles( 10UL );
        //REG( PORT, DBG_CLK_PORT ) &= ~(1u << DBG_CLK_BIT );
        M_PINp0(DBG_CLK_PORT,DBG_CLK_BIT);  // Clock=0
        __delay_cycles( 10UL );        /* delay before next bit */
    }
    //REG(PORT,DBG_DATA_PORT) &= ~(1u<<DBG_DATA_BIT);/* data low between bytes */
    M_PINp0(DBG_DATA_PORT,DBG_DATA_BIT);    //Data=0 between bytes
    __delay_cycles( 50UL );/* inter-byte delay */
}
#endif /*   _DEBUG_INTERFACE_   */
//============================================================================
// End
//============================================================================
