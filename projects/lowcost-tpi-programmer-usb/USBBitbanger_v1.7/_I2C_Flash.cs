using System;
using System.Collections.Generic;
using System.Text;

namespace GenericHid
{
    class USB_I2C_Flash : USB_I2C
    {
        public USB_I2C_Flash()
            : base()
        {
        }

        public override bool connect(List<string> msgs)
        {
            return base.connect(msgs);
        }

        public virtual byte RandomRead(int address)
        {
            return (byte)0;
        }

        public virtual byte[] SequentialRead(int address, int numBytes)
        {
            return new byte[numBytes];
        }

        public virtual void ByteWrite(int address, byte b)
        {
        }

        public virtual void PageWrite(int address, byte[] bs, bool fastWrite)
        {
        }

        public virtual int PageSize(int address)
        {
            return 256;
        }

        public virtual int FlashSize()
        {
            return 0;
        }

        public virtual void WaitBusy()
        {
        }
    }
}
