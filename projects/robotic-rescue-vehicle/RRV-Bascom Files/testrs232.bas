'-------------------------------------------------------

'File Name: tiplayCOMMON.BAS
'Function : <type> triplay cOmMoN
'Version  : 1.11.ODD
'Desc     : A tRiPlAy cOmMoN IC program
'-------------------------------------------------------
'specify from code which chip we use:
$regfile = "m16def.dat"
'Specify the on board crystal
$crystal = 8000000                                          '3579545
'Specify Baud Rate
'$baud = 9600
$baud = 9600
'$large
'$sim
'Config Serialin = Buffered , Size = 20
Dim S As String * 5
Config Lcdbus = 4
Config Lcd = 16 * 2
Config Lcdpin = Pin , Db4 = Portb.5 , Db5 = Portb.4 , Db6 = Portb.3 , Db7 = Portb.2 , E = Portb.6 , Rs = Portb.7

Config Adc = Single , Prescaler = Auto
Start Adc


Dim W As Word
Do

Input S
Cls
Lcd S
Lowerline
W = Getadc(0)
Lcd W
Loop