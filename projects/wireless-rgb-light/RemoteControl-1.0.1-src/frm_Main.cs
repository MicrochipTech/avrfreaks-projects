using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Net.Sockets;
using System.Net;

namespace RemoteControl
{
    public partial class frm_Main : Form
    {
        string m_IP   = "234.5.6.7";
        string m_Port = "12345";

        public frm_Main()
        {
            InitializeComponent();
        }

        private void btn_Click(object sender, EventArgs args )
        {
            Button button = (Button)sender;
            
            /* Prepare message
             */
            StringBuilder builder = new StringBuilder("AmbientColor:");

            builder.Append( button.BackColor.R );
            builder.Append(':');
            builder.Append( button.BackColor.G );
            builder.Append(':');
            builder.Append( button.BackColor.B );
            builder.Append(':');
            builder.Append("*");
            builder.Append(':');
            builder.Append("RGB");

            byte[] datagram = Encoding.ASCII.GetBytes( builder.ToString() );


            /* Network stuff comes here ...
             */
            UdpClient server = null;
            try
            {
                IPAddress address = IPAddress.Parse(  m_IP );
                int port = 12345;
                try
                {
                    Int16.Parse(m_Port);
                }
                catch (Exception)
                {
                    m_Port = "12345";
                    port = 12345;
                }

                IPEndPoint client = new IPEndPoint(address, port);
                server = new UdpClient(port);
                server.Send(datagram, datagram.Length, client);
            }
            catch (Exception e)
            {
                System.Console.WriteLine(e.Message);
            }
            finally
            {
                if (server != null)
                {
                    server.Close();
                }
            }
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            frm_About about = new frm_About();
            about.ShowDialog();
        }

        private void iPAddresToolStripMenuItem_Click(object sender, EventArgs e)
        {
            NetworkDialog netdlg = new NetworkDialog( m_IP, m_Port );
            if (netdlg.ShowDialog() == DialogResult.OK)
            {
                m_IP = netdlg.IP;
                m_Port = netdlg.Port;
            }
        }

    }
}
