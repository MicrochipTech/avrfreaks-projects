Imports Microsoft.VisualBasic
Imports System
Namespace LED
	Public Partial Class LED22
		''' <summary> 
		''' Required designer variable.
		''' </summary>
		Private components As System.ComponentModel.IContainer = Nothing

		''' <summary> 
		''' Clean up any resources being used.
		''' </summary>
		''' <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		Protected Overrides Sub Dispose(ByVal disposing As Boolean)
			If disposing AndAlso (Not components Is Nothing) Then
				components.Dispose()
			End If
			MyBase.Dispose(disposing)
		End Sub

		#Region "Component Designer generated code"

		''' <summary> 
		''' Required method for Designer support - do not modify 
		''' the contents of this method with the code editor.
		''' </summary>
		Private Sub InitializeComponent()
			Me.components = New System.ComponentModel.Container()
			Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(LED22))
			Me.imageList1 = New System.Windows.Forms.ImageList(Me.components)
			Me.SuspendLayout()
			' 
			' imageList1
			' 
			Me.imageList1.ImageStream = (CType(resources.GetObject("imageList1.ImageStream"), System.Windows.Forms.ImageListStreamer))
			Me.imageList1.TransparentColor = System.Drawing.Color.Transparent
			Me.imageList1.Images.SetKeyName(0, "Red LED ON.gif")
			Me.imageList1.Images.SetKeyName(1, "Red LED OFF.gif")
			Me.imageList1.Images.SetKeyName(2, "Orange LED ON.gif")
			Me.imageList1.Images.SetKeyName(3, "Orange LED OFF.gif")
			Me.imageList1.Images.SetKeyName(4, "Yellow LED ON.gif")
			Me.imageList1.Images.SetKeyName(5, "Yellow LED OFF.gif")
			Me.imageList1.Images.SetKeyName(6, "Green LED ON.gif")
			Me.imageList1.Images.SetKeyName(7, "Green LED OFF.gif")
			Me.imageList1.Images.SetKeyName(8, "Blue LED ON.gif")
			Me.imageList1.Images.SetKeyName(9, "Blue LED OFF.gif")
			' 
			' LED22
			' 
			Me.AutoScaleDimensions = New System.Drawing.SizeF(6F, 13F)
			Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
			Me.BackgroundImage = (CType(resources.GetObject("$this.BackgroundImage"), System.Drawing.Image))
			Me.Name = "LED22"
			Me.Size = New System.Drawing.Size(22, 22)
			Me.ResumeLayout(False)

		End Sub

		#End Region

		Private imageList1 As System.Windows.Forms.ImageList
	End Class
End Namespace
