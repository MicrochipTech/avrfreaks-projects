VERSION 5.00
Object = "{648A5603-2C6E-101B-82B6-000000000014}#1.1#0"; "MSCOMM32.OCX"
Begin VB.Form frmMouseMobile 
   Caption         =   "MouseMobile"
   ClientHeight    =   5532
   ClientLeft      =   48
   ClientTop       =   336
   ClientWidth     =   9672
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   ScaleHeight     =   5532
   ScaleWidth      =   9672
   StartUpPosition =   3  'Windows Default
   Begin VB.Timer tmrSample 
      Enabled         =   0   'False
      Interval        =   200
      Left            =   2940
      Top             =   3240
   End
   Begin VB.CommandButton cmdClear 
      Caption         =   "Clear"
      Enabled         =   0   'False
      Height          =   372
      Left            =   2520
      TabIndex        =   27
      Top             =   4380
      Width           =   1032
   End
   Begin VB.CommandButton cmdMidpt 
      Caption         =   "Find Midpoint"
      Enabled         =   0   'False
      Height          =   312
      Left            =   2940
      TabIndex        =   26
      Top             =   1320
      Width           =   1212
   End
   Begin VB.CommandButton cmdStart 
      Caption         =   "New curve"
      Enabled         =   0   'False
      Height          =   312
      Left            =   2940
      TabIndex        =   25
      Top             =   960
      Width           =   1212
   End
   Begin VB.OptionButton optWhat 
      Caption         =   "Trace Curve"
      Height          =   252
      Index           =   1
      Left            =   2520
      TabIndex        =   24
      Top             =   660
      Width           =   1572
   End
   Begin VB.OptionButton optWhat 
      Caption         =   "Track Movement"
      Height          =   252
      Index           =   0
      Left            =   2520
      TabIndex        =   23
      Top             =   360
      Width           =   1572
   End
   Begin VB.Frame frSts 
      Caption         =   " Statistics "
      Height          =   3192
      Left            =   360
      TabIndex        =   8
      Top             =   2040
      Width           =   1872
      Begin VB.TextBox txtVal 
         Alignment       =   1  'Right Justify
         Height          =   264
         Index           =   5
         Left            =   780
         Locked          =   -1  'True
         TabIndex        =   22
         Top             =   2700
         Width           =   732
      End
      Begin VB.TextBox txtVal 
         Alignment       =   1  'Right Justify
         Height          =   264
         Index           =   4
         Left            =   780
         Locked          =   -1  'True
         TabIndex        =   19
         Top             =   2340
         Width           =   732
      End
      Begin VB.TextBox txtVal 
         Alignment       =   1  'Right Justify
         Height          =   288
         Index           =   3
         Left            =   780
         Locked          =   -1  'True
         TabIndex        =   18
         Top             =   1980
         Width           =   732
      End
      Begin VB.TextBox txtVal 
         Alignment       =   1  'Right Justify
         Height          =   264
         Index           =   2
         Left            =   780
         Locked          =   -1  'True
         TabIndex        =   16
         Top             =   1320
         Width           =   732
      End
      Begin VB.TextBox txtVal 
         Alignment       =   1  'Right Justify
         Height          =   264
         Index           =   1
         Left            =   780
         Locked          =   -1  'True
         TabIndex        =   13
         Top             =   960
         Width           =   732
      End
      Begin VB.TextBox txtVal 
         Alignment       =   1  'Right Justify
         Height          =   288
         Index           =   0
         Left            =   780
         Locked          =   -1  'True
         TabIndex        =   12
         Top             =   600
         Width           =   732
      End
      Begin VB.Label lbl 
         Alignment       =   1  'Right Justify
         Caption         =   "Errs:"
         Height          =   192
         Index           =   9
         Left            =   360
         TabIndex        =   21
         Top             =   2736
         Width           =   372
      End
      Begin VB.Label lbl 
         Alignment       =   1  'Right Justify
         Caption         =   "Y:"
         Height          =   192
         Index           =   8
         Left            =   300
         TabIndex        =   20
         Top             =   2376
         Width           =   372
      End
      Begin VB.Label lbl 
         Alignment       =   1  'Right Justify
         Caption         =   "X:"
         Height          =   192
         Index           =   7
         Left            =   300
         TabIndex        =   17
         Top             =   2028
         Width           =   372
      End
      Begin VB.Label lbl 
         Alignment       =   1  'Right Justify
         Caption         =   "Errs:"
         Height          =   192
         Index           =   6
         Left            =   360
         TabIndex        =   15
         Top             =   1356
         Width           =   372
      End
      Begin VB.Label lbl 
         Alignment       =   1  'Right Justify
         Caption         =   "Y:"
         Height          =   192
         Index           =   5
         Left            =   300
         TabIndex        =   14
         Top             =   996
         Width           =   372
      End
      Begin VB.Label lbl 
         Alignment       =   1  'Right Justify
         Caption         =   "X:"
         Height          =   192
         Index           =   4
         Left            =   300
         TabIndex        =   11
         Top             =   648
         Width           =   372
      End
      Begin VB.Label lbl 
         Caption         =   "Rear Mouse:"
         Height          =   192
         Index           =   3
         Left            =   180
         TabIndex        =   10
         Top             =   1680
         Width           =   1032
      End
      Begin VB.Label lbl 
         Caption         =   "Front Mouse:"
         Height          =   192
         Index           =   2
         Left            =   240
         TabIndex        =   9
         Top             =   300
         Width           =   1032
      End
   End
   Begin VB.Frame frWho 
      Caption         =   " Select Mice "
      Height          =   1632
      Left            =   360
      TabIndex        =   2
      Top             =   240
      Width           =   1872
      Begin VB.ComboBox cmbWho 
         Height          =   288
         Index           =   1
         ItemData        =   "frmMouseMobile.frx":0000
         Left            =   840
         List            =   "frmMouseMobile.frx":000D
         Style           =   2  'Dropdown List
         TabIndex        =   7
         Top             =   660
         Width           =   672
      End
      Begin VB.CommandButton cmdInit 
         Caption         =   "Initialize Mice"
         Height          =   312
         Left            =   300
         TabIndex        =   5
         Top             =   1080
         Width           =   1212
      End
      Begin VB.ComboBox cmbWho 
         Height          =   288
         Index           =   0
         ItemData        =   "frmMouseMobile.frx":001A
         Left            =   840
         List            =   "frmMouseMobile.frx":0027
         Style           =   2  'Dropdown List
         TabIndex        =   4
         Top             =   300
         Width           =   672
      End
      Begin VB.Label lbl 
         Alignment       =   1  'Right Justify
         Caption         =   "Rear:"
         Height          =   192
         Index           =   1
         Left            =   180
         TabIndex        =   6
         Top             =   708
         Width           =   552
      End
      Begin VB.Label lbl 
         Alignment       =   1  'Right Justify
         Caption         =   "Front:"
         Height          =   192
         Index           =   0
         Left            =   180
         TabIndex        =   3
         Top             =   348
         Width           =   552
      End
   End
   Begin VB.PictureBox picLoc 
      AutoRedraw      =   -1  'True
      BackColor       =   &H00FFFFFF&
      Height          =   4932
      Left            =   4380
      ScaleHeight     =   -4000
      ScaleLeft       =   -2000
      ScaleMode       =   0  'User
      ScaleTop        =   2000
      ScaleWidth      =   4000
      TabIndex        =   1
      Top             =   300
      Width           =   4932
   End
   Begin VB.CommandButton cmdQuit 
      Caption         =   "Quit"
      Height          =   372
      Left            =   2520
      TabIndex        =   0
      Top             =   4860
      Width           =   1032
   End
   Begin MSCommLib.MSComm Comm1 
      Left            =   2400
      Top             =   3120
      _ExtentX        =   804
      _ExtentY        =   804
      _Version        =   393216
      DTREnable       =   -1  'True
      BaudRate        =   19200
   End
   Begin VB.Label lblFind 
      Height          =   252
      Left            =   2940
      TabIndex        =   29
      Top             =   1740
      Width           =   1152
   End
   Begin VB.Label lblBtn 
      Caption         =   "*"
      Height          =   252
      Left            =   2580
      TabIndex        =   28
      Top             =   3840
      Width           =   1152
   End
End
Attribute VB_Name = "frmMouseMobile"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Const PMAX = 4000
Private Type points
  xf1 As Long
  yf1 As Long
  xr1 As Long
  yr1 As Long
  x As Long
  y As Long
  din As Single
End Type

Dim holding As Boolean, btnsts As Boolean
Dim fm$, rm$, ertn As Boolean
Dim fronterrs As Long, rearerrs As Long
Dim xf As Long, yf As Long, xr As Long, yr As Long
Dim npts As Integer, endless As Boolean
Dim pts(PMAX) As points

Sub DrawIt(n As Integer)

  picLoc.Line (pts(n - 1).x * 0.2, pts(n - 1).y * 0.2)- _
    (pts(n).x * 0.2, pts(n).y * 0.2)

End Sub

Sub CalcPoint(n As Integer)
  Dim ay As Long, cy As Long, xp As Long, yp As Long
  
  With pts(n)
    ay = .yr1 - 2300
    cy = .yf1 + 2300
    xp = 41 * (.xr1 - .xf1) / 46
    yp = 41 * (cy - ay) / 46
    .x = .xf1 - xp
    .y = cy + yp
  End With

End Sub

Private Sub cmdMidpt_Click()
  Dim n As Integer, tot As Single, d As Single, a$
  Dim i As Integer, half As Single, tx As Long, ty As Long
  
  tot = 0
  For i = 2 To npts
    d = Sqr((pts(i).x - pts(i - 1).x) ^ 2 + (pts(i).y - pts(i - 1).y) ^ 2)
    tot = tot + d
    pts(i).din = tot
    Next i
    
  half = tot / 2
  For i = 2 To npts
    If pts(i).din > half Then Exit For
    Next i
    
  tx = pts(i).x
  ty = pts(i).y
  
  picLoc.Line ((tx - 30) * 0.2, (ty - 30) * 0.2)- _
    ((tx + 30) * 0.2, (ty + 30) * 0.2), , BF
    
  endless = True
  Do While endless
    DoEvents
    With pts(PMAX)
      .xr1 = xr
      .yr1 = yr
      .xf1 = xf
      .yf1 = yf
      Call CalcPoint(PMAX)
      a$ = ""
      If Abs(.x - tx) < 20 Then
        a$ = "on X, "
      Else
        If .x < tx Then
          a$ = "Right, "
        Else
          a$ = "Left, "
        End If
      End If
      If Abs(.y - ty) < 20 Then
        a$ = a$ & "on Y"
      Else
        If .y < ty Then
          a$ = a$ & "Up"
        Else
          a$ = a$ & "Down"
        End If
      End If
    End With
    If endless Then lblFind.Caption = a$
  Loop
    
End Sub

Private Sub cmdStart_Click()
  Dim a$, chg As Boolean
  
  endless = False
  lblFind.Caption = ""
  Call cmdClear_Click
  DoEvents
  
  a$ = "Move the pointer to the" & vbLf & _
       "start of the curve and click" & vbLf & _
        "and hold while tracing"
  MsgBox a$, 48, "Trace curve"
  
  Do While Not btnsts
    DoEvents
    Loop
    
  With pts(1)
    .xf1 = xf
    .yf1 = yf
    .xr1 = xr
    .yr1 = yr
  End With
  npts = 1
  Call CalcPoint(npts)
  
  Do While btnsts
    DoEvents
    With pts(npts)
      If xf <> .xf1 Or yf <> .yf1 Or xr <> .xr1 Or yr <> .yr1 Then
        chg = True
      Else
        chg = False
      End If
    End With
    If chg Then
      If npts < PMAX Then npts = npts + 1
      With pts(npts)
        .xr1 = xr
        .yr1 = yr
        .xf1 = xf
        .yf1 = yf
      End With
      Call CalcPoint(npts)
      Call DrawIt(npts)
    End If
    Loop
    
  If npts > 1 Then cmdMidpt.Enabled = True

End Sub

Sub Steering()
  Dim yyr As Long, sf As Single
  
  yyr = yr - 4600
  sf = 0.2
  
  picLoc.Line (xf * sf, yf * sf)-(xr * sf, yyr * sf)

End Sub

Private Sub cmdClear_Click()
  Dim b$
  
  holding = True        ' don't update position right now
  picLoc.Cls
  
  Comm1.Output = fm$
  Call WaitAck(b$)
  Comm1.Output = "S"
  Call WaitAck(b$)
  Comm1.Output = rm$
  Call WaitAck(b$)
  Comm1.Output = "S"
  Call WaitAck(b$)
  holding = False
  
End Sub

Private Sub cmdInit_Click()
  Dim i As Integer, ecnt As Integer, b$, btn As Integer
  
  i = cmbWho(0).ListIndex
  fm$ = Trim$(cmbWho(0).List(i))
  i = cmbWho(1).ListIndex
  rm$ = Trim$(cmbWho(1).List(i))
  
  If fm$ = rm$ Then
    MsgBox "You must select different mice", 48, "Init"
    Exit Sub
  End If
  
  cmdInit.Enabled = False
  cmdClear.Enabled = True
  optWhat(0).Value = True
  
  Comm1.CommPort = 1
  Comm1.Settings = "19200,N,8,2"
  Comm1.InputLen = 0
  Comm1.PortOpen = True

  ertn = False
  picLoc.CurrentX = -1900
  picLoc.CurrentY = 1900
  picLoc.Print "Initializing front mouse:"
  Comm1.Output = fm$
  Call WaitAck(b$)
  Comm1.Output = "I"
  Call WaitAck(b$)
  If ertn Then
    ertn = False
    Comm1.Output = "I"
    Call WaitAck(b$)
  End If
  
  If Not ertn Then
    picLoc.CurrentX = -1900
    picLoc.CurrentY = 1750
    picLoc.Print "Initializing rear mouse"
    Comm1.Output = rm$
    Call WaitAck(b$)
    Comm1.Output = "I"
    Call WaitAck(b$)
    If ertn Then
      ertn = False
      Comm1.Output = "I"
      Call WaitAck(b$)
    End If
    picLoc.Cls
  End If
  
  If ertn Then
    MsgBox "Init failure - cannot continue", 48, "Errors"
    Comm1.PortOpen = False
    Exit Sub
  End If
  
  optWhat(0).SetFocus
  tmrSample.Enabled = True
  
End Sub

Function conv(b$) As Long
' convert compressed hex to decimal
  Dim v As Long, lng As Integer, c$, j As Integer
  
  j = InStr(b$, "-")
  If j = 0 Then
    conv = xVal(b$)
    Exit Function
  End If

  c$ = Mid$(b$, j + 1)
  lng = Len(c$)
  conv = xVal(c$) - 16 ^ lng
    
End Function

Function xVal(a$)
' this is a hex to decimal routine that doesn't freak out with
' four digit negative hex numbers like val does
' (try val("&hffff") and val("&h10000"))
' we're assuming you gave us a good hex number
  Dim rtn As Long, i As Integer, n As Integer
  
  n = Len(a$)
  rtn = 0
  
  If n > 0 Then
    For i = 1 To n
      rtn = rtn * 16 + Val("&h" & Mid$(a$, i, 1))
      Next i
  End If
  xVal = rtn

End Function

Sub WaitAck(buf$)
  Dim j As Integer, a$
  
  buf$ = ""
  j = 0
  Do
    DoEvents
    If Comm1.InBufferCount > 0 Then
      a$ = Comm1.Input
      buf$ = buf$ & a$
      j = InStr(buf$, "!")
      If j = 0 Then j = InStr(buf$, "?")
    End If
  Loop Until j > 0
  
  If Mid$(buf$, j, 1) = "?" Then
    MsgBox "Nak received from interface", 48, "Errors"
    ertn = True
  End If
  buf$ = Left$(buf$, j - 1)
 
End Sub

Private Sub cmdQuit_Click()

  tmrSample.Enabled = False
  On Error Resume Next
  Comm1.PortOpen = False
  Unload Me

End Sub

Private Sub Form_Load()
  Me.Left = (Screen.Width - Me.Width) / 2
  Me.Top = (Screen.Height - Me.Height) / 2
  cmbWho(0).ListIndex = 0
  cmbWho(1).ListIndex = 1
  fronterrs = 0
  rearerrs = 0
  
  lblBtn.Caption = "Button Off"
  holding = False
  btnsts = False
  
End Sub

Private Sub optWhat_Click(Index As Integer)
  Dim e As Integer
  
  endless = False
  e = Index > 0
  cmdStart.Enabled = e
  cmdMidpt.Enabled = False
  lblFind.Caption = ""

  
End Sub

Private Sub tmrSample_Timer()
  Dim b$, ecnt As Integer, btn As Integer
  
  If holding Then Exit Sub

' read the rear mouse
  Comm1.Output = rm$
  Call WaitAck(b$)
  Comm1.Output = "X"
  DoEvents
  Call WaitAck(b$)
  xr = conv(b$)

  txtVal(3).Text = xr
  Comm1.Output = "Y"
  Call WaitAck(b$)
  yr = conv(b$)
  txtVal(4).Text = yr
  Comm1.Output = "*"
  Call WaitAck(b$)
  ecnt = Val("&h" & b$) And 63
  rearerrs = rearerrs + ecnt
  txtVal(5).Text = Str$(rearerrs)

' read the front mouse
  Comm1.Output = fm$
  Call WaitAck(b$)
  Comm1.Output = "X"
  DoEvents
  Call WaitAck(b$)
  xf = conv(b$)
  txtVal(0).Text = xf
  Comm1.Output = "Y"
  Call WaitAck(b$)
  yf = conv(b$)
  txtVal(1).Text = yf
  Comm1.Output = "*"
  Call WaitAck(b$)
  ecnt = Val("&h" & b$) And 63
  fronterrs = fronterrs + ecnt
  txtVal(2).Text = Str$(fronterrs)
  Comm1.Output = "B"
  Call WaitAck(b$)
  btn = Val("&h" & b$) And 4
  If btn = 0 Then
    lblBtn.Caption = "Button Off"
    btnsts = False
  Else
    lblBtn.Caption = "Button On"
    btnsts = True
    If optWhat(0).Value Then
      Call cmdClear_Click
      Exit Sub
    End If
  End If
    
  If optWhat(0).Value Then
    Call Steering
  Else
  End If

End Sub
