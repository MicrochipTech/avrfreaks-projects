namespace LED
{
    partial class LED22
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(LED22));
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.SuspendLayout();
            // 
            // imageList1
            // 
            this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList1.Images.SetKeyName(0, "Red LED ON.gif");
            this.imageList1.Images.SetKeyName(1, "Red LED OFF.gif");
            this.imageList1.Images.SetKeyName(2, "Orange LED ON.gif");
            this.imageList1.Images.SetKeyName(3, "Orange LED OFF.gif");
            this.imageList1.Images.SetKeyName(4, "Yellow LED ON.gif");
            this.imageList1.Images.SetKeyName(5, "Yellow LED OFF.gif");
            this.imageList1.Images.SetKeyName(6, "Green LED ON.gif");
            this.imageList1.Images.SetKeyName(7, "Green LED OFF.gif");
            this.imageList1.Images.SetKeyName(8, "Blue LED ON.gif");
            this.imageList1.Images.SetKeyName(9, "Blue LED OFF.gif");
            // 
            // LED22
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("$this.BackgroundImage")));
            this.Name = "LED22";
            this.Size = new System.Drawing.Size(22, 22);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ImageList imageList1;
    }
}
