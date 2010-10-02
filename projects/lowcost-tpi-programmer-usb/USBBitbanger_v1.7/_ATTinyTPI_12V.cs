using System;
using System.Collections.Generic;
using System.Text;

namespace GenericHid
{
    class USB_ATTinyTPI_12V : USB_ATTinyTPI
    {
        public USB_ATTinyTPI_12V()
            : base()
        {
        }

        public override bool connect(List<string> msgs)
        {
            bool result = do_connect(msgs);

            try
            {

                if (result)
                {
                    // TPI works at 2MHz = 0.5us
                    SET_DELAY(0);
                    S_LOW();
                    C_HIGH();
                    //usb.D_HIGH();
                    S_HIGH();
                    flush();
                }

                if (result)
                {
                    TPI_enable();
                    // TPI enabled... pins are tristated

                    // test
                    D_LOW();
                    byte b = Q_READ();
                    if (b != 0)
                    {
                        msgs.Add("Programmer: self test failed. Connect both D and Q to the TPIDATA pin");
                        result = false;
                    }
                    D_HIGH();
                    b = Q_READ();
                    if (b != 1)
                    {
                        msgs.Add("Programmer: self test failed. Connect both D and Q to the TPIDATA pin");
                        result = false;
                    }
                }

                if (result)
                {
                    // TPIIR (address 0x0f) == 0x80 ?
                    byte TPIIR = TPI_SLDCS(0x0f);
                    if (TPIIR != 0x80)
                    {
                        msgs.Add("Target: TPIIR reads back value " + TPIIR.ToString() + " expected 128");
                        result = false;
                    }
                }

                if (result)
                {
                    // reduce the idle bits...
                    TPI_TPIPCR_setidlebits(0x07);
                    byte ibits = TPI_TPIPCR_getidlebits();
                }

                if (result)
                {
                    NVM_enable();
                    if (CPU_identify() == Processor.UNKNOWN)
                    {
                        msgs.Add("Target: Cannot identify CPU");
                        result = false;
                    }
                }

            }
            catch (System.Exception e)
            {
                msgs.Add(e.Message);
                result = false;
            }

            return result;
        }

        public override void TPI_enable()
        {
            // wait t_TOUT after power on 
            System.Threading.Thread.Sleep(128 + 1);
            // apply 12V
            S_HIGH();
            flush();
            // wait t_RST after reset
            System.Threading.Thread.Sleep(2 + 1); // at VCC=1.8V
            // send 16 clockcycles with data high
            senddata(0xff, 0xff);
            flush();
        }

        public override void TPI_disable()
        {
            // turn off 12V
            S_LOW();
            C_HIGH();
            D_HIGH();
            flush();
        }
    }
}
