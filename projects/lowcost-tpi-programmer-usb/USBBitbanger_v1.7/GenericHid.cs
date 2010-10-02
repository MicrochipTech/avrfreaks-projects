/// <summary>
///  Runs the application.
/// </summary> 
///  
using System;
using System.Collections;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Windows.Forms;
using System.Collections.Generic;

namespace GenericHid
{
    
    public class GenericHid  
    { 
        [STAThread()]
#if false
        public static void Main() 
        {
            USB_A25L40P usb = new USB_A25L40P();
            if (!usb.connect())
            {
                return;
            }

            byte[] id = usb.RDID();
            byte res = usb.RES();

            byte status = usb.RDSR();
            usb.WREN();
            status = usb.RDSR();
            usb.WRDI();
            status = usb.RDSR();

            byte[] memold = usb.READ(0x0, 1024);

            usb.WREN();
            status = usb.RDSR();
            //usb.SE(0x0);
            usb.RDSR_waitforWIPclear();

            byte[] memnew = usb.READ(0x0, 1024);
        } 
#endif
#if false
        public static void Main()
        {
            USB_ATTinyTPI usb = new USB_ATTinyTPI();
            if (!usb.connect())
            {
                return;
            }

            USB_ATTinyTPI.Processor proc = usb.CPU_identify();

            usb.NVM_command(USB_ATTinyTPI.NVMCommand.NO_OPERATION);
            usb.NVM_waitbusy();

            byte[] fuses = usb.CPU_configurationbits_read();
            byte[] locks = usb.CPU_nvmlockbits_read();
            byte[] devid = usb.CPU_deviceidbits_read();

            OpenFileDialog fd = new OpenFileDialog();
            fd.ShowDialog();
            string filename = fd.FileName;// "ATTiny10_helloworld.hex";
            if (filename == "")
            {
                return;
            }
            IntelHEXfile hexfile = new IntelHEXfile(filename);
            byte[] hexdata = hexfile.GetData();

            long tick1 = System.DateTime.Now.Ticks;
            usb.NVM_SECTION_ERASE(usb.CPU_flashaddress());
            usb.NVM_WORD_WRITE(usb.CPU_flashaddress(), hexdata);
            double tock1 = (System.DateTime.Now.Ticks - tick1) / 10000000.0;

            return;

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
                    flashedok = false;
                    break;
                }
            }

            usb.S_HIGH();
            usb.flush();
        }
#endif
#if false
        public static void Main()
        {
            USB_ATTinyTPI_12V usb = new USB_ATTinyTPI_12V();
            if (!usb.connect())
            {
                return;
            }

            USB_ATTinyTPI.Processor proc = usb.CPU_identify();

            usb.NVM_command(USB_ATTinyTPI.NVMCommand.NO_OPERATION);
            usb.NVM_waitbusy();

            byte[] fuses = usb.CPU_configurationbits_read();
            byte[] locks = usb.CPU_nvmlockbits_read();
            byte[] devid = usb.CPU_deviceidbits_read();

            OpenFileDialog fd = new OpenFileDialog();
            fd.ShowDialog();
            string filename = fd.FileName;// "ATTiny10_helloworld.hex";
            if (filename == "")
            {
                return;
            }
            IntelHEXfile hexfile = new IntelHEXfile(filename);
            byte[] hexdata = hexfile.GetData();

            long tick1 = System.DateTime.Now.Ticks;
            usb.NVM_SECTION_ERASE(usb.CPU_flashaddress());
            usb.NVM_WORD_WRITE(usb.CPU_flashaddress(), hexdata);
            double tock1 = (System.DateTime.Now.Ticks - tick1) / 10000000.0;

            return;

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
                    flashedok = false;
                    break;
                }
            }

            usb.S_HIGH();
            usb.flush();
        }
#endif
#if false
        public static void Main()
        {
            List<string> msgs = new List<string>();

            USB_24FC256 usb = new USB_24FC256();
            if (!usb.connect(msgs))
            {
                return;
            }

            usb.Address = 0;

            //usb.PageWrite(0, new byte[]{161,162,163,164,165});

            byte ss = usb.RandomRead(0);
            ss = usb.RandomRead(1);
            ss = usb.CurrentAddressRead();
 
            byte[] b = usb.SequentialRead(0, 512);
        }
#endif
#if true
        public static void Main()
        {
            FormFlash ff = new FormFlash();
            ff.ShowDialog();
        }
#endif
    }
}
