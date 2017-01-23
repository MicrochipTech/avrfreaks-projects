namespace UpgradeTool
{
    partial class UpgradeForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(UpgradeForm));
            this.UpgradeProgressBar = new System.Windows.Forms.ProgressBar();
            this.MessageLabel = new System.Windows.Forms.Label();
            this.ActionButton = new System.Windows.Forms.Button();
            this.CurrentFirmwareVersionLabel = new System.Windows.Forms.Label();
            this.FirmwareDeviceNameLabel = new System.Windows.Forms.Label();
            this.StatusMessageLabel = new System.Windows.Forms.Label();
            this.UpgradeTimer = new System.Windows.Forms.Timer(this.components);
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.OpenFirmwareFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.label3 = new System.Windows.Forms.Label();
            this.FirmwareVersionLabel = new System.Windows.Forms.Label();
            this.Output = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // UpgradeProgressBar
            // 
            this.UpgradeProgressBar.Location = new System.Drawing.Point(12, 71);
            this.UpgradeProgressBar.Name = "UpgradeProgressBar";
            this.UpgradeProgressBar.Size = new System.Drawing.Size(559, 26);
            this.UpgradeProgressBar.TabIndex = 0;
            // 
            // MessageLabel
            // 
            this.MessageLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.MessageLabel.ForeColor = System.Drawing.Color.White;
            this.MessageLabel.Location = new System.Drawing.Point(12, 9);
            this.MessageLabel.Name = "MessageLabel";
            this.MessageLabel.Size = new System.Drawing.Size(559, 23);
            this.MessageLabel.TabIndex = 1;
            this.MessageLabel.Text = "Message";
            this.MessageLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // ActionButton
            // 
            this.ActionButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.ActionButton.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.ActionButton.Location = new System.Drawing.Point(157, 172);
            this.ActionButton.Name = "ActionButton";
            this.ActionButton.Padding = new System.Windows.Forms.Padding(10, 0, 10, 0);
            this.ActionButton.Size = new System.Drawing.Size(258, 23);
            this.ActionButton.TabIndex = 2;
            this.ActionButton.Text = "ACTION";
            this.ActionButton.UseVisualStyleBackColor = true;
            this.ActionButton.TextChanged += new System.EventHandler(this.ActionButton_TextChanged);
            this.ActionButton.Click += new System.EventHandler(this.ActionButton_Click);
            // 
            // CurrentFirmwareVersionLabel
            // 
            this.CurrentFirmwareVersionLabel.AutoSize = true;
            this.CurrentFirmwareVersionLabel.ForeColor = System.Drawing.Color.White;
            this.CurrentFirmwareVersionLabel.Location = new System.Drawing.Point(104, 150);
            this.CurrentFirmwareVersionLabel.Name = "CurrentFirmwareVersionLabel";
            this.CurrentFirmwareVersionLabel.Size = new System.Drawing.Size(82, 13);
            this.CurrentFirmwareVersionLabel.TabIndex = 3;
            this.CurrentFirmwareVersionLabel.Text = "(not connected)";
            // 
            // FirmwareDeviceNameLabel
            // 
            this.FirmwareDeviceNameLabel.AutoSize = true;
            this.FirmwareDeviceNameLabel.ForeColor = System.Drawing.Color.White;
            this.FirmwareDeviceNameLabel.Location = new System.Drawing.Point(104, 117);
            this.FirmwareDeviceNameLabel.Name = "FirmwareDeviceNameLabel";
            this.FirmwareDeviceNameLabel.Size = new System.Drawing.Size(84, 13);
            this.FirmwareDeviceNameLabel.TabIndex = 4;
            this.FirmwareDeviceNameLabel.Text = "(no file selected)";
            // 
            // StatusMessageLabel
            // 
            this.StatusMessageLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.StatusMessageLabel.ForeColor = System.Drawing.Color.White;
            this.StatusMessageLabel.Location = new System.Drawing.Point(12, 40);
            this.StatusMessageLabel.Name = "StatusMessageLabel";
            this.StatusMessageLabel.Size = new System.Drawing.Size(559, 23);
            this.StatusMessageLabel.TabIndex = 5;
            this.StatusMessageLabel.Text = "Status Message";
            this.StatusMessageLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // UpgradeTimer
            // 
            this.UpgradeTimer.Tick += new System.EventHandler(this.UpgradeTimer_Tick);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.ForeColor = System.Drawing.Color.White;
            this.label1.Location = new System.Drawing.Point(12, 117);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(87, 13);
            this.label1.TabIndex = 6;
            this.label1.Text = "Firmware device:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.ForeColor = System.Drawing.Color.White;
            this.label2.Location = new System.Drawing.Point(12, 150);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(86, 13);
            this.label2.TabIndex = 7;
            this.label2.Text = "Current firmware:";
            // 
            // OpenFirmwareFileDialog
            // 
            this.OpenFirmwareFileDialog.Filter = "Bin files|*.bin|All files|*.*";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.ForeColor = System.Drawing.Color.White;
            this.label3.Location = new System.Drawing.Point(12, 134);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(89, 13);
            this.label3.TabIndex = 9;
            this.label3.Text = "Firmware version:";
            // 
            // FirmwareVersionLabel
            // 
            this.FirmwareVersionLabel.AutoSize = true;
            this.FirmwareVersionLabel.ForeColor = System.Drawing.Color.White;
            this.FirmwareVersionLabel.Location = new System.Drawing.Point(104, 134);
            this.FirmwareVersionLabel.Name = "FirmwareVersionLabel";
            this.FirmwareVersionLabel.Size = new System.Drawing.Size(84, 13);
            this.FirmwareVersionLabel.TabIndex = 8;
            this.FirmwareVersionLabel.Text = "(no file selected)";
            // 
            // Output
            // 
            this.Output.Location = new System.Drawing.Point(421, 103);
            this.Output.Multiline = true;
            this.Output.Name = "Output";
            this.Output.Size = new System.Drawing.Size(150, 92);
            this.Output.TabIndex = 10;
            this.Output.Visible = false;
            // 
            // UpgradeForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Black;
            this.ClientSize = new System.Drawing.Size(583, 207);
            this.Controls.Add(this.Output);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.FirmwareVersionLabel);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.StatusMessageLabel);
            this.Controls.Add(this.FirmwareDeviceNameLabel);
            this.Controls.Add(this.CurrentFirmwareVersionLabel);
            this.Controls.Add(this.ActionButton);
            this.Controls.Add(this.MessageLabel);
            this.Controls.Add(this.UpgradeProgressBar);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "UpgradeForm";
            this.Text = "Firmware upgrade";
            this.Load += new System.EventHandler(this.UpgradeForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ProgressBar UpgradeProgressBar;
        private System.Windows.Forms.Label MessageLabel;
        private System.Windows.Forms.Button ActionButton;
        private System.Windows.Forms.Label CurrentFirmwareVersionLabel;
        private System.Windows.Forms.Label FirmwareDeviceNameLabel;
        private System.Windows.Forms.Label StatusMessageLabel;
        private System.Windows.Forms.Timer UpgradeTimer;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.OpenFileDialog OpenFirmwareFileDialog;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label FirmwareVersionLabel;
        private System.Windows.Forms.TextBox Output;
    }
}