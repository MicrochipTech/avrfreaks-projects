'
' Smiley Terminal evolved over time and like many a creature of evolution, 
' you might find some atavistic artifacts. As my Grandpappy, the hog 
' farmer, used to say: "That's about as useful as tits an a boar." Well,
' I'm going to guess that you'll find some artifacts in this code that 
' are even less useful.
' 
' Don't you just love the legalistic bullshit that is usually inserted at
' this point? I've rewritten some standard boilerplate stuff to say what
' I think we really mean:
'  This work is entirely by Joe Pardue (except for the major portions he
'  stole and forgot to attribute). Copyright by Joe Pardue 4/26/07. and all
'  rights are reserved - any theft of this intellectual property will 
'  result in a challenge to an arm wrestling contest and depending on the
'  nature of the theft, I may break you wrist, or arm, or just rip the
'  sucker out of the socket. 
'
'  This software is not warrented in any way what-so-ever. It WILL screw
'  something up and will kill someone after doing serious property
'  damage. You've been warned. If you sic a lawyer on me, just remember:
'  I'm old, grouchy, have little to lose, and support the Second Amendment?
'
' Ah, yes I feel much better now.
'
' Please send any bug reports, comments, or concerns to joe@smileymicros.com
'
' 7/6/06 - Version 0.0 moved up to C# 2005 Express
' 3/14/07 - Revision 0.1 got back to it and added a bunch of stuff.
' 4/26/07 - Revision 0.2 God only knows what I did, but it is different.
' 4/28/07 - Revision 0.3 changes marked by comment // JWP 4/28/07 to allow byte reception

' BETA - THIS IS THE BETA RELEASE AND WILL HAVE PROBLEMS THAT I MISSED AND YOU WON'T
' BETA - SINCE MICROSOFT RELEASES CODE FOR THE BUYER TO DEBUG, I THOUGHT 'HELL, WHY NOT'
' BETA - PLEASE SEND COMMENTS AND CORRECTIONS TO joe@smileymicros.com


Imports Microsoft.VisualBasic
Imports System
Imports System.Drawing
Imports System.Collections
Imports System.ComponentModel
Imports System.Windows.Forms
Imports System.Data
Imports System.IO
Imports System.IO.Ports

Imports LED

Namespace DeveloperTerminal
	Public Class MainForm
		Inherits System.Windows.Forms.Form
		#Region "Initialization"
		' If you build Developer Terminal from scratch using the Express edition
		 '* you will see some differences in the intializaion section. This is because
		 '* most of this code was written in earlier versions and imported into Express.
		 '* The code will work the same thought.

		#Region "C# Express added definitions"
		Private groupBoxSendText As System.Windows.Forms.GroupBox
		Private WithEvents richTextBoxSend As System.Windows.Forms.RichTextBox
		Private WithEvents linkLabelVisitWebSite As System.Windows.Forms.LinkLabel
		Private groupBox3 As System.Windows.Forms.GroupBox
		Private groupBox2 As System.Windows.Forms.GroupBox
		Private WithEvents radioButtonImmediate As System.Windows.Forms.RadioButton
		Private WithEvents richTextBoxReceive As System.Windows.Forms.RichTextBox
		Private WithEvents radioButtonASCII As System.Windows.Forms.RadioButton
		Private groupBox1 As System.Windows.Forms.GroupBox
		Private groupBoxReceive As System.Windows.Forms.GroupBox
		Private label7 As System.Windows.Forms.Label
		Private WithEvents radioButtonHEX As System.Windows.Forms.RadioButton
		Private WithEvents listBoxMacros As System.Windows.Forms.ListBox
		Private WithEvents radioButtonQueued As System.Windows.Forms.RadioButton
		Private groupBoxMacros As System.Windows.Forms.GroupBox
		Private label3 As System.Windows.Forms.Label
		Private label2 As System.Windows.Forms.Label
		Private label1 As System.Windows.Forms.Label
		Private label6 As System.Windows.Forms.Label
		Private label4 As System.Windows.Forms.Label
		Private WithEvents comboBoxSendByte As System.Windows.Forms.ComboBox
		Private WithEvents buttonSendQue As System.Windows.Forms.Button
		Private menuStrip1 As MenuStrip
		Private settingsToolStripMenuItem As ToolStripMenuItem
		Private WithEvents portToolStripMenuItem As ToolStripMenuItem
		Private WithEvents sendToolStripMenuItem As ToolStripMenuItem
		Private WithEvents receiveToolStripMenuItem As ToolStripMenuItem
		Private helpToolStripMenuItem As ToolStripMenuItem
		Private contextMenuStripSend As ContextMenuStrip
		Private components As IContainer
		Private WithEvents copyToolStripMenuItem As ToolStripMenuItem
		Private WithEvents cutToolStripMenuItem As ToolStripMenuItem
		Private WithEvents pasteToolStripMenuItem As ToolStripMenuItem
		Private WithEvents selectAllToolStripMenuItem As ToolStripMenuItem
		Private WithEvents undoToolStripMenuItem As ToolStripMenuItem
		Private WithEvents redoToolStripMenuItem As ToolStripMenuItem
		Private WithEvents serialPort1 As SerialPort
		Private WithEvents openPortToolStripMenuItem As ToolStripMenuItem
		Private WithEvents saveAsToolStripMenuItem As ToolStripMenuItem
		Private saveFileDialog1 As SaveFileDialog
		Private WithEvents loadFileToolStripMenuItem As ToolStripMenuItem
		Private openFileDialog1 As OpenFileDialog
		Private toolStripSeparator1 As ToolStripSeparator
		Private toolStripSeparator2 As ToolStripSeparator
		Private WithEvents deleteToolStripMenuItem As ToolStripMenuItem
		Private groupBox5 As GroupBox
		Private WithEvents radioButtonTermCharYes As RadioButton
		Private WithEvents radioButtonTermCharNo As RadioButton
		Private label5 As Label
		Private textBoxSendCount As TextBox
		Private label8 As Label
		Private textBoxReceiveCount As TextBox
		Private label9 As Label
		Private LEDCTS As LED.LED22
		Private label10 As Label
		Private label11 As Label
		Private label12 As Label
		Private LEDDSR As LED22
		Private LEDDCD As LED22
		Private WithEvents timerModemLines As Timer
		Private groupBox4 As GroupBox
		Private WithEvents buttonDTR As Button
		Private WithEvents buttonRTS As Button
		Private label15 As Label
		Private LEDDTR As LED22
		Private LEDRTS As LED22
		Private LEDRI As LED22
		Private WithEvents panel1 As Panel
		#End Region

		#Region "Startup definitions"

		' Create an instance of the settings form
		Private portSettings As PortSettings = New PortSettings()

		' Booleans to toggle the modem lines
		Private RTSToggle As Boolean = True
		Private DTRToggle As Boolean = True

		' Keep count of sent and received bytes
		Private SendCount As Integer = 0
		Private ReceiveCount As Integer = 0

		' Immediate or Queued state for sending data
		Private SendImmediate As Boolean = True
		Private QueuedString As String = ""

		' Send with terminal characters
		Private UseTermChar As Boolean = False

		' Receive as ASCII or Hex state
		Private ReceiveASCII As Boolean = True

		' Create an instance of the TerminalData class
		Private xMLData As XMLData = New XMLData()

		' Create an instance of the TerminalXMLIO class to read the terminal data
		Private dataXMLIO As XMLIO = New XMLIO()

		' Create MacroData arrays
		Private MacroDataText As String() = New String(51){}
		Private MacroDataData As String() = New String(51){}
		Private MacroDataType As String() = New String(51){}

		' Boolean for the port open state
		Public PortOpen As Boolean = False
		Private toolsToolStripMenuItem As ToolStripMenuItem
		Private WithEvents sendCharToolStripMenuItem As ToolStripMenuItem
		Private WithEvents timerSendByte As Timer
		Private fileToolStripMenuItem As ToolStripMenuItem
		Private WithEvents openInSendBoxToolStripMenuItem As ToolStripMenuItem
		Private WithEvents saveSendBoxToolStripMenuItem As ToolStripMenuItem
		Private WithEvents saveReceiveBoxToolStripMenuItem As ToolStripMenuItem
		Private WithEvents manualToolStripMenuItem As ToolStripMenuItem
		Private WithEvents smileyMicrosToolStripMenuItem As ToolStripMenuItem
		Private WithEvents copyToolStripMenuItem1 As ToolStripMenuItem
		Private WithEvents cutToolStripMenuItem1 As ToolStripMenuItem
		Private WithEvents deleteToolStripMenuItem1 As ToolStripMenuItem
		Private WithEvents selectAllToolStripMenuItem1 As ToolStripMenuItem
		Private toolStripSeparator4 As ToolStripSeparator
		Private WithEvents saveFileAsToolStripMenuItem As ToolStripMenuItem
		Private contextMenuStripReceive As ContextMenuStrip

		' Boolean for the Ring Indicator modem state
		Public RingIndicator As Boolean = False

		#End Region

		#Region "Startup functions"

		Public Sub New()
			'
			' Required for Windows Form Designer support
			'
			InitializeComponent()

			' Read the terminal data from TerminalData.xml
			dataXMLIO.appPath = System.Environment.CurrentDirectory
			xMLData = dataXMLIO.XMLRead()

		End Sub

		''' <summary>
		''' Clean up any resources being used.
		''' </summary>
		Protected Overrides Overloads Sub Dispose(ByVal disposing As Boolean)
			MyBase.Dispose(disposing)
		End Sub

		''' <summary>
		''' The main entry point for the application.
		''' </summary>
		<STAThread> _
		Shared Sub Main()
			Application.Run(New MainForm())
		End Sub

		Private Sub Form1_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Load
		   Me.Text = "Smiley Micros COM Port Terminal - Port is closed"
		   ' Start the terminal with default settings
		   portSettings.selectedPort = xMLData.COMPort
		   serialPort1.PortName = portSettings.selectedPort
		   portSettings.selectedBaudrate = xMLData.Baud
		   serialPort1.BaudRate = portSettings.selectedBaudrate
		   portSettings.selectedDataBits = xMLData.Databits
		   serialPort1.DataBits = portSettings.selectedDataBits

		   Dim temp As String = xMLData.Parity
		   Select Case temp
			   Case "Even"
				   portSettings.selectedParity = Parity.Even
				   serialPort1.Parity = portSettings.selectedParity
			   Case "Mark"
				   portSettings.selectedParity = Parity.Mark
				   serialPort1.Parity = portSettings.selectedParity
			   Case "None"
				   portSettings.selectedParity = Parity.None
				   serialPort1.Parity = portSettings.selectedParity
			   Case "Odd"
				   portSettings.selectedParity = Parity.Odd
				   serialPort1.Parity = portSettings.selectedParity
			   Case "Space"
				   portSettings.selectedParity = Parity.Space
				   serialPort1.Parity = portSettings.selectedParity
			   Case Else
				   portSettings.selectedParity = Parity.None
				   serialPort1.Parity = portSettings.selectedParity
		   End Select

		   temp = xMLData.Stopbits
		   Select Case temp
			   Case "None"
				   portSettings.selectedStopBits = StopBits.None
				   serialPort1.StopBits = portSettings.selectedStopBits
			   Case "1"
				   portSettings.selectedStopBits = StopBits.One
				   serialPort1.StopBits = portSettings.selectedStopBits
			   'case "1.5": // not supported by FT232R
			   'serialPort1.StopBits = = StopBits.OnePointFive;
			   'break;
			   Case "2"
				   portSettings.selectedStopBits = StopBits.Two
				   serialPort1.StopBits = portSettings.selectedStopBits
			   Case Else
				   portSettings.selectedStopBits = StopBits.One
				   serialPort1.StopBits = portSettings.selectedStopBits
		   End Select

		   temp = xMLData.Handshake
		   Select Case temp
			   Case "None"
				   portSettings.selectedHandshaking = Handshake.None
				   serialPort1.Handshake = portSettings.selectedHandshaking
			   Case "RTS/CTS"
				   portSettings.selectedHandshaking = Handshake.RequestToSend
				   serialPort1.Handshake = portSettings.selectedHandshaking
			   Case "Xon/Xoff"
				   portSettings.selectedHandshaking = Handshake.XOnXOff
				   serialPort1.Handshake = portSettings.selectedHandshaking
			   Case Else
				   portSettings.selectedHandshaking = Handshake.None
				   serialPort1.Handshake = portSettings.selectedHandshaking
		   End Select

			' Load the comboBox with 0x00 to 0xFF
			For i As Integer = 0 To 15
				Me.comboBoxSendByte.Items.Add("0x0" & i.ToString("X"))
			Next i
			For i As Integer = 16 To 255
				Me.comboBoxSendByte.Items.Add("0x" & i.ToString("X"))
			Next i

			loadXMLData()

			' Set the default LED images
			Me.LEDCTS.image = CInt(Fix(LED.LED22.LEDColor.BlueOn))
			Me.LEDDSR.image = CInt(Fix(LED.LED22.LEDColor.BlueOff))
			Me.LEDDCD.image = CInt(Fix(LED.LED22.LEDColor.BlueOff))
			Me.LEDRI.image = CInt(Fix(LED.LED22.LEDColor.BlueOff))
			Me.LEDRTS.image = CInt(Fix(LED.LED22.LEDColor.RedOn))
			Me.LEDDTR.image = CInt(Fix(LED.LED22.LEDColor.RedOff))

		End Sub
		#End Region

		#Region "Windows Form Designer generated code"
		''' <summary>
		''' Required method for Designer support - do not modify
		''' the contents of this method with the code editor.
		''' </summary>
		Private Sub InitializeComponent()
			Me.components = New System.ComponentModel.Container()
			Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(MainForm))
			Me.buttonSendQue = New System.Windows.Forms.Button()
			Me.comboBoxSendByte = New System.Windows.Forms.ComboBox()
			Me.label4 = New System.Windows.Forms.Label()
			Me.label6 = New System.Windows.Forms.Label()
			Me.label1 = New System.Windows.Forms.Label()
			Me.label2 = New System.Windows.Forms.Label()
			Me.label3 = New System.Windows.Forms.Label()
			Me.groupBoxMacros = New System.Windows.Forms.GroupBox()
			Me.label7 = New System.Windows.Forms.Label()
			Me.listBoxMacros = New System.Windows.Forms.ListBox()
			Me.radioButtonQueued = New System.Windows.Forms.RadioButton()
			Me.radioButtonHEX = New System.Windows.Forms.RadioButton()
			Me.groupBoxReceive = New System.Windows.Forms.GroupBox()
			Me.textBoxReceiveCount = New System.Windows.Forms.TextBox()
			Me.label9 = New System.Windows.Forms.Label()
			Me.richTextBoxReceive = New System.Windows.Forms.RichTextBox()
			Me.groupBox2 = New System.Windows.Forms.GroupBox()
			Me.radioButtonASCII = New System.Windows.Forms.RadioButton()
			Me.groupBox1 = New System.Windows.Forms.GroupBox()
			Me.radioButtonImmediate = New System.Windows.Forms.RadioButton()
			Me.groupBox3 = New System.Windows.Forms.GroupBox()
			Me.linkLabelVisitWebSite = New System.Windows.Forms.LinkLabel()
			Me.richTextBoxSend = New System.Windows.Forms.RichTextBox()
			Me.groupBoxSendText = New System.Windows.Forms.GroupBox()
			Me.textBoxSendCount = New System.Windows.Forms.TextBox()
			Me.groupBox5 = New System.Windows.Forms.GroupBox()
			Me.radioButtonTermCharNo = New System.Windows.Forms.RadioButton()
			Me.radioButtonTermCharYes = New System.Windows.Forms.RadioButton()
			Me.label5 = New System.Windows.Forms.Label()
			Me.label8 = New System.Windows.Forms.Label()
			Me.menuStrip1 = New System.Windows.Forms.MenuStrip()
			Me.fileToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.openInSendBoxToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.saveSendBoxToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.saveReceiveBoxToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.settingsToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.portToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.sendToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.receiveToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.openPortToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.toolsToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.sendCharToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.helpToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.manualToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.smileyMicrosToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.contextMenuStripSend = New System.Windows.Forms.ContextMenuStrip(Me.components)
			Me.undoToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.redoToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.toolStripSeparator1 = New System.Windows.Forms.ToolStripSeparator()
			Me.cutToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.copyToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.pasteToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.deleteToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.selectAllToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.toolStripSeparator2 = New System.Windows.Forms.ToolStripSeparator()
			Me.loadFileToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.saveAsToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.serialPort1 = New System.IO.Ports.SerialPort(Me.components)
			Me.saveFileDialog1 = New System.Windows.Forms.SaveFileDialog()
			Me.openFileDialog1 = New System.Windows.Forms.OpenFileDialog()
			Me.label10 = New System.Windows.Forms.Label()
			Me.label11 = New System.Windows.Forms.Label()
			Me.label12 = New System.Windows.Forms.Label()
			Me.timerModemLines = New System.Windows.Forms.Timer(Me.components)
			Me.groupBox4 = New System.Windows.Forms.GroupBox()
			Me.label15 = New System.Windows.Forms.Label()
			Me.buttonDTR = New System.Windows.Forms.Button()
			Me.buttonRTS = New System.Windows.Forms.Button()
			Me.panel1 = New System.Windows.Forms.Panel()
			Me.timerSendByte = New System.Windows.Forms.Timer(Me.components)
			Me.LEDDTR = New LED.LED22()
			Me.LEDRTS = New LED.LED22()
			Me.LEDRI = New LED.LED22()
			Me.LEDCTS = New LED.LED22()
			Me.LEDDCD = New LED.LED22()
			Me.LEDDSR = New LED.LED22()
			Me.copyToolStripMenuItem1 = New System.Windows.Forms.ToolStripMenuItem()
			Me.cutToolStripMenuItem1 = New System.Windows.Forms.ToolStripMenuItem()
			Me.deleteToolStripMenuItem1 = New System.Windows.Forms.ToolStripMenuItem()
			Me.selectAllToolStripMenuItem1 = New System.Windows.Forms.ToolStripMenuItem()
			Me.toolStripSeparator4 = New System.Windows.Forms.ToolStripSeparator()
			Me.saveFileAsToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
			Me.contextMenuStripReceive = New System.Windows.Forms.ContextMenuStrip(Me.components)
			Me.groupBoxMacros.SuspendLayout()
			Me.groupBoxReceive.SuspendLayout()
			Me.groupBox2.SuspendLayout()
			Me.groupBox1.SuspendLayout()
			Me.groupBox3.SuspendLayout()
			Me.groupBoxSendText.SuspendLayout()
			Me.groupBox5.SuspendLayout()
			Me.menuStrip1.SuspendLayout()
			Me.contextMenuStripSend.SuspendLayout()
			Me.groupBox4.SuspendLayout()
			Me.contextMenuStripReceive.SuspendLayout()
			Me.SuspendLayout()
			' 
			' buttonSendQue
			' 
			Me.buttonSendQue.BackColor = System.Drawing.Color.Transparent
			Me.buttonSendQue.Enabled = False
			Me.buttonSendQue.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.buttonSendQue.Location = New System.Drawing.Point(130, 13)
			Me.buttonSendQue.Name = "buttonSendQue"
			Me.buttonSendQue.Size = New System.Drawing.Size(76, 20)
			Me.buttonSendQue.TabIndex = 84
			Me.buttonSendQue.Text = "Send Queue"
			Me.buttonSendQue.UseVisualStyleBackColor = False
'			Me.buttonSendQue.Click += New System.EventHandler(Me.buttonSend_Click);
			' 
			' comboBoxSendByte
			' 
			Me.comboBoxSendByte.BackColor = System.Drawing.SystemColors.Window
			Me.comboBoxSendByte.Font = New System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.comboBoxSendByte.Location = New System.Drawing.Point(63, 11)
			Me.comboBoxSendByte.Name = "comboBoxSendByte"
			Me.comboBoxSendByte.Size = New System.Drawing.Size(60, 23)
			Me.comboBoxSendByte.TabIndex = 97
			Me.comboBoxSendByte.Text = "0x00"
'			Me.comboBoxSendByte.SelectedIndexChanged += New System.EventHandler(Me.comboBoxSendByte_SelectedIndexChanged);
			' 
			' label4
			' 
			Me.label4.Font = New System.Drawing.Font("Arial", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.label4.Location = New System.Drawing.Point(4, 10)
			Me.label4.Name = "label4"
			Me.label4.Size = New System.Drawing.Size(100, 16)
			Me.label4.TabIndex = 100
			Me.label4.Text = "Send Text"
			' 
			' label6
			' 
			Me.label6.Font = New System.Drawing.Font("Arial", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.label6.Location = New System.Drawing.Point(4, 10)
			Me.label6.Name = "label6"
			Me.label6.Size = New System.Drawing.Size(100, 16)
			Me.label6.TabIndex = 101
			Me.label6.Text = "Receive Text"
			' 
			' label1
			' 
			Me.label1.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.label1.Location = New System.Drawing.Point(7, 15)
			Me.label1.Name = "label1"
			Me.label1.Size = New System.Drawing.Size(64, 12)
			Me.label1.TabIndex = 0
			Me.label1.Text = "Receive as: "
			' 
			' label2
			' 
			Me.label2.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.label2.Location = New System.Drawing.Point(4, 9)
			Me.label2.Name = "label2"
			Me.label2.Size = New System.Drawing.Size(72, 32)
			Me.label2.TabIndex = 98
			Me.label2.Text = "Send HEX Immediate:"
			' 
			' label3
			' 
			Me.label3.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.label3.Location = New System.Drawing.Point(1, 17)
			Me.label3.Name = "label3"
			Me.label3.Size = New System.Drawing.Size(64, 20)
			Me.label3.TabIndex = 99
			Me.label3.Text = "Send Text:"
			' 
			' groupBoxMacros
			' 
			Me.groupBoxMacros.BackColor = System.Drawing.Color.Transparent
			Me.groupBoxMacros.Controls.Add(Me.label7)
			Me.groupBoxMacros.Controls.Add(Me.listBoxMacros)
			Me.groupBoxMacros.Location = New System.Drawing.Point(7, 339)
			Me.groupBoxMacros.Name = "groupBoxMacros"
			Me.groupBoxMacros.Size = New System.Drawing.Size(560, 118)
			Me.groupBoxMacros.TabIndex = 96
			Me.groupBoxMacros.TabStop = False
			' 
			' label7
			' 
			Me.label7.Font = New System.Drawing.Font("Arial", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.label7.Location = New System.Drawing.Point(6, 10)
			Me.label7.Name = "label7"
			Me.label7.Size = New System.Drawing.Size(100, 16)
			Me.label7.TabIndex = 102
			Me.label7.Text = "Send Macro"
			' 
			' listBoxMacros
			' 
			Me.listBoxMacros.Location = New System.Drawing.Point(5, 29)
			Me.listBoxMacros.Name = "listBoxMacros"
			Me.listBoxMacros.Size = New System.Drawing.Size(550, 82)
			Me.listBoxMacros.TabIndex = 99
'			Me.listBoxMacros.DoubleClick += New System.EventHandler(Me.ListBoxMacrosDoubleClick);
			' 
			' radioButtonQueued
			' 
			Me.radioButtonQueued.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.radioButtonQueued.Location = New System.Drawing.Point(60, 22)
			Me.radioButtonQueued.Name = "radioButtonQueued"
			Me.radioButtonQueued.Size = New System.Drawing.Size(64, 16)
			Me.radioButtonQueued.TabIndex = 96
			Me.radioButtonQueued.Text = "Queued"
'			Me.radioButtonQueued.CheckedChanged += New System.EventHandler(Me.radioButtonQueued_CheckedChanged);
			' 
			' radioButtonHEX
			' 
			Me.radioButtonHEX.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.radioButtonHEX.Location = New System.Drawing.Point(131, 15)
			Me.radioButtonHEX.Name = "radioButtonHEX"
			Me.radioButtonHEX.Size = New System.Drawing.Size(48, 16)
			Me.radioButtonHEX.TabIndex = 2
			Me.radioButtonHEX.Text = "HEX"
'			Me.radioButtonHEX.CheckedChanged += New System.EventHandler(Me.radioButtonHEX_CheckedChanged_1);
			' 
			' groupBoxReceive
			' 
			Me.groupBoxReceive.BackColor = System.Drawing.Color.Transparent
			Me.groupBoxReceive.Controls.Add(Me.textBoxReceiveCount)
			Me.groupBoxReceive.Controls.Add(Me.label9)
			Me.groupBoxReceive.Controls.Add(Me.label6)
			Me.groupBoxReceive.Controls.Add(Me.richTextBoxReceive)
			Me.groupBoxReceive.Controls.Add(Me.groupBox2)
			Me.groupBoxReceive.Location = New System.Drawing.Point(7, 180)
			Me.groupBoxReceive.Name = "groupBoxReceive"
			Me.groupBoxReceive.Size = New System.Drawing.Size(560, 163)
			Me.groupBoxReceive.TabIndex = 87
			Me.groupBoxReceive.TabStop = False
			' 
			' textBoxReceiveCount
			' 
			Me.textBoxReceiveCount.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.textBoxReceiveCount.Location = New System.Drawing.Point(454, 138)
			Me.textBoxReceiveCount.Name = "textBoxReceiveCount"
			Me.textBoxReceiveCount.Size = New System.Drawing.Size(100, 20)
			Me.textBoxReceiveCount.TabIndex = 104
			' 
			' label9
			' 
			Me.label9.AutoSize = True
			Me.label9.Font = New System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.label9.Location = New System.Drawing.Point(454, 127)
			Me.label9.Name = "label9"
			Me.label9.Size = New System.Drawing.Size(67, 12)
			Me.label9.TabIndex = 105
			Me.label9.Text = "Receive Count"
			' 
			' richTextBoxReceive
			' 
			Me.richTextBoxReceive.EnableAutoDragDrop = True
			Me.richTextBoxReceive.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.richTextBoxReceive.Location = New System.Drawing.Point(4, 26)
			Me.richTextBoxReceive.Name = "richTextBoxReceive"
			Me.richTextBoxReceive.Size = New System.Drawing.Size(550, 100)
			Me.richTextBoxReceive.TabIndex = 5
			Me.richTextBoxReceive.Text = ""
'			Me.richTextBoxReceive.TextChanged += New System.EventHandler(Me.richTextBoxReceive_TextChanged);
'			Me.richTextBoxReceive.MouseDown += New System.Windows.Forms.MouseEventHandler(Me.richTextBoxReceive_MouseDown);
			' 
			' groupBox2
			' 
			Me.groupBox2.BackColor = System.Drawing.Color.Transparent
			Me.groupBox2.Controls.Add(Me.radioButtonHEX)
			Me.groupBox2.Controls.Add(Me.radioButtonASCII)
			Me.groupBox2.Controls.Add(Me.label1)
			Me.groupBox2.Location = New System.Drawing.Point(4, 121)
			Me.groupBox2.Name = "groupBox2"
			Me.groupBox2.Size = New System.Drawing.Size(188, 37)
			Me.groupBox2.TabIndex = 98
			Me.groupBox2.TabStop = False
			' 
			' radioButtonASCII
			' 
			Me.radioButtonASCII.Checked = True
			Me.radioButtonASCII.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.radioButtonASCII.Location = New System.Drawing.Point(75, 15)
			Me.radioButtonASCII.Name = "radioButtonASCII"
			Me.radioButtonASCII.Size = New System.Drawing.Size(56, 16)
			Me.radioButtonASCII.TabIndex = 1
			Me.radioButtonASCII.TabStop = True
			Me.radioButtonASCII.Text = "ASCII"
'			Me.radioButtonASCII.CheckedChanged += New System.EventHandler(Me.radioButtonASCII_CheckedChanged_1);
			' 
			' groupBox1
			' 
			Me.groupBox1.Controls.Add(Me.buttonSendQue)
			Me.groupBox1.Controls.Add(Me.radioButtonImmediate)
			Me.groupBox1.Controls.Add(Me.radioButtonQueued)
			Me.groupBox1.Controls.Add(Me.label3)
			Me.groupBox1.Location = New System.Drawing.Point(135, 122)
			Me.groupBox1.Name = "groupBox1"
			Me.groupBox1.Size = New System.Drawing.Size(210, 38)
			Me.groupBox1.TabIndex = 96
			Me.groupBox1.TabStop = False
			' 
			' radioButtonImmediate
			' 
			Me.radioButtonImmediate.Checked = True
			Me.radioButtonImmediate.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.radioButtonImmediate.Location = New System.Drawing.Point(60, 7)
			Me.radioButtonImmediate.Name = "radioButtonImmediate"
			Me.radioButtonImmediate.Size = New System.Drawing.Size(76, 16)
			Me.radioButtonImmediate.TabIndex = 95
			Me.radioButtonImmediate.TabStop = True
			Me.radioButtonImmediate.Text = "Immediate"
'			Me.radioButtonImmediate.CheckedChanged += New System.EventHandler(Me.radioButtonImmediate_CheckedChanged);
			' 
			' groupBox3
			' 
			Me.groupBox3.Controls.Add(Me.comboBoxSendByte)
			Me.groupBox3.Controls.Add(Me.label2)
			Me.groupBox3.Location = New System.Drawing.Point(4, 122)
			Me.groupBox3.Name = "groupBox3"
			Me.groupBox3.Size = New System.Drawing.Size(130, 38)
			Me.groupBox3.TabIndex = 99
			Me.groupBox3.TabStop = False
			' 
			' linkLabelVisitWebSite
			' 
			Me.linkLabelVisitWebSite.BackColor = System.Drawing.Color.PeachPuff
			Me.linkLabelVisitWebSite.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.linkLabelVisitWebSite.Location = New System.Drawing.Point(446, 4)
			Me.linkLabelVisitWebSite.Name = "linkLabelVisitWebSite"
			Me.linkLabelVisitWebSite.Size = New System.Drawing.Size(136, 20)
			Me.linkLabelVisitWebSite.TabIndex = 3
			Me.linkLabelVisitWebSite.TabStop = True
			Me.linkLabelVisitWebSite.Text = "www.SmileyMicros.com"
'			Me.linkLabelVisitWebSite.LinkClicked += New System.Windows.Forms.LinkLabelLinkClickedEventHandler(Me.linkLabelVisitWebSite_LinkClicked);
			' 
			' richTextBoxSend
			' 
			Me.richTextBoxSend.EnableAutoDragDrop = True
			Me.richTextBoxSend.Location = New System.Drawing.Point(5, 26)
			Me.richTextBoxSend.Name = "richTextBoxSend"
			Me.richTextBoxSend.Size = New System.Drawing.Size(550, 100)
			Me.richTextBoxSend.TabIndex = 0
			Me.richTextBoxSend.Text = ""
'			Me.richTextBoxSend.KeyPress += New System.Windows.Forms.KeyPressEventHandler(Me.richTextBoxSend_KeyPress);
'			Me.richTextBoxSend.MouseDown += New System.Windows.Forms.MouseEventHandler(Me.richTextBoxSend_MouseDown);
			' 
			' groupBoxSendText
			' 
			Me.groupBoxSendText.BackColor = System.Drawing.Color.Transparent
			Me.groupBoxSendText.Controls.Add(Me.textBoxSendCount)
			Me.groupBoxSendText.Controls.Add(Me.richTextBoxSend)
			Me.groupBoxSendText.Controls.Add(Me.label4)
			Me.groupBoxSendText.Controls.Add(Me.groupBox5)
			Me.groupBoxSendText.Controls.Add(Me.label8)
			Me.groupBoxSendText.Controls.Add(Me.groupBox3)
			Me.groupBoxSendText.Controls.Add(Me.groupBox1)
			Me.groupBoxSendText.Location = New System.Drawing.Point(7, 21)
			Me.groupBoxSendText.Name = "groupBoxSendText"
			Me.groupBoxSendText.Size = New System.Drawing.Size(560, 163)
			Me.groupBoxSendText.TabIndex = 86
			Me.groupBoxSendText.TabStop = False
			' 
			' textBoxSendCount
			' 
			Me.textBoxSendCount.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.textBoxSendCount.Location = New System.Drawing.Point(455, 139)
			Me.textBoxSendCount.Name = "textBoxSendCount"
			Me.textBoxSendCount.Size = New System.Drawing.Size(100, 20)
			Me.textBoxSendCount.TabIndex = 102
			' 
			' groupBox5
			' 
			Me.groupBox5.Controls.Add(Me.radioButtonTermCharNo)
			Me.groupBox5.Controls.Add(Me.radioButtonTermCharYes)
			Me.groupBox5.Controls.Add(Me.label5)
			Me.groupBox5.Location = New System.Drawing.Point(347, 121)
			Me.groupBox5.Name = "groupBox5"
			Me.groupBox5.Size = New System.Drawing.Size(103, 38)
			Me.groupBox5.TabIndex = 101
			Me.groupBox5.TabStop = False
			' 
			' radioButtonTermCharNo
			' 
			Me.radioButtonTermCharNo.AutoSize = True
			Me.radioButtonTermCharNo.Checked = True
			Me.radioButtonTermCharNo.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.radioButtonTermCharNo.Location = New System.Drawing.Point(61, 22)
			Me.radioButtonTermCharNo.Name = "radioButtonTermCharNo"
			Me.radioButtonTermCharNo.Size = New System.Drawing.Size(39, 17)
			Me.radioButtonTermCharNo.TabIndex = 1
			Me.radioButtonTermCharNo.TabStop = True
			Me.radioButtonTermCharNo.Text = "No"
			Me.radioButtonTermCharNo.UseVisualStyleBackColor = True
'			Me.radioButtonTermCharNo.CheckedChanged += New System.EventHandler(Me.radioButtonTermCharNo_CheckedChanged);
			' 
			' radioButtonTermCharYes
			' 
			Me.radioButtonTermCharYes.AutoSize = True
			Me.radioButtonTermCharYes.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.radioButtonTermCharYes.Location = New System.Drawing.Point(61, 6)
			Me.radioButtonTermCharYes.Name = "radioButtonTermCharYes"
			Me.radioButtonTermCharYes.Size = New System.Drawing.Size(43, 17)
			Me.radioButtonTermCharYes.TabIndex = 0
			Me.radioButtonTermCharYes.Text = "Yes"
			Me.radioButtonTermCharYes.UseVisualStyleBackColor = True
'			Me.radioButtonTermCharYes.CheckedChanged += New System.EventHandler(Me.radioButtonTermCharYes_CheckedChanged);
			' 
			' label5
			' 
			Me.label5.AutoSize = True
			Me.label5.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.label5.Location = New System.Drawing.Point(-1, 16)
			Me.label5.Name = "label5"
			Me.label5.Size = New System.Drawing.Size(63, 13)
			Me.label5.TabIndex = 2
			Me.label5.Text = "Terminator?"
			' 
			' label8
			' 
			Me.label8.AutoSize = True
			Me.label8.Font = New System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.label8.Location = New System.Drawing.Point(455, 128)
			Me.label8.Name = "label8"
			Me.label8.Size = New System.Drawing.Size(53, 12)
			Me.label8.TabIndex = 103
			Me.label8.Text = "Send Count"
			' 
			' menuStrip1
			' 
			Me.menuStrip1.BackColor = System.Drawing.Color.PeachPuff
			Me.menuStrip1.Items.AddRange(New System.Windows.Forms.ToolStripItem() { Me.fileToolStripMenuItem, Me.settingsToolStripMenuItem, Me.openPortToolStripMenuItem, Me.toolsToolStripMenuItem, Me.helpToolStripMenuItem})
			Me.menuStrip1.Location = New System.Drawing.Point(0, 0)
			Me.menuStrip1.Name = "menuStrip1"
			Me.menuStrip1.Size = New System.Drawing.Size(575, 24)
			Me.menuStrip1.TabIndex = 100
			Me.menuStrip1.Text = "menuStrip1"
			' 
			' fileToolStripMenuItem
			' 
			Me.fileToolStripMenuItem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() { Me.openInSendBoxToolStripMenuItem, Me.saveSendBoxToolStripMenuItem, Me.saveReceiveBoxToolStripMenuItem})
			Me.fileToolStripMenuItem.Name = "fileToolStripMenuItem"
			Me.fileToolStripMenuItem.Size = New System.Drawing.Size(35, 20)
			Me.fileToolStripMenuItem.Text = "File"
			' 
			' openInSendBoxToolStripMenuItem
			' 
			Me.openInSendBoxToolStripMenuItem.Name = "openInSendBoxToolStripMenuItem"
			Me.openInSendBoxToolStripMenuItem.Size = New System.Drawing.Size(160, 22)
			Me.openInSendBoxToolStripMenuItem.Text = "Open in Send Box"
'			Me.openInSendBoxToolStripMenuItem.Click += New System.EventHandler(Me.openInSendBoxToolStripMenuItem_Click_1);
			' 
			' saveSendBoxToolStripMenuItem
			' 
			Me.saveSendBoxToolStripMenuItem.Name = "saveSendBoxToolStripMenuItem"
			Me.saveSendBoxToolStripMenuItem.Size = New System.Drawing.Size(160, 22)
			Me.saveSendBoxToolStripMenuItem.Text = "Save Send Box"
'			Me.saveSendBoxToolStripMenuItem.Click += New System.EventHandler(Me.saveSendBoxToolStripMenuItem_Click_1);
			' 
			' saveReceiveBoxToolStripMenuItem
			' 
			Me.saveReceiveBoxToolStripMenuItem.Name = "saveReceiveBoxToolStripMenuItem"
			Me.saveReceiveBoxToolStripMenuItem.Size = New System.Drawing.Size(160, 22)
			Me.saveReceiveBoxToolStripMenuItem.Text = "Save Receive Box"
'			Me.saveReceiveBoxToolStripMenuItem.Click += New System.EventHandler(Me.saveReceiveBoxToolStripMenuItem_Click_1);
			' 
			' settingsToolStripMenuItem
			' 
			Me.settingsToolStripMenuItem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() { Me.portToolStripMenuItem, Me.sendToolStripMenuItem, Me.receiveToolStripMenuItem})
			Me.settingsToolStripMenuItem.Name = "settingsToolStripMenuItem"
			Me.settingsToolStripMenuItem.Size = New System.Drawing.Size(58, 20)
			Me.settingsToolStripMenuItem.Text = "Settings"
			' 
			' portToolStripMenuItem
			' 
			Me.portToolStripMenuItem.Name = "portToolStripMenuItem"
			Me.portToolStripMenuItem.Size = New System.Drawing.Size(205, 22)
			Me.portToolStripMenuItem.Text = "Port"
'			Me.portToolStripMenuItem.Click += New System.EventHandler(Me.portToolStripMenuItem_Click);
			' 
			' sendToolStripMenuItem
			' 
			Me.sendToolStripMenuItem.Name = "sendToolStripMenuItem"
			Me.sendToolStripMenuItem.Size = New System.Drawing.Size(205, 22)
			Me.sendToolStripMenuItem.Text = "Send textBox Properties"
'			Me.sendToolStripMenuItem.Click += New System.EventHandler(Me.sendToolStripMenuItem_Click);
			' 
			' receiveToolStripMenuItem
			' 
			Me.receiveToolStripMenuItem.Name = "receiveToolStripMenuItem"
			Me.receiveToolStripMenuItem.Size = New System.Drawing.Size(205, 22)
			Me.receiveToolStripMenuItem.Text = "Receive textBox Properties"
'			Me.receiveToolStripMenuItem.Click += New System.EventHandler(Me.receiveToolStripMenuItem_Click);
			' 
			' openPortToolStripMenuItem
			' 
			Me.openPortToolStripMenuItem.Name = "openPortToolStripMenuItem"
			Me.openPortToolStripMenuItem.Size = New System.Drawing.Size(68, 20)
			Me.openPortToolStripMenuItem.Text = "Open Port"
'			Me.openPortToolStripMenuItem.Click += New System.EventHandler(Me.openPortToolStripMenuItem_Click);
			' 
			' toolsToolStripMenuItem
			' 
			Me.toolsToolStripMenuItem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() { Me.sendCharToolStripMenuItem})
			Me.toolsToolStripMenuItem.Name = "toolsToolStripMenuItem"
			Me.toolsToolStripMenuItem.Size = New System.Drawing.Size(44, 20)
			Me.toolsToolStripMenuItem.Text = "Tools"
			' 
			' sendCharToolStripMenuItem
			' 
			Me.sendCharToolStripMenuItem.Name = "sendCharToolStripMenuItem"
			Me.sendCharToolStripMenuItem.Size = New System.Drawing.Size(179, 22)
			Me.sendCharToolStripMenuItem.Text = "Send Byte Periodically"
'			Me.sendCharToolStripMenuItem.Click += New System.EventHandler(Me.sendCharToolStripMenuItem_Click);
			' 
			' helpToolStripMenuItem
			' 
			Me.helpToolStripMenuItem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() { Me.manualToolStripMenuItem, Me.smileyMicrosToolStripMenuItem})
			Me.helpToolStripMenuItem.Name = "helpToolStripMenuItem"
			Me.helpToolStripMenuItem.Size = New System.Drawing.Size(40, 20)
			Me.helpToolStripMenuItem.Text = "Help"
			' 
			' manualToolStripMenuItem
			' 
			Me.manualToolStripMenuItem.Name = "manualToolStripMenuItem"
			Me.manualToolStripMenuItem.Size = New System.Drawing.Size(137, 22)
			Me.manualToolStripMenuItem.Text = "Manual"
'			Me.manualToolStripMenuItem.Click += New System.EventHandler(Me.manualToolStripMenuItem_Click);
			' 
			' smileyMicrosToolStripMenuItem
			' 
			Me.smileyMicrosToolStripMenuItem.Name = "smileyMicrosToolStripMenuItem"
			Me.smileyMicrosToolStripMenuItem.Size = New System.Drawing.Size(137, 22)
			Me.smileyMicrosToolStripMenuItem.Text = "Smiley Micros"
'			Me.smileyMicrosToolStripMenuItem.Click += New System.EventHandler(Me.smileyMicrosToolStripMenuItem_Click);
			' 
			' contextMenuStripSend
			' 
			Me.contextMenuStripSend.Items.AddRange(New System.Windows.Forms.ToolStripItem() { Me.undoToolStripMenuItem, Me.redoToolStripMenuItem, Me.toolStripSeparator1, Me.cutToolStripMenuItem, Me.copyToolStripMenuItem, Me.pasteToolStripMenuItem, Me.deleteToolStripMenuItem, Me.selectAllToolStripMenuItem, Me.toolStripSeparator2, Me.loadFileToolStripMenuItem, Me.saveAsToolStripMenuItem})
			Me.contextMenuStripSend.Name = "contextMenuStripSend"
			Me.contextMenuStripSend.Size = New System.Drawing.Size(133, 214)
			' 
			' undoToolStripMenuItem
			' 
			Me.undoToolStripMenuItem.Name = "undoToolStripMenuItem"
			Me.undoToolStripMenuItem.Size = New System.Drawing.Size(132, 22)
			Me.undoToolStripMenuItem.Text = "Undo"
'			Me.undoToolStripMenuItem.Click += New System.EventHandler(Me.undoToolStripMenuItem_Click);
			' 
			' redoToolStripMenuItem
			' 
			Me.redoToolStripMenuItem.Name = "redoToolStripMenuItem"
			Me.redoToolStripMenuItem.Size = New System.Drawing.Size(132, 22)
			Me.redoToolStripMenuItem.Text = "Redo"
'			Me.redoToolStripMenuItem.Click += New System.EventHandler(Me.redoToolStripMenuItem_Click);
			' 
			' toolStripSeparator1
			' 
			Me.toolStripSeparator1.Name = "toolStripSeparator1"
			Me.toolStripSeparator1.Size = New System.Drawing.Size(129, 6)
			' 
			' cutToolStripMenuItem
			' 
			Me.cutToolStripMenuItem.Name = "cutToolStripMenuItem"
			Me.cutToolStripMenuItem.Size = New System.Drawing.Size(132, 22)
			Me.cutToolStripMenuItem.Text = "Cut"
'			Me.cutToolStripMenuItem.Click += New System.EventHandler(Me.cutToolStripMenuItem_Click);
			' 
			' copyToolStripMenuItem
			' 
			Me.copyToolStripMenuItem.Name = "copyToolStripMenuItem"
			Me.copyToolStripMenuItem.Size = New System.Drawing.Size(132, 22)
			Me.copyToolStripMenuItem.Text = "Copy"
'			Me.copyToolStripMenuItem.Click += New System.EventHandler(Me.copyToolStripMenuItem_Click);
			' 
			' pasteToolStripMenuItem
			' 
			Me.pasteToolStripMenuItem.Name = "pasteToolStripMenuItem"
			Me.pasteToolStripMenuItem.Size = New System.Drawing.Size(132, 22)
			Me.pasteToolStripMenuItem.Text = "Paste"
'			Me.pasteToolStripMenuItem.Click += New System.EventHandler(Me.pasteToolStripMenuItem_Click);
			' 
			' deleteToolStripMenuItem
			' 
			Me.deleteToolStripMenuItem.Name = "deleteToolStripMenuItem"
			Me.deleteToolStripMenuItem.Size = New System.Drawing.Size(132, 22)
			Me.deleteToolStripMenuItem.Text = "Clear"
'			Me.deleteToolStripMenuItem.Click += New System.EventHandler(Me.deleteToolStripMenuItem_Click);
			' 
			' selectAllToolStripMenuItem
			' 
			Me.selectAllToolStripMenuItem.Name = "selectAllToolStripMenuItem"
			Me.selectAllToolStripMenuItem.Size = New System.Drawing.Size(132, 22)
			Me.selectAllToolStripMenuItem.Text = "Select All"
'			Me.selectAllToolStripMenuItem.Click += New System.EventHandler(Me.selectAllToolStripMenuItem_Click);
			' 
			' toolStripSeparator2
			' 
			Me.toolStripSeparator2.Name = "toolStripSeparator2"
			Me.toolStripSeparator2.Size = New System.Drawing.Size(129, 6)
			' 
			' loadFileToolStripMenuItem
			' 
			Me.loadFileToolStripMenuItem.Name = "loadFileToolStripMenuItem"
			Me.loadFileToolStripMenuItem.Size = New System.Drawing.Size(132, 22)
			Me.loadFileToolStripMenuItem.Text = "Load File"
'			Me.loadFileToolStripMenuItem.Click += New System.EventHandler(Me.loadFileToolStripMenuItem_Click);
			' 
			' saveAsToolStripMenuItem
			' 
			Me.saveAsToolStripMenuItem.Name = "saveAsToolStripMenuItem"
			Me.saveAsToolStripMenuItem.Size = New System.Drawing.Size(132, 22)
			Me.saveAsToolStripMenuItem.Text = "Save File As"
'			Me.saveAsToolStripMenuItem.Click += New System.EventHandler(Me.saveAsToolStripMenuItem_Click);
			' 
			' serialPort1
			' 
			Me.serialPort1.RtsEnable = True
'			Me.serialPort1.DataReceived += New System.IO.Ports.SerialDataReceivedEventHandler(Me.serialPort1_DataReceived);
'			Me.serialPort1.PinChanged += New System.IO.Ports.SerialPinChangedEventHandler(Me.serialPort1_PinChanged);
			' 
			' openFileDialog1
			' 
			Me.openFileDialog1.FileName = "openFileDialog1"
			' 
			' label10
			' 
			Me.label10.AutoSize = True
			Me.label10.Location = New System.Drawing.Point(94, 13)
			Me.label10.Name = "label10"
			Me.label10.Size = New System.Drawing.Size(28, 13)
			Me.label10.TabIndex = 103
			Me.label10.Text = "CTS"
			' 
			' label11
			' 
			Me.label11.AutoSize = True
			Me.label11.Location = New System.Drawing.Point(124, 13)
			Me.label11.Name = "label11"
			Me.label11.Size = New System.Drawing.Size(30, 13)
			Me.label11.TabIndex = 104
			Me.label11.Text = "DSR"
			' 
			' label12
			' 
			Me.label12.AutoSize = True
			Me.label12.Location = New System.Drawing.Point(155, 13)
			Me.label12.Name = "label12"
			Me.label12.Size = New System.Drawing.Size(30, 13)
			Me.label12.TabIndex = 105
			Me.label12.Text = "DCD"
			' 
			' timerModemLines
			' 
			Me.timerModemLines.Enabled = True
			Me.timerModemLines.Interval = 500
'			Me.timerModemLines.Tick += New System.EventHandler(Me.timerOneSec_Tick);
			' 
			' groupBox4
			' 
			Me.groupBox4.BackColor = System.Drawing.Color.Bisque
			Me.groupBox4.Controls.Add(Me.LEDDTR)
			Me.groupBox4.Controls.Add(Me.LEDRTS)
			Me.groupBox4.Controls.Add(Me.LEDRI)
			Me.groupBox4.Controls.Add(Me.label15)
			Me.groupBox4.Controls.Add(Me.buttonDTR)
			Me.groupBox4.Controls.Add(Me.buttonRTS)
			Me.groupBox4.Controls.Add(Me.LEDCTS)
			Me.groupBox4.Controls.Add(Me.label10)
			Me.groupBox4.Controls.Add(Me.label11)
			Me.groupBox4.Controls.Add(Me.label12)
			Me.groupBox4.Controls.Add(Me.LEDDCD)
			Me.groupBox4.Controls.Add(Me.LEDDSR)
			Me.groupBox4.Location = New System.Drawing.Point(7, 453)
			Me.groupBox4.Name = "groupBox4"
			Me.groupBox4.Size = New System.Drawing.Size(217, 51)
			Me.groupBox4.TabIndex = 110
			Me.groupBox4.TabStop = False
			' 
			' label15
			' 
			Me.label15.AutoSize = True
			Me.label15.Location = New System.Drawing.Point(190, 14)
			Me.label15.Name = "label15"
			Me.label15.Size = New System.Drawing.Size(18, 13)
			Me.label15.TabIndex = 112
			Me.label15.Text = "RI"
			' 
			' buttonDTR
			' 
			Me.buttonDTR.Location = New System.Drawing.Point(51, 27)
			Me.buttonDTR.Name = "buttonDTR"
			Me.buttonDTR.Size = New System.Drawing.Size(38, 21)
			Me.buttonDTR.TabIndex = 111
			Me.buttonDTR.Text = "DTR"
			Me.buttonDTR.UseVisualStyleBackColor = True
'			Me.buttonDTR.Click += New System.EventHandler(Me.buttonDTR_Click);
			' 
			' buttonRTS
			' 
			Me.buttonRTS.Location = New System.Drawing.Point(9, 27)
			Me.buttonRTS.Name = "buttonRTS"
			Me.buttonRTS.Size = New System.Drawing.Size(38, 21)
			Me.buttonRTS.TabIndex = 110
			Me.buttonRTS.Text = "RTS"
			Me.buttonRTS.UseVisualStyleBackColor = True
'			Me.buttonRTS.Click += New System.EventHandler(Me.buttonRTS_Click);
			' 
			' panel1
			' 
			Me.panel1.BackgroundImage = (CType(resources.GetObject("panel1.BackgroundImage"), System.Drawing.Image))
			Me.panel1.Location = New System.Drawing.Point(328, 461)
			Me.panel1.Name = "panel1"
			Me.panel1.Size = New System.Drawing.Size(200, 40)
			Me.panel1.TabIndex = 111
'			Me.panel1.MouseLeave += New System.EventHandler(Me.panel1_MouseLeave);
'			Me.panel1.MouseClick += New System.Windows.Forms.MouseEventHandler(Me.panel1_MouseClick);
'			Me.panel1.MouseEnter += New System.EventHandler(Me.panel1_MouseEnter);
			' 
			' timerSendByte
			' 
'			Me.timerSendByte.Tick += New System.EventHandler(Me.timerSendByte_Tick);
			' 
			' LEDDTR
			' 
			Me.LEDDTR.BackgroundImage = (CType(resources.GetObject("LEDDTR.BackgroundImage"), System.Drawing.Image))
			Me.LEDDTR.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
			Me.LEDDTR.image = 0
			Me.LEDDTR.Location = New System.Drawing.Point(63, 13)
			Me.LEDDTR.Name = "LEDDTR"
			Me.LEDDTR.Size = New System.Drawing.Size(12, 12)
			Me.LEDDTR.TabIndex = 115
			' 
			' LEDRTS
			' 
			Me.LEDRTS.BackgroundImage = (CType(resources.GetObject("LEDRTS.BackgroundImage"), System.Drawing.Image))
			Me.LEDRTS.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
			Me.LEDRTS.image = 0
			Me.LEDRTS.Location = New System.Drawing.Point(22, 13)
			Me.LEDRTS.Name = "LEDRTS"
			Me.LEDRTS.Size = New System.Drawing.Size(12, 12)
			Me.LEDRTS.TabIndex = 114
			' 
			' LEDRI
			' 
			Me.LEDRI.BackgroundImage = (CType(resources.GetObject("LEDRI.BackgroundImage"), System.Drawing.Image))
			Me.LEDRI.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
			Me.LEDRI.image = 0
			Me.LEDRI.Location = New System.Drawing.Point(192, 30)
			Me.LEDRI.Name = "LEDRI"
			Me.LEDRI.Size = New System.Drawing.Size(12, 12)
			Me.LEDRI.TabIndex = 113
			' 
			' LEDCTS
			' 
			Me.LEDCTS.BackgroundImage = (CType(resources.GetObject("LEDCTS.BackgroundImage"), System.Drawing.Image))
			Me.LEDCTS.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
			Me.LEDCTS.image = 0
			Me.LEDCTS.Location = New System.Drawing.Point(102, 30)
			Me.LEDCTS.Name = "LEDCTS"
			Me.LEDCTS.Size = New System.Drawing.Size(12, 12)
			Me.LEDCTS.TabIndex = 101
			' 
			' LEDDCD
			' 
			Me.LEDDCD.BackgroundImage = (CType(resources.GetObject("LEDDCD.BackgroundImage"), System.Drawing.Image))
			Me.LEDDCD.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
			Me.LEDDCD.image = 0
			Me.LEDDCD.Location = New System.Drawing.Point(162, 30)
			Me.LEDDCD.Name = "LEDDCD"
			Me.LEDDCD.Size = New System.Drawing.Size(12, 12)
			Me.LEDDCD.TabIndex = 107
			' 
			' LEDDSR
			' 
			Me.LEDDSR.BackgroundImage = (CType(resources.GetObject("LEDDSR.BackgroundImage"), System.Drawing.Image))
			Me.LEDDSR.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch
			Me.LEDDSR.image = 0
			Me.LEDDSR.Location = New System.Drawing.Point(132, 30)
			Me.LEDDSR.Name = "LEDDSR"
			Me.LEDDSR.Size = New System.Drawing.Size(12, 12)
			Me.LEDDSR.TabIndex = 106
			' 
			' copyToolStripMenuItem1
			' 
			Me.copyToolStripMenuItem1.Name = "copyToolStripMenuItem1"
			Me.copyToolStripMenuItem1.Size = New System.Drawing.Size(132, 22)
			Me.copyToolStripMenuItem1.Text = "Copy"
'			Me.copyToolStripMenuItem1.Click += New System.EventHandler(Me.copyToolStripMenuItem1_Click);
			' 
			' cutToolStripMenuItem1
			' 
			Me.cutToolStripMenuItem1.Name = "cutToolStripMenuItem1"
			Me.cutToolStripMenuItem1.Size = New System.Drawing.Size(132, 22)
			Me.cutToolStripMenuItem1.Text = "Cut"
'			Me.cutToolStripMenuItem1.Click += New System.EventHandler(Me.cutToolStripMenuItem1_Click);
			' 
			' deleteToolStripMenuItem1
			' 
			Me.deleteToolStripMenuItem1.Name = "deleteToolStripMenuItem1"
			Me.deleteToolStripMenuItem1.Size = New System.Drawing.Size(132, 22)
			Me.deleteToolStripMenuItem1.Text = "Clear"
'			Me.deleteToolStripMenuItem1.Click += New System.EventHandler(Me.deleteToolStripMenuItem1_Click);
			' 
			' selectAllToolStripMenuItem1
			' 
			Me.selectAllToolStripMenuItem1.Name = "selectAllToolStripMenuItem1"
			Me.selectAllToolStripMenuItem1.Size = New System.Drawing.Size(132, 22)
			Me.selectAllToolStripMenuItem1.Text = "Select All"
'			Me.selectAllToolStripMenuItem1.Click += New System.EventHandler(Me.selectAllToolStripMenuItem1_Click);
			' 
			' toolStripSeparator4
			' 
			Me.toolStripSeparator4.Name = "toolStripSeparator4"
			Me.toolStripSeparator4.Size = New System.Drawing.Size(129, 6)
			' 
			' saveFileAsToolStripMenuItem
			' 
			Me.saveFileAsToolStripMenuItem.Name = "saveFileAsToolStripMenuItem"
			Me.saveFileAsToolStripMenuItem.Size = New System.Drawing.Size(132, 22)
			Me.saveFileAsToolStripMenuItem.Text = "Save File As"
'			Me.saveFileAsToolStripMenuItem.Click += New System.EventHandler(Me.saveFileAsToolStripMenuItem_Click);
			' 
			' contextMenuStripReceive
			' 
			Me.contextMenuStripReceive.Items.AddRange(New System.Windows.Forms.ToolStripItem() { Me.copyToolStripMenuItem1, Me.cutToolStripMenuItem1, Me.deleteToolStripMenuItem1, Me.selectAllToolStripMenuItem1, Me.toolStripSeparator4, Me.saveFileAsToolStripMenuItem})
			Me.contextMenuStripReceive.Name = "contextMenuStripReceive"
			Me.contextMenuStripReceive.Size = New System.Drawing.Size(133, 120)
			' 
			' MainForm
			' 
			Me.AllowDrop = True
			Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
			Me.BackColor = System.Drawing.Color.Bisque
			Me.ClientSize = New System.Drawing.Size(575, 507)
			Me.Controls.Add(Me.linkLabelVisitWebSite)
			Me.Controls.Add(Me.menuStrip1)
			Me.Controls.Add(Me.groupBoxSendText)
			Me.Controls.Add(Me.panel1)
			Me.Controls.Add(Me.groupBoxReceive)
			Me.Controls.Add(Me.groupBoxMacros)
			Me.Controls.Add(Me.groupBox4)
			Me.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
			Me.Icon = (CType(resources.GetObject("$this.Icon"), System.Drawing.Icon))
			Me.MainMenuStrip = Me.menuStrip1
			Me.MaximizeBox = False
			Me.Name = "MainForm"
			Me.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide
			Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
			Me.Text = "Developer's Terminal"
'			Me.Load += New System.EventHandler(Me.Form1_Load);
			Me.groupBoxMacros.ResumeLayout(False)
			Me.groupBoxReceive.ResumeLayout(False)
			Me.groupBoxReceive.PerformLayout()
			Me.groupBox2.ResumeLayout(False)
			Me.groupBox1.ResumeLayout(False)
			Me.groupBox3.ResumeLayout(False)
			Me.groupBoxSendText.ResumeLayout(False)
			Me.groupBoxSendText.PerformLayout()
			Me.groupBox5.ResumeLayout(False)
			Me.groupBox5.PerformLayout()
			Me.menuStrip1.ResumeLayout(False)
			Me.menuStrip1.PerformLayout()
			Me.contextMenuStripSend.ResumeLayout(False)
			Me.groupBox4.ResumeLayout(False)
			Me.groupBox4.PerformLayout()
			Me.contextMenuStripReceive.ResumeLayout(False)
			Me.ResumeLayout(False)
			Me.PerformLayout()

		End Sub
		#End Region

		#End Region

		#Region "GUI"

		#Region "Open REALLY GREAT website"

		' And this is not shameless commercialism. I have NO shame.
		' And2 - now you know how to do it.
		Private Sub linkLabelVisitWebSite_LinkClicked(ByVal sender As Object, ByVal e As System.Windows.Forms.LinkLabelLinkClickedEventArgs) Handles linkLabelVisitWebSite.LinkClicked
			System.Diagnostics.Process.Start("www.smileymicros.com")
		End Sub

		' Open website 
		Private Sub panel1_MouseClick(ByVal sender As Object, ByVal e As MouseEventArgs) Handles panel1.MouseClick
			System.Diagnostics.Process.Start("www.smileymicros.com")
		End Sub

		' Show hand cursor and change color to intice click to website
		Private Sub panel1_MouseEnter(ByVal sender As Object, ByVal e As EventArgs) Handles panel1.MouseEnter
			panel1.Cursor = Cursors.Hand
			panel1.BackColor = Color.SandyBrown
		End Sub

		' Cursor reverts on leaving, but color doesn't so revert it
		Private Sub panel1_MouseLeave(ByVal sender As Object, ByVal e As EventArgs) Handles panel1.MouseLeave
			panel1.BackColor = Color.Bisque
		End Sub

		#End Region

		#Region "Buttons   "
		Private Sub buttonClear_Click(ByVal sender As Object, ByVal e As System.EventArgs)
			richTextBoxReceive.Text = ""
		End Sub

		Private Sub buttonClearSend_Click(ByVal sender As Object, ByVal e As EventArgs)
			richTextBoxSend.Text = ""
		End Sub

		Private Sub buttonSettings_Click(ByVal sender As Object, ByVal e As System.EventArgs)
			Dim portSettings As PortSettings = New PortSettings()

			portSettings.ShowDialog()
			If portSettings.DialogResult = System.Windows.Forms.DialogResult.OK Then
				serialPort1.PortName = portSettings.selectedPort
				serialPort1.BaudRate = portSettings.selectedBaudrate
			End If
		End Sub

		Private Sub buttonSend_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles buttonSendQue.Click
			richTextBoxSend.Text += Constants.vbLf & " Sent QueuedString: " & Constants.vbLf & QueuedString & Constants.vbLf
			SendASCIIString(QueuedString)
			If UseTermChar Then
			sendTermChar()
			End If
			QueuedString = ""
		End Sub

		Private Sub buttonOpenClosePort_Click(ByVal sender As Object, ByVal e As System.EventArgs)
			If PortOpen Then
				PortOpen = False
				serialPort1.Close()
				Me.Text = "Smiley Micros COM Port Terminal - Port is closed"
			Else
				PortOpen = True

				Try
					serialPort1.Open()
				Catch ex As System.Exception
					MessageBox.Show("Error - buttonOpenClosePort_ClickSystem.Exception: ", ex.Message)
				End Try
				showSettings()
			End If
		End Sub

		Private Sub buttonHelp_Click(ByVal sender As Object, ByVal e As System.EventArgs)
			MessageBox.Show("HELP! is coming soon to a computer near you...")
		End Sub

		Private Sub ButtonReloadMacrosClick(ByVal sender As Object, ByVal e As System.EventArgs)
			xMLData = dataXMLIO.XMLRead()
			listBoxMacros.Items.Clear()
			loadXMLData()
		End Sub
		#End Region

		#Region "Check Box and Radio Buttons"

		Private Sub radioButtonImmediate_CheckedChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles radioButtonImmediate.CheckedChanged
			If radioButtonImmediate.Checked = True Then
				SendImmediate = True
				buttonSendQue.Enabled = False
			End If
		End Sub

		Private Sub radioButtonQueued_CheckedChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles radioButtonQueued.CheckedChanged
			If radioButtonQueued.Checked = True Then
				SendImmediate = False
				buttonSendQue.Enabled = True
			End If
		End Sub

		Private Sub radioButtonASCII_CheckedChanged_1(ByVal sender As Object, ByVal e As System.EventArgs) Handles radioButtonASCII.CheckedChanged
			ReceiveASCII = True
		End Sub

		Private Sub radioButtonHEX_CheckedChanged_1(ByVal sender As Object, ByVal e As System.EventArgs) Handles radioButtonHEX.CheckedChanged
			ReceiveASCII = False
		End Sub

		Private Sub radioButtonTermCharYes_CheckedChanged(ByVal sender As Object, ByVal e As EventArgs) Handles radioButtonTermCharYes.CheckedChanged
			If radioButtonTermCharYes.Checked = True Then
				UseTermChar = True
			End If
		End Sub

		Private Sub radioButtonTermCharNo_CheckedChanged(ByVal sender As Object, ByVal e As EventArgs) Handles radioButtonTermCharNo.CheckedChanged
			If radioButtonTermCharNo.Checked = True Then
				UseTermChar = False
			End If
		End Sub

		#End Region

		#End Region

		#Region "Receive functions"

		' we want to have the serial port thread report back data received, but to display
		' that data we must create a delegate function to show the data in the richTextBox

		' define the delegate 
		Public Delegate Sub SetText()
		' define an instance of the delegate
		Private setTextI As SetText

		' create a string that will be loaded with the data received from the port
		Public str As String = ""
		' JWP 4/28/07 added public byt for receiving bytes
		Public byt As Byte = 0

		' note that this function runs in a separate thread and thus we must use a delegate in order
		' to display the results in the richTextBox.
		Private Sub serialPort1_DataReceived(ByVal sender As Object, ByVal e As SerialDataReceivedEventArgs) Handles serialPort1.DataReceived
			' instantiate the delegate to be invoked by this thread
			setTextI = New SetText(AddressOf mySetText)

			' load the data into the string
			Try
				' JWP 4/28/07 added ReceiveASCII and byte reception
				If ReceiveASCII Then
					str = serialPort1.ReadExisting()
				Else
					byt = CByte(serialPort1.ReadByte()) '.ReadExisting();
				End If
			Catch ex As System.Exception
				MessageBox.Show("Error - port_DataReceived Exception: ", ex.Message)
			End Try

			' invoke the delegate in the MainForm thread
			Me.Invoke(setTextI)
		End Sub

		' create the instance of the delegate to be used to write the received data to the richTextBox
		Public Sub mySetText()
			ReceiveCount += str.Length
			textBoxReceiveCount.Text = ReceiveCount.ToString()

			If ReceiveASCII Then
				richTextBoxReceive.Text += str.ToString()
			Else ' ReceiveHEX
				' JWP 4/28/07 changed byte display
				If byt > 15 Then
					richTextBoxReceive.Text &= "0x" & byt.ToString("X") & ","
				Else
					richTextBoxReceive.Text &= "0x0" & byt.ToString("X") & ","
				End If

			End If

		End Sub

		' This rigaramole is needed to keep the last received item displayed
		' it kind of flickers and should be fixed
		Private Sub richTextBoxReceive_TextChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles richTextBoxReceive.TextChanged
			moveCaretToEnd()
		End Sub

		Private Sub moveCaretToEnd()
			richTextBoxReceive.SelectionStart = richTextBoxReceive.Text.Length
			richTextBoxReceive.SelectionLength = 0
			richTextBoxReceive.ScrollToCaret()
		End Sub

	   #End Region

		#Region "Send Text functions"
		Private Sub richTextBoxSend_KeyPress(ByVal sender As Object, ByVal e As KeyPressEventArgs) Handles richTextBoxSend.KeyPress
			If SendImmediate Then
				sendChar(e.KeyChar)
				If UseTermChar Then
				sendTermChar()
				End If
			Else
				QueuedString &= e.KeyChar
			End If
		End Sub

		Private Sub sendTermChar()
			SendHEXString(xMLData.TerminalCharacters)
		End Sub

		Private Sub sendChar(ByVal c As Char)
			Dim data As Char() = New Char(0){}
			data(0) = c
			Try
				serialPort1.Write(data, 0, 1)
				SendCount += 1
				textBoxSendCount.Text = SendCount.ToString()
				' Show bytes sent by shooter
				If shoot Then
				richTextBoxSend.Text += c
				End If
			Catch
				MessageBox.Show("Error: sendChar - failed to send." & Constants.vbLf & "Is the port open?")
			End Try
		End Sub

		'JWP 4/28/07 - add sendByte function
		Private Sub sendByte(ByVal b As Byte)
			Dim data As Byte() = New Byte(0){}
			data(0) = b
			Try
				serialPort1.Write(data, 0, 1)
				SendCount += 1
				textBoxSendCount.Text = SendCount.ToString()
				' Show bytes sent by shooter
				'JWP 4/28/07 added show with 0x or 0x0
				If shoot Then 'richTextBoxSend.Text += b.ToString();
					If byt > 15 Then
						richTextBoxSend.Text &= "0x" & b.ToString("X") & ","
					Else
						richTextBoxSend.Text &= "0x0" & b.ToString("X") & ","
					End If
				End If
			Catch
				MessageBox.Show("Error: sendChar - failed to send." & Constants.vbLf & "Is the port open?")
			End Try
		End Sub

		' Send single hex bytes immediately
		Private Sub comboBoxSendByte_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles comboBoxSendByte.SelectedIndexChanged
			'JWP 4/28/07 - changed from sendChar to sendByte
			'sendChar((char)comboBoxSendByte.SelectedIndex);
            sendByte(Convert.ToByte(comboBoxSendByte.SelectedIndex))
			Me.richTextBoxSend.Text = "Sent: 0x" & comboBoxSendByte.SelectedIndex.ToString("X")
		End Sub



		#End Region

		#Region "Send Macro Functions"
		' Send Macro Stings
		Private Sub ListBoxMacrosDoubleClick(ByVal sender As Object, ByVal e As System.EventArgs) Handles listBoxMacros.DoubleClick
			processMacroClick(MacroDataData(listBoxMacros.SelectedIndex),MacroDataType(listBoxMacros.SelectedIndex))
		End Sub
		Private Sub processMacroClick(ByVal MacroData As String, ByVal MacroType As String)
			richTextBoxSend.Text = MacroData
			If MacroType = "ASCII" Then
				SendASCIIString(MacroData)
				If UseTermChar Then
				sendTermChar()
				End If
			ElseIf MacroType = "HEX" Then
				SendHEXString(MacroData)
				If UseTermChar Then
				sendTermChar()
				End If
			Else
				MessageBox.Show("Error: MacroType: " & MacroType)
			End If
		End Sub

		Public Sub SendASCIIString(ByVal str As String)
			Dim c As Char() = str.ToCharArray()

			For i As Integer = 0 To c.GetLength(0) - 1
				sendChar(c(i))
			Next i
		End Sub

		' For our purposes, HEX strings must be in the format 0xFF,0xFE, 
		' including the trailing comma on the last value
		Public Sub SendHEXString(ByVal str As String)
			Dim c As Char() = str.ToCharArray()
			Dim b As Byte = 0

			If c.GetLength(0) < 5 Then
				MessageBox.Show("Error: Hex string incorrect length: " & str & Constants.vbLf & "Length = " & c.GetLength(0).ToString())
				Return
			End If
			For i As Integer = 0 To c.GetLength(0) - 1 Step 5
				' Check array length
				If i > c.GetLength(0) Then
					MessageBox.Show("Error: Hex string incorrect length: " & str & Constants.vbLf & "Length = " & c.GetLength(0).ToString() & Constants.vbLf & "i = " & i.ToString())
					Return
				End If
				' Check for remaining length 
				If i + 4 >= c.GetLength(0) Then
					MessageBox.Show("Error: Hex string: " & str & " problem with length")
					Return
				End If
				' Check format trailing comma
				If c(i + 4) <> ","c Then
					MessageBox.Show("Error: Hex string: " & str & " lacks a trailing comma.")
					Return
				End If
				' Check high nibble range 0 - F
				If (c(i + 2) >= "0"c) AndAlso (c(i + 2) <= "9"c) AndAlso (c(i + 2) >= "A"c) AndAlso (c(i + 2) <= "F"c) Then
					MessageBox.Show("Error: Hex string first digit: " & c(i + 2).ToString() & " not in range 0 - F")
					Return
				End If
				' Check low nibble range 0 - F
				If (c(i + 3) >= "0"c) AndAlso (c(i + 3) <= "9"c) AndAlso (c(i + 3) >= "A"c) AndAlso (c(i + 3) <= "F"c) Then
					MessageBox.Show("Error: Hex string first digit: " & c(i + 2).ToString() & " not in range 0 - F")
					Return
                End If
                If c(i + 2) <= "9"c Then
                    b = CByte(AscW(c(i + 2)) - AscW("O"c))
                Else
                    b = CByte(AscW(c(i + 2)) - AscW("A"c))
                    b += CByte(10)
                End If

                b = CByte(b << 4)

                If c(i + 3) <= "9"c Then
                    b += CByte(AscW(c(i + 3)) - AscW("0"))
                Else
                    b += CByte(AscW(c(i + 3)) - AscW("A"))
                    b += CByte(10)
                End If

                sendChar(ChrW(b))

            Next i
		End Sub

		#End Region

		#Region "Load macro data"
		Private Sub loadXMLData()
			listBoxMacros.Items.Add(xMLData.MacroText1)
			listBoxMacros.Items.Add(xMLData.MacroText2)
			listBoxMacros.Items.Add(xMLData.MacroText3)
			listBoxMacros.Items.Add(xMLData.MacroText4)
			listBoxMacros.Items.Add(xMLData.MacroText5)
			listBoxMacros.Items.Add(xMLData.MacroText6)
			listBoxMacros.Items.Add(xMLData.MacroText7)
			listBoxMacros.Items.Add(xMLData.MacroText8)
			listBoxMacros.Items.Add(xMLData.MacroText9)
			listBoxMacros.Items.Add(xMLData.MacroText10)
			listBoxMacros.Items.Add(xMLData.MacroText11)
			listBoxMacros.Items.Add(xMLData.MacroText12)
			listBoxMacros.Items.Add(xMLData.MacroText13)
			listBoxMacros.Items.Add(xMLData.MacroText14)
			listBoxMacros.Items.Add(xMLData.MacroText15)
			listBoxMacros.Items.Add(xMLData.MacroText16)
			listBoxMacros.Items.Add(xMLData.MacroText17)
			listBoxMacros.Items.Add(xMLData.MacroText18)
			listBoxMacros.Items.Add(xMLData.MacroText19)
			listBoxMacros.Items.Add(xMLData.MacroText20)
			listBoxMacros.Items.Add(xMLData.MacroText21)
			listBoxMacros.Items.Add(xMLData.MacroText22)
			listBoxMacros.Items.Add(xMLData.MacroText23)
			listBoxMacros.Items.Add(xMLData.MacroText24)
			listBoxMacros.Items.Add(xMLData.MacroText25)
			listBoxMacros.Items.Add(xMLData.MacroText26)
			listBoxMacros.Items.Add(xMLData.MacroText27)
			listBoxMacros.Items.Add(xMLData.MacroText28)
			listBoxMacros.Items.Add(xMLData.MacroText29)
			listBoxMacros.Items.Add(xMLData.MacroText30)
			listBoxMacros.Items.Add(xMLData.MacroText31)
			listBoxMacros.Items.Add(xMLData.MacroText32)
			listBoxMacros.Items.Add(xMLData.MacroText33)
			listBoxMacros.Items.Add(xMLData.MacroText34)
			listBoxMacros.Items.Add(xMLData.MacroText35)
			listBoxMacros.Items.Add(xMLData.MacroText36)
			listBoxMacros.Items.Add(xMLData.MacroText37)
			listBoxMacros.Items.Add(xMLData.MacroText38)
			listBoxMacros.Items.Add(xMLData.MacroText39)
			listBoxMacros.Items.Add(xMLData.MacroText40)
			listBoxMacros.Items.Add(xMLData.MacroText41)
			listBoxMacros.Items.Add(xMLData.MacroText42)
			listBoxMacros.Items.Add(xMLData.MacroText43)
			listBoxMacros.Items.Add(xMLData.MacroText44)
			listBoxMacros.Items.Add(xMLData.MacroText45)
			listBoxMacros.Items.Add(xMLData.MacroText46)
			listBoxMacros.Items.Add(xMLData.MacroText47)
			listBoxMacros.Items.Add(xMLData.MacroText48)
			listBoxMacros.Items.Add(xMLData.MacroText49)
			listBoxMacros.Items.Add(xMLData.MacroText50)
			listBoxMacros.Items.Add(xMLData.MacroText51)
			listBoxMacros.Items.Add(xMLData.MacroText52)

			MacroDataText(0) = xMLData.MacroText1
			MacroDataText(1) = xMLData.MacroText2
			MacroDataText(2) = xMLData.MacroText3
			MacroDataText(3) = xMLData.MacroText4
			MacroDataText(4) = xMLData.MacroText5
			MacroDataText(5) = xMLData.MacroText6
			MacroDataText(6) = xMLData.MacroText7
			MacroDataText(7) = xMLData.MacroText8
			MacroDataText(8) = xMLData.MacroText9
			MacroDataText(9) = xMLData.MacroText10
			MacroDataText(10) = xMLData.MacroText11
			MacroDataText(11) = xMLData.MacroText12
			MacroDataText(12) = xMLData.MacroText13
			MacroDataText(13) = xMLData.MacroText14
			MacroDataText(14) = xMLData.MacroText15
			MacroDataText(15) = xMLData.MacroText16
			MacroDataText(16) = xMLData.MacroText17
			MacroDataText(17) = xMLData.MacroText18
			MacroDataText(18) = xMLData.MacroText19
			MacroDataText(19) = xMLData.MacroText20
			MacroDataText(20) = xMLData.MacroText21
			MacroDataText(21) = xMLData.MacroText22
			MacroDataText(22) = xMLData.MacroText23
			MacroDataText(23) = xMLData.MacroText24
			MacroDataText(24) = xMLData.MacroText25
			MacroDataText(25) = xMLData.MacroText26
			MacroDataText(26) = xMLData.MacroText27
			MacroDataText(27) = xMLData.MacroText28
			MacroDataText(28) = xMLData.MacroText29
			MacroDataText(29) = xMLData.MacroText30
			MacroDataText(30) = xMLData.MacroText31
			MacroDataText(31) = xMLData.MacroText32
			MacroDataText(32) = xMLData.MacroText33
			MacroDataText(33) = xMLData.MacroText34
			MacroDataText(34) = xMLData.MacroText35
			MacroDataText(35) = xMLData.MacroText36
			MacroDataText(36) = xMLData.MacroText37
			MacroDataText(37) = xMLData.MacroText38
			MacroDataText(38) = xMLData.MacroText39
			MacroDataText(39) = xMLData.MacroText40
			MacroDataText(40) = xMLData.MacroText41
			MacroDataText(41) = xMLData.MacroText42
			MacroDataText(42) = xMLData.MacroText43
			MacroDataText(43) = xMLData.MacroText44
			MacroDataText(44) = xMLData.MacroText45
			MacroDataText(45) = xMLData.MacroText46
			MacroDataText(46) = xMLData.MacroText47
			MacroDataText(47) = xMLData.MacroText48
			MacroDataText(48) = xMLData.MacroText49
			MacroDataText(49) = xMLData.MacroText50
			MacroDataText(50) = xMLData.MacroText51
			MacroDataText(51) = xMLData.MacroText52

			MacroDataData(0) = xMLData.MacroData1
			MacroDataData(1) = xMLData.MacroData2
			MacroDataData(2) = xMLData.MacroData3
			MacroDataData(3) = xMLData.MacroData4
			MacroDataData(4) = xMLData.MacroData5
			MacroDataData(5) = xMLData.MacroData6
			MacroDataData(6) = xMLData.MacroData7
			MacroDataData(7) = xMLData.MacroData8
			MacroDataData(8) = xMLData.MacroData9
			MacroDataData(9) = xMLData.MacroData10
			MacroDataData(10) = xMLData.MacroData11
			MacroDataData(11) = xMLData.MacroData12
			MacroDataData(12) = xMLData.MacroData13
			MacroDataData(13) = xMLData.MacroData14
			MacroDataData(14) = xMLData.MacroData15
			MacroDataData(15) = xMLData.MacroData16
			MacroDataData(16) = xMLData.MacroData17
			MacroDataData(17) = xMLData.MacroData18
			MacroDataData(18) = xMLData.MacroData19
			MacroDataData(19) = xMLData.MacroData20
			MacroDataData(20) = xMLData.MacroData21
			MacroDataData(21) = xMLData.MacroData22
			MacroDataData(22) = xMLData.MacroData23
			MacroDataData(23) = xMLData.MacroData24
			MacroDataData(24) = xMLData.MacroData25
			MacroDataData(25) = xMLData.MacroData26
			MacroDataData(26) = xMLData.MacroData27
			MacroDataData(27) = xMLData.MacroData28
			MacroDataData(28) = xMLData.MacroData29
			MacroDataData(29) = xMLData.MacroData30
			MacroDataData(30) = xMLData.MacroData31
			MacroDataData(31) = xMLData.MacroData32
			MacroDataData(32) = xMLData.MacroData33
			MacroDataData(33) = xMLData.MacroData34
			MacroDataData(34) = xMLData.MacroData35
			MacroDataData(35) = xMLData.MacroData36
			MacroDataData(36) = xMLData.MacroData37
			MacroDataData(37) = xMLData.MacroData38
			MacroDataData(38) = xMLData.MacroData39
			MacroDataData(39) = xMLData.MacroData40
			MacroDataData(40) = xMLData.MacroData41
			MacroDataData(41) = xMLData.MacroData42
			MacroDataData(42) = xMLData.MacroData43
			MacroDataData(43) = xMLData.MacroData44
			MacroDataData(44) = xMLData.MacroData45
			MacroDataData(45) = xMLData.MacroData46
			MacroDataData(46) = xMLData.MacroData47
			MacroDataData(47) = xMLData.MacroData48
			MacroDataData(48) = xMLData.MacroData49
			MacroDataData(49) = xMLData.MacroData50
			MacroDataData(50) = xMLData.MacroData51
			MacroDataData(51) = xMLData.MacroData52

			MacroDataType(0) = xMLData.MacroType1
			MacroDataType(1) = xMLData.MacroType2
			MacroDataType(2) = xMLData.MacroType3
			MacroDataType(3) = xMLData.MacroType4
			MacroDataType(4) = xMLData.MacroType5
			MacroDataType(5) = xMLData.MacroType6
			MacroDataType(6) = xMLData.MacroType7
			MacroDataType(7) = xMLData.MacroType8
			MacroDataType(8) = xMLData.MacroType9
			MacroDataType(9) = xMLData.MacroType10
			MacroDataType(10) = xMLData.MacroType11
			MacroDataType(11) = xMLData.MacroType12
			MacroDataType(12) = xMLData.MacroType13
			MacroDataType(13) = xMLData.MacroType14
			MacroDataType(14) = xMLData.MacroType15
			MacroDataType(15) = xMLData.MacroType16
			MacroDataType(16) = xMLData.MacroType17
			MacroDataType(17) = xMLData.MacroType18
			MacroDataType(18) = xMLData.MacroType19
			MacroDataType(19) = xMLData.MacroType20
			MacroDataType(20) = xMLData.MacroType21
			MacroDataType(21) = xMLData.MacroType22
			MacroDataType(22) = xMLData.MacroType23
			MacroDataType(23) = xMLData.MacroType24
			MacroDataType(24) = xMLData.MacroType25
			MacroDataType(25) = xMLData.MacroType26
			MacroDataType(26) = xMLData.MacroType27
			MacroDataType(27) = xMLData.MacroType28
			MacroDataType(28) = xMLData.MacroType29
			MacroDataType(29) = xMLData.MacroType30
			MacroDataType(30) = xMLData.MacroType31
			MacroDataType(31) = xMLData.MacroType32
			MacroDataType(32) = xMLData.MacroType33
			MacroDataType(33) = xMLData.MacroType34
			MacroDataType(34) = xMLData.MacroType35
			MacroDataType(35) = xMLData.MacroType36
			MacroDataType(36) = xMLData.MacroType37
			MacroDataType(37) = xMLData.MacroType38
			MacroDataType(38) = xMLData.MacroType39
			MacroDataType(39) = xMLData.MacroType40
			MacroDataType(40) = xMLData.MacroType41
			MacroDataType(41) = xMLData.MacroType42
			MacroDataType(42) = xMLData.MacroType43
			MacroDataType(43) = xMLData.MacroType44
			MacroDataType(44) = xMLData.MacroType45
			MacroDataType(45) = xMLData.MacroType46
			MacroDataType(46) = xMLData.MacroType47
			MacroDataType(47) = xMLData.MacroType48
			MacroDataType(48) = xMLData.MacroType49
			MacroDataType(49) = xMLData.MacroType50
			MacroDataType(50) = xMLData.MacroType51
			MacroDataType(51) = xMLData.MacroType52
		End Sub

		#End Region                

		#Region "Send Textbox Context menus "
		Private Sub richTextBoxSend_MouseDown(ByVal sender As Object, ByVal e As MouseEventArgs) Handles richTextBoxSend.MouseDown
			If e.Button = MouseButtons.Right Then
				contextMenuStripSend.Show(MousePosition)
			End If
		End Sub

		Private Sub copyToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles copyToolStripMenuItem.Click
			richTextBoxSend.Copy()
		End Sub

		Private Sub cutToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles cutToolStripMenuItem.Click
			richTextBoxSend.Cut()
		End Sub


		Private Sub deleteToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles deleteToolStripMenuItem.Click
			richTextBoxSend.Clear()
			textBoxSendCount.Text = "0"
			SendCount = 0
		End Sub

		Private Sub selectAllToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles selectAllToolStripMenuItem.Click
			richTextBoxSend.SelectAll()
		End Sub

		Private Sub undoToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles undoToolStripMenuItem.Click
			richTextBoxSend.Undo()
		End Sub

		Private Sub redoToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles redoToolStripMenuItem.Click
			richTextBoxSend.Redo()
		End Sub

		Private Sub saveAsToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles saveAsToolStripMenuItem.Click
			saveSendBox()
		End Sub

		Private Sub pasteToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles pasteToolStripMenuItem.Click
			richTextBoxSend.Paste()
			setToQueued()
		End Sub

		' use for paste and load
		Private Sub setToQueued()
			' set to send queued
			radioButtonQueued.Checked = True
			SendImmediate = False
			buttonSendQue.Enabled = True
			QueuedString = richTextBoxSend.Text
		End Sub

		' Context menu item
		Private Sub loadFileToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles loadFileToolStripMenuItem.Click
			openInSendBox()
		End Sub
		' File menu item
		Private Sub openInSendBoxToolStripMenuItem_Click_1(ByVal sender As Object, ByVal e As EventArgs) Handles openInSendBoxToolStripMenuItem.Click
			openInSendBox()
		End Sub

		Private Sub saveSendBoxToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs)
			saveSendBox()
		End Sub

		Private Sub saveSendBoxToolStripMenuItem_Click_1(ByVal sender As Object, ByVal e As EventArgs) Handles saveSendBoxToolStripMenuItem.Click
			saveSendBox()
		End Sub

		Private Sub openInSendBox()
			Dim filename As String = ""
			If openFileDialog1.ShowDialog() = System.Windows.Forms.DialogResult.OK Then
				filename = openFileDialog1.FileName
				If filename <> "" Then
					richTextBoxSend.LoadFile(filename, RichTextBoxStreamType.PlainText)
					setToQueued()
				End If
			Else
				MessageBox.Show("Error loading file.")
			End If
		End Sub

		Private Sub saveSendBox()
			Dim filename As String = ""
			If saveFileDialog1.ShowDialog() = System.Windows.Forms.DialogResult.OK Then
				filename = saveFileDialog1.FileName
				If filename <> "" Then
				richTextBoxSend.SaveFile(filename, RichTextBoxStreamType.PlainText)
				End If
			End If
		End Sub

		#End Region

		#Region "Receive Textbox Context menus"

		Private Sub richTextBoxReceive_MouseDown(ByVal sender As Object, ByVal e As MouseEventArgs) Handles richTextBoxReceive.MouseDown
			If e.Button = MouseButtons.Right Then
				contextMenuStripReceive.Show(MousePosition)
			End If
		End Sub

		Private Sub copyToolStripMenuItem1_Click(ByVal sender As Object, ByVal e As EventArgs) Handles copyToolStripMenuItem1.Click
			richTextBoxReceive.Copy()
		End Sub

		Private Sub cutToolStripMenuItem1_Click(ByVal sender As Object, ByVal e As EventArgs) Handles cutToolStripMenuItem1.Click
			richTextBoxReceive.Cut()
		End Sub

		Private Sub selectAllToolStripMenuItem1_Click(ByVal sender As Object, ByVal e As EventArgs) Handles selectAllToolStripMenuItem1.Click
			richTextBoxReceive.SelectAll()
		End Sub

		Private Sub deleteToolStripMenuItem1_Click(ByVal sender As Object, ByVal e As EventArgs) Handles deleteToolStripMenuItem1.Click
			richTextBoxReceive.Clear()
		End Sub

		Private Sub saveFileAsToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles saveFileAsToolStripMenuItem.Click
			saveReceiveBox()
		End Sub

		Private Sub saveReceiveBoxToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs)
			saveReceiveBox()
		End Sub

		Private Sub saveReceiveBoxToolStripMenuItem_Click_1(ByVal sender As Object, ByVal e As EventArgs) Handles saveReceiveBoxToolStripMenuItem.Click
			saveReceiveBox()
		End Sub

		Private Sub saveReceiveBox()
			Dim filename As String = ""
			If saveFileDialog1.ShowDialog() = System.Windows.Forms.DialogResult.OK Then
				filename = saveFileDialog1.FileName
				If filename <> "" Then
				richTextBoxReceive.SaveFile(filename, RichTextBoxStreamType.PlainText)
				End If
			End If
		End Sub

		#End Region

		#Region "Send & Receive richTextBox settings"
		Private Sub sendToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles sendToolStripMenuItem.Click
			Dim send As TextBoxSettings = New TextBoxSettings()

			If send.ShowDialog() = System.Windows.Forms.DialogResult.OK Then
				richTextBoxSend.BackColor = send.backClr
				richTextBoxSend.ForeColor = send.foreClr
				If Not send.font Is Nothing Then
				richTextBoxSend.Font = send.fnt
				End If
			Else
				MessageBox.Show("Send richTextBox settings canceled.")
			End If

		End Sub

		Private Sub receiveToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles receiveToolStripMenuItem.Click
			Dim receive As TextBoxSettings = New TextBoxSettings()

			If receive.ShowDialog() = System.Windows.Forms.DialogResult.OK Then
				richTextBoxReceive.BackColor = receive.backClr
				richTextBoxReceive.ForeColor = receive.foreClr
				If Not receive.font Is Nothing Then
                    richTextBoxReceive.Font = receive.fnt
				End If
			Else
				MessageBox.Show("Receive richTextBox settings canceled.")
			End If
		End Sub

		#End Region

		#Region "COM port settings"
		Private Sub portToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles portToolStripMenuItem.Click
			' Make sure the port isn't already open
			If serialPort1.IsOpen Then
				MessageBox.Show("The port must be closed before changing the settings.")
				Return
			Else
				If portSettings.ShowDialog() = System.Windows.Forms.DialogResult.OK Then
					If portSettings.selectedPort <> "" Then
						' set the serial port to the new settings
						serialPort1.PortName = portSettings.selectedPort
						serialPort1.BaudRate = portSettings.selectedBaudrate
						serialPort1.DataBits = portSettings.selectedDataBits
						serialPort1.Parity = portSettings.selectedParity
						serialPort1.StopBits = portSettings.selectedStopBits

						' Show the new settings in the form text line
						showSettings()
					Else
						MessageBox.Show("Error: Settings form returned with no COM port selected.")
						Return ' bail out
					End If
				Else
					MessageBox.Show("Error: buttonSetup_Click - Settings dialog box did not return Okay.")
					Return ' bail out
				End If

				' Open the port
				openPort()

				' Show the modem line states
				showCTS_DSR_CD()
			End If
		End Sub

		' show the settings in the form text line
		Private Sub showSettings()
			Me.Text = "Smiley Micros - " & portSettings.selectedPort & " " & portSettings.selectedBaudrate.ToString() & "," & portSettings.selectedParity & "," & portSettings.selectedDataBits.ToString() & "," & portSettings.selectedStopBits & " - " & portSettings.selectedHandshaking
			If serialPort1.IsOpen Then
				Me.Text &= " - Port is open"
			Else
				Me.Text &= " - Port is closed"
			End If
		End Sub

		Private Sub openPortToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles openPortToolStripMenuItem.Click
			openPort()
		End Sub

		Private Sub openPort()
			Try
				If serialPort1.IsOpen Then
					serialPort1.Close()
					openPortToolStripMenuItem.Text = "Open Port"
				Else
					serialPort1.Open()
					openPortToolStripMenuItem.Text = "Close Port"
				End If

				showSettings()
			Catch ex As System.Exception
				MessageBox.Show("Error - openPortToolStripMenuItem_Click Exception: ", ex.Message)
			End Try
		End Sub
		#End Region

		#Region "Modem lines"
		Private Sub showCTS_DSR_CD()
			If serialPort1.IsOpen Then
				If serialPort1.CtsHolding Then
				Me.LEDCTS.image = CInt(Fix(LED.LED22.LEDColor.BlueOn))
				Else
					Me.LEDCTS.image = CInt(Fix(LED.LED22.LEDColor.BlueOff))
				End If

				If serialPort1.DsrHolding Then
				Me.LEDDSR.image = CInt(Fix(LED.LED22.LEDColor.BlueOn))
				Else
					Me.LEDDSR.image = CInt(Fix(LED.LED22.LEDColor.BlueOff))
				End If

				If serialPort1.CDHolding Then
				Me.LEDDCD.image = CInt(Fix(LED.LED22.LEDColor.BlueOn))
				Else
					Me.LEDDCD.image = CInt(Fix(LED.LED22.LEDColor.BlueOff))
				End If

				If RingIndicator Then
				Me.LEDRI.image = CInt(Fix(LED.LED22.LEDColor.BlueOn))
				Else
					Me.LEDRI.image = CInt(Fix(LED.LED22.LEDColor.BlueOff))
				End If
			End If
		End Sub

		' Toggle RTS state
		Private Sub buttonRTS_Click(ByVal sender As Object, ByVal e As EventArgs) Handles buttonRTS.Click
			If RTSToggle Then
				serialPort1.RtsEnable = True
				LEDRTS.image = CInt(Fix(LED.LED22.LEDColor.RedOn))
			Else
				serialPort1.RtsEnable = False
				LEDRTS.image = CInt(Fix(LED.LED22.LEDColor.RedOff))
			End If
			RTSToggle = Not RTSToggle
		End Sub

		' Toggle DTR state
		Private Sub buttonDTR_Click(ByVal sender As Object, ByVal e As EventArgs) Handles buttonDTR.Click
			If DTRToggle Then
				serialPort1.DtrEnable = True
				LEDDTR.image = CInt(Fix(LED.LED22.LEDColor.RedOn))
			Else
				serialPort1.DtrEnable = False
				LEDDTR.image = CInt(Fix(LED.LED22.LEDColor.RedOff))
			End If
			DTRToggle = Not DTRToggle
		End Sub

		' Check Modem input line status
		Private Sub timerOneSec_Tick(ByVal sender As Object, ByVal e As EventArgs) Handles timerModemLines.Tick
			showCTS_DSR_CD()
		End Sub

		' Show the modem states on the virtual LEDs
		Private Sub serialPort1_PinChanged(ByVal sender As Object, ByVal e As SerialPinChangedEventArgs) Handles serialPort1.PinChanged
			' Toggle RI since we can't determine the state with the SerialPort class
			If e.EventType = SerialPinChange.Ring Then
			RingIndicator = Not RingIndicator
			End If

			showCTS_DSR_CD()
		End Sub

		#End Region

		#Region "Byte Shooter"
		' Send a character at regular intervals
		Private shoot As Boolean = False
		Private byteToSend As Byte = 0
		Private Sub sendCharToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles sendCharToolStripMenuItem.Click
			If (Not shoot) Then
				shoot = True
				sendCharToolStripMenuItem.Text = "Stop sending bytes!"
				Dim shooter As ByteShooter = New ByteShooter()

				If shooter.ShowDialog() = System.Windows.Forms.DialogResult.OK Then
					byteToSend = shooter.byteToSend
					timerSendByte.Interval = shooter.interval
					timerSendByte.Enabled = True
				End If
			Else
				timerSendByte.Enabled = False
				shoot = False
				sendCharToolStripMenuItem.Text = "Send byte periodically."
			End If
		End Sub

		Private Sub timerSendByte_Tick(ByVal sender As Object, ByVal e As EventArgs) Handles timerSendByte.Tick
			'JWP 4/28/07 - changed from sendChar to sendByte
			sendByte(byteToSend)
		End Sub
		#End Region

		#Region "Help"

		Private Sub manualToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles manualToolStripMenuItem.Click
			System.Diagnostics.Process.Start("Developer Terminal.pdf")
		End Sub

		Private Sub smileyMicrosToolStripMenuItem_Click(ByVal sender As Object, ByVal e As EventArgs) Handles smileyMicrosToolStripMenuItem.Click
			System.Diagnostics.Process.Start("www.smileymicros.com")
		End Sub

		#End Region
	End Class
End Namespace

