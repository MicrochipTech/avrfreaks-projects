VERSION 5.00
Begin VB.Form frm_2RDAC 
   AutoRedraw      =   -1  'True
   Caption         =   "2^r (DAC)"
   ClientHeight    =   5640
   ClientLeft      =   165
   ClientTop       =   915
   ClientWidth     =   8580
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   ScaleHeight     =   5640
   ScaleWidth      =   8580
   StartUpPosition =   3  'Windows Default
   Begin VB.PictureBox pic_OUT 
      AutoRedraw      =   -1  'True
      BackColor       =   &H00FFFFFF&
      Height          =   5505
      Left            =   2280
      ScaleHeight     =   363
      ScaleMode       =   3  'Pixel
      ScaleWidth      =   411
      TabIndex        =   2
      Top             =   60
      Width           =   6225
   End
   Begin VB.TextBox txt_VALUE 
      Height          =   345
      Left            =   330
      MaxLength       =   10
      TabIndex        =   1
      Top             =   1290
      Visible         =   0   'False
      Width           =   2175
   End
   Begin VB.PictureBox pic_RESISTOR 
      Appearance      =   0  'Flat
      AutoRedraw      =   -1  'True
      BackColor       =   &H80000005&
      BorderStyle     =   0  'None
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H80000008&
      Height          =   435
      Index           =   0
      Left            =   510
      ScaleHeight     =   435
      ScaleWidth      =   1575
      TabIndex        =   0
      Top             =   360
      Width           =   1575
   End
   Begin VB.Menu mnu_ARQUIVO 
      Caption         =   "&Arquivo"
      Begin VB.Menu mnu_SAIR 
         Caption         =   "&Sair"
      End
   End
   Begin VB.Menu mnu_RESISTORES 
      Caption         =   "&Número resistores"
      Begin VB.Menu mnu_NUMERO_RESISTORES 
         Caption         =   "2"
         Index           =   0
      End
      Begin VB.Menu mnu_NUMERO_RESISTORES 
         Caption         =   "3"
         Index           =   1
      End
      Begin VB.Menu mnu_NUMERO_RESISTORES 
         Caption         =   "4"
         Index           =   2
      End
      Begin VB.Menu mnu_NUMERO_RESISTORES 
         Caption         =   "5"
         Checked         =   -1  'True
         Index           =   3
      End
      Begin VB.Menu mnu_NUMERO_RESISTORES 
         Caption         =   "6"
         Index           =   4
      End
      Begin VB.Menu mnu_NUMERO_RESISTORES 
         Caption         =   "7"
         Index           =   5
      End
      Begin VB.Menu mnu_NUMERO_RESISTORES 
         Caption         =   "8"
         Index           =   6
      End
   End
End
Attribute VB_Name = "frm_2RDAC"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
'+---------------------+
'|  GLOBAL VARIABLES   |
'+---------------------+
Dim sel_RESISTOR As Integer

Private Sub Form_Load()
'+-------------------------+
'|  CREATE FIRST RESISTOR  |
'+-------------------------+
Dim j As Integer

    'CREATE RESISTORS
    
    pic_RESISTOR(0).BackColor = Me.BackColor
    For j = 1 To 10
        If j > 1 Then
            Load pic_RESISTOR(j - 1)
            pic_RESISTOR(j - 1).Top = pic_RESISTOR(j - 2).Top + pic_RESISTOR(j - 2).Height + 4 * Screen.TwipsPerPixelY
            pic_RESISTOR(j - 1).Tag = ""
        End If
    Next

    CREATE_RESISTORS 10000
    
End Sub

Private Sub Form_Unload(Cancel As Integer)
'+---------------------+
'|  END PROGRAM        |
'+---------------------+
    End
End Sub

Sub CREATE_RESISTORS(ByVal value As Double, Optional ByVal REDRAW As Boolean = False)
'+---------------------+
'|  CREATE A RESISTOR  |
'+---------------------+
Const DIV = 6
Dim i   As Integer
Dim j   As Integer
Dim w   As Single
Dim w2  As Single
Dim x1  As Integer
Dim y1  As Integer
Dim x2  As Integer
Dim y2  As Integer
Dim x3  As Integer
Dim y3  As Integer
Dim x4  As Integer
Dim y4  As Integer
Dim ym  As Integer
Dim s   As String
Dim RESISTOR_COUNT As Integer

    Me.Cls
    Me.Refresh
    'GET NUMBER OF RESISTORS
    For i = 0 To mnu_NUMERO_RESISTORES.Count - 1
        If mnu_NUMERO_RESISTORES(i).Checked Then
            RESISTOR_COUNT = Val(mnu_NUMERO_RESISTORES(i).Caption)
            Exit For
        End If
    Next
    
    For i = 0 To pic_RESISTOR.Count - 1
        If i > 0 Then
            pic_RESISTOR(i).Visible = False
        End If
        pic_RESISTOR(i).Cls
        pic_RESISTOR(i).Refresh
    Next
    
    If REDRAW Then
    Else
        pic_RESISTOR(0).Tag = Trim(value)
    End If
    For j = 1 To RESISTOR_COUNT + 2
        
        pic_RESISTOR(j - 1).Visible = True
        If REDRAW Then
        Else
            If j > RESISTOR_COUNT Then
                pic_RESISTOR(j - 1).Tag = "1E12"
            Else
                pic_RESISTOR(j - 1).Tag = Trim(value)
            End If
        End If
       'DRAW RESISTOR
        w = (pic_RESISTOR(j - 1).Width - 20 * Screen.TwipsPerPixelX) / DIV
        w2 = w / 3
        ym = (pic_RESISTOR(j - 1).Height - pic_RESISTOR(j - 1).TextHeight(s)) / 2 + pic_RESISTOR(j - 1).TextHeight(s)
        pic_RESISTOR(j - 1).Line (0, ym)- _
                             (10 * Screen.TwipsPerPixelX, ym), vbBlack
        pic_RESISTOR(j - 1).Line (pic_RESISTOR(j - 1).Width - 10 * Screen.TwipsPerPixelX, ym)- _
                             (pic_RESISTOR(j - 1).Width, ym), vbBlack
        s = fmt(Val(pic_RESISTOR(j - 1).Tag))
        pic_RESISTOR(j - 1).CurrentX = (pic_RESISTOR(j - 1).Width - pic_RESISTOR(j - 1).TextWidth(s)) / 2
        pic_RESISTOR(j - 1).CurrentY = 0
        pic_RESISTOR(j - 1).Print s
        value = value / 2
        For i = 0 To DIV - 1
            x1 = i * w + 10 * Screen.TwipsPerPixelX
            y1 = ym
            x2 = i * w + w2 + 10 * Screen.TwipsPerPixelX
            y2 = pic_RESISTOR(j - 1).TextHeight(s)
            x3 = i * w + 2 * w2 + 10 * Screen.TwipsPerPixelX
            y3 = pic_RESISTOR(j - 1).Height
            x4 = i * w + 3 * w2 + 10 * Screen.TwipsPerPixelX
            y4 = ym
            pic_RESISTOR(j - 1).Line (x1, y1)-(x2, y2), vbBlack
            pic_RESISTOR(j - 1).Line -(x3, y3), vbBlack
            pic_RESISTOR(j - 1).Line -(x4, y4), vbBlack
        Next
        If j <= RESISTOR_COUNT Then
            s = "D" + Trim(j - 1)
        Else
            If j = RESISTOR_COUNT + 1 Then
                s = "+V"
            Else
                s = "Gnd"
            End If
        End If
        Me.CurrentX = pic_RESISTOR(j - 1).Left - pic_RESISTOR(j - 1).TextWidth(s)
        Me.CurrentY = pic_RESISTOR(j - 1).Top + ym - pic_RESISTOR(j - 1).TextHeight(s) / 2
        Me.Print s
    Next
    x1 = pic_RESISTOR(0).Left + pic_RESISTOR(0).Width
    y1 = pic_RESISTOR(0).Top + ym
    x2 = pic_RESISTOR(RESISTOR_COUNT + 1).Left + pic_RESISTOR(RESISTOR_COUNT + 1).Width
    y2 = pic_RESISTOR(RESISTOR_COUNT + 1).Top + ym
    Me.Line (x1, y1)-(x2, y2), vbBlack
    Me.Refresh
    DRAW_LEVELS pic_OUT
    
End Sub

Function fmt(ByVal value As Double) As String
'+--------------------------------------+
'| format value to appropriate range    |
'+--------------------------------------+

    If value > 1000000# Then
        fmt = Format(value / 1000000, "0.00") + " M"
    Else
        If value > 1000 Then
            fmt = Format(value / 1000, "0.00") + " K"
        Else
            fmt = Format(value, "0.00") + " Ohm"
        End If
    End If
End Function

Private Sub mnu_NUMERO_RESISTORES_Click(Index As Integer)
'+--------------------------------------+
'| RECRIATE RESISTORS NET               |
'+--------------------------------------+
Dim i As Integer
    For i = 0 To mnu_NUMERO_RESISTORES.Count - 1
        mnu_NUMERO_RESISTORES(i).Checked = False
    Next
    mnu_NUMERO_RESISTORES(Index).Checked = True
    CREATE_RESISTORS Val(pic_RESISTOR(0).Tag)
    
End Sub

Private Sub pic_RESISTOR_Click(Index As Integer)
'+--------------------------------------+
'| GET VALUE                            |
'+--------------------------------------+

    sel_RESISTOR = Index
    txt_VALUE.Text = ""
    txt_VALUE.Top = pic_RESISTOR(Index).Top + pic_RESISTOR(Index).TextHeight("S")
    txt_VALUE.Left = pic_RESISTOR(Index).Left
    txt_VALUE.Width = pic_RESISTOR(Index).Width
    txt_VALUE.Height = pic_RESISTOR(Index).Height - pic_RESISTOR(Index).TextHeight("S")
    txt_VALUE.Visible = True
    txt_VALUE.SetFocus
    
End Sub

Private Sub txt_VALUE_KeyDown(KeyCode As Integer, Shift As Integer)
'+--------------------------------------+
'| PROCESS WHEN PRESS TAB               |
'+--------------------------------------+

    Select Case KeyCode
        Case vbKeyReturn
            txt_VALUE.Visible = False
            pic_RESISTOR(sel_RESISTOR).Tag = Trim(gvalue(txt_VALUE.Text))
            CREATE_RESISTORS 0, True
    End Select
End Sub

Function gvalue(ByVal VALUE_STRING As String) As Double
'+--------------------------------------+
'| get formated value                   |
'+--------------------------------------+
Dim a As Double

    a = Val(VALUE_STRING)
    If InStr(1, UCase(VALUE_STRING), "K") Then a = a * 1000
    If InStr(1, UCase(VALUE_STRING), "M") Then a = a * 1000000
    gvalue = a
    
End Function

Sub DRAW_LEVELS(CTRL As Control)
'+--------------------------------------+
'| DRAW 2RDAC OUTPUT WAVE FORMAT        |
'+--------------------------------------+
Dim i       As Integer
Dim j       As Integer
Dim LEVELS  As Integer
Dim BITS    As Integer
Dim w       As Double
Dim h       As Double
Dim EQ_HI   As Double
Dim EQ_LO   As Double
Dim ITOT    As Double
Dim VTOP    As Double
Dim VDAC    As Double
Dim VDACL   As Double


    'GET NUMBER OF LEVELS
    For i = 0 To mnu_NUMERO_RESISTORES.Count - 1
        If mnu_NUMERO_RESISTORES(i).Checked Then
            LEVELS = Val(mnu_NUMERO_RESISTORES(i).Caption)
            Exit For
        End If
    Next

    'CLEAR SCREEN OF GRAPH
    CTRL.Cls
    BITS = LEVELS
    LEVELS = 2 ^ LEVELS
    w = CTRL.ScaleWidth / LEVELS
    'DRAW REFERENCE LINES
    For i = 0 To LEVELS - 1
        CTRL.Line (i * w, 0)-(i * w, CTRL.ScaleHeight), RGB(200, 200, 200)
    Next
    h = CTRL.ScaleHeight / 10
    For i = 0 To 10 - 1
        CTRL.Line (0, i * h)-(CTRL.ScaleWidth, i * h), RGB(200, 200, 200)
        CTRL.CurrentX = 0
        CTRL.CurrentY = i * h
        CTRL.Print Format(100 - i / 10 * 100, "##0") & "%"
    Next
    'DRAW LEVELS
    VTOP = pic_OUT.ScaleHeight
    VDACL = 0
    For i = 0 To LEVELS - 1
        'COMPUTE EQUIVALENTS NETWORK OF RESISTORS HI AND LO
        EQ_HI = 0
        EQ_LO = 0
        For j = 0 To BITS - 1
            If (i And 2 ^ j) <> 0 Then
                EQ_HI = EQ_HI + 1 / Val(pic_RESISTOR(j).Tag)
            Else
                EQ_LO = EQ_LO + 1 / Val(pic_RESISTOR(j).Tag)
            End If
        Next
        EQ_LO = EQ_LO + 1 / 1000000000000#
        EQ_HI = EQ_HI + 1 / 1000000000000#
        EQ_LO = EQ_LO + 1 / Val(pic_RESISTOR(BITS + 1).Tag)
        EQ_HI = EQ_HI + 1 / Val(pic_RESISTOR(BITS).Tag)
        EQ_LO = 1 / EQ_LO
        EQ_HI = 1 / EQ_HI
        ITOT = VTOP / (EQ_LO + EQ_HI)
        VDAC = ITOT * EQ_LO
        pic_OUT.Line (i * w, VTOP - VDAC)-(i * w + w, VTOP - VDAC), vbBlue
        pic_OUT.Line (i * w, VTOP - VDACL)-(i * w, VTOP - VDAC), vbBlue
        VDACL = VDAC
    Next
    CTRL.Refresh
End Sub
