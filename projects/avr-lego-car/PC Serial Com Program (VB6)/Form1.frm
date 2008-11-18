VERSION 5.00
Object = "{648A5603-2C6E-101B-82B6-000000000014}#1.1#0"; "MSCOMM32.OCX"
Begin VB.Form Intelligent_car_form 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Intelligent car control panel..."
   ClientHeight    =   4830
   ClientLeft      =   45
   ClientTop       =   345
   ClientWidth     =   8685
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4830
   ScaleWidth      =   8685
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.Frame Connect_frame 
      Caption         =   "Connection settings"
      Height          =   2175
      Left            =   4800
      TabIndex        =   22
      Top             =   2520
      Width           =   3135
      Begin VB.CommandButton Disconnect_bt 
         Caption         =   "Disconnect"
         Height          =   495
         Left            =   1680
         TabIndex        =   25
         Top             =   1560
         Width           =   1215
      End
      Begin VB.CommandButton Connectbt 
         Caption         =   "Connect"
         Height          =   495
         Left            =   240
         TabIndex        =   24
         Top             =   1560
         Width           =   1215
      End
      Begin VB.TextBox PortNotxt 
         Height          =   375
         Left            =   2160
         TabIndex        =   23
         Text            =   "1"
         Top             =   480
         Width           =   615
      End
      Begin VB.Label Label4 
         Caption         =   "Select COM port number:"
         Height          =   615
         Left            =   240
         TabIndex        =   26
         Top             =   480
         Width           =   1935
      End
   End
   Begin VB.Frame ADC_frame 
      Caption         =   "ADC results"
      Height          =   2175
      Left            =   120
      TabIndex        =   19
      Top             =   2520
      Width           =   3975
      Begin VB.CommandButton Refresh_adc_bt 
         Caption         =   "Refresh ADC data"
         Height          =   495
         Left            =   2160
         TabIndex        =   21
         Top             =   840
         Width           =   1575
      End
      Begin VB.TextBox Text1 
         Height          =   1455
         Left            =   240
         MultiLine       =   -1  'True
         ScrollBars      =   2  'Vertical
         TabIndex        =   20
         Top             =   360
         Width           =   1575
      End
   End
   Begin VB.Frame Hut_frame 
      Caption         =   "Hut"
      Height          =   2295
      Left            =   6480
      TabIndex        =   14
      Top             =   120
      Width           =   2175
      Begin VB.TextBox Angletxt 
         Height          =   495
         Left            =   240
         TabIndex        =   17
         Text            =   "0"
         Top             =   600
         Width           =   1695
      End
      Begin VB.CommandButton Movehutbt 
         Caption         =   "Move hut"
         Height          =   615
         Left            =   1200
         TabIndex        =   16
         Top             =   1440
         Width           =   735
      End
      Begin VB.CommandButton Inithutbt 
         Caption         =   "Initialize hut"
         Height          =   615
         Left            =   240
         TabIndex        =   15
         Top             =   1440
         Width           =   735
      End
      Begin VB.Label Angle 
         Caption         =   "Angle (0-48)"
         Height          =   255
         Left            =   240
         TabIndex        =   18
         Top             =   240
         Width           =   975
      End
   End
   Begin VB.Frame Steering_frame 
      Caption         =   "Steering"
      Height          =   2295
      Left            =   4200
      TabIndex        =   10
      Top             =   120
      Width           =   2175
      Begin VB.CommandButton Turnbt 
         Caption         =   "Turn"
         Height          =   615
         Left            =   480
         TabIndex        =   13
         Top             =   1440
         Width           =   1095
      End
      Begin VB.TextBox steeringtxt 
         Height          =   495
         Left            =   240
         TabIndex        =   11
         Text            =   "4"
         Top             =   600
         Width           =   1575
      End
      Begin VB.Label Label3 
         Caption         =   "Steering posotion (5-250)"
         Height          =   375
         Left            =   240
         TabIndex        =   12
         Top             =   240
         Width           =   1815
      End
   End
   Begin VB.Frame Acceleration_frame 
      Caption         =   "Acceleration"
      Height          =   2295
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   3975
      Begin VB.CommandButton movedistance 
         Caption         =   "Move distance"
         Height          =   615
         Left            =   2640
         TabIndex        =   7
         Top             =   1440
         Width           =   1095
      End
      Begin VB.CommandButton Stopmoving 
         Caption         =   "Stop moving"
         Height          =   615
         Left            =   1440
         TabIndex        =   6
         Top             =   1440
         Width           =   1095
      End
      Begin VB.CommandButton Startmoving 
         Caption         =   "Start moving"
         Height          =   615
         Left            =   240
         TabIndex        =   5
         Top             =   1440
         Width           =   1095
      End
      Begin VB.TextBox distval 
         Height          =   495
         Left            =   2160
         TabIndex        =   4
         Text            =   "10"
         Top             =   600
         Width           =   1575
      End
      Begin VB.OptionButton rearopt 
         Caption         =   "Rear"
         Height          =   375
         Left            =   2160
         TabIndex        =   3
         Top             =   1080
         Width           =   1455
      End
      Begin VB.OptionButton frontopt 
         Caption         =   "Front"
         Height          =   195
         Left            =   240
         TabIndex        =   2
         Top             =   1200
         Value           =   -1  'True
         Width           =   1455
      End
      Begin VB.TextBox speedval 
         BeginProperty DataFormat 
            Type            =   0
            Format          =   "0"
            HaveTrueFalseNull=   0
            FirstDayOfWeek  =   0
            FirstWeekOfYear =   0
            LCID            =   1032
            SubFormatType   =   0
         EndProperty
         Height          =   495
         Left            =   240
         TabIndex        =   1
         Text            =   "128"
         Top             =   600
         Width           =   1575
      End
      Begin VB.Label Label2 
         Caption         =   "Distance (0-255)"
         Height          =   255
         Left            =   2160
         TabIndex        =   9
         Top             =   240
         Width           =   1575
      End
      Begin VB.Label Label1 
         Caption         =   "Speed (0-255)"
         Height          =   375
         Left            =   240
         TabIndex        =   8
         Top             =   240
         Width           =   1215
      End
   End
   Begin MSCommLib.MSComm MSComm1 
      Left            =   8040
      Top             =   4080
      _ExtentX        =   1005
      _ExtentY        =   1005
      _Version        =   393216
      CommPort        =   4
      DTREnable       =   -1  'True
   End
End
Attribute VB_Name = "Intelligent_car_form"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False


Private Const front = "2"
Private Const rear = "1"
Private Const call_turn = "1"
Private Const call_move_distance = "2"
Private Const call_move_hut = "3"
Private Const call_start_hd_adc = "4"
Private Const call_init_hut = "5"
Private Const call_start_fast_adc = "6"
Private Const call_start_moving = "7"
Private Const call_stop_moving = "8"

Private Sub SendNumber(ByVal n As String)
MSComm1.Output = Chr(Val(n))
End Sub

Private Sub Refresh_adc()
    Dim i, length As Integer
    Dim buffer As String
    length = MSComm1.InBufferCount
    buffer = MSComm1.Input
        
    Text1.Text = ""
        
    For i = 1 To length
        Text1.Text = Text1.Text & Str(Asc(Right(Left(buffer, i), 1))) & vbNewLine
    Next i
    
End Sub

Private Sub Connectbt_Click()
If Val(PortNotxt.Text) <> 0 Then
    On Error GoTo errorhandle
    MSComm1.CommPort = Val(PortNotxt.Text)
    MSComm1.PortOpen = True
    Call enable_buttons
Else
    MsgBox ("Please give a valid port number.")
End If

Exit Sub
errorhandle:
MsgBox (Error(Err))
End Sub
Private Sub enable_buttons()
    Disconnect_bt.Enabled = True
    Connectbt.Enabled = False
    Acceleration_frame.Enabled = True
    Steering_frame.Enabled = True
    Hut_frame.Enabled = True
    ADC_frame.Enabled = True
End Sub
Private Sub disable_buttons()
    Disconnect_bt.Enabled = False
    Connectbt.Enabled = True
    Acceleration_frame.Enabled = False
    Steering_frame.Enabled = False
    Hut_frame.Enabled = False
    ADC_frame.Enabled = False
End Sub
Private Sub Disconnect_bt_Click()
If MSComm1.PortOpen = True Then
    MSComm1.PortOpen = False
    Call disable_buttons
End If
End Sub

Private Sub Form_Load()
Call disable_buttons
End Sub

Private Sub Inithutbt_Click()
    Call SendNumber(call_init_hut)
End Sub

Private Sub movedistance_Click()
If Val(distval.Text) < 256 And Val(distval.Text) > 0 _
Or distval.Text = "0" Then
    Call SendNumber(call_move_distance)
    Call SendNumber(speedval.Text)
    Call SendNumber(distval.Text)
    If frontopt = True Then
        Call SendNumber(front)
    Else
        Call SendNumber(rear)
    End If
Else
    MsgBox ("Please give a number (0-255)!")
End If
    
End Sub

Private Sub Movehutbt_Click()
If Val(Angletxt.Text) < 49 And Val(Angletxt.Text) > 0 _
Or Angletxt.Text = "0" Then
    Call SendNumber(call_move_hut)
    Call SendNumber(Angletxt.Text)
Else
    MsgBox ("Please give a number (0-48)!")
End If
End Sub



Private Sub Refresh_adc_bt_Click()
    Call Refresh_adc
End Sub

Private Sub Startmoving_Click()
    If Val(speedval.Text) < 256 And Val(speedval.Text) > 0 _
    Or speedval.Text = "0" Then
        Call SendNumber(call_start_moving)
        Call SendNumber(speedval.Text)
        If frontopt = True Then
            Call SendNumber(front)
        Else
            Call SendNumber(rear)
        End If
    Else
        MsgBox ("Please give a number (0-255)!")
    End If
End Sub

Private Sub Stopmoving_Click()
    Call SendNumber(call_stop_moving)
End Sub

Private Sub Turnbt_Click()
If Val(steeringtxt.Text) < 251 And Val(steeringtxt.Text) > 4 Then
    Call SendNumber(call_turn)
    Call SendNumber(steeringtxt.Text)
Else
    MsgBox ("Please give a number (5-250)!")
End If
    
End Sub
