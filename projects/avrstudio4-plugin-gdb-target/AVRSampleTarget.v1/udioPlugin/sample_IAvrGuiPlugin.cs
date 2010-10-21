using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;
using System.Windows.Forms;

// crashes AVRStudio and messes up Registry badly (reinstall AVRStudio) ?!?

#if false
namespace myCompany
{
    // implementation of a sample IAvrProgEventsSink and IAvrGuiPlugin
    [Guid("0D53A3E8-E51A-49C7-944E-E72A2064F93A")]
    [ClassInterface(ClassInterfaceType.None)]
    [ProgId("myCompany.myAvrPlugin")]
    public class myPluginEventSink : AVRStudioPlugin.AvrStudioPlugin, AVRStudioPlugin.IAvrProgEventsSink
    {
        public void Error(string msg)
        {
        }

        public void Progress(double Progress)
        {
        }
    }

    [Guid("0D53A3E8-E51A-49C7-944E-E72A2064F939")]
    [ClassInterface(ClassInterfaceType.None)]
    [ProgId("myCompany.myAvrPlugin")]
    public class myPlugin : AVRStudioPlugin.AvrStudioPlugin, AVRStudioPlugin.IAvrGuiPlugin
    {
        public myPlugin()
        {
            MessageBox.Show("myPlugin instantiated");
        }

        public void guiPlugInSetup(out string projectFile, int status)
        {
            MessageBox.Show("myPlugin guiPlugInSetup");

            FileInfo fi = new FileInfo(this.GetType().Module.FullyQualifiedName);
            FileStream fs = new FileStream(fi.Directory.FullName + "\\myPluginSetup.xml", FileMode.Open);
            StreamReader sr = new StreamReader(fs);
            projectFile = sr.ReadToEnd();
            sr.Close();
            fs.Close();
            status = 0;
        }


        public void guiPlugInExecuteCommand(uint command)
        {
            //MessageBox.Show("myPlugin guiPlugInExecuteCommand");
        }

        public void guiPlugInInit(ref string projectFile, int status)
        {
            //MessageBox.Show("myPlugin guiPlugInInit");

            //AVRFRAMEWORKLib.FrameWorkClass fwc = new AVRFRAMEWORKLib.FrameWorkClass();
            //fwc.create("myPlugin", true);
        }

        public void guiPlugInGetCommandStatus(uint commandId, out uint pCmndStatus)
        {
            MessageBox.Show("myPlugin guiPlugInGetCommandStatus");
            pCmndStatus = 0;
        }
    }
}
#endif
