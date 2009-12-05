using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace AvrTimer
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Calc_Click(object sender, EventArgs e)
        {
            Double result, input_clk, Targettime, prescaler,RequiredPWM_freq_HZ,result_pwm;
            input_clk = Convert.ToDouble(textBox1_InputClk.Text)*1000000;
            Targettime = Convert.ToDouble(textBox3_targettime.Text)/1000;
            prescaler = Convert.ToDouble(textBox2_prescaler.Text);
            RequiredPWM_freq_HZ = Convert.ToDouble(textBox1_Req_pwm_freq.Text);


            result = (Targettime) / (prescaler / (input_clk));

            result_pwm= input_clk/(2*prescaler*RequiredPWM_freq_HZ);

            textBox4_resultCOUNTS.Text = result.ToString();
            textBox2_Result_TOP.Text = result_pwm.ToString();

        }

        private void Form1_Enter(object sender, EventArgs e)
        {
            Double result, input_clk, Targettime, prescaler;
            input_clk = Convert.ToDouble(textBox1_InputClk.Text) * 1000000;
            Targettime = Convert.ToDouble(textBox3_targettime.Text) / 1000;
            prescaler = Convert.ToDouble(textBox2_prescaler.Text);

            result = (Targettime) / (prescaler / (input_clk));

            textBox4_resultCOUNTS.Text = result.ToString();


        }


        
    }
}