VERSION 5.00
Begin VB.Form frmDFTIFT 
   Caption         =   "Discrete Fourier Transform and Inverse Discrete Fourier Transform - by Dartagnan"
   ClientHeight    =   8145
   ClientLeft      =   165
   ClientTop       =   450
   ClientWidth     =   12705
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
   MinButton       =   0   'False
   ScaleHeight     =   543
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   847
   StartUpPosition =   2  'CenterScreen
   WindowState     =   2  'Maximized
   Begin VB.Frame frFASES 
      BorderStyle     =   0  'None
      Height          =   435
      Left            =   9120
      TabIndex        =   12
      Top             =   2490
      Width           =   1425
      Begin VB.Label lbFASES 
         Alignment       =   2  'Center
         BackColor       =   &H00C0FFC0&
         BorderStyle     =   1  'Fixed Single
         BeginProperty Font 
            Name            =   "Courier New"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00000000&
         Height          =   285
         Left            =   120
         TabIndex        =   13
         Top             =   90
         Width           =   1185
      End
   End
   Begin VB.Frame frLEVELS 
      BorderStyle     =   0  'None
      Height          =   435
      Left            =   9150
      TabIndex        =   10
      Top             =   870
      Width           =   1425
      Begin VB.Label lbLEVELS 
         Alignment       =   2  'Center
         BackColor       =   &H00C0FFC0&
         BorderStyle     =   1  'Fixed Single
         BeginProperty Font 
            Name            =   "Courier New"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00000000&
         Height          =   285
         Left            =   120
         TabIndex        =   11
         Top             =   90
         Width           =   1185
      End
   End
   Begin VB.PictureBox picPOWER 
      Appearance      =   0  'Flat
      AutoRedraw      =   -1  'True
      BackColor       =   &H80000005&
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H80000008&
      Height          =   1635
      Left            =   6660
      ScaleHeight     =   107
      ScaleMode       =   3  'Pixel
      ScaleWidth      =   173
      TabIndex        =   18
      Top             =   4680
      Width           =   2625
   End
   Begin VB.PictureBox picBPARMS 
      Appearance      =   0  'Flat
      AutoRedraw      =   -1  'True
      BackColor       =   &H80000005&
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H80000008&
      Height          =   1635
      Left            =   10020
      ScaleHeight     =   107
      ScaleMode       =   3  'Pixel
      ScaleWidth      =   173
      TabIndex        =   15
      Top             =   2280
      Width           =   2625
   End
   Begin VB.PictureBox picAPARMS 
      Appearance      =   0  'Flat
      AutoRedraw      =   -1  'True
      BackColor       =   &H80000005&
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H80000008&
      Height          =   1635
      Left            =   10020
      ScaleHeight     =   107
      ScaleMode       =   3  'Pixel
      ScaleWidth      =   173
      TabIndex        =   14
      Top             =   510
      Width           =   2625
   End
   Begin VB.Timer Timer 
      Interval        =   100
      Left            =   10770
      Top             =   1620
   End
   Begin VB.Frame frINPUTFASES 
      BackColor       =   &H00E0E0E0&
      Caption         =   "Input Function Fases"
      Height          =   1455
      Left            =   6510
      TabIndex        =   8
      Top             =   2280
      Width           =   3465
      Begin VB.VScrollBar vsINPUTFASES 
         Height          =   1035
         Index           =   0
         LargeChange     =   9
         Left            =   180
         Max             =   90
         Min             =   -90
         TabIndex        =   9
         Top             =   390
         Width           =   225
      End
   End
   Begin VB.HScrollBar hsDFTPOINTS 
      Height          =   285
      Left            =   7920
      Max             =   10
      Min             =   3
      TabIndex        =   6
      Top             =   240
      Value           =   6
      Width           =   1515
   End
   Begin VB.Frame frINPUTLEVELS 
      BackColor       =   &H00E0E0E0&
      Caption         =   "Input Function Levels"
      Height          =   1455
      Left            =   6510
      TabIndex        =   4
      Top             =   540
      Width           =   3465
      Begin VB.CheckBox ckFREQS 
         Height          =   240
         Index           =   0
         Left            =   150
         TabIndex        =   20
         Top             =   1170
         Width           =   285
      End
      Begin VB.VScrollBar vsINPUTLEVELS 
         Height          =   1035
         Index           =   0
         LargeChange     =   8
         Left            =   150
         Max             =   127
         TabIndex        =   5
         Top             =   120
         Value           =   127
         Width           =   225
      End
   End
   Begin VB.PictureBox picFOUTPUT 
      Appearance      =   0  'Flat
      AutoRedraw      =   -1  'True
      BackColor       =   &H80000005&
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H80000008&
      Height          =   4305
      Left            =   60
      ScaleHeight     =   285
      ScaleMode       =   3  'Pixel
      ScaleWidth      =   411
      TabIndex        =   1
      Top             =   4230
      Width           =   6195
   End
   Begin VB.PictureBox picFINPUT 
      Appearance      =   0  'Flat
      AutoRedraw      =   -1  'True
      BackColor       =   &H80000005&
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H80000008&
      Height          =   4305
      Left            =   60
      ScaleHeight     =   285
      ScaleMode       =   3  'Pixel
      ScaleWidth      =   411
      TabIndex        =   0
      Top             =   -30
      Width           =   6195
   End
   Begin VB.Label lbPOWER 
      BackColor       =   &H00C0FFFF&
      BorderStyle     =   1  'Fixed Single
      Caption         =   "Power Spectrum - 32F"
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00000000&
      Height          =   285
      Left            =   9660
      TabIndex        =   19
      Top             =   5190
      Width           =   2865
   End
   Begin VB.Label lbBPARMS 
      BackColor       =   &H00808000&
      BorderStyle     =   1  'Fixed Single
      Caption         =   "DFT B Parms"
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   285
      Left            =   9960
      TabIndex        =   17
      Top             =   2130
      Width           =   2865
   End
   Begin VB.Label lbAPARMS 
      BackColor       =   &H00808000&
      BorderStyle     =   1  'Fixed Single
      Caption         =   "DFT A Parms"
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   285
      Left            =   9750
      TabIndex        =   16
      Top             =   90
      Width           =   2865
   End
   Begin VB.Label lbDFTPOINTS 
      BackColor       =   &H000000C0&
      BorderStyle     =   1  'Fixed Single
      Caption         =   "DFT SIZE 64"
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   345
      Left            =   6330
      TabIndex        =   7
      Top             =   240
      Width           =   1545
   End
   Begin VB.Label lbFOUTPUT 
      BackColor       =   &H00C00000&
      BorderStyle     =   1  'Fixed Single
      Caption         =   "Output Function (IDFT) "
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   285
      Left            =   6480
      TabIndex        =   3
      Top             =   4050
      Width           =   2865
   End
   Begin VB.Label lbFINPUT 
      BackColor       =   &H00C00000&
      BorderStyle     =   1  'Fixed Single
      Caption         =   "Input Function (IFT)"
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   285
      Left            =   6270
      TabIndex        =   2
      Top             =   0
      Width           =   2865
   End
End
Attribute VB_Name = "frmDFTIFT"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Const DFT_MAX = 1024                            'MY MAX DFT SIZE
Const pi = 3.14159265354
Const MAX_FREQS = 16
Dim FINPUT(0 To DFT_MAX) As Integer             'RESERVE SPACE TO IT
Dim A_PARMS(0 To DFT_MAX) As Integer            'DFT A PARMS
Dim B_PARMS(0 To DFT_MAX) As Integer            'DFT B PARMS
Dim S_TABLE(0 To DFT_MAX) As Single             'DFT SINE TABLE
Dim LB_TIMER As Long
Dim tm As New clsJDAO_TIME

Private Sub ckFREQS_Click(Index As Integer)
    DRAW_GRID
    DEF_AND_DRAW_FINPUT
End Sub

Private Sub Form_Load()
Dim i As Integer
    For i = 1 To MAX_FREQS - 1
        Load vsINPUTLEVELS(i)
        Load vsINPUTFASES(i)
        Load ckFREQS(i)
        vsINPUTLEVELS(i).Visible = True
        vsINPUTFASES(i).Visible = True
        ckFREQS(i).Visible = True
    Next
End Sub

Private Sub Form_Resize()
Dim W As Integer, i As Integer, H As Single
Static ch As Boolean
'+--------------------------------------------
'| RESIZE INPUT AND OUTPUT FUNCTIONS SCREEN,LABELS
'+--------------------------------------------
    lbFINPUT.Left = 0
    lbFINPUT.Top = 0
    lbFINPUT.Width = Me.ScaleWidth / 2
    picFINPUT.Left = 0
    picFINPUT.Top = lbFINPUT.Top + lbFINPUT.Height
    picFINPUT.Width = Me.ScaleWidth / 2
    picFINPUT.Height = Me.ScaleHeight / 2 - lbFINPUT.Height
    lbFOUTPUT.Left = 0
    lbFOUTPUT.Top = picFINPUT.Top + picFINPUT.Height
    lbFOUTPUT.Width = Me.ScaleWidth / 2
    picFOUTPUT.Left = 0
    picFOUTPUT.Top = lbFOUTPUT.Top + lbFOUTPUT.Height
    picFOUTPUT.Width = Me.ScaleWidth / 2
    picFOUTPUT.Height = Me.ScaleHeight / 2 - lbFINPUT.Height
'+-------------------------------------------
'| A & B PARMS PICTURES AND LABELS ADJUST
'+-------------------------------------------
    lbAPARMS.Top = lbFINPUT.Top
    lbAPARMS.Left = lbFINPUT.Left + lbFINPUT.Width
    lbAPARMS.Width = Me.ScaleWidth / 4
    picAPARMS.Top = lbAPARMS.Top + lbAPARMS.Height
    picAPARMS.Left = lbAPARMS.Left
    picAPARMS.Width = lbAPARMS.Width
    picAPARMS.Height = picFINPUT.Height
    lbBPARMS.Top = picAPARMS.Top + picAPARMS.Height
    lbBPARMS.Left = picAPARMS.Left
    lbBPARMS.Width = lbAPARMS.Width
    picBPARMS.Top = lbBPARMS.Top + lbBPARMS.Height
    picBPARMS.Left = lbBPARMS.Left
    picBPARMS.Width = lbBPARMS.Width
    picBPARMS.Height = picAPARMS.Height
'+-------------------------------------------
'| RESIZE SCROLLS BAR FOR DFT POINTS
'+-------------------------------------------
    lbDFTPOINTS.Left = lbAPARMS.Left + lbAPARMS.Width
    lbDFTPOINTS.Top = lbAPARMS.Top
    lbDFTPOINTS.Height = lbAPARMS.Height
    lbDFTPOINTS.Width = lbAPARMS.Width * 2 / 3
    hsDFTPOINTS.Top = lbAPARMS.Top
    hsDFTPOINTS.Left = lbDFTPOINTS.Left + lbDFTPOINTS.Width - 2
    hsDFTPOINTS.Width = lbAPARMS.Width * 1 / 3
'+-------------------------------------------
'| RESIZE SCROLLS BAR FOR INPUT LEVELS
'+-------------------------------------------
    frINPUTLEVELS.Top = lbAPARMS.Top + lbAPARMS.Height
    frINPUTLEVELS.Left = lbAPARMS.Left + lbAPARMS.Width
    frINPUTLEVELS.Width = Me.ScaleWidth / 4
    frINPUTLEVELS.Height = picFINPUT.Height / 2
    vsINPUTLEVELS(0).Width = ((frINPUTLEVELS.Width - 4) / MAX_FREQS) * Screen.TwipsPerPixelX
    vsINPUTLEVELS(0).Left = 2 * Screen.TwipsPerPixelX
    vsINPUTLEVELS(0).Top = TextHeight(lbFINPUT.Caption) * Screen.TwipsPerPixelX
    vsINPUTLEVELS(0).Height = (frINPUTLEVELS.Height - TextHeight(lbFINPUT.Caption) - 2) * Screen.TwipsPerPixelX - ckFREQS(0).Height
    ckFREQS(0).Top = vsINPUTLEVELS(0).Top + vsINPUTLEVELS(0).Height
    ckFREQS(0).Left = vsINPUTLEVELS(0).Left
    ckFREQS(0).Width = vsINPUTLEVELS(0).Width
    For i = 1 To MAX_FREQS - 1
        vsINPUTLEVELS(i).Left = vsINPUTLEVELS(i - 1).Left + vsINPUTLEVELS(i - 1).Width
        vsINPUTLEVELS(i).Top = vsINPUTLEVELS(i - 1).Top
        vsINPUTLEVELS(i).Width = vsINPUTLEVELS(i - 1).Width
        vsINPUTLEVELS(i).Height = vsINPUTLEVELS(i - 1).Height
        ckFREQS(i).Left = ckFREQS(i - 1).Left + ckFREQS(i - 1).Width
        ckFREQS(i).Top = ckFREQS(i - 1).Top
        ckFREQS(i).Width = ckFREQS(i - 1).Width
        ckFREQS(i).Height = ckFREQS(i - 1).Height
    Next
'+-------------------------------------------
'| RESIZE SCROLLS BAR FOR INPUT FASES
'+-------------------------------------------
    frINPUTFASES.Top = frINPUTLEVELS.Top + frINPUTLEVELS.Height
    frINPUTFASES.Left = lbAPARMS.Left + lbAPARMS.Width
    frINPUTFASES.Width = Me.ScaleWidth / 4
    frINPUTFASES.Height = picFINPUT.Height / 2
    vsINPUTFASES(0).Width = ((frINPUTFASES.Width - 4) / MAX_FREQS) * Screen.TwipsPerPixelX
    vsINPUTFASES(0).Left = 2 * Screen.TwipsPerPixelX
    vsINPUTFASES(0).Top = TextHeight(lbFINPUT.Caption) * Screen.TwipsPerPixelX
    vsINPUTFASES(0).Height = (frINPUTFASES.Height - TextHeight(lbFINPUT.Caption) - 2) * Screen.TwipsPerPixelX
    For i = 1 To MAX_FREQS - 1
        vsINPUTFASES(i).Left = vsINPUTFASES(i - 1).Left + vsINPUTFASES(i - 1).Width
        vsINPUTFASES(i).Top = vsINPUTFASES(i - 1).Top
        vsINPUTFASES(i).Width = vsINPUTFASES(i - 1).Width
        vsINPUTFASES(i).Height = vsINPUTFASES(i - 1).Height
    Next
'+-------------------------------------------
'| INDICATION LABELS
'+-------------------------------------------
    frLEVELS.Top = frINPUTLEVELS.Top + frINPUTLEVELS.Height / 2 - frLEVELS.Height / 2
    frLEVELS.Left = frINPUTLEVELS.Left + frINPUTLEVELS.Width / 2 - frLEVELS.Width / 2
    frFASES.Top = frINPUTFASES.Top + frINPUTFASES.Height / 2 - frFASES.Height / 2
    frFASES.Left = frINPUTFASES.Left + frINPUTFASES.Width / 2 - frFASES.Width / 2
'+-------------------------------------------
'| POWER SPECTRUM
'+-------------------------------------------
    lbPOWER.Top = frINPUTFASES.Top + frINPUTFASES.Height
    lbPOWER.Left = frINPUTFASES.Left
    lbPOWER.Width = frINPUTFASES.Width
    picPOWER.Top = lbPOWER.Top + lbPOWER.Height
    picPOWER.Left = lbPOWER.Left
    picPOWER.Width = lbPOWER.Width
    picPOWER.Height = picFOUTPUT.Height
End Sub

Sub DRAW_GRID()
Dim W As Single, i As Integer, H As Single
'+-------------------------------------------
'| DRAW A GRID LINES
'+-------------------------------------------
    picFINPUT.Cls
    W = picFINPUT.ScaleWidth / 10
    H = picFINPUT.ScaleHeight / 10
    For i = 0 To 9
        picFINPUT.Line (i * W, 0)-(i * W, picFINPUT.ScaleHeight), RGB(160, 160, 160)
        picFINPUT.Line (0, i * H)-(picFINPUT.ScaleWidth, i * H), RGB(160, 160, 160)
    Next

    picFOUTPUT.Cls
    W = picFOUTPUT.ScaleWidth / 10
    H = picFOUTPUT.ScaleHeight / 10
    For i = 0 To 9
        picFOUTPUT.Line (i * W, 0)-(i * W, picFOUTPUT.ScaleHeight), RGB(160, 160, 160)
        picFOUTPUT.Line (0, i * H)-(picFOUTPUT.ScaleWidth, i * H), RGB(160, 160, 160)
    Next

    picPOWER.Cls
    picAPARMS.Cls
    picBPARMS.Cls
End Sub

Private Sub hsDFTPOINTS_Change()
    lbDFTPOINTS.Caption = "DFT SIZE" & Str$(2 ^ hsDFTPOINTS.Value)
    DRAW_GRID
    DEF_AND_DRAW_FINPUT
End Sub

Private Sub picFINPUT_Paint()
    DRAW_GRID
    DEF_AND_DRAW_FINPUT
End Sub

Private Sub picFINPUT_Resize()
    picFOUTPUT_Paint
End Sub

Private Sub picFOUTPUT_Paint()
    DRAW_GRID
    DEF_AND_DRAW_FINPUT
End Sub

Private Sub DEF_AND_DRAW_FINPUT()
Dim DFT_SIZE As Integer, i As Integer, j As Integer, f As Integer
Dim W As Single, s As Single, c As Single, s_index As Integer, c_index As Integer
Dim xi As Single, yi As Single, xf As Single, yf As Single, a As Single

    DFT_SIZE = 2 ^ hsDFTPOINTS.Value
    'CALCULATE FUNCTION
    For i = 0 To DFT_SIZE - 1
        s = 0
        For j = 0 To vsINPUTLEVELS.Count - 1
            a = IIf(ckFREQS(j).Value = vbChecked, 1.5, 1)
            W = (i / DFT_SIZE * 2 * pi * (j + a)) + (vsINPUTFASES(j).Value / 180 * pi)
            s = s + (127 - vsINPUTLEVELS(j).Value) * Sin(W)
        Next
        FINPUT(i) = 127 + s
    Next
    'DRAW FUNCTION
    picFINPUT.PSet (0, picFINPUT.ScaleHeight / 2), vbBlack
    For i = 0 To DFT_SIZE - 1
        picFINPUT.Line -(i / (DFT_SIZE - 1) * picFINPUT.ScaleWidth, FINPUT(i) / 256 * picFINPUT.ScaleHeight), vbBlack
    Next
    picFINPUT.Refresh
    'FILL SINE TABLE
    For i = 0 To DFT_SIZE - 1
        W = i / DFT_SIZE * 2 * pi
        S_TABLE(i) = Sin(W)
    Next
    'START DFT CALCULATION
    tm.TIME_CLEAR
    s_index = 0
    c_index = DFT_SIZE / 4
    For f = 1 To DFT_SIZE / 2 - 1
        s = 0
        c = 0
        For j = 0 To DFT_SIZE - 1
            s = s + FINPUT(j) * S_TABLE(s_index)
            c = c + FINPUT(j) * S_TABLE(c_index)
            s_index = s_index + f
            c_index = c_index + f
            If s_index >= DFT_SIZE Then s_index = s_index - DFT_SIZE
            If c_index >= DFT_SIZE Then c_index = c_index - DFT_SIZE
        Next
        A_PARMS(f) = 2 * s / DFT_SIZE
        B_PARMS(f) = 2 * c / DFT_SIZE
    Next
    Caption = tm.TIME_NOW
    'DRAW A PARMS
    For f = 1 To DFT_SIZE / 2 - 1
        xi = (f - 1) / 32 * picAPARMS.ScaleWidth
        yi = picAPARMS.Height / 2
        xf = xi + (0.9 / 32 * picAPARMS.ScaleWidth)
        yf = yi - A_PARMS(f) / 256 * picAPARMS.ScaleHeight
        picAPARMS.Line (xi, yi)-(xf, yf), vbGreen, BF
        picAPARMS.Line (xi, yi)-(xf, yf), vbBlack, B
    Next
    picAPARMS.Refresh
    'DRAW B PARMS
    For f = 1 To DFT_SIZE / 2 - 1
        xi = (f - 1) / 32 * picBPARMS.ScaleWidth
        yi = picBPARMS.Height / 2
        xf = xi + (0.9 / 32 * picBPARMS.ScaleWidth)
        yf = yi - B_PARMS(f) / 256 * picBPARMS.ScaleHeight
        picBPARMS.Line (xi, yi)-(xf, yf), vbGreen, BF
        picBPARMS.Line (xi, yi)-(xf, yf), vbBlack, B
    Next
    picBPARMS.Refresh
   'DRAW POWER SPECTRUM
    For f = 1 To DFT_SIZE / 2 - 1
        xi = (f - 1) / 32 * picPOWER.ScaleWidth             'fixed length 16
        yi = picPOWER.Height - 1
        xf = xi + (0.9 / 32 * picPOWER.ScaleWidth)
        yf = yi - Sqr(A_PARMS(f) ^ 2 + B_PARMS(f) ^ 2) / 128 * picPOWER.ScaleHeight
        picPOWER.Line (xi, yi)-(xf, yf), vbCyan, BF
        picPOWER.Line (xi, yi)-(xf, yf), vbBlack, B
    Next
    'DRAW IFT FUNCTION
    picFOUTPUT.PSet (0, picFOUTPUT.ScaleHeight / 2)
    For j = 0 To DFT_SIZE - 1
        s = 0
        c = 0
        s_index = j
        c_index = j + DFT_SIZE / 4
        For f = 1 To DFT_SIZE / 2 - 1
            If s_index >= DFT_SIZE Then s_index = s_index - DFT_SIZE
            If c_index >= DFT_SIZE Then c_index = c_index - DFT_SIZE
            s = s + A_PARMS(f) * S_TABLE(s_index)
            c = c + B_PARMS(f) * S_TABLE(c_index)
            s_index = s_index + j
            c_index = c_index + j
        Next
        picFOUTPUT.Line -(j / (DFT_SIZE - 1) * picFOUTPUT.ScaleWidth, picFOUTPUT.ScaleHeight / 2 + (s + c) / 256 * picFOUTPUT.ScaleHeight)
    Next
    picPOWER.Refresh
End Sub

Private Sub picFOUTPUT_Resize()
    picFOUTPUT_Paint
End Sub

Private Sub picPOWER_Resize()
    DRAW_GRID
    DEF_AND_DRAW_FINPUT
End Sub

Private Sub Timer_Timer()
    LB_TIMER = LB_TIMER + 1
    If LB_TIMER = 1 Then
       frLEVELS.Visible = True
       frFASES.Visible = True
    End If
    If LB_TIMER = 10 Then
       frLEVELS.Visible = False
       frFASES.Visible = False
       LB_TIMER = 10
    End If
End Sub

Private Sub vsINPUTFASES_Change(Index As Integer)
    LB_TIMER = 0
    lbLEVELS.Caption = "F" & Trim$(Str$((Index + 1))) & "=" & (127 - vsINPUTLEVELS(Index).Value)
    lbFASES.Caption = vsINPUTFASES(Index).Value & "o"
    DRAW_GRID
    DEF_AND_DRAW_FINPUT
    DoEvents
End Sub

Private Sub vsINPUTFASES_Scroll(Index As Integer)
    vsINPUTFASES_Change Index
End Sub

Private Sub vsINPUTLEVELS_Change(Index As Integer)
    LB_TIMER = 0
    lbLEVELS.Caption = "F" & Trim$(Str$((Index + 1))) & "=" & (127 - vsINPUTLEVELS(Index).Value)
    lbFASES.Caption = vsINPUTFASES(Index).Value & "o"
    DRAW_GRID
    DEF_AND_DRAW_FINPUT
    DoEvents
End Sub

Private Sub vsINPUTLEVELS_Scroll(Index As Integer)
    vsINPUTLEVELS_Change Index
End Sub
