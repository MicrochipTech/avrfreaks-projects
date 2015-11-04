VERSION 5.00
Object = "{3B7C8863-D78F-101B-B9B5-04021C009402}#1.2#0"; "RICHTX32.OCX"
Begin VB.Form form_GENERATE_FORMATTED_FILE 
   Caption         =   "Generate Formatted Ouput from NMEA.txt doc"
   ClientHeight    =   5250
   ClientLeft      =   60
   ClientTop       =   510
   ClientWidth     =   17685
   LinkTopic       =   "Form1"
   ScaleHeight     =   5250
   ScaleWidth      =   17685
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmd_GEN_FORMATED_FILE 
      Caption         =   "&Generate Formatted File"
      Height          =   495
      Left            =   120
      TabIndex        =   1
      Top             =   240
      Width           =   1335
   End
   Begin RichTextLib.RichTextBox rich_OUTPUT 
      Height          =   4815
      Left            =   1800
      TabIndex        =   0
      Top             =   240
      Width           =   15735
      _ExtentX        =   27755
      _ExtentY        =   8493
      _Version        =   393217
      Enabled         =   -1  'True
      ScrollBars      =   2
      TextRTF         =   $"form_GENERATE_FORMATTED_FILE.frx":0000
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "Courier New"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
   End
End
Attribute VB_Name = "form_GENERATE_FORMATTED_FILE"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub cmd_GEN_FORMATED_FILE_Click()
'+-----------------------------------+
'| GET INPUT TEXT AND FORMATTED IT   |
'+-----------------------------------+
Dim f1       As Integer
Dim f2      As Integer
Dim sIn     As String
Dim sOut    As String

    Me.MousePointer = vbHourglass
    f1 = FreeFile
    Open App.Path + "\NMEA_PROTOCOL.TXT" For Input As #f1
    f2 = FreeFile
    Open App.Path + "\NMEA_PROTOCOL_F.TXT" For Output As #f2
    Do
        Line Input #f1, sIn
        sIn = ";|" + vbTab + sIn
        Print #f2, sIn
        sOut = sOut + sIn + vbCrLf
    Loop Until EOF(f1)
    Close #f1
    Close #f2
    rich_OUTPUT.Text = sOut
    Me.MousePointer = vbArrow
End Sub
