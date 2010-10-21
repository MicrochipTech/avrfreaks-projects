using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;
using System.Windows.Forms;
using System.Net.Sockets;

namespace myCompany
{
    [Guid("0D53A3E8-E51A-49C7-944E-E72A2064F939")]
    [ClassInterface(ClassInterfaceType.None)]
    [ProgId("myCompany.myTarget")]
    unsafe public partial class myTarget : AVRStudioSDK.AvrStudioPlugin, AVRStudioSDK.IAvrTarget
    {
        string mPartname = "";
        AVRFRAMEWORKLib.FrameWorkClass mFrameWork = null;
        TcpClient mTCPClient = null;
        AVRStudioSDK.TARGET.STATE mState = AVRStudioSDK.TARGET.STATE.TARGET_STATE_STOP;
        uint mIOoffset = 0x20;
        uint mREGoffset = 0x00;
        List<uint> mAddr_USER = new List<uint>();
        List<uint> mAddr_HLLSTAT = new List<uint>();
        List<uint> mAddr_FUNCRET = new List<uint>();
        List<uint> mAddr_FUNCCALL = new List<uint>();
        List<uint> mAddr_TEMP = new List<uint>();
        List<uint> mAddr_EXIT = new List<uint>();

        public myTarget()
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget constructor");

            // create a framework
            bool fullframework = true;
            mFrameWork = new AVRFRAMEWORKLib.FrameWorkClass();
            mFrameWork.create(this.GetType().FullName, fullframework);
        }

        ~myTarget()
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget destructor");

            Marshal.FinalReleaseComObject(mFrameWork);
            mFrameWork = null;
        }

        void sendGDBmessage(string msg)
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget sendGDBmessage " + msg);
            if (!mTCPClient.Connected)
            {
                // not connected
                return;
            }

            if ( mTCPClient.Available > 0 )
            {
                mTCPClient.GetStream().ReadTimeout = 1;
                byte[] data = new byte[2048];
                int bytes = mTCPClient.GetStream().Read(data, 0, data.Length);
                msg = System.Text.Encoding.ASCII.GetString(data, 0, bytes);
                System.Diagnostics.Debug.WriteLine("myCompany.myTarget sendGDBmessage trashing " + msg);
            }

            byte[] data2 = System.Text.Encoding.ASCII.GetBytes(msg);
            mTCPClient.GetStream().Write(data2, 0, data2.Length);
        }

        void sendGDBmessageBreak()
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget sendGDBmessage <break>");
            if (!mTCPClient.Connected)
            {
                // not connected
                return;
            }

            byte[] data = new byte[] { 3 };
            mTCPClient.GetStream().Write(data, 0, data.Length);

            recvGDBmessage();
        }

        string recvGDBmessage()
        {
            if (!mTCPClient.Connected)
            {
                // not connected
                return "";
            }

            bool complete = false;
            string msg = "";
            while (!complete)
            {
                mTCPClient.GetStream().ReadTimeout = 20;
                try
                {
                    byte[] data = new byte[2048];
                    int bytes = mTCPClient.GetStream().Read(data, 0, data.Length);
                    msg = System.Text.Encoding.ASCII.GetString(data, 0, bytes);
                }
                catch
                {
                    // timeout
                    Connect("");
                    break;
                }
                if (msg.Contains("#"))
                {
                    if (msg.Length >= msg.IndexOf('#') + 3)
                        complete = true;
                }
            }

            System.Diagnostics.Debug.WriteLine("myCompany.myTarget recvGDBmessage " + msg);
            return msg;
        }

        string asHex(uint x)
        {
            return string.Format("{0:X08}", x);
        }

        string asLEHex(uint x)
        {
            List<byte> xarr = new List<byte>(BitConverter.GetBytes(x));
            xarr.Reverse();
            x = BitConverter.ToUInt32(xarr.ToArray(), 0);
            return string.Format("{0:X08}", x);
        }

        string asHex(int x)
        {
            return string.Format("{0:X08}", x);
        }

        string asHex(byte x)
        {
            return string.Format("{0:X02}", (byte)x);
        }

        uint UInt32fromLEHex(string hex)
        {
            uint x = uint.Parse(hex, System.Globalization.NumberStyles.HexNumber);
            List<byte> xarr = new List<byte>(BitConverter.GetBytes(x));
            xarr.Reverse();
            x = BitConverter.ToUInt32(xarr.ToArray(), 0);
            return x;
        }

        #region IAvrTarget
        public void ClearAllEvents()
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget ClearAllEvents");

            mAddr_USER = new List<uint>();
            mAddr_HLLSTAT = new List<uint>();
            mAddr_FUNCRET = new List<uint>();
            mAddr_FUNCCALL = new List<uint>();
            mAddr_TEMP = new List<uint>();
            mAddr_EXIT = new List<uint>();
        }

        unsafe public void ClearEvent(uint ulCount, uint* pulAddress, uint ulMask)
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget ClearEvent");

            List<uint> lst = null;
            switch (ulMask)
            {
                case 1:
                    lst = mAddr_USER;
                    break;
                case 2:
                    lst = mAddr_HLLSTAT;
                    break;
                case 4:
                    lst = mAddr_FUNCRET;
                    break;
                case 8:
                    lst = mAddr_FUNCCALL;
                    break;
                case 64:
                    lst = mAddr_TEMP;
                    break;
                case 128:
                    lst = mAddr_EXIT;
                    break;
            }
            if (lst == null)
                return;

            for (uint i = 0; i < ulCount; i++)
            {
                if (lst.Contains(pulAddress[i]))
                {
                    lst.Remove(pulAddress[i]);

                    if (ulMask == 1)
                    {
                        sendGDBmessage("+$z0," + asHex(pulAddress[i] / 2) + ",2#");
                        string msg = recvGDBmessage();
                    }
                }
            }
        }

        public void Connect(string bstrComPort)
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget Connect " + bstrComPort);

            string port = "1234";
            string addr = "192.168.1.129";
            mFrameWork.GetProjectFileValue("DEBUG_TARGET", "GDBADDR", out addr);
            mFrameWork.GetProjectFileValue("DEBUG_TARGET", "GDBPORT", out port);

            if (bstrComPort != "")
            {
                AVRStudioPlugin.FormConnect fc = new AVRStudioPlugin.FormConnect();
                fc.textBox_Address.Text = addr;
                fc.textBox_Port.Text = port;
                fc.ShowDialog();
                addr = fc.textBox_Address.Text;
                port = fc.textBox_Port.Text;
            }

            int iport = 1234;
            System.Net.IPAddress iaddr = new System.Net.IPAddress(new byte[] { 192, 168, 1, 129 });
            try
            {
                iport = int.Parse(port);
                iaddr = System.Net.Dns.GetHostAddresses(addr)[0];
            }
            catch { }

            mFrameWork.SetProjectFileValue("DEBUG_TARGET", "GDBADDR", addr);
            port = iport.ToString();
            mFrameWork.SetProjectFileValue("DEBUG_TARGET", "GDBPORT", port);

            try
            {
                mTCPClient.Close();
                mTCPClient = null;
            }
            catch { }

            try
            {
                mTCPClient = new TcpClient();
                mTCPClient.Connect(iaddr, iport);

                string msg = "";
                mFrameWork.evtBroadcast((int)AVRStudioSDK.EVENT.AvrEventEnum.AVREVENT_TARGETINITOK, ref msg);
            }
            catch
            {
                mFrameWork.errMsgWrite(2, "myCompany.myTarget", "Connect()", "Cannot connect to gdb-server " + iaddr.ToString() + ":" + iport );

                string msg = "";
                mFrameWork.evtBroadcast((int)AVRStudioSDK.EVENT.AvrEventEnum.AVREVENT_TARGETINITFAILED, ref msg);

                Disconnect();
            }
        }

        public void Disconnect()
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget Disconnect " );

            mState = AVRStudioSDK.TARGET.STATE.TARGET_STATE_STOP;

            ClearAllEvents();

            string msg = "";
            mFrameWork.evtBroadcast((int)AVRStudioSDK.EVENT.AvrEventEnum.AVREVENT_UPDATEVIEWS, ref msg);
            msg = "Stop";
            mFrameWork.evtBroadcast((int)AVRStudioSDK.EVENT.AvrEventEnum.AVREVENT_TARGETSTOP, ref msg);
            msg = "";
            mFrameWork.evtBroadcast((int)AVRStudioSDK.EVENT.AvrEventEnum.AVREVENT_DEBUGSTOPPING, ref msg);
        }

        public void GetEvent(uint ulAddress, out uint pulMask)
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget GetEvent");

            pulMask = 0;
        }

        public void GetGlobalEventMask(out uint pulGlobalMask)
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget GetGlobalEventMask");

            pulGlobalMask = 0;
        }

        public void GetInformation(out string pstrTargetConfigurationXML)
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget GetInformation");

            // provide XML with infos about this IAvrTarget
            FileInfo fi = new FileInfo(this.GetType().Module.FullyQualifiedName);
            FileStream fs = new FileStream(fi.Directory.FullName + "\\myTargetSetup.xml", FileMode.Open);
            StreamReader sr = new StreamReader(fs);
            pstrTargetConfigurationXML = sr.ReadToEnd();
            pstrTargetConfigurationXML = pstrTargetConfigurationXML.Replace("<!--SELECTEDPART-->", mPartname);
            sr.Close();
            fs.Close();
        }

        public void GetState(out byte ucState)
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget GetState");

            ucState = (byte)mState;
        }

        public void GetVersion(out string pbstrVersion)
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget GetVersion");

            pbstrVersion = "1.0.0";
        }

        public void GotoAddress(uint ulAddress)
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget GotoAddress");

            dummyrun();
        }

        public void IsPresent(out byte pucPresent)
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget IsPresent");

            pucPresent = 1;
        }

        public void ReadCycleCounter(out uint pulCycleCounter)
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget ReadCycleCounter");

            pulCycleCounter = 0;
        }

        public void ReadMemory(uint ulMemoryType, uint ulAddress, uint ulSize, out string pMemory)
        {
            AVRStudioSDK.TARGET.MEM memtype = (AVRStudioSDK.TARGET.MEM)ulMemoryType;
            pMemory = "";
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget ReadMemory " + memtype.ToString() + " " + asHex(ulAddress) + ":" + asHex(ulSize));

            bool readit = false;
            switch (memtype)
            {
                case AVRStudioSDK.TARGET.MEM.TARGET_MEM_PROGMEMORY:
                    readit = true;
                    break;

                case AVRStudioSDK.TARGET.MEM.TARGET_MEM_IO:
                    readit = true;
                    ulAddress += 0x800000 + mIOoffset;
                    break;

                case AVRStudioSDK.TARGET.MEM.TARGET_MEM_SRAM:
                    readit = true;
                    ulAddress += 0x800000;
                    break;

                case AVRStudioSDK.TARGET.MEM.TARGET_MEM_EEPROM:
                    readit = true;
                    ulAddress += 0x810000;
                    break;

                case AVRStudioSDK.TARGET.MEM.TARGET_MEM_REGISTERS:
                    sendGDBmessage("+$g#");
                    string msg = recvGDBmessage();
                    msg = msg.Substring( msg.IndexOf("$")+1);
                    List<byte> res = new List<byte>();
                    for (uint i = ulAddress; i < (ulAddress+ulSize); i++)
                    {
                        res.Add( byte.Parse(msg.Substring((int)(i*2), 2),System.Globalization.NumberStyles.HexNumber) ); 
                    }
                    for (uint i = 0; i < ulSize / 2; i++)
                    {
                        pMemory += BitConverter.ToChar(res.ToArray(), (int)i*2);
                    }
                    return;
                    break;
            }

            if (readit)
            {
                sendGDBmessage("+$m" + asHex(ulAddress) + "," + asHex(ulSize) + "#");
                string msg = recvGDBmessage();
                msg = msg.Substring(msg.IndexOf("$") + 1);
                List<byte> res = new List<byte>();
                for (int i = 0; i < ulSize; i++)
                {
                    res.Add(byte.Parse(msg.Substring(i * 2, 2), System.Globalization.NumberStyles.HexNumber));
                }
                for (int i = 0; i < ulSize / 2; i++)
                {
                    pMemory += BitConverter.ToChar(res.ToArray(), i * 2);
                }
            }
        }

        public void ReadProgramCounter(out uint pulPC)
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget ReadProgramCounter");
            pulPC = 0;

            sendGDBmessage("+$p22#");
            string addr = recvGDBmessage();
            addr = addr.Substring(addr.IndexOf('$') + 1).Substring(0,8);
            pulPC = UInt32fromLEHex(addr);
        }

        public void Reset(byte bHLLMode)
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget Reset");

            sendGDBmessageBreak();

            sendGDBmessage("+$r#");
            string msg = recvGDBmessage();

            dummyrun();
        }

        public void Run()
        {
            sendGDBmessage("+$c#");
            //recvGDBmessage();

            mState = AVRStudioSDK.TARGET.STATE.TARGET_STATE_RUNNING;

            string msg = "Run";
            mFrameWork.evtBroadcast((int)AVRStudioSDK.EVENT.AvrEventEnum.AVREVENT_TARGETRUN, ref msg);
        }

        unsafe public void SetEvent(uint ulCount, uint* pulAddress, uint ulMask)
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget SetEvent");

            List<uint> lst = null;
            switch (ulMask)
            {
                case 1:
                    lst = mAddr_USER;
                    break;
                case 2:
                    lst = mAddr_HLLSTAT;
                    break;
                case 4:
                    lst = mAddr_FUNCRET;
                    break;
                case 8:
                    lst = mAddr_FUNCCALL;
                    break;
                case 64:
                    lst = mAddr_TEMP;
                    break;
                case 128:
                    lst = mAddr_EXIT;
                    break;
            }
            if (lst == null)
                return;

            for (uint i = 0; i < ulCount; i++)
            {
                if ( !lst.Contains(pulAddress[i] ))
                {
                    lst.Add(pulAddress[i]);

                    if (ulMask == 1)
                    {
                        sendGDBmessage("+$Z0," + asHex(pulAddress[i] / 2) + ",2#");
                        string msg = recvGDBmessage();
                    }
                }
            }
        }

        public void SetGlobalEventMask(uint ulMask)
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget SetGlobalEventMask");
        }

        public void SetOptions(byte ucType)
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget SetOptions");

            mFrameWork.GetProjectFileValue("DEBUG_TARGET", "CURRENT_PART", out mPartname);
            switch (mPartname)
            {
                case "ATtiny4":
                case "ATtiny5":
                case "ATtiny9":
                case "ATtiny10":
                    mIOoffset = 0x0;
                    mREGoffset = 0x10;
                    break;

                default:
                    mIOoffset = 0x20;
                    mREGoffset = 0x0;
                    break;
            }
        }

        public void SingleStep(byte bHLLMode)
        {
            sendGDBmessage("+$s#");
            recvGDBmessage();

            dummyrun();
        }

        public void StepOut(byte bHLLMode)
        {
            sendGDBmessage("+$s#");
            recvGDBmessage();

            dummyrun();
        }

        public void StepOver(byte bHLLMode)
        {
            sendGDBmessage("+$s#");
            recvGDBmessage();

            dummyrun();
        }

        public void Stop(byte bHLLMode)
        {
            sendGDBmessageBreak();

            string msg = "";
            mFrameWork.evtBroadcast((int)AVRStudioSDK.EVENT.AvrEventEnum.AVREVENT_UPDATEVIEWS, ref msg);
            msg = "Stop";
            mFrameWork.evtBroadcast((int)AVRStudioSDK.EVENT.AvrEventEnum.AVREVENT_TARGETSTOP, ref msg);
        }
    
        public void WriteCycleCounter(uint ulCycleCounter)
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget WriteCycleCounter");
        }

        unsafe public void WriteMemory(uint ulMemoryType, uint ulAddress, byte* pMemory)
        {
            AVRStudioSDK.TARGET.MEM memtype = (AVRStudioSDK.TARGET.MEM)ulMemoryType;
            int numbytes = ((int*)pMemory)[-1];
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget WriteMemory " + memtype.ToString() + " " + asHex(ulAddress) + ":" + asHex(numbytes));

            bool writemem = false;
            switch(memtype)
            {
                case AVRStudioSDK.TARGET.MEM.TARGET_MEM_PROGMEMORY:
                    writemem = true;
                    break;

                case AVRStudioSDK.TARGET.MEM.TARGET_MEM_IO:
                    writemem = true;
                    ulAddress += 0x800000 + mIOoffset;
                    break;

                case AVRStudioSDK.TARGET.MEM.TARGET_MEM_SRAM:
                    writemem = true;
                    ulAddress += 0x800000;
                    break;

                case AVRStudioSDK.TARGET.MEM.TARGET_MEM_EEPROM:
                    writemem = true;
                    ulAddress += 0x810000;
                    break;

                case AVRStudioSDK.TARGET.MEM.TARGET_MEM_REGISTERS:
                    sendGDBmessage("+$P" + asHex(ulAddress + mREGoffset) + "=" + asHex(pMemory[0]) + "#");
                    recvGDBmessage();
                    break;
            }

            if (writemem)
            {
                string mem = "";
                for (int i=0; i<numbytes; i++)
                {
                    mem += asHex(pMemory[i]);
                }
                sendGDBmessage("+$M" + asHex(ulAddress) + "," + asHex(numbytes) + ":" + mem + "#");
                recvGDBmessage();
            }
        }

        public void WriteProgramCounter(uint ulPC)
        {
            System.Diagnostics.Debug.WriteLine("myCompany.myTarget WriteMemory");

            sendGDBmessage("+$P22=" + asLEHex(ulPC) + "#");
            string addr = recvGDBmessage();
        }
        #endregion

        void dummyrun()
        {
            string msg = "Run";
            mFrameWork.evtBroadcast((int)AVRStudioSDK.EVENT.AvrEventEnum.AVREVENT_TARGETRUN, ref msg);
            msg = "";
            mFrameWork.evtBroadcast((int)AVRStudioSDK.EVENT.AvrEventEnum.AVREVENT_UPDATEVIEWS, ref msg);
            msg = "Stop";
            mFrameWork.evtBroadcast((int)AVRStudioSDK.EVENT.AvrEventEnum.AVREVENT_TARGETSTOP, ref msg);

            mState = AVRStudioSDK.TARGET.STATE.TARGET_STATE_STOP;
        }
    }
}
