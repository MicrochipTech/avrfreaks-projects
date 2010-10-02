using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;
using Microsoft.Win32.SafeHandles;
using System.Runtime.InteropServices; 

namespace GenericHid
{
    public class USBBitbanger
    {
        enum SpecialCommands : byte
        {
            S_LOW=0,
            S_HIGH=1,
            C_LOW=2,
            C_HIGH=3,
            D_LOW=4,
            D_HIGH=5,
            Q_READ=6,
            GET_DELAY=0x7f,
            SET_DELAY=0x80
        };

        Hid.InFeatureReport m_InFeatureReport = new Hid.InFeatureReport();
        Hid.OutFeatureReport m_OutFeatureReport = new Hid.OutFeatureReport();
        Hid m_Hid = new Hid();
        DeviceManagement m_DeviceManagement = new DeviceManagement();
        SafeFileHandle m_hidHandle; 
        String m_DevicePathName;
        Boolean m_DeviceDetected = false;
        SafeFileHandle m_readHandle;
        SafeFileHandle m_writeHandle; 

        public USBBitbanger()
        {
        }

        ~USBBitbanger()
        {
            if (!(m_hidHandle == null))
            {
                if (!(m_hidHandle.IsInvalid))
                {
                    m_hidHandle.Close();
                }
            }
        }

        public bool do_connect(List<string> msgs)
        {
            if (m_DeviceDetected)
            {
                msgs.Add("USB device: was already open");
                return m_DeviceDetected;
            }

            msgs.Add("USB device: connecting...");

            Boolean deviceFound = false;
            Guid hidGuid = Guid.Empty;
            String[] devicePathName = new String[128];
            Int32 myVendorID = 0x16c0;
            Int32 myProductID = 0x05df;
            Int32 memberIndex = 0;
            Boolean success = false;

            Hid.HidD_GetHidGuid(ref hidGuid);

            deviceFound = m_DeviceManagement.FindDeviceFromGuid(hidGuid, ref devicePathName);
            if (deviceFound)
            {
                memberIndex = 0;

                do
                {
                    m_hidHandle = FileIO.CreateFile(devicePathName[memberIndex], 0, FileIO.FILE_SHARE_READ | FileIO.FILE_SHARE_WRITE, IntPtr.Zero, FileIO.OPEN_EXISTING, 0, 0);
                    if (!m_hidHandle.IsInvalid)
                    {
                        m_Hid.DeviceAttributes.Size = Marshal.SizeOf(m_Hid.DeviceAttributes);
                        success = Hid.HidD_GetAttributes(m_hidHandle, ref m_Hid.DeviceAttributes);
                        if (success)
                        {
                            if ((m_Hid.DeviceAttributes.VendorID == myVendorID) && (m_Hid.DeviceAttributes.ProductID == myProductID))
                            {
                                m_DeviceDetected = true;

                                //  Save the DevicePathName for OnDeviceChange().

                                m_DevicePathName = devicePathName[memberIndex];
                            }
                            else
                            {
                                //  It's not a match, so close the handle.

                                m_DeviceDetected = false;
                                m_hidHandle.Close();
                            }
                        }
                        else
                        {
                            m_DeviceDetected = false;
                            m_hidHandle.Close();
                        }
                    }
                    memberIndex = memberIndex + 1;
                }
                while (!((m_DeviceDetected || (memberIndex == devicePathName.Length))));

                if (m_DeviceDetected)
                {
                    msgs.Add("USB device: found the device on the bus");

                    m_Hid.Capabilities = m_Hid.GetDeviceCapabilities(m_hidHandle);

                    m_readHandle = FileIO.CreateFile(m_DevicePathName, FileIO.GENERIC_READ, FileIO.FILE_SHARE_READ | FileIO.FILE_SHARE_WRITE, IntPtr.Zero, FileIO.OPEN_EXISTING, FileIO.FILE_FLAG_OVERLAPPED, 0);
                    m_writeHandle = FileIO.CreateFile(m_DevicePathName, FileIO.GENERIC_WRITE, FileIO.FILE_SHARE_READ | FileIO.FILE_SHARE_WRITE, IntPtr.Zero, FileIO.OPEN_EXISTING, 0, 0);
                    m_Hid.FlushQueue(m_readHandle);

                    Hid.InFeatureReport myInFeatureReport = new Hid.InFeatureReport();
                    Hid.OutFeatureReport myOutFeatureReport = new Hid.OutFeatureReport();
                    newOutBuffer();
                    newInBuffer();
                }
                else
                {
                    msgs.Add("USB device: cannot find the device on the bus");
                }
            }

            return m_DeviceDetected;
        }

        public virtual bool connect(List<string> msgs)
        {
            return do_connect(msgs);
        }

        void newOutBuffer()
        {
            m_outFeatureReportBuffer = new Byte[m_Hid.Capabilities.FeatureReportByteLength];
        }

        void newInBuffer()
        {
            m_inFeatureReportBuffer = new Byte[m_Hid.Capabilities.FeatureReportByteLength];
        }

        public void S_LOW()
        {
            sendspecial(SpecialCommands.S_LOW);
        }
        public void S_HIGH()
        {
            sendspecial(SpecialCommands.S_HIGH);
        }
        public void C_LOW()
        {
            sendspecial(SpecialCommands.C_LOW);
        }
        public void C_HIGH()
        {
            sendspecial(SpecialCommands.C_HIGH);
        }
        public void D_LOW()
        {
            sendspecial(SpecialCommands.D_LOW);
        }
        public void D_HIGH()
        {
            sendspecial(SpecialCommands.D_HIGH);
        }
        public byte Q_READ()
        {
            return receivespecial(SpecialCommands.Q_READ);
        }
        public void SET_DELAY(byte us)
        {
            sendspecial(SpecialCommands.SET_DELAY + us);
        }
        public byte GET_DELAY()
        {
            return receivespecial(SpecialCommands.GET_DELAY);
        }

        void sendspecial(SpecialCommands spc)
        {
            send(new byte[] {0x80, (byte)spc });
        }

        byte receivespecial(SpecialCommands spc)
        {
            return receive(new byte[]{0x80, (byte)spc})[1];
        }

        public byte[] receivedata(int numBytes)
        {
            return receivedata(new byte[numBytes]);
        }

        byte[] receivedata(byte[] data)
        {
            return receivedata(data, data.Length * 8);
        }

        byte[] receivedata(byte[] data, int bits)
        {
            int datalen = Math.Min(data.Length, (int)Math.Ceiling(bits / 8.0));
            byte[] result = new byte[datalen];

            int ptr = 0;
            while (ptr < datalen)
            {
                int maxlen = m_outFeatureReportBuffer.Length - m_count - 1;
                maxlen = Math.Min(maxlen, datalen - ptr);
                if (maxlen > 48)
                {
                    maxlen = 48;
                    receive48bytes(data, result, ptr);
                    bits -= 48 * 8;
                }
                else if (maxlen > 32)
                {
                    maxlen = 32;
                    receive32bytes(data, result, ptr);
                    bits -= 32 * 8;
                }
                else if (maxlen > 16)
                {
                    maxlen = 16;
                    receive16bytes(data, result, ptr);
                    bits -= 16 * 8;
                }
                else
                {
                    maxlen = Math.Min(maxlen, 16);
                    byte[] dat = new byte[maxlen + 1];
                    byte thisbits = (byte)Math.Min(bits, maxlen * 8);

                    int oldidx = m_count;
                    dat[0] = (byte)(0x00 + thisbits - 1);
                    Array.Copy(data, ptr, dat, 1, maxlen);
                    send(dat);
                    flush();

                    read();
                    Array.Copy(m_inFeatureReportBuffer, oldidx + 1, result, ptr, maxlen);

                    bits -= thisbits;
                }

                ptr += maxlen;
            }

            return result;
        }

        void receive16bytes(byte[] data, byte[] result, int ptr)
        {
            int maxlen = 16;
            byte[] dat = new byte[maxlen + 1];

            int oldidx = m_count;
            dat[0] = (byte)(0x00 + (maxlen * 8) - 1);
            Array.Copy(data, ptr, dat, 1, maxlen);
            send(dat);
            flush();

            read();
            Array.Copy(m_inFeatureReportBuffer, oldidx + 1, result, ptr, maxlen);
        }

        void receive32bytes(byte[] data, byte[] result, int ptr)
        {
            int maxlen1 = 16;
            int maxlen2 = 16;
            byte[] dat = new byte[maxlen1 + 1 + maxlen2 + 1];

            int oldidx1 = m_count;
            int oldidx2 = m_count+16+1;
            dat[0] = (byte)(0x00 + (maxlen1 * 8) - 1);
            Array.Copy(data, ptr, dat, 1, maxlen1);
            dat[maxlen1+1] = (byte)(0x00 + (maxlen2 * 8) - 1);
            Array.Copy(data, ptr+maxlen1, dat, 1+maxlen1, maxlen2);
            send(dat);
            flush();

            read();
            Array.Copy(m_inFeatureReportBuffer, oldidx1 + 1, result, ptr, maxlen1);
            Array.Copy(m_inFeatureReportBuffer, oldidx2 + 1, result, ptr+maxlen1, maxlen2);
        }

        void receive48bytes(byte[] data, byte[] result, int ptr)
        {
            int maxlen1 = 16;
            int maxlen2 = 16;
            int maxlen3 = 16;
            byte[] dat = new byte[1 + maxlen1 + 1 + maxlen2 + 1 + maxlen3];

            int oldidx1 = m_count + 1;
            int oldidx2 = m_count + 1 + maxlen1 + 1;
            int oldidx3 = m_count + 1 + maxlen1 + 1 + maxlen2 + 1;
            dat[0] = (byte)(0x00 + (maxlen1 * 8) - 1);
            Array.Copy(data, ptr, dat, 1, maxlen1);
            dat[1 + maxlen1] = (byte)(0x00 + (maxlen2 * 8) - 1);
            Array.Copy(data, ptr+maxlen1, dat, 1+maxlen1+1, maxlen2);
            dat[1 + maxlen1 + 1 + maxlen2] = (byte)(0x00 + (maxlen3 * 8) - 1);
            Array.Copy(data, ptr+maxlen1+maxlen2, dat, 1+maxlen1+1+maxlen2+1, maxlen3);
            send(dat);
            flush();

            read();
            Array.Copy(m_inFeatureReportBuffer, oldidx1, result, ptr, maxlen1);
            Array.Copy(m_inFeatureReportBuffer, oldidx2, result, ptr + maxlen1, maxlen2);
            Array.Copy(m_inFeatureReportBuffer, oldidx3, result, ptr + maxlen1 + maxlen2, maxlen2);
        }

        byte[] receive(byte[] data)
        {
            byte[] result = new byte[data.Length];
            flush();

            int ptr = 0;
            while (ptr < data.Length)
            {
                int maxlen = m_outFeatureReportBuffer.Length - m_count;
                maxlen = Math.Min(maxlen, data.Length - ptr);
                byte[] dat = new byte[maxlen];

                int oldidx = m_count;
                Array.Copy(data, ptr, dat, 0, maxlen);
                send(dat);
                flush();

                read();
                Array.Copy(m_inFeatureReportBuffer, oldidx, result, ptr, maxlen);

                ptr += maxlen;
            }

            return result;
        }

        Int32 m_count = 0;
        Byte[] m_inFeatureReportBuffer = null;
        Byte[] m_outFeatureReportBuffer = null;

        void send(byte[] data)
        {
            if (data.Length <= 2)
            {
                if (m_outFeatureReportBuffer.Length - m_count < data.Length)
                {
                    flush();
                }
            }
            if (m_count == 0)
            {
                //  Store the report ID in the buffer:
                m_outFeatureReportBuffer[m_count++] = 0;
            }
            foreach (byte b in data)
            {
                m_outFeatureReportBuffer[m_count++] = b;
            }
            if (m_count >= m_outFeatureReportBuffer.Length-1)
            {
                flush();
            }
        }

        public void sendbits(Bitbuffer buf)
        {
#if true
            senddata(buf.ToByteArray(), buf.length);
#else
            for (int i = 0; i < buf.length; i++)
            {
                C_LOW();
                if (buf[i] > 0)
                {
                    D_HIGH();
                }
                else
                {
                    D_LOW();
                }
                flush();
                C_HIGH();
                flush();
            }
#endif
        }

        public void sendNreceive(Bitbuffer snd1, Bitbuffer rcv1, Bitbuffer snd2, Bitbuffer rcv2)
        {
            byte[] snd1msg = composesendmsg(snd1);
            byte[] snd2msg = composesendmsg(snd2);
            byte[] rcv1msg = composereceivemsg(rcv1);
            byte[] rcv2msg = composereceivemsg(rcv2);
            int allbytes = snd1msg.Length + rcv1msg.Length + snd2msg.Length + rcv2msg.Length;
            if (m_outFeatureReportBuffer.Length - m_count < allbytes)
            {
                flush();
            }

            send(snd1msg);
            int ofsrcv1 = m_count;
            send(rcv1msg);

            send(snd2msg);
            int ofsrcv2 = m_count;
            send(rcv2msg);
            flush();

            read();
            rcv1.setFromSubArray(m_inFeatureReportBuffer, ofsrcv1 + 1, rcv1.length);
            rcv2.setFromSubArray(m_inFeatureReportBuffer, ofsrcv2 + 1, rcv2.length);
        }

        public void sendNreceive(Bitbuffer snd1, Bitbuffer rcv1, Bitbuffer snd2, Bitbuffer rcv2, Bitbuffer snd3, Bitbuffer rcv3)
        {
            byte[] snd1msg = composesendmsg(snd1);
            byte[] snd2msg = composesendmsg(snd2);
            byte[] snd3msg = composesendmsg(snd2);
            byte[] rcv1msg = composereceivemsg(rcv1);
            byte[] rcv2msg = composereceivemsg(rcv2);
            byte[] rcv3msg = composereceivemsg(rcv2);
            int allbytes = snd1msg.Length + rcv1msg.Length + snd2msg.Length + rcv2msg.Length + snd3msg.Length + rcv3msg.Length;
            if (m_outFeatureReportBuffer.Length - m_count < allbytes)
            {
                flush();
            }

            send(snd1msg);
            int ofsrcv1 = m_count;
            send(rcv1msg);

            send(snd2msg);
            int ofsrcv2 = m_count;
            send(rcv2msg);

            send(snd3msg);
            int ofsrcv3 = m_count;
            send(rcv3msg);

            flush();
            read();

            rcv1.setFromSubArray(m_inFeatureReportBuffer, ofsrcv1 + 1, rcv1.length);
            rcv2.setFromSubArray(m_inFeatureReportBuffer, ofsrcv2 + 1, rcv2.length);
            rcv3.setFromSubArray(m_inFeatureReportBuffer, ofsrcv3 + 1, rcv3.length);
        }

        public byte[] composesendmsg(Bitbuffer buf)
        {
            List<byte> bytes = new List<byte>();
            int bits = buf.length;
            bytes.Add((byte)(0x80 + bits - 1));
            bytes.AddRange(buf.ToByteArray());
            return bytes.ToArray();
        }

        public byte[] composereceivemsg(Bitbuffer buf)
        {
            List<byte> bytes = new List<byte>();
            int bits = buf.length;
            bytes.Add((byte)(0x00 + bits - 1));
            bytes.AddRange(buf.ToByteArray());
            return bytes.ToArray();
        }

        public Bitbuffer receivebits(int numbits)
        {
#if true
            int bytes = (int)Math.Ceiling(numbits/8.0);
            byte[] rawdat = receivedata(new byte[bytes], numbits);
            return new Bitbuffer(rawdat,numbits);
#else
            Bitbuffer data = new Bitbuffer();
            for(int i=0; i<len; i++)
            {
                C_LOW();
                flush();
                C_HIGH();
                flush();
                data.enqueuebit(Q_READ());
            }
            return data;
#endif
        }

        public void senddata(byte d1)
        {
            senddata(new byte[] { d1 });
        }

        public void senddata(byte d1, byte d2)
        {
            senddata(new byte[] { d1, d2 });
        }

        public void senddata(byte d1, byte d2, byte d3)
        {
            senddata(new byte[] { d1, d2,d3 });
        }

        public void senddata(byte d1, byte d2, byte d3, byte d4)
        {
            senddata(new byte[] { d1, d2, d3, d4 });
        }

        public void senddata(byte[] data)
        {
            senddata(data, data.Length * 8);
            /*
            int ptr = 0;
            while (ptr < data.Length)
            {
                int maxlen = m_outFeatureReportBuffer.Length - m_count - 1;
                maxlen = Math.Min(maxlen, data.Length - ptr);
                byte[] dat = new byte[maxlen + 1];
                dat[0] = (byte)(0x80 + (maxlen * 8) - 1);
                Array.Copy(data, ptr, dat, 1, maxlen);
                send(dat);
                ptr += maxlen;
            }
            */
        }

        public void senddata(byte[] data, int bits)
        {
            int datalen = Math.Min(data.Length, (int)Math.Ceiling(bits / 8.0));
            int ptr = 0;
            while (ptr < datalen)
            {
                int maxlen = m_outFeatureReportBuffer.Length - m_count - 1;
                maxlen = Math.Min(maxlen, datalen - ptr);
                maxlen = Math.Min(maxlen, (0x80 / 8));
                int maxbits = Math.Min(maxlen * 8, bits);
                byte[] dat = new byte[maxlen + 1];
                if ( maxbits == 1 )
                {
                    throw new System.Exception("Cannot send a single bit");
                }
                dat[0] = (byte)(0x80 + maxbits - 1);
                Array.Copy(data, ptr, dat, 1, maxlen);
                send(dat);
                ptr += maxlen;
                bits -= maxbits;
            }
        }

        public void flush()
        {
            if (m_count <= 1)
            {
                return;
            }
#if true
            bool success = m_OutFeatureReport.Write(m_outFeatureReportBuffer, m_hidHandle);
#else
            byte[] temp = new Byte[m_Hid.Capabilities.FeatureReportByteLength];
            Array.Copy(m_outFeatureReportBuffer, 0, temp, 0, m_count);
            bool success = m_OutFeatureReport.Write(temp, m_hidHandle);
            /*
            string res = "";
            for (int count = 0; count <= m_outFeatureReportBuffer.Length - 1; count++)
            {
                res += String.Format("{0:X2} ", m_outFeatureReportBuffer[count]);
            }
            */
#endif
            if (success)
            {
                //Debug.WriteLine("Send(OK) " + m_count + "/" + m_outFeatureReportBuffer.Length + " bytes");
            }
            else
            {
                Debug.WriteLine("Send(fl) " + m_count + "/" + m_outFeatureReportBuffer.Length + " bytes");
            }

            m_count = 0;
            newOutBuffer();
            //  Store the report ID in the buffer:
            m_outFeatureReportBuffer[m_count++] = 0;
        }

        void read()
        {
            newInBuffer();
            bool success = true;
#if true
            m_InFeatureReport.Read(m_hidHandle, m_readHandle, m_writeHandle, ref m_DeviceDetected, ref m_inFeatureReportBuffer, ref success);
#else
            byte[] temp = new Byte[m_Hid.Capabilities.FeatureReportByteLength];
            m_InFeatureReport.Read(m_hidHandle, m_readHandle, m_writeHandle, ref m_DeviceDetected, ref temp, ref success);
            /*
            string res = "";
            for (int count = 0; count <= m_inFeatureReportBuffer.Length - 1; count++)
            {
                res += String.Format("{0:X2} ", m_inFeatureReportBuffer[count]);
            }
            */
            Array.Copy(temp, 0, m_inFeatureReportBuffer, 0, m_Hid.Capabilities.FeatureReportByteLength - 1);
#endif
            if (success)
            {
                //Debug.WriteLine("Read(OK)");// + res);
            }
            else
            {
                Debug.WriteLine("Read(fl)");//: " + res);
            }
        }

        List<int> m_asyncReceiveOffsets = new List<int>();
        List<int> m_asyncReceiveLengths = new List<int>();
        List<Bitbuffer> m_asyncReceiveBuffs = new List<Bitbuffer>();
        Bitbuffer m_asyncCurrentBuff = new Bitbuffer();
        List<int> m_asyncRequestLength = new List<int>();
        public void async_clear()
        {
            m_asyncReceiveOffsets = new List<int>();
            m_asyncReceiveLengths = new List<int>();
            m_asyncReceiveBuffs = new List<Bitbuffer>();
            m_asyncCurrentBuff = new Bitbuffer();
            m_asyncRequestLength = new List<int>();
        }

        public Bitbuffer async_getBuffer()
        {
            if (m_asyncReceiveBuffs.Count > 0)
            {
                Bitbuffer b = m_asyncReceiveBuffs[0];
                m_asyncReceiveBuffs.RemoveAt(0);
                return b;
            }
            async_flush();
            if (m_asyncReceiveBuffs.Count > 0)
            {
                Bitbuffer b = m_asyncReceiveBuffs[0];
                m_asyncReceiveBuffs.Remove(b);
                return b;
            }
            throw new System.Exception("There is not enough data received to fill a buffer");
        }

        public void async_S_LOW()
        {
            async_sendspecial(SpecialCommands.S_LOW);
        }
        public void async_S_HIGH()
        {
            async_sendspecial(SpecialCommands.S_HIGH);
        }
        public void async_C_LOW()
        {
            async_sendspecial(SpecialCommands.C_LOW);
        }
        public void async_C_HIGH()
        {
            async_sendspecial(SpecialCommands.C_HIGH);
        }
        public void async_D_LOW()
        {
            async_sendspecial(SpecialCommands.D_LOW);
        }
        public void async_D_HIGH()
        {
            async_sendspecial(SpecialCommands.D_HIGH);
        }

        void async_sendspecial(SpecialCommands spc)
        {
            async_sendraw(new byte[] { 0x80, (byte)spc });
        }

        public void async_send(byte[] data)
        {
            int datalen = data.Length;
            int dataofs = 0;

            while (datalen > 0)
            {
                int maxsend = Math.Min(datalen, async_maxsendbytes());
                int maxsendbits = maxsend * 8;

                byte[] dat = new byte[maxsend + 1];
                dat[0] = (byte)(0x80 + maxsendbits - 1);
                Array.Copy(data, dataofs, dat, 1, maxsend);
                async_send(dat);

                dataofs += maxsend;
                datalen -= maxsend;
            }
        }

        public void async_receive(int numBytes)
        {
            m_asyncRequestLength.Add(numBytes*8);

            while (numBytes > 0)
            {
                int maxrecv = Math.Min(numBytes, async_maxsendbytes());
                int maxrecvbits = maxrecv * 8;

                byte[] dat = new byte[maxrecv + 1];
                dat[0] = (byte)(0x00 + maxrecvbits - 1);

                m_asyncReceiveOffsets.Add(m_count);
                m_asyncReceiveLengths.Add(maxrecvbits);
                async_sendraw(dat);

                numBytes -= maxrecv;
            }
        }

        public void async_flush()
        {
            if (m_count <= 1)
            {
                return;
            }

            bool success = m_OutFeatureReport.Write(m_outFeatureReportBuffer, m_hidHandle);

            if (success)
            {
                //Debug.WriteLine("Send(OK) " + m_count + "/" + m_outFeatureReportBuffer.Length + " bytes");
            }
            else
            {
                Debug.WriteLine("Send(fl) " + m_count + "/" + m_outFeatureReportBuffer.Length + " bytes");
            }

            if ( m_asyncReceiveLengths.Count > 0 )
            {
                read();
            }
            while (m_asyncReceiveLengths.Count > 0)
            {
                if (m_asyncCurrentBuff.length == m_asyncRequestLength[0])
                {
                    m_asyncRequestLength.RemoveAt(0);
                    m_asyncReceiveBuffs.Add(m_asyncCurrentBuff);
                    m_asyncCurrentBuff = new Bitbuffer();
                }

                int dataofs = m_asyncReceiveOffsets[0];
                m_asyncReceiveOffsets.RemoveAt(0);
                int datasiz = m_asyncReceiveLengths[0];
                m_asyncReceiveLengths.RemoveAt(0);
                m_asyncCurrentBuff.enqueueFromSubArray(m_inFeatureReportBuffer, dataofs + 1, datasiz);
                if (m_asyncCurrentBuff.length == m_asyncRequestLength[0])
                {
                    m_asyncRequestLength.RemoveAt(0);
                    m_asyncReceiveBuffs.Add(m_asyncCurrentBuff);
                    m_asyncCurrentBuff = new Bitbuffer();
                }
            }

            m_count = 0;
            newOutBuffer();
            //  Store the report ID in the buffer:
            m_outFeatureReportBuffer[m_count++] = 0;
        }

        void async_sendraw(byte[] data)
        {
            int datalen = data.Length;

            int maxd = async_maxsendrawbytes();
            if ( data.Length > maxd )
            {
                async_flush();
            }

            if ( data.Length > maxd )
            {
                throw new System.Exception("I cannot send that much data");
            }

            foreach( byte b in data )
            {
                m_outFeatureReportBuffer[ m_count++] = b;
            }
        }

        int async_maxsendrawbytes()
        {
            int max = 0x80 / 8;
            max = Math.Min(max, m_outFeatureReportBuffer.Length - m_count);
            if (max <= 0)
            {
                async_flush();
                return async_maxsendrawbytes();
            }
            return max;
        }

        int async_maxsendbytes()
        {
            int max = 0x80 / 8;
            max = Math.Min(max, m_outFeatureReportBuffer.Length - m_count-1);
            if (max <= 0)
            {
                async_flush();
                return async_maxsendbytes();
            }
            return max;
        }
    }
}
