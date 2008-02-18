Imports Microsoft.VisualBasic
Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Data
Imports System.Drawing
Imports System.Text
Imports System.Windows.Forms

Namespace DeveloperTerminal
	Public Partial Class TextBoxSettings
		Inherits Form
		#Region "Initialization"
		Public Sub New()
			InitializeComponent()
		End Sub
		#End Region

		#Region "Assessors"
		Private BackColr As Color = Color.White
        Public ReadOnly Property backClr() As Color
            Get
                Return BackColr
            End Get
        End Property

		Private ForeColr As Color = Color.Black
        Public ReadOnly Property foreClr() As Color
            Get
                Return ForeColr
            End Get
        End Property

        Private Ft As Font = Nothing
        Public ReadOnly Property fnt() As Font
            Get
                Return Ft
            End Get
        End Property
		#End Region

		#Region "Components"
		Private Sub buttonFont_Click(ByVal sender As Object, ByVal e As EventArgs) Handles buttonFont.Click
			fontDialog1.ShowDialog()
            Ft = fontDialog1.Font
		End Sub

		Private Sub buttonForeColor_Click(ByVal sender As Object, ByVal e As EventArgs) Handles buttonForeColor.Click
			colorDialog1.ShowDialog()
			ForeColr = colorDialog1.Color
		End Sub

		Private Sub buttonBackColor_Click(ByVal sender As Object, ByVal e As EventArgs) Handles buttonBackColor.Click
			colorDialog1.ShowDialog()
			BackColr = colorDialog1.Color
		End Sub

		Private Sub buttonOkay_Click(ByVal sender As Object, ByVal e As EventArgs) Handles buttonOkay.Click
			DialogResult = System.Windows.Forms.DialogResult.OK
		End Sub

        Private Sub buttonCancel_Click(ByVal sender As Object, ByVal e As EventArgs) Handles buttonCancel.Click
            DialogResult = DialogResult.Cancel
        End Sub
#End Region
    End Class
End Namespace