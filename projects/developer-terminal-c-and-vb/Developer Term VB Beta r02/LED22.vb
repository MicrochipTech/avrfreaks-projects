Imports Microsoft.VisualBasic
Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Drawing
Imports System.Data
Imports System.Text
Imports System.Windows.Forms

Namespace LED
	Public Partial Class LED22
		Inherits UserControl
		Public Enum LEDColor
			RedOn
			RedOff
			YellowOn
			YellowOff
			OrangeOn
			OrangeOff
			GreenOn
			GreenOff
			BlueOn
			BlueOff
		End Enum

		Public Sub New()
			InitializeComponent()

			Me.BackgroundImage = imageList1.Images(CInt(Fix(LEDColor.RedOn)))

			setImage(CInt(Fix(LEDColor.RedOn)))
		End Sub

		Private Image_Renamed As Integer = CInt(Fix(LEDColor.RedOn))
		Public Property image() As Integer
			Get
				Return Image_Renamed
			End Get
			Set(ByVal value As Integer)
				Image_Renamed = value
				setImage(Image_Renamed)
			End Set
		End Property

		Private Sub setImage(ByVal num As Integer)
			If num <= CInt(Fix(LEDColor.BlueOff)) Then
				'MessageBox.Show("Set backgroung image to: " + num.ToString());
				Me.BackgroundImage = imageList1.Images(num)
			Else
				MessageBox.Show("Error: LED control, setImage - num > LEDColor.BlueOff.")
			End If
		End Sub

	End Class
End Namespace
