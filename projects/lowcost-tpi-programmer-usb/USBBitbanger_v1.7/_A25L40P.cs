using System;
using System.Collections.Generic;
using System.Text;

namespace GenericHid
{
    class USB_A25L40P : USBBitbanger
    {
        public USB_A25L40P()
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

                byte[] id = RDID();
                if (id[0] != 0x7f ||
                     id[1] != 0x37 ||
                     id[2] != 0x20 ||
                     id[3] != 0x13 )
                {
                    msgs.Add("Wrong ID returned : " + id.ToString());
                    result = false;
                }
            }

            return result;
        }

        public void WREN()
        {
            S_LOW();
            senddata(0x06);                     // WREN
            S_HIGH();
            flush();
        }

        public void WRDI()
        {
            S_LOW();
            senddata(0x04);                     // WRDI
            S_HIGH();
            flush();
        }

        public byte RDSR()
        {
            S_LOW();
            senddata(0x05);                     // RDSR
            byte[] dat = receivedata(1);
            S_HIGH();
            flush();
            return dat[0];
        }

        public void RDSR_waitforWIPclear()
        {
            S_LOW();
            while ((RDSR() & 0x01) == 0x01) { };
            S_HIGH();
            flush();
        }

        public void WRSR(byte status)
        {
            S_LOW();
            senddata(0x01, status);             // WRSR
            S_HIGH();
            flush();
        }

        public byte[] READ(long address, int length)
        {
            S_LOW();
            senddata(0x03, (byte)((address >> 16) & 0xff),
                           (byte)((address >> 8) & 0xff),
                           (byte)(address & 0xff));// READ
            byte[] dat = receivedata(length);
            S_HIGH();
            flush();
            return dat;
        }

        public void PP(long address, byte[] data)
        {
            S_LOW();
            senddata(0x02, (byte)((address >> 16) & 0xff),
                           (byte)((address >> 8) & 0xff),
                           (byte)(address & 0xff)); // PP
            senddata(data);
            S_HIGH();
            flush();
        }

        public void SE(long address)
        {
            S_LOW();
            senddata(0xD8, (byte)((address >> 16) & 0xff),
                           (byte)((address >> 8) & 0xff),
                           (byte)(address & 0xff)); // SE
            S_HIGH();
            flush();
        }

        public void BE()
        {
            S_LOW();
            senddata(0xC7);                     // BE
            S_HIGH();
            flush();
        }

        public void DP()
        {
            S_LOW();
            senddata(0xB9);                     // DP
            S_HIGH();
            flush();
        }

        public byte[] RDID()
        {
            S_LOW();
            senddata(0x9f);                     // RDID
            byte[] dat = receivedata(4);
            S_HIGH();
            flush();
            return dat;
        }

        public byte RES()
        {
            S_LOW();
            senddata(0xab, 0xab, 0xab, 0xab);   // RES (might wake up from deep-sleep: 3 dummy bytes)
            byte[] dat = receivedata(1);
            S_HIGH();
            flush();
            return dat[0];
        }
    }
}
