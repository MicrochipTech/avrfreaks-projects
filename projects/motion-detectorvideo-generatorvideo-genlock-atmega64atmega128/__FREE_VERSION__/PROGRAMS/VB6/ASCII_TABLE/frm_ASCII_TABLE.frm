VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Object = "{3B7C8863-D78F-101B-B9B5-04021C009402}#1.2#0"; "RICHTX32.OCX"
Begin VB.Form frm_ASCII_TABLE 
   BorderStyle     =   5  'Sizable ToolWindow
   Caption         =   "Generate inc file with ASCII table for AVRStudio 4.0"
   ClientHeight    =   7470
   ClientLeft      =   60
   ClientTop       =   360
   ClientWidth     =   10995
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
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   7470
   ScaleWidth      =   10995
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmd_SAVE 
      Caption         =   "&Save to File"
      Height          =   285
      Left            =   1680
      TabIndex        =   2
      Top             =   60
      Width           =   1605
   End
   Begin RichTextLib.RichTextBox rich_CODE 
      Height          =   7065
      Left            =   60
      TabIndex        =   1
      Top             =   390
      Width           =   10845
      _ExtentX        =   19129
      _ExtentY        =   12462
      _Version        =   393217
      ScrollBars      =   2
      TextRTF         =   $"frm_ASCII_TABLE.frx":0000
   End
   Begin MSComDlg.CommonDialog CommonDialog 
      Left            =   3150
      Top             =   150
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.CommandButton cmd_GENERATE 
      Caption         =   "&Generate"
      Height          =   285
      Left            =   60
      TabIndex        =   0
      Top             =   60
      Width           =   1605
   End
End
Attribute VB_Name = "frm_ASCII_TABLE"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Command1_Click()

End Sub

Private Sub cmd_GENERATE_Click()
'+----------------------------------------------------------+
'| GENERATE ASCII TABLE FOR AVR STUDIO 4.0 ASSEMBLER CODE   |
'+----------------------------------------------------------+
Dim s_out           As String
Dim s_bits(1 To 8)  As String
Dim s_dots(1 To 8)  As String
Dim s_i             As Integer
Dim a_c             As Integer
Dim i               As Integer
Dim j               As Integer
Dim k               As Integer
Dim b               As Byte
Dim F               As Integer

    Screen.MousePointer = vbHourglass
    'get free file and open it
    F = FreeFile
    Open App.Path + "\MSXALT1.ROM" For Random As #F Len = Len(b)
    'read MSX ASCII table area
    For i = &H1BBF To &H1BBF + 2047 Step 8
        'one char each time
        s_i = 0
        For j = i To i + 7
            'get a byte
            Get #F, j + 1, b
            s_i = s_i + 1
            s_bits(s_i) = ""
            s_dots(s_i) = ""
            For k = 7 To 0 Step -1
                If (b And 2 ^ k) = 0 Then
                    s_bits(s_i) = s_bits(s_i) + "0"
                    s_dots(s_i) = s_dots(s_i) + " "
                Else
                    s_bits(s_i) = s_bits(s_i) + "1"
                    s_dots(s_i) = s_dots(s_i) + "*"
                End If
            Next
        Next
        'format in text box
        s_out = s_out + vbTab + ".DB 0b" + s_bits(1) + ",0b" + s_bits(2) + vbTab + ";" + s_dots(1) + " ASCII(" + Trim$(Str$(a_c)) + ",0x" + Right$("00" + Hex$(a_c), 2) + ")" + vbCrLf
        s_out = s_out + vbTab + ".DB 0b" + s_bits(3) + ",0b" + s_bits(4) + vbTab + ";" + s_dots(2) + vbCrLf
        s_out = s_out + vbTab + ".DB 0b" + s_bits(5) + ",0b" + s_bits(6) + vbTab + ";" + s_dots(3) + vbCrLf
        s_out = s_out + vbTab + ".DB 0b" + s_bits(7) + ",0b" + s_bits(8) + vbTab + ";" + s_dots(4) + vbCrLf
        s_out = s_out + vbTab + vbTab + vbTab + vbTab + vbTab + ";" + s_dots(5) + vbCrLf
        s_out = s_out + vbTab + vbTab + vbTab + vbTab + vbTab + ";" + s_dots(6) + vbCrLf
        s_out = s_out + vbTab + vbTab + vbTab + vbTab + vbTab + ";" + s_dots(7) + vbCrLf
        s_out = s_out + vbTab + vbTab + vbTab + vbTab + vbTab + ";" + s_dots(8) + vbCrLf
        s_out = s_out + ";" + String(80, "-") + vbCrLf
        a_c = a_c + 1
    Next
    'put into text box
    rich_CODE.Text = "+--------------------------+" + vbCrLf + _
                    "| ASCII TABLE CODES 0..255 |" + vbCrLf + _
                    "+--------------------------+" + vbCrLf + vbCrLf + _
                    "_VIDEO_OUT_CHARACTERS:" + vbCrLf + vbCrLf + _
                    s_out
    Close #F
    Screen.MousePointer = vbArrow
End Sub

Private Sub cmd_SAVE_Click()
'+----------------------+
'| SELECT FILE TO SAVE  |
'+----------------------+
Dim r As Integer
Dim F As Integer

    CommonDialog.CancelError = True
    
    On Error GoTo SAVE_ERROR
    
    CommonDialog.Filter = "*.INC|*.INC|*.ASM|*.ASM"
    CommonDialog.ShowSave
    
    'see if already exits
    If Dir(CommonDialog.FileName) <> "" Then
        Beep
        r = MsgBox("File already exist, Overwrite?", vbOKCancel, "Warning!!!")
        If r = vbCancel Then Exit Sub
    End If
    'write file
    F = FreeFile
    Open CommonDialog.FileName For Output As #F
    Print #F, rich_CODE.Text
    Close #F
    Exit Sub
SAVE_ERROR:
    Resume SAVE_ERROR_EXIT
SAVE_ERROR_EXIT:
End Sub
