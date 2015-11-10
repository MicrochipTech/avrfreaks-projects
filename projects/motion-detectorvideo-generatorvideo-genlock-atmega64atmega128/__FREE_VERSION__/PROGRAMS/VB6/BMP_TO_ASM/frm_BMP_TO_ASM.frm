VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "comdlg32.ocx"
Begin VB.Form frm_BMP_TO_ASM 
   Caption         =   "BMP To ASM (Atmel Avr Studio 4.0)"
   ClientHeight    =   6960
   ClientLeft      =   165
   ClientTop       =   1035
   ClientWidth     =   14520
   Icon            =   "frm_BMP_TO_ASM.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   6960
   ScaleWidth      =   14520
   StartUpPosition =   3  'Windows Default
   Begin VB.TextBox txt_CODE 
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   6765
      Left            =   1890
      MultiLine       =   -1  'True
      ScrollBars      =   3  'Both
      TabIndex        =   0
      Top             =   60
      Width           =   12465
   End
   Begin MSComDlg.CommonDialog CommonDialog 
      Left            =   240
      Top             =   3780
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.Image img_LOADED 
      Appearance      =   0  'Flat
      BorderStyle     =   1  'Fixed Single
      Height          =   1635
      Left            =   90
      Stretch         =   -1  'True
      Top             =   90
      Width           =   1755
   End
   Begin VB.Menu mnu_ARQUIVO 
      Caption         =   "&Arquivo"
      Begin VB.Menu mnu_ARQUIVO_LOAD_IMAGE 
         Caption         =   "&Load Image"
      End
      Begin VB.Menu mnu_ARQUIVO_SAVE_ASM 
         Caption         =   "&Convert to ASM 2 Levels old style"
      End
      Begin VB.Menu mnu_ARQUIVO_SAVE_16_LEVELS 
         Caption         =   "&Convert to ASM 16 Levels old style"
      End
      Begin VB.Menu mnu_ARQUIVO_SAVE_2_LEVELS_NEW 
         Caption         =   "&Convert to ASM 2 Levels new style"
      End
      Begin VB.Menu mnu_ARQUIVO_SAVE_16_LEVELS_NEW 
         Caption         =   "&Convert to ASM 16 levels new style"
      End
      Begin VB.Menu mnu_H1 
         Caption         =   "-"
      End
      Begin VB.Menu mnu_ARQUIVO_SAIR 
         Caption         =   "&Sair"
      End
   End
End
Attribute VB_Name = "frm_BMP_TO_ASM"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'+------------------------------------------------------------------------------------------+
'| CONVERSOR DE IMAGENS BMP PARA BMP FORMATADO PARA ASSEMBLER TO AVR STUDIO 4.0 DA ATMEL    |
'| BY JOAO DARTAGNAN ANTUNES OLIVEIRA                                                       |
'| DATA: 9/12/2006                                                                          |
'+------------------------------------------------------------------------------------------+

'+----------------------+
'| MY GLOBAL OBJECTS    |
'+----------------------+
Dim FDIR As New cls_DIR_ENTRIES
Dim im As New cls_IMAGE_FRAME

'+----------------------+
'| MY GLOBAL VARIABLES  |
'+----------------------+

Dim sprite_I    As Long

Private Sub Form_Load()
'+--------------------------+
'| ADJUST CONTROLS FIRST    |
'+--------------------------+

    ADJUST_CONTROLS
    
End Sub

Private Sub Form_Resize()
'+----------------------------------+
'| ADJUST CONTROLS IF FORM RESIZED  |
'+----------------------------------+

    ADJUST_CONTROLS
    
End Sub

Private Sub Form_Unload(Cancel As Integer)
    End
End Sub

Private Sub mnu_ARQUIVO_LOAD_IMAGE_Click()
'+--------------------------+
'| LOAD IMAGE TO CONVERT    |
'+--------------------------+

    CommonDialog.CancelError = True
    
    On Error GoTo LOAD_IMAGE_ERROR
    
    CommonDialog.DialogTitle = "Abrir arquivo de image"
    CommonDialog.Filter = "*.BMP|*.BMP|*.GIF|*.GIF|*.WMF|*.WMF|*.JPG|*.JPG"
    CommonDialog.ShowOpen
    FDIR.PATH_NAME = CommonDialog.FileName
    img_LOADED.Picture = LoadPicture(FDIR.PATH_NAME)
    Set im = Nothing
    sprite_I = im.SPRITE_ADD_FROM_DISK(FDIR.PATH_NAME)
    
    Exit Sub
LOAD_IMAGE_ERROR:
    Resume LOAD_IMAGE_ERROR_EXIT
LOAD_IMAGE_ERROR_EXIT:
End Sub

Private Sub mnu_ARQUIVO_SAIR_Click()
    End
End Sub

Sub ADJUST_CONTROLS()
'+--------------------------+
'| ADJUST CONTROLS ON FORM  |
'+--------------------------+

    If Me.WindowState = vbMinimized Then Exit Sub
    
    'image box
    img_LOADED.Left = 0
    img_LOADED.Top = 0
    'text box
    txt_CODE.Left = img_LOADED.Left + img_LOADED.Width
    txt_CODE.Top = 0
    txt_CODE.Width = Me.ScaleWidth - img_LOADED.Width
    txt_CODE.Height = Me.ScaleHeight
End Sub

Private Sub mnu_ARQUIVO_SAVE_16_LEVELS_Click()
Dim X As Integer
Dim Y As Integer
Dim l As Long
Dim bh As Byte
Dim bl As Byte
Dim K As Integer
Dim s As String

    pic_TARGET.Cls
    im.FRAME_LOAD_IMAGE CommonDialog.FileName
    s = ""
    For Y = 0 To im.FRAME_HEIGHT - 1
        s = s + ".db "
        For X = 0 To im.FRAME_WIDTH - 1 Step 2
            bh = im.PIXEL_GET_LUMINANCE(X, im.FRAME_HEIGHT - Y) And &HF0
            bl = im.PIXEL_GET_LUMINANCE(X + 1, im.FRAME_HEIGHT - Y) And &HF0
            pic_TARGET.PSet (X, Y), RGB(bh, bh, bh)
            pic_TARGET.PSet (X + 1, Y), RGB(bl, bl, bl)
            s = s + "0x" + Right$("00" + Hex$(bh Or bl \ 16), 2) + ","
        Next
        s = Left(s, Len(s) - 1) + vbCrLf
    Next
    txt_CODE.Text = s
End Sub

Private Sub mnu_ARQUIVO_SAVE_16_LEVELS_NEW_Click()
'+-------------------------------------------------------------------------+
'|                                                                         |
'| PROCESS AND SAVE ASM AVR STUDIO 4.0 ASSEMBLER FORMAT 16 COLORS NEW STYLE|
'|                                                                         |
'+-------------------------------------------------------------------------+
Dim w       As Integer
Dim X       As Integer
Dim Y       As Integer
Dim f       As Integer
Dim s       As String
Dim so      As String
Dim sLu     As String
Dim T       As Variant
Dim r       As Byte
Dim b       As Byte
Dim g       As Byte
Dim l       As Integer
Dim bb      As Byte
Dim mask    As Byte
Dim rs      As Integer
Dim sc      As Integer

Dim Width_Pixels    As Long
Dim Width_Bytes     As Long
Dim Height_Pixels   As Long
Dim Color_Bits      As Integer
Dim SizeBytes       As Long
Dim b_UPDOWN        As Boolean

    sLu = "@X#x=+:. "
    If im.SPRITE_COUNT < 1 Then
        txt_CODE.Text = "No Image Loaded"
        Exit Sub
    End If
    Screen.MousePointer = vbHourglass
    
    Width_Pixels = im.SPRITE_WIDTH(sprite_I)
    Height_Pixels = im.SPRITE_HEIGHT(sprite_I)
    Width_Bytes = IIf((im.SPRITE_WIDTH(sprite_I) Mod 2) = 0, im.SPRITE_WIDTH(sprite_I) \ 2, im.SPRITE_WIDTH(sprite_I) \ 2 + 1)
    Color_Bits = 4
    SizeBytes = Width_Bytes * Height_Pixels
    so = ""
    so = so + ";+" + String(Len(FDIR.Path) + 14, "-") + vbCrLf
    so = so + ";| Source" + vbCrLf
    so = so + ";|     Path : " + FDIR.Path + vbCrLf
    so = so + ";|     File : " + FDIR.PATH_FILE_NAME + "." + FDIR.PATH_EXTENSION_NAME + vbCrLf
    so = so + ";|" + vbCrLf
    so = so + ";| Destiny" + vbCrLf
    so = so + ";|     Path : " + FDIR.Path + vbCrLf
    so = so + ";|     File : " + "BMP_" + FDIR.PATH_FILE_NAME + ".INC" + vbCrLf
    so = so + ";|" + vbCrLf
    so = so + ";| Destiny Image Properties" + vbCrLf
    so = so + ";|     Size   : " + Format(Trim(SizeBytes), "@@@@@") + " Bytes" + vbCrLf
    so = so + ";|     Width  : " + Format(Trim(Width_Pixels), "@@@@@") + " Pixels" + vbCrLf
    so = so + ";|     Height : " + Format(Trim(Height_Pixels), "@@@@@") + " Pixels" + vbCrLf
    so = so + ";|     WidthBt: " + Format(Trim(Width_Bytes), "@@@@@") + " Bytes" + vbCrLf
    so = so + ";|     Colors : " + Format(Trim(2 ^ Color_Bits), "@@@@@") + " Levels (" + Trim(Color_Bits) + " bit)" + vbCrLf
    so = so + ";+" + String(Len(FDIR.Path) + 14, "-") + vbCrLf + vbCrLf
    so = so + ";( IMAGE CONTENTS )" + vbCrLf + vbCrLf
    'IMAGE
    b_UPDOWN = True
    GoSub AXIS_X
    For Y = 0 To im.SPRITE_HEIGHT(sprite_I) - 1
        so = so + ";" + Format(Y, "@@@@") + "-"
        s = ""
        'For X = 0 To im.SPRITE_WIDTH(sprite_I) Step 8
        For w = 0 To Width_Bytes * 2 - 1
            'get rgb value
            im.SPRITE_PIXEL_GET_RGB sprite_I, w, im.SPRITE_HEIGHT(sprite_I) - Y - 1, r, g, b
            'get luminance
            l = (CInt(r) * 33 + CInt(g) * 56 + CInt(b) * 11) / 100
            l = Int((l * Len(sLu) / 256))
            s = s + Mid$(sLu, l + 1, 1) + Mid$(sLu, l + 1, 1)
        Next
        so = so + s + "-" + Format(Y, "@@@@") + vbCrLf
    Next
    b_UPDOWN = False
    GoSub AXIS_X
    so = so + vbCrLf
    so = so + ";( IMAGE DATA )" + vbCrLf + vbCrLf
    so = so + "BMP_" + FDIR.PATH_FILE_NAME + ":" + vbCrLf + vbCrLf
    so = so + vbTab + ";+------------+" + vbCrLf
    so = so + vbTab + ";| BMP Header |" + vbCrLf
    so = so + vbTab + ";+------------+" + vbCrLf
    so = so + vbTab + ".DW  " + Format(Trim(Color_Bits), "@@@@@") + " ;Bits Color " + Trim(Color_Bits) + " bit" + vbCrLf
    so = so + vbTab + ".DW  " + Format(Trim(Width_Pixels), "@@@@@") + " ;Width Pixels" + vbCrLf
    so = so + vbTab + ".DW  " + Format(Trim(Height_Pixels), "@@@@@") + " ;Height Pixels" + vbCrLf
    so = so + vbTab + ".DW  " + Format(Trim(Width_Bytes), "@@@@@") + " ;Width Bytes" + vbCrLf
    so = so + vbTab + ".DW  " + Format(Trim(SizeBytes), "@@@@@") + " ;Size Bytes" + vbCrLf
    so = so + vbTab + ";+------------+" + vbCrLf
    so = so + vbTab + ";|  BMP Data  |" + vbCrLf
    so = so + vbTab + ";+------------+" + vbCrLf
    'DBs and DWs
    s = ""
    For Y = 0 To im.SPRITE_HEIGHT(sprite_I) - 1
        'For X = 0 To im.SPRITE_WIDTH(sprite_I) - 1 Step 8
        For w = 0 To Width_Bytes - 1
            X = w * 2
            mask = &HF0
            bb = 0
            For K = X To X + 1
                'get rgb value
                im.SPRITE_PIXEL_GET_RGB sprite_I, K, im.SPRITE_HEIGHT(sprite_I) - Y - 1, r, g, b
                'get luminance
                l = (CInt(r) * 33 + CInt(g) * 56 + CInt(b) * 11) / 100
                l = (l * 15) / 256
                If mask = &HF0 Then
                    bb = l * 16
                Else
                    bb = bb + l
                End If
                mask = mask \ 4
            Next
            s = s + "0x" + Right$("00" + Hex$(bb), 2) + ","
            sc = sc + 1
            If sc > 15 Then
                sc = 0
                so = so + vbTab + ".DB " + Left(s, Len(s) - 1) + vbCrLf
                s = ""
            End If
        Next
    Next
    If s <> "" Then
        If (sc And 1) = 0 Then
            'last line even case
            so = so + vbTab + ".DB " + Left(s, Len(s) - 1) + vbCrLf
        Else
            'last line odd case
            so = so + vbTab + ".DB " + Left(s, Len(s) - 1) + ",0x00" + vbCrLf
        End If
    End If
    
    Close #f
    txt_CODE.Text = so
    s = FDIR.Path + "BMP_" + FDIR.PATH_FILE_NAME + ".INC"
    If Dir(s) <> "" Then
        'update with question
        Beep
        rs = MsgBox("File " + FDIR.PATH_FILE_NAME + ".ASM already exist, overwrite ?", vbYesNo, "Warning!!!")
        If rs = vbYes Then
            f = FreeFile
            Open s For Output As #f
            Print #f, txt_CODE.Text
            Close #f
        End If
    Else
        'update without question
        f = FreeFile
        Open s For Output As #f
        Print #f, txt_CODE.Text
        Close #f
    End If
    Screen.MousePointer = vbArrow
    Exit Sub
'+----------------------+
'| DRAW AXIS X VALUES   |
'+----------------------+
AXIS_X:
    If Not (b_UPDOWN) Then
        so = so + ";" + Space(5)
        For X = 0 To im.SPRITE_WIDTH(sprite_I) - 1
            If (X Mod 10) = 0 Then
                so = so + "| "
            Else
                so = so + "  "
            End If
        Next
        so = so + vbCrLf
    End If
    so = so + ";" + Space(5)
    For X = 0 To im.SPRITE_WIDTH(sprite_I) - 1
        If (X Mod 100) = 0 Then
            If X = 0 Then
                so = so + "  "
            Else
                so = so + Mid(Right$("0000" + Trim(X), 4), 2, 1) + " "
            End If
        Else
            so = so + "  "
        End If
    Next
    so = so + vbCrLf
    so = so + ";" + Space(5)
    For X = 0 To im.SPRITE_WIDTH(sprite_I) - 1
        If (X Mod 10) = 0 Then
            If X = 0 Then
                so = so + "  "
            Else
                so = so + Mid(Right$("0000" + Trim(X), 4), 3, 1) + " "
            End If
        Else
            so = so + "  "
        End If
    Next
    so = so + vbCrLf
    so = so + ";" + Space(5)
    For X = 0 To im.SPRITE_WIDTH(sprite_I) - 1
        so = so + Mid(Right$("0000" + Trim(X), 4), 4, 1) + " "
    Next
    so = so + vbCrLf
    If b_UPDOWN Then
        so = so + ";" + Space(5)
        For X = 0 To im.SPRITE_WIDTH(sprite_I) - 1
            If (X Mod 10) = 0 Then
                so = so + "| "
            Else
                so = so + "  "
            End If
        Next
        so = so + vbCrLf
    End If
    Return

End Sub

Private Sub mnu_ARQUIVO_SAVE_2_LEVELS_NEW_Click()
'+-------------------------------------------------------------------------+
'|                                                                         |
'| PROCESS AND SAVE ASM AVR STUDIO 4.0 ASSEMBLER FORMAT 2 COLORS NEW STYLE |
'|                                                                         |
'+-------------------------------------------------------------------------+
Dim w       As Integer
Dim X       As Integer
Dim Y       As Integer
Dim f       As Integer
Dim s       As String
Dim so      As String
Dim T       As Variant
Dim r       As Byte
Dim b       As Byte
Dim g       As Byte
Dim l       As Integer
Dim bb      As Byte
Dim mask    As Byte
Dim rs      As Integer
Dim sc      As Integer

Dim Width_Pixels    As Long
Dim Width_Bytes     As Long
Dim Height_Pixels   As Long
Dim Color_Bits      As Integer
Dim SizeBytes       As Long
Dim b_UPDOWN        As Boolean

    If im.SPRITE_COUNT < 1 Then
        txt_CODE.Text = "No Image Loaded"
        Exit Sub
    End If
    Screen.MousePointer = vbHourglass
    
    Width_Pixels = im.SPRITE_WIDTH(sprite_I)
    Height_Pixels = im.SPRITE_HEIGHT(sprite_I)
    Width_Bytes = IIf((im.SPRITE_WIDTH(sprite_I) Mod 8) = 0, im.SPRITE_WIDTH(sprite_I) \ 8, im.SPRITE_WIDTH(sprite_I) \ 8 + 1)
    Color_Bits = 1
    SizeBytes = Width_Bytes * Height_Pixels
    so = ""
    so = so + ";+" + String(Len(FDIR.Path) + 14, "-") + vbCrLf
    so = so + ";| Source" + vbCrLf
    so = so + ";|     Path : " + FDIR.Path + vbCrLf
    so = so + ";|     File : " + FDIR.PATH_FILE_NAME + "." + FDIR.PATH_EXTENSION_NAME + vbCrLf
    so = so + ";|" + vbCrLf
    so = so + ";| Destiny" + vbCrLf
    so = so + ";|     Path : " + FDIR.Path + vbCrLf
    so = so + ";|     File : " + "BMP_" + FDIR.PATH_FILE_NAME + ".INC" + vbCrLf
    so = so + ";|" + vbCrLf
    so = so + ";| Destiny Image Properties" + vbCrLf
    so = so + ";|     Size   : " + Format(Trim(SizeBytes), "@@@@@") + " Bytes" + vbCrLf
    so = so + ";|     Width  : " + Format(Trim(Width_Pixels), "@@@@@") + " Pixels" + vbCrLf
    so = so + ";|     Height : " + Format(Trim(Height_Pixels), "@@@@@") + " Pixels" + vbCrLf
    so = so + ";|     WidthB : " + Format(Trim(Width_Bytes), "@@@@@") + " Bytes" + vbCrLf
    so = so + ";|     Colors : " + Format(Trim(2 ^ Color_Bits), "@@@@@") + " Levels (" + Trim(Color_Bits) + " bit)" + vbCrLf
    so = so + ";+" + String(Len(FDIR.Path) + 14, "-") + vbCrLf + vbCrLf
    so = so + ";( IMAGE CONTENTS )" + vbCrLf + vbCrLf
    'IMAGE
    b_UPDOWN = True
    GoSub AXIS_X
    For Y = 0 To im.SPRITE_HEIGHT(sprite_I) - 1
        so = so + ";" + Format(Y, "@@@@") + "-"
        s = ""
        'For X = 0 To im.SPRITE_WIDTH(sprite_I) Step 8
        For w = 0 To Width_Bytes - 1
            X = w * 8
            For K = X To X + 7
                'get rgb value
                im.SPRITE_PIXEL_GET_RGB sprite_I, K, im.SPRITE_HEIGHT(sprite_I) - Y - 1, r, g, b
                'get luminance
                l = (CInt(r) * 33 + CInt(g) * 56 + CInt(b) * 11) / 100
                If l < 128 And K < Width_Pixels Then
                    s = s + "#"
                Else
                    s = s + " "
                End If
            Next
        Next
        so = so + s + "-" + Format(Y, "@@@@") + vbCrLf
    Next
    b_UPDOWN = False
    GoSub AXIS_X
    so = so + vbCrLf
    so = so + ";( IMAGE DATA )" + vbCrLf + vbCrLf
    so = so + "BMP_" + FDIR.PATH_FILE_NAME + ":" + vbCrLf + vbCrLf
    so = so + vbTab + ";+------------+" + vbCrLf
    so = so + vbTab + ";| BMP Header |" + vbCrLf
    so = so + vbTab + ";+------------+" + vbCrLf
    so = so + vbTab + ".DW  " + Format(Trim(Color_Bits), "@@@@@") + " ;Bits Color " + Trim(Color_Bits) + " bit" + vbCrLf
    so = so + vbTab + ".DW  " + Format(Trim(Width_Pixels), "@@@@@") + " ;Width Pixels" + vbCrLf
    so = so + vbTab + ".DW  " + Format(Trim(Height_Pixels), "@@@@@") + " ;Height Pixels" + vbCrLf
    so = so + vbTab + ".DW  " + Format(Trim(Width_Bytes), "@@@@@") + " ;Width Bytes" + vbCrLf
    so = so + vbTab + ".DW  " + Format(Trim(SizeBytes), "@@@@@") + " ;Size Bytes" + vbCrLf
    so = so + vbTab + ";+------------+" + vbCrLf
    so = so + vbTab + ";|  BMP Data  |" + vbCrLf
    so = so + vbTab + ";+------------+" + vbCrLf
    'DBs and DWs
    s = ""
    For Y = 0 To im.SPRITE_HEIGHT(sprite_I) - 1
        'For X = 0 To im.SPRITE_WIDTH(sprite_I) - 1 Step 8
        For w = 0 To Width_Bytes - 1
            X = w * 8
            mask = &H80
            bb = 0
            For K = X To X + 7
                'get rgb value
                im.SPRITE_PIXEL_GET_RGB sprite_I, K, im.SPRITE_HEIGHT(sprite_I) - Y - 1, r, g, b
                'get luminance
                l = (CInt(r) * 33 + CInt(g) * 56 + CInt(b) * 11) / 100
                If l < 128 Then
                    mask = mask \ 2
                Else
                    bb = bb Or mask
                    mask = mask \ 2
                End If
            Next
            s = s + "0x" + Right$("00" + Hex$(bb), 2) + ","
            sc = sc + 1
            If sc > 15 Then
                sc = 0
                so = so + vbTab + ".DB " + Left(s, Len(s) - 1) + vbCrLf
                s = ""
            End If
        Next
    Next
    If s <> "" Then
        If (sc And 1) = 0 Then
            'last line even case
            so = so + vbTab + ".DB " + Left(s, Len(s) - 1) + vbCrLf
        Else
            'last line odd case
            so = so + vbTab + ".DB " + Left(s, Len(s) - 1) + ",0x00" + vbCrLf
        End If
    End If
    
    Close #f
    txt_CODE.Text = so
    s = FDIR.Path + "BMP_" + FDIR.PATH_FILE_NAME + ".INC"
    If Dir(s) <> "" Then
        'update with question
        Beep
        rs = MsgBox("File " + FDIR.PATH_FILE_NAME + ".INC already exist, overwrite ?", vbYesNo, "Warning!!!")
        If rs = vbYes Then
            f = FreeFile
            Open s For Output As #f
            Print #f, txt_CODE.Text
            Close #f
        End If
    Else
        'update without question
        f = FreeFile
        Open s For Output As #f
        Print #f, txt_CODE.Text
        Close #f
    End If
    Screen.MousePointer = vbArrow
    Exit Sub
'+----------------------+
'| DRAW AXIS X VALUES   |
'+----------------------+
AXIS_X:
    If Not (b_UPDOWN) Then
        so = so + ";" + Space(5)
        For X = 0 To im.SPRITE_WIDTH(sprite_I) - 1
            If (X Mod 10) = 0 Then
                so = so + "|"
            Else
                so = so + " "
            End If
        Next
        so = so + vbCrLf
    End If
    so = so + ";" + Space(5)
    For X = 0 To im.SPRITE_WIDTH(sprite_I) - 1
        If (X Mod 100) = 0 Then
            If X = 0 Then
                so = so + " "
            Else
                so = so + Mid(Right$("0000" + Trim(X), 4), 2, 1)
            End If
        Else
            so = so + " "
        End If
    Next
    so = so + vbCrLf
    so = so + ";" + Space(5)
    For X = 0 To im.SPRITE_WIDTH(sprite_I) - 1
        If (X Mod 10) = 0 Then
            If X = 0 Then
                so = so + " "
            Else
                so = so + Mid(Right$("0000" + Trim(X), 4), 3, 1)
            End If
        Else
            so = so + " "
        End If
    Next
    so = so + vbCrLf
    so = so + ";" + Space(5)
    For X = 0 To im.SPRITE_WIDTH(sprite_I) - 1
        so = so + Mid(Right$("0000" + Trim(X), 4), 4, 1)
    Next
    so = so + vbCrLf
    If b_UPDOWN Then
        so = so + ";" + Space(5)
        For X = 0 To im.SPRITE_WIDTH(sprite_I) - 1
            If (X Mod 10) = 0 Then
                so = so + "|"
            Else
                so = so + " "
            End If
        Next
        so = so + vbCrLf
    End If
    Return
End Sub

Private Sub mnu_ARQUIVO_SAVE_ASM_Click()
'+------------------------------------------------------+
'| PROCESS AND SAVE ASM AVR STUDIO 4.0 ASSEMBLER FORMAT |
'+------------------------------------------------------+
Dim X       As Integer
Dim Y       As Integer
Dim f       As Integer
Dim s       As String
Dim so      As String
Dim T       As Variant
Dim r       As Byte
Dim b       As Byte
Dim g       As Byte
Dim l       As Integer
Dim bb      As Byte
Dim mask    As Byte
Dim rs      As Integer

    If im.SPRITE_COUNT < 1 Then
        txt_CODE.Text = "No Image Loaded"
        Exit Sub
    End If
    Screen.MousePointer = vbHourglass
    so = ""
    so = so + ";+" + String(Len(FDIR.Path) + 14, "-") + vbCrLf
    so = so + ";| Source" + vbCrLf
    so = so + ";|     Path : " + FDIR.Path + vbCrLf
    so = so + ";|     File : " + FDIR.PATH_FILE_NAME + "." + FDIR.PATH_EXTENSION_NAME + vbCrLf
    so = so + ";|" + vbCrLf
    so = so + ";| Destiny" + vbCrLf
    so = so + ";|     Path : " + FDIR.Path + vbCrLf
    so = so + ";|     File : " + FDIR.PATH_FILE_NAME + ".ASM" + vbCrLf
    so = so + ";|" + vbCrLf
    so = so + ";| Destiny Image Properties" + vbCrLf
    so = so + ";|     Width  : " + Format(Trim(im.SPRITE_WIDTH(sprite_I)), "@@@@") + " Pixels" + vbCrLf
    so = so + ";|     Height : " + Format(Trim(im.SPRITE_HEIGHT(sprite_I)), "@@@@") + " Pixels" + vbCrLf
    so = so + ";|     Width  : " + Format(Trim(Int(im.SPRITE_WIDTH(sprite_I) / 8)), "@@@@") + " Bytes" + vbCrLf
    so = so + ";|     Colors : " + Format(Color_Bits, "@@@@") + " Levels (1 bit)" + vbCrLf
    so = so + ";+" + String(Len(FDIR.Path) + 14, "-") + vbCrLf + vbCrLf
    so = so + ";( IMAGE CONTENTS )" + vbCrLf + vbCrLf
    'IMAGE
    For Y = 0 To im.SPRITE_HEIGHT(sprite_I) - 1
        so = so + ";"
        s = ""
        For X = 0 To im.SPRITE_WIDTH(sprite_I) - 1 Step 8
            For K = X To X + 7
                'get rgb value
                im.SPRITE_PIXEL_GET_RGB sprite_I, K, im.SPRITE_HEIGHT(sprite_I) - Y - 1, r, g, b
                'get luminance
                l = (CInt(r) * 33 + CInt(g) * 56 + CInt(b) * 11) / 100
                If l < 128 Then
                    s = s + "#"
                Else
                    s = s + " "
                End If
            Next
        Next
        so = so + Left(s, Len(s) - 1) + vbCrLf
    Next
    so = so + vbCrLf
    so = so + ";( IMAGE DATA )" + vbCrLf + vbCrLf
    so = so + "BMP_" + FDIR.PATH_FILE_NAME + ":" + vbCrLf + vbCrLf
    so = so + vbTab + ".DW  " + Format(Trim(im.SPRITE_WIDTH(sprite_I) / 8), "@@@@@") + " ;bytes per row" + vbCrLf
    so = so + vbTab + ".DW  " + Format(Trim(im.SPRITE_HEIGHT(sprite_I)), "@@@@@") + " ;rows" + vbCrLf
    'DBs and DWs
    For Y = 0 To im.SPRITE_HEIGHT(sprite_I) - 1
        so = so + vbTab + ".DB "
        s = ""
        For X = 0 To im.SPRITE_WIDTH(sprite_I) - 1 Step 8
            mask = &H80
            bb = 0
            For K = X To X + 7
                'get rgb value
                im.SPRITE_PIXEL_GET_RGB sprite_I, K, im.SPRITE_HEIGHT(sprite_I) - Y - 1, r, g, b
                'get luminance
                l = (CInt(r) * 33 + CInt(g) * 56 + CInt(b) * 11) / 100
                If l < 128 Then
                    mask = mask \ 2
                Else
                    bb = bb Or mask
                    mask = mask \ 2
                End If
            Next
            s = s + "0x" + Right$("00" + Hex$(bb), 2) + ","
        Next
        so = so + Left(s, Len(s) - 1) + vbCrLf
    Next
    Close #f
    txt_CODE.Text = so
    If Dir(FDIR.Path + FDIR.PATH_FILE_NAME + ".ASM") <> "" Then
        Beep
        rs = MsgBox("File " + FDIR.PATH_FILE_NAME + ".ASM already exist, overwrite ?", vbYesNo, "Warning!!!")
        If rs = vbYes Then
            f = FreeFile
            Open FDIR.Path + FDIR.PATH_FILE_NAME + ".asm" For Output As #f
            Print #f, txt_CODE.Text
            Close #f
        End If
    End If
    Screen.MousePointer = vbArrow
    
End Sub

