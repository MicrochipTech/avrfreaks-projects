VERSION 5.00
Begin VB.Form frm_MSX_ASCII_TABLE_TO_BMP 
   AutoRedraw      =   -1  'True
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "MSX ASCII Table to BMP"
   ClientHeight    =   2175
   ClientLeft      =   150
   ClientTop       =   900
   ClientWidth     =   2070
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   145
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   138
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows Default
   Begin VB.Menu mnu_EXIT 
      Caption         =   "&Exit"
   End
   Begin VB.Menu mnu_CAPTURE 
      Caption         =   "&Capture"
   End
End
Attribute VB_Name = "frm_MSX_ASCII_TABLE_TO_BMP"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'+--------------------------------------------------------------+
'| GET MSX ASCII TABLE FROM YOUR BIOS AND SAVE AS MSX_ASCII.BMP |
'| SAVED INTO prj_MSX_ASCII_TABLE_TO_BMP FOLDER                 |
'+--------------------------------------------------------------+


Dim b As Byte


Sub PLOT_CHAR(obj As Object, ByVal x As Integer, ByVal y As Integer, ByVal ASCII_CODE As Integer)
Dim mask    As Integer
Dim m       As Integer
Dim i       As Integer
Dim px      As Integer
Dim py      As Integer

    m = &H1BBF + 1 + ASCII_CODE * 8
    py = 0
    For i = m To m + 7
        Get #1, i, b
        mask = &H80
        For px = 0 To 7
            If (b And mask) = 0 Then
                obj.PSet (x + px, y + py), vbWhite
            Else
                obj.PSet (x + px, y + py), vbBlack
            End If
            mask = mask \ 2
        Next
        py = py + 1
    Next
End Sub

Sub PLOT_CHAR_w6(obj As Object, ByVal x As Integer, ByVal y As Integer, ByVal ASCII_CODE As Integer)
Dim mask    As Integer
Dim m       As Integer
Dim i       As Integer
Dim px      As Integer
Dim py      As Integer

    m = &H1BBF + 1 + ASCII_CODE * 8
    py = 0
    For i = m To m + 7
        Get #1, i, b
        mask = &H80
        For px = 0 To 5
            If (b And mask) = 0 Then
                obj.PSet (x + px, y + py), vbWhite
            Else
                obj.PSet (x + px, y + py), vbBlack
            End If
            mask = mask \ 2
        Next
        py = py + 1
    Next
End Sub

Private Sub Form_Load()
'+------------------------------------------+
'| resize form to keep all chars            |
'+------------------------------------------+

    Me.Width = (16 * 8 + 8) * Screen.TwipsPerPixelX
    Me.Height = (16 * 8 + 7 * 8) * Screen.TwipsPerPixelY
    Me.Show
    
End Sub

Private Sub mnu_CAPTURE_Click()
'+------------------------------------------+
'| get msx characters from bios             |
'| start address &H1bbf lenght 2048 bytes   |
'+------------------------------------------+
Dim x As Integer
Dim y As Integer
Dim c As Integer

    Open App.Path + "\MSXALT1.ROM" For Random As #1 Len = Len(b)
    
    For y = 0 To 15
        For x = 0 To 15
            c = y * 16 + x
            PLOT_CHAR Me, x * 8, y * 8, c
        Next
    Next
    
    Close #1
    SavePicture Me.Image, App.Path + "\MSX_ASCII.BMP"
    Me.Cls
    Open App.Path + "\MSXALT1.ROM" For Random As #1 Len = Len(b)
    
    For y = 0 To 15
        For x = 0 To 15
            c = y * 16 + x
            PLOT_CHAR_w6 Me, x * 6, y * 8, c
        Next
    Next
    
    Close #1
    SavePicture Me.Image, App.Path + "\MSX_ASCII_w6.BMP"

End Sub

Private Sub mnu_EXIT_Click()
    End
End Sub
