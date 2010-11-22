using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace RemoteControl
{
    public partial class NetworkDialog : Form
    {
        string m_IP;
        public string IP
        {
            get { return m_IP; }
            set { m_IP = value; }
        }

        string m_Port;
        public string Port
        {
            get { return m_Port; }
            set { m_Port = value; }
        }

        public NetworkDialog( string ip, string port )
        {
            InitializeComponent();
            m_IP = ip;
            txt_IP.Text = m_IP;

            m_Port = port;
            txt_Port.Text = m_Port;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            txt_IP.Text = "234.5.6.7";
            txt_Port.Text = "12345";
        }

        private void button3_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            m_IP = txt_IP.Text;
            m_Port = txt_Port.Text;
            this.DialogResult = DialogResult.OK;
        }

    }
}
