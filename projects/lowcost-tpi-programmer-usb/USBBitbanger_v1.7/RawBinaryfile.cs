using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace GenericHid
{
    public class RawBinaryfile
    {
        public RawBinaryfile(byte[] dat)
        {
            data = new List<byte>(dat);
        }

        public RawBinaryfile(string filename)
        {
            FileStream fstr = new FileStream(filename, FileMode.Open);
            BinaryReader strr = new BinaryReader(fstr);
            data.AddRange(strr.ReadBytes((int)fstr.Length));
            strr.Close();
        }

        public byte[] GetData()
        {
            return data.ToArray();
        }

        public void SaveFile(string filename)
        {
            FileStream fstr = new FileStream(filename, FileMode.Create);
            BinaryWriter strw = new BinaryWriter(fstr);
            foreach (byte b in data)
            {
                strw.Write( b );
            }
            strw.Close();
        }

        List<byte> data = new List<byte>();
    }
}
