VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "Comdlg32.ocx"
Begin VB.Form Form1 
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Generate a 16 bits Sine Table"
   ClientHeight    =   3045
   ClientLeft      =   45
   ClientTop       =   435
   ClientWidth     =   5610
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3045
   ScaleWidth      =   5610
   StartUpPosition =   3  'Windows Default
   Begin MSComDlg.CommonDialog CommonDialog 
      Left            =   90
      Top             =   3060
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.CommandButton cmd_SAVETXT 
      Caption         =   "Save Table into a Txt File"
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   2670
      TabIndex        =   2
      Top             =   60
      Width           =   2895
   End
   Begin VB.TextBox txt_SINE_TABLE 
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   2505
      Left            =   60
      MultiLine       =   -1  'True
      ScrollBars      =   2  'Vertical
      TabIndex        =   1
      Top             =   480
      Width           =   5475
   End
   Begin VB.CommandButton cmd_GENERATE_SINE_TABLE 
      Caption         =   "Generate a Sine Table"
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   30
      TabIndex        =   0
      Top             =   60
      Width           =   2625
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub cmd_GENERATE_SINE_TABLE_Click()
Dim i  As Integer
Dim pi As Double
Dim a  As Double

    
    pi = Atn(1#) * 4#           'calculate a value of pi
    txt_SINE_TABLE.Text = ""
    For i = 0 To 90             'generate my sine table
        a = Sin(i / 180 * pi)
        txt_SINE_TABLE.Text = txt_SINE_TABLE.Text + _
                              vbTab + _
                              ".dw 0x" + _
                              Right$("0000" + Hex$(a * 16384), 4) + _
                              " ;" + _
                              Str$(i) + _
                              "o" + _
                              vbCrLf
    Next
End Sub

Private Sub cmd_SAVETXT_Click()
Dim F As Integer
    CommonDialog.CancelError = True
    On Error GoTo SAVE_ERROR
    CommonDialog.Filter = "Text Files|*.txt"
    CommonDialog.ShowSave
    F = FreeFile
    Open CommonDialog.FileName For Output As #F
    Print #F, txt_SINE_TABLE.Text
    Close #F
    Exit Sub
SAVE_ERROR:
    Resume SAVE_ERROR_EXIT
SAVE_ERROR_EXIT:
End Sub

Private Sub Form_Load()

End Sub

Private Sub Form_Unload(Cancel As Integer)
    End
End Sub
