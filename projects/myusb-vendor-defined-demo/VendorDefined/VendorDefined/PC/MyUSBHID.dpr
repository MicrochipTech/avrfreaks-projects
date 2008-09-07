program MyUSBHID;

uses
  Forms,
  Main in 'Main.pas' {HIDForm};

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(THIDForm, HIDForm);
  Application.Run;
end.
