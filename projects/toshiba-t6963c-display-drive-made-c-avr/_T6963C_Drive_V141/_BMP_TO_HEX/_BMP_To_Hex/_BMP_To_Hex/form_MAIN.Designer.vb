<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class form_MAIN
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(form_MAIN))
        Me.MenuStrip = New System.Windows.Forms.MenuStrip()
        Me.FileToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.LoadImageToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.ToolStripMenuItem2 = New System.Windows.Forms.ToolStripMenuItem()
        Me.ToolStripMenuItem3 = New System.Windows.Forms.ToolStripMenuItem()
        Me.ToolStripMenuItem4 = New System.Windows.Forms.ToolStripMenuItem()
        Me.ToolStripMenuItem1 = New System.Windows.Forms.ToolStripSeparator()
        Me.ExitToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.ConvertToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.BitToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.BitToolStripMenuItem1 = New System.Windows.Forms.ToolStripMenuItem()
        Me.BitToolStripMenuItem2 = New System.Windows.Forms.ToolStripMenuItem()
        Me.GrayLevelToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.RGB332ToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.BitToolStripMenuItem3 = New System.Windows.Forms.ToolStripMenuItem()
        Me.GrayLevelToolStripMenuItem1 = New System.Windows.Forms.ToolStripMenuItem()
        Me.RGB565ToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.TypeToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.AssemblerToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.CCToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.ByToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.groupBox_IMAGE = New System.Windows.Forms.GroupBox()
        Me.pict_SAMPLE_IMAGE = New System.Windows.Forms.PictureBox()
        Me.text_HEX_OUTPUT = New System.Windows.Forms.TextBox()
        Me.buttom_LOAD_IMAGE = New System.Windows.Forms.Button()
        Me.buttom_GENERATE = New System.Windows.Forms.Button()
        Me.buttom_SAVE = New System.Windows.Forms.Button()
        Me.buttom_COPY_TO_CLIPBOARD = New System.Windows.Forms.Button()
        Me.groupBox_COMMANDS = New System.Windows.Forms.GroupBox()
        Me.pict_CPLUSPLUS = New System.Windows.Forms.PictureBox()
        Me.pict_ASM = New System.Windows.Forms.PictureBox()
        Me.ColorToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.NormalToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.InvertedToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.ProgressBar = New System.Windows.Forms.ProgressBar()
        Me.MenuStrip.SuspendLayout()
        Me.groupBox_IMAGE.SuspendLayout()
        CType(Me.pict_SAMPLE_IMAGE, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.groupBox_COMMANDS.SuspendLayout()
        CType(Me.pict_CPLUSPLUS, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.pict_ASM, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'MenuStrip
        '
        Me.MenuStrip.BackColor = System.Drawing.Color.FromArgb(CType(CType(128, Byte), Integer), CType(CType(128, Byte), Integer), CType(CType(255, Byte), Integer))
        Me.MenuStrip.Font = New System.Drawing.Font("Courier New", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.MenuStrip.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.FileToolStripMenuItem, Me.ConvertToolStripMenuItem, Me.TypeToolStripMenuItem, Me.ColorToolStripMenuItem, Me.ByToolStripMenuItem})
        Me.MenuStrip.Location = New System.Drawing.Point(0, 0)
        Me.MenuStrip.Name = "MenuStrip"
        Me.MenuStrip.Size = New System.Drawing.Size(984, 24)
        Me.MenuStrip.TabIndex = 0
        Me.MenuStrip.Text = "MenuStrip1"
        '
        'FileToolStripMenuItem
        '
        Me.FileToolStripMenuItem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.LoadImageToolStripMenuItem, Me.ToolStripMenuItem2, Me.ToolStripMenuItem3, Me.ToolStripMenuItem4, Me.ToolStripMenuItem1, Me.ExitToolStripMenuItem})
        Me.FileToolStripMenuItem.Name = "FileToolStripMenuItem"
        Me.FileToolStripMenuItem.Size = New System.Drawing.Size(52, 20)
        Me.FileToolStripMenuItem.Text = "&File"
        '
        'LoadImageToolStripMenuItem
        '
        Me.LoadImageToolStripMenuItem.Image = Global._BMP_To_Hex.My.Resources.Resources.Image_Editor
        Me.LoadImageToolStripMenuItem.Name = "LoadImageToolStripMenuItem"
        Me.LoadImageToolStripMenuItem.Size = New System.Drawing.Size(212, 22)
        Me.LoadImageToolStripMenuItem.Text = "&Load Image"
        '
        'ToolStripMenuItem2
        '
        Me.ToolStripMenuItem2.Image = Global._BMP_To_Hex.My.Resources.Resources.Truck___Wheel_Loader
        Me.ToolStripMenuItem2.Name = "ToolStripMenuItem2"
        Me.ToolStripMenuItem2.Size = New System.Drawing.Size(212, 22)
        Me.ToolStripMenuItem2.Text = "&Generate"
        '
        'ToolStripMenuItem3
        '
        Me.ToolStripMenuItem3.Image = Global._BMP_To_Hex.My.Resources.Resources.Clipboard__3_
        Me.ToolStripMenuItem3.Name = "ToolStripMenuItem3"
        Me.ToolStripMenuItem3.Size = New System.Drawing.Size(212, 22)
        Me.ToolStripMenuItem3.Text = "&Copy to Clipboard"
        '
        'ToolStripMenuItem4
        '
        Me.ToolStripMenuItem4.Image = Global._BMP_To_Hex.My.Resources.Resources.Floppy___Disk__23_
        Me.ToolStripMenuItem4.Name = "ToolStripMenuItem4"
        Me.ToolStripMenuItem4.Size = New System.Drawing.Size(212, 22)
        Me.ToolStripMenuItem4.Text = "&Save"
        '
        'ToolStripMenuItem1
        '
        Me.ToolStripMenuItem1.Name = "ToolStripMenuItem1"
        Me.ToolStripMenuItem1.Size = New System.Drawing.Size(209, 6)
        '
        'ExitToolStripMenuItem
        '
        Me.ExitToolStripMenuItem.Image = Global._BMP_To_Hex.My.Resources.Resources._Exit
        Me.ExitToolStripMenuItem.Name = "ExitToolStripMenuItem"
        Me.ExitToolStripMenuItem.Size = New System.Drawing.Size(212, 22)
        Me.ExitToolStripMenuItem.Text = "&Exit"
        '
        'ConvertToolStripMenuItem
        '
        Me.ConvertToolStripMenuItem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.BitToolStripMenuItem, Me.BitToolStripMenuItem1, Me.BitToolStripMenuItem2, Me.BitToolStripMenuItem3})
        Me.ConvertToolStripMenuItem.Name = "ConvertToolStripMenuItem"
        Me.ConvertToolStripMenuItem.Size = New System.Drawing.Size(52, 20)
        Me.ConvertToolStripMenuItem.Text = "&Bits"
        '
        'BitToolStripMenuItem
        '
        Me.BitToolStripMenuItem.Checked = True
        Me.BitToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked
        Me.BitToolStripMenuItem.Image = Global._BMP_To_Hex.My.Resources.Resources._2_Bit
        Me.BitToolStripMenuItem.Name = "BitToolStripMenuItem"
        Me.BitToolStripMenuItem.Size = New System.Drawing.Size(124, 22)
        Me.BitToolStripMenuItem.Text = "&2 Bit"
        '
        'BitToolStripMenuItem1
        '
        Me.BitToolStripMenuItem1.Enabled = False
        Me.BitToolStripMenuItem1.Image = Global._BMP_To_Hex.My.Resources.Resources._4_Bit
        Me.BitToolStripMenuItem1.Name = "BitToolStripMenuItem1"
        Me.BitToolStripMenuItem1.Size = New System.Drawing.Size(124, 22)
        Me.BitToolStripMenuItem1.Text = "&4 Bit"
        '
        'BitToolStripMenuItem2
        '
        Me.BitToolStripMenuItem2.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.GrayLevelToolStripMenuItem, Me.RGB332ToolStripMenuItem})
        Me.BitToolStripMenuItem2.Enabled = False
        Me.BitToolStripMenuItem2.Image = Global._BMP_To_Hex.My.Resources.Resources._8_Bit
        Me.BitToolStripMenuItem2.Name = "BitToolStripMenuItem2"
        Me.BitToolStripMenuItem2.Size = New System.Drawing.Size(124, 22)
        Me.BitToolStripMenuItem2.Text = "&8 Bit"
        '
        'GrayLevelToolStripMenuItem
        '
        Me.GrayLevelToolStripMenuItem.Image = Global._BMP_To_Hex.My.Resources.Resources.Gray
        Me.GrayLevelToolStripMenuItem.Name = "GrayLevelToolStripMenuItem"
        Me.GrayLevelToolStripMenuItem.Size = New System.Drawing.Size(156, 22)
        Me.GrayLevelToolStripMenuItem.Text = "&Gray Level"
        '
        'RGB332ToolStripMenuItem
        '
        Me.RGB332ToolStripMenuItem.Image = Global._BMP_To_Hex.My.Resources.Resources._8_Bit
        Me.RGB332ToolStripMenuItem.Name = "RGB332ToolStripMenuItem"
        Me.RGB332ToolStripMenuItem.Size = New System.Drawing.Size(156, 22)
        Me.RGB332ToolStripMenuItem.Text = "&RGB 332"
        '
        'BitToolStripMenuItem3
        '
        Me.BitToolStripMenuItem3.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.GrayLevelToolStripMenuItem1, Me.RGB565ToolStripMenuItem})
        Me.BitToolStripMenuItem3.Enabled = False
        Me.BitToolStripMenuItem3.Image = Global._BMP_To_Hex.My.Resources.Resources._16_Bit
        Me.BitToolStripMenuItem3.Name = "BitToolStripMenuItem3"
        Me.BitToolStripMenuItem3.Size = New System.Drawing.Size(124, 22)
        Me.BitToolStripMenuItem3.Text = "&16 Bit"
        '
        'GrayLevelToolStripMenuItem1
        '
        Me.GrayLevelToolStripMenuItem1.Image = Global._BMP_To_Hex.My.Resources.Resources.Gray
        Me.GrayLevelToolStripMenuItem1.Name = "GrayLevelToolStripMenuItem1"
        Me.GrayLevelToolStripMenuItem1.Size = New System.Drawing.Size(156, 22)
        Me.GrayLevelToolStripMenuItem1.Text = "&Gray Level"
        '
        'RGB565ToolStripMenuItem
        '
        Me.RGB565ToolStripMenuItem.Image = Global._BMP_To_Hex.My.Resources.Resources._16_Bit
        Me.RGB565ToolStripMenuItem.Name = "RGB565ToolStripMenuItem"
        Me.RGB565ToolStripMenuItem.Size = New System.Drawing.Size(156, 22)
        Me.RGB565ToolStripMenuItem.Text = "&RGB 565"
        '
        'TypeToolStripMenuItem
        '
        Me.TypeToolStripMenuItem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.AssemblerToolStripMenuItem, Me.CCToolStripMenuItem})
        Me.TypeToolStripMenuItem.Name = "TypeToolStripMenuItem"
        Me.TypeToolStripMenuItem.Size = New System.Drawing.Size(68, 20)
        Me.TypeToolStripMenuItem.Text = "&Output"
        '
        'AssemblerToolStripMenuItem
        '
        Me.AssemblerToolStripMenuItem.Checked = True
        Me.AssemblerToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked
        Me.AssemblerToolStripMenuItem.Image = CType(resources.GetObject("AssemblerToolStripMenuItem.Image"), System.Drawing.Image)
        Me.AssemblerToolStripMenuItem.Name = "AssemblerToolStripMenuItem"
        Me.AssemblerToolStripMenuItem.Size = New System.Drawing.Size(152, 22)
        Me.AssemblerToolStripMenuItem.Text = "&Assembler"
        '
        'CCToolStripMenuItem
        '
        Me.CCToolStripMenuItem.Image = Global._BMP_To_Hex.My.Resources.Resources.CPlusPlus
        Me.CCToolStripMenuItem.Name = "CCToolStripMenuItem"
        Me.CCToolStripMenuItem.Size = New System.Drawing.Size(152, 22)
        Me.CCToolStripMenuItem.Text = "&C,C++"
        '
        'ByToolStripMenuItem
        '
        Me.ByToolStripMenuItem.Name = "ByToolStripMenuItem"
        Me.ByToolStripMenuItem.Size = New System.Drawing.Size(36, 20)
        Me.ByToolStripMenuItem.Text = "&By"
        '
        'groupBox_IMAGE
        '
        Me.groupBox_IMAGE.Controls.Add(Me.pict_SAMPLE_IMAGE)
        Me.groupBox_IMAGE.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.groupBox_IMAGE.Location = New System.Drawing.Point(12, 27)
        Me.groupBox_IMAGE.Name = "groupBox_IMAGE"
        Me.groupBox_IMAGE.Size = New System.Drawing.Size(192, 192)
        Me.groupBox_IMAGE.TabIndex = 1
        Me.groupBox_IMAGE.TabStop = False
        Me.groupBox_IMAGE.Text = "Sample Image"
        '
        'pict_SAMPLE_IMAGE
        '
        Me.pict_SAMPLE_IMAGE.Location = New System.Drawing.Point(6, 19)
        Me.pict_SAMPLE_IMAGE.Name = "pict_SAMPLE_IMAGE"
        Me.pict_SAMPLE_IMAGE.Size = New System.Drawing.Size(180, 167)
        Me.pict_SAMPLE_IMAGE.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage
        Me.pict_SAMPLE_IMAGE.TabIndex = 2
        Me.pict_SAMPLE_IMAGE.TabStop = False
        '
        'text_HEX_OUTPUT
        '
        Me.text_HEX_OUTPUT.Font = New System.Drawing.Font("Courier New", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.text_HEX_OUTPUT.Location = New System.Drawing.Point(210, 33)
        Me.text_HEX_OUTPUT.Multiline = True
        Me.text_HEX_OUTPUT.Name = "text_HEX_OUTPUT"
        Me.text_HEX_OUTPUT.ScrollBars = System.Windows.Forms.ScrollBars.Both
        Me.text_HEX_OUTPUT.Size = New System.Drawing.Size(762, 416)
        Me.text_HEX_OUTPUT.TabIndex = 2
        Me.text_HEX_OUTPUT.WordWrap = False
        '
        'buttom_LOAD_IMAGE
        '
        Me.buttom_LOAD_IMAGE.BackColor = System.Drawing.Color.FromArgb(CType(CType(0, Byte), Integer), CType(CType(0, Byte), Integer), CType(CType(64, Byte), Integer))
        Me.buttom_LOAD_IMAGE.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.buttom_LOAD_IMAGE.ForeColor = System.Drawing.Color.White
        Me.buttom_LOAD_IMAGE.Location = New System.Drawing.Point(6, 19)
        Me.buttom_LOAD_IMAGE.Name = "buttom_LOAD_IMAGE"
        Me.buttom_LOAD_IMAGE.Size = New System.Drawing.Size(180, 23)
        Me.buttom_LOAD_IMAGE.TabIndex = 3
        Me.buttom_LOAD_IMAGE.Text = "&Load Image"
        Me.buttom_LOAD_IMAGE.UseVisualStyleBackColor = False
        '
        'buttom_GENERATE
        '
        Me.buttom_GENERATE.BackColor = System.Drawing.Color.Navy
        Me.buttom_GENERATE.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.buttom_GENERATE.ForeColor = System.Drawing.Color.White
        Me.buttom_GENERATE.Location = New System.Drawing.Point(6, 48)
        Me.buttom_GENERATE.Name = "buttom_GENERATE"
        Me.buttom_GENERATE.Size = New System.Drawing.Size(180, 23)
        Me.buttom_GENERATE.TabIndex = 4
        Me.buttom_GENERATE.Text = "&Generate"
        Me.buttom_GENERATE.UseVisualStyleBackColor = False
        '
        'buttom_SAVE
        '
        Me.buttom_SAVE.BackColor = System.Drawing.Color.Blue
        Me.buttom_SAVE.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.buttom_SAVE.ForeColor = System.Drawing.Color.White
        Me.buttom_SAVE.Location = New System.Drawing.Point(6, 106)
        Me.buttom_SAVE.Name = "buttom_SAVE"
        Me.buttom_SAVE.Size = New System.Drawing.Size(180, 23)
        Me.buttom_SAVE.TabIndex = 5
        Me.buttom_SAVE.Text = "&Save"
        Me.buttom_SAVE.UseVisualStyleBackColor = False
        '
        'buttom_COPY_TO_CLIPBOARD
        '
        Me.buttom_COPY_TO_CLIPBOARD.BackColor = System.Drawing.Color.FromArgb(CType(CType(0, Byte), Integer), CType(CType(0, Byte), Integer), CType(CType(192, Byte), Integer))
        Me.buttom_COPY_TO_CLIPBOARD.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.buttom_COPY_TO_CLIPBOARD.ForeColor = System.Drawing.Color.White
        Me.buttom_COPY_TO_CLIPBOARD.Location = New System.Drawing.Point(6, 77)
        Me.buttom_COPY_TO_CLIPBOARD.Name = "buttom_COPY_TO_CLIPBOARD"
        Me.buttom_COPY_TO_CLIPBOARD.Size = New System.Drawing.Size(180, 23)
        Me.buttom_COPY_TO_CLIPBOARD.TabIndex = 6
        Me.buttom_COPY_TO_CLIPBOARD.Text = "&Copy to Clipboard"
        Me.buttom_COPY_TO_CLIPBOARD.UseVisualStyleBackColor = False
        '
        'groupBox_COMMANDS
        '
        Me.groupBox_COMMANDS.Controls.Add(Me.ProgressBar)
        Me.groupBox_COMMANDS.Controls.Add(Me.pict_CPLUSPLUS)
        Me.groupBox_COMMANDS.Controls.Add(Me.pict_ASM)
        Me.groupBox_COMMANDS.Controls.Add(Me.buttom_LOAD_IMAGE)
        Me.groupBox_COMMANDS.Controls.Add(Me.buttom_SAVE)
        Me.groupBox_COMMANDS.Controls.Add(Me.buttom_COPY_TO_CLIPBOARD)
        Me.groupBox_COMMANDS.Controls.Add(Me.buttom_GENERATE)
        Me.groupBox_COMMANDS.Font = New System.Drawing.Font("Microsoft Sans Serif", 8.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.groupBox_COMMANDS.Location = New System.Drawing.Point(12, 225)
        Me.groupBox_COMMANDS.Name = "groupBox_COMMANDS"
        Me.groupBox_COMMANDS.Size = New System.Drawing.Size(192, 224)
        Me.groupBox_COMMANDS.TabIndex = 7
        Me.groupBox_COMMANDS.TabStop = False
        Me.groupBox_COMMANDS.Text = "Commands"
        '
        'pict_CPLUSPLUS
        '
        Me.pict_CPLUSPLUS.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.pict_CPLUSPLUS.Image = CType(resources.GetObject("pict_CPLUSPLUS.Image"), System.Drawing.Image)
        Me.pict_CPLUSPLUS.Location = New System.Drawing.Point(106, 135)
        Me.pict_CPLUSPLUS.Name = "pict_CPLUSPLUS"
        Me.pict_CPLUSPLUS.Size = New System.Drawing.Size(80, 48)
        Me.pict_CPLUSPLUS.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage
        Me.pict_CPLUSPLUS.TabIndex = 9
        Me.pict_CPLUSPLUS.TabStop = False
        Me.pict_CPLUSPLUS.Visible = False
        '
        'pict_ASM
        '
        Me.pict_ASM.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.pict_ASM.Image = CType(resources.GetObject("pict_ASM.Image"), System.Drawing.Image)
        Me.pict_ASM.Location = New System.Drawing.Point(55, 135)
        Me.pict_ASM.Name = "pict_ASM"
        Me.pict_ASM.Size = New System.Drawing.Size(80, 48)
        Me.pict_ASM.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage
        Me.pict_ASM.TabIndex = 8
        Me.pict_ASM.TabStop = False
        '
        'ColorToolStripMenuItem
        '
        Me.ColorToolStripMenuItem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.NormalToolStripMenuItem, Me.InvertedToolStripMenuItem})
        Me.ColorToolStripMenuItem.Name = "ColorToolStripMenuItem"
        Me.ColorToolStripMenuItem.Size = New System.Drawing.Size(60, 20)
        Me.ColorToolStripMenuItem.Text = "&Color"
        '
        'NormalToolStripMenuItem
        '
        Me.NormalToolStripMenuItem.Checked = True
        Me.NormalToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked
        Me.NormalToolStripMenuItem.Name = "NormalToolStripMenuItem"
        Me.NormalToolStripMenuItem.Size = New System.Drawing.Size(152, 22)
        Me.NormalToolStripMenuItem.Text = "&Normal"
        '
        'InvertedToolStripMenuItem
        '
        Me.InvertedToolStripMenuItem.Name = "InvertedToolStripMenuItem"
        Me.InvertedToolStripMenuItem.Size = New System.Drawing.Size(152, 22)
        Me.InvertedToolStripMenuItem.Text = "&Inverted"
        '
        'ProgressBar
        '
        Me.ProgressBar.BackColor = System.Drawing.Color.FromArgb(CType(CType(0, Byte), Integer), CType(CType(0, Byte), Integer), CType(CType(64, Byte), Integer))
        Me.ProgressBar.Location = New System.Drawing.Point(6, 189)
        Me.ProgressBar.Name = "ProgressBar"
        Me.ProgressBar.Size = New System.Drawing.Size(180, 23)
        Me.ProgressBar.TabIndex = 8
        '
        'form_MAIN
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.BackColor = System.Drawing.Color.FromArgb(CType(CType(192, Byte), Integer), CType(CType(192, Byte), Integer), CType(CType(255, Byte), Integer))
        Me.ClientSize = New System.Drawing.Size(984, 461)
        Me.Controls.Add(Me.groupBox_COMMANDS)
        Me.Controls.Add(Me.text_HEX_OUTPUT)
        Me.Controls.Add(Me.groupBox_IMAGE)
        Me.Controls.Add(Me.MenuStrip)
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.MainMenuStrip = Me.MenuStrip
        Me.Name = "form_MAIN"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "BMP To Hex Version "
        Me.MenuStrip.ResumeLayout(False)
        Me.MenuStrip.PerformLayout()
        Me.groupBox_IMAGE.ResumeLayout(False)
        CType(Me.pict_SAMPLE_IMAGE, System.ComponentModel.ISupportInitialize).EndInit()
        Me.groupBox_COMMANDS.ResumeLayout(False)
        CType(Me.pict_CPLUSPLUS, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.pict_ASM, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents MenuStrip As System.Windows.Forms.MenuStrip
    Friend WithEvents FileToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents LoadImageToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ToolStripMenuItem1 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents ExitToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ConvertToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents BitToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents BitToolStripMenuItem1 As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents BitToolStripMenuItem2 As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents GrayLevelToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents RGB332ToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents BitToolStripMenuItem3 As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents GrayLevelToolStripMenuItem1 As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents RGB565ToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents groupBox_IMAGE As System.Windows.Forms.GroupBox
    Friend WithEvents pict_SAMPLE_IMAGE As System.Windows.Forms.PictureBox
    Friend WithEvents text_HEX_OUTPUT As System.Windows.Forms.TextBox
    Friend WithEvents buttom_LOAD_IMAGE As System.Windows.Forms.Button
    Friend WithEvents buttom_GENERATE As System.Windows.Forms.Button
    Friend WithEvents TypeToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents AssemblerToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents CCToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents buttom_SAVE As System.Windows.Forms.Button
    Friend WithEvents ByToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents buttom_COPY_TO_CLIPBOARD As System.Windows.Forms.Button
    Friend WithEvents groupBox_COMMANDS As System.Windows.Forms.GroupBox
    Friend WithEvents ToolStripMenuItem2 As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ToolStripMenuItem3 As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ToolStripMenuItem4 As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents pict_ASM As System.Windows.Forms.PictureBox
    Friend WithEvents pict_CPLUSPLUS As System.Windows.Forms.PictureBox
    Friend WithEvents ColorToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents NormalToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents InvertedToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ProgressBar As System.Windows.Forms.ProgressBar

End Class
