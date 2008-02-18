namespace DeveloperTerminal
{
    partial class TextBoxSettings
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TextBoxSettings));
            this.colorDialog1 = new System.Windows.Forms.ColorDialog();
            this.fontDialog1 = new System.Windows.Forms.FontDialog();
            this.buttonBackColor = new System.Windows.Forms.Button();
            this.buttonForeColor = new System.Windows.Forms.Button();
            this.buttonFont = new System.Windows.Forms.Button();
            this.buttonOkay = new System.Windows.Forms.Button();
            this.buttonCancel = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // buttonBackColor
            // 
            this.buttonBackColor.Location = new System.Drawing.Point(12, 40);
            this.buttonBackColor.Name = "buttonBackColor";
            this.buttonBackColor.Size = new System.Drawing.Size(172, 23);
            this.buttonBackColor.TabIndex = 0;
            this.buttonBackColor.Text = "Back Color";
            this.buttonBackColor.UseVisualStyleBackColor = true;
            this.buttonBackColor.Click += new System.EventHandler(this.buttonBackColor_Click);
            // 
            // buttonForeColor
            // 
            this.buttonForeColor.Location = new System.Drawing.Point(12, 69);
            this.buttonForeColor.Name = "buttonForeColor";
            this.buttonForeColor.Size = new System.Drawing.Size(172, 23);
            this.buttonForeColor.TabIndex = 1;
            this.buttonForeColor.Text = "Fore Color";
            this.buttonForeColor.UseVisualStyleBackColor = true;
            this.buttonForeColor.Click += new System.EventHandler(this.buttonForeColor_Click);
            // 
            // buttonFont
            // 
            this.buttonFont.Location = new System.Drawing.Point(12, 13);
            this.buttonFont.Name = "buttonFont";
            this.buttonFont.Size = new System.Drawing.Size(172, 23);
            this.buttonFont.TabIndex = 2;
            this.buttonFont.Text = "Font";
            this.buttonFont.UseVisualStyleBackColor = true;
            this.buttonFont.Click += new System.EventHandler(this.buttonFont_Click);
            // 
            // buttonOkay
            // 
            this.buttonOkay.Location = new System.Drawing.Point(12, 117);
            this.buttonOkay.Name = "buttonOkay";
            this.buttonOkay.Size = new System.Drawing.Size(74, 23);
            this.buttonOkay.TabIndex = 3;
            this.buttonOkay.Text = "Okay";
            this.buttonOkay.UseVisualStyleBackColor = true;
            this.buttonOkay.Click += new System.EventHandler(this.buttonOkay_Click);
            // 
            // buttonCancel
            // 
            this.buttonCancel.Location = new System.Drawing.Point(116, 116);
            this.buttonCancel.Name = "buttonCancel";
            this.buttonCancel.Size = new System.Drawing.Size(68, 23);
            this.buttonCancel.TabIndex = 4;
            this.buttonCancel.Text = "Cancel";
            this.buttonCancel.UseVisualStyleBackColor = true;
            this.buttonCancel.Click += new System.EventHandler(this.buttonCancel_Click);
            // 
            // TextBoxSettings
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Bisque;
            this.ClientSize = new System.Drawing.Size(196, 151);
            this.Controls.Add(this.buttonCancel);
            this.Controls.Add(this.buttonOkay);
            this.Controls.Add(this.buttonFont);
            this.Controls.Add(this.buttonForeColor);
            this.Controls.Add(this.buttonBackColor);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "TextBoxSettings";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "TextBoxSettings";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ColorDialog colorDialog1;
        private System.Windows.Forms.FontDialog fontDialog1;
        private System.Windows.Forms.Button buttonBackColor;
        private System.Windows.Forms.Button buttonForeColor;
        private System.Windows.Forms.Button buttonFont;
        private System.Windows.Forms.Button buttonOkay;
        private System.Windows.Forms.Button buttonCancel;
    }
}