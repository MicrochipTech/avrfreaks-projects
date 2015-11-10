VERSION 5.00
Begin VB.Form frm_DIGITIZING 
   Caption         =   "Digitizing using ATMega Analog Comparator Study"
   ClientHeight    =   7605
   ClientLeft      =   60
   ClientTop       =   510
   ClientWidth     =   13080
   Icon            =   "frm_DIGITIZING.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   507
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   872
   StartUpPosition =   1  'CenterOwner
   WindowState     =   2  'Maximized
   Begin VB.Frame frm_VALUES 
      BackColor       =   &H00E0E0E0&
      Caption         =   "Lo Step"
      Height          =   585
      Index           =   3
      Left            =   5070
      TabIndex        =   7
      Top             =   0
      Width           =   1665
      Begin VB.Label lbl_LO_STEP 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H00E0E0E0&
         Caption         =   "Label1"
         BeginProperty Font 
            Name            =   "Courier New"
            Size            =   12
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H80000008&
         Height          =   255
         Left            =   90
         TabIndex        =   8
         Top             =   210
         Width           =   1515
      End
   End
   Begin VB.Frame frm_VALUES 
      BackColor       =   &H00E0E0E0&
      Caption         =   "Hi Step"
      Height          =   585
      Index           =   2
      Left            =   3360
      TabIndex        =   5
      Top             =   0
      Width           =   1665
      Begin VB.Label lbl_HI_STEP 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H00E0E0E0&
         Caption         =   "Label1"
         BeginProperty Font 
            Name            =   "Courier New"
            Size            =   12
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H80000008&
         Height          =   255
         Left            =   90
         TabIndex        =   6
         Top             =   210
         Width           =   1515
      End
   End
   Begin VB.Frame frm_VALUES 
      BackColor       =   &H00E0E0E0&
      Caption         =   "Bottom Value"
      Height          =   585
      Index           =   1
      Left            =   1680
      TabIndex        =   3
      Top             =   0
      Width           =   1665
      Begin VB.Label lbl_BOTTON_VALUE 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H00E0E0E0&
         Caption         =   "Label1"
         BeginProperty Font 
            Name            =   "Courier New"
            Size            =   12
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H80000008&
         Height          =   255
         Left            =   90
         TabIndex        =   4
         Top             =   210
         Width           =   1515
      End
   End
   Begin VB.Frame frm_VALUES 
      BackColor       =   &H00E0E0E0&
      Caption         =   "Top Value"
      Height          =   585
      Index           =   0
      Left            =   0
      TabIndex        =   1
      Top             =   0
      Width           =   1665
      Begin VB.Label lbl_TOP_VALUE 
         Alignment       =   1  'Right Justify
         Appearance      =   0  'Flat
         BackColor       =   &H00E0E0E0&
         Caption         =   "Label1"
         BeginProperty Font 
            Name            =   "Courier New"
            Size            =   12
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H80000008&
         Height          =   255
         Left            =   90
         TabIndex        =   2
         Top             =   210
         Width           =   1515
      End
   End
   Begin VB.PictureBox pic_WORK 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BorderStyle     =   0  'None
      ForeColor       =   &H80000008&
      Height          =   5385
      Left            =   1500
      ScaleHeight     =   359
      ScaleMode       =   3  'Pixel
      ScaleWidth      =   609
      TabIndex        =   0
      Top             =   1590
      Width           =   9135
   End
End
Attribute VB_Name = "frm_DIGITIZING"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'    ____________________________________________________________________________________________
'   |////////////////////////////////////////////////////////////////////////////////////////////|
'   |///|                                                                                   |////|
'   |///| PROJETO    : DETECTOR DE MOVIMENTO VERSÃO 5.0(MOTION DETECTOR V5.0)               |////|
'   |///| VERSAO     : 1.0                                                                  |////|
'   |///| PROGRAMADOR: JOÃO DARTAGNAN ANTUNES OLIVEIRA                                      |////|
'   |///| DATA   : 6 de Dezembro de 2006 (00:06)                                            |////|
'   |///| DESCRIÇÃO  : Esse projeto possui 2 conectores BNC, 1 para entrada de video e outro|////|
'   |///|          para saida, 1 saida de rele NC E NO,1 porta de comunicação 485 com       |////|
'   |///|          leds de TX E RX, 1 porta de expansão e 1 para programacao ISP            |////|
'   |///| OBJETIVO:    O circuito destina-se a captura de sinal composto de video padrao PAL|////|
'   |///|          NTSC color ou BW, digitalizar e processar a ocorrencia de movimento      |////|
'   |///|          em areas especificas e avisar atraves de comunicação serial e/ou ati-    |////|
'   |///|          vação de relé. Possuindo ainda um software para geração de video com-    |////|
'   |///|          posto BW com MENU de configuração e GENLOCK com sinal de entrada de      |////|
'   |///|          video.                                                                   |////|
'   |///|                                                                                   |////|
'   |///| OBS:    This software run only AVRMega family chips                               |////|
'   |///|___________________________________________________________________________________|////|
'---|////////////////////////////////////////////////////////////////////////////////////////////|
'   >--------------------------------------------------------------------------------------------<
'---|                                                                                            |
'   |               Digitizing using AVRMega Analog Comparator study                             |
'   |                                                                                            |
'   \--------------------------------------------------------------------------------------------/

'+----------------------+
'| PUBLIC OBJECTS       |
'+----------------------+
Dim tm As New cls_JDAO_TIME
Dim im As New cls_IMAGE_FRAME

'+----------------------+
'| PUBLIC ARRAYS        |
'+----------------------+

Const DA_PWM_ARRAY_MAX = 128

Dim DA_PWM_ARRAY(0 To DA_PWM_ARRAY_MAX) As Integer
Dim DA_PWM_ARRAY_COLOR(0 To DA_PWM_ARRAY_MAX) As Long

'+----------------------+
'| PUBLIC VARIABLES     |
'+----------------------+

Dim M_X             As Integer
Dim M_Y             As Integer
Dim value_BOTTOM    As Integer
Dim value_TOP       As Integer
Dim rgn_STOP        As Long
Dim b_WORK          As Boolean


Private Sub Form_Load()
'+--------------------------------------------+
'| INITIALIZE MAIN PAGE AND CREATE NEW FRAME  |
'+--------------------------------------------+
Const MAX_DIV = 16
Const EVENT_TO_UP = 1
Const EVENT_TO_DOWN = 2
Const EVENT_DIGITIZING_UP = 3
Const EVENT_DIGITIZING_DOWN = 4

Dim H           As Single
Dim W           As Single
Dim I           As Integer
Dim digi_EVENT  As Integer
Dim color_EVENT As Long
Dim lo_STEP     As Integer
Dim hi_STEP     As Integer
Dim d_STEP      As Integer
Dim da_PWM      As Integer
Dim da_PWM_PREP As Integer
Dim da_PWM_LAST As Integer


    'create frame
    CREATE_FRAME
    
    'show main form
    Me.Show
        
    'MAIN PROCESS LOOP
    digi_EVENT = EVENT_TO_UP
    lo_STEP = 1
    hi_STEP = 4
    tm.SYNC_RATE 30
    b_WORK = True
    d_STEP = 1
    Do
            'clear frame
            im.FRAME_CLS 150, 150, 150
            If b_WORK Then
                'DIGITIZING PROCESS
                Select Case digi_EVENT
                    Case EVENT_TO_UP
                        color_EVENT = vbRed
                        da_PWM_PREP = da_PWM_PREP + hi_STEP
                        da_PWM = da_PWM_PREP + lo_STEP
                        If da_PWM > value_TOP Then
                            digi_EVENT = EVENT_DIGITIZING_UP
                        End If
                    Case EVENT_DIGITIZING_UP
                        color_EVENT = vbWhite
                        hi_STEP = Int(Abs(da_PWM - da_PWM_LAST) / MAX_DIV) + 1
                        da_PWM_LAST = da_PWM
                        If hi_STEP <= 0 Then hi_STEP = 1
                        If lo_STEP > hi_STEP Then lo_STEP = hi_STEP
                        digi_EVENT = EVENT_TO_DOWN
                        lo_STEP = lo_STEP + d_STEP
                        If lo_STEP > hi_STEP Then
                            lo_STEP = hi_STEP
                            d_STEP = -d_STEP
                        Else
                            If lo_STEP < 0 Then
                                lo_STEP = 0
                                d_STEP = -d_STEP
                            End If
                        End If
                        'lo_STEP = 0
                    Case EVENT_TO_DOWN
                        color_EVENT = vbBlue
                        da_PWM_PREP = da_PWM_PREP - hi_STEP
                        da_PWM = da_PWM_PREP + lo_STEP
                        If da_PWM < value_BOTTOM Then
                            digi_EVENT = EVENT_DIGITIZING_DOWN
                        End If
                    Case EVENT_DIGITIZING_DOWN
                        color_EVENT = vbWhite
                        hi_STEP = Int(Abs(da_PWM - da_PWM_LAST) / MAX_DIV) + 1
                        da_PWM_LAST = da_PWM
                        If hi_STEP <= 0 Then hi_STEP = 1
                        If lo_STEP > hi_STEP Then lo_STEP = hi_STEP
                        digi_EVENT = EVENT_TO_UP
                        lo_STEP = lo_STEP + d_STEP
                        If lo_STEP > hi_STEP Then
                            lo_STEP = hi_STEP
                            d_STEP = -d_STEP
                        Else
                            If lo_STEP < 0 Then
                                lo_STEP = 0
                                d_STEP = -d_STEP
                            End If
                        End If
                        'lo_STEP = 0
                End Select
            End If
            
            'draw ideal position lines
            H = (value_TOP - value_BOTTOM) / MAX_DIV
            For I = 0 To MAX_DIV - 1
                im.PIXEL_LINE 0, value_BOTTOM + H * I, im.FRAME_WIDTH, value_BOTTOM + H * I, RGB(120, 120, 120)
            Next
            'draw top line
            DRAW_HLINES 0, im.FRAME_WIDTH, value_TOP, vbYellow
            'draw bottom line
            DRAW_HLINES 0, im.FRAME_WIDTH, value_BOTTOM, vbGreen
            'draw cross line
            im.PIXEL_LINE 0, M_Y, im.FRAME_WIDTH, M_Y, vbWhite
        
            If b_WORK Then
                'draw PWM lines
                For I = 0 To DA_PWM_ARRAY_MAX - 1
                    DA_PWM_ARRAY(I) = DA_PWM_ARRAY(I + 1)
                    DA_PWM_ARRAY_COLOR(I) = DA_PWM_ARRAY_COLOR(I + 1)
                Next
                DA_PWM_ARRAY(DA_PWM_ARRAY_MAX) = da_PWM
                DA_PWM_ARRAY_COLOR(DA_PWM_ARRAY_MAX) = color_EVENT
            End If
            
            W = im.FRAME_WIDTH / DA_PWM_ARRAY_MAX
            For I = 0 To DA_PWM_ARRAY_MAX - 1
                DRAW_HLINES I * W, I * W + W, DA_PWM_ARRAY(I), DA_PWM_ARRAY_COLOR(I)
                DRAW_VLINES I * W + W, DA_PWM_ARRAY(I), DA_PWM_ARRAY(I + 1), DA_PWM_ARRAY_COLOR(I)
            Next
            'DRAW REGION
            im.SPRITE_REGION_SHOW rgn_STOP, vbWhite, , True
            im.SPRITE_REGION_SHOW rgn_STOP, vbBlack, , Not (b_WORK)
            
            lbl_TOP_VALUE.Caption = Trim(value_TOP)
            lbl_BOTTON_VALUE.Caption = Trim(value_BOTTOM)
            lbl_HI_STEP.Caption = Trim(hi_STEP)
            lbl_LO_STEP.Caption = Trim(lo_STEP)
        'show frame
        im.FRAME_SHOW pic_WORK, 0, 0
        'leave other events
        tm.SYNC
        DoEvents
    Loop
End Sub


Sub DRAW_HLINES(ByVal XI As Integer, ByVal XF As Integer, ByVal YI As Integer, ByVal COLOR As Long)
'+--------------------------------------------+
'| DRAW HORIZONTAL LINE WITH SHADOW           |
'+--------------------------------------------+
    im.PIXEL_LINE XI + 1, YI - 1, XF + 1, YI - 1, vbBlack
    im.PIXEL_LINE XI, YI, XF, YI, COLOR
End Sub
Sub DRAW_VLINES(ByVal XI As Integer, ByVal YI As Integer, ByVal YF As Integer, ByVal COLOR As Long)
'+--------------------------------------------+
'| DRAW VERTICAL LINE WITH SHADOW             |
'+--------------------------------------------+
    im.PIXEL_LINE XI + 1, YI - 1, XI + 1, YF - 1, vbBlack
    im.PIXEL_LINE XI, YI, XI, YF, COLOR
End Sub

Sub CREATE_FRAME()
'+--------------------------------------------+
'| CREATE FRAME                               |
'+--------------------------------------------+
    
    pic_WORK.Left = 0
    pic_WORK.Top = 50
    pic_WORK.WIDTH = Me.ScaleWidth
    pic_WORK.HEIGHT = Me.ScaleHeight - 50
    Set im = Nothing
    im.FRAME_CREATE pic_WORK.ScaleWidth, pic_WORK.ScaleHeight
    rgn_STOP = im.SPRITE_REGION_CREATE_RECT(im.FRAME_WIDTH - 50, im.FRAME_HEIGHT - 50, im.FRAME_WIDTH - 10, im.FRAME_HEIGHT - 10)
    
End Sub

Private Sub pic_WORK_MouseDown(Button As Integer, Shift As Integer, x As Single, y As Single)
'+---------------------------------------------------------------------+
'| SET TOP AND BOTTOM LINES ACCORDING CLICK LEFT OR RIGHT BUTTON MOUSE |
'+---------------------------------------------------------------------+

    If im.SPRITE_REGIONS_COUNT = 0 Then Exit Sub
    If im.SPRITE_REGION_CHECK(rgn_STOP, M_X, M_Y) Then
        b_WORK = Not (b_WORK)
    Else
        Select Case Button
            Case vbLeftButton
                value_TOP = pic_WORK.ScaleHeight - y
            Case vbRightButton
                value_BOTTOM = pic_WORK.ScaleHeight - y
        End Select
        
        If value_BOTTOM > value_TOP Then
            value_BOTTOM = value_TOP
        Else
            If value_TOP < value_BOTTOM Then
                value_TOP = value_BOTTOM
            End If
        End If
    End If
End Sub

Private Sub pic_WORK_MouseMove(Button As Integer, Shift As Integer, x As Single, y As Single)
'+--------------------------------------------+
'| GET COORDINATES WHEN MOUSE MOVE            |
'+--------------------------------------------+
    
    M_X = x
    M_Y = pic_WORK.ScaleHeight - y
    
    If im.SPRITE_REGIONS_COUNT = 0 Then Exit Sub
    If im.SPRITE_REGION_CHECK(rgn_STOP, M_X, M_Y) Then
    Else
        Select Case Button
            Case vbLeftButton
                value_TOP = pic_WORK.ScaleHeight - y
            Case vbRightButton
                value_BOTTOM = pic_WORK.ScaleHeight - y
        End Select
        If value_BOTTOM > value_TOP Then
            value_BOTTOM = value_TOP
        Else
            If value_TOP < value_BOTTOM Then
                value_TOP = value_BOTTOM
            End If
        End If
    End If
End Sub

Private Sub Form_Resize()
'+--------------------------------------------+
'| RECREATE FRAME IF WINDOWS RESIZE           |
'+--------------------------------------------+

    CREATE_FRAME
    
End Sub

Private Sub Form_Unload(Cancel As Integer)
'+--------------------------------------------+
'| EXIT PROGRAM                               |
'+--------------------------------------------+
    
    Set im = Nothing
    End
End Sub

