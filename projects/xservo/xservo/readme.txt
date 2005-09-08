  
                                         - XSERVO -

 * Intro

 The purpose of XServo is to show how to control 8 servos using an Atmel AT90S2313 clocked at 10Mhz.
 
 * Features 
   - Can control 8 servos (and more)
   - interrupt driven, so cpu is free 99% of the time
   - high precission (steps of 1 microsecond) 
   - controlled by a serial port
   - low code/data memory footprint
 
 * How to send commands to the Atmel

 It understands commands in the following syntax

 s<servo_number_from_0_to_7>p<pulse_lenght_in_microseconds>.

 
 Examples:
  1) s0p1500.   will center the servo at pb0
  2) s0p1000.   will move servo all the way to the left
  3) s0p2000.   will move servo all the way to the right
  4) s0p1534s1p2034.  you can add one command after the other
  
 Mind the final dot! This program will buffer all the commands in a 20bytes buffer and will execute the buffer
 after you send the dot character.
  
 After submitting a command the atmel will echo the command and will append and OK/Error message

 * About the pulse width 
 
  Some servos have an operating range bigger than what their specs say, for example I have a HS-55 that according 
  to the specs it's OpRange is 40 degrees but I made it go 90 degrees with commands like s0p600 and s0p2400.
  The maximun pulse length is 6553 microseconds and the minimun is 0. if you try to send longer pulses the behaviour is unexpected.
  
 * Future work
   Protect the reception buffer from buffer overrrun
   Allow support to control 8+5 servos
   Add a pwm control for motors using timer0

 * Modifications
   Please share your source code if you make any improvements 

 * Feedback

 Any feedback is welcome! Also if you plan to use this in your project let me know as that will make me very happy!
 email: raul_aguaviva@yahoo.es
 


 Raul Aguaviva
 raul_aguaviva@yahoo.es
