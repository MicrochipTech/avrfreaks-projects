Imports Microsoft.VisualBasic
Imports System
Namespace DeveloperTerminal
	Public Partial Class TextBoxSettings
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

		#Region "Windows Form Designer generated code"

		''' <summary>
		''' Required method for Designer support - do not modify
		''' the contents of this method with the code editor.
		''' </summary>
		Private Sub InitializeComponent()
			Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(TextBoxSettings))
			Me.colorDialog1 = New System.Windows.Forms.ColorDialog()
			Me.fontDialog1 = New System.Windows.Forms.FontDialog()
			Me.buttonBackColor = New System.Windows.Forms.Button()
			Me.buttonForeColor = New System.Windows.Forms.Button()
			Me.buttonFont = New System.Windows.Forms.Button()
			Me.buttonOkay = New System.Windows.Forms.Button()
			Me.buttonCancel = New System.Windows.Forms.Button()
			Me.SuspendLayout()
			' 
			' buttonBackColor
			' 
			Me.buttonBackColor.Location = New System.Drawing.Point(12, 40)
			Me.buttonBackColor.Name = "buttonBackColor"
			Me.buttonBackColor.Size = New System.Drawing.Size(172, 23)
			Me.buttonBackColor.TabIndex = 0
			Me.buttonBackColor.Text = "Back Color"
			Me.buttonBackColor.UseVisualStyleBackColor = True
'			Me.buttonBackColor.Click += New System.EventHandler(Me.buttonBackColor_Click);
			' 
			' buttonForeColor
			' 
			Me.buttonForeColor.Location = New System.Drawing.Point(12, 69)
			Me.buttonForeColor.Name = "buttonForeColor"
			Me.buttonForeColor.Size = New System.Drawing.Size(172, 23)
			Me.buttonForeColor.TabIndex = 1
			Me.buttonForeColor.Text = "Fore Color"
			Me.buttonForeColor.UseVisualStyleBackColor = True
'			Me.buttonForeColor.Click += New System.EventHandler(Me.buttonForeColor_Click);
			' 
			' buttonFont
			' 
			Me.buttonFont.Location = New System.Drawing.Point(12, 13)
			Me.buttonFont.Name = "buttonFont"
			Me.buttonFont.Size = New System.Drawing.Size(172, 23)
			Me.buttonFont.TabIndex = 2
			Me.buttonFont.Text = "Font"
			Me.buttonFont.UseVisualStyleBackColor = True
'			Me.buttonFont.Click += New System.EventHandler(Me.buttonFont_Click);
			' 
			' buttonOkay
			' 
			Me.buttonOkay.Location = New System.Drawing.Point(12, 117)
			Me.buttonOkay.Name = "buttonOkay"
			Me.buttonOkay.Size = New System.Drawing.Size(74, 23)
			Me.buttonOkay.TabIndex = 3
			Me.buttonOkay.Text = "Okay"
			Me.buttonOkay.UseVisualStyleBackColor = True
'			Me.buttonOkay.Click += New System.EventHandler(Me.buttonOkay_Click);
			' 
			' buttonCancel
			' 
			Me.buttonCancel.Location = New System.Drawing.Point(116, 116)
			Me.buttonCancel.Name = "buttonCancel"
			Me.buttonCancel.Size = New System.Drawing.Size(68, 23)
			Me.buttonCancel.TabIndex = 4
			Me.buttonCancel.Text = "Cancel"
			Me.buttonCancel.UseVisualStyleBackColor = True
'			Me.buttonCancel.Click += New System.EventHandler(Me.buttonCancel_Click);
			' 
			' TextBoxSettings
			' 
			Me.AutoScaleDimensions = New System.Drawing.SizeF(6F, 13F)
			Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
			Me.BackColor = System.Drawing.Color.Bisque
			Me.ClientSize = New System.Drawing.Size(196, 151)
			Me.Controls.Add(Me.buttonCancel)
			Me.Controls.Add(Me.buttonOkay)
			Me.Controls.Add(Me.buttonFont)
			Me.Controls.Add(Me.buttonForeColor)
			Me.Controls.Add(Me.buttonBackColor)
			Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
			Me.Icon = (CType(resources.GetObject("$this.Icon"), System.Drawing.Icon))
			Me.Name = "TextBoxSettings"
			Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
			Me.Text = "TextBoxSettings"
			Me.ResumeLayout(False)

		End Sub

		#End Region

		Private colorDialog1 As System.Windows.Forms.ColorDialog
		Private fontDialog1 As System.Windows.Forms.FontDialog
		Private WithEvents buttonBackColor As System.Windows.Forms.Button
		Private WithEvents buttonForeColor As System.Windows.Forms.Button
		Private WithEvents buttonFont As System.Windows.Forms.Button
		Private WithEvents buttonOkay As System.Windows.Forms.Button
		Private WithEvents buttonCancel As System.Windows.Forms.Button
	End Class
End Namespace