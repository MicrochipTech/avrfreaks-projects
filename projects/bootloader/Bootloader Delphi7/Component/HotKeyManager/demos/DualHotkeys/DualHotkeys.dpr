program DualHotkeys;

uses
  Forms,
  Main in 'Main.pas' {MainForm};

{$R *.res}

begin
  Application.Initialize;
  Application.Title := 'Dual hotkeys';
  Application.CreateForm(TMainForm, MainForm);
  Application.Run;
end.
