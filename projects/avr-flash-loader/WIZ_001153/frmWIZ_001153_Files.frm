VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "comdlg32.ocx"
Begin VB.Form frmWIZ_001153_Files 
   BorderStyle     =   1  'Fixed Single
   Caption         =   " AutoLoad Files"
   ClientHeight    =   3264
   ClientLeft      =   36
   ClientTop       =   324
   ClientWidth     =   7968
   ControlBox      =   0   'False
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3264
   ScaleWidth      =   7968
   StartUpPosition =   3  'Windows Default
   Begin VB.CheckBox chkAuto 
      Caption         =   "Update customization file"
      Height          =   192
      Index           =   2
      Left            =   2640
      TabIndex        =   9
      Top             =   2520
      Width           =   2472
   End
   Begin VB.CommandButton cmdAuto 
      Caption         =   "..."
      Height          =   252
      Index           =   1
      Left            =   7260
      TabIndex        =   6
      Top             =   1092
      Width           =   312
   End
   Begin VB.CheckBox chkAuto 
      Alignment       =   1  'Right Justify
      Caption         =   "Copy EEPROM from"
      Height          =   312
      Index           =   1
      Left            =   780
      TabIndex        =   4
      Top             =   1080
      Width           =   1752
   End
   Begin VB.TextBox txtAuto 
      Height          =   288
      Index           =   1
      Left            =   2640
      TabIndex        =   5
      Top             =   1092
      Width           =   4452
   End
   Begin VB.CommandButton cmdAuto 
      Caption         =   "..."
      Height          =   252
      Index           =   0
      Left            =   7260
      TabIndex        =   3
      Top             =   672
      Width           =   312
   End
   Begin VB.CheckBox chkAuto 
      Alignment       =   1  'Right Justify
      Caption         =   "Copy Flash from"
      Height          =   312
      Index           =   0
      Left            =   780
      TabIndex        =   1
      Top             =   660
      Width           =   1752
   End
   Begin VB.TextBox txtAuto 
      Height          =   288
      Index           =   0
      Left            =   2640
      TabIndex        =   2
      Top             =   672
      Width           =   4452
   End
   Begin VB.CommandButton cmdAuto 
      Caption         =   "..."
      Height          =   252
      Index           =   2
      Left            =   7260
      TabIndex        =   8
      Top             =   2040
      Width           =   312
   End
   Begin VB.TextBox txtAuto 
      Height          =   288
      Index           =   2
      Left            =   2640
      TabIndex        =   7
      Top             =   2040
      Width           =   4452
   End
   Begin MSComDlg.CommonDialog comDiag 
      Left            =   240
      Top             =   2340
      _ExtentX        =   677
      _ExtentY        =   677
      _Version        =   393216
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "Done"
      Height          =   372
      Left            =   6780
      TabIndex        =   0
      Top             =   2580
      Width           =   792
   End
   Begin VB.Label lbl 
      Caption         =   "If checked, autoload will copy from this location:"
      ForeColor       =   &H000000FF&
      Height          =   192
      Index           =   3
      Left            =   480
      TabIndex        =   12
      Top             =   360
      Width           =   3588
   End
   Begin VB.Label lbl 
      Caption         =   "Customizations may be applied through a text file:"
      ForeColor       =   &H000000FF&
      Height          =   192
      Index           =   4
      Left            =   480
      TabIndex        =   11
      Top             =   1680
      Width           =   3732
   End
   Begin VB.Label lbl 
      Alignment       =   1  'Right Justify
      Caption         =   "Customization file:"
      Height          =   192
      Index           =   5
      Left            =   1080
      TabIndex        =   10
      Top             =   2088
      Width           =   1392
   End
End
Attribute VB_Name = "frmWIZ_001153_Files"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public Function OkToGo() As Boolean
' see if everything is kosher to continue with autoload
' if so, set the state sequence in GoWith$
  Dim aTop As Integer, f$, e As Integer, upd As Integer, p$
  
  On Error GoTo notOK
  OkToGo = False
  GoWith$ = ""                          ' build command string here
  p$ = txtAuto(2).Text                  ' patch file
  upd = chkAuto(2).Value                ' updates to patches?
  
  If chkAuto(0).Value = 1 Then          ' do they want to write flash?
    f$ = txtAuto(0).Text
    If f$ <> "" Then
      Call ClearFl
      aTop = FLASHSIZE - 1
      Open f$ For Input As #1
      Call MergeHex(0, aTop, e)
      Close
      If e > 0 Then Err.Raise 600
      OkToGo = True                     ' assume all is well
      GoWith$ = "XFV"                   ' erase, write flash and verify
      If p$ <> "" Then Call ApplyPatches(p$, 0, aTop, upd)
      internal = True
      frmWIZ_001153.optMem(0).Value = True    ' click event? maybe, or not...
      internal = False
      Call ShowData(frmWIZ_001153.lstMem, 0)  ' so do it manually
    End If
  End If
  
  If chkAuto(1).Value = 1 Then            ' do they want to write EEPROM?
    f$ = txtAuto(1).Text
    If f$ <> "" Then
      Call ClearEE
      aTop = EEPROMSIZE - 1
      Open f$ For Input As #1
      Call MergeHex(1, aTop, e)
      Close
      If e > 0 Then Err.Raise 601
      OkToGo = True
      GoWith$ = GoWith$ & "Ev"                ' add EEPROM write and verify
      If p$ <> "" Then Call ApplyPatches(p$, 1, aTop, upd)
      internal = True
      frmWIZ_001153.optMem(1).Value = True    ' click event? maybe...
      internal = False
      Call ShowData(frmWIZ_001153.lstMem, 1)  ' so do it manually
    End If
  End If
  Exit Function

notOK:
  On Error GoTo 0
  MsgBox "Error reading file " & f$, 48, "AutoLoad"
  OkToGo = False
  GoWith$ = ""
  Call ClearFl                            ' do some cleanup
  Call ClearEE
  internal = True
  frmWIZ_001153.optMem(0).Value = True    ' click event? maybe...
  internal = False
  Call ShowData(frmWIZ_001153.lstMem, 0)  ' so do it manually

End Function

Private Sub cmdAuto_Click(Index As Integer)
  On Error GoTo err4
  
  comDiag.Flags = cdlOFNNoReadOnlyReturn Or cdlOFNPathMustExist Or _
    cdlOFNHideReadOnly Or cdlOFNFileMustExist
  comDiag.CancelError = True
  If Index = 2 Then
    comDiag.Filter = "ASCII (*.txt)|*.txt"
  Else
    comDiag.Filter = "Intel Hex (*.hex)|*.hex|AVR EEPROM (*.eep)|*.eep"
  End If
  comDiag.FileName = ""
  comDiag.ShowOpen
  txtAuto(Index).Text = comDiag.FileName
  Exit Sub
  
err4:
  On Error GoTo 0
End Sub

Private Sub cmdOK_Click()
  Call WriteIniValues
  Me.Hide
End Sub

Private Sub Form_Load()
  Me.Left = (Screen.Width - Me.Width) / 2
  Me.Top = (Screen.Height - Me.Height) / 2
  Call GetPath                                ' initialize the path
  Call LoadIniVals                            ' read the .ini file
End Sub

Sub LoadIniVals()
' grab all the last used file names, etc. off the .ini file
' and display and save them
  Dim i As Integer, a$, v$, c As Integer
   
  For i = 0 To 2                              ' the auto files
    a$ = "File" & i
    v$ = GetProfile("Auto", a$, "")
    txtAuto(i).Text = v$
    
    With hold(i)
      .sect = "Auto"
      .key = a$
      .s = v$
      Set .tb = txtAuto(i)
    End With
    Next i
    
  For i = 0 To 2                              ' the checkboxes
    a$ = "Check" & i
    v$ = GetProfile("Auto", a$, "")
    If LCase$(v$) = "y" Then c = 1 Else c = 0
    chkAuto(i).Value = c
    
    With hold(i + 3)
      .sect = "Auto"
      .key = a$
      .s = Mid$("NY", c + 1, 1)
      Set .tb = chkAuto(i)
    End With
    Next i

End Sub

Sub WriteIniValues()
' we be gone now - rewrite the .ini file
  Dim i As Integer, a$, j As Integer
  
  For i = 0 To 5
    With hold(i)
      If TypeOf .tb Is TextBox Then
        a$ = .tb.Text
      Else
        If .tb.Value = 1 Then a$ = "Y" Else a$ = "N"
      End If
      If a$ <> .s Then
        Call PutProfile(.sect, .key, a$)
        .s = a$
      End If
    End With
    Next i

End Sub

