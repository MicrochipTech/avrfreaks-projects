namespace SerialPortTerminal
{
  partial class frmAbout
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.web = new System.Windows.Forms.WebBrowser();
      this.SuspendLayout();
      // 
      // web
      // 
      this.web.AllowWebBrowserDrop = false;
      this.web.Dock = System.Windows.Forms.DockStyle.Fill;
      this.web.IsWebBrowserContextMenuEnabled = false;
      this.web.Location = new System.Drawing.Point(0, 0);
      this.web.Name = "web";
      this.web.ScrollBarsEnabled = false;
      this.web.Size = new System.Drawing.Size(377, 318);
      this.web.WebBrowserShortcutsEnabled = false;
      this.web.Navigated += new System.Windows.Forms.WebBrowserNavigatedEventHandler(this.web_Navigated);
      // 
      // frmAbout
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(377, 318);
      this.Controls.Add(this.web);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "frmAbout";
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "About Terminal";
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.WebBrowser web;
  }
}