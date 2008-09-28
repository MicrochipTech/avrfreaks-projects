using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
//using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Net.NetworkInformation;
using System.Net.Mail;
using System.Text.RegularExpressions;
using System.Threading;



namespace final001

{
    
    public partial class Form1 : Form
    {
        internal int[] testarray = new int[2100];
        internal int[] tosendarray = new int[2100];
                
        public Form1()
        {
            InitializeComponent();


        }

        public partial class ParallelPort : UserControl
        {
            private int _PortAddress = 888;

            private short DataBusAddress
            {
                get
                {
                    return Convert.ToInt16(_PortAddress);
                }
            }
            

        }

        public string ToSend = " Hello World                     ";
        
        public int cc=0;
        
        //DateTime prevtime = DateTime.Now;
        TimeSpan durationprev = DateTime.Now - DateTime.Now;

        private void timer1_Tick(object sender, EventArgs e)
        {
            /*DateTime startTime = DateTime.Now;
            TimeSpan duration = startTime - prevtime;
            prevtime = startTime;

            if (duration > durationprev)
                durationprev = duration;*/
        }



        private void cmdEnter_Click(object sender, EventArgs e)
        {
            if (txtID.Text == "")
            {
                Microsoft.VisualBasic.Interaction.MsgBox("Please Enter Device ID first.", Microsoft.VisualBasic.MsgBoxStyle.Critical, "Error:Unspecified Device ID");
                return;
            }
            
            string CC;
            int paritybit = 0;
            CC = Microsoft.VisualBasic.Interaction.InputBox("Please enter the text to send", "Send Via IR", lblSend.Text, 200, 400);
            if (CC.Length == 0)
            {
                return;    
            }
            lblStatus.ForeColor = Color.Red;
            lblStatus.Text = "Busy";
            Form1.ActiveForm.Cursor = Cursors.WaitCursor;
            Application.DoEvents(); 
            
            
            lblSend.Text = "" + CC + "                                                      ";
            CC = lblSend.Text; 
            lblSend.Text = "" + CC.Substring(0, 32);    
            ToSend = lblSend.Text;

            // calibrate timer


            int ctr = 0;
            int i;
            int charcounter;
            char chid = txtID.Text[0];
            int chhid = chid -48;
            for (i=0; i<2100;i++)
                tosendarray[i] = 1;
            for (charcounter =0; charcounter < 32; charcounter ++)
            {
                                

                // give the initial delay
                for (i=0; i<10;i++)
                    tosendarray[ctr++] = 1;

                //pulse initiation
                for (i=0; i<4;i++)
                    tosendarray[ctr++] = 0;

                for (i = 0; i < 3; i++) //device ID
                {
                    if (((chhid >> i) & 1) == 1)
                    {
                        tosendarray[ctr++] = 1;
                        tosendarray[ctr++] = 0;
                        tosendarray[ctr++] = 0;
                    }
                    else
                    {
                        tosendarray[ctr++] = 1;
                        tosendarray[ctr++] = 0;
                    }
                }

                for (i = 0; i < 8; i++) //8databits
                {
                    if (((ToSend[charcounter] >> i) & 1) == 1)
                    {
                        tosendarray[ctr++] = 1;
                        tosendarray[ctr++] = 0;
                        tosendarray[ctr++] = 0;
                    }
                    else
                    {
                        tosendarray[ctr++] = 1;
                        tosendarray[ctr++] = 0;
                    }
                }

                for (i = 0; i < 5; i++) //5 location bits
                {
                    if ( ( (charcounter  >> i) & 1) == 1)
                    {
                        tosendarray[ctr++] = 1;
                        tosendarray[ctr++] = 0;
                        tosendarray[ctr++] = 0;
                    }
                    else 
                    {
                        tosendarray[ctr++] = 1;
                        tosendarray[ctr++] = 0;
                    }
                }

                // if code comes here, then this is the last bit. Send parity bit


                paritybit = chhid ^ (chhid >> 1) ^ (chhid >> 2);
                paritybit ^= charcounter ^ (charcounter >> 1) ^ (charcounter >> 2) ^ (charcounter >> 3);
                paritybit ^= (charcounter >> 4);
                paritybit ^= ToSend[charcounter] ^ (ToSend[charcounter] >> 1) ^ (ToSend[charcounter] >> 2);
                paritybit ^= (ToSend[charcounter] >> 3) ^ (ToSend[charcounter] >> 4) ^ (ToSend[charcounter] >> 5);
                paritybit ^= (ToSend[charcounter] >> 6) ^ (ToSend[charcounter] >> 7);
                paritybit = ~paritybit;
                paritybit &= 1;
                    if ( paritybit == 1)
                    {
                        tosendarray[ctr++] = 1;
                        tosendarray[ctr++] = 0;
                        tosendarray[ctr++] = 0;
                    }
                    else 
                    {
                        tosendarray[ctr++] = 1;
                        tosendarray[ctr++] = 0;
                    }
                
            }
            txtStream.Text = "";
            for (i = 0; i < 2100; i++)
                txtStream.Text += tosendarray[i];
            lblStatus.ForeColor = Color.Green ; 
            lblStatus.Text = "Ready";
            Form1.ActiveForm.Cursor = Cursors.Default ;


        }
    
            

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            lblStatus.ForeColor = Color.Red;
            lblStatus.Text = "Busy";
            Form1.ActiveForm.Cursor = Cursors.WaitCursor;
            Application.DoEvents(); 
            Thread newThread = new Thread(new ThreadStart(ThreadMethod));
            DateTime prevtime = DateTime.Now;
            if (chkSendNow.Checked == true)
            {
                if (tosendarray[0] == 0)
                {
                    Microsoft.VisualBasic.Interaction.MsgBox("No Text Entered. Please Enter Text First.", Microsoft.VisualBasic.MsgBoxStyle.Critical, "Error");
                    chkSendNow.Checked = false;
                    lblStatus.ForeColor = Color.Green;
                    lblStatus.Text = "Ready";
                    return;
                }

                

                /*long c1 = DateTime.Now.Second * 1000 + DateTime.Now.Millisecond;
                for (long ii = 0; ii < 300000000; ii++) ;
                PortAccess.Output(888, 1);
                long c2 = DateTime.Now.Second * 1000 + DateTime.Now.Millisecond;
                label1.Text = "" + (300000000 / (c2 - c1));
                osccal = (300000000 / (c2 - c1));*/
                for (int i = 0; i < 3; i++)
                {
                    while (cc < 2101)
                    {
                        if (cc < 2100)
                        {
                            PortAccess.Output(888, tosendarray[cc++]);
                            //lblBSC.Text = "" + cc;
                        }
                        else
                        {
                            cc = 0;
                            chkSendNow.Checked = false;
                            timer1.Enabled = false;
                            DateTime startTime = DateTime.Now;
                            TimeSpan duration = startTime - prevtime;
                            prevtime = startTime;
                            lblBSC.Text = "" + duration.Minutes + ":" + duration.Seconds + "." + duration.Milliseconds;
                            break;
                        }
                        ;
                        //long jj = osccal *32;
                        //while (jj-- > 0) ;
                        Thread.Sleep(1);
                    }
                }
                chkSendNow.Checked = false;
                lblStatus.ForeColor = Color.Green;
                lblStatus.Text = "Ready";
                Form1.ActiveForm.Cursor = Cursors.Default ;
                //if (timer1.Enabled == false)
                  //  timer1.Enabled = true;
                //DateTime prevtime = DateTime.Now;
            }
            else
            {
                if (timer1.Enabled == true )
                    timer1.Enabled = false;


            }
            

        }

        private void cmdSend_Click(object sender, EventArgs e)
        {
            if (chkSendNow.Checked == false)
                chkSendNow.Checked = true;
            else
                chkSendNow.Checked = false;
        }

        static void ThreadMethod()
        {
            Thread.Sleep(1000);
        }



        private void cmdCalib_Click(object sender, EventArgs e)
        {
            lblStatus.Text = "Busy";
            lblStatus.ForeColor = Color.Red ;
            Form1.ActiveForm.Cursor = Cursors.WaitCursor;
            Application.DoEvents(); 
            
            Thread newThread =    new Thread(new ThreadStart(ThreadMethod));

            DateTime prevtime = DateTime.Now;
                /*long c1 = DateTime.Now.Second * 1000 + DateTime.Now.Millisecond;
                for (long ii = 0; ii < 300000000; ii++) ;
                PortAccess.Output(888, 1);
                long c2 = DateTime.Now.Second * 1000 + DateTime.Now.Millisecond;
                label1.Text = "" + (300000000 / (c2 - c1));
                osccal = (300000000 / (c2 - c1));*/

                //generate array. We will use exactly same instructions for calibration to ensure same times
                
                for (int ii = 0; ii < 1050; ii++)
                {
                    if ((ii % 2) == 0)
                        testarray[ii] = 1;
                    else
                        testarray[ii] = 0;
                }
                cc = 1;
                PortAccess.Output(888, testarray[0]);
                while (cc < 901)
                {
                    if (cc < 900)
                    {
                        //if (testarray [cc] != testarray [cc-1])
                            PortAccess.Output(888, testarray[cc]);

                        cc++;
                        //lblBSC.Text = "" + cc;
                    }
                    else
                    {
                        cc = 0;
                        chkSendNow.Checked = false;
                        timer1.Enabled = false;
                        DateTime startTime = DateTime.Now;
                        TimeSpan duration = startTime - prevtime;
                        prevtime = startTime;
                        lblBSC.Text = "" + duration.Minutes + ":" + duration.Seconds + "." + duration.Milliseconds;
                        Thread.Sleep(1);
                        break;
                    }
                    Thread.Sleep(1);
                    //long jj = osccal*16;
                    //while (jj-- > 0) ;
                }
                PortAccess.Output(888, 1);
                lblStatus.ForeColor = Color.Green;
                lblStatus.Text = "Ready";
                Form1.ActiveForm.Cursor = Cursors.Default;

        }

        private void button1_Click(object sender, EventArgs e)
        {
            Microsoft.VisualBasic.Interaction.MsgBox("This IR Software was written by Sajid Muhaimin Choudhury, 0406023", Microsoft.VisualBasic.MsgBoxStyle.Information, "About");
               
        }

        private void txtID_TextChanged(object sender, EventArgs e)
        {
            
            if (txtID.Text != "")
            {
                char cc = txtID.Text[0];
                if (cc < '0' || cc > '7')
                {
                    Microsoft.VisualBasic.Interaction.MsgBox("Numeric Values between 0 and 7 are accepted only", Microsoft.VisualBasic.MsgBoxStyle.Critical, "Error: Invalid Value");
                    txtID.Text = "7";
                }
                else
                {
                    int AX=1;
                    cmdEnter_Click(AX, e);
                    
                }
            }
        }






        

    }
    
}
