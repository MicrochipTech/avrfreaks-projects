unit HighResTimer;

{******************************************************************************
*                                                                             *
* THighResTimer Component                                                     *
*                                                                             *
* Component for high resolution time measurement and wait routines.           *
* The component can reach resolution and accuracy of a few micro seconds.     *
* Because Win32 is not a real time operating system the resolution and the    *
* accuracy of this component can't be garuanteed. Setting the execution thread*
* and the component thread to an approbiate priority the resolution and       *
* accuracy will be with in the limits for more than 90% of the calls.         *
* Remarks:                                                                    *
*   For high accuracy of the time stamp counter a calibration should last two *
*   seconds or longer.                                                        *
* Ussage hints:                                                               *
*   For pure time measurement the timer doesn't need to be enabled. At the    *
*   begining test wether the TSC can be used or not.                          *
*   Befor using the Wait function enable the timer. The default calibration   *
*   is fast but not very accurate, so do a custom calibrate if needed.        *
*   After a change of the UseTSC property always a recalibration is needed for*
*   high accuracy. Use TSC can be cahnged only if timer is disabled.          *
*                                                                             *
* If you find any bugs or improvements, please let me know. If you modify the *
* source code, please send me a copy.                                         *
*                                                                             *
* Legal issues:                                                               *
*                                                                             *
* Copyright (C) 2002 by Roman Lauer <lauerr@gmx.de>                           *
*                                                                             *
*  This software is provided 'as-is', without any express or implied          *
*  warranty.  In no event will the author be held liable for any damages      *
*  arising from the use of this software.                                     *
*                                                                             *
*  Permission is granted to anyone to use this software for any purpose,      *
*  including commercial applications, and to alter it and redistribute it     *
*  freely, subject to the following restrictions:                             *
*                                                                             *
*  1. The origin of this software must not be misrepresented, you must not    *
*     claim that you wrote the original software. If you use this software    *
*     in a product, an acknowledgment in the product documentation would be   *
*     appreciated but is not required.                                        *
*                                                                             *
*  2. Altered source versions must be plainly marked as such, and must not be *
*     misrepresented as being the original software.                          *
*                                                                             *
*  3. This notice may not be removed or altered from any source distribution. *
*                                                                             *
*  4. These components may not be included in any component package that is   *
*      distributed for profit.                                                *
*                                                                             *
* Credits go to:                                                              *
*  Robert T. Palmqvist: I copy his header for these comments                  *
*  Udo Juerss: I used his TimeStampCounter and put it into this component.    *
*  Autor of Component tacpuid: Used some of his assembler routines.           *
*      (with modifications)                                                   *
*                                                                             *
* Bibliography:                                                               *
*                                                                             *
*   http://www.pergolesi.demon.co.uk/prog/threads/ToC.html                    *
*      Tutorial about multi threading with delphi                             *
*   MSDN                                                                      *
*      Learned about multi media timers.                                      *
*   IA-32 Intel ® Architecture Software Developer’s Manual Volume 3 : System  *
*   Programming Guide, 2002, Intel                                            *
*      Learned about the Pentium system architekture                          *
*   AP-485, Intel Processor Identification and the CPUID Instruction,         *
*   June 1998, Intel                                                          *
*      Learnd about the CPUID instruction and how to use it to identifiy the  *
*      used CPU ant it's features                                             *
*                                                                             *
* History:                                                                    *
*   Ver 1.02 Released 08/11/2002                                              *
*       Component could crash if no time stamp counter is available. Now the  *
*       time stamp counter is tested. If it is not available the component    *
*       switches to the high performance counter of windows. There for        *
*       renamed some properties. Component still doesn't test if access to    *
*       time stamp counter is restricted. Don't know how to access the TSD    *
*       flag in the CR4 register.                                             *
*                                                                             *
*   Ver 1.01 Released 07/31/2002                                              *
*       Removed to assembler lines that caused problems on NT machines.       *
*                                                                             *
*   Ver 1.00 Released 07/20/2002                                              *
*       No bugs found yet                                                     *
*                                                                             *
*                                                                             *
******************************************************************************}

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  MMSystem;

type

  TThreadClass = (tcIdle, tcNormal, tcHigh, tcRealTime);
  TMMTimerThread = class;
  TGetCounterValue = function : Int64 of object; stdcall;


  THighResTimer = class(TComponent)
  private
    FStartMeasureTime: Int64;
    FStopMeasureTime: Int64;
    FMinAccuracy: Cardinal;
    FMinResolution: Cardinal;
    FAccuracy: Cardinal;
    FResolution: Cardinal;
    FResolutionTicks: Int64;
    FThreadClass: TThreadClass;
    FThreadPriority: TThreadPriority;
    FMMTimerThread: TMMTimerThread;
    FTSC: Boolean;
    FUseTSC: Boolean;
    FEnabled: Boolean;
    FGetCounterValue: TGetCounterValue;
    FCountSpeed: Double;
    procedure SetAccuracy(const Value: Cardinal);
    procedure SetMinAccuracy(const Value: Cardinal);
    procedure SetMinResolution(const Value: Cardinal);
    procedure SetHRThreadClass(const Value: TThreadClass);
    procedure SetHRThreadPriority(const Value: TThreadPriority);
    procedure SetResolution(const Value: Cardinal);
    procedure SetResolutionTicks;
    function ExecuteCPUID(const Level:Integer; var eax, ebx, ecx, edx:Integer):Integer;
    function ExecuteCPUIDPtr(const Level:Integer;
      var eax, ebx, ecx, edx:Pointer):Integer; stdcall;
    function IsCPUIDAvailable: Boolean;
    function IsRDTSCAvailable: Boolean;
    procedure SetUseTSC(const Value: Boolean);
    procedure SetEnabled(const Value: Boolean);
    procedure SetCountSpeed(const Value: Double);
    procedure SetCounter;
  protected
  public
    constructor Create(aOwner : TComponent); override;
    destructor Destroy; override;
    procedure Calibrate(msCalibrateTime: Cardinal);
    function GetTSCValue: Int64;stdcall;
    function GetHPValue: Int64;stdcall;
    function GetCountValue: Int64;
    function GetCounterTicks(usTime: Cardinal): Int64;
    procedure Wait(usTime: Cardinal);
    procedure StartTimeMeasure;
    procedure StopTimeMeasure;
    function GetTimeDifference: Double;
  published
    property Accuracy: Cardinal read FAccuracy write SetAccuracy;
    property CountSpeed: Double read FCountSpeed write SetCountSpeed stored False;
    property Enabled: Boolean read FEnabled write SetEnabled default False;
    property MinAccuracy: Cardinal read FMinAccuracy write SetMinAccuracy stored False;
    property MinResolution: Cardinal read FMinResolution write SetMinResolution stored False;
    property Resolution: Cardinal read FResolution write SetResolution;
    property ThreadClass: TThreadClass read FThreadClass write SetHRThreadClass default tcNormal;
    property ThreadPriority: TThreadPriority read FThreadPriority write SetHRThreadPriority default tpNormal;
    property TSC: Boolean read FTSC stored False;
    property UseTSC: Boolean read FUseTSC write SetUseTSC default False;
  end;

  TMMTimerThread = class(TThread)
  private
    FGetCountValue: TGetCounterValue;
    FAccuracy: Cardinal;
    FTimerID: Cardinal;
    FEventHandle: THandle;
    FTickEventHandle: THandle;
    FChanged: Boolean;
    FCountValue: Int64;
    FResolution: Cardinal;
    FReadWriteSync: TMultiReadExclusiveWriteSynchronizer;
    procedure SetAccuracy(const Value: Cardinal);
    function GetCountValue: Int64;
    procedure SetCountValue(const Value: Int64);
    procedure SetResolution(const Value: Cardinal);
  protected
    procedure Execute; override;
  public
    constructor Create(CreateSuspended: Boolean);
    destructor Destroy; override;
    procedure WaitNextTick;
    property Accuracy: Cardinal read FAccuracy write SetAccuracy;
    property CountValue: Int64 read GetCountValue;
    property Resolution: Cardinal read FResolution write SetResolution;
  end;

procedure Register;

implementation

const
  CalibrateDelayTime = 100; // Time for calibation at start up in ms
  PriorityClasses: array [TThreadClass] of Integer =
   (IDLE_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, HIGH_PRIORITY_CLASS,
    REALTIME_PRIORITY_CLASS);

procedure Register;
begin
  RegisterComponents('System', [THighResTimer]);
end;

{ THighResTimer }

procedure THighResTimer.Calibrate(msCalibrateTime: Cardinal);
// Calibration routine to compute the processor speed
var
  TSCStartTime, TSCEndTime: Int64;
  HPStartTime, HPEndTime: Int64;
  PriorityClass,Priority: Integer;
begin
  // calculate the count speed of high performance counter
  QueryPerformanceFrequency(HPStartTime);
  FCountSpeed := HPStartTime / 1000000.0;
  if FUseTSC then
  begin
    // use the time stamp counter
    PriorityClass:=GetPriorityClass(GetCurrentProcess);
    Priority:=GetThreadPriority(GetCurrentThread);
    SetPriorityClass(GetCurrentProcess,REALTIME_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread,{THREAD_PRIORITY_TIME_CRITICAL}31);
    Sleep(10);
    TSCStartTime := GetTSCValue;
    QueryPerformanceCounter(HPStartTime);
    Sleep(msCalibrateTime);
    TSCEndTime:=GetTSCValue;
    QueryPerformanceCounter(HPEndTime);
    SetThreadPriority(GetCurrentThread,Priority);
    SetPriorityClass(GetCurrentProcess,PriorityClass);
    // calculate the time that passed by
    FCountSpeed := (HPEndTime - HPStartTime)/FCountSpeed;
    // calculate the count speed of the time stamp counter
    FCountSpeed:=(TSCEndTime - TSCStartTime) / FCountSpeed;
  end;
end;

constructor THighResTimer.Create(aOwner: TComponent);
var
  tc: TIMECAPS;
begin
  inherited;
  // set measure values to 0, so the time since when the CPU run's can be calculated
  FStartMeasureTime:=0;
  FStopMeasureTime:=0;
  // figure out the default settings for accuracy and resolution
  if VER_PLATFORM_WIN32_NT = Win32Platform then
  begin
    // working on WINNT
    // on NT systems the accuracy of the multi media timer is 1 ms in most of the cases
    FMinAccuracy := 1;
  end
  else
  begin
    // working on Win9x
    // on Win9x the accuracy of the multi media timer is 5 ms in most of the
    // cases but it still supports higher resolutions.
    FMinAccuracy := 5;
  end;
  FAccuracy := 0; // set highest possible accuracy
  // get min resolution supported by system
  TimeGetDevCaps(@tc, SizeOf(tc));
  FMinResolution := tc.wPeriodMin;
  FResolution := FMinAccuracy;
  SetResolutionTicks;  // init FResolutionTicks
  // set the thread priorities and the thread it self
  FThreadClass := tcNormal;
  FThreadPriority := tpNormal;
  FMMTimerThread := nil;
  // by default the timer is disabled
  FEnabled := False;
  // Test for time stamp counter. By default TSC is not available
  FTSC := False;
  FUseTSC := False;
  if IsCPUIDAvailable then
  begin
    // test for rdtsc instruction
    if IsRDTSCAvailable then
    begin
      // time stamp counter available, so by default use it
      FTSC := True;
      FUseTSC := True;
    end;
  end;
  // now it's clear what counter to use and whats available, so set
  SetCounter;                    // init FGetCounterValue
  Calibrate(CalibrateDelayTime); // init  FCountSpeed
end;

destructor THighResTimer.Destroy;
begin
  // stop timer if running
  Enabled := False;
  inherited;
end;

function THighResTimer.ExecuteCPUID(const Level: Integer; var eax, ebx,
  ecx, edx: Integer): Integer;
begin
  Result:=ExecuteCPUIDPtr(Level, Pointer(eax), Pointer(ebx), Pointer(ecx),
    Pointer(edx));
end;

const _cpuType :Byte = 0; //it had to be here, do not move after funct declaration
function THighResTimer.ExecuteCPUIDPtr(const Level: Integer; var eax, ebx,
  ecx, edx: Pointer): Integer;
const
  _cpuTypeBit = 12;
  _PSNBitMask = $200000;
asm
  @@Begin:
    cmp Level, 3             // Cyrix workaround:
    jnz @@CyrixPass          // PSN-bit mus be enabled
    pushfd                   // no way to turn it back (off) ;)
    pop EAX                  // if you want to do so
    or EAX, _PSNBitMask      // pushfd at begin and popfd at end
    push EAX                 // beware of lost of flow-control
    popfd
  @@CyrixPass:
    cmp Level, 2
    jnz @@Synchronized
  @@MPCheck:                 // Multi Processor Check Synchronicity
                             // Differentiate only primary & non-primary
    mov EAX,1
    dw $A20F                 // cpuid
    shr EAX, _cpuTypeBit     // extract cpuType
    and AL, 3                // validate bit-0 and bit-1
    cmp AL, _cpuType         // compare wih previous result
    mov _cpuType, AL         // save current value
    loopnz @@MPCheck
  @@Synchronized:
    mov EAX, Level
    dw $A20F                 // cpuid
    push EAX
    mov EAX, [&ecx]          // var argument is REALLY a pointer-
    mov [EAX], ECX           // load it first to register & then
    mov EAX, [&edx]          // you can get the value it's refers to
    mov [EAX], EDX
    mov EAX, [&ebx]
    mov [EAX], EBX
    pop EAX
    push EBX
    mov EBX, [&eax]
    mov [EBX], EAX
    pop EBX
    cmp Level, 0             // is it a level 0 Query?
    jnz @@End
    push EAX                 // save eax result
    shr EAX, _cpuTypeBit     // extract cpuType
    and AL, 3                // validate bit-0 and bit-1
    mov _cpuType, AL
    pop EAX
  @@End:
    mov @Result, EAX                   // done.
end;

function THighResTimer.GetCounterTicks(usTime: Cardinal): Int64;
// calculates the number of counter ticks for a given time in micro seconds
begin
  Result:=Round(usTime * FCountSpeed);
end;

function THighResTimer.GetCountValue: Int64;
begin
  Result := FGetCounterValue;
end;

function THighResTimer.GetHPValue: Int64;
begin
  QueryPerformanceCounter(Result);
end;

function THighResTimer.GetTimeDifference: Double;
// calculates the time in micro seconds passed between start and stop
begin
  Result := (FStopMeasureTime - FStartMeasureTime) / FCountSpeed;
end;

// returns the time stamp counter value
function THighResTimer.GetTSCValue: Int64;
asm
    dw    $310F  // rdtsc
end;

function THighResTimer.IsCPUIDAvailable: Boolean;
asm
    pushfd                // push original EFLAGS
    pop EAX               // get original EFLAGS
    mov ECX, EAX          // save original EFLAGS
                          // Checks ability to set/clear ID flag (Bit 21) in EFLAGS
                          // (indicating the presence of cpuid instruction)
    xor EAX, $200000      // flip bit-21 (ID) in EFLAGS
    push EAX              // save new EFLAGS value
    popfd                 // replace current EFLAGS value
    pushfd                // get new EFLAGS
    pop EAX               // store new EFLAGS in EAX
    push ECX              // restore back
    popfd                 // original flags - intel's slipped here ;-(
    xor EAX, ECX          // compare ID bit,
    mov EAX, False        // without cpuid
    je @@end              // cannot toggle ID bit
    mov EAX, True         // cpuid available
  @@end:                  // done.
end;

function THighResTimer.IsRDTSCAvailable: Boolean;
var
  eax, ebx, ecx, edx :Integer;
begin
  Result := False;
  ExecuteCPUID(0, eax, ebx, ecx, edx);
  if eax > 0 then
  begin
    // CPUID can be called with parameter 1 to get the features flags
    ExecuteCPUID(1, eax, ebx, ecx, edx);
    // test the tsc flag
    if (edx and $10) <> 0 then
    begin
      Result := True;
    end
  end;
end;

procedure THighResTimer.SetAccuracy(const Value: Cardinal);
begin
  FAccuracy := Value;
  if Assigned(FMMTimerThread) then
  begin
    FMMTimerThread.Accuracy := FAccuracy;
  end;
end;

procedure THighResTimer.SetCounter;
// updates the function pointer for reading a counter value
begin
  if FUseTSC then
  begin
    // use ime stamp counter
    FGetCounterValue := GetTSCValue;
  end
  else
  begin
    // use high performance counter
    FGetCounterValue := GetHPValue;
  end;
end;

procedure THighResTimer.SetCountSpeed(const Value: Double);
// dumy, so the count speed in MHz appears in the object inspector
begin
end;

procedure THighResTimer.SetEnabled(const Value: Boolean);
begin
  if Value <> FEnabled then
  begin
    FEnabled := Value;
    if FEnabled then
    begin
      // start timer thread
      if not (csDesigning in ComponentState) then
      begin
        // create timer thread only at runtime
        FMMTimerThread := TMMTimerThread.Create(True);
        FMMTimerThread.Accuracy := FAccuracy;
        FMMTimerThread.Resolution := FResolution;
        FMMTimerThread.FGetCountValue := FGetCounterValue; // ????
        FMMTimerThread.FreeOnTerminate := False;
        // start timer thread
        FMMTimerThread.Resume;
      end;
    end
    else
    begin
      // stop timer thread
      if Assigned(FMMTimerThread) then
      begin
        // thread exists, so terminate it
        FMMTimerThread.Terminate;
        // wait until thread terminated
        WaitForSingleObject(FMMTimerThread.Handle, INFINITE);
        // free thread
        FMMTimerThread.Free;
      end;
    end;
  end;
end;

procedure THighResTimer.SetHRThreadClass(const Value: TThreadClass);
begin
  FThreadClass := Value;
  if Assigned(FMMTimerThread) then
  begin
    SetPriorityClass(FMMTimerThread.Handle, PriorityClasses[FThreadClass]);
  end;
end;

procedure THighResTimer.SetHRThreadPriority(const Value: TThreadPriority);
begin
  FThreadPriority := Value;
  if Assigned(FMMTimerThread) then
  begin
    FMMTimerThread.Priority := FThreadPriority
  end;
end;

procedure THighResTimer.SetMinAccuracy(const Value: Cardinal);
// dummy, so MinAccuracy in ms is displayed in object inspector
begin
end;

procedure THighResTimer.SetMinResolution(const Value: Cardinal);
// dummy, so MinResolution in ms is displayed in object inspector
begin
end;

procedure THighResTimer.SetResolution(const Value: Cardinal);
begin
  FResolution := Value;
  SetResolutionTicks;
  if Assigned(FMMTimerThread) then
  begin
    FMMTimerThread.Resolution := FResolution;
  end;
end;

procedure THighResTimer.SetResolutionTicks;
begin
  // FResolution is in ms, so convert into counter ticks
  FResolutionTicks := GetCounterTicks(FResolution * 1000);
end;

procedure THighResTimer.SetUseTSC(const Value: Boolean);
begin
  if Value <> FUseTSC then
  begin
    if FTSC then
    begin
      // Time stamp counter available
      if not FEnabled then
      begin
        // timer not enabled, so allowed to change
        FUseTSC := Value;
        SetCounter;
      end;
    end
    else
    begin
      FUseTSC := False;
    end;
    Calibrate(CalibrateDelayTime);
  end;
end;

procedure THighResTimer.StartTimeMeasure;
// starts a time measurement
begin
  FStartMeasureTime := FGetCounterValue;
end;

procedure THighResTimer.StopTimeMeasure;
// stops a time measurement
begin
  FStopMeasureTime := FGetCounterValue;
end;

procedure THighResTimer.Wait(usTime: Cardinal);
// waits the given time in micro seconds
var
  ThreadTime: Int64;
  EndTime: Int64;
  TestForSuspend: Boolean;
begin
  // read actual counter value
  EndTime := FGetCounterValue;
  if FEnabled then
  begin
    // calculate the counter value at end of wait time
    EndTime := EndTime + GetCounterTicks(usTime);
    // by defuault test wether thread can be suspended or not
    TestForSuspend := True;
    while EndTime > FGetCounterValue do
    begin
      if True = TestForSuspend then
      begin
        // get counter value the timer thread last was active
        ThreadTime := FMMTimerThread.CountValue;
        // test for next expected occurance of a timer event
        if (EndTime - FResolutionTicks) < ThreadTime then
        begin
          // the end of the wait time is before the next timer tick will occur
          // so do not suspend and count until end time reached
          TestForSuspend := False;
        end
        else
        begin
          // if next timer tick occures in time, than enough time to suspend
          FMMTimerThread.WaitNextTick;
        end;
      end;
    end;
  end;
end;

{ TMMTimerThread }

constructor TMMTimerThread.Create(CreateSuspended: Boolean);
begin
  inherited Create(CreateSuspended);
  FChanged := True;
  FAccuracy := 0;  // use highes possible accuracy by default
  FResolution := 1;   // use 1 ms resolution by default
  FTimerID := 0;
  FEventHandle := INVALID_HANDLE_VALUE;
  // create the synchronisation object
  FReadWriteSync := TMultiReadExclusiveWriteSynchronizer.Create;
  FTickEventHandle := CreateEvent(nil, False, False, nil);
  FCountValue := 0;
  FGetCountValue := nil;
end;

destructor TMMTimerThread.Destroy;
begin
  CloseHandle(FTickEventHandle);
  FReadWriteSync.Free;
  inherited Destroy;
end;

procedure TMMTimerThread.Execute;
var
  EventCountValue: Int64;
begin
  // create event
  FEventHandle := CreateEvent(nil, False, False, nil);
  // delete the changed flag, because all changes go into this start
  FChanged := False;
  // start MM timer so it periodical sets an event
  FTimerID :=  timeSetEvent(FResolution, FAccuracy, TFNTimeCallBack(FEventHandle), 0,
                            TIME_PERIODIC or TIME_CALLBACK_EVENT_SET);
  while not Terminated do
  begin
    if FChanged then
    begin
      // stop timer
      timeKillEvent(FTimerID);
      // restart MM timer with new settings
      FTimerID :=  timeSetEvent(FResolution, FAccuracy, TFNTimeCallBack(FEventHandle), 0,
                                TIME_PERIODIC or TIME_CALLBACK_EVENT_SET);
    end;
    // suspend on event
    WaitForSingleObject(FEventHandle, INFINITE);
    // log count value
    EventCountValue := FGetCountValue;
    // store count value for component to use
    SetCountValue(EventCountValue);
    // release a waiting timer
    SetEvent(FTickEventHandle);
  end;
  // stop timer
  timeKillEvent(FTimerID);
  // release event
  CloseHandle(FEventHandle);
end;

function TMMTimerThread.GetCountValue: Int64;
begin
  FReadWriteSync.BeginRead;
  Result := FCountValue;
  FReadWriteSync.EndRead;
end;

procedure TMMTimerThread.SetAccuracy(const Value: Cardinal);
begin
  FAccuracy := Value;
  FChanged := True;
end;

procedure TMMTimerThread.SetResolution(const Value: Cardinal);
begin
  FResolution := Value;
  FChanged := True;
end;

procedure TMMTimerThread.SetCountValue(const Value: Int64);
begin
  FReadWriteSync.BeginWrite;
  FCountValue := Value;
  FReadWriteSync.EndWrite;
end;

procedure TMMTimerThread.WaitNextTick;
begin
  WaitForSingleObject(FTickEventHandle, FResolution*2);
end;

end.
