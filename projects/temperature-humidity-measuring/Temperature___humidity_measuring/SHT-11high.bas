'//////////////////////////////////////////////////////////////
'///    FastAVR Basic Compiler for AVR by MICRODESIGN       ///
'///    SHT11/15 Temperature and Humidity measurement       ///
'///    14/12 bit                                           ///
'//////////////////////////////////////////////////////////////
$Device= 8515
$Stack = 32                	' stack size
$Clock = 7.3728
$Baud=115200
$ShiftOut Data=PORTD.2, Clk=PORTD.3, 1
$LeadChar=" ", Format(2,3)

$Def dDta=DDRD.2
$Def inDta=PIND.2
$Def Dta=PORTD.2
$Def Sck=PORTD.3

Declare Sub Init()
Declare Sub StartTX()
Declare Sub CommReset()
Declare Function WriteByte(db As Byte) As Byte
Declare Function ReadByte(ack As Byte) As Byte
Declare Function Measure(what As Byte) As Word

Dim tmp As Integer, ftmp As Float
Dim n As Byte
Dim Cta As Byte, Ctb As Integer

Const True=1, False=0
Const ACK=1, NAK=0

Const TmpSHT=&h03
Const HumSHT=&h05
Const ResSHT=&h1e
Const Rstat =&h07
Const Wstat =&h06

Const c1=-4, c2=0.0405, c3=-2.8E-06

Init()

Do
	tmp=Measure(TmpSHT)		' measure Temperature
	ftmp=tmp/100-40
	Print "Temp:"; ftmp
	
	tmp=Measure(HumSHT)		' measure Humidity
	ftmp=c1+tmp*c2+Sqr(tmp)*c3
	Print "Humy:"; ftmp
	Print
	Wait 1
Loop

'//////////////////////////////////////////////////////////////
Sub Init()
WaitMs 20
CommReset()
WriteByte(Wstat)		' write to Status
Set dDta
WriteByte(0)			' 14bit for T, 12 bit for RH
End Sub

'//////////////////////////////////////////////////////////////
Function Measure(what As Byte) As Word
Local tmp As Word

CommReset()
If WriteByte(what)=0 Then	' isue Cmd
	BitWait inDta, 0			' wait for data ready
	tmp=ReadByte(ACK)			' read first byte
	Shift(Left, 8, tmp)			' make it MSB
	tmp=tmp Or ReadByte(NAK)	' read second byte and combine
	'n=ReadByte(NAK)			' no CRC for now
Else
	Print "No sensor!"
	tmp=0
End If
Return tmp
End Function

'//////////////////////////////////////////////////////////////
Sub StartTX()
Set dDta
Set Sck
Reset Dta
Reset Sck
Set Sck
Set Dta
Reset Sck
End Sub

'//////////////////////////////////////////////////////////////
Sub CommReset()
Local i As Byte

Set Dta
For i=0 To 8
	Set Sck: Reset Sck
Next
StartTX()
End Sub

'//////////////////////////////////////////////////////////////
Function WriteByte(db As Byte) As Byte
Local mask As Byte

ShiftOut db				' send data
Reset dDta				' back to input
Set Sck:Reset Sck		' clock ACK
Return inDta			' return ACK (0=SHT present)
End Function

'//////////////////////////////////////////////////////////////
Function ReadByte(ack As Byte) As Byte
Local i As Byte, db As Byte

db=ShiftIn
Set dDta
Dta=Not ack
Set Sck
Reset Sck
Reset dDta
Return db
End Function

