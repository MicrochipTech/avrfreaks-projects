VERSION 5.00
Object = "{248DD890-BB45-11CF-9ABC-0080C7E7B78D}#1.0#0"; "MSWINSCK.OCX"
Begin VB.Form frm001185 
   Caption         =   " WIZnet 001185 - Terminalator II - The SQL"
   ClientHeight    =   4152
   ClientLeft      =   48
   ClientTop       =   336
   ClientWidth     =   7800
   LinkTopic       =   "Form1"
   ScaleHeight     =   4152
   ScaleWidth      =   7800
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton cmdClose 
      Caption         =   "Close"
      Height          =   372
      Left            =   6600
      TabIndex        =   1
      Top             =   3480
      Width           =   912
   End
   Begin VB.ListBox lstInfo 
      Height          =   3312
      Left            =   300
      TabIndex        =   0
      TabStop         =   0   'False
      Top             =   540
      Width           =   6072
   End
   Begin MSWinsockLib.Winsock tcpServer 
      Left            =   6600
      Top             =   540
      _ExtentX        =   593
      _ExtentY        =   593
      _Version        =   393216
      RemotePort      =   19001
      LocalPort       =   19002
   End
   Begin VB.Label lbl 
      Caption         =   "Activity:"
      Height          =   192
      Left            =   300
      TabIndex        =   2
      Top             =   240
      Width           =   1812
   End
End
Attribute VB_Name = "frm001185"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'   WIZnet Contest Entry: 001185
'   Terminalator II - The SQL
'
'   This TCP server recognizes the following commands:
'      Open - opens the MS Access database specifed below
'      Close - closes the database and shuts down the server
'      Quit - same as close
'      SQL statements:
'        Select statements create a recordset which are returned
'          as ascii lines (ending with CRLF)
'        All other statements are assumed to be SQL action statements
'          and are passed to the Jet engine for processing

Option Explicit

Const DB = "c:\WIZnet Contest\DB_001185.mdb"
Dim dbOpen As Boolean
Dim adb As Database

Private Sub cmdClose_Click()
' shut everything down and quit
  Call CloseDB

End Sub

Private Sub Form_Load()
  Me.Left = (Screen.Width - Me.Width) / 2
  Me.Top = (Screen.Height - Me.Height) / 2
  dbOpen = False
  
  lstInfo.AddItem "Listening"
  tcpServer.Listen
  
End Sub

Private Sub tcpServer_ConnectionRequest(ByVal requestID As Long)

   If tcpServer.State <> sckClosed Then tcpServer.Close
   tcpServer.Accept requestID
   lstInfo.AddItem "Connection accepted"
   Call Ready

End Sub

Private Sub tcpServer_DataArrival(ByVal bCnt As Long)
  Dim s$, a$

  tcpServer.GetData s$
  If Right$(s$, 1) = vbCr Then s$ = Left$(s$, bCnt - 1)
  s$ = Trim$(s$)
  lstInfo.AddItem "recv: " & s$

  a$ = LCase$(s$)
  If a$ = "open" Then
    Call OpenDB
  ElseIf InStr("quit.stop.end.close.done.", a$ & ".") > 0 Then
    Call CloseDB
  Else
    If dbOpen Then
      If Left$(a$, 7) = "select " Then
        Call SelectStmt(s$)
      Else
        Call SQLStmt(s$)
      End If
    Else
      SendMsg ("You must open the database first")
    End If
  End If
  Call Ready
  
End Sub

Sub Ready()
' give them a prompt
  tcpServer.SendData ">> "
  
End Sub
Sub SendMsg(a$)
' send a message to the client and show it here, too

  lstInfo.AddItem "send: " & a$
  tcpServer.SendData a$ & vbCrLf
  
End Sub

Sub SelectStmt(s$)
' process a select statement
  Dim rs As Recordset
  
  On Error GoTo selerr
  Set rs = adb.openrecordset(s$, dbOpenSnapshot)
  Call rsOut(rs)
  Exit Sub
  
selerr:
  Call SendMsg("Error executing statement")

End Sub

Sub SQLStmt(s$)
' process an SQL statement

  On Error GoTo sqlerror
  adb.execute s$
  Exit Sub
  
sqlerror:
  Call SendMsg("Error executing statement")
  
End Sub

Sub OpenDB()
' open the microsoft database
  If dbOpen Then
    Call SendMsg("The database is already open")
  Else
    On Error GoTo hey
    Set adb = OpenDatabase(DB, True)
    dbOpen = True
    Call SendMsg("Database open")
  End If
  Exit Sub
  
hey:
  Call SendMsg("Error opening database")
  
End Sub

Sub CloseDB()
' close the database, the connection, and shut down

  On Error Resume Next
  If dbOpen Then adb.Close
  tcpServer.Close
  End
  
End Sub

Private Sub tcpServer_Error(ByVal Number As Integer, _
  Description As String, ByVal Scode As Long, ByVal Source As String, _
  ByVal HelpFile As String, ByVal HelpContext As Long, _
  CancelDisplay As Boolean)
  
  lstInfo.AddItem "Error: " & Description
  
End Sub

Sub rsOut(rs As Recordset)
' put out a recordset in ASCII
  Dim a$, f As Field
  
  With rs
    Do While Not .EOF
      For Each f In rs.Fields
        a$ = "   [" & f.Name & "] = " & f.Value
        Call SendMsg(a$)
        Next f
      .MoveNext
    Loop
  End With

End Sub
