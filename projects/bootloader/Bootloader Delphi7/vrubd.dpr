program avrubd;

uses
  Forms,
  uMain in 'uMain.pas' {frmMain},
  uOption in 'uOption.pas' {frmOption},
  uAutoCode in 'uAutoCode.pas' {frmAutoCode},
  uAbout in 'uAbout.pas' {frmAbout};

{$R *.RES}

begin
  Application.Initialize;
  Application.Title := 'AVRUBD';
  Application.CreateForm(TfrmMain, frmMain);
  Application.CreateForm(TfrmOption, frmOption);
  Application.CreateForm(TfrmAutoCode, frmAutoCode);
  Application.CreateForm(TfrmAbout, frmAbout);
  //Load language file
  frmMain.loadlanguage(cfg.Language);
  Application.Run;
end.
