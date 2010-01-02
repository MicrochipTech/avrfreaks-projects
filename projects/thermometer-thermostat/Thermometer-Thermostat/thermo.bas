$regfile = "2313def.dat"
$lib "mcsbyte.lbx"
$crystal = 4000000
Config Timer1 = Timer , Prescale = 1 : Enable Interrupts
Config I2cdelay = 10 : Config Debounce = 30
Config Portd = Output : On Timer1 Multiplex
Dim Tempmsb As Byte , Thset As Byte , Thlow As Byte , Sectic As Byte , Status As Byte
Dim Tempten As Byte , Tempone As Byte , Dispcount As Byte , Seccount As Byte
Dim Z As Bit , Dispchange As Bit , Onoff As Bit , Templsb As Byte
Dim Result As String * 2 , Resultten As String * 1 , Resultone As String * 1
Ddrb = &B00011111 : Portb = &B11100011
Const Ds1621w = &H90                                        ' Addresses of Ds1621 thermometer
Const Ds1621r = &H91
Digit1 Alias Portb.2 : Digit2 Alias Portb.3 : Heater Alias Portb.4
Offt Alias Pinb.5 : Up Alias Pinb.6 : Down Alias Pinb.7
Set Digit1 : Set Digit2 : Sectic = 0 : Dispchange = 0 : Thset = 20 : Thlow = 18
Dispcount = 0 : Seccount = 0 : Z = 0 : Onoff = 0
I2cinit
I2cstart                                                    'Write config (DS1621)
I2cwbyte Ds1621w
I2cwbyte &HAC
I2cwbyte &H08
I2cstart                                                    'Write high temperature
I2cwbyte Ds1621w
I2cwbyte &HA1
I2cwbyte 10
I2cwbyte 0
I2cstart                                                    'Write low temperature
I2cwbyte Ds1621w
I2cwbyte &HA2
I2cwbyte 8
I2cwbyte 0
I2cstart                                                    'Start convert
I2cwbyte Ds1621w
I2cwbyte &HEE
I2cstop
Enable Timer1
Gosub Settemp

'------------------------------------------------------------------------------------------
Do
If Up = 0 Or Down = 0 Then Gosub Settemp
If Offt = 0 Then Gosub Onoffthermo
If Sectic = 0 Then
I2cstart
I2cwbyte Ds1621w
I2cwbyte &HAA
I2cstart
I2cwbyte Ds1621r
I2crbyte Tempmsb , Ack
I2crbyte Templsb , Nack
I2cstop
End If
If Templsb = 128 Then Templsb = 5 Else Templsb = 0
Result = Str(tempmsb)
Resultten = Left(result , 1) : Resultone = Right(result , 1)
Tempten = Val(resultten) : Tempone = Val(resultone)
Loop

'------------------------------------------------------------------------------------------
Onoffthermo:
If Onoff = 0 Then
Dispchange = 1
Do
Tempten = 0 : Tempone = 10
If Dispcount = 254 Then Exit Do
Loop
Dispchange = 1 : Dispcount = 0 : Set Heater
Do
Result = Str(thset)
Resultten = Left(result , 1) : Resultone = Right(result , 1)
Tempten = Val(resultten) : Tempone = Val(resultone)
If Dispcount = 254 Then Exit Do
Loop
Onoff = 1
I2cstart                                                    'Write high temperature
I2cwbyte Ds1621w
I2cwbyte &HA1
I2cwbyte Thset
I2cwbyte 0
I2cstart                                                    'Write low temperature
I2cwbyte Ds1621w
I2cwbyte &HA2
I2cwbyte Thlow
I2cwbyte 0
I2cstop
Else
Onoff = 0 : Reset Heater
I2cstart                                                    'Write high temperature
I2cwbyte Ds1621w
I2cwbyte &HA1
I2cwbyte 10
I2cwbyte 0
I2cstart                                                    'Write low temperature
I2cwbyte Ds1621w
I2cwbyte &HA2
I2cwbyte 8
I2cwbyte 0
I2cstop
Dispchange = 1
Do
Tempten = 0 : Tempone = 11
If Dispcount = 254 Then Exit Do
Loop
End If
Bitwait Offt , Set
Return

'------------------------------------------------------------------------------------------
Settemp:
Stop Timer1 : Portd = 0 : Reset Heater : Waitms 500 : Dispchange = 1
Start Timer1 : Set Heater
Do
If Up = 0 Then
Incr Thset : Dispcount = 0
If Thset > 40 Then Thset = 40
Waitms 200
End If
If Down = 0 Then
Decr Thset : Dispcount = 0
If Thset < 10 Then Thset = 10
Waitms 200
End If
Thlow = Thset - 2
Result = Str(thset)
Resultten = Left(result , 1) : Resultone = Right(result , 1)
Tempten = Val(resultten) : Tempone = Val(resultone)
If Dispcount = 254 Then Exit Do
Loop
If Onoff = 0 Then Goto Exitset
I2cstart                                                    'Write high temperature
I2cwbyte Ds1621w
I2cwbyte &HA1
I2cwbyte Thset
I2cwbyte 0
I2cstart                                                    'Write low temperature
I2cwbyte Ds1621w
I2cwbyte &HA2
I2cwbyte Thlow
I2cwbyte 0
I2cstop
Exitset:
Dispchange = 0 : Dispcount = 0
If Onoff = 0 Then Reset Heater Else Set Heater
Return

'------------------------------------------------------------------------------------------
Multiplex:
Timer1 = &H63C0
Incr Sectic
If Sectic = 200 Then Sectic = 0
If Dispchange = 1 Then
Incr Dispcount
If Dispcount = 255 Then
Dispcount = 0 : Dispchange = 0
End If
Goto Exit3
End If
If Onoff = 1 Then
Incr Seccount
If Z = 1 Then Goto Exit1
If Tempmsb >= Thset Then
Z = 1 : Set Heater
End If
Exit1:
If Z = 0 Then Goto Exit2
If Tempmsb = Thlow And Templsb = 0 Then Z = 0
Exit2:
If Seccount = 50 Then
If Z = 0 Then Toggle Heater
Seccount = 0
End If
End If
Exit3:
If Digit1 = 0 Then
Portd = Lookup(tempten , Digits)
Set Digit1 : Reset Digit2
Else
Portd = Lookup(tempone , Digits)
Reset Digit1 : Set Digit2
End If
Return

'------------------------------------------------------------------------------------------
Digits:
'CC      0,           1,           2,         3,           4,         5,          6,          7,          8,          9          n            F
Data &B1111110 , &B0110000 , &B1101101 , &B1111001 , &B0110011 , &B1011011 , &B0011111 , &B1110000 , &B1111111 , &B1110011 , &B0010101 , &B1000111
Digitca:
'CA      0,           1,           2,         3,           4,         5,          6,          7,          8,          9           n           F
Data &B0000001 , &B1001111 , &B0010010 , &B0000110 , &B1001100 , &B0100100 , &B1100000 , &B0001111 , &B0000000 , &B0001100 , &B1101010 , &B0111000