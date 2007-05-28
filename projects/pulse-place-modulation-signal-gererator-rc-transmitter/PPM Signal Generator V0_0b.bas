'******************************************************************************
'PPM Signal Generator   Version 0.0b
'******************************************************************************
'
'Written for  : Atmel "BASCOM-AVR" Version 1.11.7.9
'        when : 2005 December 5
'        where: BRUSSELS - BELGIUM - EUROPE
'        who  : Jan Huygh
'       E-mail: jan.huygh@skynet.be

'----[ A WORD FROM THE AUTHOR ]-------------------------------------------------
'I would find it motivating to find out that people use this code.
'
'On regular base (every 10 to 12 weeks) I gather the comments on the code I
'posted and use the comments to improve my code. If you have constructive
'comment then let me know :
'     * Give me an evaluation.
'     * Send me a personal message
'     * Send me a mail (jan.huygh@skynet.be)

'----[ ABOUT VERSION 0.0a ]-----------------------------------------------------
'This is the 2nd posted version.
'* Previous version 0.0a had a bug that made the last channel trasmitted way to
'  long. As far as my scope enables me to check I believe it's now OK. (And
'  I need a beter scope)
'
'  KNOWN BUGS :
'    * None so far
'  OPPORTUNITIES FOR IMPROVEMENT :
'    * The code is now using an array Pulselen(). It has tice the number of
'      elements then the number of channels you want to transmit. It would be
'      wise to reduce the number of number of elements in the array to the
'      number of channels you want to transmit.

'----[ INTRODUCTION ]-----------------------------------------------------------
'If you want to build your own RC transmitter then using a commercial HF module
'such as the Graupner/JR Sender-HF-Modul Part Nr 4057 or the Graupner/JR
'PLL-SYNTHESIZER-Sender HF-Modul Part Nr 4059 is a good idea.
'Using such a module you will not have to worry about a transmitter license
'nor will you have to worry about building the HF-portion of the transmitter.
'I can buy such a module for about 75 Euro. If somebody can tell me where I can
'buy a comparable module for less please send me a mail jan.huygh@skynet.be

'This program does generate the postitive Pulse Position Modulation signal that
'you will need if you want to build your own RC transmitter using such a
'commercial High Frequency module. These Modules take the PPM signal as input.

'If you have a Graupner X-756 or comparable transmitter then the trainer signal
'is this PPM-signal. So you could use the trainer connection on that transmitter
'and avoid buying the above mentioned modules.

'--------[ CONNECTING TO A HF MODULE ]------------------------------------------

'  |                    If you lay the HF module in front of you with the
'  |                    connector (5 small holes) in the lower left corner
'  |  ° PPM             then the connections are as indicated here on the left.
'  |  ° +5V
'  |  ° +8V                PPM is the Positive Pulse Position Signal
'  |  ° GND                + 5V is a stabelized 5V tention
'  |  ° To Antenna         + 8V to this you connect your main battery
'  |______________            8 * 1.5 = 12V is OK too.
'                          GND is the ground connection (the - from the battery)
'
'--------[ DESCRIPTION OF THE POSITIVE PPM SIGNAL ]-----------------------------

'The line goes low for 0,4ms then high for 1 to 2 ms then low again for 0,4 ms.
'The 1 to 2 ms pulse represents channel 1.
'You can now add channel2 to channel8 by bringing the line high for 1 to 2 ms
'and then low again for 0,4 ms.
'After channel 8 you have to end with a 0,4 ms low.
'This pulsetrain is repeated every 20 ms.
'Since all the channel might want to transmit a 2 ms pulse and we need to split
'the channels with a 0,4 ms pulse we can transmit 8 channels within 20 ms since
'0,4 + 8 * (0,4 + 2) = 19.6 ms.
'You do not need to transmit 8 channels. You can stop after any channel just
'don't forget to end with a 0,4ms low.
'The number of channels you will trasmit is set with the constant channels here
'below.

'--------[ How this program works ]---------------------------------------------
'We use 2 timers: Timer0 will generate an interrupt every 20ms. Timer1 will
'be used to actually generate the PPM signal.
'When timer0 generates an interrupt we will firts reload timer0 so that it
'will generate a new interrupt after 20ms.
'When Timer0 generates an interrupt the line will be high (since this is the
'end state of the previous pulse train. In the Timer0 interrupt service routine
'we will bring the line low, load timer1 so that it will generate an interrupt
'after 0,4 ms. Then we return to the main program and wait for Timer1 to
'generate an interrupt.
'When timer1 generates an intterupt we will toggle the output, reload the timer
'with a value coming from a Pulselen()-array, increment the index value for
'pulselen() and return to the main program waiting for the next Timer1 int.
'We will continue to do that untill all the channels have been trasmitted.
'When all the channels have been trasmitted we will stop Timer1 so that he will
'not generate an additional interrupt. (Timer1 will be restarted in the timer0
'interupt service routine.
'The interupt service routines for timer0 and timer1 only take 10 to 20
'instructioncycles so both take certainly less then 2µs to execute. The result
'is that even if you transmit 8 channels (=the maximum) then still the
'controller can spend over 99% of his time on executing the main program.

'*******************************************************************************
'I'm using an AT Mega 16 running at 10 MHz
$regfile = "m16def.dat" : $crystal = 10000000

'------[ Configure Timer 0 ]----------------------------------------------------
'In my BASCOM compiler version 1.11.7.9 I could not get "Config Timer0" to work
'the way I want so that is why I'm writing myself to the Timer Counter Control
'Register 0. (Tccr0)
'The settings I put in Tccr0 using "Tccr0=&B00000101" mean:
'  Do not force output compare
'  Waveform generation mode  = Normal
'  Compare Match output mode = Normal port operation, OC0 disconnected
'  Clock Select = Clk/1024
'I believe that this is exactly what BASCOM would write to Tccr0 when I would
'use the command "Config Timer0 = Timer , Presacle = 1024"
Tccr0 = &B00000101 : On Timer0 Timer0_isr

'------[ Configure Timer 1 ]----------------------------------------------------
Config Timer1 = Timer , Prescale = 1 : On Timer1 Timer1_isr : Stop Timer1

'------[ Enable timers and global interrupt ]-----------------------------------
Enable Timer0
Enable Timer1
Enable Interrupts


'------[ Variables & Constants ]------------------------------------------------
'Timer1 has no presale and counts 1 up every clock cycle.
'Timer1 generates an overflow interrupt when it reaches 65.535 (=2^16).
'To obtain a 0,4 ms delay you need to initially load Timer1 with :
'  65.535 - (400 * CPU-frequency /1.000.000)
'in my situation with a 10MHz crystal that makes 61.535.
Const Us400 = 61535
'To obtain a 1,5 ms delay you need to initially load Timer1 with :
'  65.535 - (1.500 * CPU-frequency / 1.000.000)
'in my situation with a 10MHz crystal that makes 50.535
Const Us1500 = 50535
'Here we set the number of channels that we intend to transmit.
'The minimum value is 1 and the maximum value is 8
Const Channels = 8
'The delays we will need are stored in the Pulselen() array.
'The number of elements we need in that array is twice the number of channels we
'want to transmit.
Const Arrayvalue =(channels * 2)
Dim Pulselen(arrayvalue) As Word
'To enbale you to use any I/O-pin as the output pin for the PPM signal we set
'here an Alias for the pin you want to use and that alias will be used in the
'rest of the program to identify the pin you indicate here below.
'Just replace "Portd.7" with the pin you want to use.
Ppm_output_pin Alias Portd.5
'We need a small counter
Dim Abyte As Byte

'------[ Configure I/O Pins ]---------------------------------------------------
Config Ppm_output_pin = Output

'------[ Initialization ]-------------------------------------------------------
'We initialize the Pulselen() array.
'The value transmitted for channel n will be loaded in Pulselen(2n-1)
'The value for the 0,4 ms separation pulses will be loaded in Pulselen(2n)
'The way I play arround with "Abyte" is strage I admit. It makes me save a
'variable and since this is a really small routine that is only used once during
'the initialization just after the initial boot, I decided speed was not
'important.
For Abyte = 1 To Channels
   Abyte = Abyte * 2
   Pulselen(abyte) = Us400
   Abyte = Abyte - 1
   Pulselen(abyte) = Us1500
   Abyte = Abyte + 1
   Abyte = Abyte / 2
Next I

'------[ Main Program ]---------------------------------------------------------

Do
'Here comes your main program.
'If you are indeed building an RC transmitter then the code will mean measuring
'positions of stearing sticks and translate those value to values for
'Pulselen(n).
'Your value for channel n need to be loaded in Pulselen(2n-1). So for channel 1
'you would write "Pulselen(1) = XXX" and for channel 2 "Pulselen(3)= XXX"

   'Readsticks
   'Calculatechannels
   'Printonlcd

Loop

'------[ Interrupt services routines ]------------------------------------------
Timer0_isr:
'Timer 0 generates an interrupt every 20 ms.
'When we get that interrupt we first make sure we get our next interrupt on time
'that is why we reload timer0 with 62 using the statement "Timer0 = 62".
'We will use Abyte to indicate where we are in our PPM signal and at the start
'we are at the start so Abyte has to be 0
'Then we start the PPM-signal output with the first 0,4 ms pulse by bringing the
'line low with the statement "Reset Ppm_output_pin" followed by the
'initialization of timer1 to the value that will generate an interrupt after
'0,4ms. Finally we start Timer1 and return to the main program waiting for the
'next intterrupt.
'The next interrupt will be Timer1 since that delay is much smaller then the
'timer0 delay. Actually all channels will be trasmitted before Timer0 will
'interrupt again (even if we go for the maximum of 8 channels)
   Timer0 = 62
   'just to have a decent trigger
      Config Portd.0 = Output
      Set Portd.0
      For Abyte = 1 To 10
      Next Abyte
      Reset Portd.0
   Abyte = 1
   Reset Ppm_output_pin
   Timer1 = Us400
   Start Timer1
   Return

Timer1_isr:
'After the initial 0,4ms pulse the line needs to go up again so we toggle
'the PPM_Output_pin.
'As long as we have not reached the last value in the array Pulselen() we need
'to continue reloading Timer1 with the correct Pulselen() value.
'When we reach the last value in the array Pulselen() we need to make sure we
'get not additional Timer1 interrupt. That's why we stop the Timer1 when we
'reach Arrayvalue.
   Toggle Ppm_output_pin
   If Abyte <= Arrayvalue Then
      Timer1 = Pulselen(abyte)
      Abyte = Abyte + 1
   Else
      Stop Timer1
   End If

   Return

End