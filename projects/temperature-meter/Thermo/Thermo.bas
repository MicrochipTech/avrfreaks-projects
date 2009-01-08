$regfile = "m8def.dat"
Config 1wire = Portb.0
Config Pinc.0 = Output
Config Pinc.1 = Output
Config Pind.2 = Input
Config Timer1 = Pwm , Pwm = 10 , Compare A Pwm = Clear Up , Prescale = 1
Pwm1a = 0
Dim Crc As Byte
Dim Tmp As Byte
Dim Y As Byte
Dim Pointer As Integer
Dim K As Integer
Dim L As Integer
Dim Ar(9) As Byte
Dim Temperatur As Single
Dim Minne As Single , Minne2 As Single
Dim Skala As Integer
Dim Nuvarde As Integer
Dim Maxtemp As Single , Mintemp As Single
Dim Tempminne As Single
Dim Testmode As Byte
Dim Steptime As Byte


Declare Sub Convert()
Declare Sub Checkcrc()

Testmode = 0
Pointer = 0
Skala = 389                                                 '0
Crc = 0
Maxtemp = -100
Mintemp = 100
Steptime = 40

If Pind.2 = 0 Then
   Portc.0 = 1
   Portc.1 = 0
   Testmode = 1
   Pwm1a = 1023
   Wait 8
   Pwm1a = 0
   Wait 8
   Pwm1a = 1023 - 511
   Wait 8
   Portc.0 = 0
   Portc.1 = 0
Elseif Pind.2 = 1 Then
   Testmode = 0
End If

Nuvarde = 511
Config Timer0 = Timer , Prescale = 256
On Ovf0 Tim0_isr
On Int0 Button1
Set Tifr.1
Enable Timer0
Enable Int0
Start Timer0
Enable Interrupts

Do
   If Nuvarde < Skala Then
      Incr Nuvarde
   Elseif Nuvarde > Skala Then
      Decr Nuvarde
   End If
   Waitms Steptime
   If Nuvarde > 1023 Then
      Nuvarde = 1023
   End If
   If Nuvarde < 0 Then
      Nuvarde = 0
   End If
   Pwm1a = 1023 - Nuvarde
Loop

Tim0_isr:
   Incr Pointer
   Select Case Pointer
   Case 1
         If Testmode = 1 Then
            Portc.0 = 1
            Portc.1 = 0
         End If
         1wreset Pinb , 0
         1wwrite &HCC , 1 , Pinb , 0
         1wwrite &H44 , 1 , Pinb , 0
   Case 2
        If Testmode = 1 Then
            Portc.0 = 0
            Portc.1 = 0
         End If
   Case 60
         If Testmode = 1 Then
            Portc.0 = 0
            Portc.1 = 1
         End If
         1wreset Pinb , 0
         1wwrite &HCC , 1 , Pinb , 0
         1wwrite &HBE , 1 , Pinb , 0
   Case 61
         If Testmode = 1 Then
            Portc.0 = 0
            Portc.1 = 0
         End If
   Case 70
         For K = 1 To 9
            Ar(k) = 1wread(1 , Pinb , 0)
         Next
   Case 75
         Call Checkcrc
   Case 80
         If Crc = 0 Then
            Call Convert
         End If
   Case 150
         Pointer = 0
   End Select
Return

Sub Convert()
   Select Case Ar(2)
      Case Is > 0
         Minne = 256 - Ar(1)
         Minne = Minne - 0.25
         Minne2 = Ar(7) / Ar(8)
         Temperatur = Minne + Minne2
         Temperatur = Temperatur / 2
         Temperatur = Temperatur * -1
      Case 0
         Minne = Ar(1)
         Minne = Minne - 0.25
         Minne2 = Ar(7) / Ar(8)
         Temperatur = Minne + Minne2
         Temperatur = Temperatur / 2
      End Select
      'Temperatur = Temperatur - 1.1
      If Temperatur > Maxtemp Then
         Maxtemp = Temperatur
      End If
      If Temperatur < Mintemp Then
         Mintemp = Temperatur
      End If
      Temperatur = Temperatur + 50
      Temperatur = Temperatur * 10.24
      Skala = Round(temperatur)
      If Skala > 1023 Then
         Skala = 1023
      End If
      If Skala < 0 Then
         Skala = 0
      End If
      Tempminne = Skala
End Sub

Sub Checkcrc()
   Crc = 0
   For Y = 1 To 9
      Tmp = Crc Xor Ar(y)
      Crc = Lookup(tmp , Crc8)
   Next Y


   If Ar(1) = 0 And Ar(2) = 0 And Ar(3) = 0 And Ar(4) = 0 And Ar(5) = 0 And Ar(6) = 0 And Ar(7) = 0 And Ar(8) = 0 Then
      Crc = 1
   End If

   If Ar(1) = 1 And Ar(2) = 1 And Ar(3) = 1 And Ar(4) = 1 And Ar(5) = 1 And Ar(6) = 1 And Ar(7) = 1 And Ar(8) = 1 Then
      Crc = 1
   End If

   If Crc > 0 Then
      Portc.0 = 1
      Pointer = 130
   End If
End Sub

Button1:
   Disable Int0
   Dim C1 As Integer , C2 As Integer
   Dim Visa As Single
   Stop Timer0
   Waitms 650
   '**** Reset memory if button is held down more than 650 ms
   If Pind.2 = 0 Then
      Portc.0 = 1
      Waitms 500
      Portc.0 = 0
      Waitms 2000
      Maxtemp = -100
      Mintemp = 100
      Pointer = 0
      Enable Int0
      Start Timer0

   Elseif Pind.2 = 1 Then
      '***** Show MAX
      Visa = Maxtemp + 50
      Visa = Visa * 10.24
      Skala = Round(visa)
      If Skala > 1023 Then
         Skala = 1023
      End If
      If Skala < 0 Then
         Skala = 0
      End If
      Do
      If Nuvarde < Skala Then
         Incr Nuvarde
      Elseif Nuvarde > Skala Then
         Decr Nuvarde
      End If
      Waitms 5
      Pwm1a = 1023 - Nuvarde
      Loop Until Nuvarde = Skala
      Waitms 60
      Portc.0 = 1
      Portc.1 = 0
      Waitms 1500
      Portc.0 = 0
      Portc.1 = 0
      Waitms 60

      '***** Show MIN
      Visa = Mintemp + 50
      Visa = Visa * 10.24
      Skala = Round(visa)
      If Skala > 1023 Then
         Skala = 1023
      End If
      If Skala < 0 Then
         Skala = 0
      End If
      Do
      If Nuvarde < Skala Then
         Incr Nuvarde
      Elseif Nuvarde > Skala Then
         Decr Nuvarde
      End If
      Waitms 5
      Pwm1a = 1023 - Nuvarde
      Loop Until Nuvarde = Skala
      Waitms 60
      Portc.0 = 0
      Portc.1 = 1
      Waitms 1500
      Portc.0 = 0
      Portc.1 = 0
      Waitms 60

      Skala = Tempminne
      If Skala > 1023 Then
         Skala = 1023
      End If
      If Skala < 0 Then
         Skala = 0
      End If

      Do
      If Nuvarde < Skala Then
         Incr Nuvarde
      Elseif Nuvarde > Skala Then
         Decr Nuvarde
      End If
      Waitms 5
      Pwm1a = 1023 - Nuvarde
      Loop Until Nuvarde = Skala

      Pointer = 140
      Enable Int0
      Start Timer0
   End If
Return

Crc8:
Data 0 , 94 , 188 , 226 , 97 , 63 , 221 , 131 , 194 , 156
Data 126 , 32 , 163 , 253 , 31 , 65 , 157 , 195 , 33 , 127
Data 252 , 162 , 64 , 30 , 95 , 1 , 227 , 189 , 62 , 96
Data 130 , 220 , 35 , 125 , 159 , 193 , 66 , 28 , 254 , 160
Data 225 , 191 , 93 , 3 , 128 , 222 , 60 , 98 , 190 , 224
Data 2 , 92 , 223 , 129 , 99 , 61 , 124 , 34 , 192 , 158
Data 29 , 67 , 161 , 255 , 70 , 24 , 250 , 164 , 39 , 121
Data 155 , 197 , 132 , 218 , 56 , 102 , 229 , 187 , 89 , 7
Data 219 , 133 , 103 , 57 , 186 , 228 , 6 , 88 , 25 , 71
Data 165 , 251 , 120 , 38 , 196 , 154 , 101 , 59 , 217 , 135
Data 4 , 90 , 184 , 230 , 167 , 249 , 27 , 69 , 198 , 152
Data 122 , 36 , 248 , 166 , 68 , 26 , 153 , 199 , 37 , 123
Data 58 , 100 , 134 , 216 , 91 , 5 , 231 , 185 , 140 , 210
Data 48 , 110 , 237 , 179 , 81 , 15 , 78 , 16 , 242 , 172
Data 47 , 113 , 147 , 205 , 17 , 79 , 173 , 243 , 112 , 46
Data 204 , 146 , 211 , 141 , 111 , 49 , 178 , 236 , 14 , 80
Data 175 , 241 , 19 , 77 , 206 , 144 , 114 , 44 , 109 , 51
Data 209 , 143 , 12 , 82 , 176 , 238 , 50 , 108 , 142 , 208
Data 83 , 13 , 239 , 177 , 240 , 174 , 76 , 18 , 145 , 207
Data 45 , 115 , 202 , 148 , 118 , 40 , 171 , 245 , 23 , 73
Data 8 , 86 , 180 , 234 , 105 , 55 , 213 , 139 , 87 , 9
Data 235 , 181 , 54 , 104 , 138 , 212 , 149 , 203 , 41 , 119
Data 244 , 170 , 72 , 22 , 233 , 183 , 85 , 11 , 136 , 214
Data 52 , 106 , 43 , 117 , 151 , 201 , 74 , 20 , 246 , 168
Data 116 , 42 , 200 , 150 , 21 , 75 , 169 , 247 , 182 , 232
Data 10 , 84 , 215 , 137 , 107 , 53