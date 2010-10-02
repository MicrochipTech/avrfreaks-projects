using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace GenericHid
{
    public class IntelHEXfile
    {
        public IntelHEXfile(string filename)
        {
            StreamReader sr = new StreamReader(filename);
            bool eof = false;
            while (!eof)
            {
                HEXline line = new HEXline(sr.ReadLine());

                switch (line.recordtype)
                {
                    case HEXline.RecordType.DataRecord:
                        data.AddRange(line.data);
                        break;

                    case HEXline.RecordType.EndOfFile:
                        eof = true;
                        break;
                }

                if (sr.EndOfStream)
                {
                    eof = true;
                }
            }

            sr.Close();
            sr.Dispose();
        }

        public byte[] GetData()
        {
            return data.ToArray();
        }

        List<byte> data = new List<byte>();

        public class HEXline
        {
            public enum RecordType
            {
                DataRecord = 0,
                EndOfFile = 1,
                ExtendedSegmentAddress = 2,
                StartSegmentAddress = 3,
                ExtendedLinearAddress = 4,
                StartLinearAddress = 5
            }

            public HEXline(string s)
            {
                colon = s[0];
                s = s.Substring(1);

                length = long.Parse(s.Substring(0, 2), System.Globalization.NumberStyles.HexNumber);
                s = s.Substring(2);

                address = long.Parse(s.Substring(0, 4), System.Globalization.NumberStyles.HexNumber);
                s = s.Substring(4);

                recordtype = (RecordType)int.Parse(s.Substring(0, 2), System.Globalization.NumberStyles.HexNumber);
                s = s.Substring(2);

                data = new byte[length];
                for (int i = 0; i < length; i++)
                {
                    data[i] = byte.Parse(s.Substring(0, 2), System.Globalization.NumberStyles.HexNumber);
                    s = s.Substring(2);
                }

                checksum = byte.Parse(s.Substring(0, 2), System.Globalization.NumberStyles.HexNumber);
                s = s.Substring(2);
            }

            public char colon;
            public long length;
            public long address;
            public RecordType recordtype;
            public byte[] data;
            public byte checksum;
        }
    }
}
