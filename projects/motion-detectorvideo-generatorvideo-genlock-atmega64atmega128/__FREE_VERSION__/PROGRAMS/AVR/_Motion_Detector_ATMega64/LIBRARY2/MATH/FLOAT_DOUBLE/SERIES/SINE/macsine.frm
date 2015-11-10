VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Calculate a sine function using maclaurium series"
   ClientHeight    =   4275
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   11175
   BeginProperty Font 
      Name            =   "Courier New"
      Size            =   8.25
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   LinkTopic       =   "Form1"
   ScaleHeight     =   4275
   ScaleWidth      =   11175
   StartUpPosition =   3  'Windows Default
   Begin VB.TextBox Text1 
      Height          =   405
      Left            =   8940
      TabIndex        =   1
      Top             =   840
      Width           =   1845
   End
   Begin VB.CommandButton Command1 
      Caption         =   "Compute"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   705
      Left            =   8940
      TabIndex        =   0
      Top             =   90
      Width           =   1875
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Dim fat(1 To 13) As Double
Private Sub Command1_Click()
'+--------------------------------------+
'| calculate a maclaurium sine series   |
'+--------------------------------------+
Dim i As Integer, s As Double, x As Double, j As Integer, x2 As Double
    Cls
    x = Val(Text1.Text)

    j = UBound(fat)
    s = fat(j)
    j = j - 1
    x2 = -x * x
    Do
        s = s * x2 + fat(j)
        Print j * 2 - 1, s, hexview(s)
        j = j - 1
    Loop Until j < 1
    s = s * x
    Print
    Print "me ="; s
    Print "sys="; Sin(x)
    Caption = s
End Sub

Private Sub Form_Load()
'+--------------------------------------+
'| generate a inverse fatorial table    |
'+--------------------------------------+

Dim i As Integer, f As Double, c As Integer
    f = 1
    c = 1
    For i = 1 To UBound(fat)
        fat(i) = 1 / f
        c = c + 2
        f = f * c * (c - 1)
    Next
End Sub

Function hexview(ByVal x As Double) As String
Dim l As Long, i As Integer, s As String

    If x = 0 Then Exit Function
    s = ""
    Do
        x = x * 2
    Loop Until (x And 128) = 128
    s = s + Right$("00" + Hex$(x), 2) + " "
    For i = 1 To 6
        x = x - Int(x)
        x = x * 256
        s = s + Right$("00" + Hex$(x), 2) + " "
    Next
    hexview = s
End Function
