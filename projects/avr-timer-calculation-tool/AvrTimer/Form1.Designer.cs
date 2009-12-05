namespace AvrTimer
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
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.button1_Calc = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.textBox1_InputClk = new System.Windows.Forms.TextBox();
            this.textBox2_prescaler = new System.Windows.Forms.TextBox();
            this.textBox3_targettime = new System.Windows.Forms.TextBox();
            this.textBox4_resultCOUNTS = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.textBox1_Req_pwm_freq = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.textBox2_Result_TOP = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(5, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(90, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Input Clock(MHZ)";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(100, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(51, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "Prescaler";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(154, 9);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(85, 13);
            this.label3.TabIndex = 2;
            this.label3.Text = "Target Time(ms)";
            // 
            // button1_Calc
            // 
            this.button1_Calc.Location = new System.Drawing.Point(265, 16);
            this.button1_Calc.Name = "button1_Calc";
            this.button1_Calc.Size = new System.Drawing.Size(68, 84);
            this.button1_Calc.TabIndex = 3;
            this.button1_Calc.Text = "Calculate";
            this.button1_Calc.UseVisualStyleBackColor = true;
            this.button1_Calc.Click += new System.EventHandler(this.button1_Calc_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(341, 9);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(81, 13);
            this.label4.TabIndex = 4;
            this.label4.Text = "Counts Needed";
            // 
            // textBox1_InputClk
            // 
            this.textBox1_InputClk.Location = new System.Drawing.Point(8, 25);
            this.textBox1_InputClk.Name = "textBox1_InputClk";
            this.textBox1_InputClk.Size = new System.Drawing.Size(80, 20);
            this.textBox1_InputClk.TabIndex = 5;
            this.textBox1_InputClk.Text = "8";
            this.textBox1_InputClk.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // textBox2_prescaler
            // 
            this.textBox2_prescaler.Location = new System.Drawing.Point(104, 25);
            this.textBox2_prescaler.Name = "textBox2_prescaler";
            this.textBox2_prescaler.Size = new System.Drawing.Size(38, 20);
            this.textBox2_prescaler.TabIndex = 6;
            this.textBox2_prescaler.Text = "1";
            this.textBox2_prescaler.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // textBox3_targettime
            // 
            this.textBox3_targettime.Location = new System.Drawing.Point(157, 25);
            this.textBox3_targettime.Name = "textBox3_targettime";
            this.textBox3_targettime.Size = new System.Drawing.Size(75, 20);
            this.textBox3_targettime.TabIndex = 7;
            this.textBox3_targettime.Text = "1";
            this.textBox3_targettime.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // textBox4_resultCOUNTS
            // 
            this.textBox4_resultCOUNTS.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox4_resultCOUNTS.Location = new System.Drawing.Point(342, 25);
            this.textBox4_resultCOUNTS.Name = "textBox4_resultCOUNTS";
            this.textBox4_resultCOUNTS.Size = new System.Drawing.Size(78, 23);
            this.textBox4_resultCOUNTS.TabIndex = 8;
            this.textBox4_resultCOUNTS.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(2, 56);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(241, 13);
            this.label5.TabIndex = 9;
            this.label5.Text = "MODE:PWM,Phase, Frequency Correct @ 16-bit ";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(-1, 73);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(159, 13);
            this.label6.TabIndex = 10;
            this.label6.Text = "Required PWM Frequency (HZ):";
            // 
            // textBox1_Req_pwm_freq
            // 
            this.textBox1_Req_pwm_freq.Location = new System.Drawing.Point(156, 70);
            this.textBox1_Req_pwm_freq.Name = "textBox1_Req_pwm_freq";
            this.textBox1_Req_pwm_freq.Size = new System.Drawing.Size(93, 20);
            this.textBox1_Req_pwm_freq.TabIndex = 11;
            this.textBox1_Req_pwm_freq.Text = "50";
            this.textBox1_Req_pwm_freq.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(354, 58);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(59, 13);
            this.label7.TabIndex = 12;
            this.label7.Text = "TOP(ICRx)";
            // 
            // textBox2_Result_TOP
            // 
            this.textBox2_Result_TOP.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Bold);
            this.textBox2_Result_TOP.Location = new System.Drawing.Point(344, 74);
            this.textBox2_Result_TOP.Name = "textBox2_Result_TOP";
            this.textBox2_Result_TOP.Size = new System.Drawing.Size(76, 23);
            this.textBox2_Result_TOP.TabIndex = 13;
            this.textBox2_Result_TOP.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(443, 110);
            this.Controls.Add(this.textBox2_Result_TOP);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.textBox1_Req_pwm_freq);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.textBox4_resultCOUNTS);
            this.Controls.Add(this.textBox3_targettime);
            this.Controls.Add(this.textBox2_prescaler);
            this.Controls.Add(this.textBox1_InputClk);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.button1_Calc);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Name = "Form1";
            this.Text = "Avr Timer By Abdullrahman Alfeky ";
            this.Enter += new System.EventHandler(this.Form1_Enter);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button button1_Calc;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox textBox1_InputClk;
        private System.Windows.Forms.TextBox textBox2_prescaler;
        private System.Windows.Forms.TextBox textBox3_targettime;
        private System.Windows.Forms.TextBox textBox4_resultCOUNTS;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox textBox1_Req_pwm_freq;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox textBox2_Result_TOP;
    }
}

