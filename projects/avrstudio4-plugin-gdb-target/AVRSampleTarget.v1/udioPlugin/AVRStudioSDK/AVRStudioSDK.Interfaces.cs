using System;
using System.Text;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using System.ComponentModel;
using System.Windows.Forms;
using System.Collections.Generic;

namespace AVRStudioSDK
{
    [ComImport]
    [Guid("9B206088-BF25-4EC4-A5AD-3EDAAE26849E")]
    [InterfaceTypeAttribute(ComInterfaceType.InterfaceIsDual | ComInterfaceType.InterfaceIsIDispatch)]
    public interface IGuiPlugin
    {
        [return: MarshalAs(UnmanagedType.BStr)]
        string GetSetup();
        void ExecuteCommand(int cmd);
        void HandleEvent([In, MarshalAs(UnmanagedType.I4)]AVRStudioSDK.EVENT.AvrEventEnum evnt, string evnttxt);
        void Init([In, MarshalAs(UnmanagedType.IDispatch)] object iguiworks);
        bool SaveFile(string fname);
        void Close();
    }

    [ComImport]
    [Guid("9B206088-BF25-4EC4-A5AD-3EDAAE26849F")]
    [InterfaceTypeAttribute(ComInterfaceType.InterfaceIsDual | ComInterfaceType.InterfaceIsIDispatch)]
    public interface IAvrProjectPlugin
    {
    }

    [ComImport]
    [Guid("797604A0-6A16-11D5-809B-006008125A1D")]
    [InterfaceTypeAttribute(ComInterfaceType.InterfaceIsDual | ComInterfaceType.InterfaceIsIDispatch)]
    public interface IAvrGuiPlugin
    {
        void guiPlugInExecuteCommand(uint command);
        void guiPlugInGetCommandStatus(uint commandId, out uint pCmndStatus);
        void guiPlugInInit(ref string projectFile, int status);
        void guiPlugInSetup(out string projectFile, int status);
    }

    [ComImport]
    [Guid("AD79A7D6-ADD3-4A9E-96C9-F1430F56EBE0")]
    [InterfaceTypeAttribute(ComInterfaceType.InterfaceIsDual | ComInterfaceType.InterfaceIsIDispatch)]
    public interface IAvrProgEventsSink
    {
        void Error(string msg);
        void Progress(double Progress);
    }

    [ComImport]
    [Guid("6B1E9886-0A8D-4201-9350-6EC848A18CFD")]
    [InterfaceTypeAttribute(ComInterfaceType.InterfaceIsDual | ComInterfaceType.InterfaceIsIDispatch)]
    public interface ISimulatorMKII
    {
    }

    [ComImport]
    [Guid("7005425C-5A1A-4941-BB55-BD2F014D6A5E")]
    [InterfaceTypeAttribute(ComInterfaceType.InterfaceIsDual | ComInterfaceType.InterfaceIsIDispatch)]
    public interface IAvrTarget
    {
        void IsPresent(out byte pucPresent);
        void GetState(out byte ucState);
        unsafe void WriteMemory(uint ulMemoryType, uint ulAddress, byte* pMemory);
        void ReadMemory(uint ulMemoryType, uint ulAddress, uint ulSize, [Out, MarshalAs(UnmanagedType.BStr)] out string pMemory);
        void ReadCycleCounter(out uint pulCycleCounter);
        void ReadProgramCounter(out uint pulPC);
        void Run();
        void SingleStep(byte bHLLMode);
        void Stop(byte bHLLMode);
        void Reset(byte bHLLMode);
        void GetInformation(out string pstrTargetConfigurationXML);
        void GetVersion(out string pbstrVersion);
        void WriteCycleCounter(uint ulCycleCounter);
        void WriteProgramCounter(uint ulPC);
        unsafe void SetEvent(uint ulCount, uint* pulAddress, uint ulMask);
        unsafe void ClearEvent(uint ulCount, uint* pulAddress, uint ulMask);
        void GetEvent(uint ulAddress, out uint pulMask);
        void GetGlobalEventMask(out uint pulGlobalMask);
        void SetGlobalEventMask(uint ulMask);
        void StepOver(byte bHLLMode);
        void StepOut(byte bHLLMode);
        void GotoAddress(uint ulAddress);
        void SetOptions(byte ucType);
        void Connect(string bstrComPort);
        void Disconnect();
        void ClearAllEvents();
    }

    [ComImport]
    [Guid("7A749B8A-22E8-4663-8072-C927A62EBFDD")]
    [InterfaceTypeAttribute(ComInterfaceType.InterfaceIsDual | ComInterfaceType.InterfaceIsIDispatch)]
    public interface IAvrSimulatorTest
    {
        void SetPart(string bstrPartName);
        void GotoAddressSync(int ulAddress, float timeout);
        Array ReadMemorySA(uint ulMemoryType, uint ulAddress, uint ulSize);
        void WriteMemorySA(uint ulMemoryType, uint ulAddress, Array pSafeArray);
    }

    [ComImport]
    [Guid("D7DEA3C4-1462-4D1B-A0B7-91347E7D5AE1")]
    [InterfaceTypeAttribute(ComInterfaceType.InterfaceIsDual | ComInterfaceType.InterfaceIsIDispatch)]
    public interface IAvrTargetDebugTriggerSupport
    {
        string GetDebugTriggerInfo(string clsid);
        void SetDebugTrigger(IAvrDebugTrigger trigger);
        void ClearDebugTrigger(IAvrDebugTrigger trigger);
        void ClearAllDebugTriggers(string clsid);
        IAvrDebugTrigger GetCurrentUserBreakpoint();
        bool UserBreakEnabledWhileStepping { set; }
        bool CanDisableUserBreakWhileStepping { get; }
        bool DisableDataBreakWhileResettingHighLevel { set; }
    }

    [ComImport]
    [Guid("3D4C54CB-1EFC-4C80-BC80-BCCB00AA3103")]
    [InterfaceTypeAttribute(ComInterfaceType.InterfaceIsDual | ComInterfaceType.InterfaceIsIDispatch)]
    public interface IAvrObject
    {
        void InitObj();
        void UninitObj();
        void LoadObj(object xmldomelement);
        void SaveObj(object xmldomelement);
        string info { get; }
        IAvrObject Parent { get; set; }
    }

    [ComImport]
    [Guid("A1FAE62D-5223-42FD-84BC-81F9E217C668")]
    [InterfaceTypeAttribute(ComInterfaceType.InterfaceIsDual | ComInterfaceType.InterfaceIsIDispatch)]
    public interface IAvrDebugTrigger : IAvrObject
    {
        /*DebugTriggerState*/ int State { get; set; }
        string Warning { get; set; }
        string Text { get; }
        string TypeName { get; }
        void GetImage(/*DebugTriggerIcon*/ int iconType, ref int hImageList, ref int index);
        string GetProperty(string name);
        void Restore();
        void Update(object moduleobject);
        int Line { get; set; }
        string File { get; set; }
        string Token { get; set; }
        bool UpdateAndContinue { get; set; }
        IAvrDebugTrigger MakeCopy(IAvrDebugTrigger inObj);
    }
}
