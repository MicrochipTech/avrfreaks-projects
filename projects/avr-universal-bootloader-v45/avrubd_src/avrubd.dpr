{
  Project:  AVR Universal Bootloader Download
  Author:   Shaoziyang
  Date:     2008.4
  Version:  4.0
  Web:      http://shaoziyang.googlepages.com
        or  http://shaoziyang.bloger.com.cn (Chinese)
  Email:    shaoziyang@gmail.com
}

program avrubd;

uses
  Forms,
  uMain in 'uMain.pas' {frmMain},
  uOption in 'uOption.pas' {frmOption},
  uAutoCode in 'uAutoCode.pas' {frmAutoCode},
  uAbout in 'uAbout.pas' {frmAbout},
  aes in 'aes.pas',
  PC1 in 'PC1.pas';

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
