 $regfile = "m8def.dat"
$crystal = 6000000
$hwstack 256
$swstack 256

Dim X(450) As Byte
Dim Y(350) As Byte
Dim H(27) As String * 1                                     'Array für Codiertes Byte und Präambel
Dim I As Byte                                               'For-Schleifen-Zählvariable
Dim R As Byte                                               'For-Schleifen-Zählvariable
Dim B As Word                                               '
Dim T As Word
Dim L As Word
Dim O As Word
Dim Q(25) As Byte
Dim U As Byte
U = 1
Dim J As Byte
J = 1
Dim W As Byte
W = 1
Dim E As Byte
Dim M As String * 8
Dim P As String * 8
Dim C As Byte
C = 0
Dim D As Word
D = 0
Dim Z As Word
Z = 1
Dim K As Word

Ddrb.1 = 1                                                  'FET-Ausgang

Ddrb.2 = 0                                                  'Sende-Taster-Eingang
Portb.2 = 1

Ddrb.3 = 0
Portb.3 = 1                                                 'Up-Taster

Ddrb.4 = 0
Portb.4 = 1                                                 'Down-Taster

Ddrb.5 = 0
Portb.5 = 1                                                 'Enter-Taster

H(1) = " "
H(2) = "A"
H(3) = "B"
H(4) = "C"
H(5) = "D"
H(6) = "E"
H(7) = "F"
H(8) = "G"
H(9) = "H"
H(10) = "I"
H(11) = "J"
H(12) = "K"
H(13) = "L"
H(14) = "M"
H(15) = "N"
H(16) = "O"
H(17) = "P"
H(18) = "Q"
H(19) = "R"
H(20) = "S"
H(21) = "T"
H(22) = "U"
H(23) = "V"
H(24) = "W"
H(25) = "X"
H(26) = "Y"
H(27) = "Z"

X(1) = 0
X(2) = 1
X(3) = 0
X(4) = 1
X(5) = 0
X(6) = 1
X(7) = 0
X(8) = 1

Dim Hg As Byte

Config Lcd = 20 * 2
Config Lcdpin = Pin , E = Portb.0 , Rs = Portd.4 , Db7 = Portd.3 , Db6 = Portd.2 , Db5 = Portd.1 , Db4 = Portd.0
Cursor On Blink
Cls



Config Timer1 = Timer , Prescale = 1
Enable Interrupts
Enable Timer1
On Timer1 Senden
Stop Timer1
L = 54000                                                   'Zuerst die Präambel mit Zeitabstand x

O = 65534 - L
O = O / 2
O = 65534 - O

T = 1
Locate 1 , 1
Lcd "Bitte Text eingeben"
Locate 2 , 1

Do

If Pinb.2 = 0 Then                                          ' Senden-Taste
Waitms 200
Cls
Hg = W - 1
Q(1) = Hg

For I = 1 To U
E = Q(i)


For R = 1 To 8
M = Bin(e)
P = Mid(m , R , 1)
C = Val(p)
Incr D
Y(d) = C
Next R

Next I


' Jetzt wird dieses Byte  per Manchester-Verfahren codiert:
' 1=01
' 0=10

B = 9
K = 8 * U
For I = 1 To K Step 1

If Y(i) = 1 Then
X(b) = 0
Incr B
X(b) = 1

Elseif Y(i) = 0 Then
X(b) = 1
Incr B
X(b) = 0
End If

Incr B

Next I


' Nun haben wir im Array x(1-...) das Manchester-Codierte Byte und die (nicht codierte) Präambel.
' Jetzt muss dieses Byte gesendet werden.
' Dazu verwenden wir Timer1


Timer1 = L
Start Timer1

End If

Z = U * 16
Z = Z + 9

If T = 9 Then
L = O + 250                                                 'Dann das Byte mit Flankenwechsel nach Zeitabstand x/2
Timer1 = L

Portb.1 = X(t)
Incr T
End If



If T = Z Then
Stop Timer1
Timer1 = 0

T = 1
Portb.1 = 0
L = 54000
U = 1
J = 1
W = 1
C = 0
D = 0
Z = 20
O = 65534 - L
O = O / 2
O = 65534 - O


Cls
Locate 1 , 1
Lcd " Text gesendet"
Wait 2
Locate 1 , 1
Lcd "Bitte Text eingeben"
Locate 2 , 1
Lcd "                           "
End If


If Pinb.3 = 0 Then                                          ' Up-Taste
Waitms 200
Incr J
If J = 28 Then
J = 1
End If
Locate 2 , W
Lcd H(j)
End If

If Pinb.4 = 0 Then                                          ' Down-Taste
Waitms 200
Decr J
If J = 0 Then
J = 27
End If
Locate 2 , W
Lcd H(j)
End If

If Pinb.5 = 0 Then
Incr W                                                      'Enter-Taste
Waitms 200

Incr U
Q(u) = J
Locate 2 , W
J = 1
Lcd H(j)
End If



Loop

Senden:
Timer1 = L

Portb.1 = X(t)
Incr T
Return

End                                                         'end program