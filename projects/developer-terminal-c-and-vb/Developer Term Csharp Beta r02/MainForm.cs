//
// Smiley Terminal evolved over time and like many a creature of evolution, 
// you might find some atavistic artifacts. As my Grandpappy, the hog 
// farmer, used to say: "That's about as useful as tits an a boar." Well,
// I'm going to guess that you'll find some artifacts in this code that 
// are even less useful.
// 
// Don't you just love the legalistic bullshit that is usually inserted at
// this point? I've rewritten some standard boilerplate stuff to say what
// I think we really mean:
//  This work is entirely by Joe Pardue (except for the major portions he
//  stole and forgot to attribute). Copyright by Joe Pardue 4/26/07. and all
//  rights are reserved - any theft of this intellectual property will 
//  result in a challenge to an arm wrestling contest and depending on the
//  nature of the theft, I may break you wrist, or arm, or just rip the
//  sucker out of the socket. 
//
//  This software is not warrented in any way what-so-ever. It WILL screw
//  something up and will kill someone after doing serious property
//  damage. You've been warned. If you sic a lawyer on me, just remember:
//  I'm old, grouchy, have little to lose, and support the Second Amendment?
//
// Ah, yes I feel much better now.
//
// Please send any bug reports, comments, or concerns to joe@smileymicros.com
//
// 7/6/06 - Version 0.0 moved up to C# 2005 Express
// 3/14/07 - Revision 0.1 got back to it and added a bunch of stuff.
// 4/26/07 - Revision 0.2 God only knows what I did, but it is different.
// 4/28/07 - Revision 0.3 changes marked by comment // JWP 4/28/07 to allow byte reception

// BETA - THIS IS THE BETA RELEASE AND WILL HAVE PROBLEMS THAT I MISSED AND YOU WON'T
// BETA - SINCE MICROSOFT RELEASES CODE FOR THE BUYER TO DEBUG, I THOUGHT 'HELL, WHY NOT'
// BETA - PLEASE SEND COMMENTS AND CORRECTIONS TO joe@smileymicros.com

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.IO;
using System.IO.Ports;

using LED;

namespace DeveloperTerminal
{
	public class MainForm : System.Windows.Forms.Form
    {
        #region Initialization
        /* If you build Developer Terminal from scratch using the Express edition
         * you will see some differences in the intializaion section. This is because
         * most of this code was written in earlier versions and imported into Express.
         * The code will work the same thought.*/

        #region C# Express added definitions
        private System.Windows.Forms.GroupBox groupBoxSendText;
		private System.Windows.Forms.RichTextBox richTextBoxSend;
        private System.Windows.Forms.LinkLabel linkLabelVisitWebSite;
		private System.Windows.Forms.GroupBox groupBox3;
		private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.RadioButton radioButtonImmediate;
        private System.Windows.Forms.RichTextBox richTextBoxReceive;
		private System.Windows.Forms.RadioButton radioButtonASCII;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.GroupBox groupBoxReceive;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.RadioButton radioButtonHEX;
		private System.Windows.Forms.ListBox listBoxMacros;
		private System.Windows.Forms.RadioButton radioButtonQueued;
		private System.Windows.Forms.GroupBox groupBoxMacros;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox comboBoxSendByte;
		private System.Windows.Forms.Button buttonSendQue;
        private MenuStrip menuStrip1;
        private ToolStripMenuItem settingsToolStripMenuItem;
        private ToolStripMenuItem portToolStripMenuItem;
        private ToolStripMenuItem sendToolStripMenuItem;
        private ToolStripMenuItem receiveToolStripMenuItem;
        private ToolStripMenuItem helpToolStripMenuItem;
        private ContextMenuStrip contextMenuStripSend;
        private IContainer components;
        private ToolStripMenuItem copyToolStripMenuItem;
        private ToolStripMenuItem cutToolStripMenuItem;
        private ToolStripMenuItem pasteToolStripMenuItem;
        private ToolStripMenuItem selectAllToolStripMenuItem;
        private ToolStripMenuItem undoToolStripMenuItem;
        private ToolStripMenuItem redoToolStripMenuItem;
        private SerialPort serialPort1;
        private ToolStripMenuItem openPortToolStripMenuItem;
        private ToolStripMenuItem saveAsToolStripMenuItem;
        private SaveFileDialog saveFileDialog1;
        private ToolStripMenuItem loadFileToolStripMenuItem;
        private OpenFileDialog openFileDialog1;
        private ToolStripSeparator toolStripSeparator1;
        private ToolStripSeparator toolStripSeparator2;
        private ToolStripMenuItem deleteToolStripMenuItem;
        private GroupBox groupBox5;
        private RadioButton radioButtonTermCharYes;
        private RadioButton radioButtonTermCharNo;
        private Label label5;
        private TextBox textBoxSendCount;
        private Label label8;
        private TextBox textBoxReceiveCount;
        private Label label9;
        private LED.LED22 LEDCTS;
        private Label label10;
        private Label label11;
        private Label label12;
        private LED22 LEDDSR;
        private LED22 LEDDCD;
        private Timer timerModemLines;
        private GroupBox groupBox4;
        private Button buttonDTR;
        private Button buttonRTS;
        private Label label15;
        private LED22 LEDDTR;
        private LED22 LEDRTS;
        private LED22 LEDRI;
        private Panel panel1;
        #endregion

        #region Startup definitions

        // Create an instance of the settings form
        PortSettings portSettings = new PortSettings();

		// Booleans to toggle the modem lines
        private bool RTSToggle = true;
        private bool DTRToggle = true;

        // Keep count of sent and received bytes
        private int SendCount = 0;
        private int ReceiveCount = 0;

		// Immediate or Queued state for sending data
		private bool SendImmediate = true;
		private string QueuedString = "";

		// Send with terminal characters
        private bool UseTermChar = false;

		// Receive as ASCII or Hex state
		private bool ReceiveASCII = true;

		// Create an instance of the TerminalData class
		XMLData xMLData = new XMLData();
	
		// Create an instance of the TerminalXMLIO class to read the terminal data
		XMLIO dataXMLIO = new XMLIO();
		
		// Create MacroData arrays
		string[] MacroDataText = new string[52];
		string[] MacroDataData = new string[52];
        string[] MacroDataType = new string[52];		
		
        // Boolean for the port open state
		public bool PortOpen = false;
        private ToolStripMenuItem toolsToolStripMenuItem;
        private ToolStripMenuItem sendCharToolStripMenuItem;
        private Timer timerSendByte;
        private ToolStripMenuItem fileToolStripMenuItem;
        private ToolStripMenuItem openInSendBoxToolStripMenuItem;
        private ToolStripMenuItem saveSendBoxToolStripMenuItem;
        private ToolStripMenuItem saveReceiveBoxToolStripMenuItem;
        private ToolStripMenuItem manualToolStripMenuItem;
        private ToolStripMenuItem smileyMicrosToolStripMenuItem;
        private ToolStripMenuItem copyToolStripMenuItem1;
        private ToolStripMenuItem cutToolStripMenuItem1;
        private ToolStripMenuItem deleteToolStripMenuItem1;
        private ToolStripMenuItem selectAllToolStripMenuItem1;
        private ToolStripSeparator toolStripSeparator4;
        private ToolStripMenuItem saveFileAsToolStripMenuItem;
        private ContextMenuStrip contextMenuStripReceive;

        // Boolean for the Ring Indicator modem state
        public bool RingIndicator = false;

		#endregion
	
		#region Startup functions

		public MainForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			// Read the terminal data from TerminalData.xml
			dataXMLIO.appPath = System.Environment.CurrentDirectory; 
			xMLData = dataXMLIO.XMLRead();

		}
		
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			base.Dispose( disposing );
		}

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new MainForm());
		}
		
		private void Form1_Load(object sender, System.EventArgs e)
		{
           this.Text = "Smiley Micros COM Port Terminal - Port is closed";	
		   // Start the terminal with default settings
           serialPort1.PortName = portSettings.selectedPort = xMLData.COMPort;                     
           serialPort1.BaudRate = portSettings.selectedBaudrate = xMLData.Baud;
           serialPort1.DataBits = portSettings.selectedDataBits = xMLData.Databits;
 
           string temp = xMLData.Parity;
           switch (temp)
           {
               case "Even":
                   serialPort1.Parity = portSettings.selectedParity = Parity.Even;
                   break;
               case "Mark":
                   serialPort1.Parity = portSettings.selectedParity = Parity.Mark;
                   break;
               case "None":
                   serialPort1.Parity = portSettings.selectedParity = Parity.None;
                   break;
               case "Odd":
                   serialPort1.Parity = portSettings.selectedParity = Parity.Odd;
                   break;
               case "Space":
                   serialPort1.Parity = portSettings.selectedParity = Parity.Space;
                   break;
               default:
                   serialPort1.Parity = portSettings.selectedParity = Parity.None;
                   break;
           }

           temp = xMLData.Stopbits;
           switch (temp)
           {
               case "None":
                   serialPort1.StopBits = portSettings.selectedStopBits = StopBits.None;
                   break;
               case "1":
                   serialPort1.StopBits = portSettings.selectedStopBits = StopBits.One;
                   break;
               //case "1.5": // not supported by FT232R
               //serialPort1.StopBits = = StopBits.OnePointFive;
               //break;
               case "2":
                   serialPort1.StopBits = portSettings.selectedStopBits = StopBits.Two;
                   break;
               default:
                   serialPort1.StopBits = portSettings.selectedStopBits = StopBits.One;
                   break;
           }

           temp = xMLData.Handshake;
           switch (temp)
           {
               case "None":
                   serialPort1.Handshake = portSettings.selectedHandshaking = Handshake.None;
                   break;
               case "RTS/CTS":
                   serialPort1.Handshake = portSettings.selectedHandshaking = Handshake.RequestToSend;
                   break;
               case "Xon/Xoff":
                   serialPort1.Handshake = portSettings.selectedHandshaking = Handshake.XOnXOff;
                   break;
               default:
                   serialPort1.Handshake = portSettings.selectedHandshaking = Handshake.None;
                   break;
           }
	
			// Load the comboBox with 0x00 to 0xFF
			for(int i = 0; i < 16; i++)
			{
				this.comboBoxSendByte.Items.Add("0x0" + i.ToString("X"));
			}
			for(int i = 16; i < 256; i++)
			{
				this.comboBoxSendByte.Items.Add("0x" + i.ToString("X"));
			}
			
			loadXMLData();

            // Set the default LED images
            this.LEDCTS.image = (int)LED.LED22.LEDColor.BlueOn;
            this.LEDDSR.image = (int)LED.LED22.LEDColor.BlueOff;
            this.LEDDCD.image = (int)LED.LED22.LEDColor.BlueOff;
            this.LEDRI.image = (int)LED.LED22.LEDColor.BlueOff;
            this.LEDRTS.image = (int)LED.LED22.LEDColor.RedOn;
            this.LEDDTR.image = (int)LED.LED22.LEDColor.RedOff;

		}
		#endregion

        #region Windows Form Designer generated code
        /// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent() {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.buttonSendQue = new System.Windows.Forms.Button();
            this.comboBoxSendByte = new System.Windows.Forms.ComboBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.groupBoxMacros = new System.Windows.Forms.GroupBox();
            this.label7 = new System.Windows.Forms.Label();
            this.listBoxMacros = new System.Windows.Forms.ListBox();
            this.radioButtonQueued = new System.Windows.Forms.RadioButton();
            this.radioButtonHEX = new System.Windows.Forms.RadioButton();
            this.groupBoxReceive = new System.Windows.Forms.GroupBox();
            this.textBoxReceiveCount = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.richTextBoxReceive = new System.Windows.Forms.RichTextBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.radioButtonASCII = new System.Windows.Forms.RadioButton();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.radioButtonImmediate = new System.Windows.Forms.RadioButton();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.linkLabelVisitWebSite = new System.Windows.Forms.LinkLabel();
            this.richTextBoxSend = new System.Windows.Forms.RichTextBox();
            this.groupBoxSendText = new System.Windows.Forms.GroupBox();
            this.textBoxSendCount = new System.Windows.Forms.TextBox();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.radioButtonTermCharNo = new System.Windows.Forms.RadioButton();
            this.radioButtonTermCharYes = new System.Windows.Forms.RadioButton();
            this.label5 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openInSendBoxToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveSendBoxToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveReceiveBoxToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.settingsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.portToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.sendToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.receiveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openPortToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.sendCharToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.manualToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.smileyMicrosToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.contextMenuStripSend = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.undoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.redoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.cutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.copyToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.pasteToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.deleteToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.selectAllToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.loadFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveAsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.label10 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.timerModemLines = new System.Windows.Forms.Timer(this.components);
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.label15 = new System.Windows.Forms.Label();
            this.buttonDTR = new System.Windows.Forms.Button();
            this.buttonRTS = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.timerSendByte = new System.Windows.Forms.Timer(this.components);
            this.LEDDTR = new LED.LED22();
            this.LEDRTS = new LED.LED22();
            this.LEDRI = new LED.LED22();
            this.LEDCTS = new LED.LED22();
            this.LEDDCD = new LED.LED22();
            this.LEDDSR = new LED.LED22();
            this.copyToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.cutToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.deleteToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.selectAllToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.saveFileAsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.contextMenuStripReceive = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.groupBoxMacros.SuspendLayout();
            this.groupBoxReceive.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBoxSendText.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.contextMenuStripSend.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.contextMenuStripReceive.SuspendLayout();
            this.SuspendLayout();
            // 
            // buttonSendQue
            // 
            this.buttonSendQue.BackColor = System.Drawing.Color.Transparent;
            this.buttonSendQue.Enabled = false;
            this.buttonSendQue.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.buttonSendQue.Location = new System.Drawing.Point(130, 13);
            this.buttonSendQue.Name = "buttonSendQue";
            this.buttonSendQue.Size = new System.Drawing.Size(76, 20);
            this.buttonSendQue.TabIndex = 84;
            this.buttonSendQue.Text = "Send Queue";
            this.buttonSendQue.UseVisualStyleBackColor = false;
            this.buttonSendQue.Click += new System.EventHandler(this.buttonSend_Click);
            // 
            // comboBoxSendByte
            // 
            this.comboBoxSendByte.BackColor = System.Drawing.SystemColors.Window;
            this.comboBoxSendByte.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.comboBoxSendByte.Location = new System.Drawing.Point(63, 11);
            this.comboBoxSendByte.Name = "comboBoxSendByte";
            this.comboBoxSendByte.Size = new System.Drawing.Size(60, 23);
            this.comboBoxSendByte.TabIndex = 97;
            this.comboBoxSendByte.Text = "0x00";
            this.comboBoxSendByte.SelectedIndexChanged += new System.EventHandler(this.comboBoxSendByte_SelectedIndexChanged);
            // 
            // label4
            // 
            this.label4.Font = new System.Drawing.Font("Arial", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(4, 10);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(100, 16);
            this.label4.TabIndex = 100;
            this.label4.Text = "Send Text";
            // 
            // label6
            // 
            this.label6.Font = new System.Drawing.Font("Arial", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(4, 10);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(100, 16);
            this.label6.TabIndex = 101;
            this.label6.Text = "Receive Text";
            // 
            // label1
            // 
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(7, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(64, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "Receive as: ";
            // 
            // label2
            // 
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(4, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(72, 32);
            this.label2.TabIndex = 98;
            this.label2.Text = "Send HEX Immediate:";
            // 
            // label3
            // 
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(1, 17);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(64, 20);
            this.label3.TabIndex = 99;
            this.label3.Text = "Send Text:";
            // 
            // groupBoxMacros
            // 
            this.groupBoxMacros.BackColor = System.Drawing.Color.Transparent;
            this.groupBoxMacros.Controls.Add(this.label7);
            this.groupBoxMacros.Controls.Add(this.listBoxMacros);
            this.groupBoxMacros.Location = new System.Drawing.Point(7, 339);
            this.groupBoxMacros.Name = "groupBoxMacros";
            this.groupBoxMacros.Size = new System.Drawing.Size(560, 118);
            this.groupBoxMacros.TabIndex = 96;
            this.groupBoxMacros.TabStop = false;
            // 
            // label7
            // 
            this.label7.Font = new System.Drawing.Font("Arial", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label7.Location = new System.Drawing.Point(6, 10);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(100, 16);
            this.label7.TabIndex = 102;
            this.label7.Text = "Send Macro";
            // 
            // listBoxMacros
            // 
            this.listBoxMacros.Location = new System.Drawing.Point(5, 29);
            this.listBoxMacros.Name = "listBoxMacros";
            this.listBoxMacros.Size = new System.Drawing.Size(550, 82);
            this.listBoxMacros.TabIndex = 99;
            this.listBoxMacros.DoubleClick += new System.EventHandler(this.ListBoxMacrosDoubleClick);
            // 
            // radioButtonQueued
            // 
            this.radioButtonQueued.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.radioButtonQueued.Location = new System.Drawing.Point(60, 22);
            this.radioButtonQueued.Name = "radioButtonQueued";
            this.radioButtonQueued.Size = new System.Drawing.Size(64, 16);
            this.radioButtonQueued.TabIndex = 96;
            this.radioButtonQueued.Text = "Queued";
            this.radioButtonQueued.CheckedChanged += new System.EventHandler(this.radioButtonQueued_CheckedChanged);
            // 
            // radioButtonHEX
            // 
            this.radioButtonHEX.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.radioButtonHEX.Location = new System.Drawing.Point(131, 15);
            this.radioButtonHEX.Name = "radioButtonHEX";
            this.radioButtonHEX.Size = new System.Drawing.Size(48, 16);
            this.radioButtonHEX.TabIndex = 2;
            this.radioButtonHEX.Text = "HEX";
            this.radioButtonHEX.CheckedChanged += new System.EventHandler(this.radioButtonHEX_CheckedChanged_1);
            // 
            // groupBoxReceive
            // 
            this.groupBoxReceive.BackColor = System.Drawing.Color.Transparent;
            this.groupBoxReceive.Controls.Add(this.textBoxReceiveCount);
            this.groupBoxReceive.Controls.Add(this.label9);
            this.groupBoxReceive.Controls.Add(this.label6);
            this.groupBoxReceive.Controls.Add(this.richTextBoxReceive);
            this.groupBoxReceive.Controls.Add(this.groupBox2);
            this.groupBoxReceive.Location = new System.Drawing.Point(7, 180);
            this.groupBoxReceive.Name = "groupBoxReceive";
            this.groupBoxReceive.Size = new System.Drawing.Size(560, 163);
            this.groupBoxReceive.TabIndex = 87;
            this.groupBoxReceive.TabStop = false;
            // 
            // textBoxReceiveCount
            // 
            this.textBoxReceiveCount.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBoxReceiveCount.Location = new System.Drawing.Point(454, 138);
            this.textBoxReceiveCount.Name = "textBoxReceiveCount";
            this.textBoxReceiveCount.Size = new System.Drawing.Size(100, 20);
            this.textBoxReceiveCount.TabIndex = 104;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label9.Location = new System.Drawing.Point(454, 127);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(67, 12);
            this.label9.TabIndex = 105;
            this.label9.Text = "Receive Count";
            // 
            // richTextBoxReceive
            // 
            this.richTextBoxReceive.EnableAutoDragDrop = true;
            this.richTextBoxReceive.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.richTextBoxReceive.Location = new System.Drawing.Point(4, 26);
            this.richTextBoxReceive.Name = "richTextBoxReceive";
            this.richTextBoxReceive.Size = new System.Drawing.Size(550, 100);
            this.richTextBoxReceive.TabIndex = 5;
            this.richTextBoxReceive.Text = "";
            this.richTextBoxReceive.TextChanged += new System.EventHandler(this.richTextBoxReceive_TextChanged);
            this.richTextBoxReceive.MouseDown += new System.Windows.Forms.MouseEventHandler(this.richTextBoxReceive_MouseDown);
            // 
            // groupBox2
            // 
            this.groupBox2.BackColor = System.Drawing.Color.Transparent;
            this.groupBox2.Controls.Add(this.radioButtonHEX);
            this.groupBox2.Controls.Add(this.radioButtonASCII);
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Location = new System.Drawing.Point(4, 121);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(188, 37);
            this.groupBox2.TabIndex = 98;
            this.groupBox2.TabStop = false;
            // 
            // radioButtonASCII
            // 
            this.radioButtonASCII.Checked = true;
            this.radioButtonASCII.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.radioButtonASCII.Location = new System.Drawing.Point(75, 15);
            this.radioButtonASCII.Name = "radioButtonASCII";
            this.radioButtonASCII.Size = new System.Drawing.Size(56, 16);
            this.radioButtonASCII.TabIndex = 1;
            this.radioButtonASCII.TabStop = true;
            this.radioButtonASCII.Text = "ASCII";
            this.radioButtonASCII.CheckedChanged += new System.EventHandler(this.radioButtonASCII_CheckedChanged_1);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.buttonSendQue);
            this.groupBox1.Controls.Add(this.radioButtonImmediate);
            this.groupBox1.Controls.Add(this.radioButtonQueued);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Location = new System.Drawing.Point(135, 122);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(210, 38);
            this.groupBox1.TabIndex = 96;
            this.groupBox1.TabStop = false;
            // 
            // radioButtonImmediate
            // 
            this.radioButtonImmediate.Checked = true;
            this.radioButtonImmediate.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.radioButtonImmediate.Location = new System.Drawing.Point(60, 7);
            this.radioButtonImmediate.Name = "radioButtonImmediate";
            this.radioButtonImmediate.Size = new System.Drawing.Size(76, 16);
            this.radioButtonImmediate.TabIndex = 95;
            this.radioButtonImmediate.TabStop = true;
            this.radioButtonImmediate.Text = "Immediate";
            this.radioButtonImmediate.CheckedChanged += new System.EventHandler(this.radioButtonImmediate_CheckedChanged);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.comboBoxSendByte);
            this.groupBox3.Controls.Add(this.label2);
            this.groupBox3.Location = new System.Drawing.Point(4, 122);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(130, 38);
            this.groupBox3.TabIndex = 99;
            this.groupBox3.TabStop = false;
            // 
            // linkLabelVisitWebSite
            // 
            this.linkLabelVisitWebSite.BackColor = System.Drawing.Color.PeachPuff;
            this.linkLabelVisitWebSite.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.linkLabelVisitWebSite.Location = new System.Drawing.Point(446, 4);
            this.linkLabelVisitWebSite.Name = "linkLabelVisitWebSite";
            this.linkLabelVisitWebSite.Size = new System.Drawing.Size(136, 20);
            this.linkLabelVisitWebSite.TabIndex = 3;
            this.linkLabelVisitWebSite.TabStop = true;
            this.linkLabelVisitWebSite.Text = "www.SmileyMicros.com";
            this.linkLabelVisitWebSite.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabelVisitWebSite_LinkClicked);
            // 
            // richTextBoxSend
            // 
            this.richTextBoxSend.EnableAutoDragDrop = true;
            this.richTextBoxSend.Location = new System.Drawing.Point(5, 26);
            this.richTextBoxSend.Name = "richTextBoxSend";
            this.richTextBoxSend.Size = new System.Drawing.Size(550, 100);
            this.richTextBoxSend.TabIndex = 0;
            this.richTextBoxSend.Text = "";
            this.richTextBoxSend.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.richTextBoxSend_KeyPress);
            this.richTextBoxSend.MouseDown += new System.Windows.Forms.MouseEventHandler(this.richTextBoxSend_MouseDown);
            // 
            // groupBoxSendText
            // 
            this.groupBoxSendText.BackColor = System.Drawing.Color.Transparent;
            this.groupBoxSendText.Controls.Add(this.textBoxSendCount);
            this.groupBoxSendText.Controls.Add(this.richTextBoxSend);
            this.groupBoxSendText.Controls.Add(this.label4);
            this.groupBoxSendText.Controls.Add(this.groupBox5);
            this.groupBoxSendText.Controls.Add(this.label8);
            this.groupBoxSendText.Controls.Add(this.groupBox3);
            this.groupBoxSendText.Controls.Add(this.groupBox1);
            this.groupBoxSendText.Location = new System.Drawing.Point(7, 21);
            this.groupBoxSendText.Name = "groupBoxSendText";
            this.groupBoxSendText.Size = new System.Drawing.Size(560, 163);
            this.groupBoxSendText.TabIndex = 86;
            this.groupBoxSendText.TabStop = false;
            // 
            // textBoxSendCount
            // 
            this.textBoxSendCount.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBoxSendCount.Location = new System.Drawing.Point(455, 139);
            this.textBoxSendCount.Name = "textBoxSendCount";
            this.textBoxSendCount.Size = new System.Drawing.Size(100, 20);
            this.textBoxSendCount.TabIndex = 102;
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.radioButtonTermCharNo);
            this.groupBox5.Controls.Add(this.radioButtonTermCharYes);
            this.groupBox5.Controls.Add(this.label5);
            this.groupBox5.Location = new System.Drawing.Point(347, 121);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(103, 38);
            this.groupBox5.TabIndex = 101;
            this.groupBox5.TabStop = false;
            // 
            // radioButtonTermCharNo
            // 
            this.radioButtonTermCharNo.AutoSize = true;
            this.radioButtonTermCharNo.Checked = true;
            this.radioButtonTermCharNo.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.radioButtonTermCharNo.Location = new System.Drawing.Point(61, 22);
            this.radioButtonTermCharNo.Name = "radioButtonTermCharNo";
            this.radioButtonTermCharNo.Size = new System.Drawing.Size(39, 17);
            this.radioButtonTermCharNo.TabIndex = 1;
            this.radioButtonTermCharNo.TabStop = true;
            this.radioButtonTermCharNo.Text = "No";
            this.radioButtonTermCharNo.UseVisualStyleBackColor = true;
            this.radioButtonTermCharNo.CheckedChanged += new System.EventHandler(this.radioButtonTermCharNo_CheckedChanged);
            // 
            // radioButtonTermCharYes
            // 
            this.radioButtonTermCharYes.AutoSize = true;
            this.radioButtonTermCharYes.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.radioButtonTermCharYes.Location = new System.Drawing.Point(61, 6);
            this.radioButtonTermCharYes.Name = "radioButtonTermCharYes";
            this.radioButtonTermCharYes.Size = new System.Drawing.Size(43, 17);
            this.radioButtonTermCharYes.TabIndex = 0;
            this.radioButtonTermCharYes.Text = "Yes";
            this.radioButtonTermCharYes.UseVisualStyleBackColor = true;
            this.radioButtonTermCharYes.CheckedChanged += new System.EventHandler(this.radioButtonTermCharYes_CheckedChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(-1, 16);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(63, 13);
            this.label5.TabIndex = 2;
            this.label5.Text = "Terminator?";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label8.Location = new System.Drawing.Point(455, 128);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(53, 12);
            this.label8.TabIndex = 103;
            this.label8.Text = "Send Count";
            // 
            // menuStrip1
            // 
            this.menuStrip1.BackColor = System.Drawing.Color.PeachPuff;
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.settingsToolStripMenuItem,
            this.openPortToolStripMenuItem,
            this.toolsToolStripMenuItem,
            this.helpToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(575, 24);
            this.menuStrip1.TabIndex = 100;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openInSendBoxToolStripMenuItem,
            this.saveSendBoxToolStripMenuItem,
            this.saveReceiveBoxToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // openInSendBoxToolStripMenuItem
            // 
            this.openInSendBoxToolStripMenuItem.Name = "openInSendBoxToolStripMenuItem";
            this.openInSendBoxToolStripMenuItem.Size = new System.Drawing.Size(160, 22);
            this.openInSendBoxToolStripMenuItem.Text = "Open in Send Box";
            this.openInSendBoxToolStripMenuItem.Click += new System.EventHandler(this.openInSendBoxToolStripMenuItem_Click_1);
            // 
            // saveSendBoxToolStripMenuItem
            // 
            this.saveSendBoxToolStripMenuItem.Name = "saveSendBoxToolStripMenuItem";
            this.saveSendBoxToolStripMenuItem.Size = new System.Drawing.Size(160, 22);
            this.saveSendBoxToolStripMenuItem.Text = "Save Send Box";
            this.saveSendBoxToolStripMenuItem.Click += new System.EventHandler(this.saveSendBoxToolStripMenuItem_Click_1);
            // 
            // saveReceiveBoxToolStripMenuItem
            // 
            this.saveReceiveBoxToolStripMenuItem.Name = "saveReceiveBoxToolStripMenuItem";
            this.saveReceiveBoxToolStripMenuItem.Size = new System.Drawing.Size(160, 22);
            this.saveReceiveBoxToolStripMenuItem.Text = "Save Receive Box";
            this.saveReceiveBoxToolStripMenuItem.Click += new System.EventHandler(this.saveReceiveBoxToolStripMenuItem_Click_1);
            // 
            // settingsToolStripMenuItem
            // 
            this.settingsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.portToolStripMenuItem,
            this.sendToolStripMenuItem,
            this.receiveToolStripMenuItem});
            this.settingsToolStripMenuItem.Name = "settingsToolStripMenuItem";
            this.settingsToolStripMenuItem.Size = new System.Drawing.Size(58, 20);
            this.settingsToolStripMenuItem.Text = "Settings";
            // 
            // portToolStripMenuItem
            // 
            this.portToolStripMenuItem.Name = "portToolStripMenuItem";
            this.portToolStripMenuItem.Size = new System.Drawing.Size(205, 22);
            this.portToolStripMenuItem.Text = "Port";
            this.portToolStripMenuItem.Click += new System.EventHandler(this.portToolStripMenuItem_Click);
            // 
            // sendToolStripMenuItem
            // 
            this.sendToolStripMenuItem.Name = "sendToolStripMenuItem";
            this.sendToolStripMenuItem.Size = new System.Drawing.Size(205, 22);
            this.sendToolStripMenuItem.Text = "Send textBox Properties";
            this.sendToolStripMenuItem.Click += new System.EventHandler(this.sendToolStripMenuItem_Click);
            // 
            // receiveToolStripMenuItem
            // 
            this.receiveToolStripMenuItem.Name = "receiveToolStripMenuItem";
            this.receiveToolStripMenuItem.Size = new System.Drawing.Size(205, 22);
            this.receiveToolStripMenuItem.Text = "Receive textBox Properties";
            this.receiveToolStripMenuItem.Click += new System.EventHandler(this.receiveToolStripMenuItem_Click);
            // 
            // openPortToolStripMenuItem
            // 
            this.openPortToolStripMenuItem.Name = "openPortToolStripMenuItem";
            this.openPortToolStripMenuItem.Size = new System.Drawing.Size(68, 20);
            this.openPortToolStripMenuItem.Text = "Open Port";
            this.openPortToolStripMenuItem.Click += new System.EventHandler(this.openPortToolStripMenuItem_Click);
            // 
            // toolsToolStripMenuItem
            // 
            this.toolsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.sendCharToolStripMenuItem});
            this.toolsToolStripMenuItem.Name = "toolsToolStripMenuItem";
            this.toolsToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
            this.toolsToolStripMenuItem.Text = "Tools";
            // 
            // sendCharToolStripMenuItem
            // 
            this.sendCharToolStripMenuItem.Name = "sendCharToolStripMenuItem";
            this.sendCharToolStripMenuItem.Size = new System.Drawing.Size(179, 22);
            this.sendCharToolStripMenuItem.Text = "Send Byte Periodically";
            this.sendCharToolStripMenuItem.Click += new System.EventHandler(this.sendCharToolStripMenuItem_Click);
            // 
            // helpToolStripMenuItem
            // 
            this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.manualToolStripMenuItem,
            this.smileyMicrosToolStripMenuItem});
            this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            this.helpToolStripMenuItem.Size = new System.Drawing.Size(40, 20);
            this.helpToolStripMenuItem.Text = "Help";
            // 
            // manualToolStripMenuItem
            // 
            this.manualToolStripMenuItem.Name = "manualToolStripMenuItem";
            this.manualToolStripMenuItem.Size = new System.Drawing.Size(137, 22);
            this.manualToolStripMenuItem.Text = "Manual";
            this.manualToolStripMenuItem.Click += new System.EventHandler(this.manualToolStripMenuItem_Click);
            // 
            // smileyMicrosToolStripMenuItem
            // 
            this.smileyMicrosToolStripMenuItem.Name = "smileyMicrosToolStripMenuItem";
            this.smileyMicrosToolStripMenuItem.Size = new System.Drawing.Size(137, 22);
            this.smileyMicrosToolStripMenuItem.Text = "Smiley Micros";
            this.smileyMicrosToolStripMenuItem.Click += new System.EventHandler(this.smileyMicrosToolStripMenuItem_Click);
            // 
            // contextMenuStripSend
            // 
            this.contextMenuStripSend.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.undoToolStripMenuItem,
            this.redoToolStripMenuItem,
            this.toolStripSeparator1,
            this.cutToolStripMenuItem,
            this.copyToolStripMenuItem,
            this.pasteToolStripMenuItem,
            this.deleteToolStripMenuItem,
            this.selectAllToolStripMenuItem,
            this.toolStripSeparator2,
            this.loadFileToolStripMenuItem,
            this.saveAsToolStripMenuItem});
            this.contextMenuStripSend.Name = "contextMenuStripSend";
            this.contextMenuStripSend.Size = new System.Drawing.Size(133, 214);
            // 
            // undoToolStripMenuItem
            // 
            this.undoToolStripMenuItem.Name = "undoToolStripMenuItem";
            this.undoToolStripMenuItem.Size = new System.Drawing.Size(132, 22);
            this.undoToolStripMenuItem.Text = "Undo";
            this.undoToolStripMenuItem.Click += new System.EventHandler(this.undoToolStripMenuItem_Click);
            // 
            // redoToolStripMenuItem
            // 
            this.redoToolStripMenuItem.Name = "redoToolStripMenuItem";
            this.redoToolStripMenuItem.Size = new System.Drawing.Size(132, 22);
            this.redoToolStripMenuItem.Text = "Redo";
            this.redoToolStripMenuItem.Click += new System.EventHandler(this.redoToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(129, 6);
            // 
            // cutToolStripMenuItem
            // 
            this.cutToolStripMenuItem.Name = "cutToolStripMenuItem";
            this.cutToolStripMenuItem.Size = new System.Drawing.Size(132, 22);
            this.cutToolStripMenuItem.Text = "Cut";
            this.cutToolStripMenuItem.Click += new System.EventHandler(this.cutToolStripMenuItem_Click);
            // 
            // copyToolStripMenuItem
            // 
            this.copyToolStripMenuItem.Name = "copyToolStripMenuItem";
            this.copyToolStripMenuItem.Size = new System.Drawing.Size(132, 22);
            this.copyToolStripMenuItem.Text = "Copy";
            this.copyToolStripMenuItem.Click += new System.EventHandler(this.copyToolStripMenuItem_Click);
            // 
            // pasteToolStripMenuItem
            // 
            this.pasteToolStripMenuItem.Name = "pasteToolStripMenuItem";
            this.pasteToolStripMenuItem.Size = new System.Drawing.Size(132, 22);
            this.pasteToolStripMenuItem.Text = "Paste";
            this.pasteToolStripMenuItem.Click += new System.EventHandler(this.pasteToolStripMenuItem_Click);
            // 
            // deleteToolStripMenuItem
            // 
            this.deleteToolStripMenuItem.Name = "deleteToolStripMenuItem";
            this.deleteToolStripMenuItem.Size = new System.Drawing.Size(132, 22);
            this.deleteToolStripMenuItem.Text = "Clear";
            this.deleteToolStripMenuItem.Click += new System.EventHandler(this.deleteToolStripMenuItem_Click);
            // 
            // selectAllToolStripMenuItem
            // 
            this.selectAllToolStripMenuItem.Name = "selectAllToolStripMenuItem";
            this.selectAllToolStripMenuItem.Size = new System.Drawing.Size(132, 22);
            this.selectAllToolStripMenuItem.Text = "Select All";
            this.selectAllToolStripMenuItem.Click += new System.EventHandler(this.selectAllToolStripMenuItem_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(129, 6);
            // 
            // loadFileToolStripMenuItem
            // 
            this.loadFileToolStripMenuItem.Name = "loadFileToolStripMenuItem";
            this.loadFileToolStripMenuItem.Size = new System.Drawing.Size(132, 22);
            this.loadFileToolStripMenuItem.Text = "Load File";
            this.loadFileToolStripMenuItem.Click += new System.EventHandler(this.loadFileToolStripMenuItem_Click);
            // 
            // saveAsToolStripMenuItem
            // 
            this.saveAsToolStripMenuItem.Name = "saveAsToolStripMenuItem";
            this.saveAsToolStripMenuItem.Size = new System.Drawing.Size(132, 22);
            this.saveAsToolStripMenuItem.Text = "Save File As";
            this.saveAsToolStripMenuItem.Click += new System.EventHandler(this.saveAsToolStripMenuItem_Click);
            // 
            // serialPort1
            // 
            this.serialPort1.RtsEnable = true;
            this.serialPort1.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serialPort1_DataReceived);
            this.serialPort1.PinChanged += new System.IO.Ports.SerialPinChangedEventHandler(this.serialPort1_PinChanged);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(94, 13);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(28, 13);
            this.label10.TabIndex = 103;
            this.label10.Text = "CTS";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(124, 13);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(30, 13);
            this.label11.TabIndex = 104;
            this.label11.Text = "DSR";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(155, 13);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(30, 13);
            this.label12.TabIndex = 105;
            this.label12.Text = "DCD";
            // 
            // timerModemLines
            // 
            this.timerModemLines.Enabled = true;
            this.timerModemLines.Interval = 500;
            this.timerModemLines.Tick += new System.EventHandler(this.timerOneSec_Tick);
            // 
            // groupBox4
            // 
            this.groupBox4.BackColor = System.Drawing.Color.Bisque;
            this.groupBox4.Controls.Add(this.LEDDTR);
            this.groupBox4.Controls.Add(this.LEDRTS);
            this.groupBox4.Controls.Add(this.LEDRI);
            this.groupBox4.Controls.Add(this.label15);
            this.groupBox4.Controls.Add(this.buttonDTR);
            this.groupBox4.Controls.Add(this.buttonRTS);
            this.groupBox4.Controls.Add(this.LEDCTS);
            this.groupBox4.Controls.Add(this.label10);
            this.groupBox4.Controls.Add(this.label11);
            this.groupBox4.Controls.Add(this.label12);
            this.groupBox4.Controls.Add(this.LEDDCD);
            this.groupBox4.Controls.Add(this.LEDDSR);
            this.groupBox4.Location = new System.Drawing.Point(7, 453);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(217, 51);
            this.groupBox4.TabIndex = 110;
            this.groupBox4.TabStop = false;
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(190, 14);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(18, 13);
            this.label15.TabIndex = 112;
            this.label15.Text = "RI";
            // 
            // buttonDTR
            // 
            this.buttonDTR.Location = new System.Drawing.Point(51, 27);
            this.buttonDTR.Name = "buttonDTR";
            this.buttonDTR.Size = new System.Drawing.Size(38, 21);
            this.buttonDTR.TabIndex = 111;
            this.buttonDTR.Text = "DTR";
            this.buttonDTR.UseVisualStyleBackColor = true;
            this.buttonDTR.Click += new System.EventHandler(this.buttonDTR_Click);
            // 
            // buttonRTS
            // 
            this.buttonRTS.Location = new System.Drawing.Point(9, 27);
            this.buttonRTS.Name = "buttonRTS";
            this.buttonRTS.Size = new System.Drawing.Size(38, 21);
            this.buttonRTS.TabIndex = 110;
            this.buttonRTS.Text = "RTS";
            this.buttonRTS.UseVisualStyleBackColor = true;
            this.buttonRTS.Click += new System.EventHandler(this.buttonRTS_Click);
            // 
            // panel1
            // 
            this.panel1.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("panel1.BackgroundImage")));
            this.panel1.Location = new System.Drawing.Point(328, 461);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(200, 40);
            this.panel1.TabIndex = 111;
            this.panel1.MouseLeave += new System.EventHandler(this.panel1_MouseLeave);
            this.panel1.MouseClick += new System.Windows.Forms.MouseEventHandler(this.panel1_MouseClick);
            this.panel1.MouseEnter += new System.EventHandler(this.panel1_MouseEnter);
            // 
            // timerSendByte
            // 
            this.timerSendByte.Tick += new System.EventHandler(this.timerSendByte_Tick);
            // 
            // LEDDTR
            // 
            this.LEDDTR.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("LEDDTR.BackgroundImage")));
            this.LEDDTR.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.LEDDTR.image = 0;
            this.LEDDTR.Location = new System.Drawing.Point(63, 13);
            this.LEDDTR.Name = "LEDDTR";
            this.LEDDTR.Size = new System.Drawing.Size(12, 12);
            this.LEDDTR.TabIndex = 115;
            // 
            // LEDRTS
            // 
            this.LEDRTS.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("LEDRTS.BackgroundImage")));
            this.LEDRTS.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.LEDRTS.image = 0;
            this.LEDRTS.Location = new System.Drawing.Point(22, 13);
            this.LEDRTS.Name = "LEDRTS";
            this.LEDRTS.Size = new System.Drawing.Size(12, 12);
            this.LEDRTS.TabIndex = 114;
            // 
            // LEDRI
            // 
            this.LEDRI.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("LEDRI.BackgroundImage")));
            this.LEDRI.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.LEDRI.image = 0;
            this.LEDRI.Location = new System.Drawing.Point(192, 30);
            this.LEDRI.Name = "LEDRI";
            this.LEDRI.Size = new System.Drawing.Size(12, 12);
            this.LEDRI.TabIndex = 113;
            // 
            // LEDCTS
            // 
            this.LEDCTS.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("LEDCTS.BackgroundImage")));
            this.LEDCTS.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.LEDCTS.image = 0;
            this.LEDCTS.Location = new System.Drawing.Point(102, 30);
            this.LEDCTS.Name = "LEDCTS";
            this.LEDCTS.Size = new System.Drawing.Size(12, 12);
            this.LEDCTS.TabIndex = 101;
            // 
            // LEDDCD
            // 
            this.LEDDCD.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("LEDDCD.BackgroundImage")));
            this.LEDDCD.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.LEDDCD.image = 0;
            this.LEDDCD.Location = new System.Drawing.Point(162, 30);
            this.LEDDCD.Name = "LEDDCD";
            this.LEDDCD.Size = new System.Drawing.Size(12, 12);
            this.LEDDCD.TabIndex = 107;
            // 
            // LEDDSR
            // 
            this.LEDDSR.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("LEDDSR.BackgroundImage")));
            this.LEDDSR.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.LEDDSR.image = 0;
            this.LEDDSR.Location = new System.Drawing.Point(132, 30);
            this.LEDDSR.Name = "LEDDSR";
            this.LEDDSR.Size = new System.Drawing.Size(12, 12);
            this.LEDDSR.TabIndex = 106;
            // 
            // copyToolStripMenuItem1
            // 
            this.copyToolStripMenuItem1.Name = "copyToolStripMenuItem1";
            this.copyToolStripMenuItem1.Size = new System.Drawing.Size(132, 22);
            this.copyToolStripMenuItem1.Text = "Copy";
            this.copyToolStripMenuItem1.Click += new System.EventHandler(this.copyToolStripMenuItem1_Click);
            // 
            // cutToolStripMenuItem1
            // 
            this.cutToolStripMenuItem1.Name = "cutToolStripMenuItem1";
            this.cutToolStripMenuItem1.Size = new System.Drawing.Size(132, 22);
            this.cutToolStripMenuItem1.Text = "Cut";
            this.cutToolStripMenuItem1.Click += new System.EventHandler(this.cutToolStripMenuItem1_Click);
            // 
            // deleteToolStripMenuItem1
            // 
            this.deleteToolStripMenuItem1.Name = "deleteToolStripMenuItem1";
            this.deleteToolStripMenuItem1.Size = new System.Drawing.Size(132, 22);
            this.deleteToolStripMenuItem1.Text = "Clear";
            this.deleteToolStripMenuItem1.Click += new System.EventHandler(this.deleteToolStripMenuItem1_Click);
            // 
            // selectAllToolStripMenuItem1
            // 
            this.selectAllToolStripMenuItem1.Name = "selectAllToolStripMenuItem1";
            this.selectAllToolStripMenuItem1.Size = new System.Drawing.Size(132, 22);
            this.selectAllToolStripMenuItem1.Text = "Select All";
            this.selectAllToolStripMenuItem1.Click += new System.EventHandler(this.selectAllToolStripMenuItem1_Click);
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(129, 6);
            // 
            // saveFileAsToolStripMenuItem
            // 
            this.saveFileAsToolStripMenuItem.Name = "saveFileAsToolStripMenuItem";
            this.saveFileAsToolStripMenuItem.Size = new System.Drawing.Size(132, 22);
            this.saveFileAsToolStripMenuItem.Text = "Save File As";
            this.saveFileAsToolStripMenuItem.Click += new System.EventHandler(this.saveFileAsToolStripMenuItem_Click);
            // 
            // contextMenuStripReceive
            // 
            this.contextMenuStripReceive.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.copyToolStripMenuItem1,
            this.cutToolStripMenuItem1,
            this.deleteToolStripMenuItem1,
            this.selectAllToolStripMenuItem1,
            this.toolStripSeparator4,
            this.saveFileAsToolStripMenuItem});
            this.contextMenuStripReceive.Name = "contextMenuStripReceive";
            this.contextMenuStripReceive.Size = new System.Drawing.Size(133, 120);
            // 
            // MainForm
            // 
            this.AllowDrop = true;
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.BackColor = System.Drawing.Color.Bisque;
            this.ClientSize = new System.Drawing.Size(575, 507);
            this.Controls.Add(this.linkLabelVisitWebSite);
            this.Controls.Add(this.menuStrip1);
            this.Controls.Add(this.groupBoxSendText);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.groupBoxReceive);
            this.Controls.Add(this.groupBoxMacros);
            this.Controls.Add(this.groupBox4);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.MaximizeBox = false;
            this.Name = "MainForm";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Developer\'s Terminal";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.groupBoxMacros.ResumeLayout(false);
            this.groupBoxReceive.ResumeLayout(false);
            this.groupBoxReceive.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBoxSendText.ResumeLayout(false);
            this.groupBoxSendText.PerformLayout();
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.contextMenuStripSend.ResumeLayout(false);
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.contextMenuStripReceive.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

		}
		#endregion

        #endregion

        #region GUI

        #region Open REALLY GREAT website

        // And this is not shameless commercialism. I have NO shame.
        // And2 - now you know how to do it.
		private void linkLabelVisitWebSite_LinkClicked(object sender, System.Windows.Forms.LinkLabelLinkClickedEventArgs e)
		{
			System.Diagnostics.Process.Start("www.smileymicros.com");		
		}

        // Open website 
        private void panel1_MouseClick(object sender, MouseEventArgs e)
        {
            System.Diagnostics.Process.Start("www.smileymicros.com");
        }

        // Show hand cursor and change color to intice click to website
        private void panel1_MouseEnter(object sender, EventArgs e)
        {
            panel1.Cursor = Cursors.Hand;
            panel1.BackColor = Color.SandyBrown;
        }

        // Cursor reverts on leaving, but color doesn't so revert it
        private void panel1_MouseLeave(object sender, EventArgs e)
        {
            panel1.BackColor = Color.Bisque;
        }

        #endregion
        
        #region Buttons   
        private void buttonClear_Click(object sender, System.EventArgs e)
		{
			richTextBoxReceive.Text = "";
		}

        private void buttonClearSend_Click(object sender, EventArgs e)
        {
            richTextBoxSend.Text = "";
        }
		
		private void buttonSettings_Click(object sender, System.EventArgs e)
		{
            PortSettings portSettings = new PortSettings();

            portSettings.ShowDialog();
            if (portSettings.DialogResult == DialogResult.OK)
            {
                serialPort1.PortName = portSettings.selectedPort;
                serialPort1.BaudRate = portSettings.selectedBaudrate;
            }
		}

		private void buttonSend_Click(object sender, System.EventArgs e)
		{
			richTextBoxSend.Text += "\n Sent QueuedString: \n" + QueuedString + "\n";
            SendASCIIString(QueuedString);
            if (UseTermChar) sendTermChar();
			QueuedString = "";
		}		
		
		private void buttonOpenClosePort_Click(object sender, System.EventArgs e)
		{
			if(PortOpen)
			{
				PortOpen = false;
                serialPort1.Close();
                this.Text = "Smiley Micros COM Port Terminal - Port is closed";
			}
			else
			{
				PortOpen = true;
 
                try
                {
                    serialPort1.Open();
                 }
                catch(System.Exception ex)
                {
                    MessageBox.Show("Error - buttonOpenClosePort_ClickSystem.Exception: " + ex);
                }
				showSettings();		
			}
		}
		
		private void buttonHelp_Click(object sender, System.EventArgs e)
		{
			MessageBox.Show("HELP! is coming soon to a computer near you...");
		}
		
		void ButtonReloadMacrosClick(object sender, System.EventArgs e)
		{
			xMLData = dataXMLIO.XMLRead();
			listBoxMacros.Items.Clear();
			loadXMLData();
		}	
		#endregion
		
		#region Check Box and Radio Buttons

		private void radioButtonImmediate_CheckedChanged(object sender, System.EventArgs e)
		{
			if(radioButtonImmediate.Checked == true) 
			{
				SendImmediate = true;
				buttonSendQue.Enabled = false;
			}
		}

		private void radioButtonQueued_CheckedChanged(object sender, System.EventArgs e)
		{
			if(radioButtonQueued.Checked == true) 
			{
				SendImmediate = false;
				buttonSendQue.Enabled = true;
			}
		}

		private void radioButtonASCII_CheckedChanged_1(object sender, System.EventArgs e)
		{
			ReceiveASCII = true;
		}

		private void radioButtonHEX_CheckedChanged_1(object sender, System.EventArgs e)
		{
			ReceiveASCII = false;
		}

        private void radioButtonTermCharYes_CheckedChanged(object sender, EventArgs e)
        {
            if (radioButtonTermCharYes.Checked == true)
            {
                UseTermChar = true;
            }
        }

        private void radioButtonTermCharNo_CheckedChanged(object sender, EventArgs e)
        {
            if (radioButtonTermCharNo.Checked == true)
            {
                UseTermChar = false;
            }
        }

		#endregion

        #endregion

        #region Receive functions

        // we want to have the serial port thread report back data received, but to display
        // that data we must create a delegate function to show the data in the richTextBox

        // define the delegate 
        public delegate void SetText();
        // define an instance of the delegate
        SetText setText;

        // create a string that will be loaded with the data received from the port
        public string str = "";
        // JWP 4/28/07 added public byt for receiving bytes
        public byte byt = 0;

        // note that this function runs in a separate thread and thus we must use a delegate in order
        // to display the results in the richTextBox.
        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            // instantiate the delegate to be invoked by this thread
            setText = new SetText(mySetText);

            // load the data into the string
            try
            {
                // JWP 4/28/07 added ReceiveASCII and byte reception
                if (ReceiveASCII)
                {
                    str = serialPort1.ReadExisting();
                }
                else
                {
                    byt = (byte)serialPort1.ReadByte();// .ReadExisting();
                }
            }
            catch (System.Exception ex)
            {
                MessageBox.Show("Error - port_DataReceived Exception: " + ex);
            }

            // invoke the delegate in the MainForm thread
            this.Invoke(setText);
        }

        // create the instance of the delegate to be used to write the received data to the richTextBox
        public void mySetText()
        {
            ReceiveCount += str.Length;
            textBoxReceiveCount.Text = ReceiveCount.ToString();

            if (ReceiveASCII)
            {
                richTextBoxReceive.Text += str.ToString();
            }
            else // ReceiveHEX
            {
                // JWP 4/28/07 changed byte display
                /*foreach (byte b in str)
                {
                    if (b > 15)
                    {
                        richTextBoxReceive.Text += "0x" + b.ToString("X") + ",";
                    }
                    else richTextBoxReceive.Text += "0x0" + b.ToString("X") + ",";
                }*/
                if (byt > 15)
                {
                    richTextBoxReceive.Text += "0x" + byt.ToString("X") + ",";
                }
                else richTextBoxReceive.Text += "0x0" + byt.ToString("X") + ",";

            }

        }

        // This rigaramole is needed to keep the last received item displayed
        // it kind of flickers and should be fixed
        private void richTextBoxReceive_TextChanged(object sender, System.EventArgs e)
        {
            moveCaretToEnd();
        }

        private void moveCaretToEnd()
        {
            richTextBoxReceive.SelectionStart = richTextBoxReceive.Text.Length;
            richTextBoxReceive.SelectionLength = 0;
            richTextBoxReceive.ScrollToCaret();
        }

       #endregion

		#region Send Text functions
        private void richTextBoxSend_KeyPress(object sender, KeyPressEventArgs e)
        {
			if(SendImmediate)
			{
                sendChar(e.KeyChar);
				if(UseTermChar) sendTermChar();
			}
			else
			{
				QueuedString += e.KeyChar;
			}
		}

        private void sendTermChar()
        {
            SendHEXString(xMLData.TerminalCharacters);
        }

        private void sendChar(char c)
        {
            char[] data = new Char[1];
            data[0] = c;
            try
            {
                serialPort1.Write(data, 0, 1);
                SendCount++;
                textBoxSendCount.Text = SendCount.ToString();
                // Show bytes sent by shooter
                if (shoot) richTextBoxSend.Text += c;
            }
            catch
            {
                MessageBox.Show("Error: sendChar - failed to send.\nIs the port open?");
            }
        }

        //JWP 4/28/07 - add sendByte function
        private void sendByte(byte b)
        {
            byte[] data = new byte[1];
            data[0] = b;
            try
            {
                serialPort1.Write(data, 0, 1);
                SendCount++;
                textBoxSendCount.Text = SendCount.ToString();
                // Show bytes sent by shooter
                //JWP 4/28/07 added show with 0x or 0x0
                if (shoot) //richTextBoxSend.Text += b.ToString();
                {
                    if (byt > 15)
                    {
                        richTextBoxSend.Text += "0x" + b.ToString("X") + ",";
                    }
                    else richTextBoxSend.Text += "0x0" + b.ToString("X") + ",";
                }
            }
            catch
            {
                MessageBox.Show("Error: sendChar - failed to send.\nIs the port open?");
            }
        }

		// Send single hex bytes immediately
		private void comboBoxSendByte_SelectedIndexChanged(object sender, System.EventArgs e)
		{
            //JWP 4/28/07 - changed from sendChar to sendByte
            //sendChar((char)comboBoxSendByte.SelectedIndex);
            sendByte((byte)comboBoxSendByte.SelectedIndex);
            this.richTextBoxSend.Text = "Sent: 0x" + comboBoxSendByte.SelectedIndex.ToString("X");
		}



        #endregion
        
        #region Send Macro Functions
        // Send Macro Stings
        void ListBoxMacrosDoubleClick(object sender, System.EventArgs e)
		{
			processMacroClick(MacroDataData[listBoxMacros.SelectedIndex],MacroDataType[listBoxMacros.SelectedIndex]);
		}
		private void processMacroClick(string MacroData, string MacroType)
		{
			richTextBoxSend.Text = MacroData;
			if(MacroType == "ASCII")
			{
				SendASCIIString(MacroData);
                if (UseTermChar) sendTermChar();
			}
			else if (MacroType == "HEX")
			{
				SendHEXString(MacroData);
                if (UseTermChar) sendTermChar();
			}
			else MessageBox.Show("Error: MacroType: " + MacroType);
		}

        public void SendASCIIString(string str)
        {
            char[] c = str.ToCharArray();

            for (int i = 0; i < c.GetLength(0); i++)
            {
                sendChar(c[i]);
            }
        }

        // For our purposes, HEX strings must be in the format 0xFF,0xFE, 
        // including the trailing comma on the last value
        public void SendHEXString(string str)
        {
            char[] c = str.ToCharArray();
            byte b = 0;

            if (c.GetLength(0) < 5)
            {
                MessageBox.Show("Error: Hex string incorrect length: " + str + "\nLength = " + c.GetLength(0).ToString());
                return;
            }
            for (int i = 0; i < c.GetLength(0); i += 5)
            {
                // Check array length
                if (i > c.GetLength(0))
                {
                    MessageBox.Show("Error: Hex string incorrect length: " + str + "\nLength = " + c.GetLength(0).ToString() + "\ni = " + i.ToString());
                    return;
                }
                // Check for remaining length 
                if (i + 4 >= c.GetLength(0))
                {
                    MessageBox.Show("Error: Hex string: " + str + " problem with length");
                    return;
                }
                // Check format trailing comma
                if (c[i + 4] != ',')
                {
                    MessageBox.Show("Error: Hex string: " + str + " lacks a trailing comma.");
                    return;
                }
                // Check high nibble range 0 - F
                if ((c[i + 2] >= '0') && (c[i + 2] <= '9') && (c[i + 2] >= 'A') && (c[i + 2] <= 'F'))
                {
                    MessageBox.Show("Error: Hex string first digit: " + c[i + 2].ToString() + " not in range 0 - F");
                    return;
                }
                // Check low nibble range 0 - F
                if ((c[i + 3] >= '0') && (c[i + 3] <= '9') && (c[i + 3] >= 'A') && (c[i + 3] <= 'F'))
                {
                    MessageBox.Show("Error: Hex string first digit: " + c[i + 2].ToString() + " not in range 0 - F");
                    return;
                }
                if (c[i + 2] <= '9') b = (byte)(c[i + 2] - '0');
                else
                {
                    b = (byte)(c[i + 2] - 'A');
                    b += 10;
                }

                b = (byte)(b << 4);

                if (c[i + 3] <= '9') b += (byte)(c[i + 3] - '0');
                else
                {
                    b += (byte)(c[i + 3] - 'A');
                    b += 10;
                }

                sendChar((char)b);
            }
        }
		
		#endregion
		
		#region Load macro data
		void loadXMLData()
		{
			listBoxMacros.Items.Add(xMLData.MacroText1);
			listBoxMacros.Items.Add(xMLData.MacroText2);
			listBoxMacros.Items.Add(xMLData.MacroText3);
			listBoxMacros.Items.Add(xMLData.MacroText4);
			listBoxMacros.Items.Add(xMLData.MacroText5);
			listBoxMacros.Items.Add(xMLData.MacroText6);
			listBoxMacros.Items.Add(xMLData.MacroText7);
			listBoxMacros.Items.Add(xMLData.MacroText8);
			listBoxMacros.Items.Add(xMLData.MacroText9);
			listBoxMacros.Items.Add(xMLData.MacroText10);
			listBoxMacros.Items.Add(xMLData.MacroText11);
			listBoxMacros.Items.Add(xMLData.MacroText12);
			listBoxMacros.Items.Add(xMLData.MacroText13);
			listBoxMacros.Items.Add(xMLData.MacroText14);
			listBoxMacros.Items.Add(xMLData.MacroText15);
			listBoxMacros.Items.Add(xMLData.MacroText16);
			listBoxMacros.Items.Add(xMLData.MacroText17);
			listBoxMacros.Items.Add(xMLData.MacroText18);
			listBoxMacros.Items.Add(xMLData.MacroText19);
			listBoxMacros.Items.Add(xMLData.MacroText20);
			listBoxMacros.Items.Add(xMLData.MacroText21);
			listBoxMacros.Items.Add(xMLData.MacroText22);
			listBoxMacros.Items.Add(xMLData.MacroText23);
			listBoxMacros.Items.Add(xMLData.MacroText24);
			listBoxMacros.Items.Add(xMLData.MacroText25);
			listBoxMacros.Items.Add(xMLData.MacroText26);
			listBoxMacros.Items.Add(xMLData.MacroText27);
			listBoxMacros.Items.Add(xMLData.MacroText28);
			listBoxMacros.Items.Add(xMLData.MacroText29);
			listBoxMacros.Items.Add(xMLData.MacroText30);	
			listBoxMacros.Items.Add(xMLData.MacroText31);
			listBoxMacros.Items.Add(xMLData.MacroText32);	
			listBoxMacros.Items.Add(xMLData.MacroText33);
			listBoxMacros.Items.Add(xMLData.MacroText34);
			listBoxMacros.Items.Add(xMLData.MacroText35);
			listBoxMacros.Items.Add(xMLData.MacroText36);
			listBoxMacros.Items.Add(xMLData.MacroText37);
			listBoxMacros.Items.Add(xMLData.MacroText38);
			listBoxMacros.Items.Add(xMLData.MacroText39);
			listBoxMacros.Items.Add(xMLData.MacroText40);
			listBoxMacros.Items.Add(xMLData.MacroText41);
			listBoxMacros.Items.Add(xMLData.MacroText42);
			listBoxMacros.Items.Add(xMLData.MacroText43);
			listBoxMacros.Items.Add(xMLData.MacroText44);
			listBoxMacros.Items.Add(xMLData.MacroText45);
			listBoxMacros.Items.Add(xMLData.MacroText46);
			listBoxMacros.Items.Add(xMLData.MacroText47);
			listBoxMacros.Items.Add(xMLData.MacroText48);
			listBoxMacros.Items.Add(xMLData.MacroText49);
			listBoxMacros.Items.Add(xMLData.MacroText50);
			listBoxMacros.Items.Add(xMLData.MacroText51);	
			listBoxMacros.Items.Add(xMLData.MacroText52);
			
			MacroDataText[0] = xMLData.MacroText1;
			MacroDataText[1] = xMLData.MacroText2;
			MacroDataText[2] = xMLData.MacroText3;
			MacroDataText[3] = xMLData.MacroText4;
			MacroDataText[4] = xMLData.MacroText5;
			MacroDataText[5] = xMLData.MacroText6;
			MacroDataText[6] = xMLData.MacroText7;
			MacroDataText[7] = xMLData.MacroText8;
			MacroDataText[8] = xMLData.MacroText9;
			MacroDataText[9] = xMLData.MacroText10;
			MacroDataText[10] = xMLData.MacroText11;
			MacroDataText[11] = xMLData.MacroText12;
			MacroDataText[12] = xMLData.MacroText13;
			MacroDataText[13] = xMLData.MacroText14;
			MacroDataText[14] = xMLData.MacroText15;
			MacroDataText[15] = xMLData.MacroText16;
			MacroDataText[16] = xMLData.MacroText17;
			MacroDataText[17] = xMLData.MacroText18;
			MacroDataText[18] = xMLData.MacroText19;
			MacroDataText[19] = xMLData.MacroText20;
			MacroDataText[20] = xMLData.MacroText21;
			MacroDataText[21] = xMLData.MacroText22;
			MacroDataText[22] = xMLData.MacroText23;
			MacroDataText[23] = xMLData.MacroText24;
			MacroDataText[24] = xMLData.MacroText25;
			MacroDataText[25] = xMLData.MacroText26;
			MacroDataText[26] = xMLData.MacroText27;
			MacroDataText[27] = xMLData.MacroText28;
			MacroDataText[28] = xMLData.MacroText29;
			MacroDataText[29] = xMLData.MacroText30;
			MacroDataText[30] = xMLData.MacroText31;
			MacroDataText[31] = xMLData.MacroText32;
			MacroDataText[32] = xMLData.MacroText33;
			MacroDataText[33] = xMLData.MacroText34;
			MacroDataText[34] = xMLData.MacroText35;
			MacroDataText[35] = xMLData.MacroText36;
			MacroDataText[36] = xMLData.MacroText37;
			MacroDataText[37] = xMLData.MacroText38;
			MacroDataText[38] = xMLData.MacroText39;
			MacroDataText[39] = xMLData.MacroText40;
			MacroDataText[40] = xMLData.MacroText41;
			MacroDataText[41] = xMLData.MacroText42;
			MacroDataText[42] = xMLData.MacroText43;
			MacroDataText[43] = xMLData.MacroText44;
			MacroDataText[44] = xMLData.MacroText45;
			MacroDataText[45] = xMLData.MacroText46;
			MacroDataText[46] = xMLData.MacroText47;
			MacroDataText[47] = xMLData.MacroText48;
			MacroDataText[48] = xMLData.MacroText49;
			MacroDataText[49] = xMLData.MacroText50;
			MacroDataText[50] = xMLData.MacroText51;
			MacroDataText[51] = xMLData.MacroText52;
						
			MacroDataData[0] = xMLData.MacroData1;
			MacroDataData[1] = xMLData.MacroData2;
			MacroDataData[2] = xMLData.MacroData3;
			MacroDataData[3] = xMLData.MacroData4;
			MacroDataData[4] = xMLData.MacroData5;
			MacroDataData[5] = xMLData.MacroData6;
			MacroDataData[6] = xMLData.MacroData7;
			MacroDataData[7] = xMLData.MacroData8;
			MacroDataData[8] = xMLData.MacroData9;
			MacroDataData[9] = xMLData.MacroData10;
			MacroDataData[10] = xMLData.MacroData11;
			MacroDataData[11] = xMLData.MacroData12;
			MacroDataData[12] = xMLData.MacroData13;
			MacroDataData[13] = xMLData.MacroData14;
			MacroDataData[14] = xMLData.MacroData15;
			MacroDataData[15] = xMLData.MacroData16;
			MacroDataData[16] = xMLData.MacroData17;
			MacroDataData[17] = xMLData.MacroData18;
			MacroDataData[18] = xMLData.MacroData19;
			MacroDataData[19] = xMLData.MacroData20;
			MacroDataData[20] = xMLData.MacroData21;
			MacroDataData[21] = xMLData.MacroData22;
			MacroDataData[22] = xMLData.MacroData23;
			MacroDataData[23] = xMLData.MacroData24;
			MacroDataData[24] = xMLData.MacroData25;
			MacroDataData[25] = xMLData.MacroData26;
			MacroDataData[26] = xMLData.MacroData27;
			MacroDataData[27] = xMLData.MacroData28;
			MacroDataData[28] = xMLData.MacroData29;
			MacroDataData[29] = xMLData.MacroData30;
			MacroDataData[30] = xMLData.MacroData31;
			MacroDataData[31] = xMLData.MacroData32;
			MacroDataData[32] = xMLData.MacroData33;
			MacroDataData[33] = xMLData.MacroData34;
			MacroDataData[34] = xMLData.MacroData35;
			MacroDataData[35] = xMLData.MacroData36;
			MacroDataData[36] = xMLData.MacroData37;
			MacroDataData[37] = xMLData.MacroData38;
			MacroDataData[38] = xMLData.MacroData39;
			MacroDataData[39] = xMLData.MacroData40;
			MacroDataData[40] = xMLData.MacroData41;
			MacroDataData[41] = xMLData.MacroData42;
			MacroDataData[42] = xMLData.MacroData43;
			MacroDataData[43] = xMLData.MacroData44;
			MacroDataData[44] = xMLData.MacroData45;
			MacroDataData[45] = xMLData.MacroData46;
			MacroDataData[46] = xMLData.MacroData47;
			MacroDataData[47] = xMLData.MacroData48;
			MacroDataData[48] = xMLData.MacroData49;
			MacroDataData[49] = xMLData.MacroData50;
			MacroDataData[50] = xMLData.MacroData51;
			MacroDataData[51] = xMLData.MacroData52;

			MacroDataType[0] = xMLData.MacroType1;
			MacroDataType[1] = xMLData.MacroType2;
			MacroDataType[2] = xMLData.MacroType3;
			MacroDataType[3] = xMLData.MacroType4;
			MacroDataType[4] = xMLData.MacroType5;
			MacroDataType[5] = xMLData.MacroType6;
			MacroDataType[6] = xMLData.MacroType7;
			MacroDataType[7] = xMLData.MacroType8;
			MacroDataType[8] = xMLData.MacroType9;
			MacroDataType[9] = xMLData.MacroType10;
			MacroDataType[10] = xMLData.MacroType11;
			MacroDataType[11] = xMLData.MacroType12;
			MacroDataType[12] = xMLData.MacroType13;
			MacroDataType[13] = xMLData.MacroType14;
			MacroDataType[14] = xMLData.MacroType15;
			MacroDataType[15] = xMLData.MacroType16;
			MacroDataType[16] = xMLData.MacroType17;
			MacroDataType[17] = xMLData.MacroType18;
			MacroDataType[18] = xMLData.MacroType19;
			MacroDataType[19] = xMLData.MacroType20;
			MacroDataType[20] = xMLData.MacroType21;
			MacroDataType[21] = xMLData.MacroType22;
			MacroDataType[22] = xMLData.MacroType23;
			MacroDataType[23] = xMLData.MacroType24;
			MacroDataType[24] = xMLData.MacroType25;
			MacroDataType[25] = xMLData.MacroType26;
			MacroDataType[26] = xMLData.MacroType27;
			MacroDataType[27] = xMLData.MacroType28;
			MacroDataType[28] = xMLData.MacroType29;
			MacroDataType[29] = xMLData.MacroType30;
			MacroDataType[30] = xMLData.MacroType31;
			MacroDataType[31] = xMLData.MacroType32;
			MacroDataType[32] = xMLData.MacroType33;
			MacroDataType[33] = xMLData.MacroType34;
			MacroDataType[34] = xMLData.MacroType35;
			MacroDataType[35] = xMLData.MacroType36;
			MacroDataType[36] = xMLData.MacroType37;
			MacroDataType[37] = xMLData.MacroType38;
			MacroDataType[38] = xMLData.MacroType39;
			MacroDataType[39] = xMLData.MacroType40;
			MacroDataType[40] = xMLData.MacroType41;
			MacroDataType[41] = xMLData.MacroType42;
			MacroDataType[42] = xMLData.MacroType43;
			MacroDataType[43] = xMLData.MacroType44;
			MacroDataType[44] = xMLData.MacroType45;
			MacroDataType[45] = xMLData.MacroType46;
			MacroDataType[46] = xMLData.MacroType47;
			MacroDataType[47] = xMLData.MacroType48;
			MacroDataType[48] = xMLData.MacroType49;
			MacroDataType[49] = xMLData.MacroType50;
			MacroDataType[50] = xMLData.MacroType51;
			MacroDataType[51] = xMLData.MacroType52;
		}
		
		#endregion				

        #region Send Textbox Context menus 
        private void richTextBoxSend_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                contextMenuStripSend.Show(MousePosition);
            }   
        }

        private void copyToolStripMenuItem_Click(object sender, EventArgs e)
        {
            richTextBoxSend.Copy();
        }

        private void cutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            richTextBoxSend.Cut();
        }


        private void deleteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            richTextBoxSend.Clear();
            textBoxSendCount.Text = "0";
            SendCount = 0;
        }

        private void selectAllToolStripMenuItem_Click(object sender, EventArgs e)
        {
            richTextBoxSend.SelectAll();
        }

        private void undoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            richTextBoxSend.Undo();
        }

        private void redoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            richTextBoxSend.Redo();
        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            saveSendBox();
        }

        private void pasteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            richTextBoxSend.Paste();
            setToQueued();        
        }

        // use for paste and load
        private void setToQueued()
        {
            // set to send queued
            radioButtonQueued.Checked = true;
            SendImmediate = false;
            buttonSendQue.Enabled = true;
            QueuedString = richTextBoxSend.Text;
        }

        // Context menu item
        private void loadFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            openInSendBox();
        }
        // File menu item
        private void openInSendBoxToolStripMenuItem_Click_1(object sender, EventArgs e)
        {
            openInSendBox();
        }

        private void saveSendBoxToolStripMenuItem_Click(object sender, EventArgs e)
        {
            saveSendBox();
        }

        private void saveSendBoxToolStripMenuItem_Click_1(object sender, EventArgs e)
        {
            saveSendBox();
        }

        private void openInSendBox()
        {
            string filename = "";
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                filename = openFileDialog1.FileName;
                if (filename != "")
                {
                    richTextBoxSend.LoadFile(filename, RichTextBoxStreamType.PlainText);
                    setToQueued();
                }
            }
            else
            {
                MessageBox.Show("Error loading file.");
            }
        }

        private void saveSendBox()
        {
            string filename = "";
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                filename = saveFileDialog1.FileName;
                if (filename != "") richTextBoxSend.SaveFile(filename, RichTextBoxStreamType.PlainText);
            }
        }

        #endregion

        #region Receive Textbox Context menus

        private void richTextBoxReceive_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                contextMenuStripReceive.Show(MousePosition);
            }
        }

        private void copyToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            richTextBoxReceive.Copy();
        }

        private void cutToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            richTextBoxReceive.Cut();
        }

        private void selectAllToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            richTextBoxReceive.SelectAll();
        }

        private void deleteToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            richTextBoxReceive.Clear();
        }

        private void saveFileAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            saveReceiveBox();
        }

        private void saveReceiveBoxToolStripMenuItem_Click(object sender, EventArgs e)
        {
            saveReceiveBox();
        }

        private void saveReceiveBoxToolStripMenuItem_Click_1(object sender, EventArgs e)
        {
            saveReceiveBox();
        }

        private void saveReceiveBox()
        {
            string filename = "";
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                filename = saveFileDialog1.FileName;
                if (filename != "") richTextBoxReceive.SaveFile(filename, RichTextBoxStreamType.PlainText);
            }
        }

        #endregion

        #region Send & Receive richTextBox settings
        private void sendToolStripMenuItem_Click(object sender, EventArgs e)
        {
            TextBoxSettings send = new TextBoxSettings();

            if (send.ShowDialog() == DialogResult.OK)
            {
                richTextBoxSend.BackColor = send.backColor;
                richTextBoxSend.ForeColor = send.foreColor;
                if(send.font != null) richTextBoxSend.Font = send.font;
            }
            else
            {
                MessageBox.Show("Send richTextBox settings canceled.");
            }

        }

        private void receiveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            TextBoxSettings receive = new TextBoxSettings();

            if (receive.ShowDialog() == DialogResult.OK)
            {
                richTextBoxReceive.BackColor = receive.backColor;
                richTextBoxReceive.ForeColor = receive.foreColor;
                if (receive.font != null) richTextBoxReceive.Font = receive.font;
            }
            else
            {
                MessageBox.Show("Receive richTextBox settings canceled.");
            }
        }

        #endregion

        #region COM port settings
        private void portToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // Make sure the port isn't already open
            if (serialPort1.IsOpen)
            {
                MessageBox.Show("The port must be closed before changing the settings.");
                return;
            }
            else
            {
                if (portSettings.ShowDialog() == DialogResult.OK)
                {
                    if (portSettings.selectedPort != "")
                    {
                        // set the serial port to the new settings
                        serialPort1.PortName = portSettings.selectedPort;
                        serialPort1.BaudRate = portSettings.selectedBaudrate;
                        serialPort1.DataBits = portSettings.selectedDataBits;
                        serialPort1.Parity = portSettings.selectedParity;
                        serialPort1.StopBits = portSettings.selectedStopBits;

                        // Show the new settings in the form text line
                        showSettings();
                    }
                    else
                    {
                        MessageBox.Show("Error: Settings form returned with no COM port selected.");
                        return; // bail out
                    }
                }
                else
                {
                    MessageBox.Show("Error: buttonSetup_Click - Settings dialog box did not return Okay.");
                    return; // bail out
                }

                // Open the port
                openPort();

                // Show the modem line states
                showCTS_DSR_CD();
            }          
        }

        // show the settings in the form text line
        private void showSettings()
        {
            this.Text = "Smiley Micros - " +
                portSettings.selectedPort + " " +
                portSettings.selectedBaudrate.ToString() + "," +
                portSettings.selectedParity + "," +
                portSettings.selectedDataBits.ToString() + "," +
                portSettings.selectedStopBits + " - " +
                portSettings.selectedHandshaking;
            if (serialPort1.IsOpen)
            {
                this.Text += " - Port is open";
            }
            else
            {
                this.Text += " - Port is closed";
            }
        }

        private void openPortToolStripMenuItem_Click(object sender, EventArgs e)
        {
            openPort(); 
        }

        private void openPort()
        {
            try
            {
                if (serialPort1.IsOpen)
                {
                    serialPort1.Close();
                    openPortToolStripMenuItem.Text = "Open Port";
                }
                else
                {
                    serialPort1.Open();
                    openPortToolStripMenuItem.Text = "Close Port";
                }

                showSettings();
            }
            catch (System.Exception ex)
            {
                MessageBox.Show("Error - openPortToolStripMenuItem_Click Exception: " + ex);
            }
        }
        #endregion

        #region Modem lines
        private void showCTS_DSR_CD()
        {
            if (serialPort1.IsOpen)
            {
                if (serialPort1.CtsHolding) this.LEDCTS.image = (int)LED.LED22.LEDColor.BlueOn;
                else this.LEDCTS.image = (int)LED.LED22.LEDColor.BlueOff;

                if (serialPort1.DsrHolding) this.LEDDSR.image = (int)LED.LED22.LEDColor.BlueOn;
                else this.LEDDSR.image = (int)LED.LED22.LEDColor.BlueOff;

                if (serialPort1.CDHolding) this.LEDDCD.image = (int)LED.LED22.LEDColor.BlueOn;
                else this.LEDDCD.image = (int)LED.LED22.LEDColor.BlueOff;
                
                if (RingIndicator) this.LEDRI.image = (int)LED.LED22.LEDColor.BlueOn;
                else this.LEDRI.image = (int)LED.LED22.LEDColor.BlueOff;          
            }
        }

        // Toggle RTS state
        private void buttonRTS_Click(object sender, EventArgs e)
        {
            if (RTSToggle)
            {
                serialPort1.RtsEnable = true;
                LEDRTS.image = (int)LED.LED22.LEDColor.RedOn;
            }
            else
            {
                serialPort1.RtsEnable = false;
                LEDRTS.image = (int)LED.LED22.LEDColor.RedOff;
            }
            RTSToggle = !RTSToggle;
        }

        // Toggle DTR state
        private void buttonDTR_Click(object sender, EventArgs e)
        {
            if (DTRToggle)
            {
                serialPort1.DtrEnable = true;
                LEDDTR.image = (int)LED.LED22.LEDColor.RedOn;
            }
            else
            {
                serialPort1.DtrEnable = false;
                LEDDTR.image = (int)LED.LED22.LEDColor.RedOff;
            }
            DTRToggle = !DTRToggle;
        }

        // Check Modem input line status
        private void timerOneSec_Tick(object sender, EventArgs e)
        {
            showCTS_DSR_CD();
        }

        // Show the modem states on the virtual LEDs
        private void serialPort1_PinChanged(object sender, SerialPinChangedEventArgs e)
        {
            // Toggle RI since we can't determine the state with the SerialPort class
            if(e.EventType == SerialPinChange.Ring) RingIndicator = !RingIndicator;

            showCTS_DSR_CD();
        }

        #endregion

        #region Byte Shooter
        // Send a character at regular intervals
        private bool shoot = false;
        private byte byteToSend = 0;
        private void sendCharToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (!shoot)
            {
                shoot = true;
                sendCharToolStripMenuItem.Text = "Stop sending bytes!";
                ByteShooter shooter = new ByteShooter();

                if (shooter.ShowDialog() == DialogResult.OK)
                {
                    byteToSend = shooter.byteToSend;
                    timerSendByte.Interval = shooter.interval;
                    timerSendByte.Enabled = true;
                }
            }
            else
            {
                timerSendByte.Enabled = false;
                shoot = false;
                sendCharToolStripMenuItem.Text = "Send byte periodically.";
            }
        }

        private void timerSendByte_Tick(object sender, EventArgs e)
        {
            //JWP 4/28/07 - changed from sendChar to sendByte
            sendByte(byteToSend);
        }
        #endregion

        #region Help

        private void manualToolStripMenuItem_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start("Developer Terminal.pdf");
        }

        private void smileyMicrosToolStripMenuItem_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start("www.smileymicros.com");
        }

        #endregion
    }
}

