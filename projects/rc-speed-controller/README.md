# RC SPEED CONTROLLER

Uploaded by atrueland on 2001-10-07 21:22:00 (rating 0 out of 5)

## Summary

RC SPEED CONTROLLER


 BASIC DESCRIPTION  

This application note describes a circuit designed to control the speed of a dc motor used in radio-controlled boats. The electronic speed controller (ESC) is a “no-brake, forward-only” controller. I wanted to keep the part count low and the circuit simple so a couple features were sacrificed. The circuit does not have: over-current protection, over-temp protection, polarity protection, or over/under voltage protection. All the parts can be obtained from digikey distributors. The circuit was designed with a popular 12-cell model boat motor in mind (graupner speed 700). This project was my first attempt at using Bascom-avr and Atmel microcontrollers.


CIRCUIT DESCRIPTION  

The ESC is designed around an Atmel AT90S2313 microcontroller. I chose to build the circuit on two separate boards, a digital control board, and a FET driver board. The digital control section is powered by 4.8-6Vdc from the receiver and the FET driver section is powered by 12 NiCad cells (14.4Vdc).  

The ESC receives the 1-2mS pulses from the receiver and converts it into a 2400Hz PWM signal that drives the FET board controlling the motor. The AT90S2313 converts the receiver signal into the motor drive signal then feeds it into an opto-isolator. The isolator converts the logic level signal into the 14.4Vdc signal needed for FET operation. The signal is then fed into a 9A FET driver chip. The FET driver is responsible for providing the short high current pulses needed to overcome gate capacitance. Three LEDS and a button are used for calibrating/arming the controller.


SOFTWARE DESCRIPTION  

The software begins by calculating the center and full forward values. A button is pushed when the transmitter stick is at center and once again when the stick is full forward. These values are stored as Dbf and C. After storing the center and forward values, the software waits for the stick to return to center before arming the circuit. The armed led will come on after the stick is returned to center. Next a continuous loop of measuring the RX signal and updating the PWM drive signal is performed. IF two consecutive pulses are missed, the “mispulse” routine will turn off the motor drive. 


OPERATION  

1. Connect receiver to the ESC receiver connections.  

2. Connect motor to the ESC motor connections.  

3. Connect battery to the ESC battery connections.  

4. Turn on receiver switch. Two ESC LEDs should be on.  

5. Turn on transmitter.  

6. Press the calibrate button once with the transmitter in a neutral position. Center LED should go out.  

7. Press the calibrate button once again with the transmitter in a full-forward position. The forward LED should go out.  

8. Return transmitter to a neutral position. The Armed LED should go on. The speed controller is ready.


FINAL NOTES  

I chose to build the circuit using point-to-point because of the relatively low parts count and large wires needed for current handling. The thick traces on the schematic represent where 12 gage wire should be used. Circuit layout and packaging is left to the builder. I chose to use IRF540s for convenience and price. The number and quality of FETs can be changed to meet the demands of the builder, but must be N-type and rated for 100Vdc. The use of a heat sink is recommended. In model boats it is common to water-cool the FETs. The number, quality, and cooling efficiency of the FETs will determine current capability. Because this circuit does not include some safety features that you would find on a commercial unit, the builder/operator should exercise caution when experimenting with this device.


DISCLAIMER  

This information is for experimental use only. I do not assume any responsibility for injury or damages resulting from the use of any information found in this application note.


CODE:


$crystal = 10000000  

Dim B As Word  

Dim A As Word  

Dim C As Word  

Dim Dbf As Word  

Dim Pp As Word  

Dim Px As Word  

Dim Ss As Byte  

Dim Xx As Single  

Dim Zz As Single


Config Pind.5 = Input 'set-up button  

Config Pind.6 = Input 'receiver input  

Config Pinb.5 = Output 'center led  

Config Pinb.6 = Output 'forward led  

Config Pinb.7 = Output 'armed led  

Config Pinb.3 = Output 'motor ouput


Set Portb.5 'turn on center led  

Set Portb.6 'turn on forward led  

Reset Portb.7 'turn off armed led  

'***this sets the PWM drive to 2400Hz***  

Config Timer1 = Pwm , Pwm = 8 , Compare A Pwm = Clear Down , Prescale = 8


Config Timer0 = Timer , Prescale = 1024  

On Timer0 Mispulse


Do  

Debounce Pind.5 , 0 , Center , Sub 'set center value  

Loop


Trap:  

Do  

Debounce Pind.5 , 0 , Forward , Sub 'set full forward value  

Loop


Centercheck: 'check for return to center  

Gosub Getpulse  

If B < Dbf Then  

 Goto Centercheck  

End If


Enable Interrupts  

Enable Timer0 'enable missing pulse interrupt  

Start Timer0  

Wait 1  

Set Portb.7 'turn on armed led


Do  

Gosub Getpulse 'measure rx input  

Gosub Setpulse 'set motor speed  

Loop


End


Center: '**calculates center value**  

Gosub Getpulse  

A = B  

Dbf = A - 15  

Reset Portb.5 'turn off center led  

Goto Trap  

Return


Forward: ' **calculates forward value**  

Gosub Getpulse  

C = B  

C = C + 15  

Pp = Dbf - C  

Reset Portb.6  

Goto Centercheck  

Return


Setpulse:  

If B < C Then 'turns motor on 100% close to full forward  

 Pwm1a = 255  

 Goto Done  

End If


If B >= Dbf Then 'turns motor off at center  

 Pwm1a = 0  

 Goto Done  

End If


If B < Dbf Then 'sets motor drive from input signal  

 Px = Dbf - B  

 Xx = Px / Pp  

If Xx < .05 Then 'this sets a 5% minimum on value  

 Xx = .05  

End If  

If Xx > .94 Then 'this sets a max on value at 95%  

 Xx = 1  

End If  

 Zz = Xx * 255  

 Ss = Zz  

 Pwm1a = Ss  

 Goto Done  

End If  

Done:  

Return


Getpulse: 'measures the RX pulse width  

B = 0  

Getpulse\_1:  

sbis pind,6  

rjmp getpulse\_1  

Getpulse\_2:  

Incr B  

sbic pind,6  

rjmp getpulse\_2  

Timer0 = 0 'resets missing pulse timer  

Return


Mispulse: 'if two pulses are missed motor turns off  

Pwm1a = 0  

Return

## Compilers

- BASCOM-AVR

## Tags

- Complete code with HW
- BASCOM-AVR
