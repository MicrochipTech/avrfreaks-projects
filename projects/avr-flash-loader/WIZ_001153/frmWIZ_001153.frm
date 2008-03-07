VERSION 5.00
Object = "{248DD890-BB45-11CF-9ABC-0080C7E7B78D}#1.0#0"; "MSWINSCK.OCX"
Begin VB.Form frmWIZ_001153 
   Caption         =   " WIZnet Contest 001153 - Bootloader Server"
   ClientHeight    =   5892
   ClientLeft      =   132
   ClientTop       =   420
   ClientWidth     =   10344
   Icon            =   "frmWIZ_001153.frx":0000
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   ScaleHeight     =   5892
   ScaleWidth      =   10344
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdClrLog 
      Caption         =   "Clear Log"
      Height          =   312
      Left            =   6900
      TabIndex        =   2
      Top             =   5160
      Width           =   1092
   End
   Begin VB.TextBox txtStatus 
      Height          =   288
      Left            =   4560
      Locked          =   -1  'True
      TabIndex        =   10
      TabStop         =   0   'False
      Top             =   660
      Width           =   2052
   End
   Begin VB.ListBox lstLog 
      Height          =   4464
      Left            =   6900
      TabIndex        =   9
      TabStop         =   0   'False
      Top             =   420
      Width           =   3072
   End
   Begin MSWinsockLib.Winsock tcpServer 
      Left            =   8220
      Top             =   5040
      _ExtentX        =   593
      _ExtentY        =   593
      _Version        =   393216
      LocalPort       =   19002
   End
   Begin VB.Frame frAuto 
      Caption         =   " AutoLoader "
      Height          =   672
      Left            =   360
      TabIndex        =   8
      Top             =   300
      Width           =   3972
      Begin VB.CommandButton cmdDisp 
         Caption         =   "Toggle Image"
         Height          =   252
         Left            =   1560
         TabIndex        =   23
         Top             =   270
         Width           =   1272
      End
      Begin VB.CommandButton cmdFiles 
         Caption         =   "Files..."
         Height          =   252
         Left            =   3060
         TabIndex        =   14
         Top             =   270
         Width           =   672
      End
      Begin VB.OptionButton optAuto 
         Caption         =   "Off"
         Height          =   192
         Index           =   0
         Left            =   300
         TabIndex        =   13
         Top             =   300
         Value           =   -1  'True
         Width           =   492
      End
      Begin VB.OptionButton optAuto 
         Caption         =   "On"
         Height          =   192
         Index           =   1
         Left            =   900
         TabIndex        =   1
         Top             =   300
         Width           =   612
      End
   End
   Begin VB.Frame frManual 
      Caption         =   " Manual Connection "
      Height          =   4272
      Left            =   360
      TabIndex        =   6
      Top             =   1200
      Width           =   6252
      Begin VB.Frame frFlashOps 
         Caption         =   " Flash Ops "
         Height          =   792
         Left            =   300
         TabIndex        =   20
         Top             =   2160
         Width           =   1092
         Begin VB.CommandButton cmdErase 
            Caption         =   "Erase"
            Height          =   222
            Left            =   180
            TabIndex        =   22
            Top             =   480
            Width           =   672
         End
         Begin VB.CheckBox chkWriteAll 
            Caption         =   "Wr All"
            Height          =   192
            Left            =   180
            TabIndex        =   21
            Top             =   240
            Value           =   1  'Checked
            Width           =   732
         End
      End
      Begin VB.CommandButton cmdConnect 
         Caption         =   "Connect"
         Height          =   252
         Left            =   300
         TabIndex        =   19
         Top             =   600
         Width           =   1092
      End
      Begin VB.OptionButton optMem 
         Caption         =   "EEPROM (0.5K)"
         Height          =   192
         Index           =   1
         Left            =   4440
         TabIndex        =   18
         Top             =   360
         Width           =   1512
      End
      Begin VB.OptionButton optMem 
         Caption         =   "Flash (16K)"
         Height          =   192
         Index           =   0
         Left            =   3240
         TabIndex        =   17
         Top             =   360
         Value           =   -1  'True
         Width           =   1152
      End
      Begin VB.CommandButton cmdMem 
         Caption         =   "Verify"
         Height          =   252
         Index           =   2
         Left            =   300
         TabIndex        =   16
         Top             =   1800
         Width           =   1092
      End
      Begin VB.CommandButton cmdImEdit 
         Caption         =   "Image Editor"
         Height          =   252
         Left            =   300
         TabIndex        =   15
         Top             =   3360
         Width           =   1092
      End
      Begin VB.CommandButton cmdJump 
         Caption         =   "Jump to App"
         Height          =   252
         Left            =   300
         TabIndex        =   5
         Top             =   3660
         Width           =   1092
      End
      Begin VB.CommandButton cmdMem 
         Caption         =   "Write"
         Height          =   252
         Index           =   1
         Left            =   300
         TabIndex        =   4
         Top             =   1500
         Width           =   1092
      End
      Begin VB.CommandButton cmdMem 
         Caption         =   "Read"
         Height          =   252
         Index           =   0
         Left            =   300
         TabIndex        =   3
         Top             =   1200
         Width           =   1092
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
         Height          =   3312
         Left            =   1620
         TabIndex        =   7
         TabStop         =   0   'False
         Top             =   600
         Width           =   4212
      End
      Begin VB.Label lbl 
         Caption         =   "Memory Image:"
         Height          =   192
         Index           =   1
         Left            =   1620
         TabIndex        =   12
         Top             =   360
         Width           =   1176
      End
   End
   Begin VB.CommandButton cmdEnd 
      Caption         =   "Quit"
      Height          =   312
      Left            =   8880
      TabIndex        =   0
      Top             =   5160
      Width           =   1092
   End
   Begin VB.Label lbl 
      Alignment       =   1  'Right Justify
      Caption         =   "Status:"
      Height          =   204
      Index           =   0
      Left            =   4560
      TabIndex        =   11
      Top             =   420
      Width           =   552
   End
End
Attribute VB_Name = "frmWIZ_001153"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'  WizNet 001153 - AVR Flash Loader
'
'  note: this is set up for the Atmel Butterfly, which uses an
'        ATmega169 MCU. the protected bootloader starts at 0x1c00.
'
'       command                action              originates
'       -------                ------              ----------
'         X                    erase app memory    host (PC)
'         A                    ack                 client
'         F page <data>        write page flash    host
'         E addr count <data>  write bytes EEPROM  host
'         f page               read page flash     host
'         e page               read page EEPROM    host
'         <data>               returned data       client
'         J                    jump to app         host
'
'       the client sends an Ack when it starts up, connects, and is ready.
'       pages are numbered 0-111 for flash and 0-3 for EEPROM.
'       pages are 64 words (128 bytes) long.
'       EEPROM addresses are 0 - 511. requested/written data is limited
'          to 128 bytes.
'       EEPROM addresses are 2 bytes, high first.
'       page numbers are 1 byte long.
'       maximum message length from client to host is 128 bytes.
'       maximum message length from host to client is 132 bytes.
'       <data> is usually 128 bytes, but can be less for eeprom access.
'
'       flash below bootloader is 0x0000 to 0x1bff words, or
'           0 to 7167 (0 to 14,335 bytes)
'       bootloader is 0x1c00 to 0x1fff words, or 1K words (2048 bytes)
'
Option Explicit

Sub StepState(recurse As Boolean)
' handle the automated sequences
' we come here when we receive something we were expecting
' set recurse to true to have caller call again, false for a normal
'   situation where we sent something and will get a response
  Dim c$, sent As Boolean, ertn As Integer, a$, udef As Boolean

  recurse = False
  If NxtState$ = "" Then Exit Sub           ' nothing to do
  
  c$ = Left$(NxtState$, 1)
  NxtState$ = Mid$(NxtState$, 2)            ' toss that one
  
  Select Case c$
    Case "A"                                ' ----- initial ACK on connection
      Call SetState(S_CONNECT2)
      lstLog.AddItem "Initial ACK received"
      If Not Auto Then
        Call Buttons(True)                  ' if manual, user guides us now
      Else
        recurse = True                      ' otherwise, start things going
      End If
    Case "a"                                ' ----- normal ACK with nothing else
    Case "b"                                ' ----- normal ACK, manual; end of ops
      Call Buttons(True)                    ' enable command buttons again
      Screen.MousePointer = 0               ' turn off hourglass
      Call SetState(S_CONNECT2)
    Case "f", "V"                           ' ----- read/verify flash image
      Call ShowFlash
      If c$ = "f" Then
        Call SetState(S_READ_F)
        verify = False
      Else
        Call SetState(S_VERIFY_F)
        verify = True
        vOK = True                          ' verify is good so far
      End If
      
      rPage = 0                             ' page we're requesting
      NxtState$ = "2" & NxtState$           ' what we'll do when it comes
      rFlag = FPAGESIZE                     ' expecting a page of bytes
      tcpServer.SendData "f" & Chr$(0)
    Case "2"                                ' ----- received flash page
      Call Recv_fPage(ertn)
      If ertn >= 0 Then
        Call Verify_Error(ertn)
        recurse = True                      ' we're breaking out - keep it going
      Else
        rPage = rPage + 1
        If rPage < FPAGES Then
          NxtState$ = "2" & NxtState$       ' do that again
          rFlag = FPAGESIZE
          a$ = "f" & Chr$(rPage)
          tcpServer.SendData a$
        Else
          If Not Auto Then
            NxtState$ = "b"                 ' if manual we're done
            If verify Then
              lstLog.AddItem "Verify successful"
            Else
              Call ShowFlash
            End If
          End If
          recurse = True                    ' done with command - do next
        End If
      End If
    
    Case "F"                                ' ----- write flash image
      Call ShowFlash
      Call SetState(S_WRITE_F)
      If Auto Then
        udef = True                         ' write all mem if auto
      Else
        udef = chkWriteAll.Value = 1        ' otherwise let them tell us
      End If
      Call Initiate_F_Write(udef, sent)     ' start the pages going, maybe
      
      If Not sent Then
        recurse = True                      ' if none, we need to advance
        If Not Auto Then NxtState$ = "b"
      End If
    Case "1"                                ' ----- secondary flash write
      rPage = rPage + 1                     ' next page up
      If Not fWrite Then Call Next_Page(rPage)  ' go find one with data
      
      If rPage >= 0 And rPage < FPAGES Then ' we have a real page
        NxtState$ = "1" & NxtState$         ' do this again
        rFlag = 0                           ' looking for Ack
        Call Send_fPage
      Else                                  ' we're out of pages
        If Not Auto Then NxtState$ = "b"    ' if manual, wrap it up
        recurse = True                      ' come back in for next command
      End If
    Case "e", "v"                           ' ----- read/verify EEPROM image
      Call ShowEEPROM
      If c$ = "e" Then
        Call SetState(S_READ_E)
        verify = False
      Else
        Call SetState(S_VERIFY_E)
        verify = True                       ' verify is OK for now
      End If
      
      NxtState$ = "3" & NxtState$           ' what we'll do when it comes
      rFlag = EPAGESIZE                     ' expecting a page of bytes
      rPage = 0                             ' page we are requesting
      tcpServer.SendData "e" & Chr$(rPage)
    Case "3"                                ' secondary EEPROM read/verify
      Call Recv_ePage(ertn)
      If ertn >= 0 Then
        Call Verify_Error(ertn)
        recurse = True
      Else
        rPage = rPage + 1
        If rPage < EPAGES Then
          NxtState$ = "3" & NxtState$       ' do that again
          rFlag = EPAGESIZE
          a$ = "e" & Chr$(rPage)
          tcpServer.SendData a$
        Else
          If Not Auto Then
            NxtState$ = "b"                 ' if manual we're done
            If verify Then
              lstLog.AddItem "Verify successful"
            Else
              Call ShowEEPROM
            End If
          End If
          recurse = True
        End If
      End If
    Case "E"                                ' ----- write EEPROM image
      Call ShowEEPROM
      Call SetState(S_WRITE_E)
      Call Initiate_E_Write(sent)           ' start the pages going, maybe
      If Not sent Then
        recurse = True                      ' if none, we need to advance
        If Not Auto Then NxtState$ = "b"
      End If
    Case "4"                                ' ----- secondary EEPROM writes
      eA1 = eA2 + 1
      Call Find_Range(eA1, eA2)             ' find next data
      If eA1 >= 0 Then                      ' we have some
        NxtState$ = "4" & NxtState$         ' do this again
        rFlag = 0                           ' looking for Ack
        Call Send_eRange
      Else                                  ' we're out of pages
        If Not Auto Then NxtState$ = "b"    ' if manual, wrap it up
        recurse = True                      ' come back in for next command
      End If
    Case "X"                                ' ----- erase flash
      Call SetState(S_ERASE)
      rFlag = 0                             ' wait for ACK
      tcpServer.SendData "X"
      Call ShowFlash
      If Not Auto Then NxtState$ = "b"
    Case "J"                                ' ----- jump to app
      Call SetState(S_JUMP)
      rFlag = 0                             ' wait for ack
      NxtState$ = "R"                       ' reset
      tcpServer.SendData "J"
    Case "R"                                ' ----- reset
      Call SetState(S_DEAD)
      If Auto Then                          ' we may need to update the patches
        If Not frmWIZ_001153_Files.OkToGo() Then
          MsgBox "Unexpected error reading file", 48, "AutoLoad"
          internal = True
          optAuto(0).Value = True             ' switch back to manual
          Auto = False
          internal = False
          frManual.Enabled = True
          cmdFiles.Enabled = True
          cmdFiles.SetFocus
        Else
          Call SetState(S_LISTEN)
        End If
      End If
    Case Else
  End Select

End Sub

Sub Verify_Error(ad As Integer)
' tell them the bad news
  Dim a$

  a$ = Right$("000" & Hex$(ad), 4)
  lstLog.AddItem "*** verify failure at " & a$ & " ***"
  If Not Auto Then NxtState$ = "b"
  
End Sub

Sub Initiate_E_Write(sent As Boolean)
' initiate an EEPROM write sequence
' returns sent = True if something sent, False otherwise
' format: E <addr> count <data>

  eA1 = 0                                 ' prime the pump
  Call Find_Range(eA1, eA2)               ' see what's defined in ee()
  sent = eA1 >= 0
  
  If sent Then                            ' we said we did, so we'd better
    NxtState$ = "4" & NxtState$           ' do secondary send(s) next
    rFlag = 0                             ' waiting for ACK
    Call Send_eRange
  End If

End Sub

Sub Send_eRange()
' send EEPROM data at ee(eA1) to ee(eA2)
' command is E <addr> count <data>
  Dim i As Integer, s$, n As Integer
  
  n = eA2 - eA1 + 1                       ' count
  s$ = "E" & Chr$((eA1 And &HF00) / 256) & Chr$(eA1 And &HFF) & Chr$(n)
  For i = eA1 To eA2
    s$ = s$ & Chr$(ee(i))
    Next i
  tcpServer.SendData s$

End Sub

Sub Recv_ePage(ertn As Integer)
' put away or verify a page of EEPROM
' returns ertn = -1 if not verify or OK, bad address if verify and bad
' for EEPROM, do not verify byte if ee(-) < 0
  Dim ad As Integer, i As Integer, v As Integer
  
  ertn = -1
  ad = rPage * EPAGESIZE                  ' page base address
  For i = 0 To EPAGESIZE - 1
    If verify Then
      v = ee(ad + i)                      ' value in image
      If v >= 0 Then
        If v <> Buf(i) Then
          ertn = ad + i                   ' address we don't like
          Exit For
        End If
      End If
    Else
      ee(ad + i) = Buf(i)                 ' we're reading
    End If
    Next i

End Sub

Sub Recv_fPage(ertn As Integer)
' put away or verify a page of flash
' returns ertn = -1 if not verify or OK, bad address if verify and bad
' for flash, fl(-) < 0 verifies and writes as 0xff
  Dim ad As Integer, i As Integer, v As Integer
  
  ertn = -1
  ad = rPage * FPAGESIZE                  ' page base address
  For i = 0 To FPAGESIZE - 1
    If verify Then
      v = fl(ad + i)                      ' value in image
      If (v And &HFF) <> Buf(i) Then
        ertn = ad + i
        Exit For
      End If
    Else
      fl(ad + i) = Buf(i)                 ' we're reading
    End If
    Next i

End Sub

Sub SetState(s As Integer)
  Dim a$
  
  If TCPstatus = s Then Exit Sub   ' avoids multiple closes (harmless, but...)
  
  Select Case s
   Case S_DEAD                     ' nada
     tcpServer.Close
     NxtState$ = ""
     rFlag = -1
     eCnt = 0
     a$ = " Disconnected"
     Call Buttons(False)
     cmdConnect.Caption = "Connect"
   Case S_LISTEN                   ' listening
     tcpServer.Close               ' be paranoid
     a$ = " Listening"
     rFlag = -1
     tcpServer.Listen
   Case S_CONNECT1                 ' connected
     a$ = " Connected - waiting ACK"
     rFlag = 0                     ' waiting ACK
     If Auto Then
       NxtState$ = "A" & GoWith$ & "J"    ' the automated sequence
     Else
       NxtState$ = "A"
     End If
   Case S_CONNECT2                 ' connected
     a$ = " Ready"
     rFlag = -1                    ' waiting ACK
   Case S_READ_F                   ' reading flash
     a$ = " Reading flash"
   Case S_READ_E                   ' reading EEPROM
     a$ = " Reading EEPROM"
   Case S_WRITE_E                  ' writing EEPROM
     a$ = " Writing EEPROM"
   Case S_WRITE_F                  ' writing flash
     a$ = " Writing flash"
   Case S_VERIFY_E                 ' verifying EEPROM
     a$ = " Verifying EEPROM"
   Case S_VERIFY_F                 ' verifying flash
     a$ = " Verifying flash"
   Case S_JUMP                     ' jumping to application
     a$ = " Jumping to app"
   Case S_ERASE                    ' erasing flash
     a$ = " Erasing flash"
   Case Else
     Exit Sub
  End Select
  
  TCPstatus = s
  txtStatus.Text = a$
  lstLog.AddItem Trim$(a$)
  lstLog.TopIndex = lstLog.ListCount - 1
  DoEvents
  
End Sub

Sub Send_fPage()
' send page rPage (0 to FPAGES-1) of flash, FPAGESIZE bytes
' format: F page <data>
  Dim i As Integer, a$, ad As Integer
  
  txtStatus.Text = " Writing page " & rPage
  a$ = String$(FPAGESIZE, 0)          ' build output here
  ad = rPage * FPAGESIZE              ' base page address
  For i = 0 To FPAGESIZE - 1
    Mid$(a$, i + 1, 1) = Chr$(fl(ad + i) And &HFF)
    Next i
  tcpServer.SendData "F" & Chr$(rPage) & a$

End Sub

Sub Initiate_F_Write(undef As Boolean, sent As Boolean)
' initiate a flash write sequence
' undef - if TRUE, send all pages; if false, only pages with defined values
' returns sent = True if something sent, False otherwise

  rPage = 0                               ' start with page 0
  fWrite = undef                          ' remember what we're sending
  If Not fWrite Then
    Call Next_Page(rPage)                 ' search for a good page
    If rPage < 0 Then
      sent = False                        ' didn't find one - nothing sent
      Exit Sub
    End If
  End If
  
  NxtState$ = "1" & NxtState$             ' do secondary page(s) next
  rFlag = 0                               ' waiting for ACK
  Call Send_fPage
  sent = True

End Sub

Sub Buttons(t As Boolean)
' set manual buttons
  Dim i As Integer
  
  For i = 0 To 2
    cmdMem(i).Enabled = t
    Next i
  cmdJump.Enabled = t
  cmdErase.Enabled = t
  
End Sub

Private Sub cmdClrLog_Click()
  lstLog.Clear
End Sub

Private Sub cmdDisp_Click()
  Dim w As Integer
  
  If optMem(0).Value Then w = 1 Else w = 0
  optMem(w).Value = True
  
End Sub

Private Sub cmdEnd_Click()
  tcpServer.Close
  End
End Sub

Private Sub cmdErase_Click()
  Dim recurse As Boolean
  
  Call Buttons(False)
  Screen.MousePointer = 11                  ' hourglass
  
  NxtState$ = "X"
  recurse = True
  Do While recurse
    Call StepState(recurse)
  Loop
  
End Sub

Private Sub cmdFiles_Click()
' show them the files screen

  frmWIZ_001153_Files.Show 1
  
End Sub

Private Sub cmdImEdit_Click()
' show them the image editor screen

  frmWIZ_001153_edit.Show 1
  If EditChg < 0 Then Exit Sub
  
  internal = True
  optMem(EditChg).Value = True
  internal = False
  Call ShowData(lstMem, EditChg)
  lstMem.TopIndex = EditTop
  
End Sub

Private Sub cmdJump_Click()
  Dim recurse As Boolean
  
  NxtState$ = "J"
  recurse = True
  Do While recurse
    Call StepState(recurse)
  Loop

End Sub

Private Sub cmdMem_Click(Index As Integer)
' index: 0 = read, 1 = write, 2 = verify
  Dim w As Integer, recurse As Boolean
  
  If optMem(0).Value Then w = 0 Else w = 1
  Call Buttons(False)                       ' disable the buttons
  Screen.MousePointer = 11                  ' hourglass
  
  If w = 0 Then
    NxtState$ = Mid$("fFV", Index + 1, 1)   ' flash
  Else
    NxtState$ = Mid$("eEv", Index + 1, 1)   ' EEPROM
  End If
  
  recurse = True
  Do While recurse
    Call StepState(recurse)
  Loop

End Sub

Private Sub Form_Load()
  Dim w As Integer
  
  internal = True                             ' in case someone cares
  Me.Left = (Screen.Width - Me.Width) / 2
  Me.Top = (Screen.Height - Me.Height) / 2
  tcpServer.Protocol = sckTCPProtocol
  tcpServer.LocalPort = 19002
  TCPstatus = -1
  eCnt = 0
  
  Load frmWIZ_001153_Files                    ' keep the files form up
  Call ClearEE                                ' set memory images undefined
  Call ClearFl
  If optMem(0).Value Then w = 0 Else w = 1
  Call ShowData(lstMem, w)                    ' fill the memory image
  Auto = False                                ' auto mode off
  Call SetState(S_DEAD)                       ' no connection or attempts
  optAuto(0).Value = True                     ' should already be selected
  internal = False
  
End Sub

Private Sub optAuto_Click(Index As Integer)
  Dim a$, v As Long
  
  If internal Then Exit Sub
  If (Not Auto) And (TCPstatus <> S_DEAD) Then
    a$ = "Turning on AutoLoad may discard the memory image(s)." & vbLf & "Continue?"
    v = vbQuestion Or vbYesNo
    If MsgBox(a$, v, "AutoLoad") <> vbYes Then Exit Sub
  End If
  
  Call SetState(S_DEAD)                   ' close the connection
  If optAuto(0).Value Then                ' they want manual mode
    frManual.Enabled = True
    Auto = False
  Else                                    ' they want auto mode
    If frmWIZ_001153_Files.OkToGo() Then
      Auto = True
    Else
      MsgBox "You must specify at least one file, with no errors", 48, "AutoLoad"
      internal = True
      optAuto(0).Value = True             ' switch back to manual
      Auto = False
      internal = False
      cmdFiles.SetFocus
    End If
  End If
  
  frManual.Enabled = Not Auto
  cmdFiles.Enabled = Not Auto
  If Auto Then
    cmdConnect.Caption = "Connect"
    Call Buttons(False)
    Call SetState(S_LISTEN)
  End If
  
End Sub

Sub ShowFlash()
' set the selection to flash and display it

  internal = True
  optMem(0).Value = 1                   ' may or may not cause event
  internal = False
  Call ShowData(lstMem, 0)
  
End Sub

Sub ShowEEPROM()
' set the selection to EEPROM and display it

  internal = True
  optMem(1).Value = 1                   ' may or may not cause event
  internal = False
  Call ShowData(lstMem, 1)
  
End Sub

Private Sub optMem_Click(Index As Integer)
  Dim w As Integer
  
  If internal Then Exit Sub
  If optMem(0).Value Then w = 0 Else w = 1
  Call ShowData(lstMem, w)
  
End Sub

Private Sub tcpServer_DataArrival(ByVal nbytes As Long)
  Dim s$, ok As Integer, i As Integer, recurse As Boolean
  
  If nbytes = 0 Then Exit Sub
  tcpServer.GetData s$, vbString
  If rFlag < 0 Then Exit Sub          ' wasn't expecting anything, sorry
  
  If nbytes = rFlag Then
    ok = 0
  Else
    If nbytes = 1 And rFlag = 0 Then
      If s$ = ACK Then
        ok = 0
      Else
        ok = -702
      End If
    Else
      ok = -701
    End If
  End If
  
  rFlag = -1
  If ok = 0 Then
    For i = 1 To nbytes
      Buf(i - 1) = Asc(Mid$(s$, i, 1))
      Next i
    nBuf = nbytes
'    DataIn = True
    recurse = True
    Do While recurse
      Call StepState(recurse)               ' advance to next operation
    Loop
  Else
    nBuf = 0
  End If

End Sub

Private Sub tcpServer_Error(ByVal num As Integer, desc As String, _
  ByVal Scode As Long, ByVal Source As String, ByVal HelpFile As String, _
  ByVal HelpContext As Long, CancelDisplay As Boolean)
  
  eCnt = eCnt + 1
  If eCnt < 5 Then
    MsgBox "Error: " & desc, 48, "TCP Error"
  End If
  
End Sub

Private Sub tcpServer_ConnectionRequest(ByVal requestID As Long)

  If tcpServer.State <> sckClosed Then tcpServer.Close
  tcpServer.Accept requestID
  Call SetState(S_CONNECT1)             ' connected, but no ACK yet

End Sub

Private Sub cmdConnect_Click()
' manual connect/disconnect button
' we can only get here in manual mode
  Dim a$
  
  Call SetState(S_DEAD)                        ' close the connection
  
  If cmdConnect.Caption = "Connect" Then
    Call SetState(S_LISTEN)
    a$ = "Disconnect"
  Else
    a$ = "Connect"
  End If
  cmdConnect.Caption = a$
  
End Sub

