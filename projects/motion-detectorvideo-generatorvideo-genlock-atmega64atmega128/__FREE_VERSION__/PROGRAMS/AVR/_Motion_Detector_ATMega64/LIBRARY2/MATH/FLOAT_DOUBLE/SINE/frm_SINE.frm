VERSION 5.00
Object = "{3B7C8863-D78F-101B-B9B5-04021C009402}#1.2#0"; "Richtx32.ocx"
Begin VB.Form frm_SINE 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "SINE FUNCTION IMPLEMENTATION"
   ClientHeight    =   3345
   ClientLeft      =   45
   ClientTop       =   435
   ClientWidth     =   9060
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3345
   ScaleWidth      =   9060
   ShowInTaskbar   =   0   'False
   StartUpPosition =   1  'CenterOwner
   Begin RichTextLib.RichTextBox rich_RESULT 
      Height          =   2715
      Left            =   30
      TabIndex        =   1
      Top             =   570
      Width           =   9015
      _ExtentX        =   15901
      _ExtentY        =   4789
      _Version        =   393217
      Enabled         =   -1  'True
      ScrollBars      =   2
      TextRTF         =   $"frm_SINE.frx":0000
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "Courier New"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
   End
   Begin VB.CommandButton cmd_COMPUTE 
      Caption         =   "Compute"
      Height          =   435
      Left            =   3810
      TabIndex        =   0
      Top             =   90
      Width           =   1245
   End
End
Attribute VB_Name = "frm_SINE"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub cmd_COMPUTE_Click()
Dim i As Long, result As String

    Cls
    If -5.1E-31 > -5.1E+30 Then
        Print "sim"
    Else
        Print "nao"
    End If
    
    Screen.MousePointer = vbHourglass
    For i = -1000000 To 1000000 Step 1000
        result = result + "N=" & Format(Str$(i), "@@@@@@") + _
                          " Sin(" & Format(Format(Sin(i), "#0.00000000000000"), "@@@@@@@@@@@@@@@@@") + _
                        ")  FSin(" & Format(Format(FSIN(i), "#0.00000000000000"), "@@@@@@@@@@@@@@@@@") + _
                        ") Error=" & Format(Format(Sin(i) - FSIN(i), "#0.00000000000000"), "@@@@@@@@@@@@@@@@@") + vbCrLf
    Next
    rich_RESULT.Text = result
    Screen.MousePointer = vbArrow
End Sub

Function FSIN(ByVal x As Double) As Double
'+------------------------------------------------------------------+
'| IMPLEMENT SINE FOR RANGE OF NUMBER USING PARCIAL SINE FUNCTION   |
'| THAT CALCULATE ONLY -PI/2 TO PI/2 ARC                            |
'+------------------------------------------------------------------+

Const C_PI2 = 1.5707963267949               'PI/2   CONSTANT
Const C_PI = 3.14159265358979               'PI     CONSTANT
Const C_3PI2 = 4.71238898038469             '3*PI/2 CONSTANT
Const C_2PI = 6.28318530717959              '2*PI   CONSTANT

Dim s As Boolean            'to hold a signal
Dim f As Double

s = x < 0                   'save signal
x = Abs(x)                  'compute absolute value
x = x / C_2PI               'reduce to fist quandrat
x = x - Int(x)
x = x * C_2PI
If x >= C_3PI2 Then
    x = x - C_2PI
Else
    If x >= C_PI Then
        x = C_PI - x
    Else
        If x >= C_PI2 Then
            x = C_PI - x
        End If
    End If
End If
f = Sin(x)                  'calculate partial sine function
If s Then f = -f            'invert signal if input value less than zero
FSIN = f
End Function

Function Sinus(ByVal arg As Double, ByVal quad As Integer)
Const twoopi = 0.636619772367581
Const p0 = 13578840.9787738
Const p1 = -4942908.10090284
Const p2 = 440103.053537527
Const p3 = -13847.2724998245
Const p4 = 145.968840666577
Const q0 = 8644558.65292254
Const q1 = 408179.22523433
Const q2 = 9463.09610153821
Const q3 = 132.653490878614

Dim e As Double, f As Double
Dim ysq As Double
Dim x As Double, y As Double
Dim k As Integer
Dim temp1 As Double, temp2 As Double

    x = arg
    If x < 0 Then
        x = -x
        quad = quad + 2
    End If
    x = x * twoopi '/*underflow?*/
    If (x > 32764) Then
        y = x - Int(x): e = Int(x)
        e = e + quad
        f = Int(0.25 * e)
        quad = e - 4 * f
    Else
        k = x
        y = x - k
        quad = (quad + k) And 3
    End If
    If (quad And 1) Then y = 1 - y
    If (quad > 1) Then y = -y

    ysq = y * y
    temp1 = ((((p4 * ysq + p3) * ysq + p2) * ysq + p1) * ysq + p0) * y
    temp2 = ((((ysq + q3) * ysq + q2) * ysq + q1) * ysq + q0)
    Sinus = temp1 / temp2

End Function

