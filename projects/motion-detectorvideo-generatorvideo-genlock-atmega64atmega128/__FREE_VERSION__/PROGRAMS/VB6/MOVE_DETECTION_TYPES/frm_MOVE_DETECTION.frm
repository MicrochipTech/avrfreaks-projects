VERSION 5.00
Begin VB.Form frm_MOVE_DETECTION 
   BorderStyle     =   4  'Fixed ToolWindow
   Caption         =   "Move Detection Area Selection Types"
   ClientHeight    =   5220
   ClientLeft      =   45
   ClientTop       =   645
   ClientWidth     =   11430
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   348
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   762
   ShowInTaskbar   =   0   'False
   StartUpPosition =   1  'CenterOwner
   Begin VB.Menu mnu_FILES 
      Caption         =   "&Files"
      Begin VB.Menu mnu_EXIT 
         Caption         =   "&Exit"
      End
   End
   Begin VB.Menu mnu_MOVETYPES 
      Caption         =   "&Types"
      Begin VB.Menu mnu_MOVETYPE 
         Caption         =   "&Contour"
         Checked         =   -1  'True
         Index           =   0
      End
      Begin VB.Menu mnu_MOVETYPE 
         Caption         =   "&Ilhas"
         Index           =   1
      End
   End
End
Attribute VB_Name = "frm_MOVE_DETECTION"
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
'   |                               Move Detection Area Types                                    |
'   |                                                                                            |
'   \--------------------------------------------------------------------------------------------/
Option Explicit


'+----------------------+
'| PUBLIC OBJECTS       |
'+----------------------+
Dim tm As New cls_JDAO_TIME
Dim im As New cls_IMAGE_FRAME

'+----------------------+
'| PUBLIC CONSTANTS     |
'+----------------------+

Const MATRIX_ROWS = 24
Const MATRIX_COLS = 24
Const MATRIX_WIDTH = 16
Const MATRIX_HEIGHT = 16

Const MOUSE_DOWN = 1
Const MOUSE_MOVE = 2
Const MOUSE_UP = 3

Const MOVE_TYPE_CONTOUR = 0
Const MOVE_TYPE_ISLAND = 1

'+----------------------+
'| PUBLIC ARRAYS        |
'+----------------------+

Dim MATRIX(1 To MATRIX_ROWS * MATRIX_COLS) As Boolean
Dim MATRIX_CONTOUR(1 To MATRIX_ROWS * MATRIX_COLS) As Boolean

'+----------------------+
'| PUBLIC VARIABLES     |
'+----------------------+

Dim M_X             As Integer              'MOUSE COORDINATES AND BUTTON STATE
Dim M_Y             As Integer
Dim M_B             As Integer
Dim M_E             As Integer

Dim MOVE_TYPE       As Integer

Private Sub Form_Load()
'+------------------------------------------+
'| ALL PROGRAM WORK IN LOOP IN LOAD EVENT   |
'+------------------------------------------+
Const MATRIX_X = 0
Const MATRIX_Y = 0

Dim i As Integer

    'DIMENSION FORM
    Me.Width = Me.ScaleX(MATRIX_COLS * MATRIX_WIDTH * 2 + 8, vbPixels, vbTwips)
    Me.Height = Me.ScaleY(MATRIX_ROWS * MATRIX_HEIGHT + 8, vbPixels, vbTwips)
    'SHOW MAIN FORM
    Me.Show
    'CREATE FRAME
    im.FRAME_CREATE Me.ScaleWidth, Me.ScaleHeight
    'DEFINE SYNC RATE
    tm.SYNC_RATE 30
    'CREATE MATRIX REGIONS
    CREATE_MATRIX_REGIONS MATRIX_X, MATRIX_Y
    
    Do
        'CLEAR FRAME
        im.FRAME_CLS
        'DRAW MOVE MATRIX
        DRAW_MATRIX MATRIX_X, MATRIX_Y, vbRed
        'DRAW CONTOUR ACCORDING TYPE
        Select Case MOVE_TYPE
            'DRAW MOVE MATRIX TYPE CONTOUR
            Case MOVE_TYPE_CONTOUR
                DRAW_MATRIX_MOVE_TYPE_CONTOUR MATRIX_X + MATRIX_COLS * MATRIX_WIDTH, MATRIX_Y, vbYellow
            'DRAW MOVE MATRIX TYPE CONTOUR
            Case MOVE_TYPE_ISLAND
                DRAW_MATRIX_MOVE_TYPE_ISLAND MATRIX_X + MATRIX_COLS * MATRIX_WIDTH, MATRIX_Y, vbGreen
        End Select
        'DIVISION
        im.PIXEL_LINE MATRIX_COLS * MATRIX_WIDTH, 0, MATRIX_COLS * MATRIX_WIDTH, im.FRAME_HEIGHT, vbWhite
        'CHECK MATRIX
        Select Case M_B
            Case vbLeftButton
                CHECK_MATRIX M_X, M_Y, True
            Case vbRightButton
                CHECK_MATRIX M_X, M_Y, False
        End Select
        'SHOW FRAME
        im.FRAME_SHOW Me, 0, 0
        'SYNCHRONIZE
        tm.SYNC
        DoEvents
    Loop
End Sub

Private Sub Form_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
'+--------------------------------------------------+
'| GET MOUSE COORDINATES AND STATE WHEN MOUSE DOWN  |
'+--------------------------------------------------+

    M_X = X
    M_Y = Me.ScaleHeight - Y
    M_B = Button
    M_E = MOUSE_DOWN

End Sub

Private Sub Form_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)
'+-------------------------------------------------+
'| GET MOUSE COORDINATES AND STATE WHEN MOUSE MOVE |
'+-------------------------------------------------+

    M_X = X
    M_Y = Me.ScaleHeight - Y
    M_B = Button
    M_E = MOUSE_MOVE
    
End Sub

Private Sub Form_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
'+--------------------------------------------------+
'| GET MOUSE COORDINATES AND STATE WHEN MOUSE UP    |
'+--------------------------------------------------+

    M_X = X
    M_Y = Me.ScaleHeight - Y
    M_B = Button
    M_E = MOUSE_UP

End Sub

Private Sub Form_Unload(Cancel As Integer)
'+------------------------------------------+
'| FINISH PROGRAM                           |
'+------------------------------------------+
    Set im = Nothing
    Set tm = Nothing
    End
End Sub

Sub DRAW_MATRIX(ByVal PX As Integer, ByVal PY As Integer, ByVal MATRIX_COLOR As Long)
'+------------------------------------------+
'| DRAWING INPUT MOVE MATRIX STARTING AT    |
'| COORDINATES PX,PY                        |
'+------------------------------------------+
Dim i   As Integer
Dim X   As Integer
Dim Y   As Integer
Dim xi  As Long
Dim yi  As Long
Dim xf  As Long
Dim yf  As Long


    i = 1
    For Y = 0 To MATRIX_ROWS - 1
        For X = 0 To MATRIX_COLS - 1
            im.SPRITE_REGION_SHOW i, vbWhite, False, MATRIX(i)
            im.SPRITE_REGION_GET_COORDS i, xi, yi, xf, yf
            im.PIXEL_BOX xi, yi, xf, yf, MATRIX_COLOR
            i = i + 1
        Next
    Next
    
End Sub

Sub DRAW_MATRIX_MOVE_TYPE_CONTOUR(ByVal PX As Integer, ByVal PY As Integer, ByVal MATRIX_COLOR As Long)
'+------------------------------------------+
'| DRAWING INPUT MOVE MATRIX TYPE CONTOUR   |
'| STARTING AT COORDINATES PX,PY            |
'+------------------------------------------+
Dim X   As Integer
Dim Y   As Integer
Dim xi  As Long
Dim yi  As Long
Dim xf  As Long
Dim yf  As Long

    For Y = 0 To MATRIX_ROWS - 1
        For X = 0 To MATRIX_COLS - 1
            If X < 0 Or Y < 0 Then
            Else
                im.SPRITE_REGION_GET_COORDS XYI(X, Y), xi, yi, xf, yf
            End If
            If GET_MATRIX(X, Y) <> GET_MATRIX(X + 1, Y) Then
                im.PIXEL_LINE xf + PX, yi + PY, xf + PX, yf + PY, MATRIX_COLOR
            End If
            If GET_MATRIX(X, Y) <> GET_MATRIX(X, Y + 1) Then
                im.PIXEL_LINE xi + PX, yf + PY, xf + PX, yf + PY, MATRIX_COLOR
            End If
        Next
    Next
End Sub

Sub DRAW_MATRIX_MOVE_TYPE_ISLAND(ByVal PX As Integer, ByVal PY As Integer, ByVal MATRIX_COLOR As Long)
'+------------------------------------------+
'| DRAWING INPUT MOVE MATRIX TYPE CONTOUR   |
'| STARTING AT COORDINATES PX,PY            |
'+------------------------------------------+
Dim X   As Integer
Dim Y   As Integer
Dim xi  As Long
Dim yi  As Long
Dim xf  As Long
Dim yf  As Long
Dim i   As Long

    For i = 1 To UBound(MATRIX_CONTOUR)
        MATRIX_CONTOUR(i) = False
    Next
    'search for island
    For Y = 0 To MATRIX_ROWS - 1
        For X = 0 To MATRIX_COLS - 1
            If GET_MATRIX_CONTOUR(X, Y) = False Then
                im.PIXEL_CIRCLE PX + X * MATRIX_WIDTH + MATRIX_WIDTH / 2, Y * MATRIX_HEIGHT + MATRIX_HEIGHT / 2, MATRIX_WIDTH / 4, vbRed, True
                If GET_MATRIX(X, Y) = True Then
                    'if found a island then draw island contour
                    im.PIXEL_CIRCLE X * MATRIX_WIDTH + MATRIX_WIDTH / 2, Y * MATRIX_HEIGHT + MATRIX_HEIGHT / 2, MATRIX_WIDTH / 2, vbYellow, True
                    im.PIXEL_CIRCLE X * MATRIX_WIDTH + MATRIX_WIDTH / 2, Y * MATRIX_HEIGHT + MATRIX_HEIGHT / 2, MATRIX_WIDTH / 2, vbBlack
                    DRAW_ISLAND_CONTOUR X, Y
                    GoTo DRAW_CONTOUR
                End If
            End If
        Next
    Next
DRAW_CONTOUR:
    
    
    'DRAW CONTOUR
    For Y = 0 To MATRIX_ROWS - 1
        For X = 0 To MATRIX_COLS - 1
            If X < 0 Or Y < 0 Then
            Else
                im.SPRITE_REGION_GET_COORDS XYI(X, Y), xi, yi, xf, yf
            End If
            im.PIXEL_BOX xi + PX, yi + PY, xf + PX, yf + PY, MATRIX_COLOR
            If GET_MATRIX_CONTOUR(X, Y) Then
                im.PIXEL_BOX xi + PX, yi + PY, xf + PX, yf + PY, MATRIX_COLOR, True
            End If
        Next
    Next

End Sub

Sub DRAW_ISLAND_CONTOUR(ByVal X As Integer, ByVal Y As Integer)
'+------------------------------------------+
'| DRAWING ISLAND CONTOUR                   |
'+------------------------------------------+
Dim XS  As Integer
Dim YS  As Integer
Dim XN  As Integer
Dim YN  As Integer

Dim xi  As Long
Dim yi  As Long
Dim xf  As Long
Dim yf  As Long
Dim i   As Long
Dim LA  As Integer
Dim A   As Integer
Dim CHF As Boolean
Dim C   As Integer

    'SAVE START POINT
    XS = X
    YS = Y
    'START WITH ANGLE 4
    LA = 4
    'MAIN CONTOUR LOOP
    A = LA
    Do
        'ANGLE=LAST ANGLE
        A = A + 1
        If A > 7 Then A = 0
        C = C + 1
        'POINT IS SET
        If ADPIXEL(X, Y, A, XN, YN) Then
            C = 0
            MATRIX_CONTOUR(XYI(XN, YN)) = True
            If XN = XS And YN = YS Then Exit Do
            A = IIf((A - 4) < 0, A - 4 + 8, A - 4)
            X = XN
            Y = YN
        End If
    Loop Until (C >= 8)
    MATRIX_CONTOUR(XYI(X, Y)) = True
End Sub

Function ADPIXEL(ByVal CX As Integer, ByVal CY As Integer, ByVal INDEX As Integer, ByRef PX As Integer, ByRef PY As Integer) As Boolean
'+------------------------------------------------------+
'|                                                      |
'| RETURN VALUE OF ADJACENT PIXEL OF POINT(PX,PY)       |
'| ACCORDING VALUE OF INDEX                             |
'|                                                      |
'|    +---------+---------+---------+                   |
'|    |         |         |         |                   |
'|    |    5    |    6    |    7    |                   |
'|    |         |         |         |                   |
'|    +---------+---------+---------+                   |
'|    |         |         |         |                   |
'|    |    4    |P(CX,CY) |    0    |                   |
'|    |         |         |         |                   |
'|    +---------+---------+---------+                   |
'|    |         |         |         |                   |
'|    |    3    |    2    |    1    |                   |
'|    |         |         |         |                   |
'|    +---------+---------+---------+                   |
'+------------------------------------------------------+
    Select Case INDEX
        Case 0
            ADPIXEL = GET_MATRIX(CX + 1, CY)
            PX = CX + 1
            PY = CY
        Case 1
            ADPIXEL = GET_MATRIX(CX + 1, CY + 1)
            PX = CX + 1
            PY = CY + 1
        Case 2
            ADPIXEL = GET_MATRIX(CX, CY + 1)
            PX = CX
            PY = CY + 1
        Case 3
            ADPIXEL = GET_MATRIX(CX - 1, CY + 1)
            PX = CX - 1
            PY = CY + 1
        Case 4
            ADPIXEL = GET_MATRIX(CX - 1, CY)
            PX = CX - 1
            PY = CY
        Case 5
            ADPIXEL = GET_MATRIX(CX - 1, CY - 1)
            PX = CX - 1
            PY = CY - 1
        Case 6
            ADPIXEL = GET_MATRIX(CX, CY - 1)
            PX = CX
            PY = CY - 1
        Case 7
            ADPIXEL = GET_MATRIX(CX + 1, CY - 1)
            PX = CX + 1
            PY = CY - 1
    End Select
End Function

Function XYI(ByVal X As Integer, ByVal Y As Integer) As Integer
'+------------------------------------------+
'| GET INDEX FROM X,Y COORDINATES           |
'|                                          |
'+------------------------------------------+
    XYI = Y * MATRIX_COLS + X + 1
End Function
Sub CREATE_MATRIX_REGIONS(ByVal PX As Integer, ByVal PY As Integer)
'+------------------------------------------+
'| DRAWING INPUT MOVE MATRIX STARTING AT    |
'| COORDINATES PX,PY                        |
'+------------------------------------------+
Dim i As Integer
Dim X As Integer
Dim Y As Integer

    For Y = 0 To MATRIX_ROWS - 1
        For X = 0 To MATRIX_COLS - 1
            im.SPRITE_REGION_CREATE_RECT X * MATRIX_WIDTH + PX, Y * MATRIX_HEIGHT + PY, _
                                         X * MATRIX_WIDTH + MATRIX_WIDTH + PX, Y * MATRIX_HEIGHT + MATRIX_HEIGHT + PY
        Next
    Next
    
End Sub

Sub CHECK_MATRIX(ByVal PX As Integer, ByVal PY As Integer, ByVal STATE As Boolean)
'+-------------------------------------------+
'| CHECK MATRIX AND ALTER ACCORDING NEW STATE|
'+-------------------------------------------+
Dim i As Integer

    For i = 1 To im.SPRITE_REGIONS_COUNT
        If im.SPRITE_REGION_CHECK(i, PX, PY) Then
            MATRIX(i) = STATE
        End If
    Next
End Sub

Function GET_MATRIX(ByVal X As Integer, ByVal Y As Integer) As Boolean
'+--------------------------------------------------+
'| GET MATRIX STATE AND RETURN FALSE IF OUTOF RANGE |
'+--------------------------------------------------+
    If X < 0 Or Y < 0 Then
        GET_MATRIX = False
    Else
        If X >= MATRIX_COLS Or Y >= MATRIX_ROWS Then
            GET_MATRIX = False
        Else
            GET_MATRIX = MATRIX(XYI(X, Y))
        End If
    End If
    
End Function

Function GET_MATRIX_CONTOUR(ByVal X As Integer, ByVal Y As Integer) As Boolean
'+--------------------------------------------------+
'| GET MATRIX STATE AND RETURN FALSE IF OUTOF RANGE |
'+--------------------------------------------------+
    If X < 0 Or Y < 0 Then
        GET_MATRIX_CONTOUR = False
    Else
        If X >= MATRIX_COLS Or Y >= MATRIX_ROWS Then
            GET_MATRIX_CONTOUR = False
        Else
            GET_MATRIX_CONTOUR = MATRIX_CONTOUR(XYI(X, Y))
        End If
    End If
    
End Function

Private Sub mnu_MOVETYPE_Click(INDEX As Integer)
'+--------------------------------------------------+
'| SELECT MOVE TYPES                                |
'+--------------------------------------------------+
Dim i As Integer
    
    For i = 0 To mnu_MOVETYPE.Count - 1
         mnu_MOVETYPE(i).Checked = False
    Next
    mnu_MOVETYPE(INDEX).Checked = True
    MOVE_TYPE = INDEX
End Sub
