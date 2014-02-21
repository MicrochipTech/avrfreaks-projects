program ListVirtualKeyNames;

uses
  Forms,
  Main in 'Main.pas' {Form1};

{$R *.res}

begin
  Application.Initialize;
  Application.Title := 'Virtual key names';
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
