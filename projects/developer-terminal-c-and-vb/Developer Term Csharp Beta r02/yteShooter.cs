using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace DeveloperTerminal
{
    public partial class ByteShooter : Form
    {
        #region initialization
        private int Interval = 100;

        public ByteShooter()
        {
            InitializeComponent();
        }

        private void ByteShooter_Load(object sender, EventArgs e)
        {
            char c;
            // Load the comboBox with 0x00 to 0xFF
            for (int i = 0; i < 16; i++)
            {
                c = (char)i;
                this.comboBoxSendByte.Items.Add("0x0" + i.ToString("X") + " -> " +  c.ToString());
            }
            for (int i = 16; i < 256; i++)
            {
                c = (char)i;
                this.comboBoxSendByte.Items.Add("0x" + i.ToString("X") + " -> " + c.ToString());
            }
        }
        #endregion

        #region Data Assessors
        public int interval
        {
            get
            {
                return Interval;
            }
        }

        private byte ByteToSend = 0;
        public byte byteToSend
        {
            get
            {
                return ByteToSend;
            }
        }
        #endregion

        #region Components
        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            try
            {
                Interval = Convert.ToInt32(textBox1.Text);
                timer1.Interval = Interval;
            }
            catch
            {
                timer1.Enabled = false;
                MessageBox.Show("Error with interval");
            }
        }

        private void comboBoxSendByte_SelectedIndexChanged(object sender, EventArgs e)
        {      
            ByteToSend = (byte)comboBoxSendByte.SelectedIndex;
        }

        private void buttonSend_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
        }
        #endregion

    }
}