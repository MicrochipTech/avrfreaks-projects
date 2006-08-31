using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace SerialPortTerminal
{
  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [MTAThread]
    static void Main()
    {
      Application.EnableVisualStyles();
      Application.Run(new frmTerminal());
    }
  }
}