
'-----------------------------------------------------------------------------------------
'name                     : inkey.bas
'copyright                : (c) 1995-2005, MCS Electronics
'purpose                  : demo: INKEY , WAITKEY
'micro                    : Mega48
'suited for demo          : yes
'commercial addon needed  : no
'-----------------------------------------------------------------------------------------

$regfile = "m8def.dat"                                      ' specify the used micro
$crystal = 8000000                                          ' used crystal frequency
$baud = 9600



Dim A As Byte , B As Byte

   Ddrd.0 = 0
   Ddrb.0 = 1
   Ddrb.1 = 1
   Ddrb.2 = 1
   Ddrb.3 = 1

Set Portb.0
Do
 A = 0
  Bitwait Pind.0 , Reset
  Waitus 150

  If Pind.0 = 1 Then A.0 = 1
   Waitus 100
  If Pind.0 = 1 Then A.1 = 1
   Waitus 100
  If Pind.0 = 1 Then A.2 = 1
   Waitus 100
  If Pind.0 = 1 Then A.3 = 1
   Waitus 100
  If Pind.0 = 1 Then A.4 = 1
   Waitus 100


              If A = 1 Then
              Portb.0 = Not Portb.0
              Elseif A = 2 Then
              Set Portb.1
              Waitms 100
              Reset Portb.1
              End If


  Loop