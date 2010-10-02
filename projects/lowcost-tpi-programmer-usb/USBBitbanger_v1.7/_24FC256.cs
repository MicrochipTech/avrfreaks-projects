using System;
using System.Collections.Generic;
using System.Text;

namespace GenericHid
{
    class USB_24FC256 : USB_I2C_Flash
    {
        public USB_24FC256()
            : base()
        {
        }

        public override bool connect(List<string> msgs)
        {
            bool result = base.connect(msgs);

            condition_START();
            READ();
            bool isAck = isAcknowledged();
            condition_STOP();
            if (!isAck)
            {
                msgs.Add("Device does not acknowledge READ command");
                return false;
            }


            // leaving with SCL_HIGH and SDA_HIGH (bus idle)
            return result;
        }

        byte mAddress = 0;
        public byte Address
        {
            set { mAddress = value; }
            get { return mAddress; }
        }

        public override byte RandomRead(int address)
        {
            condition_START();
            WRITE();
            bool isACK = isAcknowledged();    // recv ACK
            senddata(new byte[] { (byte)(address>>8) });
            isACK = isAcknowledged();         // recv ACK
            senddata(new byte[] { (byte)(address&255) });
            isACK = isAcknowledged();         // recv ACK
            return CurrentAddressRead();
        }

        public byte CurrentAddressRead()
        {
            condition_START();
            READ();
            bool isACK = isAcknowledged();      // recv ACK
            byte data = receivedata(1)[0];
            NOACK();                            // send NO ACK
            condition_STOP();
            return data;
        }

        public override byte[] SequentialRead(int address, int numBytes)
        {
            List<byte> data = new List<byte>();

            condition_START();
            WRITE();
            bool isACK = isAcknowledged();      // recv ACK
            senddata(new byte[] { (byte)(address >> 8) });
            isACK = isAcknowledged();           // recv ACK
            senddata(new byte[] { (byte)(address & 255) });
            isACK = isAcknowledged();           // recv ACK

            condition_START();
            READ();
            isACK = isAcknowledged();       // recv ACK
            if (numBytes > 1)
            {
                data.AddRange(batchreceive(numBytes - 1));
            }
            data.Add(receivedata(1)[0]);
            NOACK();                        // send NO ACK
            condition_STOP();

            return data.ToArray();
        }

        public byte[] batchreceive(int numBytes)
        {
            List<byte> data = new List<byte>();
#if false
            while (numBytes > 0)
            {
                data.Add(receivedata(1)[0]);
                ACK();
                SDA_HIGH();
                numBytes--;
            }
#else
            flush();
            async_clear();

            for(int i=0; i<numBytes; i++)
            {
                async_receive(1);

                async_C_LOW();  // ACK
                async_D_LOW();
                async_C_HIGH();
                async_C_LOW();

                async_D_HIGH(); // SDA_HIGH
            }
            for (int i = 0; i < numBytes; i++)
            {
                Bitbuffer tmp = async_getBuffer();
                data.Add( tmp.dequeuebyte_MSBfirst());
            }
#endif
            return data.ToArray();
        }

        public override void ByteWrite(int address, byte b)
        {
            condition_START();
            WRITE();
            bool isACK = isAcknowledged();      // recv ACK
            senddata(new byte[] { (byte)(address >> 8) });
            isACK = isAcknowledged();           // recv ACK
            senddata(new byte[] { (byte)(address & 255) });
            isACK = isAcknowledged();           // recv ACK
            senddata(new byte[] { b });
            isACK = isAcknowledged();           // recv ACK
            condition_STOP();
        }

        public override void PageWrite(int address, byte[] bs, bool fastWrite)   // 64 bytes in a page
        {
            condition_START();
            WRITE();
            bool isACK = true;
            if (fastWrite)
            {
                skipACK();
            }
            else
            {
                isACK = isAcknowledged();      // recv ACK
            }
            senddata(new byte[] { (byte)(address >> 8) });
            if (fastWrite)
            {
                skipACK();
            }
            else
            {
                isACK = isAcknowledged();      // recv ACK
            }
            senddata(new byte[] { (byte)(address & 255) });
            if (fastWrite)
            {
                skipACK();
            }
            else
            {
                isACK = isAcknowledged();      // recv ACK
            }

            if (fastWrite)
            {
                Bitbuffer buf = new Bitbuffer();
                foreach (byte b in bs)
                {
                    buf.enqueuebyte_MSBfirst(b);    // send byte
                    buf.enqueuebit(1);              // skip ACK
                }
                sendbits(buf);
            }
            else
            {
                foreach (byte b in bs)
                {
                    senddata(new byte[] { b });
                    if (fastWrite)
                    {
                        skipACK();
                    }
                    else
                    {
                        isACK = isAcknowledged();           // recv ACK
                    }
                }
            }
            condition_STOP();
            if (fastWrite)
            {
                System.Threading.Thread.Sleep(4);       // write time = 5ms
            }
            else
            {
                WaitBusy();
            }
            condition_STOP();
        }

        public override void WaitBusy()
        {
            condition_START();
            WRITE();
            bool isACK = isAcknowledged();
            while (!isACK)
            {
                condition_START();
                WRITE();
                isACK = isAcknowledged();
            }
        }

        public void READ()
        {
            senddata(new byte[] { (byte)(128 + 32 + 2*mAddress + 1) });
        }

        public void WRITE()
        {
            senddata(new byte[] { (byte)(128 + 32 + 2*mAddress + 0) });
        }

        public override int PageSize(int address)
        {
            return 64;
        }

        public override int FlashSize()
        {
            return 32768;
        }
    }
}
