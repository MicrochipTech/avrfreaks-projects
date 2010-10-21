using System;
using System.Runtime.InteropServices;

static class OLE32
{
    [DllImport("ole32.dll", ExactSpelling = true, PreserveSig = false)]
    public static extern int CoGetClassObject([In] ref Guid clsid, int dwContext, int serverInfo, [In] ref Guid refiid, [Out, MarshalAs(UnmanagedType.Interface)] out object cfp);

    [DllImport("ole32.dll", ExactSpelling = true, PreserveSig = false)]
    public static extern int CoCreateInstance([In] ref Guid clsid, [MarshalAs(UnmanagedType.Interface)] object punkOuter, int dwContext, [In] ref Guid iid, [Out, MarshalAs(UnmanagedType.Interface)] out object cfp);
}
