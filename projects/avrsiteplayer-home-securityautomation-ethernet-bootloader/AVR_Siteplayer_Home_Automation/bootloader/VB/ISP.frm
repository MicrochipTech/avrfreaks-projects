VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Object = "{248DD890-BB45-11CF-9ABC-0080C7E7B78D}#1.0#0"; "MSWINSCK.OCX"
Object = "{EAB22AC0-30C1-11CF-A7EB-0000C05BAE0B}#1.1#0"; "shdocvw.dll"
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form Form1 
   Caption         =   "Atmel ISP"
   ClientHeight    =   3390
   ClientLeft      =   4770
   ClientTop       =   8475
   ClientWidth     =   9660
   Icon            =   "ISP.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   3390
   ScaleWidth      =   9660
   Begin VB.TextBox PageSize 
      Height          =   285
      Left            =   4080
      TabIndex        =   32
      Text            =   "128"
      Top             =   3000
      Width           =   615
   End
   Begin VB.Timer Timer1 
      Enabled         =   0   'False
      Interval        =   10
      Left            =   1080
      Top             =   3480
   End
   Begin VB.CheckBox reset_toggle 
      Caption         =   "Toggle Reset"
      Height          =   255
      Left            =   4800
      TabIndex        =   30
      Top             =   600
      Width           =   1695
   End
   Begin VB.CommandButton en_udp 
      Caption         =   "Enable UDP"
      Height          =   615
      Left            =   1200
      TabIndex        =   29
      Top             =   240
      Width           =   1095
   End
   Begin MSComctlLib.ProgressBar program_status 
      Height          =   495
      Left            =   1200
      TabIndex        =   24
      Top             =   1320
      Width           =   5415
      _ExtentX        =   9551
      _ExtentY        =   873
      _Version        =   393216
      Appearance      =   1
      Scrolling       =   1
   End
   Begin VB.CheckBox program_mode 
      Caption         =   "Enter Program Mode"
      Height          =   375
      Left            =   4800
      TabIndex        =   23
      Top             =   240
      Width           =   1815
   End
   Begin VB.CommandButton reset 
      Caption         =   "Reset  Device"
      Height          =   615
      Left            =   3600
      TabIndex        =   22
      Top             =   240
      Width           =   1095
   End
   Begin VB.CommandButton Program 
      Caption         =   "Program Device"
      Height          =   615
      Left            =   4200
      TabIndex        =   21
      Top             =   2040
      Width           =   1095
   End
   Begin VB.TextBox filedata 
      Height          =   2775
      Left            =   2160
      MultiLine       =   -1  'True
      ScrollBars      =   2  'Vertical
      TabIndex        =   20
      Top             =   4920
      Width           =   5655
   End
   Begin VB.TextBox numbytes 
      Height          =   375
      Left            =   7080
      TabIndex        =   18
      Top             =   2520
      Width           =   855
   End
   Begin VB.CommandButton set_baud 
      Caption         =   "Set Baudrate 38.4kbps"
      Height          =   615
      Left            =   2400
      TabIndex        =   17
      Top             =   240
      Width           =   1095
   End
   Begin VB.CommandButton Command2 
      Caption         =   "Send 01"
      Height          =   495
      Left            =   8280
      TabIndex        =   16
      Top             =   5040
      Visible         =   0   'False
      Width           =   855
   End
   Begin SHDocVwCtl.WebBrowser WB 
      Height          =   615
      Left            =   8280
      TabIndex        =   15
      Top             =   3960
      Visible         =   0   'False
      Width           =   615
      ExtentX         =   1085
      ExtentY         =   1085
      ViewMode        =   0
      Offline         =   0
      Silent          =   0
      RegisterAsBrowser=   0
      RegisterAsDropTarget=   1
      AutoArrange     =   0   'False
      NoClientEdge    =   0   'False
      AlignLeft       =   0   'False
      NoWebView       =   0   'False
      HideFileNames   =   0   'False
      SingleClick     =   0   'False
      SingleSelection =   0   'False
      NoFolders       =   0   'False
      Transparent     =   0   'False
      ViewID          =   "{0057D0E0-3573-11CF-AE69-08002B2E1262}"
      Location        =   ""
   End
   Begin VB.TextBox Data 
      Height          =   375
      Left            =   8280
      TabIndex        =   7
      Top             =   2520
      Width           =   1095
   End
   Begin VB.CommandButton Bind 
      Caption         =   "Bind"
      Height          =   375
      Left            =   7800
      TabIndex        =   6
      Top             =   1560
      Width           =   1095
   End
   Begin VB.TextBox DestAddr 
      Height          =   375
      Left            =   2280
      TabIndex        =   5
      Top             =   4080
      Visible         =   0   'False
      Width           =   1455
   End
   Begin MSWinsockLib.Winsock Winsock1 
      Left            =   360
      Top             =   4560
      _ExtentX        =   741
      _ExtentY        =   741
      _Version        =   393216
      Protocol        =   1
   End
   Begin VB.TextBox RemotePort 
      Height          =   285
      Left            =   8640
      TabIndex        =   4
      Top             =   1080
      Width           =   855
   End
   Begin VB.TextBox RemoteIP 
      Height          =   285
      Left            =   7080
      TabIndex        =   3
      Top             =   1080
      Width           =   1215
   End
   Begin VB.TextBox LocalPort 
      Height          =   285
      Left            =   8640
      TabIndex        =   2
      Top             =   480
      Width           =   855
   End
   Begin VB.TextBox LocalIP 
      Height          =   285
      Left            =   7080
      TabIndex        =   1
      Top             =   480
      Width           =   1215
   End
   Begin MSComDlg.CommonDialog CommonDialog1 
      Left            =   360
      Top             =   3960
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.CommandButton Command1 
      Caption         =   "Open File"
      Height          =   615
      Left            =   2280
      TabIndex        =   0
      Top             =   2040
      Width           =   1095
   End
   Begin VB.Label Label14 
      Caption         =   "Proccesor Page Size"
      Height          =   260
      Left            =   2400
      TabIndex        =   33
      Top             =   3030
      Width           =   1695
   End
   Begin VB.Shape reset_led 
      BackColor       =   &H00008000&
      BackStyle       =   1  'Opaque
      BorderWidth     =   3
      Height          =   255
      Left            =   360
      Shape           =   3  'Circle
      Top             =   3000
      Width           =   255
   End
   Begin VB.Shape prog_done 
      BackColor       =   &H00008000&
      BackStyle       =   1  'Opaque
      BorderWidth     =   3
      Height          =   255
      Left            =   360
      Shape           =   3  'Circle
      Top             =   2160
      Width           =   255
   End
   Begin VB.Label Label13 
      Alignment       =   2  'Center
      Caption         =   "Device in Reset"
      Height          =   375
      Left            =   120
      TabIndex        =   31
      Top             =   2520
      Width           =   735
   End
   Begin VB.Line Line8 
      X1              =   6960
      X2              =   9600
      Y1              =   3120
      Y2              =   3120
   End
   Begin VB.Line Line7 
      X1              =   9600
      X2              =   9600
      Y1              =   3120
      Y2              =   2160
   End
   Begin VB.Line Line6 
      X1              =   6960
      X2              =   6960
      Y1              =   3120
      Y2              =   2160
   End
   Begin VB.Line Line5 
      X1              =   6960
      X2              =   9600
      Y1              =   2160
      Y2              =   2160
   End
   Begin VB.Line Line4 
      X1              =   6960
      X2              =   9600
      Y1              =   120
      Y2              =   120
   End
   Begin VB.Line Line3 
      X1              =   9600
      X2              =   9600
      Y1              =   120
      Y2              =   2040
   End
   Begin VB.Line Line2 
      X1              =   9600
      X2              =   6960
      Y1              =   2040
      Y2              =   2040
   End
   Begin VB.Line Line1 
      X1              =   6960
      X2              =   6960
      Y1              =   120
      Y2              =   2040
   End
   Begin VB.Shape ready_device 
      BackColor       =   &H00008000&
      BackStyle       =   1  'Opaque
      BorderWidth     =   3
      Height          =   255
      Left            =   360
      Shape           =   3  'Circle
      Top             =   1320
      Width           =   255
   End
   Begin VB.Shape ready_prog 
      BackColor       =   &H00008000&
      BackStyle       =   1  'Opaque
      BorderWidth     =   3
      Height          =   255
      Left            =   360
      Shape           =   3  'Circle
      Top             =   480
      Width           =   255
   End
   Begin VB.Label Label12 
      Caption         =   "Programing Status"
      Height          =   255
      Left            =   1200
      TabIndex        =   28
      Top             =   1080
      Width           =   1455
   End
   Begin VB.Label Label11 
      Alignment       =   2  'Center
      Caption         =   "Program Succesful"
      Height          =   495
      Left            =   120
      TabIndex        =   27
      Top             =   1680
      Width           =   855
   End
   Begin VB.Label Label10 
      Alignment       =   2  'Center
      Caption         =   "Device Ready"
      Height          =   495
      Left            =   120
      TabIndex        =   26
      Top             =   840
      Width           =   735
   End
   Begin VB.Label Label7 
      Alignment       =   2  'Center
      Caption         =   "File Ready"
      Height          =   495
      Left            =   240
      TabIndex        =   25
      Top             =   0
      Width           =   495
   End
   Begin VB.Label Label9 
      Caption         =   "Total Bytes"
      Height          =   375
      Left            =   7080
      TabIndex        =   19
      Top             =   2280
      Width           =   975
   End
   Begin VB.Label Label8 
      Caption         =   "Data Returned"
      Height          =   255
      Left            =   8280
      TabIndex        =   14
      Top             =   2280
      Width           =   1335
   End
   Begin VB.Label Label6 
      Caption         =   "Hex"
      Height          =   255
      Left            =   3840
      TabIndex        =   13
      Top             =   4200
      Visible         =   0   'False
      Width           =   855
   End
   Begin VB.Label Label5 
      Caption         =   "Destination Address"
      Height          =   255
      Left            =   2280
      TabIndex        =   12
      Top             =   3720
      Visible         =   0   'False
      Width           =   1575
   End
   Begin VB.Label Label4 
      Caption         =   "Remote Port"
      Height          =   255
      Left            =   8640
      TabIndex        =   11
      Top             =   840
      Width           =   975
   End
   Begin VB.Label Label3 
      Caption         =   "Remote IP"
      Height          =   255
      Left            =   7080
      TabIndex        =   10
      Top             =   840
      Width           =   1095
   End
   Begin VB.Label Label2 
      Caption         =   "Local Port"
      Height          =   255
      Left            =   8640
      TabIndex        =   9
      Top             =   240
      Width           =   855
   End
   Begin VB.Label Label1 
      Caption         =   "Local IP"
      Height          =   255
      Left            =   7080
      TabIndex        =   8
      Top             =   240
      Width           =   1215
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'Copyright (C) 2005  Edward Welch welcher@edjusted.com
'
'This program is free software; you can redistribute it and/or
'modify it under the terms of the GNU General Public License
'as published by the Free Software Foundation; either version 2
'of the License, or (at your option) any later version.
'
'This program is distributed in the hope that it will be useful,
'but WITHOUT ANY WARRANTY; without even the implied warranty of
'MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
'GNU General Public License for more details.
'
'You should have received a copy of the GNU General Public License
'along with this program; if not, write to the Free Software
'Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.



Dim gcdg As Object
Dim mFileSize As Long
Dim arrByte() As Byte
Dim returnByte() As Byte
Dim LocPort As Integer
Dim RemPort As Integer
Dim RemIP As Variant
Dim remainder As Integer
Dim pages As Integer
Dim l As Integer
Dim DataFromSP As Byte
Dim pointer As Integer
Dim lpage As Integer
Dim prog_ready As Boolean
Dim reset1 As Boolean
Dim reset2 As Boolean

Private Type LongS
l1 As Long
End Type

Private Type Bytes
b1 As Byte
b2 As Byte
b3 As Byte
b4 As Byte
End Type

Dim s As String
Dim bb As Bytes
Dim ll As LongS

Dim NoBytes As Byte

Dim d1 As Byte
Dim d2 As Byte
Dim d3 As Byte

''''''''''''''''''''''''''''''''''''''''''''
'Re-Bind Ports to Values other than default'
''''''''''''''''''''''''''''''''''''''''''''
Private Sub Bind_Click()
    
    LocPort = LocalPort.Text
    RemPort = RemotePort.Text
    RemIP = RemoteIP.Text
    With Winsock1
        .Close
        .Bind LocPort, LocIP
        .RemoteHost = RemIP
        .RemotePort = RemPort
    End With

End Sub


Private Sub en_udp_Click()
    
    WB.Navigate "192.168.0.193/factory.spi?UDPrcvr=1"
    
End Sub

'''''''''''''''''''''''''''''''''''''''''''''
'Checkbox to manually enter programming mode'
'''''''''''''''''''''''''''''''''''''''''''''
Private Sub program_mode_Click()
    
    If program_mode.Value = 1 Then
        WB.Navigate "192.168.0.193/factory.spi?io4=0"
    End If
    If program_mode.Value = 0 Then
        WB.Navigate "192.168.0.193/factory.spi?io4=1"
    End If

End Sub

''''''''''''''''''''''''''''
'Depreciated Debug Function'
''''''''''''''''''''''''''''
Private Sub Command2_Click()
    
    WB.Navigate "192.168.0.193/factory.spi?com=%01"
    
End Sub

'''''''''''''''''''''''
'Form Load, Initialize'
'''''''''''''''''''''''
Private Sub Form_Load()
       
    l = 0
    Set gcdg = CommonDialog1
    LocIP = Winsock1.LocalIP
    LocalIP.Text = LocIP
    LocPort = 2552
    LocalPort.Text = LocPort
    RemPort = 26482
    RemotePort.Text = RemPort
    RemIP = "192.168.0.193"
    RemoteIP.Text = RemIP
    With Winsock1
        .Bind LocPort, LocIP
        .RemoteHost = RemIP
        .RemotePort = RemPort
    End With
    DestAddr.Text = "FF19" 'SP Destination Address in hex
    WB.Navigate "192.168.0.193/factory.spi?UDPrcvr=1"
    program.Enabled = False
    reset1 = False
    
End Sub

''''''''''''''''''''''''''
'Program Device Processor'
''''''''''''''''''''''''''

Private Sub Program_Click()

    filedata.Text = ""

    'Debug.Print "############################################"
    lpage = 0
    pointer = 0
    prog_ready = False
    
    program_status.Max = mFileSize + remainder
    program_status.Value = 0
    
    'Set Program I/O line Low
    WB.Navigate "192.168.0.193/factory.spi?io4=0"
    
    Call reset_device
    
    'The rest of the program will be carried out in the winsock1 data arrival routine
    
End Sub

'''''''''''''''''''''
'Continue Programing'
'''''''''''''''''''''

Sub start_program()

    'Set Program I/O line High
    WB.Navigate "192.168.0.193/factory.spi?io4=1"

    With Winsock1
        .Close
        .Bind LocPort, LocIP
        .RemoteHost = RemIP
        .RemotePort = RemPort
    End With
        d1 = 4
        ll.l1 = Val("&H" & "FF19") 'FF19 is SP COM Destination Address in hex
        LSet bb = ll
        NoBytes = 1
        s = Chr(NoBytes) & Chr(255 - NoBytes) & _
        Chr(bb.b1) & Chr(bb.b2) & _
        Chr(d1) & _
        Chr(0) & Chr(0)
        Winsock1.SendData s

End Sub

''''''''''''''''''''''''
'Send byte to processor'
''''''''''''''''''''''''



Sub send_page(pointer As Integer)

    
    With Winsock1
        .Close
        .Bind LocPort, LocIP
        .RemoteHost = RemIP
        .RemotePort = RemPort
   End With
        d1 = arrByte(pointer)
        ll.l1 = Val("&H" & "FF19") 'FF19 is SP COM Destination Address in hex
        LSet bb = ll
        NoBytes = 1
        s = Chr(NoBytes) & Chr(255 - NoBytes) & _
        Chr(bb.b1) & Chr(bb.b2) & _
        Chr(d1) & _
        Chr(0) & Chr(0)
        Winsock1.SendData s
       
End Sub

''''''''''''''''''''''''''''''''
'Send Byte Command to processor'
''''''''''''''''''''''''''''''''

Sub send_command(command As Integer)
        
        With Winsock1
        .Close
        .Bind LocPort, LocIP
        .RemoteHost = RemIP
        .RemotePort = RemPort
        End With
        d1 = command
        ll.l1 = Val("&H" & "FF19") 'FF19 is SP COM Destination Address in hex
        LSet bb = ll
        NoBytes = 1
        s = Chr(NoBytes) & Chr(255 - NoBytes) & _
        Chr(bb.b1) & Chr(bb.b2) & _
        Chr(d1) & _
        Chr(0) & Chr(0)
        Winsock1.SendData s
        
End Sub

''''''''''''''''''''''''
'Reset Processor Button'
''''''''''''''''''''''''

Private Sub reset_Click()
   
   Call reset_device

End Sub


''''''''''''''''''''''''
'Reset Device Processor'
''''''''''''''''''''''''

Sub reset_device()
    
    
    'Toggle SP IO3 to Hold Processor in Reset
    
    'Toggle IO3 hi/low/hi to reset processor
    If reset1 = False Then
    
        With Winsock1
            .Close
            .Bind LocPort, LocIP
            .RemoteHost = RemIP
            .RemotePort = RemPort
        End With
            d1 = 0
            ll.l1 = Val("&H" & "FF13") 'FF19 is SP COM Destination Address in hex
            LSet bb = ll
            NoBytes = 1
            s = Chr(NoBytes) & Chr(255 - NoBytes) & _
            Chr(bb.b1) & Chr(bb.b2) & _
            Chr(d1) & _
            Chr(0) & Chr(0)
            Winsock1.SendData s
        '  Dim k As Long
         ' For k = 1 To 100000
          'k = k + 1
         ' Next k
         ' k = 0
        Timer1.Enabled = True
        
'         With Winsock1
'            .Close
'            .Bind LocPort, LocIP
'            .RemoteHost = RemIP
'            .RemotePort = RemPort
'        End With
'            d1 = 1
'            ll.l1 = Val("&H" & "FF13") 'FF19 is SP COM Destination Address in hex
'            LSet bb = ll
'            NoBytes = 1
'            s = Chr(NoBytes) & Chr(255 - NoBytes) & _
'            Chr(bb.b1) & Chr(bb.b2) & _
'            Chr(d1) & _
'            Chr(0) & Chr(0)
'            Winsock1.SendData s
'            Exit Sub
    
    End If
    
    
    
    If reset1 = True And reset2 = False Then
    
        With Winsock1
            .Close
            .Bind LocPort, LocIP
            .RemoteHost = RemIP
            .RemotePort = RemPort
        End With
            d1 = 0
            ll.l1 = Val("&H" & "FF13") 'FF19 is SP COM Destination Address in hex
            LSet bb = ll
            NoBytes = 1
            s = Chr(NoBytes) & Chr(255 - NoBytes) & _
            Chr(bb.b1) & Chr(bb.b2) & _
            Chr(d1) & _
            Chr(0) & Chr(0)
            Winsock1.SendData s
        
        reset_led.BackColor = vbGreen
        reset2 = True
        Exit Sub
    
    End If
            
    'Toggle SP IO3 to let processor out of Reset
    If reset1 = True And reset2 = True Then
       
        With Winsock1
            .Close
            .Bind LocPort, LocIP
            .RemoteHost = RemIP
            .RemotePort = RemPort
        End With
            d1 = 1
            ll.l1 = Val("&H" & "FF13") 'FF19 is SP COM Destination Address in hex
            LSet bb = ll
            NoBytes = 1
            s = Chr(NoBytes) & Chr(255 - NoBytes) & _
            Chr(bb.b1) & Chr(bb.b2) & _
            Chr(d1) & _
            Chr(0) & Chr(0)
            Winsock1.SendData s
            
        reset_led.BackColor = RGB(0, 100, 0)
        reset2 = False
        Exit Sub
    
    End If
    
    
    
    
    
    
    

End Sub

'''''''''''''''''''''''
'Open Binary Data File'
'''''''''''''''''''''''

Private Sub Command1_Click()
 
 
    Dim mHandle
    Dim tempint As Integer
    mHandle = FreeFile
       
    gcdg.Flags = cdlOFNFileMustExist
    gcdg.FileName = ""
    gcdg.ShowOpen
    If gcdg.FileName = "" Then
        Exit Sub
    End If
         
    Open gcdg.FileName For Binary As #mHandle
    mFileSize = LOF(mHandle)
    If mFileSize = 0 Then
         Close mHandle
         MsgBox "Empty file"
         Exit Sub
    End If
    Screen.MousePointer = vbHourglass
    
    'Calculate # of pages, and number of bytes last page is short of being a full page
    tempint = PageSize.Text
    pages = mFileSize / tempint
    remainder = (mFileSize Mod tempint)
    remainder = tempint - remainder
    If remainder > (tempint / 2) Then pages = pages + 1  'VB automatically rounds integer values, adjust accordingly
     
    'Read File into array arrByte
    ReDim arrByte(1 To (mFileSize + remainder))
    Get #mHandle, , arrByte
    Close mHandle
     
    'Add 0xFF to end of file such that the last page is full
    For i = mFileSize + 1 To mFileSize + remainder
    arrByte(i) = 255
    Next i
    Screen.MousePointer = vbDefault
    
    numbytes.Text = mFileSize
    ready_prog.BackColor = vbGreen
    program.Enabled = True
     
    ready_device.BackColor = RGB(0, 100, 0)
    prog_done.BackColor = RGB(0, 100, 0)
     
    
    
End Sub

''''''''''''''''''''''''''''''''
'Toggle Hold Processor in Reset'
''''''''''''''''''''''''''''''''

Private Sub reset_toggle_Click()
    
    If reset_toggle.Value = 1 Then
        reset1 = True
    End If
    
    If reset_toggle.Value = 0 Then
        reset1 = False
    End If
        
    

End Sub

''''''''''''''''''''''''''
'Manually Set SP Baudrate'
''''''''''''''''''''''''''

Private Sub set_baud_Click()
    
    WB.Navigate "192.168.0.193/factory.spi?baud=65503"

End Sub

Private Sub Timer1_Timer()
        With Winsock1
            .Close
            .Bind LocPort, LocIP
            .RemoteHost = RemIP
            .RemotePort = RemPort
        End With
            d1 = 1
            ll.l1 = Val("&H" & "FF13")
            LSet bb = ll
            NoBytes = 1
            s = Chr(NoBytes) & Chr(255 - NoBytes) & _
            Chr(bb.b1) & Chr(bb.b2) & _
            Chr(d1) & _
            Chr(0) & Chr(0)
            Winsock1.SendData s
        Timer1.Enabled = False
End Sub

''''''''''''''''''''''''''''
'UDP Packet Arrival Handler'
''''''''''''''''''''''''''''

    Private Sub Winsock1_DataArrival(ByVal bytesTotal As Long)

    Winsock1.GetData DataFromSP ', vbString
    Data.Text = DataFromSP

    '0x02 Device ready for number of pages
    If DataFromSP = 2 Then
        Call send_command(pages)
        prog_ready = True
        ready_device.BackColor = vbGreen
    End If

    '0x03 Device ready for 1st page
    If DataFromSP = 3 Then
        If program_mode.Value = False Then
            Call start_program
        End If
    End If

    '0x05 Device ready for next byte
    If DataFromSP = 5 Then
        pointer = pointer + 1
        Call send_page(pointer)
        program_status.Value = program_status.Value + 1
    End If
    
    '0x06 Device ready for next page
    If DataFromSP = 6 Then
        send_command (4)
    End If

    '0x09 Device programmed succesfully
    If DataFromSP = 9 Then
        program.Enabled = False
        prog_done.BackColor = vbGreen
    End If


End Sub

