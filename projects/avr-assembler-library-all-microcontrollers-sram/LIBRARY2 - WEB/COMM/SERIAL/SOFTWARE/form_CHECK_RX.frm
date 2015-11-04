VERSION 5.00
Object = "{648A5603-2C6E-101B-82B6-000000000014}#1.1#0"; "mscomm32.ocx"
Begin VB.Form form_CHECK_RX 
   BorderStyle     =   4  'Fixed ToolWindow
   Caption         =   "Check Data Reception at 115200 Bauds"
   ClientHeight    =   5085
   ClientLeft      =   45
   ClientTop       =   345
   ClientWidth     =   10290
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5085
   ScaleWidth      =   10290
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmd_PRESENT 
      Caption         =   "GPS Present"
      Height          =   375
      Left            =   8640
      TabIndex        =   2
      Top             =   720
      Width           =   1335
   End
   Begin VB.CommandButton cmd_RTS 
      Caption         =   "RTS"
      Height          =   375
      Left            =   8640
      TabIndex        =   1
      Top             =   240
      Width           =   1335
   End
   Begin VB.TextBox txt_INPUT 
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   4215
      Left            =   240
      MultiLine       =   -1  'True
      ScrollBars      =   2  'Vertical
      TabIndex        =   0
      Top             =   120
      Width           =   8175
   End
   Begin MSCommLib.MSComm MSComm 
      Left            =   9480
      Top             =   4440
      _ExtentX        =   1005
      _ExtentY        =   1005
      _Version        =   393216
      DTREnable       =   -1  'True
      BaudRate        =   115200
   End
End
Attribute VB_Name = "form_CHECK_RX"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub cmd_PRESENT_Click()
Dim l As Long
Dim s As String

    If MSComm.PortOpen Then MSComm.PortOpen = False
    MSComm.CommPort = 11
    MSComm.PortOpen = True
    
    'SEND P
    Do
        MSComm.Output = Chr$(l And &HFF)
        Do
            DoEvents
        Loop Until MSComm.InBufferCount >= 1
        s = MSComm.Input
        If Len(s) > 1 Or s <> Chr$(l And &HFF) Then
            txt_INPUT.Text = txt_INPUT.Text + "error " + Trim(l)
        Else
            Caption = MSComm.Input + Trim(l)
        End If
        l = l + 1
    Loop
End Sub

Private Sub cmd_RTS_Click()
    MSComm.RTSEnable = Not (MSComm.RTSEnable)
    If MSComm.RTSEnable Then
        cmd_RTS.Caption = "RTS ON"
    Else
        cmd_RTS.Caption = "RTS OFF"
    End If
End Sub

Private Sub Form_Load()
Dim sInp    As String
Dim C       As Long
Dim T       As Variant

    Me.Show
    
    Exit Sub
    MSComm.CommPort = 11
    MSComm.PortOpen = True
'    Do
'        MSComm.RTSEnable = Not (MSComm.RTSEnable)
'        T = Timer
'        Do
'            DoEvents
'        Loop Until (Timer - T) > 1
'    Loop
    
    Do
      Do
        DoEvents
      Loop Until MSComm.InBufferCount > 1000
      sInp = MSComm.Input
      txt_INPUT.Text = txt_INPUT.Text + Trim(C) + vbCrLf
      txt_INPUT.SelStart = Len(txt_INPUT.Text)
      C = C + 1
    Loop
End Sub

Private Sub Form_Unload(Cancel As Integer)
    If MSComm.PortOpen Then MSComm.PortOpen = False
    End
End Sub
