using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace DeveloperTerminal
{
    public partial class TextBoxSettings : Form
    {
        #region Initialization
        public TextBoxSettings()
        {
            InitializeComponent();
        }
        #endregion

        #region Assessors
        private Color BackColr = Color.White;
        public Color backColor
        {
            get
            {
                return BackColr;
            }
        }

        private Color ForeColr = Color.Black;
        public Color foreColor
        {
            get
            {
                return ForeColr;
            }
        }

        private Font Fnt = null;
        public Font font
        {
            get
            {
                return Fnt;
            }
        }
        #endregion

        #region Components
        private void buttonFont_Click(object sender, EventArgs e)
        {
            fontDialog1.ShowDialog();
            Fnt = fontDialog1.Font;
        } 
        
        private void buttonForeColor_Click(object sender, EventArgs e)
        {
            colorDialog1.ShowDialog();
            ForeColr = colorDialog1.Color;
        }

        private void buttonBackColor_Click(object sender, EventArgs e)
        {
            colorDialog1.ShowDialog();
            BackColr = colorDialog1.Color;
        }

        private void buttonOkay_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
        }
        #endregion
    }
}