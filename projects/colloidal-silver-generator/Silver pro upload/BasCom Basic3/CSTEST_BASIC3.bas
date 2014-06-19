'This is one of the best Ionic colloidal silver generators to ever be made,
'it is now being released by me, for your own private use. You are not allowed to build and sell these units,
'you may however help your friends to build theirs and charge a small fee for the labor.
'In other words I don't want you to profit from my hard work.

'If You Use This Code or any part there of,YOU ARE IN AGREEMENT That You Will Not Use This Information
'For Profit Or Use It In A Commercial Way What So Ever.


'Robert



'Colloidal Silver Generator constant current power Supply
'Current will have 3 pre-defined setpoints .002, .004, .006 ma independent of voltage, which is limited to 30.0 Vdc.
'There is to be one custom current setpoint via a potentiometer mounted on the front panel. 0.000 to .200 ma of current
'A Parts per million setting will also be available from the front panel potentiometer
'A progress display is to be made up from the four mode display led's, covering from 1 to 100 percent of the ppm setting
'Added a scrolling mode display upon solution completed. (press mode button to exit it)

'Chip type : ATmega8 -16
'Internal Oscillator set at 8.000 Mhz.
'Pulse width modulation timer1 is able to go to 10 bit 0x03FF.
'10 bit resolution gives us 1023 settings for the pwm at 4.00 Khz.,  instead of 255 at 8 bits of operation.
'Author:  Robert Dunn - 2013

' Port usage is as follows: *(May switch ports around to support SPI programming)*

'PortB PB0 Constant Voltage LED ( Green for constant voltage )
'PortB PB1 Pulse width modulation to the FET control terminal (Gate)
'PortB PB2 Constant Current LED ( Red for constant current )
'PortB PB3 Low level signal (Output for switching the h-bridge)
'PortB PB4 High level signal (Output for switching the h-bridge)
'PortB PB5 Not in use
'PortB PB6 Xtal input (Not in use)
'PortB PB7 Xtal input (Not in use)

'PortC PC0 ADC Current sensor input. Measures the voltage drop across a 1 ohm resistor.
'PortC PC1 Not in use
'PortC PC2 ADC custom current value input. (Potentiometer on the front panel)
'PortC PC3 Not in use
'PortC PC4 Not in use
'PortC PC5 Not in use
'PortC PC6 Not in use
'PortC PC7 Not in use

'PortD PD0 Current Switch #2 (Press and hold to set Current)
'PortD PD1 Start Switch #3 (Press and release to start the process)
'PortD PD2 Mode Switch #4 (Press and release to switch modes)
'PortD PD3 Mode 1 led
'PortD PD4 Pizo Element
'PortD PD5 Mode 2 led
'PortD PD6 Mode 3 led
'PortD PD7 Mode 4 led



'$regfile = "m88pdef.dat"                                   ' For the  Atmega 88pa 1 wire debug
$regfile = "m8def.dat"                                      ' Fot the Atmega 8
$crystal = 8000000
$hwstack = 128                                              '40
$swstack = 128                                              '16
$framesize = 128                                            '32

'define the mcs lib
$lib "mcs.lbx"


 '----------------------------------- TO DO -----------------------------
 ' Complete resistance to parts per million conversion
 ' Complete Progress bar "Completed" (Needs tweeking)



Config Portb = Output                                       '- Set PORTB as Outputs
Portb = &HFF                                                '- Set PORTB outputs to a high state
Config Portd = &B11111000                                   '- Set PORTD bits 0,1 and 2 to inputs
Portd = &B00000111                                          '- Enable internal pullups on bits 0,1 and 2
Portd.4 = 0                                                 '- Turn pizo element off

'- Set the H-bridge to off
Portb.3 = 0                                                 '- PortB PB3 Low level signal (Output for switching the h-bridge)
Portb.4 = 0                                                 '- PortB PB4 Low level signal (Output for switching the h-bridge)

'- Set the Constant current, constant voltage leds to off
Portb.2 = 0                                                 '- PORTB bit 0 is the CV LED  (Turn it off)
Portb.0 = 0                                                 '- PORTB bit 2 is the CC LED  (turn it off)

Config Debounce = 10                                        '- Debounce switch timer, set to 10 ms.


' Declarations section

 Dim Current_val As Word                                    '- Current sensor reading
 Dim Custom_val As Word                                     '- Custom  current setpoint  *** Custom_Val and PPM_Val share the same potentiometer ***
 Dim Ppm_setpoint As Word                                   '- Parts per million set point (from the potentiometer)
 Dim Op_mode As Byte                                        '- Mode setting
 Dim Display_p As Byte                                      '- Bit for turning on the PROGRESS bar
 Dim Tick1 As Word                                          '- 16 bit counter for polairity switching
 Dim Tick2 As Word                                          '- 16 bit counter for the Progress bar display
 Dim Tick3 As Word                                          '- 16 bit counter for completed
 Dim Tick4 As Byte                                          '- 8 bit counter for shorted
 Dim Run As Byte                                            '- Run status, set as 0x01 while running
 Dim Setpoint As Word                                       '- Holds our Current setpoint  (Must support a 16 bit variable for Custom current setting)
 Dim Temp As Word                                           '- Used in Progress bar calculation setpoints
 Dim Temp2 As Word                                          '- Used in Progress bar calculation setpoints
 Dim Complete As Byte                                       '- Completion byte (1= Complete, 0= Not complete)
 Dim Ppm_val As Single                                      '- Calculated Parts per million value (Used in resistance conversions)
 Dim Res_val As Single                                      '- Calculated resistance of the CS solution in ohms
 Dim Span As Single                                         '- span value (used in progress)
 Dim Short As Bit                                           '- Shorted bit (short is equal to 01)
 Dim Flash As Bit                                           '- Bit for flashing led during a paused condition
 Dim Water_test As Bit                                      '- 8 bit value for for the water test
 Dim I As Word                                              '- 10 bit water test counter
 'Dim Eval as  eram word                                     '- Store the Custom current level for next reboot/startup

 On Timer2 T2_isr_a                                         '- Fires on Timer2 Compare match A ISR

'- Set items to only run once at startup
Gosub Flash_set                                             '- Set led flash mode to zero
Gosub Set_mode                                              '- Set mode to default 0f 0x02
Gosub Set_run                                               '- Set Run to default at start up 0x00
Gosub Ticker1_set                                           '- Set counter to 0x00
Gosub Ticker2_set                                           '- Set counter to 0x00
Gosub Ticker3_set                                           '- Set counter to 0x00
Gosub Ticker4_set                                           '- Set counter to 0x00
Gosub Complete_set                                          '- Set bit to 0x00
Gosub Set_p_bit                                             '- Set the Display bit to 0x00
Gosub Ppm_val_set                                           '- Set initial value to 0x00
Gosub Configure_adc                                         '- Setup the ADC registers
Gosub Timer1_setup                                          '- Setup Timer1 for 10bit pwm PortB bit 1 only (PB1)
Gosub Timer2_setup                                          '- Setup Timer2 compare match A
Gosub Short_set                                             '- Set the value to zero at startup
Gosub Water_test_set                                        '- Set water test bit to zero at start up
'Current_val = Eval                                           '- Retrieve the last stored value from ERAM
'- End Set items to only run once at startup

'-------------------------------------------------------------------------------
'Main program loop
'-------------------------------------------------------------------------------
Do
If Run = 0 Then
Gosub Start_pressed                                         '- Check if the start button has been pressed
Gosub Mode_pressed                                          '- Check if the mode button has been pressed
Gosub Update_leds                                           '- Update the Mode status led's
Current_val = 0                                             '- Set the current value to zero, we are not running
Gosub Flash_mode                                            '- Check if we need to flash the mode led
Gosub Scroll_mode                                           '- Scroll the mode led's upon completion of solution
End If

If Run = 1 Then                                             '- If we are in run mode 1 then do these tasks
Gosub Mode_pressed                                          '- Check if the mode button has been pressed
Gosub Update_leds                                           '- Update the Mode status led's
Gosub Read_adc_0                                            '- Read adc channel 0 (Current sensor)
Gosub Read_adc_2                                            '- Read adc channel 2 (Potentiometer)
Gosub Short_circuit                                         '- Check for exessive current flow
Gosub Track                                                 '- Track and make adjustments to the cs solution
Gosub Cc_cv_mode                                            '- Display constant voltage/Curent mode
Gosub Change_polarity                                       '- Check if we need to change the Polarity
Gosub Res_calc                                              '- Calculate the Resistance of the solution
Gosub Progress                                              '- Display the progress to the user
Gosub Completed                                             '- Check to see if the process is complete
Gosub Pause_pressed                                         '- Check if the pause button has been pressed
Gosub Scroll_mode                                           '- Scroll the mode led's upon completion of solution
Else                                                        '- We are not in run mode 1
nop                                                         '- Do not peform the tasks
End If
Loop
'-------------------------------------------------------------------------------
'End main program loop
'-------------------------------------------------------------------------------




'***** Program Sub-routines *****

'-------------------------------------------------------------------------------
'scroll the mode leds after completion of solution
'-------------------------------------------------------------------------------
 Scroll_mode:                                               '- Turn off the mode led's, then scroll them from left to right
 If Complete = 1 Then                                       '- Complete bit will be a one when completed
 Portd.3 = 0                                                '- LED one off (MODE ONE)
 Portd.5 = 0                                                '- LED two off (MODE TWO)
 Portd.6 = 0                                                '- LED three off (MODE THREE)
 Portd.7 = 0                                                '- LED four off (MODE FOUR)

 If Pind.2 = 0 Then                                         '- If the mode button is pressed
 Return                                                     '- Return
 Else
 Portd.3 = 1                                                '- LED one on (MODE ONE)
 Waitms 100
 Portd.3 = 0                                                '- LED one off (MODE ONE)
 Waitms 100                                                 '- Small delay
 End If

 If Pind.2 = 0 Then                                         '- If the mode button is pressed
 Return                                                     '- Return
 Else
 Portd.5 = 1                                                '- LED two on (MODE TWO)
 Waitms 100                                                 '- Small delay
 Portd.5 = 0                                                '- LED two off (MODE TWO)
 Waitms 100                                                 '- Small delay
 End If

 If Pind.2 = 0 Then                                         '- If the mode button is pressed
 Return                                                     '- Return
 Else
 Portd.6 = 1                                                '- LED three on (MODE THREE)
 Waitms 100                                                 '- Small delay
 Portd.6 = 0                                                '- LED three off (MODE THREE)
 Waitms 100                                                 '- Small delay
 End If

 If Pind.2 = 0 Then                                         '- If the mode button is pressed
 Return                                                     '- Return
 Else
 Portd.7 = 1                                                '- LED four on (MODE FOUR)
 Waitms 100                                                 '- Small delay
 Portd.7 = 0                                                '- LED four off (MODE FOUR)
 Waitms 100                                                 '- Small delay
 End If

 End If                                                     '- End if for Complete = 1
 Return
'-------------------------------------------------------------------------------
'Flash the mode led durring a paused condition
'-------------------------------------------------------------------------------
 Flash_mode:
 If Flash = 1 Then                                          ' If we are paused, then flash will be equal to one

 Select Case Op_mode
  Case 1                                                    '- Flash the mode one led

     Portd.3 = 0                                            '- LED one off (MODE ONE)
     Waitms 200                                             '- Small delay
     Portd.3 = 1                                            '- LED one on (MODE ONE)
     Waitms 200                                             '- Small delay
     Return                                                 '- No need to check the rest

   Case 2                                                   '- Flash the mode two led

     Portd.5 = 0                                            '- LED two off (MODE TWO)
     Waitms 200                                             '- Small delay
     Portd.5 = 1                                            '- LED two on (MODE TWO)
     Waitms 200                                             '- Small delay
     Return                                                 '- No need to check the rest

   Case 3                                                   '- Flash the mode three led

     Portd.6 = 0                                            '- LED three off (MODE THREE)
     Waitms 200                                             '- Small delay
     Portd.6 = 1                                            '- LED three on (MODE THREE)
     Waitms 200                                             '- Small delay
     Return                                                 '- No need to check the rest

   Case 4                                                   '- Flash the mode two led

     Portd.7 = 0                                            '- LED four off (MODE FOUR)
     Waitms 200                                             '- Small delay
     Portd.7 = 1                                            '- LED four on (MODE FOUR)
     Waitms 200                                             '- Small delay
     Return                                                 '- No need to check the rest

   End Select

 Else                                                       ' Flash is equal to zero
 nop                                                        '- Do nothing
 End If

 Return

'-------------------------------------------------------------------------------
'Water test routine (Test water quality at start of operation)
'-------------------------------------------------------------------------------
Check_water:
I = 511                                                     '- Set the counter value for each time the water test is started @ 50%
Compare1a = 511                                             '- Set the pwm register each time the water test is started @ 50%
Portb.3 = 0                                                 '- PortB PB3 Low level signal (Output for switching the h-bridge)
Portb.4 = 1                                                 '- PortB PB4 High level signal (Output for switching the h-bridge)

For I = 511 To 1023                                         '- Load the pwm 10 bit value
Compare1a = I                                               '- Load the value into the pwm register
Waitms 50
Gosub Read_adc_0                                            '- Take a cureent reading
If Current_val >= 99 Then                                   '- If current flow is greater than 97 (about 2 ppm)
Goto Failed                                                 '- If the current goes above the set point durring the test, fail it.
End If
Next I                                                      '- Repeat until we reach the full pwm value

Goto Passed                                                 '- If we did not fail durring the water test, then we passed it

Failed:                                                     '- The water test has failed
Compare1a = 0                                               '- Set the PWM_DUTY vairable to 0x00
 Run = 0                                                    '- Pause button released, Set RUN to 0x00
 Tick2 = 0                                                  '- Reset the display counter
 Tick3 = 0                                                  '- Reset the Completion counter
 Tick4 = 0                                                  '- Reset the Short counter
 Display_p = 0                                              '- Turn off the progress display and enable mode display
 Disable Interrupts                                         ' - Disable global interupts
 Disable Timer1                                             '- Stop Timer1
 Disable Timer2                                             '- Stop Timer2
 Portb.3 = 0                                                '- PortB PB3 Low level signal (Output for switching the h-bridge)
 Portb.4 = 0                                                '- PortB PB4 Low level signal (Output for switching the h-bridge)
 Portb.2 = 0                                                '- PORTB bit 0 is the CV LED  (Turn it off)
 Portb.0 = 0
 Water_test = 1                                             '- Force the water test on the next start
 Portd.4 = 1                                                '- Turn on the pizo element
 Portd.7 = 1                                                '- LED four on
 Portd.6 = 1                                                '- LED three on
 Portd.5 = 1                                                '- LED two on
 Portd.3 = 1                                                '- LED one on
Waitms 100                                                  '- Small pause
 Portd.4 = 0                                                '- Turn off the pizo element
 Portd.7 = 0                                                '- LED four off
 Portd.6 = 0                                                '- LED three off
 Portd.5 = 0                                                '- LED two off
 Portd.3 = 0                                                '- LED one off
Waitms 100                                                  '- Small pause
 Portd.4 = 1                                                '- Turn on the pizo element
 Portd.7 = 1                                                '- LED four on
 Portd.6 = 1                                                '- LED three on
 Portd.5 = 1                                                '- LED two on
 Portd.3 = 1                                                '- LED one on
Waitms 100                                                  '- Small pause
 Portd.4 = 0                                                '- Turn off the pizo element
 Portd.7 = 0                                                '- LED four off
 Portd.6 = 0                                                '- LED three off
 Portd.5 = 0                                                '- LED two off
 Portd.3 = 0                                                '- LED one off
Waitms 100                                                  '- Small pause
 Portd.4 = 1                                                '- Turn on the pizo element
 Portd.7 = 1                                                '- LED four on
 Portd.6 = 1                                                '- LED three on
 Portd.5 = 1                                                '- LED two on
 Portd.3 = 1                                                '- LED one on
Waitms 100                                                  '- Small pause
 Portd.4 = 0                                                '- Turn off the pizo element
 Portd.7 = 0                                                '- LED four off
 Portd.6 = 0                                                '- LED three off
 Portd.5 = 0                                                '- LED two off
 Portd.3 = 0                                                '- LED one off
Waitms 100                                                  '- Small pause
 Portd.4 = 1                                                '- Turn on the pizo element
 Portd.7 = 1                                                '- LED four on
 Portd.6 = 1                                                '- LED three on
 Portd.5 = 1                                                '- LED two on
 Portd.3 = 1                                                '- LED one on
Waitms 100                                                  '- Small pause
 Portd.4 = 0                                                '- Turn off the pizo element
 Portd.7 = 0                                                '- LED four off
 Portd.6 = 0                                                '- LED three off
 Portd.5 = 0                                                '- LED two off
 Portd.3 = 0                                                '- LED one off
Waitms 100                                                  '- Small pause
Water_test = 1                                              '- Force a water test on the next start
Return

Passed:
Portb.3 = 0                                                 '- PortB PB3 Low level signal (Output for switching the h-bridge)
Portb.4 = 0                                                 '- PortB PB4 High level signal (Output for switching the h-bridge)
Water_test = 0                                              '- Water test passed, disable it
Compare1a = 1                                               '- Set PWM value for start condition
I = 511                                                     '- Set I to default
 Portd.4 = 1                                                '- Turn on the pizo element
 Waitms 100                                                 '- Small pause
 Portd.4 = 0                                                '- Turn off the pizo element
 Waitms 100                                                 '- Small pause
 Portd.4 = 1                                                '- Turn on the pizo element
 Waitms 100                                                 '- Small pause
 Portd.4 = 0                                                '- Turn off the pizo element
Return                                                      '- Return from the water test
'-------------------------------------------------------------------------------
'Short Detection
'-------------------------------------------------------------------------------
Short_circuit:
Temp = Setpoint                                             '- Save a copy of the set point
Setpoint = Setpoint + 14                                    '- Add 14 to our set point (current trip point)

 If Current_val >= Setpoint Then                            '- If Current exceeds our setpoint then
 Compare1a = 0                                              '- Set the PWM_DUTY vairable to 0x00
 Waitms 20                                                  '- Small delay befor turming off the timer
 Disable Timer1                                             '- Stops the PWM-16bit module on Timer/Counter1  (Turn off the pulse width modulation)
 Portb.3 = 0                                                '- PortB PB3 Low level signal (Output for switching the h-bridge)
 Portb.4 = 0                                                '- PortB PB4 Low level signal (Output for switching the h-bridge)
 Portd.4 = 0                                                '- Keep the pizo elemnt off
 Tick4 = Tick4 + 1                                          ' Increase the shorted counter
 Tick2 = 0                                                  '- Reset the disply counter
 Gosub Shorted_beep:
 Gosub Shorted_beep:
 Gosub Shorted_beep:
 Gosub Shorted_beep:
 Else                                                       '- We are not shorted
  Setpoint = Temp                                           '- Restore the setpoint
  Return
 End If

 '- Allow 3 attempts to clear the short, if not clear, then kick it out of run mode
 If Tick4 >= 3 Then
 Run = 0                                                    '- Take it out of run mode
 Disable Interrupts                                         '- Disable the Global interupt
  Tick4 = 0                                                 '- Reset the short counter
  End If

Setpoint = Temp                                             '- Restore the setpoint
Return
'-------------------------------------------------------------------------------
'key beep tone length
'-------------------------------------------------------------------------------
Key_beep:
Portd.4 = 1                                                 '- Turn on the pizo element
Waitms 20                                                   '- wait for 20 milliseconds
Portd.4 = 0                                                 '- Turn off the pizo element
Return
'-------------------------------------------------------------------------------
'Short Circuit beep tone length
'-------------------------------------------------------------------------------
Shorted_beep:                                               '- 1.0 second tone burst
Portd.4 = 1                                                 '- Turn on the pizo element
Waitms 1000                                                 '- wait for 1000 milliseconds
Portd.4 = 0                                                 '- Turn off the pizo element
Waitms 1000                                                 '- wait for 1000 milliseconds
Return

'-------------------------------------------------------------------------------
'Completion beep tone   "Duh, duh,duh,duh,duh, Duh,Duh
'-------------------------------------------------------------------------------
Completed_beep:                                             '- Small melody

Portd.4 = 1                                                 '- Turn on the pizo element
Waitms 400                                                  '- wait for 500 milliseconds    Duh
Portd.4 = 0                                                 '- Turn off the pizo element
Waitms 200                                                  '- Small pause


Portd.4 = 1                                                 '- Turn on the pizo element      duh
Waitms 100                                                  '- Small pause
Portd.4 = 0                                                 '- Turn off the pizo element
Waitms 100                                                  '- Small pause
Portd.4 = 1                                                 '- Turn on the pizo element      duh
Waitms 100                                                  '- Small pause
Portd.4 = 0                                                 '- Turn off the pizo element
Waitms 100                                                  '- Small pause
Portd.4 = 1                                                 '- Turn on the pizo element      duh
Waitms 100                                                  '- Small pause
Portd.4 = 0                                                 '- Turn off the pizo element
Waitms 200                                                  '- Small pause
Portd.4 = 1                                                 '- Turn on the pizo element      duh
Waitms 100                                                  '- Small pause
Portd.4 = 0                                                 '- Turn off the pizo element
Waitms 400                                                  '- Small pause


Portd.4 = 1                                                 '- Turn off the pizo element
Waitms 150                                                  '- wait for 500 milliseconds    Duh
Portd.4 = 0                                                 '- Turn off the pizo element
Waitms 200                                                  '- Small pause

Portd.4 = 1                                                 '- Turn on the pizo element      Duh
Waitms 150                                                  '- Small pause
Portd.4 = 0                                                 '- Turn off the pizo element

Return



'-------------------------------------------------------------------------------
'Set Water Test bit  at startup
'-------------------------------------------------------------------------------
Water_test_set:
  Water_test = 1                                            '- Set to one at start
 Return
'-------------------------------------------------------------------------------
'Set Flash value
'-------------------------------------------------------------------------------
Flash_set:
   Flash = 0                                                '- Set to zero at start
  Return
'-------------------------------------------------------------------------------
'Set short value
'-------------------------------------------------------------------------------
Short_set:
   Short = 0                                                '- Set to zero at start
  Return
'-------------------------------------------------------------------------------
'Setup the default operating mode
'-------------------------------------------------------------------------------
Set_mode:
   Op_mode = 2                                              '- Default to Normal Mode at  start up  (Mode Two) 0x02
   Setpoint = 110                                           '- Set the dealult setpoint value for .002 ma of current
  Return
 '-------------------------------------------------------------------------------
'Setup the default run mode to 0
'-------------------------------------------------------------------------------
Set_run:
   Run = 0                                                  '- Default to Run Mode of 0x00 at start up
  Return
'-------------------------------------------------------------------------------
'Set the TICK counter to zero at start-up
'-------------------------------------------------------------------------------
Ticker1_set:                                                '- 16 bit Counter, used for polarity change
   Tick1 = 0                                                '- zero out the counter
  Return
'-------------------------------------------------------------------------------
'Set the TICK2 counter to zero at start-up
'-------------------------------------------------------------------------------
Ticker2_set:                                                '- 16 bit Counter, used for Dispaly change
   Tick2 = 0                                                '- zero out the counter
 Return
'-------------------------------------------------------------------------------
'Set the TICK3 counter to zero at start-up
'-------------------------------------------------------------------------------
Ticker3_set:                                                '- 16 bit Counter, used for completion count
   Tick3 = 0                                                '- zero out the counter
 Return
'-------------------------------------------------------------------------------
'Set the TICK4 counter to zero at start-up
'-------------------------------------------------------------------------------
Ticker4_set:                                                '- 16 bit Counter, used for shorted count
   Tick4 = 0                                                '- zero out the counter
 Return
'-------------------------------------------------------------------------------
'Setup the Display bit
'-------------------------------------------------------------------------------
Set_p_bit:                                                  '- Progress bar display bit
  Display_p = 0                                             '- Set for no display at start up
  Return
'-------------------------------------------------------------------------------
'Setup the ppm value at start up
'-------------------------------------------------------------------------------
Ppm_val_set:
Ppm_val = 0                                                 '- Set the value to Zero
Return
'-------------------------------------------------------------------------------
'Setup timer one
'-------------------------------------------------------------------------------
Timer1_setup:                                               '- Setup Timer one for 10 bit PWM, Inverted
Config Timer1 = Pwm , Pwm = 10 , Compare_a_pwm = Clear_down , Compare_b_pwm = Disconnect , Prescale = 1
Return
'-------------------------------------------------------------------------------
'Setup timer two
'-------------------------------------------------------------------------------
Timer2_setup:
 Config Timer2 = Timer , Prescale = 1024 , Compare = Disconnect
 Return
'-------------------------------------------------------------------------------
'Configure the ADC.
'-------------------------------------------------------------------------------
Configure_adc:
  Config Adc = Single , Prescaler = Auto , Reference = Avcc
  Return
'-------------------------------------------------------------------------------
'Default Setting for the complete byte
'-------------------------------------------------------------------------------
Complete_set:
 Complete = 0                                               '- Set to Zero as start up
Return
'-------------------------------------------------------------------------------
'Complete, if this bit is 1 then we are finised with the process
'-------------------------------------------------------------------------------
Completed:
 Select Case Complete
 Case 0                                                     ' We are not complete
 nop                                                        '- Do nothing
 Return

 Case 1                                                     '- We have reached completion, shut it down
 'Complete = 0                                               '- Reset the complete bit, for the next run
  Run = 0
  Compare1a = 0                                             '- Set the PWM_DUTY vairable to 0x00
  Disable Timer1                                            '- Stops the PWM-16bit module on Timer/Counter1  (Turn off the pulse width modulation)
  Portb.3 = 0                                               '- PortB PB3 Low level signal (Output for switching the h-bridge)
  Portb.4 = 0                                               '- PortB PB4 Low level signal (Output for switching the h-bridge)
  Portd.4 = 0                                               '- Keep the pizo elemnt off
  Disable Interrupts                                        '- Disable the Global interupt
  Portb.2 = 0                                               '- PORTB bit 0 is the CV LED  (Turn it off)
  Portb.0 = 0                                               '- PORTB bit 2 is the CC LED  (turn it on)
  Gosub Completed_beep                                      '- Play a melody for completion
  End Select
Return
'-------------------------------------------------------------------------------
'Read ADC channel 0.
'-------------------------------------------------------------------------------
Read_adc_0:
 If Run = 1 Then                                            '- If we are in run mode 1 then
  Current_val = Getadc(0)                                   '- Read channel 0 of the adc
 Else                                                       '- We are not in run mode 1
 nop                                                        '- Do nothing for now
 End If
  Return
'-------------------------------------------------------------------------------
'Read ADC channel 2.
'-------------------------------------------------------------------------------
Read_adc_2:
 If Run = 1 Then                                            '- If we are in run mode 1 then
  Ppm_setpoint = Getadc(2)                                  '- Value for parts per million setpoint
  Ppm_val = Ppm_setpoint                                    '- Convert to a single

'- we only want to update the custom current value if the current button is pressed

  Select Case Pind.0                                        '- Check what state the input is in (Current button)

  Case 0                                                    '- Buttun is pushed
  Custom_val = Ppm_setpoint                                 '- Update the value from the  potentiometer
  Ppm_setpoint = Ppm_setpoint / 10                          ' - Scale it down to meet 0 to 100 ppm
  Return

  Case 1                                                    '- Button is not pushed
  '- Since the Ppm_setpoint can range from 0x000 to 0x03FF (0 to 1023 Decimal) we will scale it down to meet
  '- 0 to 100 parts per million. It will now match the potetiometer front panel scale of 0 to 100:
  Ppm_val = Ppm_val / 10
  Return                                                    '- Exit
  End Select

 Else                                                       '- We are not in run mode 1
 nop                                                        '- Do nothing for now
 End If

  Return
'-------------------------------------------------------------------------------
'Change polarity sub-routine
'-------------------------------------------------------------------------------
Change_polarity:                                            '- Check if we need to change the Polarity
If Run = 1 Then                                             '- If we are in run mode 1 then

 If Tick1 <= 950 Then                                       '- 30 Second marker, if less than, Set the bits
    If Portb.3 = 1 And Portb.4 = 0 Then                     '- If the bits are already set
    Return                                                  '- Then we do not need to set them again
    Else                                                    '- The bits are not set, we need to set them
    Portb.3 = 0                                             '-Set Portb3 and Portb4 bits to low, befor the switch occurs.
    Portb.4 = 0
    Waitms 1                                                '- Small delay to make sure the ports switch to low state
    Portb.3 = 1                                             '- PortB PB3 High level signal (Output for switching the h-bridge)
    Portb.4 = 0                                             '- PortB PB4 Low level signal (Output for switching the h-bridge)
    Return                                                  '- Exit Procedure (under 30 seconds)
    End If
 End If

 If Tick1 > 950 Then                                        '- 30 seconds to 1 minuite (Change the polarity)
    If Portb.3 = 0 And Portb.4 = 1 Then                     '- If the bits are already set
    Return                                                  '- Then we do not need to set them again
    Else                                                    '- The bits are not set, we need to set them
    Portb.3 = 0                                             '-Set Portb3 and Portb4 bits to low, befor the switch occurs.
    Portb.4 = 0
    Waitms 1                                                '- Small delay to make sure the ports switch to low state
    Portb.3 = 0                                             '- PortB PB3 Low level signal (Output for switching the h-bridge)
    Portb.4 = 1                                             '- PortB PB4 High level signal (Output for switching the h-bridge)
    End If
 End If

Else                                                        '- We are not in run mode 1
nop                                                         '- Do nothing for now
End If
  Return
'-------------------------------------------------------------------------------
'Start button sub routine
'-------------------------------------------------------------------------------
Start_pressed:                                              '- Tests the start button and debounces it
If Run = 0 Then                                             '- If we are in run mode zero then
 Debounce Pind.1 , 0 , Beep_s                               '- If in low state for more than 10 ms, then true
 Return                                                     '- Not in low state for 10 ms, exit
 Beep_s:                                                    '- Sound the  pizo element
 Gosub Key_beep                                             '- In low state for 10 ms, Key beep tone
 True_r:
 Debounce Pind.1 , 1 , Set_run_mode                         '- If in high state for more than 10 ms Set to Run
 Goto True_r                                                ' Loop until buton is released
 Set_run_mode:                                              '- Start condition is true
 Complete = 0                                               '- Turn off the completion bit
 Run = 1                                                    '- Start button released, Set RUN to 0x01
 Flash = 0                                                  '- Don't flash a mode led, we are not paused
 Tick2 = 0                                                  '- Reset the display timer
 Tick4 = 0                                                  '- Set the short counter to 0 at start
 Display_p = 0                                              '- Turn off the progress display and enable mode display
 Enable Interrupts                                          ' - Enable global interupts
 Enable Timer1                                              '- Start Timer1 (PWM timer)
 Enable Timer2                                              '- Start Timer2 (Polarity switching)
 Start Adc                                                  '- Start the adc
 Waitms 10                                                  '- small dealy for the adc to become stable
 Compare1a = 1                                              '- Set the PWM value to one at start up

Else                                                        '- We are in run mode 1 already
nop                                                         '- Do nothing for now
End If


If Water_test = 1 Then
Gosub Check_water
 End If

Return
'-------------------------------------------------------------------------------
'Pause button sub routine
'-------------------------------------------------------------------------------
Pause_pressed:                                              '- Tests the pause button and debounces it
If Run = 1 Then                                             '- If we are  in run mode one then                                                 '- Input is low, debounce it
 Debounce Pind.1 , 0 , Beep_p                               '- If in low state for more than 10 ms, then true
 Return                                                     '- Pause button was not pressed
 Beep_p:                                                    '- Sound the  pizo element
 Gosub Key_beep                                             '- In low state for 10 ms, Key beep tone
 True_p:
 Debounce Pind.1 , 1 , Set_pause_mode                       '- If in high state for more than 10 ms Set to Run
 Goto True_p                                                ' Loop until buton is released
 Set_pause_mode:                                            '- Pause condition is true
 Compare1a = 0                                              '- Set the PWM_DUTY vairable to 0x00
 Run = 0                                                    '- Pause button released, Set RUN to 0x00
 Tick2 = 0                                                  '- Reset the display timer
 Display_p = 00                                             '- Turn off the progress display and enable mode display
 Disable Interrupts                                         ' - Disable global interupts
 Disable Timer2                                             '- Stop Timer2
 Portb.3 = 0                                                '- PortB PB3 Low level signal (Output for switching the h-bridge)
 Portb.4 = 0                                                '- PortB PB4 Low level signal (Output for switching the h-bridge)
 Portb.2 = 0                                                '- PORTB bit 0 is the CV LED  (Turn it off)
 Portb.0 = 0                                                '- PORTB bit 2 is the CC LED  (turn it off)
 Flash = 1                                                  '- Set the Led flash bit to 1 (on)

Else                                                        '- We are in run mode 1 already
nop                                                         '- Do nothing for now
End If

 'Skip_pause:                                                '- Button was never pushed
Return
'-------------------------------------------------------------------------------
'Mode button sub routine
'-------------------------------------------------------------------------------
Mode_pressed:                                               '- Tests the mode button and debounces it

If Display_p = 1 Then                                       '- If the progress display is on then
Debounce Pind.2 , 0 , Beep_d                                '- If in low state for more than 10 ms, then true
Return                                                      '- Mode button was not pressed
Beep_d:                                                     '- Sound the  pizo element
Gosub Key_beep                                              '- Key beep tone
 True_prg:                                                  '- Check for button release
 Debounce Pind.2 , 1 , Set_prg                              '- If in high state for more than 10 ms, set progress
 Goto True_prg                                              '- Loop until buton is released
 Set_prg:                                                   '- Mode condition is true
 Tick2 = 0                                                  '- Reset the display timer
 Display_p = 0                                              '- Turn off the progress display and enable mode display
 Complete = 0                                               '- Clear it (reset the scrolling mode diplay)
 Else                                                       '- We are in mode display

Debounce Pind.2 , 0 , Beep_m                                '- If in low state for more than 10 ms, then true
Return                                                      '- Mode button was not pressed
Beep_m:                                                     '- Sound the  pizo element
Gosub Key_beep                                              '- Key beep tone
True_m:
 Debounce Pind.2 , 1 , Set_mode_op                          '- If in high state for more than 10 ms, set mode
 Goto True_m                                                ' Loop until buton is released
 Set_mode_op:                                               '- Mode condition is true
 Complete = 0                                               '- Clear it (reset the scrolling mode diplay)
 Incr Op_mode                                               '- Mode button released, Increase mode by 1
 Tick2 = 0                                                  '- Reset the display timer
  If Op_mode > 4 Then                                       '- There are only four modes (don't let it get above 4)
   Op_mode = 1                                              '- Roll it back to 1
  End If

 End If
Return
'-------------------------------------------------------------------------------
'Operating MODE led's sub-routine
'-------------------------------------------------------------------------------
Update_leds:                                                '- Updates the MODE status led's
  If Display_p = 0 Then                                     ' if the bit is 0x00 then, display the MODE setting

  Select Case Op_mode
  Case 1
     Portd.7 = 0                                            '- LED four off
     Portd.6 = 0                                            '- LED three off
     Portd.5 = 0                                            '- LED two off
     Portd.3 = 1                                            '- LED one on (MODE ONE)
     Return                                                 '- No need to check the rest

   Case 2
     Portd.7 = 0                                            '- LED four off
     Portd.6 = 0                                            '- LED three off
     Portd.3 = 0                                            '- LED one off
     Portd.5 = 1                                            '- LED two on (MODE TWO)
     Return

   Case 3                                                   '- No need to check the rest
     Portd.7 = 0                                            '- LED four off
     Portd.5 = 0                                            '- LED two off
     Portd.3 = 0                                            '- LED one off
     Portd.6 = 1                                            '- LED three on (MODE THREE)
     Return

   Case 4                                                   '- No need to check the rest
     Portd.6 = 0                                            '- LED three off
     Portd.5 = 0                                            '- LED two off
     Portd.3 = 0                                            '- LED one off
     Portd.7 = 1                                            '- LED four on (MODE FOUR)
     Return                                                 '- No need to check the rest

   End Select


  End If                                                    '- End if for: Display_P = 0x00
 Return




'-------------------------------------------------------------------------------
'Calculate the parts per million value
'-------------------------------------------------------------------------------
'   Constant   Current         time         volume
'- =107.87 * (0.0005/1000) * (60*60) * 1000/1192 * 1000 <---------  Scarfed from a spread sheet on the web

'- For this circuit, ppm calulation will be based on the voltage and current flow
'- (Resistance) and not a function of time.
' There will be no need for a time function, as Current is always measured and the voltage is always known.
'- With these two values, we can use ohms law to calculate the solutions resistance, this resistance will be linear
'- to the parts per million value. IE., 0 to 100M ohms would be 100 to 0.1 parts per million
'- So a simple voltage divided by current will provide the resistance.
'- IE., a voltage of 11.5 vdc/0.0035 ma gives us a result of 3285 ohms of resistance

'- The voltage is taken from the pwm value. It ranges from 0x00 to 0x03FF (0.0 to 30 vdc) so we can say that every step
'- of the pwm value is equal to 0.0293 vdc. (30/1023) with this value we can tell what the voltage is for any hex value
'- in the range of 0x0000 to 0x03FF (10 bit)  IE., (0x01FF) = 511(decimal) x 0.0293 = 14.97vdc/0.0035 ma = 4277.8 ohms
'- of resistance

Res_calc:                                                   '- Calculate the resistance value

 Res_val = Compare1a / Current_val                          '- Get the Resistance  (voltage divided by current)

 '- The current value is very small, we will need to scale it up
 '- IE., In mode two .004 ma of current gives us a value of 0x006E. (110 decimal value)
 '- Our PPM setpoint can go to 0x03FF (1023 decimal value) so we need to scale it to that value
 '- Now our 110 value is 110 X 10 = 1100, more in line with our ppm set point.

 Res_val = Res_val * 10                                     '- Scale it up by a factor of 10

Return

'-------------------------------------------------------------------------------
'Progress bar calulation sub-routine
'-------------------------------------------------------------------------------
Progress:                                                   '- Display the PROGRESS to the user

'Progress will be based on the CS solution's resistance as compared to the front panel potentiometer's set point.
'IE., if the potentiometer is set for 100 ppm then we will display the progress bar in 25% increments up to 100%.
'- So if the  potentiometer is set for 100 ppm then it's value will be equal to 1023/10 = 102.3 (scaled to meet 0 to 100 ppm)
'- We need to find the difference between the setpoint and our actual CS solution, and display this to the user as 0 to 100%
'- using the mode of operation leds 1=25%, 2=50%, 3=75%, and 4=100% we can accomplish this.

'
' When all four leds are on we start timing the process (Tick3) if one led should drop out then we clear that timer and
' Wait until all four leds are on again. (all four leds must be on for one minute to say it's complete)

'- Res_val contains our resistance for the colloidal silver solution
'- Ppm_val Contains out Parts Per Million setpoint value

If Run = 1 Then                                             '- If in Run MODE 0x01 then perform the check

    If Tick2 >= 1900 Then                                   '-  has it been one minute yet
     Display_p = 1                                          '- If so enable the progress display
     Else
     Return                                                 '- Less than one minute, Exit
     End If

    If Display_p = 1 Then                                   ' if the bit is 0x01 then, display the PROGRESS bar

'- Get the difference between the resistance and the set PPM point (Span)


     If Ppm_val < Res_val Then                              '- if resistance is greater than our setpoint (at 0%)
      Span = Res_val - Ppm_val                              '- Get the amount of the difference
      End If

      If Span >= 100 Then                                   '- if span is greater than 10K ohms, not at 25%
       Portd.7 = 0                                          '- Turn off(fourth LED) 100%
       Portd.6 = 0                                          '- Turn off (third LED) 75%
       Portd.5 = 0                                          '- Turn off (Second LED) 50%
       Portd.3 = 0                                          '- Turn off (first LED) 25%
       Tick3 = 0                                            '- Reset the counter
       Return                                               '- No need to check the rest
      End If

      If Span > 75 Then                                     '- if span is greater than 7.5K ohms (at 25%)
       Portd.3 = 1                                          '- Turn on (first LED) 25%
       Portd.7 = 0                                          '- Turn off(fourth LED) 100%
       Portd.6 = 0                                          '- Turn off (third LED) 75%
       Portd.5 = 0                                          '- Turn off (Second LED) 50%
       Tick3 = 0                                            '- Reset the counter
       Return                                               '- No need to check the rest
       End If

    If Span >= 50 Then                                      '- if span is greater than 5K ohms (at 50%)
       Portd.3 = 1                                          '- Turn on (first LED) 25%
       Portd.5 = 1                                          '- Turn on (Second LED) 50%
       Portd.7 = 0                                          '- Turn off(fourth LED) 100%
       Portd.6 = 0                                          '- Turn off (third LED) 75%
       Tick3 = 0                                            '- Reset the counter
       Return                                               '- No need to check the rest
       End If

    If Span >= 25 Then                                      '- if span is greater than 2.5K ohms (at 75%)
       Portd.3 = 1                                          '- Turn on (first LED) 25%
       Portd.5 = 1                                          '- Turn on (Second LED) 50%
       Portd.6 = 1                                          '- Turn on (third LED) 75%
       Portd.7 = 0                                          '- Turn off(fourth LED) 100%
       Tick3 = 0                                            '- Reset the counter
       Return                                               '- No need to check the rest
       End If

     If Ppm_val >= Res_val Then                             '- if resistance is lower than our setpoint (at 100%)
       Portd.7 = 1                                          '- Turn off(fourth LED) 100%
       Portd.6 = 1                                          '- Turn off (third LED) 75%
       Portd.5 = 1                                          '- Turn off (Second LED) 50%
       Portd.3 = 1                                          '- Turn off (first LED) 25%
       Return                                               '- No need to check the rest
       End If


    'If Res_val >= 100 Then                                    '- if resistance is greater than 10K ohms (at 0%)
       'Portd.7 = 0                                          '- Turn off(fourth LED) 100%
       'Portd.6 = 0                                          '- Turn off (third LED) 75%
       'Portd.5 = 0                                          '- Turn off (Second LED) 50%
       'Portd.3 = 0                                          '- Turn off (first LED) 25%
       'Return                                               '- No need to check the rest
       'End If

    'If Res_val >= 80 Then                                   '- if resistance is greater than 8K ohms (at 25%)
       'Portd.3 = 1                                          '- Turn off (first LED) 25%
       'Portd.7 = 0                                          '- Turn off(fourth LED) 100%
       'Portd.6 = 0                                          '- Turn off (third LED) 75%
       'Portd.5 = 0                                          '- Turn off (Second LED) 50%
       'Return                                               '- No need to check the rest
       'End If

    'If Res_val >= 60 Then                                   '- if resistance is greater than 6K ohms (at 50%)
       'Portd.3 = 1                                          '- Turn on (first LED) 25%
       'Portd.5 = 1                                          '- Turn on (Second LED) 50%
       'Portd.7 = 0                                          '- Turn off(fourth LED) 100%
       'Portd.6 = 0                                          '- Turn off (third LED) 75%
       'Return                                               '- No need to check the rest
       'End If

    'If Res_val >= 40 Then                                   '- if resistance is greater than 4K ohms (at 75%)
       'Portd.3 = 1                                          '- Turn on (first LED) 25%
       'Portd.5 = 1                                          '- Turn on (Second LED) 50%
       'Portd.6 = 1                                          '- Turn on (third LED) 75%
       'Portd.7 = 0                                          '- Turn off(fourth LED) 100%
       'Return                                               '- No need to check the rest
       'End If

    'If Res_val >= 20 Then                                   '- if resistance is greater then 2K ohms
       'Portd.3 = 1                                          '- 25% turn on point (first LED)
       'Portd.5 = 1                                          '- 50% turn on point (second LED)
       'Portd.6 = 1                                          '- 75% turn on point (third LED)
       'Portd.7 = 1                                          '- 100% turn on point (fourth LED)  if here, we are complete
       'End If


     End If                                                 '- End if for: Progress bar bit 0x01
    Else                                                    '- We are not in Run Mode 0x01
    nop                                                     '- Do nothing
    'Portb.3 = 0                                             '- PortB PB3 Low level signal (Output for switching the h-bridge)
    'Portb.4 = 0                                             '- PortB PB4 Low level signal (Output for switching the h-bridge)
End If                                                      '- Set the completion bit to 1
  Return
'-------------------------------------------------------------------------------
'TRACK sub-routine (Track the current and adjust) All four modes...
'-------------------------------------------------------------------------------
Track:                                                      '- Track all modes and make adjustments to the CS Solution
'If Short = 1 Then                                           '- A short was detected
'Compare1a = 0                                               '- Turn off pulse width modulation
'Goto Short_circuit                                          '- go to the Short_circuit sub routine
'End If

 If Run = 1 Then                                            '- If start button was pressed  then RUN will be equal to 0x01

   '- Here we will check what mode we are in, and set the set point value for that mode

     Select Case Op_mode

     Case 1
      Setpoint = 96                                         '- Load value for .002 milliamps of current
      Goto Sweep

     Case 2
      Setpoint = 110                                        '- Load value for .004 milliamps of current
      Goto Sweep

     Case 3
      Setpoint = 124                                        '- Load value for .006 milliamps of current
      Goto Sweep

     Case 4
      Setpoint = Custom_val                                 '- Load Reading from the potentiometer
      'Eval = Current_val                                    '- Store a copy in ram for next time it's used
      Goto Sweep

  End Select

       '- Here we will take our SETPOINT and compare it to the Current reading and make our adjustments
       '- This routine will cover all four modes of operation.
       '- Pulse witdth modulation will be locked at 1023 (0x03FF) 10 bit.

Sweep:
                                                            '- Perform auto-sweeping (ramp up/down) try to find a match
     'Waitus 900                                             '- Settle time
  If Setpoint = Current_val Then                            '- if equal, return
     Return                                                 '- Values match, no adjustment needed.
  Else                                                      '- No match yet, sweep it

   If Setpoint > Current_val Then                           '- If our setpoint is greater than our actual current flow.
    If Compare1a = 1023 Then                                '- Do not let the value go over. (At max do not increase it)
    Goto Bail_out_h                                         '- PWM_DUTY is at max bail out.
   Else                                                     '- If not, then go for increase
    Compare1a = Compare1a + 1                               '- Increment duty ratio (raise the current)
    Gosub Read_adc_0                                        '- If not equal grab another current reading and check again
    Goto Sweep                                              '- Perform the Test again
   End If
  End If                                                    '- End if for: Setpoint > Current_val

  If Setpoint < Current_val Then                            '- If our setpoint is less than our actual current flow
   If Compare1a = 0 Then                                    '- Do not let the value go lower. (At min do not decrease it)
    Goto Bail_out_l                                         '- PWM_DUTY is at min bail out
    Else                                                    '- If not, then go for decrease
    Compare1a = Compare1a - 1                               '- Decrease duty ratio (lower the current)
    Gosub Read_adc_0                                        '- If not equal grab another current reading and check again
    Goto Sweep                                              '- Perform the Test again
    End If
  End If                                                    '- End if for: Setpoint < Current_val
 End If                                                     '- End if Setpoint = current value

Bail_out_h:                                                 '- At max pwm value
Compare1a = 1023                                            '- Highest Value
Return

Bail_out_l:                                                 '- At min  pwm value
Compare1a = 0                                               '- Lowest Value
Return


Else                                                        '- If not, then RUN will be equal to 0x00  (Start button was not pressed or we are paused)
Run = 0                                                     '- Keep Run Mode at 0x00
Compare1a = 0                                               '- Set the PWM_DUTY vairable to 0x00
Disable Timer1                                              '- Stops the PWM-16bit module on Timer/Counter1  (Turn off the pulse width modulation)
Portb.3 = 0                                                 '- PortB PB3 Low level signal (Output for switching the h-bridge)
Portb.4 = 0                                                 '- PortB PB4 Low level signal (Output for switching the h-bridge)
Portd.4 = 0                                                 '- Keep the pizo elemnt off
'- Set the Constant current, constant voltage leds to off
Portb.2 = 0                                                 '- PORTB bit 0 is the CV LED  (Turn it off)
Portb.0 = 0                                                 '- PORTB bit 2 is the CC LED  (turn it off)
Disable Interrupts                                          '- Disable the Global interupt
 End If                                                     '- End if for: RUN = 0x01

Return
'-------------------------------------------------------------------------------
'Constant current/Constant voltage mode sub-routine
'-------------------------------------------------------------------------------
Cc_cv_mode:                                                 '- Check if we are in Constant current or Constant voltage mode
  If Run = 1 Then                                           '- If in Run MODE 0x01 then perform the check
    Select Case Op_mode                                     '- Check to see what MODE we are in
     Case 1                                                 '- ULTRA FINE MODE .002 miliamps  (Mode One)
     Setpoint = 96                                          '- value for .002 milliamps of current
     Case 2                                                 '- NORMAL MODE  .004 milliamps   (Mode Two)
     Setpoint = 110                                         '- value for .004 milliamps of current
     Case 3                                                 '- BOLDER MODE  .006  milliamps  (Mode Three)
     Setpoint = 124                                         '- value for .006 milliamps of current
     Case 4                                                 '- CUSTOM CURRENT MODE (Potentiometer Setpoint)  (Mode Four)
     Setpoint = Custom_val                                  '- value of Reading from the potentiometer
     End Select

    Setpoint = Setpoint - 1                                 '- Minus  one from our setpoint
    If Current_val >= Setpoint Then                         '- If our current is equal to our setpoint minus one, then we are in Constant Current Mode.
     Portb.2 = 0                                            '- PORTB bit 0 is the CV LED  (Turn it off)
     Portb.0 = 1                                            '- PORTB bit 2 is the CC LED  (turn it on)
     Setpoint = Setpoint + 1                                '- Add  one to our setpoint
     Else                                                   '- If our current is lower than our SETPOINT, then we are in Constant Voltage Mode.
     Portb.0 = 0                                            '- PORTB bit 2 is the CC LED  (turn it off)
     Portb.2 = 1                                            '- PORTB bit 0 is the CV LED  (Turn it on)
     Setpoint = Setpoint + 1                                '- Add  one to our setpoint
    End If
     Else                                                   '- We are not in Run Mode 0x01
     'Portb.3 = 0                                           '- PortB PB3 Low level signal (Output for switching the h-bridge)
     'Portb.4 = 0                                           '- PortB PB4 Low level signal (Output for switching the h-bridge)
     'SREG.SREG_I = 0                                       '- Disable the Global interupt
   End If
Return


'-------------------------------------------------------------------------------
'Timer2 Compare Match A ISR.
'-------------------------------------------------------------------------------
T2_isr_a:
 If Run = 1 Then                                            '- If in run mode 1 then
  Incr Tick1                                                '- Increment the counter by one (Polarity)
  If Tick1 > 1900 Then                                      '- 1 minute mark.
  Tick1 = 0                                                 '- reset it, force a polarity change
  Tick4 = 0                                                 '- If not shorted for one minutes time, clear the short counter
  End If

 Incr Tick2                                                 '- Increment the counter by one (Progress bar)
   If Tick2 >= 1901 Then                                    '- Check to see if counter is going over the one minute mark
    Tick2 = Tick2 - 1                                       '- Hold the counter at the one minute mark (Progress bar)
   End If



 '- If all four progress led's are on then start counting
 '- If we reach one minute and all the leds are on, set the completion bit

  If Portd.3 = 1 And Portd.5 = 1 And Portd.6 = 1 And Portd.7 = 1 Then
   Incr Tick3                                               '- Increment the completion counter
   End If

   If Tick3 >= 1900 Then                                    '- All fours leds have been on for one minute
   Complete = 1                                             '- Set the completion bit
   Tick3 = Tick3 - 1                                        '- Hold it at the one minute mark
   Else
   Complete = 0                                             '- Less than one minute, do not set it
   End If


  Enable Interrupts                                         '- Re-enable the global interupts befor returning
 Else
 Tick1 = 0                                                  '- We are not in run mode 1 Reset the counter
 Tick2 = 0                                                  '- We are not in run mode 1 Reset the counter
 Tick3 = 0                                                  '- We are not in run mode 1 Reset the counter
 Tick4 = 0                                                  '- We are not in run mode 1 Reset the counter
End If                                                      '- End if for run mode = 1
Return

End                                                         '- End for main program loop