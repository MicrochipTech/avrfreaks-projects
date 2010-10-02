using System;
using System.Collections.Generic;
using System.Text;

// we are using SDA_HIGH to simulate a pull-up on the data-line

namespace GenericHid
{
    class USB_I2C : USBBitbanger
    {
        public USB_I2C()
            : base()
        {
        }

        public override bool connect(List<string> msgs)
        {
            bool result = base.connect(msgs);

            if (result)
            {
                byte oldspeed = GET_DELAY();
                SET_DELAY(0);
                byte newspeed = GET_DELAY();
            }

            if ( result )
            {
                condition_STOP();
                condition_STOP();

                // test
                D_LOW();
                byte b = Q_READ();
                if (b != 0)
                {
                    msgs.Add("Programmer: self test failed. Connect both D and Q to the SDA pin");
                    result = false;
                }
                D_HIGH();
                b = Q_READ();
                if (b != 1)
                {
                    msgs.Add("Programmer: self test failed. Connect both D and Q to the SDA pin");
                    result = false;
                }
            }

            // leaving with SCL_HIGH and SDA_HIGH (bus idle)
            return result;
        }

        public void SDA_HIGH()
        {
            D_HIGH();
        }

        public void SDA_LOW()
        {
            D_LOW();
        }

        public void SCL_HIGH()
        {
            C_HIGH();
        }

        public void SCL_LOW()
        {
            C_LOW();
        }

        public byte SDA_READ()
        {
            return Q_READ();
        }

        public void ACK()
        {
            SCL_LOW();
            SDA_LOW();
            SCL_HIGH();
            SCL_LOW();
        }

        public void NOACK()
        {
            SCL_LOW();
            SDA_HIGH();
            SCL_HIGH();
            SCL_LOW();
        }

        public void condition_STOP()
        {
            SCL_LOW();
            SDA_LOW();      // SDA:LO-HI while SCL HI
            SCL_HIGH();
            SDA_HIGH();
        }

        public void condition_START()
        {
            //SCL_LOW();      // RESTART
            //SDA_HIGH();
            // start condition
            SCL_HIGH();
            SDA_LOW();  // SDA:HI-LO while SCL HI 
            SCL_HIGH();
        }

        public bool isAcknowledged()
        {
            SCL_LOW();
            SDA_HIGH();
            SCL_HIGH();
            byte b = SDA_READ();
            SCL_LOW();      // slave releases SDA
            return b == 0;
        }

        public bool skipACK()
        {
            SCL_LOW();
            SCL_HIGH();
            SCL_LOW();      // slave releases SDA
            return true;
        }
    }
}
