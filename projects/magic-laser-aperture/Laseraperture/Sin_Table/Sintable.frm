VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Sin Table"
   ClientHeight    =   6690
   ClientLeft      =   60
   ClientTop       =   600
   ClientWidth     =   9210
   LinkTopic       =   "Form1"
   ScaleHeight     =   6690
   ScaleWidth      =   9210
   StartUpPosition =   3  '´°¿ÚÈ±Ê¡
   Begin VB.TextBox Text1 
      BeginProperty Font 
         Name            =   "ËÎÌå"
         Size            =   12
         Charset         =   134
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   6015
      Left            =   120
      MultiLine       =   -1  'True
      TabIndex        =   0
      Text            =   "Sintable.frx":0000
      Top             =   240
      Width           =   8775
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Const PI As Double = 3.14159265358979
Const TWO_PI As Double = 6.28318530717959


Private Sub Form_Load()

Dim i As Single
Dim Sin_Tab As String
Sin_Tab = "const uchar Sin_Table[256]={ "

For i = 0 To 255
Sin_Tab = Sin_Tab & Round(128 + 127 * Sin(i * TWO_PI / 256), 0)
Sin_Tab = Sin_Tab & ", "
Next

i = Len(Sin_Tab)
Sin_Tab = Left(Sin_Tab, i - 2)

Sin_Tab = Sin_Tab & " };"

Text1.Text = Sin_Tab

Clipboard.SetText Text1.Text
End Sub

