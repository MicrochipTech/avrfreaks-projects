'
'  General Purpose Infra Red Receiver By A.HOOSHANGIAN (C) 2008 V1.00
'  This program get all kind of IrDA Remote Controls and display as string.
'  INT0 & Timer1 used by program (SFH506 Infrared to AVR_INT0)
'  so you can use this program as a multitasking program
'  To save power, idle after receive the manchester codes.
'
'    Each number represent the input pulse time perunit.
'    Therefor the "1" is the minimum_pulse_lenght [T].
'    minimum_pulse_lenght [T], The minimum value from input pulses by Min() function.
'    "1" means 1 * T or [1T] .
'    "2" means 2 * T or [2T].
'    "5" means 5 * T or [5T]. and so on.
'
'    For Example 3 pulses from SFH506:
'      _______________     ___         ___             __________________
'                     |   |   |       |   |           |
'                     | 1 | 1 |   2   | 1 |     3     |
'                     |   |   |       |   |           |
'                      ---     -------     -----------
'    Output String: "11213" it means 1T 1T 2T 1T 3T
'    Although there are 3 pulses in sample, program return 5 delay time.

'    If the time_cycle was greater than 9T then replace with ascii char:
'    0=0 1=1 2=2 .... 9=9 10=: 11=; 12=< 13== 14=> 15=? 16=@ 17=A 18=B ....
'    is equal to:
'    0=chr(48) 1=chr(49).......9=chr(57) 10=chr(58) 11=chr(59) 12=chr(60) ....
'
'------------------------------------------------------------------------------

$regfile = "m8def.dat"
$crystal = 8000000
$baud = 57600

Declare Sub Cleararray
Declare Sub Dispstring

Const Cmp1limit = &H6000                                    'End Of Transmission Time_wait
Const Ilimit = 150                                          'Max 150 pulses per 1 transmission
Const Offset = 20                                           'Make offset for best result

Dim J As Byte
Dim I(ilimit) As Word
Dim Icnt As Word : Icnt = 0
Dim Icntlast As Word
Dim Div As Word
Dim Dstmp As Word
Dim Disp As Bit
Dim Icntoverflow As Bit

Config Int0 = Change
On Int0 Int0isr
Enable Int0

Config Timer1 = Timer , Prescale = 8
Compare1a = Cmp1limit
On Compare1a Cmp1aisr
Enable Compare1a

Call Cleararray

Enable Interrupts

Print : Print "Ready..."

Do
   If Disp = 1 Then Call Dispstring
   Idle                                                     'Save AVR power dissipation
Loop

End

Int0isr:
   I(icnt) = Timer1
   Start Timer1
   Incr Icnt
   If Icnt > Ilimit Then
      Disable Int0
      Set Icntoverflow
      Set Disp
      Icnt = 0
   End If
   Timer1 = 0
Return

Cmp1aisr:
   Disable Int0
   Stop Timer1
   If Icnt > 2 Then                                         'less than 2 pulses do not show
      Icntlast = Icnt - 1
      Set Disp
      Else
      Enable Int0
   End If
   Icnt = 0
Return

Sub Cleararray
   For J = 1 To Ilimit
      I(j) = &HFFFF                                         ' For better minimum finder
   Next J
End Sub

Sub Dispstring
   If Icntoverflow = 1 Then
      Reset Icntoverflow
      Print "ERROR: [Icnt] overflow"
      Else
      Div = Min(i(1)) - Offset
      For J = 1 To Icntlast
         Dstmp = I(j) / Div
         Dstmp = Dstmp + 48                                 'Convert number to ascii code
         Print Chr(dstmp);
      Next J
   End If
   Print
   Call Cleararray
   Reset Disp
   Enable Int0
End Sub