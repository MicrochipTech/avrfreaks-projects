$regfile = "M32DEF.dat"
$sim
$crystal = 8000000
Config Lcdpin = Pin , Db4 = Portc.1 , Db5 = Portc.2 , Db6 = Portc.3 , Db7 = _
Portc.4 , E = Portd.2 , Rs = Portd.3
Config Lcd = 16 * 2
Config Portd.5 = Output

Config Timer1 = Counter , Edge = Rising
Ddrb.1 = 0 : Portb.1 = 1                                    'PULL UP RESISTOR ACTIVATED
Config Timer2 = Timer , Async = On , Prescale = 128
Enable Counter1
Enable Interrupts
Enable Timer2
On Ovf2 Ovf0occures
Dim A As Long , I As Long , B As Byte
B = 0
Start Timer0

Do

Loop

End                                                         'end program
Ovf0occures:
Toggle Portd.5
Stop Timer0
A = B * 65536
A = A + Counter1
A = A / 0.98304
Cls
Lcd " FREQ IS :" ; A ; "HZ    "
B = 0
I = 0 : Counter1 = 0
Start Timer0
Return