VERSION 5.00
Object = "{248DD890-BB45-11CF-9ABC-0080C7E7B78D}#1.0#0"; "MSWINSCK.OCX"
Begin VB.Form Form1 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "TCPSimDBG - Example"
   ClientHeight    =   4440
   ClientLeft      =   45
   ClientTop       =   345
   ClientWidth     =   7665
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4440
   ScaleWidth      =   7665
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.Frame Frame1 
      Caption         =   "Register Example (Read/Write)"
      Height          =   1095
      Left            =   4920
      TabIndex        =   5
      Top             =   2640
      Width           =   2655
      Begin VB.PictureBox REG 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         ForeColor       =   &H80000008&
         Height          =   255
         Index           =   0
         Left            =   1800
         ScaleHeight     =   225
         ScaleWidth      =   225
         TabIndex        =   14
         Top             =   720
         Width           =   255
      End
      Begin VB.PictureBox REG 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         ForeColor       =   &H80000008&
         Height          =   255
         Index           =   1
         Left            =   1560
         ScaleHeight     =   225
         ScaleWidth      =   225
         TabIndex        =   13
         Top             =   720
         Width           =   255
      End
      Begin VB.PictureBox REG 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         ForeColor       =   &H80000008&
         Height          =   255
         Index           =   2
         Left            =   1320
         ScaleHeight     =   225
         ScaleWidth      =   225
         TabIndex        =   12
         Top             =   720
         Width           =   255
      End
      Begin VB.PictureBox REG 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         ForeColor       =   &H80000008&
         Height          =   255
         Index           =   3
         Left            =   1080
         ScaleHeight     =   225
         ScaleWidth      =   225
         TabIndex        =   11
         Top             =   720
         Width           =   255
      End
      Begin VB.PictureBox REG 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         ForeColor       =   &H80000008&
         Height          =   255
         Index           =   4
         Left            =   840
         ScaleHeight     =   225
         ScaleWidth      =   225
         TabIndex        =   10
         Top             =   720
         Width           =   255
      End
      Begin VB.PictureBox REG 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         ForeColor       =   &H80000008&
         Height          =   255
         Index           =   5
         Left            =   600
         ScaleHeight     =   225
         ScaleWidth      =   225
         TabIndex        =   9
         Top             =   720
         Width           =   255
      End
      Begin VB.PictureBox REG 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         ForeColor       =   &H80000008&
         Height          =   255
         Index           =   6
         Left            =   360
         ScaleHeight     =   225
         ScaleWidth      =   225
         TabIndex        =   8
         Top             =   720
         Width           =   255
      End
      Begin VB.PictureBox REG 
         Appearance      =   0  'Flat
         BackColor       =   &H80000005&
         ForeColor       =   &H80000008&
         Height          =   255
         Index           =   7
         Left            =   120
         ScaleHeight     =   225
         ScaleWidth      =   225
         TabIndex        =   7
         Top             =   720
         Width           =   255
      End
      Begin VB.TextBox Text3 
         Height          =   285
         Left            =   1560
         TabIndex        =   6
         Text            =   "3B"
         Top             =   360
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "SRAM Addr(HEX) :"
         Height          =   255
         Left            =   120
         TabIndex        =   15
         Top             =   360
         Width           =   1455
      End
   End
   Begin VB.CommandButton Command2 
      Caption         =   "START auto Req/Get"
      Height          =   615
      Left            =   5040
      TabIndex        =   3
      Top             =   1800
      Width           =   2415
   End
   Begin VB.Timer Timer1 
      Enabled         =   0   'False
      Interval        =   50
      Left            =   4080
      Top             =   480
   End
   Begin VB.TextBox Text2 
      Height          =   285
      Left            =   120
      TabIndex        =   2
      Top             =   3960
      Width           =   3975
   End
   Begin VB.CommandButton Command1 
      Caption         =   "Send"
      Height          =   255
      Left            =   4080
      TabIndex        =   0
      Top             =   3960
      Width           =   735
   End
   Begin MSWinsockLib.Winsock Winsock1 
      Left            =   3600
      Top             =   480
      _ExtentX        =   741
      _ExtentY        =   741
      _Version        =   393216
   End
   Begin VB.TextBox Text1 
      Height          =   3375
      Left            =   120
      MultiLine       =   -1  'True
      ScrollBars      =   2  'Vertical
      TabIndex        =   1
      Top             =   360
      Width           =   4695
   End
   Begin VB.Frame Frame2 
      Caption         =   "AVR Studio Debug"
      Height          =   1215
      Left            =   4920
      TabIndex        =   16
      Top             =   360
      Width           =   2655
      Begin VB.CommandButton Command3 
         Caption         =   "Start Debug"
         Height          =   375
         Left            =   120
         TabIndex        =   19
         Top             =   240
         Width           =   1095
      End
      Begin VB.CommandButton Command4 
         Caption         =   "Stop Debug"
         Height          =   375
         Left            =   1320
         TabIndex        =   18
         Top             =   240
         Width           =   1095
      End
      Begin VB.CommandButton Command5 
         Caption         =   "Single Step"
         Height          =   375
         Left            =   120
         TabIndex        =   17
         Top             =   720
         Width           =   1095
      End
   End
   Begin VB.Label Label2 
      AutoSize        =   -1  'True
      Caption         =   "Received Data from AVR Studio:"
      Height          =   195
      Left            =   120
      TabIndex        =   4
      Top             =   120
      Width           =   2340
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Function GetGUIval(Regis As Object) As Byte
Dim res, i As Integer
res = 0
For i = 0 To 7
    res = res + ((Regis(i).BackColor And &HFF00&) / &HFF00&) * (2 ^ i)
Next i
GetGUIval = res
End Function

Sub SetGUIval(Regis As Object, valtoset As Byte)
Dim i As Integer

For i = 0 To 7
    If (valtoset And (2 ^ i)) <> 0 Then
        Regis(i).BackColor = &HFF00&
    Else
        Regis(i).BackColor = &H80000005
    End If
Next i
End Sub

Private Sub Command1_Click()
Dim i As Integer, str As String, tokens() As String

If Winsock1.State = sckConnected Then
    tokens = Split(Text2.Text, " ")
    For i = 0 To UBound(tokens())
        str = str & Chr(Val("&H" & tokens(i) & "&"))
    Next i
    Winsock1.SendData str
End If


End Sub


Private Sub Command2_Click()

If Timer1.Enabled = True Then
    Text3.Enabled = True
    Timer1.Enabled = False
    Command2.Caption = "START auto Req/Get"
Else
    Text3.Enabled = False
    Timer1.Enabled = True
    Command2.Caption = "STOP auto Req/Get"
End If
End Sub

Private Sub Command3_Click()
If Winsock1.State = sckConnected Then
    Winsock1.SendData Chr(&H31)
End If
End Sub

Private Sub Command4_Click()
If Winsock1.State = sckConnected Then
    Winsock1.SendData Chr(&H32)
End If
End Sub

Private Sub Command5_Click()
If Winsock1.State = sckConnected Then
    Winsock1.SendData Chr(&HE)
End If
End Sub

Private Sub Form_Load()
Winsock1.Close
Winsock1.LocalPort = 333
Winsock1.Listen
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Winsock1.Close
End Sub

Private Sub REG_Click(Index As Integer)
If Winsock1.State = sckConnected Then
    If REG(Index).BackColor = &H80000005 Then
        REG(Index).BackColor = &HFF00&
    Else
        REG(Index).BackColor = &H80000005
    End If
    
    Winsock1.SendData Chr(&H3) & Chr(0) & Chr(0) & Chr(0) & Chr(Val("&h" & Text3.Text)) & Chr(GetGUIval(REG))
End If
End Sub

Private Sub Timer1_Timer()
If Winsock1.State = sckConnected Then
    Winsock1.SendData Chr(&H4) & Chr(0) & Chr(0) & Chr(0) & Chr(Val("&h" & Text3.Text))
End If
End Sub

Private Sub Winsock1_ConnectionRequest(ByVal requestID As Long)
Winsock1.Close
Winsock1.Accept (requestID)
Text1.Text = Text1.Text & "Connection request: " & requestID & vbCrLf

End Sub

Private Sub Winsock1_DataArrival(ByVal bytesTotal As Long)
Dim buf As String
Winsock1.GetData buf
Text1.Text = Text1.Text & Val(buf) & " (Received length: " & bytesTotal & " )" & vbCrLf
SetGUIval REG, Asc(buf)
End Sub
