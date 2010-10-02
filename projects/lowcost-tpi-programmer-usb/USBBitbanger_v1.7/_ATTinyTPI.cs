using System;
using System.Collections.Generic;
using System.Text;

namespace GenericHid
{
    class USB_ATTinyTPI : USBBitbanger
    {
        public enum Processor
        {
            UNKNOWN=-1,
            ATTiny4,
            ATTiny5,
            ATTiny9,
            ATTiny10
        };

        public enum ATTiny4Fuses
        {
            RSTDISBL = 1,
            WDTON = 2,
            CKOUT = 4
        };

        public USB_ATTinyTPI()
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
                    S_HIGH();
                    C_HIGH();
                    //usb.D_HIGH();
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
                    _TPIidlebits = TPI_TPIPCR_getidlebits();
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

        public virtual void TPI_enable()
        {
            // wait t_TOUT after power on 
            System.Threading.Thread.Sleep(128 + 1);
            // pull reset low
            S_LOW();
            flush();
            // wait t_RST after reset
            System.Threading.Thread.Sleep(2 + 1); // at VCC=1.8V
            // send 16 clockcycles with data high
            senddata(0xff, 0xff);
            flush();
        }

        public virtual void TPI_disable()
        {
            // pull reset high
            S_HIGH();
            C_HIGH();
            D_HIGH();
            flush();
        }

        public void TPI_IDLE(int numidlebits)
        {
            Bitbuffer buf = new Bitbuffer();
            for (int i = 0; i < numidlebits; i++)
            {
                buf.enqueuebit(1);
            }
            sendbits(buf);
        }

        public void TPI_BREAK()
        {
            Bitbuffer buf = new Bitbuffer();
            for (int i = 0; i < 12; i++)
            {
                buf.enqueuebit(1);
            }
            sendbits(buf);
        }

        public byte TPI_TPIPCR_getidlebits()
        {
            byte bits = 128;
            byte TPICR = TPI_SLDCS(0x02);
            switch (TPICR & 0x7)
            {
                case 0:
                    bits = 128;
                    break;
                case 1:
                    bits = 64;
                    break;
                case 2:
                    bits = 32;
                    break;
                case 3:
                    bits = 16;
                    break;
                case 4:
                    bits = 8;
                    break;
                case 5:
                    bits = 4;
                    break;
                case 6:
                    bits = 2;
                    break;
                case 7:
                    bits = 0;
                    break;
            }
            _TPIidlebits = bits + 2;
            return (byte)_TPIidlebits;
        }

        public void TPI_TPIPCR_setidlebits(byte GT)
        {
            byte oldval = TPI_SLDCS(0x02);
            byte newval = (byte)((oldval & (0xff - 0x07)) | (GT & 0x07));
            TPI_SSTCS(0x02, newval);
        }

        int _TPIidlebits = 128;
        long _TPIaddress = 0x0000;
        Processor _processor = Processor.UNKNOWN;

        public long TPI_SLDPR()
        {
            return _TPIaddress;
        }
        
        public void TPI_SSTPR(long address)
        {
            TPI_sendbyte(0x68);
            TPI_sendbyte((byte)(address & 0xff));
            TPI_sendbyte(0x69);
            TPI_sendbyte((byte)((address >> 8) & 0xff));
            _TPIaddress = address;
        }

        public byte TPI_SLD(bool postincrement)
        {
            if (postincrement)
            {
                TPI_sendbyte(0x24);
                _TPIaddress++;
            }
            else
            {
                TPI_sendbyte(0x20);
            }
            return TPI_receivebyte_skipIDLE();
        }

        public byte[] TPI_SLD_2bytes()
        {
            Bitbuffer rcv1 = new Bitbuffer(new byte[2], 16);
            Bitbuffer rcv2 = new Bitbuffer(new byte[2], 16);

            sendNreceive(TPI_constructsendmessage(0x24), rcv1, TPI_constructsendmessage(0x24), rcv2 );
            _TPIaddress+=2;

            rcv1.discardUntil0();
            if ( rcv1.length< 9 )
            {
                // ooops
            }
            rcv1.dequeuebit();          // ST

            rcv2.discardUntil0();
            if ( rcv2.length< 9 )
            {
                // ooops
            }
            rcv2.dequeuebit();          // ST

            return new byte[] { rcv1.dequeuebyte_LSBfirst(), rcv2.dequeuebyte_LSBfirst() };
        }

        public byte[] TPI_SLD_3bytes()
        {
            Bitbuffer rcv1 = new Bitbuffer(new byte[2], 16);
            Bitbuffer rcv2 = new Bitbuffer(new byte[2], 16);
            Bitbuffer rcv3 = new Bitbuffer(new byte[2], 16);

            sendNreceive(TPI_constructsendmessage(0x24), rcv1, 
                         TPI_constructsendmessage(0x24), rcv2,
                         TPI_constructsendmessage(0x24), rcv3);
            _TPIaddress += 3;

            rcv1.discardUntil0();
            if (rcv1.length < 9)
            {
                // ooops
            }
            rcv1.dequeuebit();          // ST

            rcv2.discardUntil0();
            if (rcv2.length < 9)
            {
                // ooops
            }
            rcv2.dequeuebit();          // ST

            rcv3.discardUntil0();
            if (rcv3.length < 9)
            {
                // ooops
            }
            rcv3.dequeuebit();          // ST

            return new byte[] { rcv1.dequeuebyte_LSBfirst(), rcv2.dequeuebyte_LSBfirst(), rcv3.dequeuebyte_LSBfirst() };
        }

        public void TPI_SST(byte value, bool postincrement)
        {
            if (postincrement)
            {
                TPI_sendbyte(0x64);
                _TPIaddress++;
            }
            else
            {
                TPI_sendbyte(0x60);
            }
            TPI_sendbyte(value);
        }

        public void TPI_SSTCS(byte address, byte value)
        {
            byte command = (byte)(0xc0 + address);

            TPI_sendbyte(command);
            TPI_sendbyte(value);
        }

        public byte TPI_SLDCS(byte address)
        {
            // SLDCS(0xf) : read TPIIR
            byte command = (byte)(0x80 + address);

            TPI_sendbyte(command);
            //TPIskipidle();
            return TPI_receivebyte_skipIDLE();
        }

        public void TPI_SOUT(byte port, byte value)
        {
            byte command = (byte)(0x90 | (port & 0x0f) | ((port & 0x30) << 1));

            TPI_sendbyte(command);
            TPI_sendbyte(value);
        }

        public byte TPI_SIN(byte port)
        {
            byte command = (byte)(0x10 | (port & 0x0f) | ((port & 0x30) << 1));

            TPI_sendbyte(command);
            return TPI_receivebyte_skipIDLE();
        }

        public void TPI_sendbyte(byte b)
        {
            sendbits(TPI_constructsendmessage(b));
        }

        public Bitbuffer TPI_constructsendmessage(byte b)
        {
            Bitbuffer buf = new Bitbuffer();
            buf.enqueuebit(0);                  // ST
            buf.enqueuebyte_LSBfirst(b);        // D0..D7
            buf.enqueuebit(buf.parity(b));      // P
            buf.enqueuebit(1);                  // SP1
            buf.enqueuebit(2);                  // SP2
            return buf;
        }

        public void TPI_skipIDLEbits()
        {
            D_HIGH();
            //flush();
            Bitbuffer buf = receivebits(_TPIidlebits);
        }

        public byte TPI_receivebyte()
        {
            D_HIGH();
            //flush();
            Bitbuffer buf = receivebits(12);
            buf.discardUntil0();
            while (buf.length < 12)
            {
                Bitbuffer buf2 = receivebits(12 - buf.length);
                for (int i = 0; i < buf2.length; i++)
                {
                    buf.enqueuebit(buf2[i]);
                }
                buf.discardUntil0();
            }
            buf.dequeuebit();   // ST
            byte result = buf.dequeuebyte_LSBfirst();
            //buf.dequeuebit();   // P
            //buf.dequeuebit();   // SP1
            //buf.dequeuebit();   // SP2
            return result;
        }

        public byte TPI_receivebyte_skipIDLE()
        {
            D_HIGH();
            //flush();
            Bitbuffer buf = receivebits(_TPIidlebits + 12);
            buf.discardUntil0();
            int countbits = 0;
            while (buf.length < 12)
            {
                System.Diagnostics.Debug.Print("TPI_receivebyte_skipIDLE: Have to read more bits...");
                countbits += 12 - buf.length;
                Bitbuffer buf2 = receivebits(12 - buf.length);
                for (int i = 0; i < buf2.length; i++)
                {
                    buf.enqueuebit(buf2[i]);
                }
                buf.discardUntil0();
                if (countbits > 128 + 2 + 50)
                {
                    System.Diagnostics.Debug.Print("TPI_receivebyte_skipIDLE: Too many IDLE bits");
                    throw new System.Exception("Target: Too many IDLE bits. Target is not responding.");
                }
            }
            buf.dequeuebit();   // ST
            byte result = buf.dequeuebyte_LSBfirst();
            //buf.dequeuebit();   // P
            //buf.dequeuebit();   // SP1
            //buf.dequeuebit();   // SP2
            return result;
        }

        public enum NVMCommand
        {
            NO_OPERATION = 0x00,
            CHIP_ERASE = 0x10,
            SECTION_ERASE = 0x14,
            WORD_WRITE = 0x1d
        }

        public void NVM_enable()
        {
            TPI_sendbyte(0xe0);

            TPI_sendbyte(0xff);
            TPI_sendbyte(0x88);
            TPI_sendbyte(0xd8);
            TPI_sendbyte(0xcd);
            TPI_sendbyte(0x45);
            TPI_sendbyte(0xab);
            TPI_sendbyte(0x89);
            TPI_sendbyte(0x12);

            while ((TPI_SLDCS(0x00) & 0x02) == 0x00) { };
        }

        public void NVM_command(NVMCommand command)
        {
            TPI_SOUT(0x33, (byte)command);
        }

        public bool NVM_busy()
        {
            return (TPI_SIN(0x32) & 0x80) != 0x00;
        }

        public void NVM_waitbusy()
        {
            while (NVM_busy())
            {
                System.Diagnostics.Debug.Print("NVM_waitbusy: BUSY...");
            };
        }

        public void NVM_NO_OPERATION()
        {
            NVM_command(NVMCommand.NO_OPERATION);
            NVM_waitbusy();
        }

        public void NVM_CHIP_ERASE()
        {
            NVM_command(NVMCommand.CHIP_ERASE);
            // Start the erase operation by writing a dummy byte to the high byte of any word location inside the code section
            TPI_SSTPR(CPU_flashaddress() | 1);
            TPI_SST(0x00, false);
            NVM_waitbusy();
            NVM_NO_OPERATION();
        }

        public void NVM_SECTION_ERASE(long address)
        {
            NVM_command(NVMCommand.SECTION_ERASE);
            TPI_SSTPR(address | 1);     // make sure the address is odd (high byte address)
            TPI_SST(0x00, false);
            NVM_waitbusy();
            NVM_NO_OPERATION();
        }

        public void NVM_WORD_WRITE(long address, byte low, byte high)
        {
            NVM_command(NVMCommand.WORD_WRITE);
            TPI_SSTPR(address);
            TPI_SST(low, true);
            TPI_SST(high, true);
            NVM_waitbusy();
            NVM_NO_OPERATION();
        }

        public void NVM_WORD_WRITE(long address, byte[] data)
        {
            NVM_command(NVMCommand.WORD_WRITE);
            TPI_SSTPR(address);
            for (int i = 0; i < data.Length; i++)
            {
                TPI_SST(data[i], true);
                if ((address & 0x0001) == 0x0001)
                {
                    TPI_IDLE(64);   // send some IDLE bits
                    flush();
                    TPI_IDLE(64);   // send some IDLE bits
                    TPI_IDLE(64);   // send some IDLE bits
                    // wait for NVM after writing high bytes
                    NVM_waitbusy();
                }
                address++;
            }
            // last byte was low byte?
            // same question as: address points to high byte?
            if ((address & 0x0001) == 0x0001)
            {
                // write dummy high byte
                TPI_SST(0x00, true);
                NVM_waitbusy();
            }
            NVM_NO_OPERATION();
        }

        public void NVM_WORD_WRITE_fast(long address, byte[] data)
        {
            NVM_command(NVMCommand.WORD_WRITE);
            TPI_SSTPR(address);
            for (int i = 0; i < data.Length; i++)
            {
                TPI_SST(data[i], true);
                if ((address & 0x0001) == 0x0001)
                {
                    TPI_IDLE(64);   // send some IDLE bits
                    flush();
                    TPI_IDLE(64);   // send some IDLE bits
                    TPI_IDLE(64);   // send some IDLE bits
                    flush();
                    // wait for NVM after writing high bytes
                    //NVM_waitbusy();
                }
                address++;
            }
            // last byte was low byte?
            // same question as: address points to high byte?
            if ((address & 0x0001) == 0x0001)
            {
                // write dummy high byte
                TPI_SST(0x00, true);
                NVM_waitbusy();
            }
            NVM_NO_OPERATION();
        }

        public byte[] CPU_readbytes(long address, long num)
        {
            byte[] result = new byte[num];
            TPI_SSTPR(address);
            for (int i = 0; i < num; i++)
            {
                if (i + 2 < num)
                {
                    byte[] tmp = TPI_SLD_3bytes();
                    result[i] = tmp[0];
                    result[i + 1] = tmp[1];
                    result[i + 2] = tmp[2];
                    i+=2;
                }
                else if (i + 1 < num)
                {
                    byte[] tmp = TPI_SLD_2bytes();
                    result[i] = tmp[0];
                    result[i+1] = tmp[1];
                    i++;
                }
                else
                {
                    result[i] = TPI_SLD(true);
                }
            }

            return result;
        }

        public byte CPU_readbyte(long address)
        {
            return CPU_readbytes(address, 1)[0];
        }

        public long CPU_nvmlockbits_address()
        {
            return 0x3f00;
        }

        public byte[] CPU_nvmlockbits_read()
        {
            long oldaddress = TPI_SLDPR();
            byte[] data = CPU_readbytes(CPU_nvmlockbits_address(), 2);
            TPI_SSTPR(oldaddress);
            return data;
        }

        public long CPU_configurationbits_address()
        {
            return 0x3f40;
        }

        public byte[] CPU_configurationbits_read()
        {
            long oldaddress = TPI_SLDPR();
            byte[] data = CPU_readbytes(CPU_configurationbits_address(), 2);
            TPI_SSTPR(oldaddress);
            return data;
        }

        public long CPU_calibrationbits_address()
        {
            return 0x3f80;
        }

        public byte[] CPU_calibrationbits_read()
        {
            long oldaddress = TPI_SLDPR();
            byte[] data = CPU_readbytes(CPU_calibrationbits_address(), 2);
            TPI_SSTPR(oldaddress);
            return data;
        }

        public long CPU_deviceidbits_address()
        {
            return 0x3fc0;
        }

        public byte[] CPU_deviceidbits_read()
        {
            long oldaddress = TPI_SLDPR();
            byte[] data = CPU_readbytes(CPU_deviceidbits_address(), 3);
            TPI_SSTPR(oldaddress);
            return data;
        }

        public Processor CPU_identify()
        {
            if (_processor != Processor.UNKNOWN)
            {
                return _processor;
            }

            _processor = Processor.UNKNOWN;
            byte[] sig = CPU_deviceidbits_read();
            if (sig[0] == 0x1e)
            {
                if (sig[1] == 0x8f)     // 1e 8f
                {
                    if (sig[2] == 0x0a)
                    {
                        _processor = Processor.ATTiny4;
                    }
                    else if (sig[2] == 0x09)
                    {
                        _processor = Processor.ATTiny5;
                    }
                }
                else if (sig[1] == 0x90)    // 1e 90
                {
                    if (sig[2] == 0x08)
                    {
                        _processor = Processor.ATTiny9;
                    }
                    else if (sig[2] == 0x03)
                    {
                        _processor = Processor.ATTiny10;
                    }
                }
            }

            return _processor;
        }

        public long CPU_flashaddress()
        {
            return 0x4000;
        }

        public long CPU_flashsize()
        {
            switch (_processor)
            {
                case Processor.ATTiny4:
                case Processor.ATTiny5:
                    return 512;

                case Processor.ATTiny9:
                case Processor.ATTiny10:
                    return 1024;
            }

            return 0;
        }
    }
}
