VERSION 5.00
Begin VB.Form frmWIZ_001153_edit 
   BorderStyle     =   1  'Fixed Single
   Caption         =   " Memory Image Editor"
   ClientHeight    =   5412
   ClientLeft      =   120
   ClientTop       =   696
   ClientWidth     =   7320
   ControlBox      =   0   'False
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5412
   ScaleWidth      =   7320
   StartUpPosition =   3  'Windows Default
   Begin VB.OptionButton optImage 
      Caption         =   "EEPROM (0.5K)"
      Height          =   252
      Index           =   1
      Left            =   4860
      TabIndex        =   20
      Top             =   660
      Width           =   1632
   End
   Begin VB.OptionButton optImage 
      Caption         =   "Flash (16K)"
      Height          =   252
      Index           =   0
      Left            =   4860
      TabIndex        =   19
      Top             =   420
      Value           =   -1  'True
      Width           =   1632
   End
   Begin VB.Frame frClear 
      Caption         =   " Set All Memory "
      Height          =   612
      Left            =   420
      TabIndex        =   14
      Top             =   4440
      Width           =   4152
      Begin VB.OptionButton optSet 
         Caption         =   "0xFF"
         Height          =   192
         Index           =   2
         Left            =   2100
         TabIndex        =   18
         Top             =   270
         Width           =   672
      End
      Begin VB.OptionButton optSet 
         Caption         =   "0x00"
         Height          =   192
         Index           =   1
         Left            =   1380
         TabIndex        =   17
         Top             =   270
         Width           =   672
      End
      Begin VB.OptionButton optSet 
         Caption         =   "Undefined"
         Height          =   192
         Index           =   0
         Left            =   240
         TabIndex        =   16
         Top             =   270
         Value           =   -1  'True
         Width           =   1092
      End
      Begin VB.CommandButton cmdClear 
         Caption         =   "Set"
         Height          =   252
         Left            =   3420
         TabIndex        =   15
         Top             =   240
         Width           =   552
      End
   End
   Begin VB.TextBox txtAddr 
      Alignment       =   2  'Center
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   7.8
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   288
      Index           =   1
      Left            =   6060
      TabIndex        =   13
      Text            =   "0000"
      Top             =   3360
      Width           =   612
   End
   Begin VB.CommandButton cmdCopy 
      Caption         =   "Copy Block"
      Height          =   312
      Left            =   4860
      TabIndex        =   12
      Top             =   1440
      Width           =   2052
   End
   Begin VB.CommandButton cmdStore 
      Caption         =   "Write String"
      Height          =   312
      Index           =   2
      Left            =   4860
      TabIndex        =   11
      Top             =   2700
      Width           =   2052
   End
   Begin VB.CommandButton cmdStore 
      Caption         =   "Write Decimal Byte(s)"
      Height          =   312
      Index           =   1
      Left            =   4860
      TabIndex        =   10
      Top             =   2280
      Width           =   2052
   End
   Begin VB.CommandButton cmdStore 
      Caption         =   "Write Hex Byte(s)"
      Height          =   312
      Index           =   0
      Left            =   4860
      TabIndex        =   9
      Top             =   1860
      Width           =   2052
   End
   Begin VB.TextBox txtAddr 
      Alignment       =   2  'Center
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   7.8
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   288
      Index           =   0
      Left            =   5160
      TabIndex        =   5
      Text            =   "0000"
      Top             =   3360
      Width           =   612
   End
   Begin VB.TextBox txtVal 
      Alignment       =   2  'Center
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   7.8
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   288
      Left            =   4860
      TabIndex        =   4
      Top             =   3960
      Width           =   2052
   End
   Begin VB.CommandButton cmdCancel 
      Caption         =   "Cancel"
      Height          =   312
      Left            =   4860
      TabIndex        =   3
      Top             =   4740
      Width           =   852
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Height          =   312
      Left            =   6060
      TabIndex        =   2
      Top             =   4740
      Width           =   852
   End
   Begin VB.ListBox lstMem 
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   7.8
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   3888
      Left            =   420
      TabIndex        =   0
      TabStop         =   0   'False
      Top             =   360
      Width           =   4152
   End
   Begin VB.Label lbl 
      Caption         =   "Address(es):"
      Height          =   192
      Index           =   3
      Left            =   4860
      TabIndex        =   8
      Top             =   3120
      Width           =   1008
   End
   Begin VB.Label lbl 
      Caption         =   "New Value(s):"
      Height          =   192
      Index           =   2
      Left            =   4860
      TabIndex        =   7
      Top             =   3720
      Width           =   1008
   End
   Begin VB.Label lbl 
      Alignment       =   2  'Center
      Caption         =   "to"
      Height          =   192
      Index           =   1
      Left            =   5820
      TabIndex        =   6
      Top             =   3408
      Width           =   228
   End
   Begin VB.Label lbl 
      Caption         =   "Memory Image:"
      Height          =   192
      Index           =   0
      Left            =   420
      TabIndex        =   1
      Top             =   120
      Width           =   1272
   End
   Begin VB.Menu mnuFile 
      Caption         =   "File"
      Begin VB.Menu mnuLoad 
         Caption         =   "Load Intex Hex"
         Index           =   0
      End
      Begin VB.Menu mnuLoad 
         Caption         =   "Load ASCII Dump"
         Index           =   1
      End
      Begin VB.Menu mnuLoad 
         Caption         =   "Merge Intel Hex"
         Index           =   2
      End
      Begin VB.Menu mnuLoad 
         Caption         =   "Merge ASCII Dump"
         Index           =   3
      End
      Begin VB.Menu mnuHyphen0 
         Caption         =   "-"
      End
      Begin VB.Menu mnuSave 
         Caption         =   "Save as Intel Hex"
         Index           =   0
      End
      Begin VB.Menu mnuSave 
         Caption         =   "Save as ASCII Dump"
         Index           =   1
      End
      Begin VB.Menu mnuHyphen1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuPatch 
         Caption         =   "Apply Customizations"
         Index           =   0
      End
      Begin VB.Menu mnuPatch 
         Caption         =   "Apply Custom w/ Update"
         Index           =   1
      End
      Begin VB.Menu mnuHyphen2 
         Caption         =   "-"
      End
      Begin VB.Menu mnuCancel 
         Caption         =   "Cancel"
      End
      Begin VB.Menu mnuExit 
         Caption         =   "Exit with Changes"
      End
   End
End
Attribute VB_Name = "frmWIZ_001153_edit"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
'
' this sets EditChg at end of session:
'    -1  cancel (no changes)
'     0  flash active, possibly with changes (already in fl(), maybe ee())
'     1  eeprom active, possibly with changes (already in ee(), maybe fl())
' for good return, EditTop will be the top index in listbox

Dim holdee(EEPROMSIZE - 1) As Integer   ' safe store for cancel
Dim holdfl(FLASHSIZE - 1) As Integer    ' safe store for cancel

Private Sub lstMem_Click()
  Dim j As Integer, a$
  
  j = lstMem.ListIndex
  If j < 0 Then Exit Sub
  a$ = lstMem.List(j)
  txtAddr(0).Text = Left$(a$, 4)
  txtAddr(1).Text = ""
  txtVal.Text = Replace$(Mid$(a$, 7, 23), " ", ",")
  
End Sub

Private Sub mnuLoad_Click(Index As Integer)
' index = 0,2 intel hex, = 1,3 ascii dump; 0,1 load; 2,3 merge
  Dim f$, w As Integer, aTop As Integer, t As Integer
  Dim e As Integer
  
  On Error GoTo err1
  t = Index And 1                   ' 0 for hex, 1 for ascii
  
  With frmWIZ_001153_Files.comDiag
    .Flags = cdlOFNPathMustExist Or cdlOFNFileMustExist
    .CancelError = True
    If t = 1 Then
      .Filter = "ASCII Dump (*.txt)|*.txt"
    Else
      .Filter = "Intel Hex (*.hex)|*.hex|AVR EEPROM (*.eep)|*.eep"
    End If
    .FileName = ""
    .ShowOpen
    f$ = .FileName
  End With
  
  Open f$ For Input As #1
  If optImage(0).Value Then
    w = 0
    aTop = FLASHSIZE - 1
    If Index < 2 Then Call ClearFl
  Else
    w = 1
    aTop = EEPROMSIZE - 1
    If Index < 2 Then Call ClearEE
  End If
  If t = 0 Then
    Call MergeHex(w, aTop, e)
    If e <> 0 Then _
      MsgBox "Error reading hex file: " & e, 48, "Load/Merge"
  Else
    Call MergeAscii(w, aTop)
  End If
  
here1:
  Close
  Call ShowData(lstMem, w)
  Exit Sub
  
err1:
  On Error GoTo 0
  MsgBox "Error accessing file", 48, "Load/Merge"
  Resume here1

End Sub

Private Sub mnuPatch_Click(Index As Integer)
' index = 0 no update, = 1 with update
  Dim f$, w As Integer, aTop As Integer
  
  On Error GoTo err2
  
  With frmWIZ_001153_Files.comDiag
    .Flags = cdlOFNPathMustExist Or cdlOFNFileMustExist
    If Index = 1 Then .Flags = .Flags Or _
      cdlOFNHideReadOnly Or cdlOFNNoReadOnlyReturn
    .CancelError = True
    .Filter = "ASCII text (*.txt)|*.txt"
    .FileName = ""
    .ShowOpen
    f$ = .FileName
  End With
  
  If optImage(0).Value Then
    w = 0
    aTop = FLASHSIZE - 1
  Else
    w = 1
    aTop = EEPROMSIZE - 1
  End If
  Call ApplyPatches(f$, w, aTop, Index)
  
here2:
  Close
  Call ShowData(lstMem, w)
  Exit Sub
  
err2:
  On Error GoTo 0
  MsgBox "Error applying customizations", 48, "Patch"
  Resume here2

End Sub

Private Sub mnuSave_Click(Index As Integer)
' index = 0 save intel hex, = 1 ascii dump
  Dim f$, w As Integer, aTop As Integer, a$, v As Integer
  
  On Error GoTo err3
  
  With frmWIZ_001153_Files.comDiag
    .Flags = cdlOFNPathMustExist Or cdlOFNHideReadOnly Or _
      cdlOFNNoReadOnlyReturn
    .CancelError = True
    If Index = 1 Then
      .Filter = "ASCII Dump (*.txt)|*.txt"
    Else
      .Filter = "Intel Hex (*.hex)|*.hex|AVR EEPROM (*.eep)|*.eep"
    End If
    .FileName = ""
    .ShowOpen
    f$ = .FileName
  End With
  
  If Dir$(f$) <> "" Then
    a$ = "Overwrite " & f$ & "?"
    v = vbYesNo + vbQuestion
    If MsgBox(a$, v, "Save") <> vbYes Then Exit Sub
  End If
  
  Open f$ For Output As #1
  If optImage(0).Value Then
    w = 0
    aTop = FLASHSIZE - 1
  Else
    w = 1
    aTop = EEPROMSIZE - 1
  End If

  If Index = 0 Then
    If IntelHex(w, aTop) > 0 Then _
      MsgBox "Error writing file", 48, "Save"
  Else
    Call AsciiDump(w, aTop)
  End If

here3:
  Close
  Call ShowData(lstMem, w)
  Exit Sub
  
err3:
  On Error GoTo 0
  MsgBox "Error writing file", 48, "Save"
  Resume here3

End Sub

Private Sub cmdCancel_Click()
  Dim i As Integer
  
  EditChg = -1                      ' toss changes
  For i = 0 To EEPROMSIZE - 1       ' restore original
    ee(i) = holdee(i)
    Next i
  For i = 0 To FLASHSIZE - 1        ' restore original
    fl(i) = holdfl(i)
    Next i
  Unload Me
  
End Sub

Private Sub cmdClear_Click()
  Dim i As Integer, w As Integer, v As Integer, n As Integer
  
  If optImage(0).Value Then
    w = 0
    n = FLASHSIZE - 1
  Else
    w = 1
    n = EEPROMSIZE - 1
  End If
  
  If optSet(0).Value Then
    v = -1
  Else
    If optSet(1).Value Then v = 0 Else v = 255
  End If
  
  For i = 0 To n
    If w = 0 Then fl(i) = v Else ee(i) = v
    Next i
  Call ShowData(lstMem, w)
  
End Sub

Private Sub cmdCopy_Click()
' copy a block of memory
' address range shows what to copy
' "new value" shows starting address
  Dim w As Integer, aTop As Integer, a1 As Integer, a2 As Integer
  Dim a3 As Integer, a4 As Integer, ok As Boolean, i As Integer
  
  If optImage(0).Value Then
    w = 0
    aTop = FLASHSIZE - 1
  Else
    w = 1
    aTop = EEPROMSIZE - 1
  End If
  
  a1 = CheckAddr(0, aTop)        ' starting address
  If a1 < 0 Then
    MsgBox "Invalid address", 48, "Copy"
    txtAddr(0).SetFocus
    Exit Sub
  End If
  a2 = CheckAddr(1, aTop)        ' ending address
  If a2 < a1 Or a2 > aTop Then
    MsgBox "Invalid address", 48, "Copy"
    txtAddr(1).SetFocus
    Exit Sub
  End If
  a3 = CheckAddr(3, aTop)        ' target address
  If a3 < 0 Or a3 > aTop Then
    MsgBox "Invalid target address", 48, "Copy"
    txtVal.SetFocus
    Exit Sub
  End If
  a4 = a3 + a2 - a1              ' top target address
  If a4 > aTop Then
    MsgBox "Target falls outside range", 48, "Copy"
    txtVal.SetFocus
    Exit Sub
  End If
  
  ok = True                      ' check for overlap
  If a1 < a3 Then
    If a2 >= a3 Then ok = False
  Else
    If a1 <= a4 Then ok = False
  End If
  If Not ok Then
    MsgBox "Source and destination overlap", 48, "Copy"
    txtVal.SetFocus
    Exit Sub
  End If

  For i = a1 To a2              ' do move
    If w = 0 Then
      fl(a3) = fl(i)
    Else
      ee(a3) = ee(i)
    End If
    a3 = a3 + 1
    Next i
  Call ShowData(lstMem, w)
  
End Sub

Private Sub cmdOK_Click()
' leave and use changes

  If optImage(0).Value Then EditChg = 0 Else EditChg = 1
  EditTop = lstMem.TopIndex
  Unload Me
  
End Sub

Private Sub cmdStore_Click(Index As Integer)
' index = 0, hex; = 1, decimal; = 2, string
  Dim a$, w As Integer, a1 As Integer, a2 As Integer, v As Integer
  Dim i As Integer, n As Integer, aTop As Integer, j As Integer
  
  If optImage(0).Value Then
    w = 0
    aTop = FLASHSIZE - 1
  Else
    w = 1
    aTop = EEPROMSIZE - 1
  End If
  
  a1 = CheckAddr(0, aTop)         ' get starting address
  If a1 < 0 Then                  ' must be nonblank and in range
    MsgBox "Invalid address", 48, "Write"
    txtAddr(0).SetFocus
    Exit Sub
  End If
  
  a2 = CheckAddr(1, aTop)         ' upper address
  If a2 = -1 Then
    a$ = Trim$(txtAddr(1).Text)
    If a$ = "*" Then
      a2 = aTop
    Else
      MsgBox "Invalid address", 48, "Write"
      txtAddr(1).SetFocus
      Exit Sub
    End If
  End If
  
  a$ = txtVal.Text
  If Index < 2 Then
    a$ = Trim$(a$)
    Call Parser(a$, Index)
    If nparse = 0 Then
      MsgBox "Missing or invalid values", 48, "Write Bytes"
      txtVal.SetFocus
      Exit Sub
    End If
    n = nparse
    
    If a2 >= 0 Then
      If a2 <> (a1 + n - 1) Then
        If n = 1 Then
        ' we have a special case: one value with address range
          v = parse(1)
          For i = a1 To a2
            If w = 0 Then fl(i) = v Else ee(i) = v
            Next i
          Call ShowData(lstMem, w)
          Exit Sub
        Else
          a$ = "Inconsistent upper address" & vbLf & _
            "(may be left blank)"
          MsgBox a$, 48, "Write Bytes"
          txtAddr(1).SetFocus
          Exit Sub
        End If
      End If
    End If
  Else
    n = Len(a$)
    If n = 0 Then
      MsgBox "Please enter a string to write", 48, "Write String"
      txtVal.SetFocus
      Exit Sub
    End If
    If a2 >= 0 Then
      If a2 <> (a1 + n - 1) Then
        a$ = "Inconsistent upper address" & vbLf & _
          "(may be left blank)"
        MsgBox a$, 48, "Write String"
        txtAddr(1).SetFocus
        Exit Sub
      End If
    End If
  End If
  
  For i = 1 To n
    If Index = 2 Then v = Asc(Mid$(a$, i, 1)) Else v = parse(i)
    If w = 0 Then fl(a1) = v Else ee(a1) = v
    a1 = a1 + 1
    If a1 > aTop Then Exit For
    Next i
  Call ShowData(lstMem, w)
  
End Sub

Sub Parser(a$, abase As Integer)
' put a comma separated list of hex or decimal numbers into parse()
' PMAX is the limit on the number of numbers
' nparse is the resulting number of elements in parse()
' abase = 0 for assumed hex, = 1 for assumed decimal
' -, *, and -- are interpreted as -1 (undefined value)
' hex numbers may be designated by $ or 0x
  Dim j As Integer, b$, e As Boolean, c$
  
  nparse = 0
  b$ = Trim(a$)
  If b$ = "" Then Exit Sub
  
  e = False                            ' error flag
  If Right$(b$, 1) <> "," Then b$ = b$ & ","

  Do While b$ <> ""
    j = InStr(b$, ",")
    If j = 1 Then
      b$ = LTrim$(Mid$(b$, 2))        ' ignore double commas
    Else
      c$ = RTrim$(Left$(b$, j - 1))   ' the value
      b$ = LTrim$(Mid$(b$, j + 1))    ' the remainder, if any
      Call pNum(c$, e, abase)         ' convert and put away if good
      If e Then Exit Do
    End If
  Loop
  If e Then nparse = 0                ' errors ruin our day
  
End Sub

Sub pNum(a$, e As Boolean, ab As Integer)
' decode a small (0 - 255) number into parse(nparse + 1)
' a$ - incoming string
' e - set to True in case of error
' ab - =0 if assumed hex, =1 if assumed decimal
' -, *, --, and -1 are interpreted as -1
  Dim v As Integer, b$, i As Integer, iBase As Integer
  Dim w As Integer, n As Integer
  
  e = True                      ' assume disaster
  
  If InStr(",-,--,*,-1", "," & a$) > 0 Then
    v = -1
  Else
    b$ = Replace$(LCase$(a$), "0x", "$")
    If ab = 0 And Left$(b$, 1) <> "$" Then b$ = "$" & b$
    n = Len(b$)
    iBase = 10
    v = 0
    
    For i = 1 To n
      w = Asc(Mid$(b$, i, 1))
      Select Case w
        Case 36                       ' $
          If i <> 1 Then Exit Sub     ' must be leading char
          iBase = 16
        Case 48 To 57                 ' 0 to 9
          v = v * iBase + (w - 48)
        Case 97 To 102                ' a to f
          If iBase = 10 Then Exit Sub ' too bad for you
          v = v * iBase + (w - 87)
        Case Else
          Exit Sub                    ' unknown character
      End Select
      If v > 255 Then Exit Sub        ' too big a number
      Next i
    If (iBase = 16) And (n < 2) Then Exit Sub  ' only the base char(s)
  End If
  
  nparse = nparse + 1
  If nparse > PMAX Then
    e = True
    nparse = 0
  Else
    parse(nparse) = v
  End If
  e = False                           ' all a-OK, thanks

End Sub

Function CheckVal(a$, w As Integer) As Integer
' check a hex (w = 0) or decimal (w = 1) value for 0-255
' return: -1 if bad, 0-255 if good
  Dim b$, c$, i As Integer, n As Integer
  
  CheckVal = -1                     ' assume the worst
  b$ = LCase$(Trim$(a$))
  n = Len(b$)
  If n < 1 Or n > (2 + w) Then Exit Function
  
  For i = 1 To n
    c$ = Mid$(b$, i, 1)
    If c$ < "0" Or c$ > "9" Then
      If c$ < "a" Or c$ > "f" Then Exit Function
    End If
    Next i
  
  If w = 0 Then b$ = "&h" & b$
  i = Val(b$)
  If i >= 0 And i < 256 Then CheckVal = i

End Function

Function CheckAddr(who As Integer, ahi As Integer) As Integer
' check txtAddr(who).text for legality
'   if who = 2 then use txtVal.text instead
' aHi - highest legal address
' returns: [0 - aHi] if good, -1 if bad, -2 if blank
  Dim x As Long, a$
  
  If who < 2 Then
    a$ = Trim$(txtAddr(who).Text)       ' ignore leading/trailing blanks
  Else
    a$ = Trim$(txtVal.Text)
  End If
  
  If a$ = "" Then
    x = -2
  Else
    If Len(a$) > 4 Then
      x = -1
    Else
      If IsHex(a$) Then
        x = Val("&h" & a$)
        If x < 0 Or x > ahi Then x = -1
      Else
        x = -1
      End If
    End If
  End If
  CheckAddr = x

End Function

Private Sub Form_Load()
  Dim w As Integer, i As Integer
  
  Me.Left = (Screen.Width - Me.Width) / 2
  Me.Top = (Screen.Height - Me.Height) / 2
  
  For i = 0 To EEPROMSIZE - 1         ' save a copy of originals
    holdee(i) = ee(i)
    Next i
  For i = 0 To FLASHSIZE - 1
    holdfl(i) = fl(i)
    Next i

  internal = True
  If frmWIZ_001153.optMem(0).Value Then w = 0 Else w = 1
  optImage(w).Value = True
  Call ShowData(lstMem, w)
  lstMem.TopIndex = frmWIZ_001153.lstMem.TopIndex
  internal = False
End Sub

Private Sub mnuCancel_Click()
  Call cmdCancel_Click
End Sub

Private Sub mnuExit_Click()
  Call cmdOK_Click
End Sub


Private Sub optImage_Click(Index As Integer)
  Dim w As Integer
  
  If internal Then Exit Sub
  If optImage(0).Value Then w = 0 Else w = 1
  Call ShowData(lstMem, w)
  
End Sub

Private Sub txtAddr_Change(Index As Integer)
  If internal Then Exit Sub
  
  If Index = 0 Then
    internal = True
    txtAddr(1).Text = ""
    internal = False
  End If
  DoEvents
  
End Sub

Private Sub txtAddr_GotFocus(Index As Integer)
  Dim n As Integer
  
  n = Len(txtAddr(Index).Text)
  If n > 0 Then
    txtAddr(Index).SelStart = 0
    txtAddr(Index).SelLength = n
  End If
  
End Sub

Private Sub txtVal_GotFocus()
  Dim n As Integer
  
  n = Len(txtVal.Text)
  If n > 0 Then
    txtVal.SelStart = 0
    txtVal.SelLength = n
  End If
End Sub
