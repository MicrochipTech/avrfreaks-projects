VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Begin VB.Form GFSM 
   BackColor       =   &H00000000&
   Caption         =   "GENERIC FSM"
   ClientHeight    =   3195
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   4680
   ForeColor       =   &H00000000&
   ScaleHeight     =   8595
   ScaleWidth      =   11880
   WindowState     =   2  'Maximized
   Begin VB.PictureBox Picture1 
      Height          =   5415
      Left            =   480
      Picture         =   "GFSM.frx":0000
      ScaleHeight     =   5355
      ScaleWidth      =   3915
      TabIndex        =   9
      Top             =   2880
      Width           =   3975
   End
   Begin VB.CommandButton newprog 
      Caption         =   "PROGRAM"
      Height          =   495
      Left            =   9000
      TabIndex        =   8
      Top             =   3480
      Width           =   1215
   End
   Begin VB.TextBox Text1 
      Height          =   345
      Left            =   10320
      TabIndex        =   7
      Text            =   "S0"
      ToolTipText     =   "ENTER THE STATE NO. IN BETWEEN 0 AND 15"
      Top             =   7200
      Visible         =   0   'False
      Width           =   375
   End
   Begin MSComDlg.CommonDialog CommonDialog1 
      Left            =   240
      Top             =   960
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.CommandButton Command1 
      Caption         =   "EXIT"
      Height          =   495
      Left            =   9960
      TabIndex        =   5
      Top             =   5040
      Width           =   1215
   End
   Begin VB.CommandButton TABX 
      BackColor       =   &H00FFC0FF&
      Caption         =   "TABLES"
      Height          =   495
      Left            =   9000
      TabIndex        =   3
      Top             =   3480
      Visible         =   0   'False
      Width           =   1215
   End
   Begin VB.CommandButton HELP 
      BackColor       =   &H000000FF&
      Caption         =   "HELP"
      Height          =   495
      Left            =   8040
      TabIndex        =   1
      Top             =   5040
      Width           =   1215
   End
   Begin VB.Frame FHELP 
      BackColor       =   &H00FFC0C0&
      Caption         =   "SOME USEFUL HELP"
      Height          =   3615
      Left            =   4440
      TabIndex        =   0
      Top             =   2880
      Width           =   2775
      Begin VB.Label LHELP 
         BackColor       =   &H00FFC0C0&
         Caption         =   $"GFSM.frx":61B0
         ForeColor       =   &H000000FF&
         Height          =   3075
         Left            =   120
         TabIndex        =   2
         Top             =   360
         Width           =   2505
         WordWrap        =   -1  'True
      End
   End
   Begin VB.Label DEFSTATE 
      AutoSize        =   -1  'True
      BackColor       =   &H00000000&
      Caption         =   "PLEASE ENTER THE INITIAL STATE OF THE FSM"
      ForeColor       =   &H00FFFFFF&
      Height          =   195
      Left            =   6480
      TabIndex        =   6
      Top             =   7320
      Visible         =   0   'False
      Width           =   3765
   End
   Begin VB.Label Label1 
      BackColor       =   &H00000000&
      Caption         =   $"GFSM.frx":62EA
      BeginProperty Font 
         Name            =   "Arial Black"
         Size            =   21.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   1875
      Left            =   120
      TabIndex        =   4
      Top             =   840
      Width           =   11865
   End
End
Attribute VB_Name = "GFSM"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Command1_Click()
TABLE.MSComm1.Output = "q"
End
End Sub

Private Sub HELP_Click()
HEL.Show
End Sub

Private Sub newprog_Click()
DEFSTATE.Visible = True
Text1.Visible = True
TABX.Visible = True
newprog.Visible = False
End Sub

Private Sub TABX_Click()
TABLE.Show
End Sub
