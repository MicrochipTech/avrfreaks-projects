VERSION 5.00
Begin VB.Form frm_GENERATE 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Generate Make/Break Codes(from file in code)"
   ClientHeight    =   4725
   ClientLeft      =   150
   ClientTop       =   900
   ClientWidth     =   14775
   BeginProperty Font 
      Name            =   "Courier New"
      Size            =   9.75
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4725
   ScaleWidth      =   14775
   StartUpPosition =   3  'Windows Default
   Begin VB.TextBox txt_OUTPUT 
      Height          =   4575
      Left            =   60
      MultiLine       =   -1  'True
      ScrollBars      =   3  'Both
      TabIndex        =   0
      Top             =   60
      Width           =   14655
   End
   Begin VB.Menu mnu_ARQUIVO 
      Caption         =   "&Arquivo"
      Begin VB.Menu mnu_SAIR 
         Caption         =   "&Sair"
      End
   End
   Begin VB.Menu mnu_GERAR 
      Caption         =   "&Gerar"
   End
End
Attribute VB_Name = "frm_GENERATE"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'+----------------------------------------------+
'| GENERATE MAKE/BREAK CODE FROM LAYOUT CODE    |
'| BY JOAO DARTAGNAN ANTUNES OLIVEIRA           |
'| DATE: August, 4 2007                         |
'| TIME: 16:44                                  |
'+----------------------------------------------+
Option Explicit

Private Sub Form_Unload(Cancel As Integer)
    End
End Sub

Private Sub mnu_GERAR_Click()
'+------------------------------------------------------------------------------+
'|  LE ARQUIVO KEYBOARD_LAYOUT_USA.INC AND GENERATE KEYBOARD_LAYOUT_USA.INC2    |
'+------------------------------------------------------------------------------+
Const f_name_in = "KEYBOARD_LAYOUT_USA.INC"
Const f_name_out = "KEYBOARD_LAYOUT_USA_EXIT.INC"

Dim f_in    As Integer
Dim f_out   As Integer
Dim s_in    As String
Dim s_out   As String
Dim s_pos   As Integer
Dim s_key   As Boolean
Dim V1      As Integer
Dim V2      As Integer

f_in = FreeFile
f_out = FreeFile

txt_OUTPUT.Text = ""
Open App.Path + "\" + f_name_in For Input As #f_in

    Line Input #f_in, s_in
    Do While Not (EOF(f_in))
        
        s_in = RTrim$(s_in)
        If InStr(1, UCase(s_in), "<START>") Then s_key = True
        If InStr(1, UCase(s_in), "<END>") Then s_key = True
        If s_key Then
            If InStr(1, UCase(s_in), "<START>") Or InStr(1, UCase(s_in), "<END>") Then
            Else
                s_pos = InStr(1, UCase(s_in), "0X")
                V1 = Val("&H" + Mid$(s_in, s_pos + 2, 2))
                s_pos = InStr(s_pos + 1, UCase(s_in), "0X")
                V2 = Val("&H" + Mid$(s_in, s_pos + 2, 2))
                txt_OUTPUT.Text = txt_OUTPUT.Text + s_in + _
                                  vbTab + ";0X" + Right$("00" + Hex$(V1), 2) + _
                                  "/0X" + Right$("00" + Hex$(V1 Or &H80), 2) + _
                                  vbTab + vbTab + vbTab + vbTab + _
                                  "0X" + Right$("00" + Hex$(V2), 2) + _
                                  "/0XF0,0X" + Right$("00" + Hex$(V2), 2) + vbCrLf
            End If
        End If
        Line Input #f_in, s_in
    Loop
    
Close #f_in

End Sub
