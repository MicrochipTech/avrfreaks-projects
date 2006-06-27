VERSION 5.00
Object = "{648A5603-2C6E-101B-82B6-000000000014}#1.1#0"; "mscomm32.ocx"
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   7695
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   12240
   LinkTopic       =   "Form1"
   ScaleHeight     =   513
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   816
   StartUpPosition =   3  'Windows Default
   Begin VB.CheckBox SignatureOnly 
      Caption         =   "Signature Only"
      Enabled         =   0   'False
      Height          =   255
      Left            =   9240
      TabIndex        =   15
      Top             =   360
      Width           =   1455
   End
   Begin VB.CommandButton DeleteLearned 
      Caption         =   "Delete"
      Height          =   375
      Left            =   8280
      TabIndex        =   14
      Top             =   6360
      Width           =   1095
   End
   Begin VB.CommandButton SaveAll 
      Caption         =   "Save All"
      Height          =   495
      Left            =   2880
      TabIndex        =   12
      Top             =   6120
      Width           =   735
   End
   Begin VB.CheckBox Check2 
      Caption         =   "Invert"
      Height          =   375
      Left            =   1200
      TabIndex        =   11
      Top             =   6480
      Width           =   735
   End
   Begin VB.CheckBox Check1 
      Caption         =   "X2 scale"
      Height          =   375
      Left            =   240
      TabIndex        =   10
      Top             =   6480
      Width           =   975
   End
   Begin VB.CommandButton SendIR 
      Caption         =   "SEND"
      Height          =   375
      Left            =   8280
      TabIndex        =   7
      Top             =   5880
      Width           =   1095
   End
   Begin VB.TextBox Port 
      Height          =   285
      Left            =   8640
      TabIndex        =   5
      Text            =   "1"
      Top             =   360
      Width           =   255
   End
   Begin VB.CommandButton CopyClip 
      Caption         =   "Copy Info to Clipboard"
      Height          =   615
      Left            =   1200
      TabIndex        =   4
      Top             =   5760
      Width           =   1335
   End
   Begin VB.CommandButton Learn 
      Caption         =   "Learn Last"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   495
      Left            =   120
      TabIndex        =   3
      Top             =   5760
      Width           =   975
   End
   Begin VB.ListBox LearnList 
      BeginProperty Font 
         Name            =   "Fixedsys"
         Size            =   9
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   960
      Left            =   3720
      TabIndex        =   2
      Top             =   5880
      Width           =   4455
   End
   Begin VB.TextBox Status 
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   13.5
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   495
      Left            =   720
      TabIndex        =   1
      Text            =   "Text1"
      Top             =   240
      Width           =   6855
   End
   Begin VB.ListBox list1 
      BeginProperty Font 
         Name            =   "Fixedsys"
         Size            =   9
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   4785
      Left            =   120
      TabIndex        =   0
      Top             =   840
      Width           =   12015
   End
   Begin MSCommLib.MSComm MSComm1 
      Left            =   11160
      Top             =   120
      _ExtentX        =   1005
      _ExtentY        =   1005
      _Version        =   393216
      DTREnable       =   -1  'True
      BaudRate        =   19200
   End
   Begin VB.Label Label2 
      Caption         =   "Learned Keys"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   5160
      TabIndex        =   13
      Top             =   5640
      Width           =   1455
   End
   Begin VB.Label Ticker 
      Caption         =   "0"
      BeginProperty Font 
         Name            =   "Arial"
         Size            =   27.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   615
      Left            =   120
      TabIndex        =   9
      Top             =   240
      Width           =   735
   End
   Begin VB.Label Welcome 
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   1200
      TabIndex        =   8
      Top             =   120
      Width           =   3975
   End
   Begin VB.Label Label1 
      Caption         =   "COM port"
      Height          =   495
      Left            =   8040
      TabIndex        =   6
      Top             =   240
      Width           =   495
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Const IRFILENAME = "\TWIRPfile.txt"

Dim Vlast As Variant    ' last input data from serial port. MSCOMM requires a variant
Dim IRbytes() As Byte   ' byte array version of  data
Dim sending As Boolean
Dim abyte(0) As Byte    ' one byte
Dim pass As Integer
Dim bitWidthAvg As Integer

Dim retained() As learned




' Program starts up here
Private Sub Form_Load()

    Me.SignatureOnly.Enabled = False
    Me.Caption = App.Title & " v" & App.Major & "." & App.Minor
    Learn.Enabled = False
    
    CopyClip.Enabled = False
    Me.Port = "1"
    Call Port_Change

    Me.LearnList.Clear
    Me.SendIR.Enabled = False
    Me.Ticker = ""

    Me.Show
    Call readAll
    
End Sub

' after data is received, graph it like an o'scope
Private Sub drawWaveform(onThis As Object, d() As Byte)
    Dim x, y, dx, dy
    Dim microSecPerPixel As Single
    Dim n As Long, ht As Long
    Dim i As Integer
    Dim polarity As Boolean
    
    On Error GoTo exithere
    If UBound(d) = 0 Then Exit Sub  ' error if empty
    On Error GoTo 0
    
    onThis.Cls  ' erase prior lines
    
    onThis.ScaleMode = vbPixels   ' pixels
    onThis.DrawWidth = 2
    x = onThis.ScaleLeft + 16
    y = onThis.ScaleTop + onThis.ScaleHeight - 16

    If Check1 = vbChecked Then
        microSecPerPixel = 50# ' uSec width
    Else
        microSecPerPixel = 100# '  uSec width
    End If
    
    PSet (x, y), vbBlack
    i = 0
    polarity = (Check2 = vbChecked)
    Do
        If d(i) <> 0 Then
            dx = (d(i) * IRpwLSBuSec) / microSecPerPixel
        Else
            i = i + 1
            dx = (d(i) * 1000#) / microSecPerPixel
        End If
        onThis.Line (x, y)-(x, y - 32), vbBlackness
        If polarity Then dy = 32 Else dy = 0
        onThis.Line (x, y - dy)-(x + dx, y - dy), vbBlackness
        x = x + dx
        i = i + 1
        polarity = Not polarity
    Loop Until i > UBound(d) - 3 Or x > onThis.Width
exithere:
End Sub



Private Sub Check1_Click()
    Call drawWaveform(Me, IRbytes)
End Sub

Private Sub Check2_Click()
    Call drawWaveform(Me, IRbytes)
End Sub





' Serial Port number changed on the form
Private Sub Port_Change()
    Dim s As String
    
    Port = Left(Port, 1)

    list1.Clear
    DoEvents
    Sleep (100) ' makes screen blink
    
    If IsNumeric(Port) Then
        If MSComm1.PortOpen Then MSComm1.PortOpen = False
        On Error GoTo openfailed
        MSComm1.CommPort = Port
        MSComm1.Settings = "19200,n,8,1"
        MSComm1.InputMode = comInputModeBinary
        MSComm1.RThreshold = 0
        Status = "Opening Serial Port COM" & CStr(Port) & ", Settings: " & MSComm1.Settings
    
        MSComm1.PortOpen = True
        On Error GoTo 0

        Sleep (100)
        ' version number request
        If SignatureOnly = vbChecked Then abyte(0) = 2 Else abyte(0) = 1
        Vlast = abyte   ' copy byte to variant to placate MScomm
        
        MSComm1.Output = Vlast
        Sleep (100)
        If MSComm1.InBufferCount >= 1 Then
            Vlast = MSComm1.Input ' purge
            If Vlast(0) = 1 Then
                list1.AddItem s & "Sent Version Request Command with Signature-Only mode = " & _
                    (SignatureOnly = vbChecked)
                list1.AddItem "TWIRP responded correctly."
                list1.AddItem "TWIRP firmware version is " & Vlast(0)
            Else
                list1.AddItem "IR Hardware on Serial port " & Port & " did NOT correctly respond to initialization. Reenter COM Port #"
                MSComm1.PortOpen = False
                Port = ""
                End If
        Else
            MsgBox "IR hardware did not respond to initialization. Check COM port/cables, Reenter Port # COM Port #", vbCritical
            MSComm1.PortOpen = False
            Port = ""
        End If
    Else
        Port = ""   ' ignore non-digits
        Status = "Please enter the proper serial port number"
    End If
    MSComm1.RThreshold = 1      ' raise event on arrival of 1st byte
    Me.SignatureOnly.Enabled = True
    Exit Sub
    
openfailed:
    Me.Port = ""
    MsgBox "COMM port could not be opened. Exists? Busy?", vbCritical
End Sub


' User clicked on the Learn button
Private Sub Learn_Click()
    Static n As Integer ' initially 0
    Dim cs8 As Integer
    Dim i As Integer
    Dim dur As Long
    Dim bytes() As Byte
    
    Learn.Enabled = False
    n = UBound(retained)
    ReDim Preserve retained(n + 1)
    retained(n).learned = IRbytes       ' copies all data and the ending 0 0 status (3 bytes)
    retained(n).name = Format(Date, "yymmdd") & Format(Time, "hhmmss")

    ' compute 8 bit checksum
    cs8 = 0
    For i = 0 To UBound(IRbytes) - 1
        cs8 = (cs8 + IRbytes(i)) And 255
    Next
    retained(n).cs = CByte(cs8)
    
    dur = 0
    i = 0
    Do
        If IRbytes(i) = 0 Then
            dur = dur + (CLng(IRbytes(i + 1)) * 1000)
            i = i + 2
        Else
            dur = dur + (IRbytes(i) * IRpwLSBuSec)
            i = i + 1
        End If
    Loop While i < UBound(IRbytes)
    retained(n).duration = dur
    
    LearnList.AddItem retained(n).name
    LearnList.ListIndex = LearnList.ListCount - 1
    n = n + 1
    Status = "Retained # " & n
End Sub

' User pressed Delete key to delete from display list a learned key
Private Sub DeleteLearned_Click()
    Dim n As Integer, i As Integer
    
    n = LearnList.ListIndex
    If n < 0 Then Exit Sub
    If (n <> LearnList.ListCount - 1) Then
        For i = n To LearnList.ListCount - 2 Step -1
            retained(i) = retained(i + 1)
        Next
    End If
    LearnList.RemoveItem n
    ReDim Preserve retained(LearnList.ListCount)
    list1.Clear
    Me.Cls
    Learn.Enabled = False
End Sub

' Write the learned keys to disk
Private Sub SaveAll_Click()
    Dim f As Variant
    Dim s1 As String, s2 As String
    Dim i As Integer, j As Integer
    
    If LearnList.ListCount <= 0 Then Exit Sub
    s1 = App.Path & IRFILENAME
    s2 = s1 & ".bak"
    On Error Resume Next
    Kill s2
    Name s1 As s2
    On Error GoTo 0
    f = FreeFile()
    Open s1 For Output As #f
    For i = 0 To UBound(retained) - 1
        If i > 0 Then
            Print #f, ""
            Print #f, ""
        End If
        Print #f, retained(i).name
        For j = 0 To UBound(retained(i).learned) - 1
            If j > 0 And j Mod 8 = 0 Then Print #f, ""
            Print #f, Format(retained(i).learned(j), " 000");
        Next
    Next
    Print #f, ""
    Close #f
    MsgBox ("Saved Key Data to " & s1)
End Sub

' Retrieve all previously learned keys from disk file
Private Sub readAll()
    Dim f As Variant
    Dim s As String
    Dim n As Integer, i As Integer, j As Integer
    Dim ir() As Byte
    
    LearnList.Clear
    ReDim retained(0)
    
    f = FreeFile
    On Error GoTo nofile
    Open App.Path & IRFILENAME For Input As #f
    n = 0
    Do
        On Error GoTo endfile
        Line Input #f, s        ' read a key's name
        On Error GoTo badformat
        If Len(s) > 0 Then
            ReDim Preserve retained(n + 1)
            retained(n).name = s
            LearnList.AddItem s
            j = 0
            Do      ' read each line of data until blank line
                s = ""
                On Error Resume Next    ' EOF error on last line
                Line Input #f, s
                On Error GoTo 0
                'Debug.Print EOF(f), s
                s = Trim(s)
                If Len(s) = 0 Then Exit Do ' end of numbers
                i = 1
                Do  ' parse one line of numbers
                    If IsNumeric(Mid(s, i, 3)) Then
                        ReDim Preserve ir(j + 1)
                        ir(j) = Val(Mid(s, i, 3))
                        j = j + 1
                        i = i + 4
                    Else
                        Exit Do ' next line
                    End If
                Loop While i < Len(s)
            Loop
            retained(n).learned = ir    ' copy bytes
            n = n + 1
        End If
    Loop While Not EOF(f)
endfile:
    Close #f
    If n = 0 Then GoTo badformat
    Exit Sub
    
badformat:
    MsgBox (App.Path & IRFILENAME & " contains invalid format: " & vbCrLf & s)
nofile:
    LearnList.Clear
    ReDim retained(0)
    MsgBox ("No IR Keys data file found. Please 'learn' and save the keys.")
End Sub


' User clicked in the listbox of learned keys
Private Sub LearnList_Click()

    If LearnList.ListCount > 0 Then SendIR.Enabled = True
    ReDim IRbytes(0)
    IRbytes = retained(LearnList.ListIndex).learned
    Call displayIR(False)
End Sub
'''''''''''''''''''''''''''''''''''''''''''''
' User double-clicked in the listbox of learned keys
Private Sub LearnList_DblClick()
    Dim keyname As String
    Dim s As String
    Dim n As Integer
    
    keyname = InputBox("Name for this button? ", "Name of Button", "")
    If Len(keyname) = 0 Then Exit Sub
    retained(LearnList.ListIndex).name = keyname
    LearnList.List(LearnList.ListIndex) = keyname
    
End Sub

'''''''''''''''''''''''''''''''''''''''''''''
' Send IR for the selected key
Private Sub TransmitIR(learnedIndex As Integer)
    Dim n As Integer, n1 As Integer, i As Integer, pass As Integer
    Dim v As Variant
    Dim bytesOut() As Byte
    Dim vx As Variant
    
    If sending Or learnedIndex < 0 Then Exit Sub
    
    sending = True
    LearnList.Enabled = False

    
    i = UBound(retained(learnedIndex).learned) - 1 ' omit concluding status byte

    ReDim bytesOut(i + 1) ' leave room for preamble of 1 byte
    bytesOut(0) = 0 ' preamble command is send IR
    
    While i > 0
        bytesOut(i + 1) = retained(learnedIndex).learned(i)  ' copy
        i = i - 1
    Wend
    
    v = bytesOut   ' copy to a variant
    
    MSComm1.Output = v ' command and ir data
    Do
        DoEvents
        Sleep (100)
    Loop While MSComm1.InBufferCount < 1

    n1 = MSComm1.InBufferCount
    pass = pass + 1
    'Debug.Print Time; " Send complete. Input count: "; n1
    vx = MSComm1.Input ' purge
    bytesOut = vx
    
    'For i = 0 To UBound(bytesOut)
    '    Debug.Print "response "; i, bytesOut(i)
    'Next
    Status = Time & " Transmit complete. Status: " & bytesOut(0)
    sending = False
    LearnList.Enabled = True
End Sub
'''''''''''''''''''''''''''''''''''''''''''''
' Input and display IR data
Function getIR() As Long
    Dim n As Integer, n1 As Integer
    
    n1 = 0
    Do      ' loop to collect all data
        DoEvents
        If Not sending Then
            n = MSComm1.InBufferCount   ' number of bytes in buffer
            Sleep (100)
            n1 = MSComm1.InBufferCount  ' check again, see if more arrived
        End If
    Loop While (n1 <= 2) Or (n1 <> n)   ' expect more input
    
    Vlast = MSComm1.Input       ' get all data
    IRbytes = Vlast                ' copy input to a byte array
    Call displayIR(True)
End Function
'''''''''''''''''''''''''''''''''''''''''''''
' Display IR date now in global IRbytes
Private Sub displayIR(FromSerial As Boolean)
    Dim n As Integer, n1 As Integer, i As Integer, previous As Byte
    Dim column As Integer
    Dim msgDuration As Long
    Dim s As String
    Dim transitions As Integer
    Dim bitcount As Integer
    Dim avg As Long
    Dim bitw As Integer, minbit As Integer, minbitnum As Integer, maxbit As Integer, maxbitnum As Integer
    Dim repeatflag As Boolean
    Dim repeatnum As Integer
    Dim ReturnedStatus As Byte
    
    list1.Clear
    Me.Ticker.Caption = ""
    DoEvents
    Sleep (100) ' blinks screen
    
    pass = pass + 1
    list1.AddItem " Received " & UBound(IRbytes) + 1 & " bytes"
    
    If FromSerial And SignatureOnly Then
        If UBound(IRbytes) < 5 Then
            list1.AddItem "Expected 6 bytes from TWIRP. Received: " & CStr(UBound(IRbytes))
        Else
            Call displaySignature(0)
        End If
        Exit Sub
    End If
    
    
    s = "  "
    For i = 0 To 15 ' create headings
        s = s & "  " & (i And 1) & "   "
    Next
    list1.AddItem s
    
    s = ""
    column = 0
    previous = 255
    msgDuration = 0
    transitions = 0
    bitcount = 0
    
    For i = 0 To UBound(IRbytes) - 1       ' for every byte received
        If previous = 0 Then           ' previous value was 0?
            If IRbytes(i) = 0 Then
                Exit For ' 0 0 means end of data <<<<<<<<<<<<<<<<<<<<<<
            End If
            msgDuration = msgDuration + IRbytes(i) ' means this one is units of mSec
        Else
            msgDuration = msgDuration + (IRbytes(i) * IRpwLSBuSec)
            transitions = transitions + 1
        End If
        
        n = IRbytes(i)               ' next value
        If n <> 0 Then
            If previous <> 0 Then
                s = s & Format(Round((CDbl(IRbytes(i)) * IRpwLSBuSec)), "  0000")  ' 25.6uSec = LSB
            Else
                s = s & Format(IRbytes(i), " X0000")
            End If
            column = column + 1
        End If
        If (column > 0) And (column Mod 16) = 0 Then
            list1.AddItem s
            s = ""
        End If
        previous = n
    Next
    If Len(s) > 0 Then list1.AddItem s

    repeatflag = False
    repeatnum = 0
    avg = 0
    minbit = 32767
    maxbit = -1
    i = 2
    Do ' for all bits except first lead-in
        If IRbytes(i) = 0 Then
            If IRbytes(i + 1) = 0 Then
                Exit Do ' <<<<< 0 0 means end of data
            Else
                repeatflag = True
            End If
            i = i + 2
        Else
            bitw = CInt(IRbytes(i)) + CInt(IRbytes(i + 1))
            If i >= 2 Then avg = avg + bitw
            If bitw < minbit Then
                minbit = bitw
                minbitnum = i
            End If
            If bitw > maxbit Then
                maxbit = bitw
                maxbitnum = i
            End If
            i = i + 1
        End If
    Loop
    
    ReturnedStatus = IRbytes(i + 2)
    Me.Ticker.Caption = CStr(ReturnedStatus)
    Status = ""
    If FromSerial Then
        Status = Time & " Received IR, reported status: " & CStr(ReturnedStatus)
        Call displaySignature(i + 3)
    End If
    
    avg = avg / (i - 3)
    bitWidthAvg = avg * IRpwLSBuSec     ' save as global
    list1.AddItem ""
    If repeatflag Then s = "YES" Else s = "NO"
    list1.AddItem "Repeating: " & s
    list1.AddItem UBound(IRbytes) + 1 & vbTab & "Byte message"
    list1.AddItem Format(msgDuration / 1000, "###,###.##") & vbTab & "mSec message length"
    If IRbytes(0) = 0 Then s = CStr(IRbytes(1)) Else s = "(no)"
    list1.AddItem s & vbTab & "mSec Lead-In (AGC) bit length"
    list1.AddItem transitions & vbTab & "Logic level transitions"
    list1.AddItem Round(avg * IRpwLSBuSec) & vbTab & "uSec Average bit cell duration"
    list1.AddItem Round(minbit * IRpwLSBuSec) & vbTab & "uSec Duration minimum, cell # " & minbitnum
    list1.AddItem Round(maxbit * IRpwLSBuSec) & vbTab & "uSec Duration maximum, cell # " & maxbitnum

    list1.ListIndex = list1.ListCount - 1   ' scroll to bottom

    CopyClip.Enabled = True


    Learn.Enabled = True
    
    Call drawWaveform(Me, IRbytes)
    ''Call MakeSignature
End Sub

Private Sub displaySignature(i As Integer)
    Dim n As Integer, n1 As Integer
    Dim s As String

    ' display 6 signature bytes in hex
    s = ""
    For n = 0 To 5
        n1 = IRbytes(i + n)
        If n1 < 16 Then s = s & "0"
        s = s & Hex(n1)
    Next
    list1.AddItem ""
    list1.AddItem "Signature: " & """" & s & """" & " (6 bytes, hexadecimal)"
End Sub


#If False Then          ' signature computation is now in TWIRP's firmware
Sub MakeSignature()
    Dim n As Integer
    Dim i As Integer
    Dim k1 As Integer
    Dim signature(6) As Byte
    Dim ones As Integer
    Dim bitcount As Integer
    Dim PW As Long
    Dim s As String
    
    'Exit Sub
    
    n = UBound(IRbytes)
    For i = 0 To UBound(signature) - 1
        signature(i) = 0
    Next

    k1 = 0
    bitcount = 0
    ones = 0
    
    i = 0 ' 3   ' ignore the lead-in stored in (0) through (1)
    
    Do
        PW = 0
        For n = 1 To 2 ' for both the on/off portions of the waveform
            If IRbytes(i) = 0 Then
                PW = PW + (CLng(IRbytes(i + 1)) * 1000)
                i = i + 2
            Else
                PW = PW + (CInt(IRbytes(i)) * IRpwLSBuSec)
                i = i + 1
            End If
        Next
        If PW >= 4000 Then   ' lead in pulse or repeat
            If bitcount > 0 Then Exit Do
            k1 = 0
            bitcount = 0
        Else
            If k1 = 0 Then k1 = PW
            For n = UBound(signature) - 1 To 1 Step -1
                signature(n) = (signature(n) And &H7F) * 2 ' left shift one, bring a zero
                If (signature(n - 1) And &H80) Then signature(n) = signature(n) + 1 ' bring in a bit
            Next
            signature(0) = (signature(0) And &H7F) * 2
            If PW >= (0.75 * k1) Then signature(0) = signature(0) + 1
            bitcount = bitcount + 1
        End If
    Loop While i < (UBound(IRbytes) - 1)
    
    Debug.Print "k1:"; k1; " bitcount:"; bitcount; " ones:"; ones
    ' convert 6 bytes to hex digits
    s = ""
    For i = UBound(signature) - 1 To 0 Step -1
        If signature(i) < 16 Then s = "0" & s
        s = s & Hex(signature(i))
    Next
    Debug.Print
    
    Select Case s
        Case "000001C383C7": s = "FF"
        Case "000001C3A3C5": s = "REW"
        Case "000001C2A3D5": s = "PLAY"
        Case Else: s = "? " & s
    End Select
    Debug.Print "KEY "; s
End Sub
#End If


' --- come here when 1st byte arrives in a new IR message
Private Sub MSComm1_OnComm()
    Dim s As String
    
   Select Case MSComm1.CommEvent
   ' Handle each event or error by placing code below each case statement
   ' commented out the ones which are N/A for this application.

   ' Errors
      'Case comEventBreak   ' A Break was received.
      Case comEventFrame   ' Framing Error
        MsgBox "Serial Receive Framing error", vbCritical
      'Case comEventOverrun   ' Data Lost.
      Case comEventRxOver   ' Receive buffer overflow.
        MsgBox "Serial Receive overrun error", vbCritical
        DoEvents
        Sleep (1000)
        DoEvents
        s = MSComm1.Input ' purge

      'Case comEventRxParity   ' Parity Error.
      'Case comEventTxFull   ' Transmit buffer full.
      'Case comEventDCB   ' Unexpected error retrieving DCB]

   ' Events
      Case comEvReceive   ' Received RThreshold # of chars.
        MSComm1.RThreshold = 0 ' turn off events for 2nd-nth bytes
        Call getIR              ' get and process the rest of the bytes
        MSComm1.RThreshold = 1  ' reenable for next IR message
      'Case comEvCD   ' Change in the CD line.
      'Case comEvCTS   ' Change in the CTS line.
      'Case comEvDSR   ' Change in the DSR line.
      'Case comEvRing   ' Change in the Ring Indicator.

      'Case comEvSend   ' There are SThreshold number of
                     ' characters in the transmit
                     ' buffer.
      'Case comEvEOF   ' An EOF charater was found in
                     ' the input stream
   End Select
End Sub


Private Sub CopyClip_Click()
    Dim i As Integer
    Dim s As String
    
    If list1.ListCount = 0 Then
        MsgBox "Nothing to Copy Yet", vbCritical
        Exit Sub
    End If
    
    Clipboard.Clear
    s = ""
    i = 0
    While i < list1.ListCount
        list1.ListIndex = i
        s = s & list1.Text & vbCrLf
        i = i + 1
    Wend
    Clipboard.SetText s
End Sub





Private Sub SendIR_Click()
   If LearnList.ListIndex >= 0 Then
        TransmitIR (LearnList.ListIndex)
    Else
        MsgBox ("The learned keys list box is empty.")
    End If
End Sub

Private Sub SignatureOnly_Click()
    Call Port_Change ' force restart of TWIRP
End Sub
