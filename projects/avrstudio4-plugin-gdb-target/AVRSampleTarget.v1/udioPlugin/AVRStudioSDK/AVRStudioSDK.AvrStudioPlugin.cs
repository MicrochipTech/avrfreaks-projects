using System;
using System.Text;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using System.ComponentModel;
using System.Windows.Forms;
using System.Collections.Generic;

// define FORCE_MSCOREE_FORWARD to force the use of mscoree_forward.dll (instead of mscoree.dll normally used by regasm32)

namespace AVRStudioSDK
{
    public class AvrStudioPlugin
    {
        public static void ComRegisterFunction_mscoree_forward(Type t, bool mscoree_forward)
        {
#if FORCE_MSCOREE_FORWARD
            mscoree_forward = true;
#endif

            if (t.Equals(typeof(AvrStudioPlugin)))
            {
                // dont register this
                return;
            }
            if (t.Equals(typeof(COMfactory)))
            {
                // dont register this
                return;
            }

            try
            {
                System.IO.FileInfo myfi = new System.IO.FileInfo(t.Module.FullyQualifiedName);

                AttributeCollection attributes = TypeDescriptor.GetAttributes(t);
                ProgIdAttribute ProgIdAttr = attributes[typeof(ProgIdAttribute)] as ProgIdAttribute;

                string ProgId = ProgIdAttr != null ? ProgIdAttr.Value : t.FullName;

                GuidAttribute GUIDAttr = attributes[typeof(GuidAttribute)] as GuidAttribute;
                string GUID = "{" + GUIDAttr.Value + "}";

                RegistryKey localServer32 = Registry.ClassesRoot.CreateSubKey(String.Format("CLSID\\{0}\\LocalServer32", GUID));
                RegistryKey inprocServer32 = Registry.ClassesRoot.CreateSubKey(String.Format("CLSID\\{0}\\InprocServer32", GUID));

                if ( mscoree_forward )
                {
                    // mscoree_forward=true: invoked by mscoree_forward.dll
                    // provide path to mscoree_forward.dll to handle this dll

                    localServer32.SetValue(null, myfi.Directory.FullName + "\\mscoree_forward.dll");

                    inprocServer32.SetValue(null, myfi.Directory.FullName + "\\mscoree_forward.dll");
                    inprocServer32.SetValue("Assembly", t.Assembly.ToString());
                    inprocServer32.SetValue("Class", ProgId);
                    inprocServer32.SetValue("Codebase", t.Assembly.CodeBase);
                    inprocServer32.SetValue("RuntimeVersion", t.Assembly.ImageRuntimeVersion);
                    inprocServer32.SetValue("threadingModel", "Both");
                    RegistryKey inprocServer32version = inprocServer32.CreateSubKey("1.0.0.0");
                    inprocServer32version.SetValue("Assembly", t.Assembly.ToString());
                    inprocServer32version.SetValue("Class", ProgId);
                    inprocServer32version.SetValue("Codebase", t.Assembly.CodeBase);
                    inprocServer32version.SetValue("RuntimeVersion", t.Assembly.ImageRuntimeVersion);
                }
                else
                {
                    // mscoree_forward=false: invoked by regasm32
                    // fix path to mscoree (provide full path), otherwise AVRStudio won't load it

                    localServer32.SetValue(null, Environment.GetEnvironmentVariable("windir") + "\\system32\\mscoree.dll");

                    inprocServer32.SetValue(null, Environment.GetEnvironmentVariable("windir") + "\\system32\\mscoree.dll");
                }

                RegistryKey CLSIDProgID = Registry.ClassesRoot.CreateSubKey(String.Format("CLSID\\{0}\\ProgId", GUID));
                CLSIDProgID.SetValue(null, ProgId);

                RegistryKey ProgIDCLSID = Registry.ClassesRoot.CreateSubKey(String.Format("CLSID\\{0}", ProgId));
                ProgIDCLSID.SetValue(null, GUID);

                List<Type> tinterfaces = new List<Type>(t.GetInterfaces());
                if (tinterfaces.Contains(typeof(AVRStudioSDK.IAvrTarget)))
                {
                    // implement IAvrTarget: {0F428ED5-EB63-4ed5-88A0-9DE9D7C0BB2F}
                    Registry.ClassesRoot.CreateSubKey(String.Format("CLSID\\{0}\\Implemented Categories\\{{0F428ED5-EB63-4ed5-88A0-9DE9D7C0BB2F}}", GUID));
                }
                if (tinterfaces.Contains(typeof(AVRStudioSDK.IGuiPlugin)))
                {
                    // implement IGuiPlugin / IAvrGuiPlugin: {D6C9B3C1-6A18-11d5-809B-006008125A1D}
                    Registry.ClassesRoot.CreateSubKey(String.Format("CLSID\\{0}\\Implemented Categories\\{{D6C9B3C1-6A18-11D5-809B-006008125A1D}}", GUID));
                }
                if (tinterfaces.Contains(typeof(AVRStudioSDK.IAvrProjectPlugin)))
                {
                    // implement project-plugin: {4DB2A60E-D9FE-40bf-B3D5-58BD4363C42F}
                    Registry.ClassesRoot.CreateSubKey(String.Format("CLSID\\{0}\\Implemented Categories\\{{4DB2A60E-D9FE-40BF-B3D5-58BD4363C42F}}", GUID));
                }
                // implement _Object (mscoree): {62C8FE65-4EBB-45e7-B440-6E39B2CDBF29}
                //Registry.ClassesRoot.CreateSubKey(String.Format("CLSID\\{0}\\Implemented Categories\\{{62C8FE65-4EBB-45e7-B440-6E39B2CDBF29}}", GUID));

                if (mscoree_forward)
                {
                    //MessageBox.Show("Successful: AvrStudioPlugin.ComRegisterFunction(" + t.ToString() + ") invoked by regsvr32.exe and mscoree_forward.dll");
                }
                else
                {
                    //MessageBox.Show("Successful: AvrStudioPlugin.ComRegisterFunction(" + t.ToString() + ") invoked by regasm32.exe");
                }
            }
            catch (Exception e)
            {
                MessageBox.Show(e.ToString(), "Error during AvrStudioPlugin.ComRegisterFunction(" + t.ToString() + ")");
            }
        }

        [ComRegisterFunction]
        public static void ComRegisterFunction(Type t)
        {
            // mscoree_forward=false: invoked by regasm32
            ComRegisterFunction_mscoree_forward(t, false);
        }

        [ComUnregisterFunction]
        public static void ComUnregisterFunction(Type t)
        {
            try
            {
                AttributeCollection attributes = TypeDescriptor.GetAttributes(t);
                ProgIdAttribute ProgIdAttr = attributes[typeof(ProgIdAttribute)] as ProgIdAttribute;

                string ProgId = ProgIdAttr != null ? ProgIdAttr.Value : t.FullName;

                try
                {
                    // may not exist... ignore any errors
                    Registry.ClassesRoot.DeleteSubKeyTree("CLSID\\{" + t.GUID + "}");
                }
                catch { }
                try
                {
                    // may not exist... ignore any errors
                    Registry.ClassesRoot.DeleteSubKeyTree("CLSID\\" + ProgId);
                }
                catch { }

                //MessageBox.Show("Successful: AvrStudioPlugin.ComUnregisterFunction(" + t.ToString() + ")");
            }
            catch (Exception e)
            {
                MessageBox.Show(e.ToString(), "Error during AvrStudioPlugin.ComUnregisterFunction(" + t.ToString() + ")");
            }
        }
    }
}
