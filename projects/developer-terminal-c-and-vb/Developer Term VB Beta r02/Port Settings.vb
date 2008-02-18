Imports Microsoft.VisualBasic
Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Data
Imports System.Drawing
Imports System.Text
Imports System.Windows.Forms
Imports System.IO.Ports

Namespace DeveloperTerminal
	Public Partial Class PortSettings
		Inherits Form
		#Region "Initialization"
		Public Sub New()
			InitializeComponent()

			' Get a list of the COM port names
			Dim ports As String() = GetPorts()

			Dim i As Integer = 0
			For Each s As String In ports
				If s <> "" Then
                    listBoxPorts.Items.Insert(i, s)
                    i += 1
                End If
			Next s

			' Set first COM port as default
			GetCOM(0)

			' Initialize baudrates in combobox;
			Me.comboBoxBaud.Items.AddRange(New Object() { "75", "110", "134", "150", "300", "600", "1200", "1800", "2400", "4800", "7200", "9600", "14400", "19200", "38400", "57600", "115200", "128000"})

			' Set Handshaking selection
			'  We will only use these handshaking types
			comboBoxHandshaking.Items.Add("None")
			comboBoxHandshaking.Items.Add("RTS/CTS")
			comboBoxHandshaking.Items.Add("Xon/Xoff")

			' Set Parity types
			For Each s As String In System.Enum.GetNames(GetType(Parity))
				comboBoxParity.Items.Add(s)
			Next s

			' Set Databits
			' FT232R UART interface supports only 7 or 8 data bits           
			'comboBoxDataBits.Items.Add("5"); // not supported
			'comboBoxDataBits.Items.Add("6"); // not supported
			comboBoxDataBits.Items.Add("7")
			comboBoxDataBits.Items.Add("8")

			' Set Stopbits
			' FT232R UART interface supports only 1 or 2 stop bits 
			'comboBoxStopBits.Items.Add("None"); // not supported
			comboBoxStopBits.Items.Add("1")
			'comboBoxStopBits.Items.Add("1.5"); // not supported
			comboBoxStopBits.Items.Add("2")
		End Sub
		#End Region

		#Region "Get Port Name"
		' We don't use the GetPortNames method of the SerialPort class
		' because it doesn't work right the documentation says it reads
		' a register key and if that key ain't right then the results
		' ain't right. In my test case the key had an orphan port name in it,
		' so I reverted to the DeviceInfo class that I'd made earlier before
		' the SerialPort stuff came along.
		'
		' Oh, and DON'T LOOK AT the DeviceInfo class and after you do
		' you'll see why I said don't
		Private Shared Function GetPorts() As String()
			Dim strArray As String() = DevInfo.DeviceInfo.ParseFriendlyPorts()

			 Return strArray
		End Function

		' Select the port from the list box
		Private Sub listBoxPorts_SelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs) Handles listBoxPorts.SelectedIndexChanged
			GetCOM(listBoxPorts.SelectedIndex)
		End Sub

		' Set the selected port and display it in the label
		Private Sub GetCOM(ByVal index As Integer)
			Dim strArray As String() = DevInfo.DeviceInfo.ParsePorts()

			If strArray(index) <> "" Then
				SelectedPort_Renamed = strArray(index)
				labelPort.Text = "Selected Port = " & SelectedPort_Renamed
			End If
		End Sub
		#End Region

		#Region "Data Assessors"
		' Assessors for the selected port
		' FT232R UART interface supporta 
		'      7 or 8 data bits
		'      1 or 2 stop bits
		'      odd / even / mark / space / no parity.
		' So these will be the only options available

		#Region "Port Name"
		' Assessor for the selected port name
		Private SelectedPort_Renamed As String = ""
		Public Property selectedPort() As String
			Get
				Return SelectedPort_Renamed
			End Get
			Set(ByVal value As String)
				SelectedPort_Renamed = value
				labelPort.Text = "Selected Port = " & SelectedPort_Renamed
			End Set
		End Property
		#End Region

		#Region "Baudrate"
		Private SelectedBaudrate_Renamed As Integer
		Public Property selectedBaudrate() As Integer
			Get
				Return SelectedBaudrate_Renamed
			End Get
			Set(ByVal value As Integer)
				SelectedBaudrate_Renamed = value
				comboBoxBaud.Text = value.ToString()
			End Set
		End Property

		Private Sub comboBoxBaud_SelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs) Handles comboBoxBaud.SelectedIndexChanged
			selectedBaudrate = Convert.ToInt32(comboBoxBaud.Items(comboBoxBaud.SelectedIndex))
		End Sub
		#End Region

		#Region "Parity"
		Private SelectedParity_Renamed As Parity ' = Parity.None;
		Public Property selectedParity() As Parity
			Get
				Return SelectedParity_Renamed
			End Get
			Set(ByVal value As Parity)
				SelectedParity_Renamed = value
				comboBoxParity.Text = value.ToString()
			End Set
		End Property

		Private Sub comboBoxParity_SelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs) Handles comboBoxParity.SelectedIndexChanged
			Dim temp As String = comboBoxParity.Items(comboBoxParity.SelectedIndex).ToString()

			Select Case temp
				Case "Even"
					selectedParity = Parity.Even
				Case "Mark"
					selectedParity = Parity.Mark
				Case "None"
					selectedParity = Parity.None
				Case "Odd"
					selectedParity = Parity.Odd
				Case "Space"
					selectedParity = Parity.Space
				Case Else
					selectedParity = Parity.None
			End Select
		End Sub
		#End Region

		#Region "StobBits"
		Private SelectedStopBits_Renamed As StopBits = StopBits.One
		Public Property selectedStopBits() As StopBits
			Get
				Return SelectedStopBits_Renamed
			End Get
			Set(ByVal value As StopBits)
				SelectedStopBits_Renamed = value
				comboBoxStopBits.Text = value.ToString()
			End Set
		End Property

		Private Sub comboBoxStopBits_SelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs) Handles comboBoxStopBits.SelectedIndexChanged
			Dim temp As String = comboBoxStopBits.Items(comboBoxStopBits.SelectedIndex).ToString()

			Select Case temp
				Case "None"
					selectedStopBits = StopBits.None
				Case "1"
					selectedStopBits = StopBits.One
				'case "1.5": // not supported by FT232R
					'SelectedStopBits = StopBits.OnePointFive;
					'break;
				Case "2"
					selectedStopBits = StopBits.Two
				Case Else
					selectedStopBits = StopBits.One
			End Select
		End Sub
		#End Region

		#Region "DataBits"
		Private SelectedDataBits_Renamed As Integer = 8
		Public Property selectedDataBits() As Integer
			Get
				Return SelectedDataBits_Renamed
			End Get
			Set(ByVal value As Integer)
				SelectedDataBits_Renamed = value
				comboBoxDataBits.Text = value.ToString()
			End Set
		End Property

		Private Sub comboBoxDataBits_SelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs) Handles comboBoxDataBits.SelectedIndexChanged
			If comboBoxDataBits.SelectedIndex = 0 Then
			selectedDataBits = 7
			Else
				selectedDataBits = 8
			End If
		End Sub
		#End Region

		#Region "Handshaking"
		' We will only use None, Xon/Xoff, or Hardware (which is RTS/CTS)
		Private SelectedHandshaking_Renamed As Handshake = Handshake.None
		Public Property selectedHandshaking() As Handshake
			Get
				Return SelectedHandshaking_Renamed
			End Get
			Set(ByVal value As Handshake)
				SelectedHandshaking_Renamed = value
				comboBoxHandshaking.Text = value.ToString()
			End Set
		End Property

		Private Sub comboBoxHandshaking_SelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs) Handles comboBoxHandshaking.SelectedIndexChanged
			If comboBoxHandshaking.SelectedIndex = 0 Then
			selectedHandshaking = Handshake.None
			ElseIf comboBoxHandshaking.SelectedIndex = 1 Then
			selectedHandshaking = Handshake.RequestToSend
			ElseIf comboBoxHandshaking.SelectedIndex = 2 Then
			selectedHandshaking = Handshake.XOnXOff
			Else
				selectedHandshaking = Handshake.None
			End If
		End Sub
		#End Region

		#Region "Okay and Cancel buttons"
		Private Sub buttonOkay_Click(ByVal sender As Object, ByVal e As EventArgs) Handles buttonOkay.Click
			DialogResult = System.Windows.Forms.DialogResult.OK
		End Sub

		Private Sub buttonCancel_Click(ByVal sender As Object, ByVal e As EventArgs) Handles buttonCancel.Click
            DialogResult = System.Windows.Forms.DialogResult.Cancel
		End Sub
		#End Region
		#End Region
	End Class
End Namespace