using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace GenericHid
{
    public partial class FormFlash : Form
    {
        public FormFlash()
        {
            InitializeComponent();

            comboBoxI2CFlashType.Items.Add("Microchip 24FC256");
        }

        private void buttonFlashHEXFile_Click(object sender, EventArgs e)
        {
            OpenFileDialog fd = new OpenFileDialog();
            fd.FileName = textBoxFlashHEXFile.Text;
            fd.Filter = "Flash Intel Hex Files (*.hex)|*.hex";
            fd.ShowDialog();
            textBoxFlashHEXFile.Text = fd.FileName;
        }

        private void postMessages(List<string> msgs)
        {
            foreach (string s in msgs)
            {
                postMessage(s);
            }
        }

        private void postMessage(string s)
        {
            listBoxProgramMessages.Items.Add(s);
            listBoxProgramMessages.Refresh();
        }

        private void buttonFlashProgram_Click(object sender, EventArgs e)
        {
            listBoxProgramMessages.Items.Clear();

            USB_ATTinyTPI usb = createInterface();
            List<string> msgs = new List<string>();
            bool connected = usb.connect(msgs);
            postMessages(msgs);

            if ( !connected )
            {
                postMessage("Target: connection failed");
                return;
            }
            postMessage("Target: connected");

            USB_ATTinyTPI.Processor proc = usb.CPU_identify();
            listBoxProgramMessages.Items.Add("CPU = " + proc.ToString());

            usb.NVM_command(USB_ATTinyTPI.NVMCommand.NO_OPERATION);
            usb.NVM_waitbusy();

            postMessage("Reading HEX file");
            IntelHEXfile hexfile = new IntelHEXfile(textBoxFlashHEXFile.Text);
            byte[] hexdata = hexfile.GetData();

            if (checkBoxEraseBeforeProgram.Checked)
            {
                postMessage("Erasing device");
                usb.NVM_SECTION_ERASE(usb.CPU_flashaddress());
            }

            postMessage("Flashing " + hexdata.Length.ToString() + " bytes");
            long tick1 = System.DateTime.Now.Ticks;
            if (checkBoxProgramFastFlash.Checked)
            {
                usb.NVM_WORD_WRITE_fast(usb.CPU_flashaddress(), hexdata);
            }
            else
            {
                usb.NVM_WORD_WRITE(usb.CPU_flashaddress(), hexdata);
            }
            double tock1 = (System.DateTime.Now.Ticks - tick1) / 10000000.0;
            postMessage("Flashing took " + tock1.ToString() + "s");

            if (checkBoxVerifyAfterProgram.Checked)
            {
                postMessage("Verifying device");
                long tick2 = System.DateTime.Now.Ticks;
                byte[] flashprogrammemory = usb.CPU_readbytes(0x4000, hexdata.Length);//usb.CPU_flashsize());
                // 64 bytes : 26.5s
                // 40 bytes : 20.0s
                // 32 bytes : 18.0s
                // 24 bytes : 16.1s
                // 16 bytes : 13.9s
                //  9 bytes : 13.9s
                double tock2 = (System.DateTime.Now.Ticks - tick2) / 10000000.0;
                bool flashedok = true;
                for (int i = 0; i < hexdata.Length; i++)
                {
                    if (flashprogrammemory[i] != hexdata[i])
                    {
                        postMessage("Verification failed at address " + i);
                        flashedok = false;
                        break;
                    }
                }
                if (flashedok)
                {
                    postMessage("Verification successful");
                }
                postMessage("Verification took " + tock2.ToString() + "s");
            }

            postMessage("Deselecting device");
            usb.S_HIGH();
            usb.flush();
        }

        private USB_ATTinyTPI createInterface()
        {
            if (checkBox12V.Checked)
            {
                return new USB_ATTinyTPI_12V();
            }

            return new USB_ATTinyTPI();
        }

        private void buttonReadSignature_Click(object sender, EventArgs e)
        {
            listBoxProgramMessages.Items.Clear(); 
            textBoxSignature.Text = "Signature not read";

            USB_ATTinyTPI usb = createInterface();
            List<string> msgs = new List<string>();
            bool connected = usb.connect(msgs);
            postMessages(msgs);

            if (!connected)
            {
                postMessage("Target: connection failed");
                return;
            }
            postMessage("Target: connected");

            usb.NVM_command(USB_ATTinyTPI.NVMCommand.NO_OPERATION);
            usb.NVM_waitbusy();

            byte[] devid = usb.CPU_deviceidbits_read();
            string sigbytes = string.Format("0x{0:X02} 0x{1:X02} 0x{2:X02}", devid[0], devid[1], devid[2]);
            textBoxSignature.Text = sigbytes;
            postMessage("Signature is " + sigbytes);

            USB_ATTinyTPI.Processor proc = usb.CPU_identify();
            postMessage("CPU is " + proc.ToString());

            postMessage("Deselecting device");
            usb.S_HIGH();
            usb.flush();
        }

        private void buttonFusesRead_Click(object sender, EventArgs e)
        {
            listBoxProgramMessages.Items.Clear();
            textBoxFusesHIGH.Text = "";
            textBoxFusesLOW.Text = "";

            USB_ATTinyTPI usb = createInterface();
            List<string> msgs = new List<string>();
            bool connected = usb.connect(msgs);
            postMessages(msgs);

            if (!connected)
            {
                postMessage("Target: connection failed");
                return;
            }
            postMessage("Target: connected");

            usb.NVM_command(USB_ATTinyTPI.NVMCommand.NO_OPERATION);
            usb.NVM_waitbusy();

            byte[] conf = usb.CPU_configurationbits_read();
            string conflo = string.Format("0x{0:X02}", conf[0]);
            string confhi = string.Format("0x{0:X02}", conf[1]);
            textBoxFusesLOW.Text = conflo;
            textBoxFusesHIGH.Text = confhi;

            string confbytes = string.Format("0x{0:X02} 0x{1:X02}", conf[0], conf[1]);
            postMessage("Configuration is " + confbytes);

            postMessage("Deselecting device");
            usb.S_HIGH();
            usb.flush();
        }

        private void buttonFusesWrite_Click(object sender, EventArgs e)
        {
            listBoxProgramMessages.Items.Clear();

            USB_ATTinyTPI usb = createInterface();
            List<string> msgs = new List<string>();
            bool connected = usb.connect(msgs);
            postMessages(msgs);

            if (!connected)
            {
                postMessage("Target: connection failed");
                return;
            }
            postMessage("Target: connected");

            usb.NVM_command(USB_ATTinyTPI.NVMCommand.NO_OPERATION);
            usb.NVM_waitbusy();

            byte[] conf = usb.CPU_configurationbits_read();
            string confbytes = string.Format("0x{0:X02} 0x{1:X02}", conf[0], conf[1]);
            postMessage("Configuration was " + confbytes);

            byte conflo = Convert.ToByte(textBoxFusesLOW.Text, 16);
            byte confhi = Convert.ToByte(textBoxFusesHIGH.Text, 16);
            byte[] newconf = {conflo, confhi};
            usb.NVM_SECTION_ERASE(usb.CPU_configurationbits_address());
            usb.NVM_WORD_WRITE(usb.CPU_configurationbits_address(), newconf);

            conf = usb.CPU_configurationbits_read();
            confbytes = string.Format("0x{0:X02} 0x{1:X02}", conf[0], conf[1]);
            postMessage("Configuration is " + confbytes);

            postMessage("Deselecting device");
            usb.S_HIGH();
            usb.flush();
        }

        private void buttonProgramVerify_Click(object sender, EventArgs e)
        {
            listBoxProgramMessages.Items.Clear();

            USB_ATTinyTPI usb = createInterface();
            List<string> msgs = new List<string>();
            bool connected = usb.connect(msgs);
            postMessages(msgs);

            if (!connected)
            {
                postMessage("Target: connection failed");
                return;
            }
            postMessage("Target: connected");

            USB_ATTinyTPI.Processor proc = usb.CPU_identify();
            listBoxProgramMessages.Items.Add("CPU = " + proc.ToString());

            usb.NVM_command(USB_ATTinyTPI.NVMCommand.NO_OPERATION);
            usb.NVM_waitbusy();

            postMessage("Reading HEX file");
            IntelHEXfile hexfile = new IntelHEXfile(textBoxFlashHEXFile.Text);
            byte[] hexdata = hexfile.GetData();

            postMessage("Verifying device");
            long tick2 = System.DateTime.Now.Ticks;
            byte[] flashprogrammemory = usb.CPU_readbytes(0x4000, hexdata.Length);//usb.CPU_flashsize());
            // 64 bytes : 26.5s
            // 40 bytes : 20.0s
            // 32 bytes : 18.0s
            // 24 bytes : 16.1s
            // 16 bytes : 13.9s
            //  9 bytes : 13.9s
            double tock2 = (System.DateTime.Now.Ticks - tick2) / 10000000.0;
            bool flashedok = true;
            for (int i = 0; i < hexdata.Length; i++)
            {
                if (flashprogrammemory[i] != hexdata[i])
                {
                    postMessage("Verification failed at address " + i);
                    flashedok = false;
                    break;
                }
            }
            if (flashedok)
            {
                postMessage("Verification successful");
            }
            postMessage("Verification took " + tock2.ToString() + "s");

            postMessage("Deselecting device");
            usb.S_HIGH();
            usb.flush();
        }

        private void buttonProgramRead_Click(object sender, EventArgs e)
        {
            listBoxProgramMessages.Items.Clear();

            USB_ATTinyTPI usb = createInterface();
            List<string> msgs = new List<string>();
            bool connected = usb.connect(msgs);
            postMessages(msgs);

            if (!connected)
            {
                postMessage("Target: connection failed");
                return;
            }
            postMessage("Target: connected");

            USB_ATTinyTPI.Processor proc = usb.CPU_identify();
            listBoxProgramMessages.Items.Add("CPU = " + proc.ToString());

            usb.NVM_command(USB_ATTinyTPI.NVMCommand.NO_OPERATION);
            usb.NVM_waitbusy();

            postMessage("Reading device at address " + usb.CPU_flashaddress() + " number of bytes " + usb.CPU_flashsize());
            long tick2 = System.DateTime.Now.Ticks;
            byte[] flashprogrammemory = usb.CPU_readbytes(usb.CPU_flashaddress(), usb.CPU_flashsize());
            double tock2 = (System.DateTime.Now.Ticks - tick2) / 10000000.0;
            bool flashedok = true;
            postMessage("Reading took " + tock2.ToString() + "s");

            SaveFileDialog fd = new SaveFileDialog();
            fd.AddExtension = true;
            fd.Filter = "Raw Binary File (*.bin)|*.bin";
            DialogResult res = fd.ShowDialog();
            if (res == DialogResult.OK)
            {
                RawBinaryfile rbf = new RawBinaryfile(flashprogrammemory);
                rbf.SaveFile( fd.FileName);
            }
        }

        private void buttonLockBitsRead_Click(object sender, EventArgs e)
        {
            listBoxProgramMessages.Items.Clear();
            textBoxLockBitsLOW.Text = "";

            USB_ATTinyTPI usb = createInterface();
            List<string> msgs = new List<string>();
            bool connected = usb.connect(msgs);
            postMessages(msgs);

            if (!connected)
            {
                postMessage("Target: connection failed");
                return;
            }
            postMessage("Target: connected");

            usb.NVM_command(USB_ATTinyTPI.NVMCommand.NO_OPERATION);
            usb.NVM_waitbusy();

            byte[] conf = usb.CPU_nvmlockbits_read();
            string conflo = string.Format("0x{0:X02}", conf[0]);
            textBoxLockBitsLOW.Text = conflo;

            string confbytes = string.Format("0x{0:X02} 0x{1:X02}", conf[0], conf[1]);
            postMessage("LockBits are " + confbytes);

            postMessage("Deselecting device");
            usb.S_HIGH();
            usb.flush();
        }

        private void buttonLockBitsWrite_Click(object sender, EventArgs e)
        {
            listBoxProgramMessages.Items.Clear();

            USB_ATTinyTPI usb = createInterface();
            List<string> msgs = new List<string>();
            bool connected = usb.connect(msgs);
            postMessages(msgs);

            if (!connected)
            {
                postMessage("Target: connection failed");
                return;
            }
            postMessage("Target: connected");

            usb.NVM_command(USB_ATTinyTPI.NVMCommand.NO_OPERATION);
            usb.NVM_waitbusy();

            byte[] conf = usb.CPU_nvmlockbits_read();
            string confbytes = string.Format("0x{0:X02} 0x{1:X02}", conf[0], conf[1]);
            postMessage("LockBits were " + confbytes);

            byte conflo = Convert.ToByte(textBoxLockBitsLOW.Text, 16);
            byte confhi = 255;
            byte[] newconf = { conflo, confhi };
            postMessage("Writing LockBits");
            usb.NVM_SECTION_ERASE(usb.CPU_nvmlockbits_address());
            usb.NVM_WORD_WRITE(usb.CPU_nvmlockbits_address(), newconf);

            conf = usb.CPU_nvmlockbits_read();
            confbytes = string.Format("0x{0:X02} 0x{1:X02}", conf[0], conf[1]);
            postMessage("LockBits are " + confbytes);

            if (conf[0] != newconf[0] || conf[1] != newconf[1])
            {
                postMessage("LockBits were not set. Will erase the entire chip");
                usb.NVM_CHIP_ERASE();
                usb.NVM_WORD_WRITE(usb.CPU_nvmlockbits_address(), newconf);

                conf = usb.CPU_nvmlockbits_read();
                confbytes = string.Format("0x{0:X02} 0x{1:X02}", conf[0], conf[1]);
                postMessage("LockBits are " + confbytes);
            }

            postMessage("Deselecting device");
            usb.S_HIGH();
            usb.flush();
        }

        private void buttonAdvancedTone_Click(object sender, EventArgs e)
        {
            listBoxProgramMessages.Items.Clear();

            USBBitbanger usb = new USBBitbanger();
            List<string> msgs = new List<string>();
            bool connected = usb.connect(msgs);
            postMessages(msgs);

            if (!connected)
            {
                postMessage("Programmer: connection failed");
                return;
            }
            postMessage("Programmer: connected");

            // mess up parity and put the TPI into error mode
            usb.senddata(0x84, 0x84);

            byte del = 125;     // [us]
            double freq = (1.0 / (del * 1e-6) * 0.5) * 0.25;
            postMessage("Playing " + freq + "Hz tone on the TPICLK pin");
            postMessage("Playing " + freq*0.5 + "Hz tone on the TPIDATA pin");
            usb.SET_DELAY(del);
            for (int i = 0; i < 2000; i++)
            {
//                usb.senddata(new byte[]{85,85,85,85, 85,85,85,85});
                usb.senddata(85);
            }

            // recover from error mode
            usb.SET_DELAY(0);

            postMessage("Deselecting device");
            usb.S_HIGH();
            usb.flush();
        }

        private void buttonI2CHexFile_Click(object sender, EventArgs e)
        {
            OpenFileDialog fd = new OpenFileDialog();
            fd.FileName = textBoxFlashHEXFile.Text;
            fd.Filter = "Raw Binary File (*.*)|*.*";
            fd.ShowDialog();
            textBoxI2CFlashHEXFile.Text = fd.FileName;
        }

        private void buttonI2Cprogram_Click(object sender, EventArgs e)
        {
            listBoxProgramMessages.Items.Clear();

            USB_I2C_Flash i2cflash = null;
            switch (comboBoxI2CFlashType.Text)
            {
                case "Microchip 24FC256":
                    i2cflash = new USB_24FC256();
                    break;
            }
            if (i2cflash == null)
            {
                postMessage("No flash type selected");
                return;
            }

            List<string> msgs = new List<string>();
            bool connected = i2cflash.connect(msgs);
            postMessages(msgs);

            if (!connected)
            {
                postMessage("Programmer: connection failed");
                return;
            }
            postMessage("Programmer: connected");

            int flashsiz = i2cflash.FlashSize();
            postMessage("Flash chip info: " + flashsiz.ToString() + " bytes flash memory");

            postMessage("Reading binary file");
            RawBinaryfile rbf = new RawBinaryfile(textBoxI2CFlashHEXFile.Text);
            byte[] hexdata = rbf.GetData();
            postMessage("File info: " + hexdata.Length + " bytes data");

            bool fastWrite = checkBoxI2CfastFlash.Checked;

            int address = 0;
            long tick2 = System.DateTime.Now.Ticks;
            while ((address < flashsiz) && (address < hexdata.Length))
            {
                int pagesiz = i2cflash.PageSize(address);
                byte[] buff = new byte[pagesiz];
                int copysiz = Math.Min(pagesiz, hexdata.Length - address);
                Array.Copy(hexdata, address, buff, 0, copysiz);

                //postMessage("Writing data at " + address.ToString());
                i2cflash.PageWrite(address, buff, fastWrite);

                address += pagesiz;
            }
            double tock2 = (System.DateTime.Now.Ticks - tick2) / 10000000.0;
            postMessage("Writing took " + tock2.ToString() + "s");

            i2cflash.condition_STOP();
        }

        private void buttonI2Cread_Click(object sender, EventArgs e)
        {
            listBoxProgramMessages.Items.Clear();

            USB_I2C_Flash i2cflash = null;
            switch (comboBoxI2CFlashType.Text)
            {
                case "Microchip 24FC256":
                    i2cflash = new USB_24FC256();
                    break;
            }
            if (i2cflash == null)
            {
                postMessage("No flash type selected");
                return;
            }

            List<string> msgs = new List<string>();
            bool connected = i2cflash.connect(msgs);
            postMessages(msgs);

            if (!connected)
            {
                postMessage("Programmer: connection failed");
                return;
            }
            postMessage("Programmer: connected");

            int flashsiz = i2cflash.FlashSize();
            postMessage("FlashInfo: " + flashsiz.ToString() + " bytes flash");

            postMessage("Reading device at address 0 number of bytes " + flashsiz.ToString());
            long tick2 = System.DateTime.Now.Ticks;
            byte[] flashprogrammemory = i2cflash.SequentialRead(0, flashsiz);
            double tock2 = (System.DateTime.Now.Ticks - tick2) / 10000000.0;
            postMessage("Reading took " + tock2.ToString() + "s");

            SaveFileDialog fd = new SaveFileDialog();
            fd.AddExtension = true;
            fd.Filter = "Raw Binary File (*.bin)|*.bin";
            DialogResult res = fd.ShowDialog();
            if (res == DialogResult.OK)
            {
                RawBinaryfile rbf = new RawBinaryfile(flashprogrammemory);
                rbf.SaveFile(fd.FileName);
            }
        }

    }
}
