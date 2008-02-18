namespace DeveloperTerminal
{
    partial class PortSettings
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(PortSettings));
            this.listBoxPorts = new System.Windows.Forms.ListBox();
            this.label1 = new System.Windows.Forms.Label();
            this.labelPort = new System.Windows.Forms.Label();
            this.comboBoxBaud = new System.Windows.Forms.ComboBox();
            this.labelBaud = new System.Windows.Forms.Label();
            this.buttonOkay = new System.Windows.Forms.Button();
            this.buttonCancel = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.comboBoxDataBits = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.comboBoxHandshaking = new System.Windows.Forms.ComboBox();
            this.comboBoxParity = new System.Windows.Forms.ComboBox();
            this.comboBoxStopBits = new System.Windows.Forms.ComboBox();
            this.SuspendLayout();
            // 
            // listBoxPorts
            // 
            this.listBoxPorts.FormattingEnabled = true;
            this.listBoxPorts.Location = new System.Drawing.Point(6, 25);
            this.listBoxPorts.Name = "listBoxPorts";
            this.listBoxPorts.Size = new System.Drawing.Size(236, 82);
            this.listBoxPorts.TabIndex = 0;
            this.listBoxPorts.SelectedIndexChanged += new System.EventHandler(this.listBoxPorts_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(4, 7);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(183, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Double click on port you want to use:";
            // 
            // labelPort
            // 
            this.labelPort.AutoSize = true;
            this.labelPort.Location = new System.Drawing.Point(13, 126);
            this.labelPort.Name = "labelPort";
            this.labelPort.Size = new System.Drawing.Size(142, 13);
            this.labelPort.TabIndex = 2;
            this.labelPort.Text = "Selected Port:  Not Selected";
            // 
            // comboBoxBaud
            // 
            this.comboBoxBaud.FormattingEnabled = true;
            this.comboBoxBaud.Location = new System.Drawing.Point(102, 151);
            this.comboBoxBaud.Name = "comboBoxBaud";
            this.comboBoxBaud.Size = new System.Drawing.Size(121, 21);
            this.comboBoxBaud.TabIndex = 3;
            this.comboBoxBaud.SelectedIndexChanged += new System.EventHandler(this.comboBoxBaud_SelectedIndexChanged);
            // 
            // labelBaud
            // 
            this.labelBaud.AutoSize = true;
            this.labelBaud.Location = new System.Drawing.Point(46, 154);
            this.labelBaud.Name = "labelBaud";
            this.labelBaud.Size = new System.Drawing.Size(50, 13);
            this.labelBaud.TabIndex = 4;
            this.labelBaud.Text = "Baudrate";
            // 
            // buttonOkay
            // 
            this.buttonOkay.Location = new System.Drawing.Point(25, 283);
            this.buttonOkay.Name = "buttonOkay";
            this.buttonOkay.Size = new System.Drawing.Size(75, 23);
            this.buttonOkay.TabIndex = 5;
            this.buttonOkay.Text = "Okay";
            this.buttonOkay.UseVisualStyleBackColor = true;
            this.buttonOkay.Click += new System.EventHandler(this.buttonOkay_Click);
            // 
            // buttonCancel
            // 
            this.buttonCancel.Location = new System.Drawing.Point(148, 283);
            this.buttonCancel.Name = "buttonCancel";
            this.buttonCancel.Size = new System.Drawing.Size(75, 23);
            this.buttonCancel.TabIndex = 6;
            this.buttonCancel.Text = "Cancel";
            this.buttonCancel.UseVisualStyleBackColor = true;
            this.buttonCancel.Click += new System.EventHandler(this.buttonCancel_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(46, 176);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(50, 13);
            this.label2.TabIndex = 7;
            this.label2.Text = "Data Bits";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(63, 198);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(33, 13);
            this.label3.TabIndex = 8;
            this.label3.Text = "Parity";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(46, 220);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(49, 13);
            this.label4.TabIndex = 9;
            this.label4.Text = "Stop Bits";
            // 
            // comboBoxDataBits
            // 
            this.comboBoxDataBits.FormattingEnabled = true;
            this.comboBoxDataBits.Location = new System.Drawing.Point(102, 173);
            this.comboBoxDataBits.Name = "comboBoxDataBits";
            this.comboBoxDataBits.Size = new System.Drawing.Size(121, 21);
            this.comboBoxDataBits.TabIndex = 10;
            this.comboBoxDataBits.SelectedIndexChanged += new System.EventHandler(this.comboBoxDataBits_SelectedIndexChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(25, 242);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(70, 13);
            this.label5.TabIndex = 11;
            this.label5.Text = "Handshaking";
            // 
            // comboBoxHandshaking
            // 
            this.comboBoxHandshaking.FormattingEnabled = true;
            this.comboBoxHandshaking.Location = new System.Drawing.Point(102, 239);
            this.comboBoxHandshaking.Name = "comboBoxHandshaking";
            this.comboBoxHandshaking.Size = new System.Drawing.Size(121, 21);
            this.comboBoxHandshaking.TabIndex = 12;
            this.comboBoxHandshaking.SelectedIndexChanged += new System.EventHandler(this.comboBoxHandshaking_SelectedIndexChanged);
            // 
            // comboBoxParity
            // 
            this.comboBoxParity.FormattingEnabled = true;
            this.comboBoxParity.Location = new System.Drawing.Point(102, 195);
            this.comboBoxParity.Name = "comboBoxParity";
            this.comboBoxParity.Size = new System.Drawing.Size(121, 21);
            this.comboBoxParity.TabIndex = 13;
            this.comboBoxParity.SelectedIndexChanged += new System.EventHandler(this.comboBoxParity_SelectedIndexChanged);
            // 
            // comboBoxStopBits
            // 
            this.comboBoxStopBits.FormattingEnabled = true;
            this.comboBoxStopBits.Location = new System.Drawing.Point(102, 217);
            this.comboBoxStopBits.Name = "comboBoxStopBits";
            this.comboBoxStopBits.Size = new System.Drawing.Size(121, 21);
            this.comboBoxStopBits.TabIndex = 14;
            this.comboBoxStopBits.SelectedIndexChanged += new System.EventHandler(this.comboBoxStopBits_SelectedIndexChanged);
            // 
            // PortSettings
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Bisque;
            this.ClientSize = new System.Drawing.Size(248, 315);
            this.Controls.Add(this.comboBoxStopBits);
            this.Controls.Add(this.comboBoxParity);
            this.Controls.Add(this.comboBoxHandshaking);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.comboBoxDataBits);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.buttonCancel);
            this.Controls.Add(this.buttonOkay);
            this.Controls.Add(this.labelBaud);
            this.Controls.Add(this.comboBoxBaud);
            this.Controls.Add(this.labelPort);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.listBoxPorts);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "PortSettings";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Settings";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListBox listBoxPorts;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label labelPort;
        private System.Windows.Forms.ComboBox comboBoxBaud;
        private System.Windows.Forms.Label labelBaud;
        private System.Windows.Forms.Button buttonOkay;
        private System.Windows.Forms.Button buttonCancel;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox comboBoxDataBits;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ComboBox comboBoxHandshaking;
        private System.Windows.Forms.ComboBox comboBoxParity;
        private System.Windows.Forms.ComboBox comboBoxStopBits;
    }
}