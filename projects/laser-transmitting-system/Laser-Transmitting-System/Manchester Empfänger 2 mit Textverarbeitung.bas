$regfile = "m8def.dat"
$crystal = 6000000
$hwstack 256
$swstack 256



Ddrd.7 = 0                                                  ' Komparator-Signaleingang

Ddrb.5 = 1
Ddrb.3 = 1
Ddrd.4 = 1
Ddrd.6 = 1
Ddrd.5 = 1

Dim A As Word
Dim B As Word
Dim D As Word
Dim C As Word
Dim E As Word
Dim T As Byte
Dim G As Byte
Dim Z As Byte
Z = 0
Dim U As Byte
Dim I As Word
U = 0
G = 0
Dim R As Byte
R = 0

Dim P As String * 1
Dim M As Byte
Dim J As String * 8
Dim K As String * 8
Dim Q As Byte
Dim F As Byte
Dim Aaa As Byte
Dim Abb As Byte
Dim Ac As Byte
Dim Abc As Byte


Dim X(300) As Byte
Dim Cntr As Byte
Dim Ziff As Byte


Declare Sub Ukommando(byval Wert As Byte)
Declare Sub Udata(byval Wert As Byte)
Declare Sub Initialisierung
Declare Sub Lcdloeschen

Config Spi = Hard , Data Order = Msb , Master = Yes , Polarity = Low , Phase = 0 , Clockrate = 4

Dc Alias Portd.4
Ce Alias Portd.6
Re Alias Portd.5

Re = 0                                                      'Reset
Waitus 250
Re = 1

Spiinit                                                     'Initialisierung der SPI-Schnittstelle





Acsr = &B01001000
On Aci Flankenwechsel
Enable Interrupts

On Timer1 Zeit
Config Timer1 = Timer , Prescale = 1
Start Timer1

T = 0
M = 60

Initialisierung
Lcdloeschen



Do

If T = 7 Then
B = A / 4
D = 65534 - B
C = A / 2
E = 65534 - C
Enable Timer1
Timer1 = D
Z = 1
G = 0
Incr T
End If


If U = 8 Then

For I = 1 To 8 Step 1

P = Str(x(i))
J = J + P
Next
M = Binval(j)
Q = M
M = 8 * M
M = M + 8
End If

If U = M Then
Timsk.toie1 = 0
Disable Timer1


Initialisierung
Lcdloeschen


Abb = 16
Abc = 9

For I = 1 To Q

For Aaa = Abc To Abb Step 1
P = Str(x(aaa))
K = K + P
Next Aaa

Ac = Binval(k)
Select Case Ac
Case 1 : Restore 1
Case 2 : Restore 2
Case 3 : Restore 3
Case 4 : Restore 4
Case 5 : Restore 5
Case 6 : Restore 6
Case 7 : Restore 7
Case 8 : Restore 8
Case 9 : Restore 9
Case 10 : Restore 10
Case 11 : Restore 11
Case 12 : Restore 12
Case 13 : Restore 13
Case 14 : Restore 14
Case 15 : Restore 15
Case 16 : Restore 16
Case 17 : Restore 17
Case 18 : Restore 18
Case 19 : Restore 19
Case 20 : Restore 20
Case 21 : Restore 21
Case 22 : Restore 22
Case 23 : Restore 23
Case 24 : Restore 24
Case 25 : Restore 25
Case 26 : Restore 26
Case 27 : Restore 27
End Select

For Cntr = 1 To 6
Read Ziff
Udata Ziff
Next Cntr

K = ""
Abc = Abc + 8
Abb = Abb + 8
Next I

' hier muss noch rückgesetzt werden.
End
End If


Loop

Flankenwechsel:
A = Timer1

If Z = 0 Then
Timer1 = 0
Incr T

Elseif Z = 1 Then
Timer1 = D


If G = 1 Then
Incr U
If Acsr.5 = 0 Then
X(u) = 1
Elseif Acsr.5 = 1 Then
X(u) = 0
End If
End If

End If
Return

Zeit:
Timer1 = E

If G = 0 Then
G = 1

Elseif G = 1 Then
G = 0
End If

Return



Sub Initialisierung
   Ukommando &H21                                           'Erweiterter Kommando-Mode
   Ukommando &HC8
   Ukommando &H06
   Ukommando &H13
   Ukommando &H20                                           'Wieder in den normalen Kommando-Mode
   Ukommando &H0C
End Sub

Sub Lcdloeschen

Ukommando &H20                                              'Normaler Kommando-Mode
Ukommando &H0C                                              'Normaler Bildschirm
Ukommando &H80                                              'Setze RAM-Adresse auf 0
Dc = 1                                                      'Sicherstellen, dass man im Daten-Mode ist
For I = 0 To 503
Udata &H00
Next
End Sub


'------------------------------------------------------------------------------

Sub Ukommando(byval Wert As Byte)
   Dc = 0                                                   'in den Kommando-Mode
   Spiout Wert , 1
   Dc = 1                                                   'zurück in den Daten-Mode
End Sub

Sub Udata(byval Wert As Byte)
   Dc = 1                                                   'um sicher zu gehen, dass man auch wirklich im Datenmode ist
   Ce = 0
   Spiout Wert , 1
   Ce = 1
End Sub


'********************** Daten (ASCII) ******************************************


2:
Data &H00 , &H7C , &H12 , &H11 , &H12 , &H7C
3:
Data &H00 , &H7F , &H49 , &H49 , &H49 , &H36
4:
Data &H00 , &H3E , &H41 , &H41 , &H41 , &H22
5:
Data &H00 , &H7F , &H41 , &H41 , &H41 , &H3E
6:
Data &H00 , &H7F , &H49 , &H49 , &H49 , &H41
7:
Data &H00 , &H7F , &H09 , &H09 , &H09 , &H01
8:
Data &H00 , &H3E , &H41 , &H49 , &H49 , &H3A
9:
Data &H00 , &H7F , &H08 , &H08 , &H08 , &H7F
10:
Data &H00 , &H00 , &H41 , &H7F , &H41 , &H00
11:
Data &H00 , &H20 , &H41 , &H41 , &H41 , &H3F
12:
Data &H00 , &H7F , &H08 , &H14 , &H22 , &H41
13:
Data &H00 , &H7F , &H40 , &H40 , &H40 , &H40
14:
Data &H00 , &H7F , &H02 , &H04 , &H02 , &H7F
15:
Data &H00 , &H7F , &H04 , &H08 , &H10 , &H7F
16:
Data &H00 , &H3E , &H41 , &H41 , &H41 , &H3E
17:
Data &H00 , &H7F , &H09 , &H09 , &H09 , &H06
18:
Data &H00 , &H3E , &H41 , &H51 , &H61 , &H7E
19:
Data &H00 , &H7F , &H09 , &H19 , &H29 , &H46
20:
Data &H00 , &H06 , &H49 , &H49 , &H49 , &H30
21:
Data &H00 , &H01 , &H01 , &H7F , &H01 , &H01
22:
Data &H00 , &H3F , &H40 , &H40 , &H40 , &H3F
23:
Data &H00 , &H1F , &H20 , &H40 , &H20 , &H1F
24:
Data &H00 , &H7F , &H20 , &H10 , &H20 , &H7F
25:
Data &H00 , &H63 , &H14 , &H08 , &H14 , &H63
26:
Data &H00 , &H03 , &H04 , &H78 , &H04 , &H03
27:
Data &H00 , &H61 , &H51 , &H49 , &H45 , &H43

'Leerzeichen
1:
Data &H00 , &H00 , &H00 , &H00 , &H00 , &H00