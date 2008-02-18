Imports Microsoft.VisualBasic
Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Data
Imports System.Drawing
Imports System.Text
Imports System.Windows.Forms

Namespace DeveloperTerminal
	Public Partial Class ByteShooter
		Inherits Form
		#Region "initialization"
		Private Interval_Renamed As Integer = 100

		Public Sub New()
			InitializeComponent()
		End Sub

		Private Sub ByteShooter_Load(ByVal sender As Object, ByVal e As EventArgs) Handles MyBase.Load
			Dim c As Char
			' Load the comboBox with 0x00 to 0xFF
			For i As Integer = 0 To 15
				c = ChrW(i)
				Me.comboBoxSendByte.Items.Add("0x0" & i.ToString("X") & " -> " & c.ToString())
			Next i
			For i As Integer = 16 To 255
				c = ChrW(i)
				Me.comboBoxSendByte.Items.Add("0x" & i.ToString("X") & " -> " & c.ToString())
			Next i
		End Sub
		#End Region

		#Region "Data Assessors"
		Public ReadOnly Property interval() As Integer
			Get
				Return Interval_Renamed
			End Get
		End Property

		Private ByteToSend_Renamed As Byte = 0
		Public ReadOnly Property byteToSend() As Byte
			Get
				Return ByteToSend_Renamed
			End Get
		End Property
		#End Region

		#Region "Components"
		Private Sub textBox1_TextChanged(ByVal sender As Object, ByVal e As EventArgs) Handles textBox1.TextChanged
			Try
				Interval_Renamed = Convert.ToInt32(textBox1.Text)
				timer1.Interval = Interval_Renamed
			Catch
				timer1.Enabled = False
				MessageBox.Show("Error with interval")
			End Try
		End Sub

		Private Sub comboBoxSendByte_SelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs) Handles comboBoxSendByte.SelectedIndexChanged
			ByteToSend_Renamed = CByte(comboBoxSendByte.SelectedIndex)
		End Sub

		Private Sub buttonSend_Click(ByVal sender As Object, ByVal e As EventArgs) Handles buttonSend.Click
			Me.DialogResult = System.Windows.Forms.DialogResult.OK
		End Sub

		Private Sub buttonCancel_Click(ByVal sender As Object, ByVal e As EventArgs) Handles buttonCancel.Click
            Me.DialogResult = DialogResult.Cancel
		End Sub
		#End Region

	End Class
End Namespace