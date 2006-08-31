/* 
* Project:    SerialPort Terminal
* Company:    Coad .NET, http://coad.net
* Author:     Noah Coad, http://coad.net/noah
* Created:    March 2005
* 
* Notes:      This was created to demonstrate how to use the SerialPort control for
*             communicating with your PC's Serial RS-232 COM Port
* 
*             It is for educational purposes only and not sanctified for industrial use. :)
* 
*             Search for "sherry.ComPort" to see how I'm using the SerialPort control.
*/

#region Namespace Inclusions
using System;
using System.Data;
using System.Text;
using System.Drawing;
using System.IO.Ports;
using System.Windows.Forms;
using System.ComponentModel;
using System.Collections.Generic;

using SerialPortTerminal.Properties;
#endregion

namespace SerialPortTerminal
{

    /// <summary>
    /// frmTerminal
    /// 
    /// TODO: CurrentDataMode is on this class and SerialSherry and must be refactored
    /// (event on SerialSherry which sets form's values)
    /// 
    /// 
    /// </summary>
    public partial class frmTerminal : Form
    {
        #region Local Variables

        // The main control for communicating through the RS-232 port
        private SerialSherry sherry = new SerialSherry();
        
        // Various colors for logging info
        private Color[] LogMsgTypeColor = { Color.Blue, Color.Green, Color.Black, Color.Orange, Color.Red };

        // Temp holder for whether a key was pressed
        private bool KeyHandled = false;

        #endregion

        #region Constructor
        public frmTerminal()
        {
            // Build the form
            InitializeComponent();

            // Restore the users settings
            InitializeControlValues();

            // Enable/disable controls based on the current state
            EnableControls(false);

            // When data is recieved through the port, call this method
            sherry.DataReceived += new EventHandler<SerialDataReceivedEventArgs2>(port_DataReceived);
        }

        #endregion

        #region Local Methods

        /// <summary> Save the user's settings. </summary>
        private void SaveSettings()
        {
            Settings.Default.BaudRate = int.Parse(cmbBaudRate.Text);
            Settings.Default.DataBits = int.Parse(cmbDataBits.Text);
            Settings.Default.DataMode = CurrentDataMode;
            Settings.Default.Parity = (Parity)Enum.Parse(typeof(Parity), cmbParity.Text);
            Settings.Default.StopBits = (StopBits)Enum.Parse(typeof(StopBits), cmbStopBits.Text);
            Settings.Default.PortName = cmbPortName.Text;

            Settings.Default.Save();
        }

        /// <summary> Populate the form's controls with default settings. </summary>
        private void InitializeControlValues()
        {
            cmbParity.Items.Clear(); cmbParity.Items.AddRange(Enum.GetNames(typeof(Parity)));
            cmbStopBits.Items.Clear(); cmbStopBits.Items.AddRange(Enum.GetNames(typeof(StopBits)));

            cmbParity.Text = Settings.Default.Parity.ToString();
            cmbStopBits.Text = Settings.Default.StopBits.ToString();
            cmbDataBits.Text = Settings.Default.DataBits.ToString();
            cmbParity.Text = Settings.Default.Parity.ToString();
            cmbBaudRate.Text = Settings.Default.BaudRate.ToString();
            CurrentDataMode = Settings.Default.DataMode;

            cmbPortName.Items.Clear();
            foreach (string s in SerialPort.GetPortNames())
                cmbPortName.Items.Add(s);

            if (cmbPortName.Items.Contains(Settings.Default.PortName)) cmbPortName.Text = Settings.Default.PortName;
            else if (cmbPortName.Items.Count > 0) cmbPortName.SelectedIndex = 0;
            else
            {
                MessageBox.Show(this, "There are no COM Ports detected on this computer.\nPlease install a COM Port and restart this app.", "No COM Ports Installed", MessageBoxButtons.OK, MessageBoxIcon.Error);
                this.Close();
            }
        }

        /// <summary> Enable/disable controls based on the app's current state. </summary>
        private void EnableControls(bool IsOpen)
        {
            // Enable/disable controls based on whether the port is open or not
            gbPortSettings.Enabled = !IsOpen;
            txtSendData.Enabled = btnSend.Enabled = IsOpen;

            if (IsOpen) btnOpenPort.Text = "&Close Port";
            else btnOpenPort.Text = "&Open Port";
        }

        /// <summary> Log data to the terminal window. </summary>
        /// <param name="msgtype"> The type of message to be written. </param>
        /// <param name="msg"> The string containing the message to be shown. </param>
        private void Log(LogMsgType msgtype, string msg)
        {
            lock (sherry.syncObject){msg = sherry.ReceivedData;}

            rtfTerminal.BeginInvoke(new EventHandler(delegate
            {
                rtfTerminal.SelectedText = string.Empty;
                rtfTerminal.SelectionFont = new Font(rtfTerminal.SelectionFont, FontStyle.Bold);
                rtfTerminal.SelectionColor = LogMsgTypeColor[(int)msgtype];
                rtfTerminal.AppendText(msg);
                rtfTerminal.ScrollToCaret();
            }));
        }

        #endregion

        #region Local Properties
        private DataMode CurrentDataMode
        {
            get
            {
                if (rbHex.Checked) return DataMode.Hex;
                else return DataMode.Text;
            }
            set
            {
                if (value == DataMode.Text) rbText.Checked = true;
                else rbHex.Checked = true;
            }
        }
        #endregion

        #region Event Handlers
        private void lnkAbout_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            // Show the user the about dialog
            (new frmAbout()).ShowDialog(this);
        }

        private void frmTerminal_Shown(object sender, EventArgs e)
        {
            Log(LogMsgType.Normal, String.Format("Application Started at {0}\n", DateTime.Now));
        }
        private void frmTerminal_FormClosing(object sender, FormClosingEventArgs e)
        {
            // The form is closing, save the user's preferences
            SaveSettings();
        }

        private void rbText_CheckedChanged(object sender, EventArgs e)
        { if (rbText.Checked) CurrentDataMode = DataMode.Text; }
        private void rbHex_CheckedChanged(object sender, EventArgs e)
        { if (rbHex.Checked) CurrentDataMode = DataMode.Hex; }

        private void cmbBaudRate_Validating(object sender, CancelEventArgs e)
        { int x; e.Cancel = !int.TryParse(cmbBaudRate.Text, out x); }
        private void cmbDataBits_Validating(object sender, CancelEventArgs e)
        { int x; e.Cancel = !int.TryParse(cmbDataBits.Text, out x); }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnOpenPort_Click(object sender, EventArgs e)
        {
            bool ok = sherry.OpenPort(
                int.Parse(cmbBaudRate.Text),
                int.Parse(cmbDataBits.Text),
                (StopBits)Enum.Parse(typeof(StopBits), cmbStopBits.Text),
                (Parity)Enum.Parse(typeof(Parity), cmbParity.Text),
                cmbPortName.Text);
            
            // Change the state of the form's controls
            EnableControls(ok);

            // If the port is open, send focus to the send data box
            if (ok) txtSendData.Focus();
        }

        /// <summary>
        /// TODO: call this on a work thread
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnSend_Click(object sender, EventArgs e)
        {
            this.worker.RunWorkerAsync(txtSendData.Text);
            //this.sherry.SendData(txtSendData.Text);
            //txtSendData.SelectAll();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void port_DataReceived(object sender, SerialDataReceivedEventArgs2 e)
        {
            Log(LogMsgType.Incoming, e.Data);           
        }

        private void txtSendData_KeyDown(object sender, KeyEventArgs e)
        {
            // If the user presses [ENTER], send the data now
            if (KeyHandled = e.KeyCode == Keys.Enter) { e.Handled = true; sherry.SendData(this.txtSendData.Text); }
        }
        private void txtSendData_KeyPress(object sender, KeyPressEventArgs e)
        { e.Handled = KeyHandled; }
        #endregion

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void frmTerminal_Load(object sender, EventArgs e)
        {
            this.txtSendData.Text = System.IO.File.ReadAllText(@"c:\test.txt");
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void worker_DoWork(object sender, DoWorkEventArgs e)
        {
            this.sherry.CurrentDataMode = this.CurrentDataMode;
            this.sherry.SendData(e.Argument.ToString());
        }
    }
}