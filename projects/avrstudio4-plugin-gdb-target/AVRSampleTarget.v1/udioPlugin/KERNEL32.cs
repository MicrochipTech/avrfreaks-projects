using System.Runtime.InteropServices;

static class KERNEL32
{
    [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    public static extern int GetProcAddress([In] int hModule, [In, MarshalAs(UnmanagedType.LPStr)] string lpProcName);

    [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    public static extern int LoadLibrary([In] string lpFileName);

    [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    public static extern int GetModuleHandle(string lpModuleName);

    [DllImport("kernel32.dll")]
    public static extern bool FreeLibrary(int hModule);
}
