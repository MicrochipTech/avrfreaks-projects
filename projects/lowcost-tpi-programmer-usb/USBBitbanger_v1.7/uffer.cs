using System;
using System.Collections.Generic;
using System.Text;

// just a buffer that can handle bits and bytes

namespace GenericHid
{
    public class Bitbuffer
    {
        public Bitbuffer()
        {
        }

        public Bitbuffer(byte[] data, int numbits)
        {
            setFromArray(data, numbits);
        }

        public void enqueueFromSubArray(byte[] data, int startofs, int numbits)
        {
            int cnt = 0;
            while (numbits > 8)
            {
                enqueuebyte_MSBfirst(data[startofs + cnt]);
                cnt++;
                numbits -= 8;
            }
            while (numbits > 0)
            {
                if (bitset(data[startofs + cnt], numbits - 1))
                {
                    enqueuebit(1);
                }
                else
                {
                    enqueuebit(0);
                }
                numbits--;
            }
        }

        public void setFromSubArray(byte[] data, int startofs, int numbits)
        {
            clear();

            enqueueFromSubArray(data, startofs, numbits);
        }

        public void setFromArray(byte[] data, int numbits)
        {
            setFromSubArray(data, 0, numbits);
        }

        bool bitset(byte b, int num)
        {
            byte mask = (byte)Math.Pow(2, num);
            return ((b & mask) != 0);
        }

        List<byte> bits = new List<byte>();

        public void clear()
        {
            bits.Clear();
        }

        public int length
        {
            get
            {
                return bits.Count;
            }
        }

        public byte this[int i]
        {
            get
            {
                return bits[i];
            }
        }

        public void concat(Bitbuffer b2)
        {
            while (b2.length > 0)
            {
                enqueuebit(b2.dequeuebit());
            }
        }

        public void enqueuebit(byte b)
        {
            bits.Add(b);
        }

        public byte dequeuebit()
        {
            byte b = bits[0];
            bits.RemoveAt(0);
            return b;
        }

        public void enqueuebyte_MSBfirst(byte b)
        {
            byte mask = 0x80;
            for (int i = 0; i < 8; i++)
            {
                if ((b & mask) != 0)
                {
                    bits.Add(1);
                }
                else
                {
                    bits.Add(0);
                }
                mask /= 2;
            }
        }

        public void enqueuebyte_LSBfirst(byte b)
        {
            byte mask = 0x01;
            for (int i = 0; i < 8; i++)
            {
                if ((b & mask) != 0)
                {
                    bits.Add(1);
                }
                else
                {
                    bits.Add(0);
                }
                mask *= 2;
            }
        }

        public byte dequeuebyte_LSBfirst()
        {
            byte res = 0;
            byte mask = 0x01;
            for (int i = 0; i < 8; i++)
            {
                if (dequeuebit() > 0)
                {
                    res |= mask;
                }
                mask *= 2;
            }
            return res;
        }

        public byte dequeuebyte_MSBfirst()
        {
            byte res = 0;
            byte mask = 0x80;
            for (int i = 0; i < 8; i++)
            {
                if (dequeuebit() > 0)
                {
                    res |= mask;
                }
                mask /= 2;
            }
            return res;
        }

        public byte parity(byte b)
        {
            byte par = 0;
            byte mask = 0x80;
            for (int i = 0; i < 8; i++)
            {
                if ((b & mask) != 0)
                {
                    par ^= 1;
                }
                mask /= 2;
            }
            return par;
        }

        public byte[] ToByteArray()
        {
            List<byte> res = new List<byte>();
            int bitnum = 0;

            byte cur = 0;
            byte mask = 0x80;
            while (bitnum < bits.Count)
            {
                if (bits[bitnum] > 0)
                {
                    cur |= mask;
                }
                mask /= 2;
                if (mask == 0)
                {
                    res.Add(cur);

                    mask = 0x80;
                    cur = 0;
                }

                bitnum++;
            }
            if (mask != 0x80)
            {
                res.Add(cur);
            }

            return res.ToArray();
        }

        public void discardUntil0()
        {
            int idx = 0;
            while (idx < bits.Count)
            {
                if (bits[idx] == 1)
                {
                    idx++;
                }
                else
                {
                    break;
                }
            }
            if (idx == -1)
            {
                return;
            }
            bits.RemoveRange(0, idx);
        }
    }
}
