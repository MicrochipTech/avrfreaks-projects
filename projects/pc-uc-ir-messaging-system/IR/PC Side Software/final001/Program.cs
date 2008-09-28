using System;
using System.Collections.Generic;
//using System.Linq;
using System.Windows.Forms;

using System.ComponentModel;
using System.Data;
using System.Drawing;

using System.Text;

using System.Runtime.InteropServices;
using System.Net.NetworkInformation;
using System.Net.Mail;
using System.Text.RegularExpressions;


namespace final001
{
    public class PortAccess
    {
        [DllImport("inpout32.dll", EntryPoint = "Out32")]

        public static extern void Output(int adress, int value);
    }

    public class program
    {
        /// <summary>
        
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());
        }
    }
}
