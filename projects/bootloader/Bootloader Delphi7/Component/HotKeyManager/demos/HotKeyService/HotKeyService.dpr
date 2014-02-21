program HotKeyService;

uses
  SvcMgr,
  Service in 'Service.pas' {TestHotKeyService: TService};

{$R *.RES}

begin
  Application.Initialize;
  Application.CreateForm(TTestHotKeyService, TestHotKeyService);
  Application.Run;
end.
