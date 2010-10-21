using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;
using System.Windows.Forms;

namespace myCompany
{
    [Guid("0D53A3E8-E51A-49C7-944E-E72A2064F938")]
    [ClassInterface(ClassInterfaceType.None)]
    [ProgId("myCompany.myPlugin")]
    unsafe public partial class myPlugin : AVRStudioSDK.AvrStudioPlugin, AVRStudioSDK.IGuiPlugin
    {
        string mSelectedTarget = "";
        string mObjectFile = "";

        public myPlugin()
        {
            //MessageBox.Show("myPlugin instantiated");
        }

        public string GetSetup()
        {
            //MessageBox.Show("myPlugin GetSetup");

            // provide XML with infos about this plugin
            // note that the mentioned BITMAPID's must be present in the mscoree.dll / mscoree_forward.dll
            FileInfo fi = new FileInfo(this.GetType().Module.FullyQualifiedName);
            FileStream fs = new FileStream(fi.Directory.FullName + "\\myPluginSetup.xml", FileMode.Open);
            StreamReader sr = new StreamReader(fs);
            string myxml = sr.ReadToEnd();
            sr.Close();
            fs.Close();
            return myxml;
        }

        public unsafe void ExecuteCommand(int cmd)
        {
            //MessageBox.Show("myPlugin ExecuteCommand");

            object oTarget;
            Guid targetguid = typeof(AVRStudioSDK.IAvrTarget).GUID;
            mFrameWork.rotGetObject(ref AVRStudioSDK.ROT.COMP_TARGET, ref targetguid, out oTarget);
            AVRStudioSDK.IAvrTarget target = oTarget as AVRStudioSDK.IAvrTarget;
            if (target != null)
            {
                target.Stop(0);
                target.Disconnect();
            }
#if false
            //if (mGuiWorks.IsDebugMode())
            {
                string partname = mGuiWorks.GetPartName();
                //MessageBox.Show("The part name is " + partname);

                string rotnfo = mFrameWork.rotGetInfo();

                object oTarget;
                Guid targetguid = typeof(AVRStudioSDK.IAvrTarget).GUID;
                mFrameWork.rotGetObject(ref AVRStudioSDK.ROT.COMP_TARGET, ref targetguid, out oTarget);

                AVRStudioSDK.IAvrTargetDebugTriggerSupport ts = oTarget as AVRStudioSDK.IAvrTargetDebugTriggerSupport;
                if (ts != null)
                {
                    string dti1 = ts.GetDebugTriggerInfo("{113824F1-C410-4699-A25E-867CC860C28E}");
                    string dti2 = ts.GetDebugTriggerInfo("{11A8571C-BF39-4FA7-8642-286DD19644B8}");
                    string dti3 = ts.GetDebugTriggerInfo("{8877E43C-062B-4643-A8DF-12ED926F98C8}");
                }
                AVRStudioSDK.IAvrTarget tg = oTarget as AVRStudioSDK.IAvrTarget;
                if (tg != null)
                {
                    string inf="";
                    tg.GetInformation(out inf);
                }

                /*
                AVRTARGETSIMULATORLib.IAvrTarget target = oTarget as AVRTARGETSIMULATORLib.IAvrTarget;
                if (target != null)
                {
                    string partinfo;
                    target.GetInformation(out partinfo);

                    string regs;
                    target.ReadMemory((uint)AVRStudioSDK.TARGET.MEM.TARGET_MEM_REGISTERS, 0, 32, out regs);

                    // note: messing with the carbonmodel directly is not a good thing
                    // imagine the model is already running and you call "execute_one_instruction"
                    AVRStudioSDK.struct_SimulatorMKIIClass* simulatorClass = AVRStudioSDK.struct_SimulatorMKIIClass.GetSimulatorMKIIClassFromComInterface(target);
                    AVRStudioSDK.struct_CarbonModel* carbonModel = simulatorClass->carbonModel;

                    //carbonModel->IO[0x3d] = 0x11;   // stackpointer
                    carbonModel->SRAM[0x40] = 0x01;

                    ushort[] buff = new ushort[102];
                    fixed (ushort* buffP = &buff[0])
                    {
                        carbonModel->ReadProgMemory(0, 100, buffP);
                    }

                    ushort instr = carbonModel->ProgMemory[1];
                    carbonModel->ProgMemory[0] = carbonModel->ProgMemory[1];

                    ushort R16 = carbonModel->Registers[0];
                    carbonModel->Registers[3] = 0x0d;

                    carbonModel->ProgramCounter = 1;

                    int cnt = carbonModel->CycleCounter;
                    carbonModel->CycleCounter = 0x111;
                    cnt = carbonModel->CycleCounter;

                    int PC = carbonModel->ProgramCounter;
                    int sp = carbonModel->StackPointer;
                    carbonModel->ExecuteOneInstruction();
                    carbonModel->Reset();
                    cnt = carbonModel->CycleCounter;
                }
                */
            }
            //else
            {
                //MessageBox.Show("Starting DEBUG mode");
                //mGuiWorks.StartDebug();
            }
#endif
        }

        public void HandleEvent(AVRStudioSDK.EVENT.AvrEventEnum evnt, string evnttxt)
        {
            //MessageBox.Show("myPlugin HandleEvent");

            System.Diagnostics.Debug.WriteLine(evnt.ToString() + "  :  " + evnttxt);

            if (evnt.ToString().Length < 5)
            {
                // event cannot be converted to enum,
                // which means the list is incomplete
            }

            switch (evnt)
            {
                case AVRStudioSDK.EVENT.AvrEventEnum.AVREVENT_PARTCHANGED:
                    mSelectedTarget = evnttxt;
                    break;

                case AVRStudioSDK.EVENT.AvrEventEnum.AVREVENT_OBJECTFILE_READY:
                    mObjectFile = evnttxt;
                    break;

                case AVRStudioSDK.EVENT.AvrEventEnum.AVREVENT_F1_HELP:
                    MessageBox.Show("Help for '" + evnttxt + "' in file " + mGuiWorks.GetActiveFile());
                    break;

                case AVRStudioSDK.EVENT.AvrEventEnum.AVREVENT_DEBUGSTARTING:
                    //MessageBox.Show("Ah, the debugging is starting!");
                    break;

                case AVRStudioSDK.EVENT.AvrEventEnum.AVREVENT_DEBUGSTOPPING:
                    //MessageBox.Show("Oh, the debugging is stopping!");
                    break;
            }
        }

        AVRStudioLib.IGuiWorks mGuiWorks = null;
        AVRFRAMEWORKLib.FrameWorkClass mFrameWork = null;
        public void Init(object iguiworks)
        {
            //MessageBox.Show("myPlugin Init");

            // save pointer to IGuiWorks
            // if you do not want to save it, Marshal.ReleaseComObject() it here
            mGuiWorks = (AVRStudioLib.IGuiWorks)iguiworks;

            // create a framework
            bool fullframework = true;
            mFrameWork = new AVRFRAMEWORKLib.FrameWorkClass();
            mFrameWork.create(this.GetType().FullName, fullframework);
        }

        public bool SaveFile(string fname)
        {
            //MessageBox.Show("myPlugin SaveFile");

            return true;
        }

        public void Close()
        {
            //MessageBox.Show("myPlugin Close");

            // release the iguiworks, otherwise AVRStudio will hang on shutdown
            Marshal.ReleaseComObject(mGuiWorks);
            mGuiWorks = null;
        }
    }
}
