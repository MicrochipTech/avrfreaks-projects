namespace EncryptFirmware
{
    partial class Form1
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
            this.SaveNewFirmwareFile = new System.Windows.Forms.Button();
            this.Label1 = new System.Windows.Forms.Label();
            this.NewFirmwareVersion = new System.Windows.Forms.TextBox();
            this.SelectNewFirmwareButton = new System.Windows.Forms.Button();
            this.NewFirmwareFile = new System.Windows.Forms.TextBox();
            this.OpenFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.AllowedFirmwareVersions = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.SelectOldFirmwareFileButton = new System.Windows.Forms.Button();
            this.OldFirmwareFile = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.AESKey = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.AESIV = new System.Windows.Forms.TextBox();
            this.GenerateAESIV = new System.Windows.Forms.Button();
            this.GenerateAESKey = new System.Windows.Forms.Button();
            this.label8 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.DeviceId = new System.Windows.Forms.TextBox();
            this.label12 = new System.Windows.Forms.Label();
            this.DeviceCode = new System.Windows.Forms.TextBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.DevicePageSize = new System.Windows.Forms.ComboBox();
            this.label13 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.label14 = new System.Windows.Forms.Label();
            this.ReadOldFirmwareFileButton = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // SaveNewFirmwareFile
            // 
            this.SaveNewFirmwareFile.Location = new System.Drawing.Point(192, 487);
            this.SaveNewFirmwareFile.Name = "SaveNewFirmwareFile";
            this.SaveNewFirmwareFile.Size = new System.Drawing.Size(105, 23);
            this.SaveNewFirmwareFile.TabIndex = 9;
            this.SaveNewFirmwareFile.Text = "Save .bin";
            this.SaveNewFirmwareFile.UseVisualStyleBackColor = true;
            this.SaveNewFirmwareFile.Click += new System.EventHandler(this.SaveNewFirmwareFile_Click);
            // 
            // Label1
            // 
            this.Label1.AutoSize = true;
            this.Label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Label1.Location = new System.Drawing.Point(8, 67);
            this.Label1.Name = "Label1";
            this.Label1.Size = new System.Drawing.Size(207, 13);
            this.Label1.TabIndex = 8;
            this.Label1.Text = "New firmware version (max 4 bytes)";
            // 
            // NewFirmwareVersion
            // 
            this.NewFirmwareVersion.Location = new System.Drawing.Point(221, 65);
            this.NewFirmwareVersion.Name = "NewFirmwareVersion";
            this.NewFirmwareVersion.Size = new System.Drawing.Size(133, 20);
            this.NewFirmwareVersion.TabIndex = 7;
            this.NewFirmwareVersion.Text = "17,01,23";
            // 
            // SelectNewFirmwareButton
            // 
            this.SelectNewFirmwareButton.Location = new System.Drawing.Point(409, 34);
            this.SelectNewFirmwareButton.Name = "SelectNewFirmwareButton";
            this.SelectNewFirmwareButton.Size = new System.Drawing.Size(86, 23);
            this.SelectNewFirmwareButton.TabIndex = 6;
            this.SelectNewFirmwareButton.Text = "SELECT FILE";
            this.SelectNewFirmwareButton.UseVisualStyleBackColor = true;
            this.SelectNewFirmwareButton.Click += new System.EventHandler(this.SelectNewFirmwareButton_Click);
            // 
            // NewFirmwareFile
            // 
            this.NewFirmwareFile.Location = new System.Drawing.Point(11, 35);
            this.NewFirmwareFile.Name = "NewFirmwareFile";
            this.NewFirmwareFile.Size = new System.Drawing.Size(392, 20);
            this.NewFirmwareFile.TabIndex = 5;
            this.NewFirmwareFile.TextChanged += new System.EventHandler(this.NewFirmware_TextChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(8, 19);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(237, 13);
            this.label2.TabIndex = 10;
            this.label2.Text = "New firmware file to be encrypted (.hex):";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(8, 111);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(452, 13);
            this.label3.TabIndex = 12;
            this.label3.Text = "Previous firmware versions that are allowed to be upgraded (max 100 x 4 bytes, se" +
    "parated by ;)";
            // 
            // AllowedFirmwareVersions
            // 
            this.AllowedFirmwareVersions.Location = new System.Drawing.Point(11, 127);
            this.AllowedFirmwareVersions.Name = "AllowedFirmwareVersions";
            this.AllowedFirmwareVersions.Size = new System.Drawing.Size(484, 20);
            this.AllowedFirmwareVersions.TabIndex = 11;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(7, 10);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(262, 13);
            this.label4.TabIndex = 15;
            this.label4.Text = "Read parameters from old encrypted firmware file (.bin)";
            // 
            // SelectOldFirmwareFileButton
            // 
            this.SelectOldFirmwareFileButton.Location = new System.Drawing.Point(379, 25);
            this.SelectOldFirmwareFileButton.Name = "SelectOldFirmwareFileButton";
            this.SelectOldFirmwareFileButton.Size = new System.Drawing.Size(83, 23);
            this.SelectOldFirmwareFileButton.TabIndex = 14;
            this.SelectOldFirmwareFileButton.Text = "SELECT FILE";
            this.SelectOldFirmwareFileButton.UseVisualStyleBackColor = true;
            this.SelectOldFirmwareFileButton.Click += new System.EventHandler(this.SelectOldFirmwareFileButton_Click);
            // 
            // OldFirmwareFile
            // 
            this.OldFirmwareFile.Location = new System.Drawing.Point(9, 26);
            this.OldFirmwareFile.Name = "OldFirmwareFile";
            this.OldFirmwareFile.Size = new System.Drawing.Size(364, 20);
            this.OldFirmwareFile.TabIndex = 13;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(7, 26);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(106, 13);
            this.label6.TabIndex = 19;
            this.label6.Text = "AES key (128 bit)";
            // 
            // AESKey
            // 
            this.AESKey.Location = new System.Drawing.Point(7, 42);
            this.AESKey.Name = "AESKey";
            this.AESKey.Size = new System.Drawing.Size(423, 20);
            this.AESKey.TabIndex = 18;
            this.AESKey.Text = "160, 232, 12, 72, 242, 106, 188, 57, 232, 214, 158, 164, 215, 157, 8, 9";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label7.Location = new System.Drawing.Point(7, 87);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(218, 13);
            this.label7.TabIndex = 20;
            this.label7.Text = "AES Initialization vector (IV) (128 bit)";
            // 
            // AESIV
            // 
            this.AESIV.Location = new System.Drawing.Point(10, 102);
            this.AESIV.Name = "AESIV";
            this.AESIV.Size = new System.Drawing.Size(420, 20);
            this.AESIV.TabIndex = 21;
            this.AESIV.Text = "173, 2, 166, 235, 16, 198, 93, 161, 134, 117, 241, 175, 25, 127, 184, 79";
            // 
            // GenerateAESIV
            // 
            this.GenerateAESIV.Location = new System.Drawing.Point(434, 100);
            this.GenerateAESIV.Name = "GenerateAESIV";
            this.GenerateAESIV.Size = new System.Drawing.Size(50, 23);
            this.GenerateAESIV.TabIndex = 22;
            this.GenerateAESIV.Text = "GEN";
            this.GenerateAESIV.UseVisualStyleBackColor = true;
            this.GenerateAESIV.Click += new System.EventHandler(this.GenerateAESIV_Click);
            // 
            // GenerateAESKey
            // 
            this.GenerateAESKey.Location = new System.Drawing.Point(434, 40);
            this.GenerateAESKey.Name = "GenerateAESKey";
            this.GenerateAESKey.Size = new System.Drawing.Size(50, 23);
            this.GenerateAESKey.TabIndex = 23;
            this.GenerateAESKey.Text = "GEN";
            this.GenerateAESKey.UseVisualStyleBackColor = true;
            this.GenerateAESKey.Click += new System.EventHandler(this.GenerateAESKey_Click);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Font = new System.Drawing.Font("Microsoft Sans Serif", 7F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label8.Location = new System.Drawing.Point(9, 62);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(141, 13);
            this.label8.TabIndex = 25;
            this.label8.Text = "byte1, byte2, ... byte16 (dec)";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(218, 88);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(136, 13);
            this.label10.TabIndex = 27;
            this.label10.Text = "byte1, byte2, ... byte4 (dec)";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(13, 150);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(301, 13);
            this.label5.TabIndex = 28;
            this.label5.Text = "byte1, byte2, ... byte10 (dec); byte1, byte2, ... byte10 (dec); ... ";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label11.Location = new System.Drawing.Point(8, 28);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(111, 13);
            this.label11.TabIndex = 30;
            this.label11.Text = "Device ID (1 byte)";
            // 
            // DeviceId
            // 
            this.DeviceId.Location = new System.Drawing.Point(124, 25);
            this.DeviceId.Name = "DeviceId";
            this.DeviceId.Size = new System.Drawing.Size(57, 20);
            this.DeviceId.TabIndex = 29;
            this.DeviceId.Text = "12";
            this.DeviceId.TextChanged += new System.EventHandler(this.DeviceId_TextChanged);
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label12.Location = new System.Drawing.Point(8, 54);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(127, 13);
            this.label12.TabIndex = 32;
            this.label12.Text = "Device Code (1 byte)";
            // 
            // DeviceCode
            // 
            this.DeviceCode.Location = new System.Drawing.Point(141, 51);
            this.DeviceCode.Name = "DeviceCode";
            this.DeviceCode.Size = new System.Drawing.Size(55, 20);
            this.DeviceCode.TabIndex = 31;
            this.DeviceCode.Text = "76";
            // 
            // groupBox1
            // 
            this.groupBox1.BackColor = System.Drawing.Color.Silver;
            this.groupBox1.Controls.Add(this.DevicePageSize);
            this.groupBox1.Controls.Add(this.label13);
            this.groupBox1.Controls.Add(this.label11);
            this.groupBox1.Controls.Add(this.label12);
            this.groupBox1.Controls.Add(this.DeviceId);
            this.groupBox1.Controls.Add(this.DeviceCode);
            this.groupBox1.Location = new System.Drawing.Point(9, 62);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(501, 86);
            this.groupBox1.TabIndex = 33;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Device parameters";
            // 
            // DevicePageSize
            // 
            this.DevicePageSize.FormattingEnabled = true;
            this.DevicePageSize.Items.AddRange(new object[] {
            "512 - XMEGA 256/192/128",
            "256 - XMEGA 64/32/16"});
            this.DevicePageSize.Location = new System.Drawing.Point(335, 24);
            this.DevicePageSize.Name = "DevicePageSize";
            this.DevicePageSize.Size = new System.Drawing.Size(160, 21);
            this.DevicePageSize.TabIndex = 34;
            this.DevicePageSize.Text = "512 - XMEGA 256/192/128";
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label13.Location = new System.Drawing.Point(221, 27);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(108, 13);
            this.label13.TabIndex = 33;
            this.label13.Text = "Device Page Size";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.Label1);
            this.groupBox2.Controls.Add(this.NewFirmwareVersion);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Controls.Add(this.label10);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.AllowedFirmwareVersions);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Controls.Add(this.NewFirmwareFile);
            this.groupBox2.Controls.Add(this.SelectNewFirmwareButton);
            this.groupBox2.Location = new System.Drawing.Point(9, 155);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(501, 170);
            this.groupBox2.TabIndex = 34;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Firmware parameters";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.label14);
            this.groupBox3.Controls.Add(this.label6);
            this.groupBox3.Controls.Add(this.AESKey);
            this.groupBox3.Controls.Add(this.GenerateAESKey);
            this.groupBox3.Controls.Add(this.AESIV);
            this.groupBox3.Controls.Add(this.GenerateAESIV);
            this.groupBox3.Controls.Add(this.label8);
            this.groupBox3.Controls.Add(this.label7);
            this.groupBox3.Location = new System.Drawing.Point(10, 334);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(500, 147);
            this.groupBox3.TabIndex = 35;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "AES Parameters";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Font = new System.Drawing.Font("Microsoft Sans Serif", 7F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label14.Location = new System.Drawing.Point(9, 123);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(141, 13);
            this.label14.TabIndex = 26;
            this.label14.Text = "byte1, byte2, ... byte16 (dec)";
            // 
            // ReadOldFirmwareFileButton
            // 
            this.ReadOldFirmwareFileButton.Location = new System.Drawing.Point(460, 25);
            this.ReadOldFirmwareFileButton.Name = "ReadOldFirmwareFileButton";
            this.ReadOldFirmwareFileButton.Size = new System.Drawing.Size(53, 23);
            this.ReadOldFirmwareFileButton.TabIndex = 36;
            this.ReadOldFirmwareFileButton.Text = "READ";
            this.ReadOldFirmwareFileButton.UseVisualStyleBackColor = true;
            this.ReadOldFirmwareFileButton.Click += new System.EventHandler(this.ReadOldFirmwareFileButton_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Silver;
            this.ClientSize = new System.Drawing.Size(521, 517);
            this.Controls.Add(this.ReadOldFirmwareFileButton);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.SelectOldFirmwareFileButton);
            this.Controls.Add(this.OldFirmwareFile);
            this.Controls.Add(this.SaveNewFirmwareFile);
            this.Name = "Form1";
            this.Text = "Firmware encryption";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        internal System.Windows.Forms.Button SaveNewFirmwareFile;
        internal System.Windows.Forms.Label Label1;
        internal System.Windows.Forms.TextBox NewFirmwareVersion;
        internal System.Windows.Forms.Button SelectNewFirmwareButton;
        internal System.Windows.Forms.TextBox NewFirmwareFile;
        private System.Windows.Forms.OpenFileDialog OpenFileDialog;
        internal System.Windows.Forms.Label label2;
        internal System.Windows.Forms.Label label3;
        internal System.Windows.Forms.TextBox AllowedFirmwareVersions;
        internal System.Windows.Forms.Label label4;
        internal System.Windows.Forms.Button SelectOldFirmwareFileButton;
        internal System.Windows.Forms.TextBox OldFirmwareFile;
        internal System.Windows.Forms.Label label6;
        internal System.Windows.Forms.TextBox AESKey;
        internal System.Windows.Forms.Label label7;
        internal System.Windows.Forms.TextBox AESIV;
        private System.Windows.Forms.Button GenerateAESIV;
        private System.Windows.Forms.Button GenerateAESKey;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label5;
        internal System.Windows.Forms.Label label11;
        internal System.Windows.Forms.TextBox DeviceId;
        internal System.Windows.Forms.Label label12;
        internal System.Windows.Forms.TextBox DeviceCode;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.ComboBox DevicePageSize;
        internal System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label14;
        internal System.Windows.Forms.Button ReadOldFirmwareFileButton;
    }
}

