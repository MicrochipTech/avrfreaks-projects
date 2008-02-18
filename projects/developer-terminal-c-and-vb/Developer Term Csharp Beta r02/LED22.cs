using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace LED
{
    public partial class LED22 : UserControl
    {
        public enum LEDColor { RedOn, RedOff, YellowOn, YellowOff, OrangeOn, OrangeOff, GreenOn, GreenOff, BlueOn, BlueOff }
        
        public LED22()
        {
            InitializeComponent();

            this.BackgroundImage = imageList1.Images[(int)LEDColor.RedOn];

            setImage((int)LEDColor.RedOn);
        }

        private int Image = (int)LEDColor.RedOn;
        public int image
        {
            get
            {
                return Image;
            }
            set
            {
                Image = value;
                setImage(Image);
            }
        }

        private void setImage(int num)
        {
            if (num <= (int)LEDColor.BlueOff)
            {
                //MessageBox.Show("Set backgroung image to: " + num.ToString());
                this.BackgroundImage = imageList1.Images[num];
            }
            else MessageBox.Show("Error: LED control, setImage - num > LEDColor.BlueOff.");
        }

    }
}
