namespace GenericHid
{
    partial class FormFlash
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.tabControlMain = new System.Windows.Forms.TabControl();
            this.tabPageMain = new System.Windows.Forms.TabPage();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.buttonReadSignature = new System.Windows.Forms.Button();
            this.textBoxSignature = new System.Windows.Forms.TextBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.checkBox12V = new System.Windows.Forms.CheckBox();
            this.tabPageProgram = new System.Windows.Forms.TabPage();
            this.groupBoxFlash = new System.Windows.Forms.GroupBox();
            this.buttonProgramRead = new System.Windows.Forms.Button();
            this.buttonProgramVerify = new System.Windows.Forms.Button();
            this.checkBoxProgramFastFlash = new System.Windows.Forms.CheckBox();
            this.buttonFlashProgram = new System.Windows.Forms.Button();
            this.buttonFlashHEXFile = new System.Windows.Forms.Button();
            this.textBoxFlashHEXFile = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBoxDevice = new System.Windows.Forms.GroupBox();
            this.checkBoxVerifyAfterProgram = new System.Windows.Forms.CheckBox();
            this.checkBoxEraseBeforeProgram = new System.Windows.Forms.CheckBox();
            this.tabPageFuses = new System.Windows.Forms.TabPage();
            this.textBoxFusesLOW = new System.Windows.Forms.TextBox();
            this.textBoxFusesHIGH = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.buttonFusesWrite = new System.Windows.Forms.Button();
            this.buttonFusesRead = new System.Windows.Forms.Button();
            this.tabPageLockBits = new System.Windows.Forms.TabPage();
            this.label4 = new System.Windows.Forms.Label();
            this.textBoxLockBitsLOW = new System.Windows.Forms.TextBox();
            this.buttonLockBitsWrite = new System.Windows.Forms.Button();
            this.buttonLockBitsRead = new System.Windows.Forms.Button();
            this.tabPageAdvanced = new System.Windows.Forms.TabPage();
            this.buttonAdvancedTone = new System.Windows.Forms.Button();
            this.tabPageI2CFlash = new System.Windows.Forms.TabPage();
            this.label6 = new System.Windows.Forms.Label();
            this.comboBoxI2CFlashType = new System.Windows.Forms.ComboBox();
            this.buttonI2Cread = new System.Windows.Forms.Button();
            this.checkBoxI2CfastFlash = new System.Windows.Forms.CheckBox();
            this.buttonI2Cprogram = new System.Windows.Forms.Button();
            this.buttonI2CHexFile = new System.Windows.Forms.Button();
            this.textBoxI2CFlashHEXFile = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.listBoxProgramMessages = new System.Windows.Forms.ListBox();
            this.tabControlMain.SuspendLayout();
            this.tabPageMain.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.tabPageProgram.SuspendLayout();
            this.groupBoxFlash.SuspendLayout();
            this.groupBoxDevice.SuspendLayout();
            this.tabPageFuses.SuspendLayout();
            this.tabPageLockBits.SuspendLayout();
            this.tabPageAdvanced.SuspendLayout();
            this.tabPageI2CFlash.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControlMain
            // 
            this.tabControlMain.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControlMain.Controls.Add(this.tabPageMain);
            this.tabControlMain.Controls.Add(this.tabPageProgram);
            this.tabControlMain.Controls.Add(this.tabPageFuses);
            this.tabControlMain.Controls.Add(this.tabPageLockBits);
            this.tabControlMain.Controls.Add(this.tabPageAdvanced);
            this.tabControlMain.Controls.Add(this.tabPageI2CFlash);
            this.tabControlMain.Location = new System.Drawing.Point(13, 13);
            this.tabControlMain.Name = "tabControlMain";
            this.tabControlMain.SelectedIndex = 0;
            this.tabControlMain.Size = new System.Drawing.Size(472, 317);
            this.tabControlMain.TabIndex = 0;
            // 
            // tabPageMain
            // 
            this.tabPageMain.Controls.Add(this.groupBox2);
            this.tabPageMain.Controls.Add(this.groupBox1);
            this.tabPageMain.Location = new System.Drawing.Point(4, 22);
            this.tabPageMain.Name = "tabPageMain";
            this.tabPageMain.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageMain.Size = new System.Drawing.Size(464, 291);
            this.tabPageMain.TabIndex = 0;
            this.tabPageMain.Text = "Main";
            this.tabPageMain.UseVisualStyleBackColor = true;
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox2.Controls.Add(this.buttonReadSignature);
            this.groupBox2.Controls.Add(this.textBoxSignature);
            this.groupBox2.Location = new System.Drawing.Point(7, 7);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(451, 101);
            this.groupBox2.TabIndex = 2;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Device and Signature Bytes";
            // 
            // buttonReadSignature
            // 
            this.buttonReadSignature.Location = new System.Drawing.Point(287, 43);
            this.buttonReadSignature.Name = "buttonReadSignature";
            this.buttonReadSignature.Size = new System.Drawing.Size(122, 23);
            this.buttonReadSignature.TabIndex = 1;
            this.buttonReadSignature.Text = "Read Signature";
            this.buttonReadSignature.UseVisualStyleBackColor = true;
            this.buttonReadSignature.Click += new System.EventHandler(this.buttonReadSignature_Click);
            // 
            // textBoxSignature
            // 
            this.textBoxSignature.BackColor = System.Drawing.SystemColors.Control;
            this.textBoxSignature.Cursor = System.Windows.Forms.Cursors.Default;
            this.textBoxSignature.Location = new System.Drawing.Point(6, 47);
            this.textBoxSignature.Name = "textBoxSignature";
            this.textBoxSignature.Size = new System.Drawing.Size(227, 20);
            this.textBoxSignature.TabIndex = 0;
            this.textBoxSignature.Text = "Signature not read";
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.checkBox12V);
            this.groupBox1.Location = new System.Drawing.Point(7, 114);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(451, 120);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Programming Mode and Target Settings";
            // 
            // checkBox12V
            // 
            this.checkBox12V.AutoSize = true;
            this.checkBox12V.Location = new System.Drawing.Point(6, 19);
            this.checkBox12V.Name = "checkBox12V";
            this.checkBox12V.Size = new System.Drawing.Size(146, 17);
            this.checkBox12V.TabIndex = 0;
            this.checkBox12V.Text = "RESET line is high active";
            this.checkBox12V.UseVisualStyleBackColor = true;
            // 
            // tabPageProgram
            // 
            this.tabPageProgram.Controls.Add(this.groupBoxFlash);
            this.tabPageProgram.Controls.Add(this.groupBoxDevice);
            this.tabPageProgram.Location = new System.Drawing.Point(4, 22);
            this.tabPageProgram.Name = "tabPageProgram";
            this.tabPageProgram.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageProgram.Size = new System.Drawing.Size(464, 291);
            this.tabPageProgram.TabIndex = 1;
            this.tabPageProgram.Text = "Program";
            this.tabPageProgram.UseVisualStyleBackColor = true;
            // 
            // groupBoxFlash
            // 
            this.groupBoxFlash.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBoxFlash.Controls.Add(this.buttonProgramRead);
            this.groupBoxFlash.Controls.Add(this.buttonProgramVerify);
            this.groupBoxFlash.Controls.Add(this.checkBoxProgramFastFlash);
            this.groupBoxFlash.Controls.Add(this.buttonFlashProgram);
            this.groupBoxFlash.Controls.Add(this.buttonFlashHEXFile);
            this.groupBoxFlash.Controls.Add(this.textBoxFlashHEXFile);
            this.groupBoxFlash.Controls.Add(this.label1);
            this.groupBoxFlash.Location = new System.Drawing.Point(7, 83);
            this.groupBoxFlash.Name = "groupBoxFlash";
            this.groupBoxFlash.Size = new System.Drawing.Size(451, 82);
            this.groupBoxFlash.TabIndex = 1;
            this.groupBoxFlash.TabStop = false;
            this.groupBoxFlash.Text = "Flash";
            // 
            // buttonProgramRead
            // 
            this.buttonProgramRead.Location = new System.Drawing.Point(330, 53);
            this.buttonProgramRead.Name = "buttonProgramRead";
            this.buttonProgramRead.Size = new System.Drawing.Size(115, 23);
            this.buttonProgramRead.TabIndex = 6;
            this.buttonProgramRead.Text = "Read";
            this.buttonProgramRead.UseVisualStyleBackColor = true;
            this.buttonProgramRead.Click += new System.EventHandler(this.buttonProgramRead_Click);
            // 
            // buttonProgramVerify
            // 
            this.buttonProgramVerify.Location = new System.Drawing.Point(209, 53);
            this.buttonProgramVerify.Name = "buttonProgramVerify";
            this.buttonProgramVerify.Size = new System.Drawing.Size(115, 23);
            this.buttonProgramVerify.TabIndex = 5;
            this.buttonProgramVerify.Text = "Verify";
            this.buttonProgramVerify.UseVisualStyleBackColor = true;
            this.buttonProgramVerify.Click += new System.EventHandler(this.buttonProgramVerify_Click);
            // 
            // checkBoxProgramFastFlash
            // 
            this.checkBoxProgramFastFlash.AutoSize = true;
            this.checkBoxProgramFastFlash.Checked = true;
            this.checkBoxProgramFastFlash.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxProgramFastFlash.Location = new System.Drawing.Point(129, 57);
            this.checkBoxProgramFastFlash.Name = "checkBoxProgramFastFlash";
            this.checkBoxProgramFastFlash.Size = new System.Drawing.Size(74, 17);
            this.checkBoxProgramFastFlash.TabIndex = 4;
            this.checkBoxProgramFastFlash.Text = "Fast Flash";
            this.checkBoxProgramFastFlash.UseVisualStyleBackColor = true;
            // 
            // buttonFlashProgram
            // 
            this.buttonFlashProgram.Location = new System.Drawing.Point(6, 53);
            this.buttonFlashProgram.Name = "buttonFlashProgram";
            this.buttonFlashProgram.Size = new System.Drawing.Size(115, 23);
            this.buttonFlashProgram.TabIndex = 3;
            this.buttonFlashProgram.Text = "Program";
            this.buttonFlashProgram.UseVisualStyleBackColor = true;
            this.buttonFlashProgram.Click += new System.EventHandler(this.buttonFlashProgram_Click);
            // 
            // buttonFlashHEXFile
            // 
            this.buttonFlashHEXFile.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonFlashHEXFile.Location = new System.Drawing.Point(417, 16);
            this.buttonFlashHEXFile.Name = "buttonFlashHEXFile";
            this.buttonFlashHEXFile.Size = new System.Drawing.Size(28, 20);
            this.buttonFlashHEXFile.TabIndex = 2;
            this.buttonFlashHEXFile.Text = "...";
            this.buttonFlashHEXFile.UseVisualStyleBackColor = true;
            this.buttonFlashHEXFile.Click += new System.EventHandler(this.buttonFlashHEXFile_Click);
            // 
            // textBoxFlashHEXFile
            // 
            this.textBoxFlashHEXFile.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxFlashHEXFile.Location = new System.Drawing.Point(88, 17);
            this.textBoxFlashHEXFile.Name = "textBoxFlashHEXFile";
            this.textBoxFlashHEXFile.Size = new System.Drawing.Size(323, 20);
            this.textBoxFlashHEXFile.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(7, 20);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(75, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Input HEX File";
            // 
            // groupBoxDevice
            // 
            this.groupBoxDevice.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBoxDevice.Controls.Add(this.checkBoxVerifyAfterProgram);
            this.groupBoxDevice.Controls.Add(this.checkBoxEraseBeforeProgram);
            this.groupBoxDevice.Location = new System.Drawing.Point(7, 7);
            this.groupBoxDevice.Name = "groupBoxDevice";
            this.groupBoxDevice.Size = new System.Drawing.Size(451, 69);
            this.groupBoxDevice.TabIndex = 0;
            this.groupBoxDevice.TabStop = false;
            this.groupBoxDevice.Text = "Device";
            // 
            // checkBoxVerifyAfterProgram
            // 
            this.checkBoxVerifyAfterProgram.AutoSize = true;
            this.checkBoxVerifyAfterProgram.Checked = true;
            this.checkBoxVerifyAfterProgram.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxVerifyAfterProgram.Location = new System.Drawing.Point(271, 46);
            this.checkBoxVerifyAfterProgram.Name = "checkBoxVerifyAfterProgram";
            this.checkBoxVerifyAfterProgram.Size = new System.Drawing.Size(174, 17);
            this.checkBoxVerifyAfterProgram.TabIndex = 1;
            this.checkBoxVerifyAfterProgram.Text = "Verify device after programming";
            this.checkBoxVerifyAfterProgram.UseVisualStyleBackColor = true;
            // 
            // checkBoxEraseBeforeProgram
            // 
            this.checkBoxEraseBeforeProgram.AutoSize = true;
            this.checkBoxEraseBeforeProgram.Checked = true;
            this.checkBoxEraseBeforeProgram.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxEraseBeforeProgram.Location = new System.Drawing.Point(7, 46);
            this.checkBoxEraseBeforeProgram.Name = "checkBoxEraseBeforeProgram";
            this.checkBoxEraseBeforeProgram.Size = new System.Drawing.Size(209, 17);
            this.checkBoxEraseBeforeProgram.TabIndex = 0;
            this.checkBoxEraseBeforeProgram.Text = "Erase device before flash programming";
            this.checkBoxEraseBeforeProgram.UseVisualStyleBackColor = true;
            // 
            // tabPageFuses
            // 
            this.tabPageFuses.Controls.Add(this.textBoxFusesLOW);
            this.tabPageFuses.Controls.Add(this.textBoxFusesHIGH);
            this.tabPageFuses.Controls.Add(this.label3);
            this.tabPageFuses.Controls.Add(this.label2);
            this.tabPageFuses.Controls.Add(this.buttonFusesWrite);
            this.tabPageFuses.Controls.Add(this.buttonFusesRead);
            this.tabPageFuses.Location = new System.Drawing.Point(4, 22);
            this.tabPageFuses.Name = "tabPageFuses";
            this.tabPageFuses.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageFuses.Size = new System.Drawing.Size(464, 291);
            this.tabPageFuses.TabIndex = 2;
            this.tabPageFuses.Text = "Fuses";
            this.tabPageFuses.UseVisualStyleBackColor = true;
            // 
            // textBoxFusesLOW
            // 
            this.textBoxFusesLOW.Location = new System.Drawing.Point(169, 118);
            this.textBoxFusesLOW.Name = "textBoxFusesLOW";
            this.textBoxFusesLOW.Size = new System.Drawing.Size(77, 20);
            this.textBoxFusesLOW.TabIndex = 5;
            // 
            // textBoxFusesHIGH
            // 
            this.textBoxFusesHIGH.Location = new System.Drawing.Point(169, 91);
            this.textBoxFusesHIGH.Name = "textBoxFusesHIGH";
            this.textBoxFusesHIGH.Size = new System.Drawing.Size(77, 20);
            this.textBoxFusesHIGH.TabIndex = 4;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(128, 121);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(32, 13);
            this.label3.TabIndex = 3;
            this.label3.Text = "LOW";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(128, 94);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(34, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "HIGH";
            // 
            // buttonFusesWrite
            // 
            this.buttonFusesWrite.Location = new System.Drawing.Point(302, 262);
            this.buttonFusesWrite.Name = "buttonFusesWrite";
            this.buttonFusesWrite.Size = new System.Drawing.Size(75, 23);
            this.buttonFusesWrite.TabIndex = 1;
            this.buttonFusesWrite.Text = "Program";
            this.buttonFusesWrite.UseVisualStyleBackColor = true;
            this.buttonFusesWrite.Click += new System.EventHandler(this.buttonFusesWrite_Click);
            // 
            // buttonFusesRead
            // 
            this.buttonFusesRead.Location = new System.Drawing.Point(383, 262);
            this.buttonFusesRead.Name = "buttonFusesRead";
            this.buttonFusesRead.Size = new System.Drawing.Size(75, 23);
            this.buttonFusesRead.TabIndex = 0;
            this.buttonFusesRead.Text = "Read";
            this.buttonFusesRead.UseVisualStyleBackColor = true;
            this.buttonFusesRead.Click += new System.EventHandler(this.buttonFusesRead_Click);
            // 
            // tabPageLockBits
            // 
            this.tabPageLockBits.Controls.Add(this.label4);
            this.tabPageLockBits.Controls.Add(this.textBoxLockBitsLOW);
            this.tabPageLockBits.Controls.Add(this.buttonLockBitsWrite);
            this.tabPageLockBits.Controls.Add(this.buttonLockBitsRead);
            this.tabPageLockBits.Location = new System.Drawing.Point(4, 22);
            this.tabPageLockBits.Name = "tabPageLockBits";
            this.tabPageLockBits.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageLockBits.Size = new System.Drawing.Size(464, 291);
            this.tabPageLockBits.TabIndex = 3;
            this.tabPageLockBits.Text = "LockBits";
            this.tabPageLockBits.UseVisualStyleBackColor = true;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(128, 121);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(32, 13);
            this.label4.TabIndex = 3;
            this.label4.Text = "LOW";
            // 
            // textBoxLockBitsLOW
            // 
            this.textBoxLockBitsLOW.Location = new System.Drawing.Point(169, 118);
            this.textBoxLockBitsLOW.Name = "textBoxLockBitsLOW";
            this.textBoxLockBitsLOW.Size = new System.Drawing.Size(77, 20);
            this.textBoxLockBitsLOW.TabIndex = 2;
            // 
            // buttonLockBitsWrite
            // 
            this.buttonLockBitsWrite.Location = new System.Drawing.Point(302, 262);
            this.buttonLockBitsWrite.Name = "buttonLockBitsWrite";
            this.buttonLockBitsWrite.Size = new System.Drawing.Size(75, 23);
            this.buttonLockBitsWrite.TabIndex = 1;
            this.buttonLockBitsWrite.Text = "Program";
            this.buttonLockBitsWrite.UseVisualStyleBackColor = true;
            this.buttonLockBitsWrite.Click += new System.EventHandler(this.buttonLockBitsWrite_Click);
            // 
            // buttonLockBitsRead
            // 
            this.buttonLockBitsRead.Location = new System.Drawing.Point(383, 262);
            this.buttonLockBitsRead.Name = "buttonLockBitsRead";
            this.buttonLockBitsRead.Size = new System.Drawing.Size(75, 23);
            this.buttonLockBitsRead.TabIndex = 0;
            this.buttonLockBitsRead.Text = "Read";
            this.buttonLockBitsRead.UseVisualStyleBackColor = true;
            this.buttonLockBitsRead.Click += new System.EventHandler(this.buttonLockBitsRead_Click);
            // 
            // tabPageAdvanced
            // 
            this.tabPageAdvanced.Controls.Add(this.buttonAdvancedTone);
            this.tabPageAdvanced.Location = new System.Drawing.Point(4, 22);
            this.tabPageAdvanced.Name = "tabPageAdvanced";
            this.tabPageAdvanced.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageAdvanced.Size = new System.Drawing.Size(464, 291);
            this.tabPageAdvanced.TabIndex = 4;
            this.tabPageAdvanced.Text = "Advanced";
            this.tabPageAdvanced.UseVisualStyleBackColor = true;
            // 
            // buttonAdvancedTone
            // 
            this.buttonAdvancedTone.Location = new System.Drawing.Point(27, 36);
            this.buttonAdvancedTone.Name = "buttonAdvancedTone";
            this.buttonAdvancedTone.Size = new System.Drawing.Size(112, 23);
            this.buttonAdvancedTone.TabIndex = 0;
            this.buttonAdvancedTone.Text = "Calibration Tone";
            this.buttonAdvancedTone.UseVisualStyleBackColor = true;
            this.buttonAdvancedTone.Click += new System.EventHandler(this.buttonAdvancedTone_Click);
            // 
            // tabPageI2CFlash
            // 
            this.tabPageI2CFlash.Controls.Add(this.label6);
            this.tabPageI2CFlash.Controls.Add(this.comboBoxI2CFlashType);
            this.tabPageI2CFlash.Controls.Add(this.buttonI2Cread);
            this.tabPageI2CFlash.Controls.Add(this.checkBoxI2CfastFlash);
            this.tabPageI2CFlash.Controls.Add(this.buttonI2Cprogram);
            this.tabPageI2CFlash.Controls.Add(this.buttonI2CHexFile);
            this.tabPageI2CFlash.Controls.Add(this.textBoxI2CFlashHEXFile);
            this.tabPageI2CFlash.Controls.Add(this.label5);
            this.tabPageI2CFlash.Location = new System.Drawing.Point(4, 22);
            this.tabPageI2CFlash.Name = "tabPageI2CFlash";
            this.tabPageI2CFlash.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageI2CFlash.Size = new System.Drawing.Size(464, 291);
            this.tabPageI2CFlash.TabIndex = 5;
            this.tabPageI2CFlash.Text = "I2C Flash";
            this.tabPageI2CFlash.UseVisualStyleBackColor = true;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(13, 9);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(59, 13);
            this.label6.TabIndex = 14;
            this.label6.Text = "Flash Type";
            // 
            // comboBoxI2CFlashType
            // 
            this.comboBoxI2CFlashType.FormattingEnabled = true;
            this.comboBoxI2CFlashType.Location = new System.Drawing.Point(94, 6);
            this.comboBoxI2CFlashType.Name = "comboBoxI2CFlashType";
            this.comboBoxI2CFlashType.Size = new System.Drawing.Size(224, 21);
            this.comboBoxI2CFlashType.TabIndex = 13;
            // 
            // buttonI2Cread
            // 
            this.buttonI2Cread.Location = new System.Drawing.Point(336, 82);
            this.buttonI2Cread.Name = "buttonI2Cread";
            this.buttonI2Cread.Size = new System.Drawing.Size(115, 23);
            this.buttonI2Cread.TabIndex = 12;
            this.buttonI2Cread.Text = "Read";
            this.buttonI2Cread.UseVisualStyleBackColor = true;
            this.buttonI2Cread.Click += new System.EventHandler(this.buttonI2Cread_Click);
            // 
            // checkBoxI2CfastFlash
            // 
            this.checkBoxI2CfastFlash.AutoSize = true;
            this.checkBoxI2CfastFlash.Checked = true;
            this.checkBoxI2CfastFlash.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxI2CfastFlash.Location = new System.Drawing.Point(135, 86);
            this.checkBoxI2CfastFlash.Name = "checkBoxI2CfastFlash";
            this.checkBoxI2CfastFlash.Size = new System.Drawing.Size(74, 17);
            this.checkBoxI2CfastFlash.TabIndex = 11;
            this.checkBoxI2CfastFlash.Text = "Fast Flash";
            this.checkBoxI2CfastFlash.UseVisualStyleBackColor = true;
            // 
            // buttonI2Cprogram
            // 
            this.buttonI2Cprogram.Location = new System.Drawing.Point(12, 82);
            this.buttonI2Cprogram.Name = "buttonI2Cprogram";
            this.buttonI2Cprogram.Size = new System.Drawing.Size(115, 23);
            this.buttonI2Cprogram.TabIndex = 10;
            this.buttonI2Cprogram.Text = "Program";
            this.buttonI2Cprogram.UseVisualStyleBackColor = true;
            this.buttonI2Cprogram.Click += new System.EventHandler(this.buttonI2Cprogram_Click);
            // 
            // buttonI2CHexFile
            // 
            this.buttonI2CHexFile.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonI2CHexFile.Location = new System.Drawing.Point(423, 45);
            this.buttonI2CHexFile.Name = "buttonI2CHexFile";
            this.buttonI2CHexFile.Size = new System.Drawing.Size(28, 20);
            this.buttonI2CHexFile.TabIndex = 9;
            this.buttonI2CHexFile.Text = "...";
            this.buttonI2CHexFile.UseVisualStyleBackColor = true;
            this.buttonI2CHexFile.Click += new System.EventHandler(this.buttonI2CHexFile_Click);
            // 
            // textBoxI2CFlashHEXFile
            // 
            this.textBoxI2CFlashHEXFile.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxI2CFlashHEXFile.Location = new System.Drawing.Point(94, 46);
            this.textBoxI2CFlashHEXFile.Name = "textBoxI2CFlashHEXFile";
            this.textBoxI2CFlashHEXFile.Size = new System.Drawing.Size(323, 20);
            this.textBoxI2CFlashHEXFile.TabIndex = 8;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(13, 49);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(75, 13);
            this.label5.TabIndex = 7;
            this.label5.Text = "Input HEX File";
            // 
            // listBoxProgramMessages
            // 
            this.listBoxProgramMessages.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.listBoxProgramMessages.BackColor = System.Drawing.SystemColors.Control;
            this.listBoxProgramMessages.Location = new System.Drawing.Point(13, 336);
            this.listBoxProgramMessages.Name = "listBoxProgramMessages";
            this.listBoxProgramMessages.SelectionMode = System.Windows.Forms.SelectionMode.None;
            this.listBoxProgramMessages.Size = new System.Drawing.Size(472, 147);
            this.listBoxProgramMessages.TabIndex = 2;
            // 
            // FormFlash
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(497, 489);
            this.Controls.Add(this.listBoxProgramMessages);
            this.Controls.Add(this.tabControlMain);
            this.Name = "FormFlash";
            this.Text = "FormFlash";
            this.tabControlMain.ResumeLayout(false);
            this.tabPageMain.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.tabPageProgram.ResumeLayout(false);
            this.groupBoxFlash.ResumeLayout(false);
            this.groupBoxFlash.PerformLayout();
            this.groupBoxDevice.ResumeLayout(false);
            this.groupBoxDevice.PerformLayout();
            this.tabPageFuses.ResumeLayout(false);
            this.tabPageFuses.PerformLayout();
            this.tabPageLockBits.ResumeLayout(false);
            this.tabPageLockBits.PerformLayout();
            this.tabPageAdvanced.ResumeLayout(false);
            this.tabPageI2CFlash.ResumeLayout(false);
            this.tabPageI2CFlash.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControlMain;
        private System.Windows.Forms.TabPage tabPageMain;
        private System.Windows.Forms.TabPage tabPageProgram;
        private System.Windows.Forms.GroupBox groupBoxDevice;
        private System.Windows.Forms.GroupBox groupBoxFlash;
        private System.Windows.Forms.Button buttonFlashHEXFile;
        private System.Windows.Forms.TextBox textBoxFlashHEXFile;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox checkBoxVerifyAfterProgram;
        private System.Windows.Forms.CheckBox checkBoxEraseBeforeProgram;
        private System.Windows.Forms.Button buttonFlashProgram;
        private System.Windows.Forms.ListBox listBoxProgramMessages;
        private System.Windows.Forms.CheckBox checkBox12V;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button buttonReadSignature;
        private System.Windows.Forms.TextBox textBoxSignature;
        private System.Windows.Forms.TabPage tabPageFuses;
        private System.Windows.Forms.TextBox textBoxFusesLOW;
        private System.Windows.Forms.TextBox textBoxFusesHIGH;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button buttonFusesWrite;
        private System.Windows.Forms.Button buttonFusesRead;
        private System.Windows.Forms.Button buttonProgramVerify;
        private System.Windows.Forms.CheckBox checkBoxProgramFastFlash;
        private System.Windows.Forms.TabPage tabPageLockBits;
        private System.Windows.Forms.TabPage tabPageAdvanced;
        private System.Windows.Forms.Button buttonProgramRead;
        private System.Windows.Forms.Button buttonLockBitsWrite;
        private System.Windows.Forms.Button buttonLockBitsRead;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox textBoxLockBitsLOW;
        private System.Windows.Forms.Button buttonAdvancedTone;
        private System.Windows.Forms.TabPage tabPageI2CFlash;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ComboBox comboBoxI2CFlashType;
        private System.Windows.Forms.Button buttonI2Cread;
        private System.Windows.Forms.CheckBox checkBoxI2CfastFlash;
        private System.Windows.Forms.Button buttonI2Cprogram;
        private System.Windows.Forms.Button buttonI2CHexFile;
        private System.Windows.Forms.TextBox textBoxI2CFlashHEXFile;
        private System.Windows.Forms.Label label5;

    }
}