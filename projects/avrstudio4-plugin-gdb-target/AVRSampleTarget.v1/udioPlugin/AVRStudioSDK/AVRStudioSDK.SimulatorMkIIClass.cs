using System;
using System.Text;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using System.ComponentModel;
using System.Windows.Forms;
using System.Collections.Generic;

namespace AVRStudioSDK
{
    [StructLayout(LayoutKind.Explicit)]
    unsafe public struct struct_SimulatorMKIIClass
    {
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        delegate void Delegate_IAvrTarget_GetVersion([In, MarshalAs(UnmanagedType.Interface)] AVRSimulatorMKIILib.SimulatorMKIIClass This, [Out, MarshalAs(UnmanagedType.BStr)] out string version);

        static void SimulatorMKIIClass_IAvrTarget_GetVersion(AVRSimulatorMKIILib.SimulatorMKIIClass This, out string version)
        {
            // this will get the real interface
            IntPtr targetPtr = COMclassQuery.COMclassQuery.GetInterface(This, typeof(AVRStudioSDK.IAvrTarget));
            version = targetPtr.ToString();
            Marshal.Release(targetPtr);
            // call original function
            //Delegate_IAvrTarget_GetVersion orig = (Delegate_IAvrTarget_GetVersion)Marshal.GetDelegateForFunctionPointer(hookSimMk2GetVersion.OldProc, typeof(Delegate_IAvrTarget_GetVersion));
            //orig(This, out version);
        }

        public static struct_SimulatorMKIIClass* GetSimulatorMKIIClassFromComInterface(AVRStudioSDK.IAvrTarget target)
        {
            // temporarily hijack the GetVersion() function in order to get the proper class-pointer
            EasyHook.LocalHook hookSimMk2GetVersion = null;
            COMclassQuery.COMclassQuery.COMclassInfo SimMk2Ver = new COMclassQuery.COMclassQuery.COMclassInfo(typeof(AVRSimulatorMKIILib.SimulatorMKIIClass), typeof(AVRStudioSDK.IAvrTarget), "GetVersion");
            COMclassQuery.COMclassQuery.Query(SimMk2Ver);
            hookSimMk2GetVersion = EasyHook.LocalHook.Create(SimMk2Ver.FunctionPointer, new Delegate_IAvrTarget_GetVersion(SimulatorMKIIClass_IAvrTarget_GetVersion), null);
            hookSimMk2GetVersion.ThreadACL.SetExclusiveACL(new Int32[] { });
            // execute myGetVersion function
            string interfacePtr;
            target.GetVersion(out interfacePtr);
            // restore old function
            hookSimMk2GetVersion.Dispose();
            // the desired pointer to my class = ( COMinterface - 4 )
            int p = int.Parse(interfacePtr) - 4;
            return (struct_SimulatorMKIIClass*)p;
        }

        [FieldOffset(0x4)]
        public int** COMclass_vtable;

        [FieldOffset(0x30)]
        public struct_CarbonModel* carbonModel;
        public bool HasCarbonModel { get { return (int)carbonModel != 0; } }
    }

    [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
    unsafe delegate int Delegate_CarbonModel_intFvoid(struct_CarbonModel* This);
    [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
    unsafe delegate int Delegate_CarbonModel_intFint(struct_CarbonModel* This, int i);
    [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
    unsafe delegate int Delegate_CarbonModel_intFintint(struct_CarbonModel* This, int i1, int i2);
    [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
    unsafe delegate int Delegate_CarbonModel_intFintintint(struct_CarbonModel* This, int i1, int i2, int i3);

    [StructLayout(LayoutKind.Explicit)]
    unsafe public struct struct_CarbonModel
    {
        [FieldOffset(0)]
        struct_CarbonModel_vtable* vtable;

        public int CycleCounter
        {
            get
            {
                fixed(struct_CarbonModel*This = &this)
                    return ReadCycleCounter();
            }
            set
            {
                fixed (struct_CarbonModel* This = &this)
                    WriteCycleCounter(value, 0);
            }
        }

        public int ProgramCounter
        {
            get
            {
                fixed (struct_CarbonModel* This = &this)
                    return ReadProgramCounter();
            }
            set
            {
                fixed (struct_CarbonModel* This = &this)
                {
                    WriteProgramCounter(value);
                    ExecuteOneInstruction();
                }
            }
        }

        public int StackPointer
        {
            get
            {
                fixed (struct_CarbonModel* This = &this)
                    return ReadStackPointer();
            }
        }

        public struct structProgMemory
        {
            public ushort this[int index]
            {
                get
                {
                    fixed (structProgMemory* This = &this)
                    {
                        struct_CarbonModel* carbonModel = (struct_CarbonModel*)This;
                        return carbonModel->ReadProgMemoryWord(index);
                    }
                }
                set
                {
                    fixed (structProgMemory* This = &this)
                    {
                        struct_CarbonModel* carbonModel = (struct_CarbonModel*)This;
                        carbonModel->WriteProgMemoryWord(index, value);
                    }
                }
            }
        }
        [FieldOffset(0)]
        public structProgMemory ProgMemory;

        public struct structRegisters
        {
            public byte this[int index]
            {
                get
                {
                    fixed (structRegisters* This = &this)
                    {
                        struct_CarbonModel* carbonModel = (struct_CarbonModel*)This;
                        return carbonModel->ReadRegisterByte(index);
                    }
                }
                set
                {
                    fixed (structRegisters* This = &this)
                    {
                        struct_CarbonModel* carbonModel = (struct_CarbonModel*)This;
                        carbonModel->WriteRegisterByte(index, value);
                    }
                }
            }
        }
        [FieldOffset(0)]
        public structRegisters Registers;

        public struct structEEPROM
        {
            public byte this[int index]
            {
                get
                {
                    fixed (structEEPROM* This = &this)
                    {
                        struct_CarbonModel* carbonModel = (struct_CarbonModel*)This;
                        return carbonModel->ReadEEPROMByte(index);
                    }
                }
                set
                {
                    fixed (structEEPROM* This = &this)
                    {
                        struct_CarbonModel* carbonModel = (struct_CarbonModel*)This;
                        carbonModel->WriteEEPROMByte(index, value);
                    }
                }
            }
        }
        [FieldOffset(0)]
        public structEEPROM EEPROM;

        public struct structIO
        {
            public byte this[int index]
            {
                get
                {
                    fixed (structIO* This = &this)
                    {
                        struct_CarbonModel* carbonModel = (struct_CarbonModel*)This;
                        return carbonModel->ReadIOByte(index);
                    }
                }
                set
                {
                    fixed (structIO* This = &this)
                    {
                        struct_CarbonModel* carbonModel = (struct_CarbonModel*)This;
                        carbonModel->WriteIOByte(index, value);
                    }
                }
            }
        }
        [FieldOffset(0)]
        public structIO IO;

        public struct structSRAM
        {
            public byte this[int index]
            {
                get
                {
                    fixed (structSRAM* This = &this)
                    {
                        struct_CarbonModel* carbonModel = (struct_CarbonModel*)This;
                        return carbonModel->ReadSRAMByte(index);
                    }
                }
                set
                {
                    fixed (structSRAM* This = &this)
                    {
                        struct_CarbonModel* carbonModel = (struct_CarbonModel*)This;
                        carbonModel->WriteSRAMByte(index, value);
                    }
                }
            }
        }
        [FieldOffset(0)]
        public structSRAM SRAM;

        public void Reset()//, int HLLmode)
        {
            fixed (struct_CarbonModel* This = &this)
            {
                IntPtr tmp = new IntPtr(vtable->command_RESET);
                Delegate_CarbonModel_intFint tmpdel = (Delegate_CarbonModel_intFint)Marshal.GetDelegateForFunctionPointer(tmp, typeof(Delegate_CarbonModel_intFint));
                int res = tmpdel(This, 0);//, HLLmode);
                Marshal.Release(tmp);
            }
        }

        public void ExecuteOneInstruction()
        {
            fixed (struct_CarbonModel* This = &this)
            {
                IntPtr tmp = new IntPtr(vtable->command_EXECUTE_ONE_INSTRUCTION);
                Delegate_CarbonModel_intFvoid tmpdel = (Delegate_CarbonModel_intFvoid)Marshal.GetDelegateForFunctionPointer(tmp, typeof(Delegate_CarbonModel_intFvoid));
                int res = tmpdel(This);
                Marshal.Release(tmp);
            }
        }

        #region private functions
        int ReadCycleCounter()
        {
            fixed (struct_CarbonModel* This = &this)
            {
                IntPtr tmp = new IntPtr(vtable->command_READ_CYCLECOUNTER);
                Delegate_CarbonModel_intFvoid tmpdel = (Delegate_CarbonModel_intFvoid)Marshal.GetDelegateForFunctionPointer(tmp, typeof(Delegate_CarbonModel_intFvoid));
                int res = tmpdel(This);
                Marshal.Release(tmp);
                return res;
            }
        }

        void WriteCycleCounter(int lo, int hi)
        {
            fixed (struct_CarbonModel* This = &this)
            {
                IntPtr tmp = new IntPtr(vtable->command_WRITE_CYCLECOUNTER);
                Delegate_CarbonModel_intFintint tmpdel = (Delegate_CarbonModel_intFintint)Marshal.GetDelegateForFunctionPointer(tmp, typeof(Delegate_CarbonModel_intFintint));
                int res = tmpdel(This, lo, hi);
                Marshal.Release(tmp);
            }
        }

        int ReadProgramCounter()
        {
            fixed (struct_CarbonModel* This = &this)
            {
                IntPtr tmp = new IntPtr(vtable->command_READ_PROGRAMCOUNTER);
                Delegate_CarbonModel_intFvoid tmpdel = (Delegate_CarbonModel_intFvoid)Marshal.GetDelegateForFunctionPointer(tmp, typeof(Delegate_CarbonModel_intFvoid));
                int res = tmpdel(This);
                Marshal.Release(tmp);
                return res;
            }
        }

        // need to call ExecuteOneInstruction after this
        void WriteProgramCounter(int address)
        {
            fixed (struct_CarbonModel* This = &this)
            {
                IntPtr tmp = new IntPtr(vtable->command_WRITE_PROGRAMCOUNTER);
                Delegate_CarbonModel_intFint tmpdel = (Delegate_CarbonModel_intFint)Marshal.GetDelegateForFunctionPointer(tmp, typeof(Delegate_CarbonModel_intFint));
                int res = tmpdel(This, address);
                //Marshal.Release(tmp);
            }
        }

        int ReadStackPointer()
        {
            fixed (struct_CarbonModel* This = &this)
            {
                IntPtr tmp = new IntPtr(vtable->command_READ_STACKPOINTER);
                Delegate_CarbonModel_intFvoid tmpdel = (Delegate_CarbonModel_intFvoid)Marshal.GetDelegateForFunctionPointer(tmp, typeof(Delegate_CarbonModel_intFvoid));
                int res = tmpdel(This);
                Marshal.Release(tmp);
                return res;
            }
        }

        ushort ReadProgMemoryWord(int address)
        {
            fixed (struct_CarbonModel* This = &this)
            {
                IntPtr tmp = new IntPtr(vtable->command_READMEMORY_PROGMEMORY_GETWORD);
                Delegate_CarbonModel_intFint tmpdel = (Delegate_CarbonModel_intFint)Marshal.GetDelegateForFunctionPointer(tmp, typeof(Delegate_CarbonModel_intFint));
                ushort res = (ushort)tmpdel(This, address);
                Marshal.Release(tmp);
                return res;
            }
        }

        void WriteProgMemoryWord(int address, ushort instr)
        {
            fixed (struct_CarbonModel* This = &this)
            {
                IntPtr tmp = new IntPtr(vtable->command_WRITEMEMORY_PROGMEMORY_STOREWORD);
                Delegate_CarbonModel_intFintint tmpdel = (Delegate_CarbonModel_intFintint)Marshal.GetDelegateForFunctionPointer(tmp, typeof(Delegate_CarbonModel_intFintint));
                int res = tmpdel(This, address, (int)instr);
                Marshal.Release(tmp);
            }
        }

        byte ReadRegisterByte(int reg)
        {
            fixed (struct_CarbonModel* This = &this)
            {
                IntPtr tmp = new IntPtr(vtable->command_READMEMORY_REGISTER_GETBYTE);
                Delegate_CarbonModel_intFint tmpdel = (Delegate_CarbonModel_intFint)Marshal.GetDelegateForFunctionPointer(tmp, typeof(Delegate_CarbonModel_intFint));
                byte res = (byte)tmpdel(This, reg);
                Marshal.Release(tmp);
                return res;
            }
        }

        void WriteRegisterByte(int reg, byte val)
        {
            fixed (struct_CarbonModel* This = &this)
            {
                IntPtr tmp = new IntPtr(vtable->command_WRITEMEMORY_REGISTER_STOREBYTE);
                Delegate_CarbonModel_intFintint tmpdel = (Delegate_CarbonModel_intFintint)Marshal.GetDelegateForFunctionPointer(tmp, typeof(Delegate_CarbonModel_intFintint));
                int res = tmpdel(This, reg, (int)val);
                Marshal.Release(tmp);
            }
        }

        byte ReadEEPROMByte(int reg)
        {
            fixed (struct_CarbonModel* This = &this)
            {
                IntPtr tmp = new IntPtr(vtable->command_READMEMORY_EEPROM_GETBYTE);
                Delegate_CarbonModel_intFint tmpdel = (Delegate_CarbonModel_intFint)Marshal.GetDelegateForFunctionPointer(tmp, typeof(Delegate_CarbonModel_intFint));
                byte res = (byte)tmpdel(This, reg);
                Marshal.Release(tmp);
                return res;
            }
        }

        void WriteEEPROMByte(int reg, byte val)
        {
            fixed (struct_CarbonModel* This = &this)
            {
                IntPtr tmp = new IntPtr(vtable->command_WRITEMEMORY_EEPROM_STOREBYTE);
                Delegate_CarbonModel_intFintint tmpdel = (Delegate_CarbonModel_intFintint)Marshal.GetDelegateForFunctionPointer(tmp, typeof(Delegate_CarbonModel_intFintint));
                int res = tmpdel(This, reg, (int)val);
                Marshal.Release(tmp);
            }
        }

        byte ReadIOByte(int reg)
        {
            fixed (struct_CarbonModel* This = &this)
            {
                IntPtr tmp = new IntPtr(vtable->command_READMEMORY_IO_GETBYTE);
                Delegate_CarbonModel_intFint tmpdel = (Delegate_CarbonModel_intFint)Marshal.GetDelegateForFunctionPointer(tmp, typeof(Delegate_CarbonModel_intFint));
                byte res = (byte)tmpdel(This, reg);
                Marshal.Release(tmp);
                return res;
            }
        }

        void WriteIOByte(int reg, byte val)
        {
            fixed (struct_CarbonModel* This = &this)
            {
                IntPtr tmp = new IntPtr(vtable->command_WRITEMEMORY_IO_STOREBYTE);
                Delegate_CarbonModel_intFintint tmpdel = (Delegate_CarbonModel_intFintint)Marshal.GetDelegateForFunctionPointer(tmp, typeof(Delegate_CarbonModel_intFintint));
                int res = tmpdel(This, reg, (int)val);
                Marshal.Release(tmp);
            }
        }

        byte ReadSRAMByte(int reg)
        {
            fixed (struct_CarbonModel* This = &this)
            {
                IntPtr tmp = new IntPtr(vtable->command_READMEMORY_SRAM_GETBYTE);
                Delegate_CarbonModel_intFint tmpdel = (Delegate_CarbonModel_intFint)Marshal.GetDelegateForFunctionPointer(tmp, typeof(Delegate_CarbonModel_intFint));
                byte res = (byte)tmpdel(This, reg);
                Marshal.Release(tmp);
                return res;
            }
        }

        void WriteSRAMByte(int reg, byte val)
        {
            fixed (struct_CarbonModel* This = &this)
            {
                IntPtr tmp = new IntPtr(vtable->command_WRITEMEMORY_SRAM_STOREBYTE);
                Delegate_CarbonModel_intFintint tmpdel = (Delegate_CarbonModel_intFintint)Marshal.GetDelegateForFunctionPointer(tmp, typeof(Delegate_CarbonModel_intFintint));
                int res = tmpdel(This, reg, (int)val);
                Marshal.Release(tmp);
            }
        }
        #endregion

        public void ReadProgMemory(int address, int length, ushort* buffer)
        {
            fixed (struct_CarbonModel* This = &this)
            {
                IntPtr tmp = new IntPtr(vtable->command_READMEMORY_PROGMEMORY);
                Delegate_CarbonModel_intFintintint tmpdel = (Delegate_CarbonModel_intFintintint)Marshal.GetDelegateForFunctionPointer(tmp, typeof(Delegate_CarbonModel_intFintintint));
                int res = tmpdel(This, address, length, (int)buffer);
                Marshal.Release(tmp);
            }
        }

        public void ReadSRAMemory(int address, int length, ushort* buffer)
        {
            fixed (struct_CarbonModel* This = &this)
            {
                IntPtr tmp = new IntPtr(vtable->command_READMEMORY_SRAM);
                Delegate_CarbonModel_intFintintint tmpdel = (Delegate_CarbonModel_intFintintint)Marshal.GetDelegateForFunctionPointer(tmp, typeof(Delegate_CarbonModel_intFintintint));
                int res = tmpdel(This, address, length, (int)buffer);
                Marshal.Release(tmp);
            }
        }

        public void ReadIOMemory(int address, int length, ushort* buffer)
        {
            int SRAMaddress = IOaddress_to_SRAMaddress(address);
            this.ReadSRAMemory(SRAMaddress, length, buffer);
        }

        int IOaddress_to_SRAMaddress(int address)
        {
            fixed (struct_CarbonModel* This = &this)
            {
                IntPtr tmp = new IntPtr(vtable->IOaddress_to_SRAMaddress);
                Delegate_CarbonModel_intFint tmpdel = (Delegate_CarbonModel_intFint)Marshal.GetDelegateForFunctionPointer(tmp, typeof(Delegate_CarbonModel_intFint));
                int res = tmpdel(This, address);
                Marshal.Release(tmp);
                return res;
            }
        }
    }

    // SRAM: REGISTERS/IO - EEPROM - 2
    [StructLayout(LayoutKind.Explicit)]
    unsafe public struct struct_CarbonModel_vtable
    {
        [FieldOffset(0x4)]
        public int* command_RESET;  // ok
        [FieldOffset(0x8)]
        public int* command_RUNTOCURSOR;
        [FieldOffset(0xC)]
        public int* command_EXECUTE_ONE_INSTRUCTION;    // ok
        [FieldOffset(0x10)]
        public int* command_CONNECT;
        [FieldOffset(0x14)]
        public int* command_RUN;    // ok

        [FieldOffset(0x1C)]
        public int* command_READ_PROGRAMCOUNTER;    // ok
        [FieldOffset(0x20)]
        public int* command_WRITE_PROGRAMCOUNTER;   // ok
        [FieldOffset(0x24)]
        public int* command_READ_PREVIOUS_PROGRAMCOUNTER;   // ?

        [FieldOffset(0x2C)]
        public int* command_READ_STACKPOINTER;  // ok

        [FieldOffset(0x48)]
        public int* command_READMEMORY_REGISTER_GETBYTE;    // ok
        [FieldOffset(0x4C)]
        public int* command_WRITEMEMORY_REGISTER_STOREBYTE; // ok

        [FieldOffset(0x50)]
        public int* command_READMEMORY_PROGMEMORY_GETWORD;      // ok
        [FieldOffset(0x54)]
        public int* command_WRITEMEMORY_PROGMEMORY_STOREWORD;   // ok

        [FieldOffset(0x58)]
        public int* command_READMEMORY_SRAM_GETBYTE;
        [FieldOffset(0x5C)]
        public int* command_WRITEMEMORY_SRAM_STOREBYTE;

        [FieldOffset(0x60)]
        public int* command_READMEMORY_EEPROM_GETBYTE;      // ok
        [FieldOffset(0x64)]
        public int* command_WRITEMEMORY_EEPROM_STOREBYTE;   // ok

        [FieldOffset(0x68)]
        public int* command_READMEMORY_IO_GETBYTE;
        [FieldOffset(0x6C)]
        public int* command_WRITEMEMORY_IO_STOREBYTE;

        [FieldOffset(0x78)]
        public int* command_READMEMORY_PROGMEMORY;  // ok
        [FieldOffset(0x7C)]
        public int* command_WRITEMEMORY_PROGMEMORY; // ok

        [FieldOffset(0x80)]
        public int* command_READMEMORY_SRAM;    // ok
        [FieldOffset(0x84)]
        public int* command_WRITEMEMORY_SRAM;   // ok

        [FieldOffset(0x88)]
        public int* command_READMEMORY_EEPROM;  // ok
        [FieldOffset(0x8C)]
        public int* command_WRITEMEMORY_EEPROM; // ok

        [FieldOffset(0x90)]
        public int* command_READMEMORY_REGISTER;    // ok
        [FieldOffset(0x94)]
        public int* command_WRITEMEMORY_REGISTER;   // ok

        [FieldOffset(0xB4)]
        public int* command_READ_CYCLECOUNTER;      // ok
        [FieldOffset(0xB8)]
        public int* command_WRITE_CYCLECOUNTER;     // ok

        [FieldOffset(0xC0)]
        public int* IOaddress_to_SRAMaddress;       // ok
    }
}
