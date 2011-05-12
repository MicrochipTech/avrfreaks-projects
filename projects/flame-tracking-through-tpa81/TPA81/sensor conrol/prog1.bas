$regfile "m8535.dat"
$crystal = 6000000


Config Twi = 100000

Config Scl = Portc.0
Config Sda = Portc.1

Config Portb = Output

Config Portd.7 = Output

Config Portc.4 = Output
Config Portc.5 = Output



Config Serialin = Buffered , Size = 20
Config Serialout = Buffered , Size = 20
Config Timer2 = Pwm , Pwm = On , Async = Off , Compare Pwm = Clear Up , Prescale = 128

Enable Interrupts

Enable Timer2


Declare Sub I2c_transmit(byval Address As Byte , Byval Reg As Byte )
Declare Sub I2cread(byval Address As Byte , Byval Reg As Byte)
Declare Sub Extigush(byval Position As Byte)


Ocr2 = 0




Dim A As Byte
Dim B As Byte
Dim Read_data As Byte
Dim Temprature(9) As Word

Dim Firetem As Word

Dim M As Word
Dim Idx As Word
Dim Pos As Byte

Dim Motor1a As Bit
Dim Motor1b As Bit

Dim Exmotor As Bit

Portd.7 Alias Exmotor
Portc.4 Alias Motor1a
Portc.5 Alias Motor1b


Enable Twi
Twbr = &H16
Twsr = Twsr And &HFC

Cls

Lcd "FIRE EXTINGUSHER"
Lowerline
Lcd "  AIR UNIVERSITY  "
Wait 1


B = 0
Do


Set Motor1a
Reset Motor1b

Wait 1

Reset Motor1a
Reset Motor1b

If B <= 15 Then


   Call I2c_transmit(&Hd0 , 0 )
   Waitus 60
   Call Extigush(b)
   Cls
   Lcd "FIRE EXTINGUSHER"
   Lowerline
   Lcd "servo position" ; B



    For A = 1 To 9 Step 1

          Call I2cread(&Hd0 , A)
          Waitus 60

          Temprature(a) = Read_data

          Print "temprature " ; Temprature(a)
          Print " for register " ; A

    Next A
    Waitus 60

    Max(temprature(1) , M , Idx)
    Print " Maximum Temperature " ; M ; " index " ; Idx
    Cls
   Lcd "max   idx"
   Lowerline
   Lcd M ; "   " ; Idx

    Firetem = Temprature(1) + 8


    If M > Firetem Then
      Do

          For A = 1 To 9 Step 1

                Call I2cread(&Hd0 , A)
                Waitus 60

                 Temprature(a) = Read_data

                 Print "temprature " ; Temprature(a)
                 Print " for register " ; A


          Next A
          Waitus 60

          Max(temprature(1) , M , Idx)
          Print " Maximum Temperature " ; M ; " index " ; Idx

          Print "fire alert"
            If Idx < 4 Then
               Decr B
               Call I2c_transmit(&Hd0 , B )
               Call Extigush(b)
               Waitus 60

            Elseif Idx > 5 Then
               Incr B
               Call I2c_transmit(&Hd0 , B )
               Call Extigush(b)
               Waitus 60
            Else
               Call Extigush(b)
               Waitus 60
            End If
      Loop Until M > Firetem

   Else
         B = B + 1

   End If



Else
   B = 0

End If



Loop



End




Sub I2c_transmit(byval Address As Byte , Byval Reg As Byte )
   Dim Del As Integer

   I2cstart                                                 'start condition
   I2cwbyte Address                                         'slave address
   I2cwbyte Reg                                             'asdress of EEPROM
   I2cwbyte B                                               'value to write
   I2cstop                                                  'stop condition
   Del = B * 60
   Waitus Del
End Sub                                                     '
                                                           '
Sub I2cread(byval Address As Byte , Byval Reg As Byte)      '

 I2cstart                                                   'generate start
 I2cwbyte Address                                           'slave adsress
 I2cwbyte Reg                                               'address of EEPROM
 I2cstart                                                   'repeated start
 I2cwbyte &HD1                                              'slave address (read)
 I2crbyte Read_data , Nack                                  'read byte
 I2cstop


End Sub



Sub Extigush(byval Position As Byte)
'servo values for OCR2 is from 15 -55 only

Select Case Position
Case 0 : Ocr2 = 15
Case 1 : Ocr2 = 16
Case 2 : Ocr2 = 17
Case 3 : Ocr2 = 19
Case 4 : Ocr2 = 20
Case 5 : Ocr2 = 21
Case 6 : Ocr2 = 24
Case 7 : Ocr2 = 25
Case 8 : Ocr2 = 26
Case 9 : Ocr2 = 28
Case 10 : Ocr2 = 29
Case 11 : Ocr2 = 30
Case 12 : Ocr2 = 32
Case 13 : Ocr2 = 33
Case 14 : Ocr2 = 34
Case 15 : Ocr2 = 35
Case 16 : Ocr2 = 36
End Select



End Sub