unit Service;

interface

uses
  Windows, Classes, SvcMgr, HotKeyManager;

type
  TTestHotKeyService = class(TService)
    HotKeyManager1: THotKeyManager;
    procedure ServiceCreate(Sender: TObject);
    procedure ServiceExecute(Sender: TService);
    procedure StopService1Click(Sender: TObject);
    procedure ServiceAfterInstall(Sender: TService);
    procedure HotKeyPressed(HotKey: Cardinal; Index: Word);
  public
    function GetServiceController: TServiceController; override;
  end;

var
  TestHotKeyService: TTestHotKeyService;

implementation

uses
  ShellApi, WinSvc, Registry, SysUtils;

{$R *.DFM}

function ServiceStop(aMachine, aServiceName: String): Boolean;
// aMachine is UNC path or local machine if empty
var
  h_manager, h_svc: SC_Handle;
  ServiceStatus: TServiceStatus;
  dwCheckPoint: DWORD;
begin
  h_manager := OpenSCManager(PChar(aMachine), nil, SC_MANAGER_CONNECT);
  if h_manager > 0 then
  begin
    h_svc := OpenService(h_manager, PChar(aServiceName),
                         SERVICE_STOP or SERVICE_QUERY_STATUS);
    if h_svc > 0 then
    begin
      if (ControlService(h_svc, SERVICE_CONTROL_STOP, ServiceStatus)) then
      begin
        if (QueryServiceStatus(h_svc, ServiceStatus)) then
        begin
          while (SERVICE_STOPPED <> ServiceStatus.dwCurrentState) do
          begin
            dwCheckPoint := ServiceStatus.dwCheckPoint;
            Sleep(ServiceStatus.dwWaitHint);
            if (not QueryServiceStatus(h_svc, ServiceStatus)) then
              // couldn't check status
              break;
            if (ServiceStatus.dwCheckPoint < dwCheckPoint) then
              Break;
          end;
        end;
      end;
      CloseServiceHandle(h_svc);
    end;
    CloseServiceHandle(h_manager);
  end;

  Result := (SERVICE_STOPPED = ServiceStatus.dwCurrentState);
end;


procedure ServiceController(CtrlCode: DWord); stdcall;
begin
  TestHotKeyService.Controller(CtrlCode);
end;


function TTestHotKeyService.GetServiceController: TServiceController;
begin
  Result := ServiceController;
end;


procedure TTestHotKeyService.ServiceCreate(Sender: TObject);
begin
  HotKeyManager1.OnHotKeyPressed := HotKeyPressed;
  HotKeyManager1.AddHotKey(GetHotKey(MOD_CONTROL + MOD_ALT, Ord('F')));
end;


procedure TTestHotKeyService.ServiceExecute(Sender: TService);
begin
  MessageBox(0, 'The HotKeyManager sample service is running.' + #13 +
             'Press Ctrl+Alt+F to invoke.', 'Information', MB_ICONINFORMATION or MB_OK);
  while not Terminated do
  begin
    ServiceThread.ProcessRequests(True);
//    Sleep(1);   // If you use ProcessRequests(False), insert a Sleep(1) or the service will eat all cpu power
  end;
end;


procedure TTestHotKeyService.StopService1Click(Sender: TObject);
begin
//  WinExec(PChar('net stop '+Name), 0);  // Dirty indeed! Use ControlService instead!
  ServiceStop('', Name);
  ReportStatus;              // Notify the Service Manager (Windows)
end;


procedure TTestHotKeyService.ServiceAfterInstall(Sender: TService);
// Registers the service's description
var
  Reg: TRegistry;
begin
  Reg := TRegistry.Create(KEY_READ or KEY_WRITE);
  try
    Reg.RootKey := HKEY_LOCAL_MACHINE;
    if Reg.OpenKey('System\CurrentControlSet\Services\' + Name, True) then
    begin
      Reg.WriteString('Description', 'A sample service using the HotKeyManager component.');
    end
  finally
    Reg.Free;
  end;
end;


procedure TTestHotKeyService.HotKeyPressed(HotKey: Cardinal; Index: Word);
begin
  MessageBox(0, PChar('Hotkey ' + HotKeyToText(HotKey, False) + ' pressed.'),
             'Information', MB_ICONINFORMATION or MB_OK);
end;

end.

