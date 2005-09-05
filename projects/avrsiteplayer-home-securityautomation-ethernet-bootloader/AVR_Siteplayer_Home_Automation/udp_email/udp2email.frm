VERSION 5.00
Object = "{20C62CAE-15DA-101B-B9A8-444553540000}#1.1#0"; "MSMAPI32.OCX"
Object = "{248DD890-BB45-11CF-9ABC-0080C7E7B78D}#1.0#0"; "MSWINSCK.OCX"
Begin VB.Form udp2email 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "UDP 2 Email"
   ClientHeight    =   2085
   ClientLeft      =   150
   ClientTop       =   720
   ClientWidth     =   5190
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   ScaleHeight     =   2085
   ScaleWidth      =   5190
   StartUpPosition =   3  'Windows Default
   Begin VB.Timer Timer4 
      Enabled         =   0   'False
      Interval        =   60000
      Left            =   4440
      Top             =   1680
   End
   Begin VB.Timer Timer3 
      Enabled         =   0   'False
      Interval        =   60000
      Left            =   3840
      Top             =   1680
   End
   Begin VB.Timer Timer2 
      Enabled         =   0   'False
      Interval        =   60000
      Left            =   3120
      Top             =   1680
   End
   Begin VB.Timer Timer1 
      Interval        =   2000
      Left            =   120
      Top             =   1680
   End
   Begin VB.CommandButton cmdSend 
      Caption         =   "&Send"
      Height          =   255
      Left            =   4320
      TabIndex        =   4
      ToolTipText     =   "Manually send e-mail"
      Top             =   120
      Visible         =   0   'False
      Width           =   735
   End
   Begin VB.CommandButton cmdClear 
      Caption         =   "&Clear"
      Height          =   255
      Left            =   0
      TabIndex        =   15
      ToolTipText     =   "Clear E-Mail Text"
      Top             =   1320
      Width           =   615
   End
   Begin VB.CheckBox chkAutoSend 
      Caption         =   "&AutoSend"
      Height          =   255
      Left            =   4080
      TabIndex        =   3
      ToolTipText     =   "Check to automatically send e-mail when UDP received"
      Top             =   120
      Value           =   1  'Checked
      Width           =   1095
   End
   Begin MSWinsockLib.Winsock Winsock1 
      Left            =   1200
      Top             =   1680
      _ExtentX        =   741
      _ExtentY        =   741
      _Version        =   393216
      Protocol        =   1
   End
   Begin MSMAPI.MAPIMessages mapiMess 
      Left            =   2280
      Top             =   1560
      _ExtentX        =   1005
      _ExtentY        =   1005
      _Version        =   393216
      AddressEditFieldCount=   1
      AddressModifiable=   0   'False
      AddressResolveUI=   0   'False
      FetchSorted     =   0   'False
      FetchUnreadOnly =   0   'False
   End
   Begin MSMAPI.MAPISession mapiSess 
      Left            =   1680
      Top             =   1560
      _ExtentX        =   1005
      _ExtentY        =   1005
      _Version        =   393216
      DownloadMail    =   -1  'True
      LogonUI         =   0   'False
      NewSession      =   0   'False
   End
   Begin VB.TextBox txtOutput 
      Appearance      =   0  'Flat
      Height          =   975
      Left            =   660
      Locked          =   -1  'True
      MultiLine       =   -1  'True
      ScrollBars      =   2  'Vertical
      TabIndex        =   8
      Top             =   1080
      Width           =   4515
   End
   Begin VB.TextBox txtSubject 
      Appearance      =   0  'Flat
      Height          =   285
      Left            =   660
      TabIndex        =   7
      Top             =   840
      Width           =   4515
   End
   Begin VB.TextBox txtTo 
      Appearance      =   0  'Flat
      Height          =   285
      Left            =   660
      TabIndex        =   6
      Top             =   600
      Width           =   4515
   End
   Begin VB.TextBox txtFrom 
      Appearance      =   0  'Flat
      Height          =   285
      Left            =   660
      TabIndex        =   5
      Top             =   360
      Width           =   4515
   End
   Begin VB.CommandButton cmdBind 
      Caption         =   "Bind &Port"
      Height          =   255
      Left            =   3120
      TabIndex        =   2
      ToolTipText     =   "Bind Port to start UDP reception"
      Top             =   120
      Width           =   915
   End
   Begin VB.TextBox txtPort 
      Appearance      =   0  'Flat
      Height          =   285
      Left            =   2580
      TabIndex        =   1
      Top             =   120
      Width           =   495
   End
   Begin VB.TextBox txtIP 
      Appearance      =   0  'Flat
      Height          =   285
      Left            =   660
      TabIndex        =   0
      Top             =   120
      Width           =   1275
   End
   Begin VB.Label Label6 
      Caption         =   "Text:"
      Height          =   255
      Left            =   60
      TabIndex        =   14
      ToolTipText     =   "E-mail Text - This is overwritten whenever a UDP packet is received"
      Top             =   1080
      Width           =   615
   End
   Begin VB.Label Label5 
      Caption         =   "PC Port:"
      Height          =   255
      Left            =   1980
      TabIndex        =   13
      ToolTipText     =   "Port number to use on this PC for UDP reception"
      Top             =   120
      Width           =   615
   End
   Begin VB.Label Label4 
      Caption         =   "PC IP:"
      Height          =   255
      Left            =   60
      TabIndex        =   12
      ToolTipText     =   "IP address of this PC's Network Card"
      Top             =   120
      Width           =   555
   End
   Begin VB.Label Label3 
      Caption         =   "Subject:"
      Height          =   255
      Left            =   60
      TabIndex        =   11
      ToolTipText     =   "Subject line of e-mail message"
      Top             =   840
      Width           =   555
   End
   Begin VB.Label Label2 
      Caption         =   "To:"
      Height          =   255
      Left            =   60
      TabIndex        =   10
      ToolTipText     =   "E-mail address of recepients separated by a "";"" semicolon"
      Top             =   600
      Width           =   555
   End
   Begin VB.Label Label1 
      Caption         =   "From:"
      Height          =   255
      Left            =   60
      TabIndex        =   9
      ToolTipText     =   "Profile name of sender"
      Top             =   360
      Width           =   615
   End
   Begin VB.Menu mnuFile 
      Caption         =   "&File"
      Begin VB.Menu mnuExit 
         Caption         =   "E&xit"
      End
   End
   Begin VB.Menu mnuHelp 
      Caption         =   "&Help"
      Begin VB.Menu mnuAbout 
         Caption         =   "&About"
      End
   End
End
Attribute VB_Name = "udp2email"
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



Option Explicit
Option Base 1
Dim txt_countout As Integer
Dim data(10) As String
Dim message_count As Integer
Dim temp_flag As Boolean
Dim byte_count As Integer
Dim in_temp As Integer
Dim in_temp1 As Integer
Dim out_temp As Integer
Dim out_temp1 As Integer




Private Sub chkAutoSend_Click()
    On Error Resume Next
    If chkAutoSend.Value = vbUnchecked Then
        cmdSend.Visible = True
        txtOutput.Locked = False
    Else
        cmdSend.Visible = False
        txtOutput.Locked = True
    End If
End Sub

Private Sub cmdBind_Click()
    On Error GoTo errBind
    If cmdBind.Caption = "Bind &Port" Then
        'Set winsock for UDP protocol
        Winsock1.Protocol = sckUDPProtocol
        'Bind to computer's IP and Port number
        Winsock1.Bind txtPort.Text, txtIP.Text
        cmdBind.Caption = "Close &Port"
    Else
        Winsock1.Close
        cmdBind.Caption = "Bind &Port"
    End If
    Exit Sub
    
errBind:
    MsgBox "Error: " & Err.Number & ", " & Err.Description
End Sub

Private Sub cmdClear_Click()
    On Error Resume Next
    txtOutput.Text = ""
End Sub

Private Sub cmdSend_Click()
    Dim emailData As String
    On Error Resume Next
    emailData = txtOutput.Text
    Call SendFromUDP(emailData)
End Sub


Private Sub Form_Load()
    On Error Resume Next
    'Get settings from registry
    'Call cmdBind_Click
    message_count = 1
    
    udp2email.Left = GetSetting("udp2email", "Startup", "Left", 25)
    udp2email.Top = GetSetting("udp2email", "Startup", "Top", 25)
    chkAutoSend.Value = GetSetting("udp2email", "Startup", "AutoSend", vbUnchecked)
    txtIP.Text = GetSetting("udp2email", "Startup", "IP", "192.168.1.200")
    txtPort.Text = GetSetting("udp2email", "Startup", "Port", "2552")
    txtFrom.Text = GetSetting("udp2email", "Startup", "From", "")
    txtTo.Text = GetSetting("udp2email", "Startup", "To", "")
    txtSubject.Text = GetSetting("udp2email", "Startup", "Subject", "SitePlayer UDP Message")
    chkAutoSend.Value = vbChecked
    If chkAutoSend.Value = vbUnchecked Then
        cmdSend.Visible = True
        txtOutput.Locked = False
    End If
    If cmdBind.Caption = "Bind &Port" Then
        'Set winsock for UDP protocol
        Winsock1.Protocol = sckUDPProtocol
        'Bind to computer's IP and Port number
        Winsock1.Bind txtPort.Text, txtIP.Text
        cmdBind.Caption = "Close &Port"
    Else
        Winsock1.Close
        cmdBind.Caption = "Bind &Port"
    End If
    
    Open App.Path & "\security_log.txt" For Append As #1
    Write #1, Now, "Program Started"
    Close #1
               
    temp_flag = False

End Sub

Private Sub Form_Unload(Cancel As Integer)
    Call mnuExit_Click
End Sub

Private Sub mnuAbout_Click()
    MsgBox "UDP2EMAIL (C) Copyright NetMedia Inc. 2001", vbInformation, "UDP2EMAIL"
End Sub

Private Sub mnuExit_Click()
    Dim UPDtop, UDPleft
    On Error Resume Next
    If udp2email.WindowState = 0 Then
        'Save settings to registry
        UPDtop = udp2email.Top
        UDPleft = udp2email.Left
        SaveSetting "udp2email", "Startup", "Left", UDPleft
        SaveSetting "udp2email", "Startup", "Top", UPDtop
        SaveSetting "udp2email", "Startup", "AutoSend", udp2email.chkAutoSend.Value
        SaveSetting "udp2email", "Startup", "IP", udp2email.txtIP.Text
        SaveSetting "udp2email", "Startup", "Port", udp2email.txtPort.Text
        SaveSetting "udp2email", "Startup", "From", udp2email.txtFrom.Text
        SaveSetting "udp2email", "Startup", "To", udp2email.txtTo.Text
        SaveSetting "udp2email", "Startup", "Subject", udp2email.txtSubject.Text
    End If
    End
End Sub

Private Sub Timer1_Timer()
txt_countout = 0

End Sub

Private Sub Timer2_Timer()
    Dim i As Integer
    Dim emailData As String
    For i = 1 To (message_count - 1)
        emailData = data(i)
        Call SendFromUDP(emailData)
    Next i
    Timer2.Enabled = False
    message_count = 1
    
End Sub

Private Sub Timer3_Timer()
    Timer4.Enabled = True
    Timer3.Enabled = False
End Sub

Private Sub Timer4_Timer()
    chkAutoSend.Value = vbChecked
    Timer4.Enabled = False
End Sub

Private Sub txtIP_Change()
    On Error Resume Next
    'Close connection and re-enable Bind button
    Winsock1.Close
    cmdBind.Caption = "Bind &Port"
End Sub

Private Sub txtPort_Change()
    On Error Resume Next
    'Close connection and re-enable Bind button
    Winsock1.Close
    cmdBind.Caption = "Bind &Port"
End Sub

Private Sub Winsock1_DataArrival(ByVal bytesTotal As Long)
    On Error GoTo errWinsock
    'Dim strData As String, emailData As String
    Dim DataFromSP As Byte, emailData As String
    'Get incoming UDP data
    'Winsock1.GetData strData
    Winsock1.GetData DataFromSP
    'Call function to format UDP into email
    
'    If DataFromSP = 20 Then
'    emailData = "Balls!"
'    If chkAutoSend.Value = vbChecked Then
'        Call SendFromUDP(emailData)
'    End If
'    End If
'    If DataFromSP = 30 Then
'    emailData = "Nad!"
'    If chkAutoSend.Value = vbChecked Then
'        Call SendFromUDP(emailData)
'    End If
'    End If

    
    
    
    
    If temp_flag = False Then
    
        If DataFromSP = 10 Then
            emailData = "Left Garage Door Open " & Format(Now, "mm-dd-yyyy,hh:nn:ss")
                If chkAutoSend.Value = vbChecked Then
                    'Call SendFromUDP(emailData)
                    data(message_count) = emailData
                    If Timer2.Enabled = False Then Timer2.Enabled = True
                    message_count = message_count + 1
                    
                End If
        End If
        
        If DataFromSP = 11 Then
            emailData = "Left Garage Door Closed " & Format(Now, "mm-dd-yyyy,hh:nn:ss")
                If chkAutoSend.Value = vbChecked Then
                    'Call SendFromUDP(emailData)
                    data(message_count) = emailData
                    If Timer2.Enabled = False Then Timer2.Enabled = True
                    message_count = message_count + 1
                End If
        End If
    
        If DataFromSP = 12 Then
            emailData = "Right Garage Door Open " & Format(Now, "mm-dd-yyyy,hh:nn:ss")
                If chkAutoSend.Value = vbChecked Then
                    'Call SendFromUDP(emailData)
                    data(message_count) = emailData
                    If Timer2.Enabled = False Then Timer2.Enabled = True
                    message_count = message_count + 1
                End If
        End If
        
        If DataFromSP = 13 Then
            emailData = "Right Garage Door Closed " & Format(Now, "mm-dd-yyyy,hh:nn:ss")
                If chkAutoSend.Value = vbChecked Then
                    'Call SendFromUDP(emailData)
                    data(message_count) = emailData
                    If Timer2.Enabled = False Then Timer2.Enabled = True
                    message_count = message_count + 1
                End If
        End If
        
        If DataFromSP = 14 Then
            emailData = "Front Door Open " & Format(Now, "mm-dd-yyyy,hh:nn:ss")
                If chkAutoSend.Value = vbChecked Then
                    'Call SendFromUDP(emailData)
                    data(message_count) = emailData
                    If Timer2.Enabled = False Then Timer2.Enabled = True
                    message_count = message_count + 1
                End If
        End If
        
        If DataFromSP = 15 Then
            emailData = "Front Door Closed " & Format(Now, "mm-dd-yyyy,hh:nn:ss")
                If chkAutoSend.Value = vbChecked Then
                    'Call SendFromUDP(emailData)
                    data(message_count) = emailData
                    If Timer2.Enabled = False Then Timer2.Enabled = True
                    message_count = message_count + 1
                End If
        End If
        
        If DataFromSP = 16 Then
            emailData = "Garage Entry Door Open " & Format(Now, "mm-dd-yyyy,hh:nn:ss")
                If chkAutoSend.Value = vbChecked Then
                    'Call SendFromUDP(emailData)
                    data(message_count) = emailData
                    If Timer2.Enabled = False Then Timer2.Enabled = True
                    message_count = message_count + 1
                End If
        End If
        
        If DataFromSP = 17 Then
            emailData = "Garage Entry Door Closed " & Format(Now, "mm-dd-yyyy,hh:nn:ss")
                If chkAutoSend.Value = vbChecked Then
                    'Call SendFromUDP(emailData)
                    data(message_count) = emailData
                    If Timer2.Enabled = False Then Timer2.Enabled = True
                    message_count = message_count + 1
                End If
        End If
        
        If message_count > 8 Then message_count = 9
        
        
        If DataFromSP = 30 Then
            emailData = "Ed Code Entry"
                'If chkAutoSend.Value = vbChecked Then
                '    Call SendFromUDP(emailData)
                'End If
            Timer2.Enabled = False
            message_count = 1
            chkAutoSend.Value = vbUnchecked
            Timer3.Enabled = True
        End If
        
        If DataFromSP = 31 Then
            emailData = "Page Code Entry"
                If chkAutoSend.Value = vbChecked Then
                    Call SendFromUDP(emailData)
                End If
        End If
        
        If DataFromSP = 32 Then
            emailData = "Landlord Code Entry"
                If chkAutoSend.Value = vbChecked Then
                    Call SendFromUDP(emailData)
                End If
        End If
        
        If DataFromSP = 40 Then
            emailData = "Invalid Code Entry"
                If chkAutoSend.Value = vbChecked Then
                    Call SendFromUDP(emailData)
                End If
        End If
        
        If DataFromSP = 100 Then
            
            Open App.Path & "\security_log.txt" For Append As #1
            Write #1, Now, "Left Garage Door Opened"
            Close #1
                   
        End If
        
        If DataFromSP = 101 Then
            
            Open App.Path & "\security_log.txt" For Append As #1
            Write #1, Now, "Left Garage Door Closed"
            Close #1
                   
        End If
        
        If DataFromSP = 102 Then
            
            Open App.Path & "\security_log.txt" For Append As #1
            Write #1, Now, "Right Garage Door Opened"
            Close #1
                   
        End If
        
        If DataFromSP = 103 Then
            
            Open App.Path & "\security_log.txt" For Append As #1
            Write #1, Now, "Right Garage Door Closed"
            Close #1
                   
        End If
        
        If DataFromSP = 104 Then
            
            Open App.Path & "\security_log.txt" For Append As #1
            Write #1, Now, "Front Door Opened"
            Close #1
                   
        End If
    
        If DataFromSP = 105 Then
            
            Open App.Path & "\security_log.txt" For Append As #1
            Write #1, Now, "Front Door Closed"
            Close #1
                   
        End If
        
        If DataFromSP = 106 Then
            
            Open App.Path & "\security_log.txt" For Append As #1
            Write #1, Now, "Garage Entry Door Opened"
            Close #1
                   
        End If
    
        If DataFromSP = 107 Then
            
            Open App.Path & "\security_log.txt" For Append As #1
            Write #1, Now, "Garage Entry Door Closed"
            Close #1
                   
        End If
        
        If DataFromSP = 130 Then
            
            Open App.Path & "\security_log.txt" For Append As #1
            Write #1, Now, "Ed Code Entry"
            Close #1
                   
        End If
    
        If DataFromSP = 131 Then
            
            Open App.Path & "\security_log.txt" For Append As #1
            Write #1, Now, "Page Code Entry"
            Close #1
                   
        End If
    
        If DataFromSP = 132 Then
            
            Open App.Path & "\security_log.txt" For Append As #1
            Write #1, Now, "Landlord Code Entry"
            Close #1
                   
        End If
    
        If DataFromSP = 140 Then
            
            Open App.Path & "\security_log.txt" For Append As #1
            Write #1, Now, "Invalid Code Entry"
            Close #1
                   
        End If
        
        If DataFromSP = 150 Then
            
            Open App.Path & "\security_log.txt" For Append As #1
            Write #1, Now, "Text Message Enabled"
            Close #1
                   
        End If
    
        If DataFromSP = 151 Then
            
            Open App.Path & "\security_log.txt" For Append As #1
            Write #1, Now, "Text Message Disabled"
            Close #1
                   
        End If
    
        If DataFromSP = 152 Then
            
            Open App.Path & "\security_log.txt" For Append As #1
            Write #1, Now, "Alarm Armed"
            Close #1
                   
        End If
        
        If DataFromSP = 153 Then
            
            Open App.Path & "\security_log.txt" For Append As #1
            Write #1, Now, "Alarm Disarmed"
            Close #1
                   
        End If
    
        If DataFromSP = 154 Then
            
            Open App.Path & "\security_log.txt" For Append As #1
            Write #1, Now, "System Power On"
            Close #1
                   
        End If
        
        If DataFromSP = 155 Then
            
            Open App.Path & "\security_log.txt" For Append As #1
            Write #1, Now, "Heat 0n"
            Close #1
                   
        End If
        
        If DataFromSP = 156 Then
            
            Open App.Path & "\security_log.txt" For Append As #1
            Write #1, Now, "Heat Off"
            Close #1
                   
        End If


    End If
    
    If temp_flag = True And byte_count = 5 Then
        
        Open App.Path & "\temp\" & Format(Now, "mm-dd-yyyy") & "_temp_log.txt" For Append As #1
           If DataFromSP = 2 Then
                Print #1, Format(Now, "mm-dd-yyyy,hh:nn:ss") & "," & in_temp & "." & in_temp1 & "," & "-" & out_temp & "." & out_temp1 & ","
            Else
                Print #1, Format(Now, "mm-dd-yyyy,hh:nn:ss") & "," & in_temp & "." & in_temp1 & "," & out_temp & "." & out_temp1 & ","
            End If
            
        Close #1
        
        temp_flag = False
    End If
    
    
    If temp_flag = True And byte_count = 4 Then
        out_temp1 = DataFromSP
        byte_count = byte_count + 1
    End If
        
    If temp_flag = True And byte_count = 3 Then
        out_temp = DataFromSP
        byte_count = byte_count + 1
    End If
        
    If temp_flag = True And byte_count = 2 Then
        in_temp1 = DataFromSP
        byte_count = byte_count + 1
        'temp_flag = False
    End If
    
    If temp_flag = True And byte_count = 1 Then
        'Open App.Path & "\temp\" & Format(Now, "mm-dd-yyyy") & "_temp_log.txt" For Append As #1
        '    Print #1, Format(Now, "mm-dd-yyyy,hh:nn:ss") & "," & DataFromSP
        'Close #1
        'temp_flag = False
        in_temp = DataFromSP
        byte_count = byte_count + 1
    End If
    
    
    
    
    If DataFromSP = 200 Then
         temp_flag = True
         byte_count = 1
    End If
    







    
    'emailData = FormatEmail(strData)
    'Display in text box
    txtOutput.Text = emailData
    'Call email send program if AutoSend is checked
    'If chkAutoSend.Value = vbChecked Then
    '    Call SendFromUDP(emailData)
   ' End If
    Exit Sub
    
errWinsock:
    MsgBox "Error: " & Err.Number & ", " & Err.Description
End Sub

Private Sub SendFromUDP(emailData As String)
    txt_countout = txt_countout + 1
    
    If txt_countout > 10 Then
        chkAutoSend.Value = vbUnchecked
        Open App.Path & "security_log.txt" For Append As #1
        Write #1, Now, "Text Message Rate Limit Exceeded"
        Close #1
        'emailData = "Text Message Rate Limit Exceeded"
    End If
'
'    If txt_countout = 10 Then
'        emailData = "Text Message Rate Limit Exceeded"
'    End If
    
    
    Dim Recipient As Variant, ToList() As Variant, EntireList As String
    Dim i As Integer, pos As Integer
    'Login with mapi UserName
    On Error GoTo errSendFromUDP
    mapiSess.NewSession = True
    'mapiSess.UserName = txtFrom.Text
    mapiSess.Action = 1
    mapiMess.SessionID = mapiSess.SessionID
    'Prepare list of recipients
    i = 0
    EntireList = Trim(txtTo.Text)
    Do
        If EntireList <> "" Then
            i = i + 1
            ReDim Preserve ToList(i)
            pos = InStr(1, EntireList, ";", 1)
            If pos > 0 Then
                ToList(i) = Trim(Mid(EntireList, 1, pos - 1))
                EntireList = Trim(Mid(EntireList, pos + 1))
            Else
                ToList(i) = EntireList
                Exit Do
            End If
        Else
            Exit Do
        End If
        DoEvents
    Loop
    'Prepare and Send e-mail
    
    For Each Recipient In ToList
        mapiMess.Compose
        mapiMess.RecipDisplayName = Recipient
        mapiMess.MsgSubject = txtSubject.Text
        mapiMess.MsgNoteText = emailData
        mapiMess.Send
    Next Recipient
    
    'Logoff
    mapiSess.Action = 2
    mapiMess.SessionID = 0
    mapiSess.NewSession = False
    Exit Sub
    
errSendFromUDP:
    MsgBox "Error: " & Err.Number & ", " & Err.Description
End Sub


