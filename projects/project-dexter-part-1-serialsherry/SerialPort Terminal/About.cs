#region Namespace Inclusions
using System;
using System.Text;
using System.Diagnostics;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Windows.Forms;
using System.Reflection;
#endregion

namespace SerialPortTerminal
{
  public partial class frmAbout : Form
  {
    private string TempFile = Path.GetTempFileName();

    public frmAbout()
    {
      InitializeComponent();
      
      // Read the about HTML from the assembly
      string html = (new StreamReader(Assembly.GetExecutingAssembly().GetManifestResourceStream("SerialPortTerminal.About.htm"))).ReadToEnd();

      // Replace sections with appropriate data
      html = html.Replace("{version}", Assembly.GetExecutingAssembly().GetName().Version.ToString());

      // Save the temp file so the web browser has a target to navigate to
      File.WriteAllText(TempFile, html);

      // Show the temp about file 
      web.Navigate(TempFile);
    }

    private void web_Navigated(object sender, WebBrowserNavigatedEventArgs e)
    {
      // Since the navigation is complete, delete the temp file
      File.Delete(TempFile);
    }
  }
}