namespace final001
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
            this.components = new System.ComponentModel.Container();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.label3 = new System.Windows.Forms.Label();
            this.cmdEnter = new System.Windows.Forms.Button();
            this.cmdSend = new System.Windows.Forms.Button();
            this.lblSend = new System.Windows.Forms.Label();
            this.chkSendNow = new System.Windows.Forms.CheckBox();
            this.lbl1001 = new System.Windows.Forms.Label();
            this.txtStream = new System.Windows.Forms.TextBox();
            this.lblBSC = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.cmdCalib = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.lblStatus = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.txtID = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // timer1
            // 
            this.timer1.Interval = 1;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(109, 145);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(35, 13);
            this.label3.TabIndex = 5;
            this.label3.Text = "label3";
            this.label3.Visible = false;
            // 
            // cmdEnter
            // 
            this.cmdEnter.Location = new System.Drawing.Point(72, 78);
            this.cmdEnter.Name = "cmdEnter";
            this.cmdEnter.Size = new System.Drawing.Size(88, 55);
            this.cmdEnter.TabIndex = 6;
            this.cmdEnter.Text = "Edit Text to Send";
            this.cmdEnter.UseVisualStyleBackColor = true;
            this.cmdEnter.Click += new System.EventHandler(this.cmdEnter_Click);
            // 
            // cmdSend
            // 
            this.cmdSend.Location = new System.Drawing.Point(166, 78);
            this.cmdSend.Name = "cmdSend";
            this.cmdSend.Size = new System.Drawing.Size(90, 55);
            this.cmdSend.TabIndex = 7;
            this.cmdSend.Text = "Send Text";
            this.cmdSend.UseVisualStyleBackColor = true;
            this.cmdSend.Click += new System.EventHandler(this.cmdSend_Click);
            // 
            // lblSend
            // 
            this.lblSend.AutoSize = true;
            this.lblSend.Font = new System.Drawing.Font("Courier New", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblSend.Location = new System.Drawing.Point(98, 19);
            this.lblSend.Name = "lblSend";
            this.lblSend.Size = new System.Drawing.Size(306, 27);
            this.lblSend.TabIndex = 8;
            this.lblSend.Text = "<Enter Text  to send>";
            // 
            // chkSendNow
            // 
            this.chkSendNow.AutoSize = true;
            this.chkSendNow.Location = new System.Drawing.Point(64, 145);
            this.chkSendNow.Name = "chkSendNow";
            this.chkSendNow.Size = new System.Drawing.Size(85, 17);
            this.chkSendNow.TabIndex = 9;
            this.chkSendNow.Text = "Send It Now";
            this.chkSendNow.UseVisualStyleBackColor = true;
            this.chkSendNow.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
            // 
            // lbl1001
            // 
            this.lbl1001.AutoSize = true;
            this.lbl1001.Location = new System.Drawing.Point(9, 146);
            this.lbl1001.Name = "lbl1001";
            this.lbl1001.Size = new System.Drawing.Size(49, 13);
            this.lbl1001.TabIndex = 10;
            this.lbl1001.Text = "Bit Strem";
            // 
            // txtStream
            // 
            this.txtStream.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtStream.Location = new System.Drawing.Point(12, 162);
            this.txtStream.Multiline = true;
            this.txtStream.Name = "txtStream";
            this.txtStream.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtStream.Size = new System.Drawing.Size(521, 445);
            this.txtStream.TabIndex = 11;
            // 
            // lblBSC
            // 
            this.lblBSC.AutoSize = true;
            this.lblBSC.Location = new System.Drawing.Point(343, 146);
            this.lblBSC.Name = "lblBSC";
            this.lblBSC.Size = new System.Drawing.Size(28, 13);
            this.lblBSC.TabIndex = 12;
            this.lblBSC.Text = "0:00";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(243, 146);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(94, 13);
            this.label4.TabIndex = 13;
            this.label4.Text = "Transmission Time";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(14, 25);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(68, 13);
            this.label5.TabIndex = 14;
            this.label5.Text = "Text to Send";
            // 
            // cmdCalib
            // 
            this.cmdCalib.Location = new System.Drawing.Point(262, 78);
            this.cmdCalib.Name = "cmdCalib";
            this.cmdCalib.Size = new System.Drawing.Size(90, 55);
            this.cmdCalib.TabIndex = 15;
            this.cmdCalib.Text = "Send Calibration Burst";
            this.cmdCalib.UseVisualStyleBackColor = true;
            this.cmdCalib.Click += new System.EventHandler(this.cmdCalib_Click);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(358, 78);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(90, 55);
            this.button1.TabIndex = 16;
            this.button1.Text = "About";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // lblStatus
            // 
            this.lblStatus.AutoSize = true;
            this.lblStatus.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblStatus.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(0)))));
            this.lblStatus.Location = new System.Drawing.Point(473, 138);
            this.lblStatus.Name = "lblStatus";
            this.lblStatus.Size = new System.Drawing.Size(60, 20);
            this.lblStatus.TabIndex = 17;
            this.lblStatus.Text = "Ready";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(14, 57);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(52, 13);
            this.label1.TabIndex = 18;
            this.label1.Text = "DeviceID";
            // 
            // txtID
            // 
            this.txtID.Location = new System.Drawing.Point(103, 49);
            this.txtID.MaxLength = 1;
            this.txtID.Name = "txtID";
            this.txtID.Size = new System.Drawing.Size(118, 20);
            this.txtID.TabIndex = 19;
            this.txtID.TextChanged += new System.EventHandler(this.txtID_TextChanged);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(546, 619);
            this.Controls.Add(this.txtID);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.lblStatus);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.cmdCalib);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.lblBSC);
            this.Controls.Add(this.txtStream);
            this.Controls.Add(this.lbl1001);
            this.Controls.Add(this.chkSendNow);
            this.Controls.Add(this.lblSend);
            this.Controls.Add(this.cmdSend);
            this.Controls.Add(this.cmdEnter);
            this.Controls.Add(this.label3);
            this.Cursor = System.Windows.Forms.Cursors.Default;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "PDM IR Transmitter Software";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button cmdEnter;
        private System.Windows.Forms.Button cmdSend;
        private System.Windows.Forms.Label lblSend;
        private System.Windows.Forms.CheckBox chkSendNow;
        private System.Windows.Forms.Label lbl1001;
        private System.Windows.Forms.TextBox txtStream;
        private System.Windows.Forms.Label lblBSC;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button cmdCalib;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Label lblStatus;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtID;
    }
}

