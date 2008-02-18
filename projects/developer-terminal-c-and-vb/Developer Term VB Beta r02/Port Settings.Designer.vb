Imports Microsoft.VisualBasic
Imports System
Namespace DeveloperTerminal
	Public Partial Class PortSettings
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
			Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(PortSettings))
			Me.listBoxPorts = New System.Windows.Forms.ListBox()
			Me.label1 = New System.Windows.Forms.Label()
			Me.labelPort = New System.Windows.Forms.Label()
			Me.comboBoxBaud = New System.Windows.Forms.ComboBox()
			Me.labelBaud = New System.Windows.Forms.Label()
			Me.buttonOkay = New System.Windows.Forms.Button()
			Me.buttonCancel = New System.Windows.Forms.Button()
			Me.label2 = New System.Windows.Forms.Label()
			Me.label3 = New System.Windows.Forms.Label()
			Me.label4 = New System.Windows.Forms.Label()
			Me.comboBoxDataBits = New System.Windows.Forms.ComboBox()
			Me.label5 = New System.Windows.Forms.Label()
			Me.comboBoxHandshaking = New System.Windows.Forms.ComboBox()
			Me.comboBoxParity = New System.Windows.Forms.ComboBox()
			Me.comboBoxStopBits = New System.Windows.Forms.ComboBox()
			Me.SuspendLayout()
			' 
			' listBoxPorts
			' 
			Me.listBoxPorts.FormattingEnabled = True
			Me.listBoxPorts.Location = New System.Drawing.Point(6, 25)
			Me.listBoxPorts.Name = "listBoxPorts"
			Me.listBoxPorts.Size = New System.Drawing.Size(236, 82)
			Me.listBoxPorts.TabIndex = 0
'			Me.listBoxPorts.SelectedIndexChanged += New System.EventHandler(Me.listBoxPorts_SelectedIndexChanged);
			' 
			' label1
			' 
			Me.label1.AutoSize = True
			Me.label1.Location = New System.Drawing.Point(4, 7)
			Me.label1.Name = "label1"
			Me.label1.Size = New System.Drawing.Size(183, 13)
			Me.label1.TabIndex = 1
			Me.label1.Text = "Double click on port you want to use:"
			' 
			' labelPort
			' 
			Me.labelPort.AutoSize = True
			Me.labelPort.Location = New System.Drawing.Point(13, 126)
			Me.labelPort.Name = "labelPort"
			Me.labelPort.Size = New System.Drawing.Size(142, 13)
			Me.labelPort.TabIndex = 2
			Me.labelPort.Text = "Selected Port:  Not Selected"
			' 
			' comboBoxBaud
			' 
			Me.comboBoxBaud.FormattingEnabled = True
			Me.comboBoxBaud.Location = New System.Drawing.Point(102, 151)
			Me.comboBoxBaud.Name = "comboBoxBaud"
			Me.comboBoxBaud.Size = New System.Drawing.Size(121, 21)
			Me.comboBoxBaud.TabIndex = 3
'			Me.comboBoxBaud.SelectedIndexChanged += New System.EventHandler(Me.comboBoxBaud_SelectedIndexChanged);
			' 
			' labelBaud
			' 
			Me.labelBaud.AutoSize = True
			Me.labelBaud.Location = New System.Drawing.Point(46, 154)
			Me.labelBaud.Name = "labelBaud"
			Me.labelBaud.Size = New System.Drawing.Size(50, 13)
			Me.labelBaud.TabIndex = 4
			Me.labelBaud.Text = "Baudrate"
			' 
			' buttonOkay
			' 
			Me.buttonOkay.Location = New System.Drawing.Point(25, 283)
			Me.buttonOkay.Name = "buttonOkay"
			Me.buttonOkay.Size = New System.Drawing.Size(75, 23)
			Me.buttonOkay.TabIndex = 5
			Me.buttonOkay.Text = "Okay"
			Me.buttonOkay.UseVisualStyleBackColor = True
'			Me.buttonOkay.Click += New System.EventHandler(Me.buttonOkay_Click);
			' 
			' buttonCancel
			' 
			Me.buttonCancel.Location = New System.Drawing.Point(148, 283)
			Me.buttonCancel.Name = "buttonCancel"
			Me.buttonCancel.Size = New System.Drawing.Size(75, 23)
			Me.buttonCancel.TabIndex = 6
			Me.buttonCancel.Text = "Cancel"
			Me.buttonCancel.UseVisualStyleBackColor = True
'			Me.buttonCancel.Click += New System.EventHandler(Me.buttonCancel_Click);
			' 
			' label2
			' 
			Me.label2.AutoSize = True
			Me.label2.Location = New System.Drawing.Point(46, 176)
			Me.label2.Name = "label2"
			Me.label2.Size = New System.Drawing.Size(50, 13)
			Me.label2.TabIndex = 7
			Me.label2.Text = "Data Bits"
			' 
			' label3
			' 
			Me.label3.AutoSize = True
			Me.label3.Location = New System.Drawing.Point(63, 198)
			Me.label3.Name = "label3"
			Me.label3.Size = New System.Drawing.Size(33, 13)
			Me.label3.TabIndex = 8
			Me.label3.Text = "Parity"
			' 
			' label4
			' 
			Me.label4.AutoSize = True
			Me.label4.Location = New System.Drawing.Point(46, 220)
			Me.label4.Name = "label4"
			Me.label4.Size = New System.Drawing.Size(49, 13)
			Me.label4.TabIndex = 9
			Me.label4.Text = "Stop Bits"
			' 
			' comboBoxDataBits
			' 
			Me.comboBoxDataBits.FormattingEnabled = True
			Me.comboBoxDataBits.Location = New System.Drawing.Point(102, 173)
			Me.comboBoxDataBits.Name = "comboBoxDataBits"
			Me.comboBoxDataBits.Size = New System.Drawing.Size(121, 21)
			Me.comboBoxDataBits.TabIndex = 10
'			Me.comboBoxDataBits.SelectedIndexChanged += New System.EventHandler(Me.comboBoxDataBits_SelectedIndexChanged);
			' 
			' label5
			' 
			Me.label5.AutoSize = True
			Me.label5.Location = New System.Drawing.Point(25, 242)
			Me.label5.Name = "label5"
			Me.label5.Size = New System.Drawing.Size(70, 13)
			Me.label5.TabIndex = 11
			Me.label5.Text = "Handshaking"
			' 
			' comboBoxHandshaking
			' 
			Me.comboBoxHandshaking.FormattingEnabled = True
			Me.comboBoxHandshaking.Location = New System.Drawing.Point(102, 239)
			Me.comboBoxHandshaking.Name = "comboBoxHandshaking"
			Me.comboBoxHandshaking.Size = New System.Drawing.Size(121, 21)
			Me.comboBoxHandshaking.TabIndex = 12
'			Me.comboBoxHandshaking.SelectedIndexChanged += New System.EventHandler(Me.comboBoxHandshaking_SelectedIndexChanged);
			' 
			' comboBoxParity
			' 
			Me.comboBoxParity.FormattingEnabled = True
			Me.comboBoxParity.Location = New System.Drawing.Point(102, 195)
			Me.comboBoxParity.Name = "comboBoxParity"
			Me.comboBoxParity.Size = New System.Drawing.Size(121, 21)
			Me.comboBoxParity.TabIndex = 13
'			Me.comboBoxParity.SelectedIndexChanged += New System.EventHandler(Me.comboBoxParity_SelectedIndexChanged);
			' 
			' comboBoxStopBits
			' 
			Me.comboBoxStopBits.FormattingEnabled = True
			Me.comboBoxStopBits.Location = New System.Drawing.Point(102, 217)
			Me.comboBoxStopBits.Name = "comboBoxStopBits"
			Me.comboBoxStopBits.Size = New System.Drawing.Size(121, 21)
			Me.comboBoxStopBits.TabIndex = 14
'			Me.comboBoxStopBits.SelectedIndexChanged += New System.EventHandler(Me.comboBoxStopBits_SelectedIndexChanged);
			' 
			' PortSettings
			' 
			Me.AutoScaleDimensions = New System.Drawing.SizeF(6F, 13F)
			Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
			Me.BackColor = System.Drawing.Color.Bisque
			Me.ClientSize = New System.Drawing.Size(248, 315)
			Me.Controls.Add(Me.comboBoxStopBits)
			Me.Controls.Add(Me.comboBoxParity)
			Me.Controls.Add(Me.comboBoxHandshaking)
			Me.Controls.Add(Me.label5)
			Me.Controls.Add(Me.comboBoxDataBits)
			Me.Controls.Add(Me.label4)
			Me.Controls.Add(Me.label3)
			Me.Controls.Add(Me.label2)
			Me.Controls.Add(Me.buttonCancel)
			Me.Controls.Add(Me.buttonOkay)
			Me.Controls.Add(Me.labelBaud)
			Me.Controls.Add(Me.comboBoxBaud)
			Me.Controls.Add(Me.labelPort)
			Me.Controls.Add(Me.label1)
			Me.Controls.Add(Me.listBoxPorts)
			Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
			Me.Icon = (CType(resources.GetObject("$this.Icon"), System.Drawing.Icon))
			Me.Name = "PortSettings"
			Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
			Me.Text = "Settings"
			Me.ResumeLayout(False)
			Me.PerformLayout()

		End Sub

		#End Region

		Private WithEvents listBoxPorts As System.Windows.Forms.ListBox
		Private label1 As System.Windows.Forms.Label
		Private labelPort As System.Windows.Forms.Label
		Private WithEvents comboBoxBaud As System.Windows.Forms.ComboBox
		Private labelBaud As System.Windows.Forms.Label
		Private WithEvents buttonOkay As System.Windows.Forms.Button
		Private WithEvents buttonCancel As System.Windows.Forms.Button
		Private label2 As System.Windows.Forms.Label
		Private label3 As System.Windows.Forms.Label
		Private label4 As System.Windows.Forms.Label
		Private WithEvents comboBoxDataBits As System.Windows.Forms.ComboBox
		Private label5 As System.Windows.Forms.Label
		Private WithEvents comboBoxHandshaking As System.Windows.Forms.ComboBox
		Private WithEvents comboBoxParity As System.Windows.Forms.ComboBox
		Private WithEvents comboBoxStopBits As System.Windows.Forms.ComboBox
	End Class
End Namespace