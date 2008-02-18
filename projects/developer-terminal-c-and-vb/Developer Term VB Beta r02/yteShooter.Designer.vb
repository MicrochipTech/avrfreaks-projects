Imports Microsoft.VisualBasic
Imports System
Namespace DeveloperTerminal
	Public Partial Class ByteShooter
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
			Me.components = New System.ComponentModel.Container()
			Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(ByteShooter))
			Me.comboBoxSendByte = New System.Windows.Forms.ComboBox()
			Me.label1 = New System.Windows.Forms.Label()
			Me.label2 = New System.Windows.Forms.Label()
			Me.textBox1 = New System.Windows.Forms.TextBox()
			Me.timer1 = New System.Windows.Forms.Timer(Me.components)
			Me.buttonSend = New System.Windows.Forms.Button()
			Me.buttonCancel = New System.Windows.Forms.Button()
			Me.SuspendLayout()
			' 
			' comboBoxSendByte
			' 
			Me.comboBoxSendByte.BackColor = System.Drawing.SystemColors.Window
			Me.comboBoxSendByte.Font = New System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (CByte(0)))
			Me.comboBoxSendByte.Location = New System.Drawing.Point(123, 8)
			Me.comboBoxSendByte.Name = "comboBoxSendByte"
			Me.comboBoxSendByte.Size = New System.Drawing.Size(123, 23)
			Me.comboBoxSendByte.TabIndex = 98
			Me.comboBoxSendByte.Text = "0x00"
'			Me.comboBoxSendByte.SelectedIndexChanged += New System.EventHandler(Me.comboBoxSendByte_SelectedIndexChanged);
			' 
			' label1
			' 
			Me.label1.AutoSize = True
			Me.label1.Location = New System.Drawing.Point(13, 13)
			Me.label1.Name = "label1"
			Me.label1.Size = New System.Drawing.Size(104, 13)
			Me.label1.TabIndex = 99
			Me.label1.Text = "Select byte  to send:"
			' 
			' label2
			' 
			Me.label2.AutoSize = True
			Me.label2.Location = New System.Drawing.Point(13, 47)
			Me.label2.Name = "label2"
			Me.label2.Size = New System.Drawing.Size(86, 13)
			Me.label2.TabIndex = 100
			Me.label2.Text = "Enter time in mS:"
			' 
			' textBox1
			' 
			Me.textBox1.Location = New System.Drawing.Point(123, 47)
			Me.textBox1.Name = "textBox1"
			Me.textBox1.Size = New System.Drawing.Size(123, 20)
			Me.textBox1.TabIndex = 101
'			Me.textBox1.TextChanged += New System.EventHandler(Me.textBox1_TextChanged);
			' 
			' timer1
			' 
			Me.timer1.Interval = 500
			' 
			' buttonSend
			' 
			Me.buttonSend.Location = New System.Drawing.Point(31, 90)
			Me.buttonSend.Name = "buttonSend"
			Me.buttonSend.Size = New System.Drawing.Size(75, 23)
			Me.buttonSend.TabIndex = 102
			Me.buttonSend.Text = "Send"
			Me.buttonSend.UseVisualStyleBackColor = True
'			Me.buttonSend.Click += New System.EventHandler(Me.buttonSend_Click);
			' 
			' buttonCancel
			' 
			Me.buttonCancel.Location = New System.Drawing.Point(148, 89)
			Me.buttonCancel.Name = "buttonCancel"
			Me.buttonCancel.Size = New System.Drawing.Size(75, 23)
			Me.buttonCancel.TabIndex = 103
			Me.buttonCancel.Text = "Cancel"
			Me.buttonCancel.UseVisualStyleBackColor = True
'			Me.buttonCancel.Click += New System.EventHandler(Me.buttonCancel_Click);
			' 
			' ByteShooter
			' 
			Me.AutoScaleDimensions = New System.Drawing.SizeF(6F, 13F)
			Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
			Me.BackColor = System.Drawing.Color.Bisque
			Me.ClientSize = New System.Drawing.Size(254, 125)
			Me.Controls.Add(Me.buttonCancel)
			Me.Controls.Add(Me.buttonSend)
			Me.Controls.Add(Me.textBox1)
			Me.Controls.Add(Me.label2)
			Me.Controls.Add(Me.label1)
			Me.Controls.Add(Me.comboBoxSendByte)
			Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
			Me.Icon = (CType(resources.GetObject("$this.Icon"), System.Drawing.Icon))
			Me.Name = "ByteShooter"
			Me.Text = "ByteShooter"
'			Me.Load += New System.EventHandler(Me.ByteShooter_Load);
			Me.ResumeLayout(False)
			Me.PerformLayout()

		End Sub

		#End Region

		Private WithEvents comboBoxSendByte As System.Windows.Forms.ComboBox
		Private label1 As System.Windows.Forms.Label
		Private label2 As System.Windows.Forms.Label
		Private WithEvents textBox1 As System.Windows.Forms.TextBox
		Private timer1 As System.Windows.Forms.Timer
		Private WithEvents buttonSend As System.Windows.Forms.Button
		Private WithEvents buttonCancel As System.Windows.Forms.Button
	End Class
End Namespace