{
  Project:  AVR Universal Bootloader Download
  File:     uAbout.pas
  Describe: Show about dialog and statics data.
  Author:   Shaoziyang
  Date:     2007.6
  Version:  4.0
  Web:      http://shaoziyang.googlepages.com
        or  http://shaoziyang.bloger.com.cn (Chinese)
  Email:    shaoziyang@gmail.com
}

unit uAbout;

interface

uses
  Windows,
  Messages,
  SysUtils,
  Classes,
  Graphics,
  Controls,
  Forms,
  Dialogs,
  StdCtrls,
  Buttons,
  ExtCtrls,
  ShellAPI;

type
  TfrmAbout = class(TForm)
    btnClearStat: TBitBtn;
    btnOk: TBitBtn;
    imgAVRUBD: TImage;
    imgBK: TImage;
    Label1: TLabel;
    Label2: TLabel;
    lbAllFail: TLabel;
    lbAllSucc: TLabel;
    lbAllSum: TLabel;
    lbCurDown: TLabel;
    lbCurFail: TLabel;
    lbCurSucc: TLabel;
    lbEMAIL: TLabel;
    lbFail: TLabel;
    lbHTTP: TLabel;
    lbRun: TLabel;
    lbSucc: TLabel;
    lbSum: TLabel;
    lbTotalDown: TLabel;
    lbTotalFail: TLabel;
    lbTotalRun: TLabel;
    lbTotalSucc: TLabel;
    lbTranslator: TLabel;
    lbVer: TLabel;
    tmr: TTimer;
    procedure btnClearStatClick(Sender: TObject);
    procedure FormClick(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure FormKeyPress(Sender: TObject; var Key: Char);
    procedure FormShow(Sender: TObject);
    procedure imgAVRUBDMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure lbEMAILClick(Sender: TObject);
    procedure lbHTTPClick(Sender: TObject);
    procedure tmrTimer(Sender: TObject);
  private
    { Private declarations }
    tmrclk: Word;
  public
    { Public declarations }
    procedure ShowStat;
  end;

const
  RainbowLeft = 24;
  RainbowTop = 40;
  RainbowWidth = 233;
  RainbowHeight = 2;

var
  frmAbout: TfrmAbout;

implementation

uses
  uMain;

{$R *.DFM}

procedure TfrmAbout.tmrTimer(Sender: TObject);
var
  i, w: Integer;
begin
  tmrclk := tmrclk - 1;
  tmr.Enabled := False;
  w := RainbowWidth div 7;
  for i := 0 to w - 1 do
  begin
    //draw rainbow bar
    Canvas.Pen.Color := RGB(255, Round(255 * i / w), 0);
    Canvas.MoveTo((i + tmrclk) mod RainbowWidth + RainbowLeft, RainbowTop);
    Canvas.LineTo((i + tmrclk) mod RainbowWidth + RainbowLeft, RainbowTop + RainbowHeight);
    Canvas.Pen.Color := RGB(255 - Round(255 * i / w), 255, 0);
    Canvas.MoveTo((i + w + tmrclk) mod RainbowWidth + RainbowLeft, RainbowTop);
    Canvas.LineTo((i + w + tmrclk) mod RainbowWidth + RainbowLeft, RainbowTop + RainbowHeight);
    Canvas.Pen.Color := RGB(0, 255, Round(255 * i / w));
    Canvas.MoveTo((i + w * 2 + tmrclk) mod RainbowWidth + RainbowLeft, RainbowTop);
    Canvas.LineTo((i + w * 2 + tmrclk) mod RainbowWidth + RainbowLeft, RainbowTop + RainbowHeight);
    Canvas.Pen.Color := RGB(0, 255 - Round(255 * i / w), 255);
    Canvas.MoveTo((i + w * 3 + tmrclk) mod RainbowWidth + RainbowLeft, RainbowTop);
    Canvas.LineTo((i + w * 3 + tmrclk) mod RainbowWidth + RainbowLeft, RainbowTop + RainbowHeight);
    Canvas.Pen.Color := RGB(Round(255 * i / w), 0, 255);
    Canvas.MoveTo((i + w * 4 + tmrclk) mod RainbowWidth + RainbowLeft, RainbowTop);
    Canvas.LineTo((i + w * 4 + tmrclk) mod RainbowWidth + RainbowLeft, RainbowTop + RainbowHeight);
    Canvas.Pen.Color := RGB(255 - Round(43 * i / w), Round(208 * i / w), 255 - Round(55 * i / w));
    Canvas.MoveTo((i + w * 5 + tmrclk) mod RainbowWidth + RainbowLeft, RainbowTop);
    Canvas.LineTo((i + w * 5 + tmrclk) mod RainbowWidth + RainbowLeft, RainbowTop + RainbowHeight);
    Canvas.Pen.Color := RGB(212 + Round(43 * i / w), 208 - Round(208 * i / w), 200 - Round(200 * i / w));
    Canvas.MoveTo((i + w * 6 + tmrclk) mod RainbowWidth + RainbowLeft, RainbowTop);
    Canvas.LineTo((i + w * 6 + tmrclk) mod RainbowWidth + RainbowLeft, RainbowTop + RainbowHeight);
  end;
  tmr.Enabled := True;
end;

procedure TfrmAbout.FormShow(Sender: TObject);
begin
  Font.Name := cfg.FontName;
  Font.Size := cfg.FontSize;
  tmr.Enabled := True;
  ShowStat;
end;

procedure TfrmAbout.FormClick(Sender: TObject);
begin
  Close;
end;

procedure TfrmAbout.lbHTTPClick(Sender: TObject);
begin
  ShellExecute(Handle, nil, 'http://avrubd.googlepages.com', nil, nil, SW_SHOW);
end;

procedure TfrmAbout.lbEMAILClick(Sender: TObject);
begin
  ShellExecute(Handle, nil, 'mailto:Shaoziyang@gmail.com?subject=about AVR BootLoader Downloader', nil, nil, SW_SHOW);
end;

procedure TfrmAbout.FormKeyPress(Sender: TObject; var Key: Char);
begin
  Close;
end;

procedure TfrmAbout.imgAVRUBDMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
var
  i, j: Integer;
  C: TColor;
begin
  if Shift = [ssLeft, ssShift] then
  begin
    Debug := not Debug;
    frmMain.btnDebug.Visible := Debug;
    for j := 0 to (imgAVRUBD.Height div 2) - 1 do
      for i := 0 to imgAVRUBD.Width - 1 do
      begin
        C := imgAVRUBD.Picture.Bitmap.Canvas.Pixels[i, j];
        imgAVRUBD.Picture.Bitmap.Canvas.Pixels[i, j] :=
          imgAVRUBD.Picture.Bitmap.Canvas.Pixels[i, imgAVRUBD.Height - 1 - j];
        imgAVRUBD.Picture.Bitmap.Canvas.Pixels[i, imgAVRUBD.Height - 1 - j] := C;
      end;
  end;
end;

procedure TfrmAbout.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  tmr.Enabled := False;
end;

procedure TfrmAbout.ShowStat;
begin
  lbAllSum.Caption := IntToStr(StaSum.sum);
  if StaSum.sum = 0 then
    lbAllSucc.Caption := '0'
  else
    lbAllSucc.Caption := Format('%0.2f%%  %9d', [(StaSum.sum - StaSum.fail) * 100 / StaSum.sum, StaSum.sum - StaSum.fail]);
  lbAllFail.Caption := IntToStr(StaSum.fail);
  lbSum.Caption := IntToStr(StaToday.sum);
  if StaToday.sum = 0 then
    lbSucc.Caption := '0'
  else
    lbSucc.Caption := Format('%0.2f%%  %9d', [(StaToday.sum - StaToday.fail) * 100 / StaToday.sum, StaToday.sum - StaToday.fail]);
  lbFail.Caption := IntToStr(StaToday.fail);
  lbRun.Caption := IntToStr(cfg.run);
end;

procedure TfrmAbout.btnClearStatClick(Sender: TObject);
begin
  if Tag <> 0 then
    Exit;
  try
    Tag := 1;
    if Application.MessageBox(PChar(lang.msgQueryClearStat), PChar(lang.msgWarning + '        '), MB_OKCANCEL + MB_ICONWARNING + MB_DEFBUTTON2) = IDOK then
    begin
      StaSum.sum := 0;
      StaSum.fail := 0;
      StaToday.sum := 0;
      StaToday.fail := 0;
      ShowStat;
    end;
  finally
    Tag := 0;
  end;
end;

end.

