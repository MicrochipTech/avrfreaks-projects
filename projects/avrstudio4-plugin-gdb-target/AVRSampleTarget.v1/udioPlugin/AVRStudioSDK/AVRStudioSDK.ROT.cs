using System;
using System.Text;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using System.ComponentModel;
using System.Windows.Forms;
using System.Collections.Generic;

namespace AVRStudioSDK
{
    public static class ROT
    {
        // components in the ROT
        public static string COMP_TARGET = "AVRTARGET";
        public static string COMP_EVENTQ = "AVREVENTQ";
        public static string COMP_XMLPROJECT = "XMLPROJECT";
        public static string COMP_XMLPARTDESCRIPTION = "XMLPARTDESCRIPTION";
        public static string COMP_EVENTMSGQ = "AVREVENTMSGQ";
        public static string COMP_PARSER = "AVRPARSER";
        public static string COMP_SIM_MEMORY = "SIM_MEMORY";
        public static string COMP_COORDINATOR = "COORDINATOR";
        public static string COMP_GUI_HANDLE = "GUI_HANDLE";
    }
}

