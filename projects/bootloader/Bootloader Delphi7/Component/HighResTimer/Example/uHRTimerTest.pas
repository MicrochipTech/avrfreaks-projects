unit uHRTimerTest;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, HighResTimer, ComCtrls, ExtCtrls;

type
  TForm1 = class(TForm)
    Edit1: TEdit;
    Label1: TLabel;
    RunTest: TButton;
    Label2: TLabel;
    MeasureResult: TLabel;
    HighResTimer1: THighResTimer;
    StatusBar1: TStatusBar;
    rbUseTimeStampCounter: TRadioButton;
    rbUseHPCounter: TRadioButton;
    Label3: TLabel;
    lblCountFrequency: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure RunTestClick(Sender: TObject);
    procedure rbUseTimeStampCounterClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.DFM}

procedure TForm1.FormCreate(Sender: TObject);
begin
  MeasureResult.Caption := '';
  SetThreadPriority(GetCurrentThread, THREAD_PRIORITY_ABOVE_NORMAL);
  rbUseTimeStampCounter.Enabled := HighResTimer1.TSC;
  rbUseTimeStampCounter.Checked := HighResTimer1.TSC;
  rbUseHPCounter.Checked := not HighResTimer1.TSC;
end;

procedure TForm1.RunTestClick(Sender: TObject);
var
  WaitTime: Cardinal;
begin
  WaitTime := StrToInt(Edit1.Text);
  StatusBar1.SimpleText := 'Starting test...';
  Application.ProcessMessages;
  HighResTimer1.Enabled := True;
  HighResTimer1.StartTimeMeasure;
  HighResTimer1.Wait(WaitTime);
  HighResTimer1.StopTimeMeasure;
  HighResTimer1.Enabled := False;
  MeasureResult.Caption := FloatToStr(HighResTimer1.GetTimeDifference) + ' us';
  StatusBar1.SimpleText := StatusBar1.SimpleText + 'finished';
end;

procedure TForm1.rbUseTimeStampCounterClick(Sender: TObject);
begin
  HighResTimer1.UseTSC := rbUseTimeStampCounter.Checked;
  StatusBar1.SimpleText := 'Calibrating...';
  Application.ProcessMessages;
  HighResTimer1.Calibrate(2000);
  StatusBar1.SimpleText := StatusBar1.SimpleText + 'finished';
  lblCountFrequency.Caption := FloatToStr(HighResTimer1.CountSpeed) + ' MHz';
end;

end.
