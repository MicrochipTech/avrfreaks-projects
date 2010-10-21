using System;
using System.Text;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using System.ComponentModel;
using System.Windows.Forms;
using System.Collections.Generic;

namespace AVRStudioSDK
{
    [ComImport]
    [Guid("00000001-0000-0000-C000-000000000046")]
    [InterfaceTypeAttribute(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IClassFactory
    {
        [return: MarshalAs(UnmanagedType.I4)]
        [PreserveSig]
        int CreateInstance(
            [In, MarshalAs(UnmanagedType.Interface)] object pUnkOuter,
            ref Guid riid,
            [Out, MarshalAs(UnmanagedType.Interface)] out object obj);

        [return: MarshalAs(UnmanagedType.I4)]
        [PreserveSig]
        int LockServer(
            [In] bool fLock);
    }

    public class COMfactory : IClassFactory
    {
        public COMfactory()
        {
            //MessageBox.Show("COMfactory instantiated");
        }

        Type mTyp;
        public Type Typ
        {
            set
            {
                mTyp = value;
            }
        }

        public int CreateInstance(object pUnkOuter, ref Guid riid, out object obj)
        {
            //MessageBox.Show("COMfactory CreateInstance " + riid.ToString());

            obj = null;
            try
            {
                obj = mTyp.InvokeMember(null, System.Reflection.BindingFlags.CreateInstance, null, null, new object[] { });
            }
            catch (System.Exception e )
            {
                MessageBox.Show(e.ToString(), "COMfactory CreateInstance failed");
            }

            return 0;
        }

        public int LockServer(bool fLock)
        {
            //MessageBox.Show("COMfactory LockServer");

            return 0;
        }
    }
}
