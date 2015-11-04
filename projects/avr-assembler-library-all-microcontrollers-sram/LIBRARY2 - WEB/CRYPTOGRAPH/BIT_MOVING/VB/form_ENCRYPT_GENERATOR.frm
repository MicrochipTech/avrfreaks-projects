VERSION 5.00
Begin VB.Form form_ENCRYPT_GENERATOR 
   Caption         =   "Bit Moving Encrypt Generador"
   ClientHeight    =   5025
   ClientLeft      =   60
   ClientTop       =   510
   ClientWidth     =   12510
   LinkTopic       =   "Form1"
   ScaleHeight     =   5025
   ScaleWidth      =   12510
   StartUpPosition =   3  'Windows Default
   Begin VB.TextBox txt_RESULT 
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   12
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   4815
      Left            =   1680
      MultiLine       =   -1  'True
      ScrollBars      =   2  'Vertical
      TabIndex        =   1
      Top             =   120
      Width           =   10695
   End
   Begin VB.CommandButton cmd_GENERATE 
      Caption         =   "&Generate"
      Height          =   615
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   1335
   End
End
Attribute VB_Name = "form_ENCRYPT_GENERATOR"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Command1_Click()

End Sub

Private Sub cmd_GENERATE_Click()
Dim s(0 To 7)   As Byte
Dim d(0 To 7)   As Byte
Dim i           As Integer
Dim j           As Integer
Dim sOut        As String
Dim sV          As String

    'ENCRYPT
    s(0) = 0: s(1) = 1: s(2) = 2: s(3) = 3: s(4) = 4: s(5) = 5: s(6) = 6: s(7) = 7
        
    sOut = ";ENCRYPTING--------------------------------------------------" + vbCrLf
    For j = 1 To 10
        'encrypt
        d(6) = s(0)
        d(7) = s(1)
        d(3) = s(2)
        d(0) = s(3)
        d(2) = s(4)
        d(1) = s(5)
        d(4) = s(6)
        d(5) = s(7)
        For i = 0 To 7
            s(i) = d(i)
        Next
        'print
        sOut = sOut + ";>--------------------------------<" + vbCrLf
        sOut = sOut + ";| moving encrypting #"
        sOut = sOut + Format(j - 1, "@@") + " 76543210 |" + vbCrLf
        sOut = sOut + ";|                       "
        sV = ""
        For i = 7 To 0 Step -1
            sOut = sOut + Trim(s(i))
            sV = sV + Trim(s(i))
        Next
        sOut = sOut + " |" + vbCrLf
        sOut = sOut + ";>--------------------------------<" + vbCrLf
        sOut = sOut + "_CRYPT0_BIT_MOVING_ENCRYPT_" + Trim(j - 1) + ":" + vbCrLf
        For i = 0 To 7
            sOut = sOut + vbTab + "bst" + vbTab + "Acc," + Trim(i) + vbCrLf
            sOut = sOut + vbTab + "bld" + vbTab + "AccH," + Trim(8 - InStr(1, sV, Trim(i))) + vbCrLf
        Next
        sOut = sOut + vbTab + "rjmp" + vbTab + "_CRYPTO_BIT_MOVING_ENCRYPT_EXIT"
        sOut = sOut + vbCrLf
    Next
    
    'DECRYPTING
    s(0) = 0: s(1) = 1: s(2) = 2: s(3) = 3: s(4) = 4: s(5) = 5: s(6) = 6: s(7) = 7
        
    sOut = sOut + ";DECRYPTING--------------------------------------------------" + vbCrLf
    For j = 1 To 10
        'encrypt
        d(6) = s(0)
        d(7) = s(1)
        d(3) = s(2)
        d(0) = s(3)
        d(2) = s(4)
        d(1) = s(5)
        d(4) = s(6)
        d(5) = s(7)
        For i = 0 To 7
            s(i) = d(i)
        Next
        'print
        sOut = sOut + ";>--------------------------------<" + vbCrLf
        sOut = sOut + ";| moving decrypting #"
        sOut = sOut + Format(j - 1, "@@") + " 76543210 |" + vbCrLf
        sOut = sOut + ";|                       "
        sV = ""
        For i = 7 To 0 Step -1
            sOut = sOut + Trim(s(i))
            sV = sV + Trim(s(i))
        Next
        sOut = sOut + " |" + vbCrLf
        sOut = sOut + ";>--------------------------------<" + vbCrLf
        sOut = sOut + "_CRYPT0_BIT_MOVING_DECRYPT_" + Trim(j - 1) + ":" + vbCrLf
        For i = 0 To 7
            sOut = sOut + vbTab + "bst" + vbTab + "Acc," + Trim(8 - InStr(1, sV, Trim(i))) + vbCrLf
            sOut = sOut + vbTab + "bld" + vbTab + "AccH," + Trim(i) + vbCrLf
        Next
        sOut = sOut + vbTab + "rjmp" + vbTab + "_CRYPTO_BIT_MOVING_DECRYPT_EXIT"
        sOut = sOut + vbCrLf
    Next

    txt_RESULT.Text = sOut
End Sub
