program TestHotkeyManager;

uses
  Forms,
  Main in 'Main.pas' {MainForm};

{$R *.res}

begin
  Application.Initialize;
  Application.Title := 'HotKeyManager test';
  Application.CreateForm(TMainForm, MainForm);
  Application.Run;
end.
