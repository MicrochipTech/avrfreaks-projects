{
  Project:  AVR Universal Bootloader Download
  File:     uMain.pas
  Describe: Main interface, events, procedures, functions etc.
  Author:   Shaoziyang
  Date:     2008.4
  Version:  4.0
  Web:      http://shaoziyang.googlepages.com
        or  http://shaoziyang.bloger.com.cn (Chinese)
  Email:    shaoziyang@gmail.com
}

unit uMain;

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
  ImgList,
  Buttons,
  Menus,
  ComCtrls,
  ToolWin,
  Gauges,
  ExtCtrls,
  btOdeum,
  CPort,
  SynEdit,
  SynMemo,
  HexEdit,
  FastIniFile,
  HighResTimer,
  StdCtrls,
  XPMan,
  AppEvnts,
  ShellAPI,
  PC1,
  aes;

const
  WM_MyMsg = WM_USER + $1428;
  ATOMSTR = 'AVR Universal Bootloader Download.';

  MaxHisFileCount = 9;
  MAXLOGLINES = 512;

  XMODEM_NUL = 00;
  XMODEM_SOH = $01;
  XMODEM_STX = $02;
  XMODEM_EOT = $04;
  XMODEM_ACK = $06;
  XMODEM_NAK = $15;
  XMODEM_CAN = $18;
  XMODEM_EOF = $1A;
  XMODEM_RWC = $43; //'C'

  FlashSize: array[0..7] of Integer = (2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144);
  BusyChr: array[0..3] of Char = ('-', '\', '|', '/');

{$I tips.pas}

type
  CBuf = array[0..40960] of Byte;
  PBuf = ^CBuf;

  TfrmMain = class(TForm)
    Beep: TbtBeeper;
    ComPort: TComPort;
    HRTmr: THighResTimer;
    N2: TMenuItem;
    dlgOpen: TOpenDialog;
    dlgSave: TSaveDialog;
    ilMenu: TImageList;
    ilTool: TImageList;
    ilToolHot: TImageList;
    mmoBin: THexEdit;
    mmoHex: TSynMemo;
    mmoLog: TSynMemo;
    mnAbout: TMenuItem;
    mnAutoCode: TMenuItem;
    mnLangChinese: TMenuItem;
    mnComPortSetup: TMenuItem;
    mnContent: TMenuItem;
    mnDownLoad: TMenuItem;
    mnLangEnglish: TMenuItem;
    mnExit: TMenuItem;
    mnFile: TMenuItem;
    mnHelp: TMenuItem;
    mnLanguage: TMenuItem;
    mnLoad: TMenuItem;
    mnMCU: TMenuItem;
    mnMain: TMainMenu;
    mnOperate: TMenuItem;
    mnOption: TMenuItem;
    mnSave: TMenuItem;
    mnSystem: TMenuItem;
    pbMain: TGauge;
    pgctMain: TPageControl;
    pmHisFile: TPopupMenu;
    pmLang: TPopupMenu;
    pmLangChinese: TMenuItem;
    pmLangEnglish: TMenuItem;
    pnlMain: TPanel;
    tmrClock: TTimer;
    tmrSmartMouse: TTimer;
    tsBin: TTabSheet;
    tsHex: TTabSheet;
    tsLog: TTabSheet;
    Panel8: TPanel;
    stat: TStatusBar;
    pmImportHis: TPopupMenu;
    tsVer: TTabSheet;
    mmoVer: TSynMemo;
    Panel9: TPanel;
    Panel10: TPanel;
    Panel11: TPanel;
    Panel12: TPanel;
    XPManifest: TXPManifest;
    tbTool: TToolBar;
    Panel1: TPanel;
    btnLoad: TToolButton;
    btnSave: TToolButton;
    Panel3: TPanel;
    btnDownload: TToolButton;
    btnStop: TToolButton;
    Panel4: TPanel;
    btnOption: TToolButton;
    btnAutoCode: TToolButton;
    Panel7: TPanel;
    Panel6: TPanel;
    btnAbout: TToolButton;
    Panel5: TPanel;
    btnExit: TToolButton;
    tbMenu: TToolBar;
    Panel2: TPanel;
    btnMenuFile: TToolButton;
    btnMenuOperate: TToolButton;
    btnMenuOption: TToolButton;
    btnMenuHelp: TToolButton;
    btnDebug: TToolButton;
    Panel13: TPanel;
    btnLanguage: TSpeedButton;
    btnEncrypt: TToolButton;
    btnDecrypt: TToolButton;
    Panel14: TPanel;
    ApplicationEvents: TApplicationEvents;
    N3: TMenuItem;
    mnWeb: TMenuItem;
    mnGroup: TMenuItem;
    mnEmail: TMenuItem;
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure FormCreate(Sender: TObject);
    procedure FormKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);
    procedure FormResize(Sender: TObject);
    procedure btnAboutClick(Sender: TObject);
    procedure btnAutoCodeClick(Sender: TObject);
    procedure btnDebugClick(Sender: TObject);
    procedure btnDownloadClick(Sender: TObject);
    procedure btnExitClick(Sender: TObject);
    procedure btnLanguageClick(Sender: TObject);
    procedure btnLoadClick(Sender: TObject);
    procedure btnLoadHisClick(Sender: TObject);
    procedure btnOptionClick(Sender: TObject);
    procedure btnSaveClick(Sender: TObject);
    procedure btnStopClick(Sender: TObject);
    procedure mmoBinChange(Sender: TObject);
    procedure mnComPortSetupClick(Sender: TObject);
    procedure mnContentClick(Sender: TObject);
    procedure mnMCUClick(Sender: TObject);
    procedure mnSystemClick(Sender: TObject);
    procedure pmLangEnglishClick(Sender: TObject);
    procedure statMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure tbMenuMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure tmrClockTimer(Sender: TObject);
    procedure tmrSmartMouseTimer(Sender: TObject);
    procedure ComPortRxChar(Sender: TObject; Count: Integer);
    procedure dlgOpenShow(Sender: TObject);
    procedure dlgSaveShow(Sender: TObject);
    procedure mmoLogMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure pgctMainChange(Sender: TObject);
    procedure btnEncryptClick(Sender: TObject);
    procedure btnDecryptClick(Sender: TObject);
    procedure ApplicationEventsActivate(Sender: TObject);
    procedure mnWebClick(Sender: TObject);
    procedure mnEmailClick(Sender: TObject);
    procedure mnGroupClick(Sender: TObject);
  private
    { Private declarations }
    procedure HisFileClick(Sender: TObject);
  public
    { Public declarations }
    procedure clrSmart;
    procedure loadcfg;
    procedure savecfg;
    procedure loginfo(Msg: string);
    procedure OpenFile(FileName: string);
    procedure loadlanguage(langname: string);
    procedure delay(dt: Integer);
    procedure ImportHisClick(Sender: TObject);
    procedure UpdateHex;
    function UpdateBin(FileName: string): Boolean;
  end;

type
  DownLoadStatics = record
    sum: Cardinal;
    fail: Cardinal;
  end;

  TSystemConfig = record
    RTS, DTR: Boolean;
    SmartMouse: Boolean;
    Beep: Boolean;
    reload: Boolean;
    keyASC, keyHEX: string;
    CmdRST: string;
    keyMode: Byte;
    Protocol: Byte;
    BootCnt, SendTime: Integer;
    run: Integer;
    SendRST: Boolean;
    AutoSize: Boolean;
    FrameLen: Integer;
    FlashSize: Integer;
    fillunused: Boolean;
    filldata: string;
    Language: string;
    FontName: string;
    FontSize: Integer;
  end;

  TLangMsg = record
    msgError: string;
    msgWarning: string;
    msgOpenComportFail: string;
    msgSendRSTCmd: string;
    msgStartDownload: string;
    msgStopDownload: string;
    msgStartConnect: string;
    msgSendConnectKey: string;
    msgConnectSuccess: string;
    msgConnectFail: string;
    msgLines: string;
    msgChecksum: string;
    msgLength: string;
    msgTooManyRetry: string;
    msgLoadFileFail: string;
    msgUnsupport32BitAddress: string;
    msgLoadFileSuccess: string;
    msgFileLength: string;
    msgFlashSize: string;
    msgBufferOverflow: string;
    msgBufferSize: string;
    msgQueryClearStat: string;
    msgUpdateSuccess: string;
    msgUpdateFail: string;
    msgUpdateAbort: string;
    msgSpeed: string;
    msgTimeElapse: string;
    msgPackag: string;
    msgData: string;
    msgRetry: string;
    msgFmtErr: string;
    msgCrcErr: string;
    msgKey: string;
    msgAlgorithm: string;
    msgSetCryptOption: string;
  end;

function CRCSUM(buf: array of Byte; len: Integer; Mask, Init: Word): Word;
function MaskCrc16(buf: array of Byte; len: Integer; Mask, Init: Word): Word;
function CharToByte(c1, c2: Char): Byte;
function CharToDigit(c: Char): Byte;
function DateToChar(d: Byte): Char;
function EncryptInt(d: Integer): string;
function DecryptInt(ks: string): Integer;
function FormatHexStr(hs: string; var buf: array of Byte): string;
function TokenStr(var lns: string): string;
function FileModify(var sr: TSearchRec): Boolean;

var
  frmMain: TfrmMain;
  StaSum, StaToday: DownLoadStatics;
  cfg: TSystemConfig;
  lang: TLangMsg;
  RxBuf: CBuf;
  RxPtr, RxLen: Integer;
  DownFlag, DownCnt, ACK: Byte;
  EDBuf, DataBuf: array[0..65536 * 16] of Byte;
  DataSize, MaxDataSize: Integer;
  Debug: Boolean = False;
  TS, TB, TE: TDateTime;
  Hour, Min, Sec, MSec: Word;
  mp: TPoint;
  ini: TFastIniFile;
  msx, msy: Integer;
  Idle: Boolean = True;
  ClockCnt: ShortInt;
  HWD: THandle;
  sr: TSearchRec;

implementation

uses
  uOption,
  uAbout,
  uAutoCode;

{$R *.DFM}

//User define functions start

function FileModify(var sr: TSearchRec): Boolean;
var
  sr_new: TSearchRec;
begin
  Result := False;
  if sr.Name = '' then
    Exit;
  FindFirst(sr.Name, faAnyFile, sr_new);
  if (sr_new.Time <> sr.Time) or (sr_new.Size <> sr.Size) then
  begin
    Result := True;
    sr.Time := sr_new.Time;
    sr.Size := sr_new.Size;
  end
  else
    Result := False;
end;

function CharToByte(c1, c2: Char): Byte;
begin
  Result := CharToDigit(c1) * 16 + CharToDigit(c2);
end;

function CharToDigit(c: Char): Byte;
begin
  case c of
    '0'..'9': Result := Ord(c) - 48;
    'A'..'F': Result := Ord(c) - Ord('A') + 10;
    'a'..'f': Result := Ord(c) - Ord('a') + 10;
  else
    Result := 0;
  end;
end;

function DateToChar(d: Byte): Char;
begin
  case d of
    0..9: Result := Chr(Ord('0') + d);
    10..15: Result := Chr(Ord('A') + d - 10);
  else
    Result := '0';
  end;
end;

//checksum of accumulate

function CRCSUM(buf: array of Byte; len: Integer; Mask, Init: Word): Word;
var
  crc: Word;
  i: Integer;
begin
  crc := Init;
  for i := 0 to len - 1 do
    crc := crc + buf[i];
  Result := crc;
end;

//checksum of CRC16

function MaskCrc16(buf: array of Byte; len: Integer; Mask, Init: Word): Word;
var
  crc: Word;
  i, t: Integer;
begin
  crc := Init;
  for i := 0 to len - 1 do
  begin
    t := buf[i] * 256;
    crc := crc xor t;
    for t := 1 to 8 do
    begin
      if ((crc and $8000) = $8000) then
        crc := (crc * 2) xor Mask
      else
        crc := crc * 2;
    end;
  end;
  Result := crc;
end;

//Encrypt integer to str

function EncryptInt(d: Integer): string;
var
  i: Integer;
  cd: array[0..7] of Byte;
begin
  cd[1] := Random(256);
  cd[4] := Random(cd[1]);
  cd[6] := Random(cd[1] + cd[4]);
  cd[2] := (((d div $10000) mod $100) + cd[1]) xor cd[4];
  cd[3] := (((d div $100) mod $100) - cd[2]) xor (cd[1] - cd[4]);
  cd[5] := ((d mod $100) - cd[6]) xor (cd[1] + cd[3]);
  cd[7] := ((d div $1000000) mod $100) + cd[1] + cd[5];
  cd[0] := ((cd[1] xor cd[2]) xor cd[3]) + (cd[4] or cd[5]) + (cd[6] and cd[7]);
  Result := '';
  for i := 0 to 7 do
    Result := Result + IntToHex(cd[i], 2);
end;

function DecryptInt(ks: string): Integer;
var
  i: Integer;
  cd: array[0..7] of Byte;
  s: string;
begin
  for i := 0 to 7 do
  begin
    s := '$' + ks[i + i + 1] + ks[i + i + 2];
    cd[i] := StrToInt(s);
  end;

  if cd[0] <> Byte(((cd[1] xor cd[2]) xor cd[3]) + (cd[4] or cd[5]) + (cd[6] and cd[7])) then
  begin
    Result := 0;
    Exit;
  end;
  cd[7] := cd[7] - cd[1] - cd[5];
  cd[5] := (cd[5] xor (cd[1] + cd[3])) + cd[6];
  cd[3] := (cd[3] xor (cd[1] - cd[4])) + cd[2];
  cd[2] := (cd[2] xor cd[4]) - cd[1];
  Result := cd[7] * $1000000 + cd[2] * $10000 + cd[3] * $100 + cd[5];
end;

//Format a HEX string

function FormatHexStr(hs: string; var buf: array of Byte): string;
var
  i, len: Word;
begin
  Result := '';
  len := (Length(hs) + 2) div 3;
  for i := 1 to len do
  begin
    buf[i - 1] := CharToByte(hs[i * 3 - 2], hs[i * 3 - 1]);
  end;
  for i := 1 to len do
  begin
    Result := Result + Format('%.02X ', [buf[i - 1]]);
  end;
end;

//Token a word and delete it form a string

function TokenStr(var lns: string): string;
var
  i: Integer;
begin
  Result := '';
  if lns = '' then
    Exit;
  i := 1;
  while
    ((lns[i] = ' ') or
    (lns[i] = ',') or
    (lns[i] = '{') or
    (lns[i] = '}') or
    (lns[i] = '='))
    and (i < Length(lns))
    do
    i := i + 1;
  while
    (lns[i] <> ' ') and
    (lns[i] <> ',') and
    (lns[i] <> '=') and
    (lns[i] <> '{') and
    (lns[i] <> '}') and
    (i <= Length(lns))
    do
  begin
    Result := Result + lns[i];
    i := i + 1;
  end;
  Delete(lns, 1, i - 1);
end;

//User define functions end.

//System event

procedure TfrmMain.FormResize(Sender: TObject);
begin
  pgctMain.Left := -1;
  pgctMain.Top := -5;
  pgctMain.Width := pnlMain.Width + 3;
  pgctMain.Height := pnlMain.Height + 5;
  stat.Panels[0].Width := ClientWidth - 1 - 2 - 110 - 1;
  pbMain.Left := 1 + stat.Panels[0].Width + 2;
end;

procedure TfrmMain.FormCreate(Sender: TObject);
var
  sr: TSearchRec;
  s: string;
  i: Integer;
  mi: TMenuItem;
  K: Word;
begin
  Randomize;
  pnlMain.Align := alClient;
  HWD := Handle;

  Application.HintPause := 200;

  //embed Gauge to StatusBar
  pbMain.Parent := stat;
  pbMain.Top := 3;
  pbMain.Width := 100;
  pbMain.Height := stat.Height - 4;

  //initialization
  mmoLog.Clear;
  mmoHex.Clear;
  //mmoBin.Align := alClient;

  pbMain.Progress := 0;
  pbMain.ShowText := True;

  btnDownload.Enabled := False;
  mnDownLoad.Enabled := False;

  TS := Now;

  //Search language file
  i := 1;
  if FindFirst(ExtractFilePath(Application.ExeName) + '*.lng', faArchive, sr) = 0 then
  begin
    s := ChangeFileExt(ExtractFileName(sr.Name), '');
    if pmLang.Items.Find(s) = nil then
    begin
      mi := TMenuItem.Create(Self);
      mi.Caption := s;
      mi.RadioItem := True;
      mi.GroupIndex := 1;
      mi.AutoHotkeys := maManual;
      mi.AutoLineReduction := maManual;
      mi.Tag := 2003;
      mi.OnClick := pmLangEnglishClick;
      pmLang.Items.Add(mi);
      mi := TMenuItem.Create(Self);
      mi.Caption := s;
      mi.RadioItem := True;
      mi.GroupIndex := 1;
      mi.AutoHotkeys := maManual;
      mi.AutoLineReduction := maManual;
      mi.Tag := 2003;
      mi.OnClick := pmLangEnglishClick;
      mnLanguage.Add(mi);
    end;
    while FindNext(sr) = 0 do
    begin
      s := ChangeFileExt(ExtractFileName(sr.Name), '');
      if pmLang.Items.Find(s) = nil then
      begin
        mi := TMenuItem.Create(Self);
        mi.Caption := s;
        mi.RadioItem := True;
        mi.GroupIndex := 1;
        mi.AutoHotkeys := maManual;
        mi.AutoLineReduction := maManual;
        mi.Tag := 2003 + i;
        mi.OnClick := pmLangEnglishClick;
        pmLang.Items.Add(mi);
        mi := TMenuItem.Create(Self);
        mi.Caption := s;
        mi.RadioItem := True;
        mi.GroupIndex := 1;
        mi.AutoHotkeys := maManual;
        mi.AutoLineReduction := maManual;
        mi.Tag := 2003 + i;
        i := i + 1;
        mi.OnClick := pmLangEnglishClick;
        mnLanguage.Add(mi);
      end;
    end;
    FindClose(sr);
  end;

  sr.Size := -1;
  loadcfg;
  //showtips
  K := VK_F11;
  FormKeyDown(Sender, K, [ssAlt, ssShift]);
  pgctMain.ActivePageIndex := 0;
end;

procedure TfrmMain.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  savecfg;
end;

//Load last saved config data, or default value

procedure TfrmMain.loadcfg;
var
  s: string;
  i: Integer;
  hf: TMenuItem;
  buf: CBuf;
begin
  s := ExtractFilePath(Application.ExeName) + 'avrubd.ini';
  ini := TFastIniFile.Create(s);
  try
    try
      Left := ini.ReadInteger('Last', 'Left', 100);
      Top := ini.ReadInteger('Last', 'Top', 100);
      Width := ini.ReadInteger('Last', 'Width', 636);
      Height := ini.ReadInteger('Last', 'Height', 420);
      WindowState := wsNormal;
      //limit main window will not be outside desktop
      if Left < 0 then
        Left := 0;
      if Top < 0 then
        Top := 0;
      if Width > Screen.Width then
        Width := Screen.Width;
      if Height > Screen.Height then
        Height := Screen.Height;
      if Left > Screen.Width then
        Left := Screen.Width - Width;
      if Top > Screen.Height then
        Top := Screen.Height - Height;

      //Comport
      cfg.RTS := ini.ReadBool('ComPort', 'RTS', False);
      cfg.DTR := ini.ReadBool('ComPort', 'DTR', False);
      ComPort.Port := ini.ReadString('ComPort', 'Port', 'COM1');
      ComPort.BaudRate := TBaudRate(ini.ReadInteger('ComPort', 'BaudRate', Ord(br19200)));
      ComPort.DataBits := TDataBits(ini.ReadInteger('ComPort', 'DataBits', Ord(dbEight)));
      ComPort.StopBits := TStopBits(ini.ReadInteger('ComPort', 'StopBits', Ord(sbOneStopBit)));
      ComPort.Parity.Bits := TParityBits(ini.ReadInteger('ComPort', 'Parity', Ord(prNone)));
      ComPort.FlowControl.FlowControl := TFlowControl(ini.ReadInteger('ComPort', 'FlowControl', Ord(fcNone)));

      cfg.SmartMouse := ini.ReadBool('system', 'SmartMouse', False);
      tmrSmartMouse.Enabled := cfg.SmartMouse;
      cfg.Beep := ini.ReadBool('system', 'Beep', True);
      cfg.reload := ini.ReadBoolean('System', 'reload', False);
      cfg.keyMode := ini.ReadInteger('AVR', 'KEYMODE', 1);
      cfg.keyASC := ini.ReadString('AVR', 'KEYASC', 'd');
      cfg.keyHEX := FormatHexStr(ini.ReadString('AVR', 'KEYHEX', '64'), buf);
      cfg.Protocol := ini.ReadInteger('system', 'Protocol', 0);
      cfg.SendTime := ini.ReadInteger('system', 'SendTime', 200);
      cfg.BootCnt := ini.ReadInteger('system', 'BootCount', 10);
      cfg.SendRST := ini.ReadBool('AVR', 'SendRST', False);
      cfg.AutoSize := ini.ReadBool('AVR', 'AutoSize', True);
      cfg.CmdRST := ini.ReadString('AVR', 'CmdRST', '7E');
      cfg.FrameLen := ((ini.ReadInteger('system', 'Length', 128)) div 16) * 16;
      cfg.FlashSize := ini.ReadInteger('AVR', 'FlashSize', 2);
      cfg.fillunused := ini.ReadBool('AVR', 'Fill', True);
      cfg.filldata := FormatHexStr(ini.ReadString('AVR', 'filldata', 'FF CF'), buf);

      cfg.run := DecryptInt(ini.ReadString('last', 'run', '0000000000000000')) + 1;
      ini.WriteString('last', 'run', EncryptInt(cfg.run));
      StaSum.sum := DecryptInt(ini.ReadString('last', 'sum', '0000000000000000'));
      StaSum.fail := DecryptInt(ini.ReadString('last', 'fail', '0000000000000000'));
      StaToday.sum := 0;
      StaToday.fail := 0;

      for i := 1 to ini.ReadInteger('History', 'Count', 0) do
      begin
        s := ini.ReadString('History', 'File' + IntToStr(i - 1), '');
        if s <> '' then
        begin
          if FileExists(s) then
          begin
            hf := TMenuItem.Create(Self);
            hf.OnClick := HisFileClick;
            hf.Caption := s;
            hf.RadioItem := True;
            s := ExtractFileExt(s);
            if CompareText(s, '.hex') = 0 then
              hf.ImageIndex := 14
            else
              if CompareText(s, '.aub') = 0 then
                hf.ImageIndex := 17
              else
                hf.ImageIndex := 15;
            pmHisFile.Items.Add(hf);
          end;
        end;
        if i >= MaxHisFileCount then
          Break;
      end;

      for i := 1 to ini.ReadInteger('ImportHistory', 'Count', 0) do
      begin
        s := ini.ReadString('ImportHistory', 'File' + IntToStr(i - 1), '');
        if s <> '' then
        begin
          hf := TMenuItem.Create(Self);
          hf.OnClick := ImportHisClick;
          hf.Caption := s;
          hf.RadioItem := True;
          pmImportHis.Items.Add(hf);
        end;
      end;

      if cfg.run = 1 then
      begin
        //Get default codepage
        if (GetSystemMetrics(SM_DBCSENABLED) > 0) and (IsValidCodePage(936)) then
          cfg.Language := 'Chinese'
        else
          cfg.Language := 'English';
      end
      else
        cfg.Language := ini.ReadString('Last', 'language', 'English');

      if cfg.Language = 'Chinese' then
      begin
        cfg.FontName := ini.ReadString('Last', 'FontName', '宋体');
        cfg.FontSize := ini.ReadInteger('Last', 'FontSize', 9);
      end
      else
      begin
        cfg.FontName := ini.ReadString('Last', 'FontName', 'MS Sans Serif');
        cfg.FontSize := ini.ReadInteger('Last', 'FontSize', 8);
      end;
      //limit font size
      if cfg.FontSize < 8 then
        cfg.FontSize := 8;
      if cfg.FontSize > 24 then
        cfg.FontSize := 24;
      //Set main window's Font
      Font.Name := cfg.FontName;
      Font.Size := cfg.FontSize;
      mmoHex.Font := Font;
      mmoLog.Font := Font;
      mmoVer.Font := Font;

    except

    end;
  finally
    ini.Free;
  end;
end;

//save current config data

procedure TfrmMain.savecfg;
var
  s: string;
  i: Integer;
begin
  s := ExtractFilePath(Application.ExeName) + 'avrubd.ini';
  ini := TFastIniFile.Create(s);
  try
    try
      ini.WriteInteger('Last', 'Left', Left);
      ini.WriteInteger('Last', 'Top', Top);
      ini.WriteInteger('Last', 'Width', Width);
      ini.WriteInteger('Last', 'Height', Height);

      ini.WriteBool('ComPort', 'RTS', cfg.RTS);
      ini.WriteBool('ComPort', 'DTR', cfg.DTR);
      ini.WriteString('ComPort', 'Port', ComPort.Port);
      ini.WriteInteger('ComPort', 'BaudRate', Ord(ComPort.BaudRate));
      ini.WriteInteger('ComPort', 'DataBits', Ord(ComPort.DataBits));
      ini.WriteInteger('ComPort', 'StopBits', Ord(ComPort.StopBits));
      ini.WriteInteger('ComPort', 'Parity', Ord(ComPort.Parity.Bits));
      ini.WriteInteger('ComPort', 'FlowControl', Ord(ComPort.FlowControl.FlowControl));

      ini.WriteBool('system', 'SmartMouse', cfg.SmartMouse);
      ini.WriteBool('system', 'Beep', cfg.Beep);
      ini.WriteBoolean('System', 'reload', cfg.reload);
      ini.WriteInteger('AVR', 'KEYMODE', cfg.keyMode);
      ini.WriteString('AVR', 'KEYASC', cfg.keyASC);
      ini.WriteString('AVR', 'KEYHEX', cfg.keyHEX);
      ini.WriteInteger('system', 'Protocol', cfg.Protocol);
      ini.WriteInteger('system', 'SendTime', cfg.SendTime);
      ini.WriteInteger('system', 'BootCount', cfg.BootCnt);
      ini.WriteBool('AVR', 'SendRST', cfg.SendRST);
      ini.WriteBool('AVR', 'AutoSize', cfg.AutoSize);
      ini.WriteString('AVR', 'CmdRST', cfg.CmdRST);
      ini.WriteInteger('system', 'Length', cfg.FrameLen);
      ini.WriteInteger('AVR', 'FlashSize', cfg.FlashSize);
      ini.WriteBool('AVR', 'Fill', cfg.fillunused);
      ini.WriteString('AVR', 'filldata', cfg.filldata);

      ini.WriteString('last', 'sum', EncryptInt(StaSum.sum));
      ini.WriteString('last', 'fail', EncryptInt(StaSum.fail));

      ini.EraseSection('History');
      ini.WriteInteger('History', 'Count', pmHisFile.Items.Count);
      for i := 0 to pmHisFile.Items.Count - 1 do
        ini.WriteString('History', 'File' + IntToStr(i), pmHisFile.Items[i].Caption);

      ini.EraseSection('ImportHistory');
      ini.WriteInteger('ImportHistory', 'Count', pmImportHis.Items.Count);
      for i := 0 to pmImportHis.Items.Count - 1 do
        ini.WriteString('ImportHistory', 'File' + IntToStr(i), pmImportHis.Items[i].Caption);

      ini.WriteString('Last', 'language', cfg.Language);
      ini.WriteString('Last', 'FontName', cfg.FontName);
      ini.WriteInteger('Last', 'FontSize', cfg.FontSize);
    except

    end;
  finally
    ini.Free;
  end;
end;

//Load target file(HEX/BIN)

procedure TfrmMain.btnLoadClick(Sender: TObject);
begin
  //avoid reiteration event
  if Idle then
  begin
    Idle := False;
    try
      dlgOpen.Title := mnLoad.Caption + '           ';
      if dlgOpen.Execute then
      begin
        OpenFile(dlgOpen.FileName);
      end;
    finally
      HWD := frmMain.Handle;
      Idle := True;
    end;
  end;
end;

procedure TfrmMain.dlgOpenShow(Sender: TObject);
begin
  HWD := FindWindow(nil, PChar(dlgOpen.Title));
  clrSmart;
end;

//load history file

procedure TfrmMain.btnLoadHisClick(Sender: TObject);
begin
  //avoid reiteration event
  if Idle then
  begin
    Idle := False;
    try
      GetCursorPos(mp);
      pmHisFile.Popup(mp.X, mp.Y);
    finally
      Idle := True;
    end;
  end;
end;

//save buffer to file

procedure TfrmMain.btnSaveClick(Sender: TObject);
var
  i: Integer;
  aub: TFastIniFile;
  s: string;
begin
  //avoid reiteration event
  if Idle then
  begin
    Idle := False;
    try
      dlgSave.Title := mnSave.Caption + '           ';
      if dlgSave.Execute then
      begin
        case dlgSave.FilterIndex of
          1: //AUB
            begin
              dlgSave.FileName := ChangeFileExt(dlgSave.FileName, '.aub');
              aub := TFastIniFile.Create(dlgSave.FileName);
              try
                aub.WriteString('target', 'id', 'aub');

                aub.WriteInteger('cfg', 'protocol', cfg.Protocol);
                aub.WriteInteger('cfg', 'FrameLen', cfg.FrameLen);
                aub.WriteInteger('cfg', 'Timeout', cfg.SendTime);
                aub.WriteInteger('cfg', 'BootCnt', cfg.BootCnt);
                aub.WriteInteger('cfg', 'FlashSize', cfg.FlashSize);
                aub.WriteInteger('cfg', 'KeyMode', cfg.keyMode);
                aub.WriteString('cfg', 'KeyASC', cfg.keyASC);
                aub.WriteString('cfg', 'KeyHEX', cfg.keyHEX);
                aub.WriteString('cfg', 'FillData', cfg.filldata);
                aub.WriteBoolean('cfg', 'Fill', cfg.fillunused);
                aub.WriteBoolean('cfg', 'AutoSize', cfg.AutoSize);
                aub.WriteBoolean('cfg', 'SendRST', cfg.SendRST);
                aub.WriteString('cfg', 'CmdRST', cfg.CmdRST);
                aub.WriteBoolean('cfg', 'RTS', cfg.RTS);
                aub.WriteBoolean('cfg', 'DTR', cfg.DTR);
                aub.WriteString('cfg', 'BaudRate', BaudRateToStr(ComPort.BaudRate));
                aub.WriteString('cfg', 'DataBits', DataBitsToStr(ComPort.DataBits));
                aub.WriteString('cfg', 'StopBits', StopBitsToStr(ComPort.StopBits));
                aub.WriteString('cfg', 'Parity', ParityToStr(ComPort.Parity.Bits));
                aub.WriteString('cfg', 'FlowControl', FlowControlToStr(ComPort.FlowControl.FlowControl));

                mmoBin.SaveToBuffer(DataBuf, mmoBin.DataSize);
                s := '';
                for i := 0 to mmoBin.DataSize - 1 do
                  s := s + IntToHex(DataBuf[i], 2);
                aub.WriteString('buf', 'data', s);

              finally
                aub.Free;
              end;
            end;
          2: //HEX
            begin
              dlgSave.FileName := ChangeFileExt(dlgSave.FileName, '.hex');
              mmoHex.Lines.SaveToFile(dlgSave.FileName);
            end;
          3: //BIN
            begin
              dlgSave.FileName := ChangeFileExt(dlgSave.FileName, '.bin');
              mmoBin.SaveToFile(dlgSave.FileName);
            end;
        else
          //other
          mmoBin.SaveToFile(dlgSave.FileName);
        end;
        mmoBin.Modified := False;
        mmoHex.Modified := False;
      end;
    finally
      HWD := frmMain.Handle;
      Idle := True;
    end;
  end;
end;

procedure TfrmMain.dlgSaveShow(Sender: TObject);
begin
  HWD := FindWindow(nil, PChar(dlgSave.Title));
  clrSmart;
end;

//Download buffer's data to mcu

procedure TfrmMain.btnDownloadClick(Sender: TObject);
var
  buf: CBuf;
  crc, i, len, cnt: Word;
  PackNo: Byte;
  s: string;
  RtCnt, RtNo: Integer;
begin
  //avoid reiteration event
  if Idle then
  begin
    Idle := False;
    try
      //Disable all buttons and menus except Stop button
      for i := 0 to tbMenu.ButtonCount - 1 do
        tbMenu.Buttons[i].Enabled := False;
      for i := 0 to tbTool.ButtonCount - 1 do
        tbTool.Buttons[i].Enabled := False;

      loginfo('');
      btnStop.Enabled := True;
      pgctMain.Enabled := False;

      pgctMain.ActivePageIndex := 2;
      Application.ProcessMessages;
      try
        //open comport
        ComPort.Open;
        ComPort.SetDTR(cfg.DTR);
        ComPort.SetRTS(cfg.RTS);
      except
        //Alarm
        Application.ProcessMessages;
        if cfg.Beep then
          for i := 1 to 20 do
            Beep.BeepFor(i * 150, 15 + i);
        stat.Panels[0].Text := lang.msgOpenComportFail;
        loginfo('X ' + lang.msgOpenComportFail);
        Exit;
      end;

      loginfo('> ' + lang.msgStartDownload);

      HRTmr.Calibrate(100);
      pbMain.Progress := 0;
      pbMain.Visible := True;
      //send reset command
      if cfg.SendRST then
      begin
        DownFlag := 101;
        FormatHexStr(cfg.CmdRST, buf);
        len := (Length(cfg.CmdRST) + 2) div 3;
        ComPort.Write(buf, len);
        loginfo('$ ' + lang.msgSendRSTCmd + ': [' + cfg.CmdRST + ']');
        delay(1000);
      end;

      //start connect
      loginfo('> ' + lang.msgStartConnect);
      stat.Panels[0].Text := (lang.msgStartConnect);
      DownFlag := 1;
      DownCnt := 0;
      cnt := 0;
      while (DownFlag = 1) do
      begin
        if (cnt >= cfg.BootCnt) then
        begin
          if cfg.Beep then
            for i := 1 to 20 do
              Beep.BeepFor(i * 150, 15 + i);
          stat.Panels[0].Text := lang.msgConnectFail;
          loginfo('X ' + lang.msgConnectFail);
          Exit;
        end;

        cnt := cnt + 1;
        if cfg.keyMode = 0 then
        begin
          ComPort.WriteStr(cfg.keyASC);
          loginfo('$ <' + IntToStr(cnt) + '> ' + lang.msgSendConnectKey + ': ' + cfg.keyASC);
        end
        else
        begin
          FormatHexStr(cfg.keyHEX, buf);
          len := (Length(cfg.keyHEX) + 2) div 3;
          ComPort.Write(buf, len);
          loginfo('$ <' + IntToStr(cnt) + '> ' + lang.msgSendConnectKey + ': ' + cfg.keyHEX);
        end;
        for i := 1 to cfg.SendTime do
        begin
          delay(1);
          //press stop button
          if not btnStop.Enabled then
          begin
            loginfo('X ' + lang.msgUpdateAbort);
            loginfo('X ' + lang.msgUpdateFail);
            Exit;
          end;
        end;
      end;
      loginfo('V ' + lang.msgConnectSuccess);
      loginfo('');
      loginfo('');
      StaToday.sum := StaToday.sum + 1;
      StaSum.sum := StaSum.sum + 1;
      mmoBin.SaveToBuffer(DataBuf, mmoBin.DataSize);
      TB := Now;
      RtCnt := 0;
      RtNo := 0;
      PackNo := 1;
      cnt := 0;
      DownFlag := 3;
      repeat
        ACK := XMODEM_NUL;
        //Packag No
        buf[0] := XMODEM_SOH;
        buf[1] := PackNo;
        buf[2] := 255 xor PackNo;
        //Data
        for i := 0 to cfg.FrameLen - 1 do
          buf[3 + i] := DataBuf[cnt * cfg.FrameLen + i];
        //Checksum
        case cfg.Protocol of
          0: crc := MaskCrc16(PBuf(@buf[3])^, cfg.FrameLen, $1021, 0);
          1: crc := CRCSUM(PBuf(@buf[3])^, cfg.FrameLen, $1021, 0);
        else
          crc := 0;
        end;
        buf[cfg.FrameLen + 3] := crc div 256;
        buf[cfg.FrameLen + 4] := crc mod 256;
        ComPort.Write(buf, cfg.FrameLen + 5);
        s := Format(lang.msgPackag + ' [%u/%u], ' + lang.msgData + ' [%.1fk/%.1fk], ' + lang.msgRetry + ' [%u/%u]', [cnt + 1, mmoBin.DataSize div cfg.FrameLen, (cnt + 1) * cfg.FrameLen / 1024, mmoBin.DataSize / 1024, RtCnt, RtNo]);
        stat.Panels[0].Text := ' > ' + s;
        mmoLog.Lines.Strings[mmoLog.Lines.Count - 2] := '  ' + BusyChr[PackNo mod 4];
        pbMain.Progress := (cnt * 100) div (mmoBin.DataSize div cfg.FrameLen);

        PackNo := PackNo + 1;
        //wait respond
        for i := 1 to cfg.SendTime do
        begin
          delay(1);
          //press stop button
          if not btnStop.Enabled then
          begin
            StaSum.fail := StaSum.fail + 1;
            StaToday.fail := StaToday.fail + 1;
            mmoLog.Lines.Delete(mmoLog.Lines.Count - 2);
            loginfo('X ' + lang.msgUpdateAbort);
            loginfo('X ' + lang.msgUpdateFail);
            Exit;
          end;
          case ACK of
            XMODEM_NAK: //request resend
              begin
                RtCnt := RtCnt + 1;
                RtNo := RtNo + 1;
                loginfo(stat.Panels[0].Text);
                loginfo('');
                Break;
              end;
            XMODEM_ACK: //send success, next
              begin
                RtCnt := 0;
                cnt := cnt + 1;
                Break;
              end;
          end;
        end;
        //timeout
        if ACK = XMODEM_NUL then
        begin
          RtCnt := RtCnt + 1;
          RtNo := RtNo + 1;
        end;

        if (RtCnt > 3) or (RtNo > 10) then
        begin
          if cfg.Beep then
            for i := 1 to 20 do
              Beep.BeepFor(i * 150, 15 + i);
          mmoLog.Lines.Delete(mmoLog.Lines.Count - 2);
          loginfo('X ' + lang.msgTooManyRetry);
          loginfo('X ' + lang.msgUpdateFail);
          stat.Panels[0].Text := lang.msgTooManyRetry;
          StaSum.fail := StaSum.fail + 1;
          StaToday.fail := StaToday.fail + 1;
          Exit;
        end;

        //Send all data?
        if cnt >= (mmoBin.DataSize div cfg.FrameLen) then
        begin
          //send finish
          TE := Now;
          buf[0] := XMODEM_EOT;
          ComPort.Write(buf, 1);
          DecodeTime(TE - TB, Hour, Min, Sec, MSec);
          len := Hour * 3600 + Min * 60 + Sec;
          mmoLog.Lines.Delete(mmoLog.Lines.Count - 2);
          loginfo('> ' + s);
          s := Format(': [%u.%us]', [len, MSec div 100]);
          loginfo('V ' + lang.msgUpdateSuccess + ': ' + lang.msgTimeElapse + s + ', ' + lang.msgSpeed + Format(': %.1fk/s.', [mmoBin.DataSize / (1024 * (len + (MSec div 100) / 10))]));
          pbMain.Progress := 0;
          stat.Panels[0].Text := ' <> ' + lang.msgUpdateSuccess + s;
          if cfg.Beep then
            Beep.PlayPresetSound(psWelcome);
          Exit;
        end;
      until (False);

    finally
      //Enable all buttons and menus except Stop button
      for i := 0 to tbMenu.ButtonCount - 1 do
        tbMenu.Buttons[i].Enabled := True;
      for i := 0 to tbTool.ButtonCount - 1 do
        tbTool.Buttons[i].Enabled := True;
      btnStop.Enabled := False;
      pgctMain.Enabled := True;
      pbMain.Visible := False;
      Application.ProcessMessages;
      loginfo('');
      ComPort.Close;
      Idle := True;
    end;
  end;
end;

//Stop download

procedure TfrmMain.btnStopClick(Sender: TObject);
begin
  if btnStop.Enabled then
  begin
    btnStop.Enabled := False;
  end;
end;

procedure TfrmMain.btnOptionClick(Sender: TObject);
begin
  //avoid reiteration event
  if Idle then
  begin
    Idle := False;
    try
      HWD := frmOption.Handle;
      clrSmart;
      frmOption.ShowModal;
      HWD := frmMain.Handle;
    finally
      Idle := True;
    end;
  end;
end;

procedure TfrmMain.btnAutoCodeClick(Sender: TObject);
begin
  //avoid reiteration event
  if Idle then
  begin
    Idle := False;
    try
      HWD := frmAutoCode.Handle;
      clrSmart;
      frmAutoCode.ShowModal;
      HWD := frmMain.Handle;
    finally
      Idle := True;
    end;
  end;
end;

procedure TfrmMain.btnLanguageClick(Sender: TObject);
begin
  //avoid reiteration event
  if Idle then
  begin
    Idle := False;
    try
      GetCursorPos(mp);
      pmLang.Popup(mp.X, mp.Y);
    finally
      Idle := True;
    end;
  end;
end;

procedure TfrmMain.btnAboutClick(Sender: TObject);
begin
  //avoid reiteration event
  if Idle then
  begin
    Idle := False;
    try
      HWD := frmAbout.Handle;
      clrSmart;
      frmAbout.ShowModal;
      HWD := frmMain.Handle;
    finally
      Idle := True;
    end;
  end;
end;

//Quit software

procedure TfrmMain.btnExitClick(Sender: TObject);
begin
  //avoid reiteration event
  if Idle then
  begin
    Idle := False;
    try
      Close;
    finally
      Idle := True;
    end;
  end;
end;

procedure TfrmMain.FormKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
  case Key of
    Ord('C'):
      if Shift = [ssAlt] then
        mnComPortSetupClick(Sender);
    Ord('M'):
      if Shift = [ssAlt] then
        mnMCUClick(Sender);
    Ord('X'):
      if Shift = [ssAlt] then
        btnExitClick(Sender);
    Ord('O'):
      if Shift = [ssCtrl] then
        btnLoadClick(Sender);
    Ord('S'):
      begin
        if Shift = [ssCtrl] then
          btnSaveClick(Sender);
        if Shift = [ssAlt] then
          mnSystemClick(Sender);
      end;
    VK_F1:
      if Shift = [] then
        mnContentClick(Sender);
    VK_F9:
      if Shift = [] then
        btnDownloadClick(Sender);
    VK_F11:
      begin
        //Autocode
        if Shift = [] then
          btnAutoCodeClick(Sender);
        //toggle debug mode
        if Shift = [ssAlt] then
        begin
          frmAbout.imgAVRUBDMouseDown(Sender, mbLeft, [ssLeft, ssShift], 0, 0);
        end;
        if Shift = [ssAlt, ssShift] then
        begin
          if CompareText(cfg.Language, 'Chinese') = 0 then
          begin
            mmoHex.Text := #13#10 + tipcn[Random(MAXTIPCN)];
          end
          else
          begin
            mmoHex.Text := #13#10 + tipen[Random(MAXTIPEN)];
          end;
        end;
      end;
    VK_SCROLL: //reload default value
      begin
        if Shift = [ssAlt] then
        begin
          if DeleteFile(ExtractFilePath(Application.ExeName) + 'avrubd.ini') then
            loadcfg;
        end;
      end;
    Ord('A'):
      if Shift = [ssAlt] then
        btnAboutClick(Sender);
  end;
end;

procedure TfrmMain.mnMCUClick(Sender: TObject);
begin
  HWD := frmOption.Handle;
  frmOption.pgctOption.ActivePageIndex := 0;
  btnOptionClick(Sender);
  HWD := frmMain.Handle;
end;

procedure TfrmMain.mnSystemClick(Sender: TObject);
begin
  HWD := frmOption.Handle;
  frmOption.pgctOption.ActivePageIndex := 1;
  btnOptionClick(Sender);
  HWD := frmMain.Handle;
end;

procedure TfrmMain.mnComPortSetupClick(Sender: TObject);
begin
  HWD := frmOption.Handle;
  frmOption.pgctOption.ActivePageIndex := 2;
  btnOptionClick(Sender);
  HWD := frmMain.Handle;
end;

procedure TfrmMain.mnContentClick(Sender: TObject);
var
  hlp: string;
begin
  hlp := ExtractFilePath(Application.ExeName) + cfg.Language + '.hlp';
  if not FileExists(hlp) then
  begin
    hlp := ExtractFilePath(Application.ExeName) + 'English.hlp';
    if not FileExists(hlp) then
      Exit;
  end;
  Application.HelpFile := hlp;
  Application.HelpCommand(HELP_CONTENTS, 0);
end;

procedure TfrmMain.pmLangEnglishClick(Sender: TObject);
begin
  if Sender is TMenuItem then
  begin
    cfg.Language := TMenuItem(Sender).Caption;
    loadlanguage(cfg.Language);
  end;
end;

procedure TfrmMain.HisFileClick(Sender: TObject);
var
  FileName: string;
begin
  FileName := pmHisFile.Items[TMenuItem(Sender).MenuIndex].Caption;
  if FileExists(FileName) then
  begin
    OpenFile(FileName);
    dlgOpen.FileName := FileName;
  end;
end;

procedure TfrmMain.tmrSmartMouseTimer(Sender: TObject);
var
  mousekey: Byte;
  rect: TRect;
begin
  GetCursorPos(mp);
  if (Abs(msx - mp.X) > 2) or (Abs(msy - mp.Y) > 2) then
  begin
    msx := mp.X;
    msy := mp.Y;
    tmrSmartMouse.Tag := 990 div tmrSmartMouse.Interval;
    Exit;
  end;

  if tmrSmartMouse.Tag > 0 then
    tmrSmartMouse.Tag := tmrSmartMouse.Tag - 1;
  //get mouse status
  mousekey := 0;
  if (GetKeyState(VK_LBUTTON) and $80) = $80 then
    mousekey := mousekey or $01;
  if (GetKeyState(VK_RBUTTON) and $80) = $80 then
    mousekey := mousekey or $02;
  if (GetKeyState(VK_MBUTTON) and $80) = $80 then
    mousekey := mousekey or $04;
  //No mouse button pressed
  if mousekey <> 0 then
    tmrSmartMouse.Tag := 0;

  if tmrSmartMouse.Tag = 1 then
  begin
    //If active window is used form
    if GetForegroundWindow <> HWD then
      Exit;
    //Get form size
    GetWindowRect(HWD, rect);
    if (msx < rect.Left) or (msx > rect.Right) or (msy < rect.Top) or (msy > rect.Bottom) then
      Exit;

    mouse_event(MOUSEEVENTF_LEFTDOWN or MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
  end;
end;

procedure TfrmMain.tmrClockTimer(Sender: TObject);
begin
  tmrClock.Tag := tmrClock.Tag + 1;
  if tmrClock.Tag > 4 then
  begin
    tmrClock.Tag := 0;
    ClockCnt := ClockCnt + 1;
    if (not Debug) and (ClockCnt > 4) then
      ClockCnt := 0;
  end;

  case ClockCnt of
    1: //Date
      stat.Panels[1].Text := FormatDateTime('hh:nn:ss', Now - TS);
    2: //elapse time
      stat.Panels[1].Text := FormatDateTime('yyyy-mmm-dd', Now);
    3: //Down counter
      stat.Panels[1].Text := Format('%d/%d, %d/%d', [StaSum.sum - StaSum.fail, StaSum.sum, StaToday.sum - StaToday.fail, StaToday.sum]);
    4: //version
      stat.Panels[1].Text := 'AVRUBD v4';
    5: //run counter
      stat.Panels[1].Text := IntToStr(cfg.run);
    6: //comport configure
      stat.Panels[1].Text := ComPort.Port + ',' + BaudRateToStr(ComPort.BaudRate) + ',' + DataBitsToStr(ComPort.DataBits) + ',' + StopBitsToStr(ComPort.StopBits);
    7: //Font
      stat.Panels[1].Text := cfg.FontName + ',' + IntToStr(cfg.FontSize);
    8: //Language
      stat.Panels[1].Text := cfg.Language;
  else
    //Time
    stat.Panels[1].Text := FormatDateTime('hh:nn:ss', Now);
    ClockCnt := 0;
  end;
end;

//click panel[1] will change to next status

procedure TfrmMain.statMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  if X > (stat.Panels[0].Width + 1 + 2 + 1) then
  begin
    if Button = mbLeft then
      ClockCnt := ClockCnt + 1;
    if Button = mbRight then
    begin
      ClockCnt := ClockCnt - 1;
    end;
    if Debug then
    begin
      if ClockCnt > 8 then
        ClockCnt := 0;
      if ClockCnt < 0 then
        ClockCnt := 8;
    end
    else
    begin
      if ClockCnt > 4 then
        ClockCnt := 0;
      if ClockCnt < 0 then
        ClockCnt := 4;
    end;
    if Button = mbMiddle then
      tmrClock.Tag := -2147483647
    else
      tmrClock.Tag := -5;
    tmrClockTimer(Sender);
  end
  else
    tbMenuMouseDown(Sender, Button, Shift, X, Y);
end;

procedure TfrmMain.OpenFile(FileName: string);
var
  i, j: Integer;
  addr, len: Integer;
  s: string;
  hf: TMenuItem;
  buf: CBuf;
  stream: TMemoryStream;
  aub: TFastIniFile;
begin
  //avoid reload
  mmoBin.Tag := 12;

  sr.Name := FileName;
  FileModify(sr);

  //Disable download function
  btnDownload.Enabled := False;
  mnDownLoad.Enabled := False;
  btnEncrypt.Enabled := False;
  btnDecrypt.Enabled := False;
  DataSize := 0;

  //fill buffer with config value
  if cfg.fillunused then
  begin
    FormatHexStr(cfg.filldata, buf);
    len := (Length(cfg.filldata) + 2) div 3;
    for i := 0 to 1024 * 1024 - 1 do
    begin
      DataBuf[i] := buf[i mod len];
    end;
  end
  else
  begin
    for i := 0 to 1024 * 1024 - 1 do
      DataBuf[i] := $FF;
  end;

  mmoHex.Clear;
  mmoBin.DataSize := 0;
  stream := TMemoryStream.Create;
  try
    if CompareText('.aub', ExtractFileExt(FileName)) = 0 then
    begin
      aub := TFastIniFile.Create(FileName);
      try
        s := aub.ReadString('target', 'id', '');
        if s <> 'aub' then
        begin
          Application.MessageBox(PChar(lang.msgFmtErr + ': [ ' + FileName + ' ]'), PChar(lang.msgError), MB_OK + MB_ICONSTOP);
          Exit;
        end;
        s := aub.ReadString('buf', 'data', '');
        DataSize := Length(s) div 2;
        for i := 0 to DataSize - 1 do
        begin
          DataBuf[i] := CharToByte(s[i * 2 + 1], s[i * 2 + 2]);
        end;
        mmoBin.LoadFromBuffer(DataBuf, DataSize);

        cfg.Protocol := aub.ReadInteger('cfg', 'protocol', 0);
        cfg.FrameLen := aub.ReadInteger('cfg', 'FrameLen', 128);
        cfg.SendTime := aub.ReadInteger('cfg', 'Timeout', 500);
        cfg.BootCnt := aub.ReadInteger('cfg', 'BootCnt', 10);
        cfg.FlashSize := aub.ReadInteger('cfg', 'FlashSize', 2);
        cfg.keyMode := aub.ReadInteger('cfg', 'KeyMode', 1);
        cfg.keyASC := aub.ReadString('cfg', 'KeyASC', 'd');
        cfg.keyHEX := aub.ReadString('cfg', 'KeyHEX', '64');
        cfg.filldata := aub.ReadString('cfg', 'FillData', 'FF CF');
        cfg.fillunused := aub.ReadBoolean('cfg', 'Fill', True);
        cfg.AutoSize := aub.ReadBoolean('cfg', 'AutoSize', True);
        cfg.SendRST := aub.ReadBoolean('cfg', 'SendRST', False);
        cfg.CmdRST := aub.ReadString('cfg', 'CmdRST', '7E');
        cfg.RTS := aub.ReadBoolean('cfg', 'RTS', False);
        cfg.DTR := aub.ReadBoolean('cfg', 'DTR', False);
        ComPort.BaudRate := StrToBaudRate(aub.ReadString('cfg', 'BaudRate', '19200'));
        ComPort.DataBits := StrToDataBits(aub.ReadString('cfg', 'DataBits', '8'));
        ComPort.StopBits := StrToStopBits(aub.ReadString('cfg', 'StopBits', '1'));
        ComPort.Parity.Bits := StrToParity(aub.ReadString('cfg', 'Parity', 'None'));
        ComPort.FlowControl.FlowControl := StrToFlowControl(aub.ReadString('cfg', 'FlowControl', 'None'));

        pgctMain.ActivePage := tsBin;
      finally
        aub.Free;
      end;
    end
    else
    begin
      if CompareText('.hex', ExtractFileExt(FileName)) = 0 then
      begin
        //HEX
        mmoHex.Lines.LoadFromFile(FileName);
        if not UpdateBin(FileName) then
          Exit;
        pgctMain.ActivePage := tsHex;
      end
      else
      begin
        //BIN
        mmoBin.LoadFromFile(FileName);
        UpdateHex;
      end;
    end;
    loginfo('');
    s := Format('V %-25s [%s]', [lang.msgLoadFileSuccess + ':', FileName]);
    loginfo(s);
    s := Format('> %-25s 0x%.04X/%d', [lang.msgFileLength + ':', DataSize, DataSize]);
    loginfo(s);
    s := Format('> %-25s 0x%.04X/%d', [lang.msgFlashSize + ':', FlashSize[cfg.FlashSize], FlashSize[cfg.FlashSize]]);
    loginfo(s);

    btnDownload.Enabled := True;
    mnDownLoad.Enabled := True;
    btnEncrypt.Enabled := True;
    btnDecrypt.Enabled := True;

    if not (CompareText('.aub', ExtractFileExt(FileName)) = 0) then
    begin
      //make buffer size is multiple frame size
      if (DataSize mod cfg.FrameLen) <> 0 then
      begin
        DataSize := ((DataSize + cfg.FrameLen - 1) div cfg.FrameLen) * cfg.FrameLen;
      end;

      //even frame
      if ((DataSize div cfg.FrameLen) mod 2) = 1 then
      begin
        DataSize := DataSize + cfg.FrameLen;
      end;

    end;
    s := Format('> %-25s 0x%.04X/%d', [lang.msgBufferSize + ':', mmoBin.DataSize, mmoBin.DataSize]);
    loginfo(s);

    //change Caption
    Caption := 'AVRUBD - [' + FileName + ']';

    for i := 0 to pmHisFile.Items.Count - 1 do
      pmHisFile.Items[i].Checked := False;
    if pmHisFile.Items.Find(FileName) <> nil then
    begin
      pmHisFile.Items.Find(FileName).Free;
    end;

    //History file list
    if pmHisFile.Items.Count >= MaxHisFileCount then
      pmHisFile.Items.Delete(pmHisFile.Items.Count - 1);
    hf := TMenuItem.Create(Self);
    hf.OnClick := HisFileClick;
    hf.Caption := FileName;
    hf.RadioItem := True;
    hf.Checked := True;
    s := ExtractFileExt(FileName);
    if CompareText(s, '.hex') = 0 then
      hf.ImageIndex := 14
    else
      if CompareText(s, '.aub') = 0 then
        hf.ImageIndex := 17
      else
        hf.ImageIndex := 15;
    pmHisFile.Items.Insert(0, hf);
  finally
    stream.Free;
    mmoBin.Tag := 0;
    pgctMain.OnChange(nil);
  end;
end;

procedure TfrmMain.mmoBinChange(Sender: TObject);
var
  stream: TMemoryStream;
  i, j: Integer;
  s: string;
  jy: Byte;
begin
  if mmoBin.Tag <> 0 then
    Exit;
  mmoBin.SaveToBuffer(DataBuf, mmoBin.DataSize);
  stream := TMemoryStream.Create;
  try
    stream.Clear;
    for i := 0 to ((mmoBin.DataSize + 15) div 16) - 1 do
    begin
      if (i > 0) and (((i * 16) mod 65536) = 0) then
      begin
        //segment address
        s := ':02000002100000  ';
        j := (i * 16 * 16) div 65536;
        s[10] := DateToChar(j div 16);
        s[11] := DateToChar(j mod 16);
        jy := 4 + CharToByte(s[10], s[11]);
        jy := -jy;
        s[14] := DateToChar(jy div 16);
        s[15] := DateToChar(jy mod 16);
        s[16] := #13;
        s[17] := #10;
        stream.WriteBuffer(s[1], 17);
      end;
      SetLength(s, 45);
      s[1] := ':';
      s[2] := '1';
      s[3] := '0';
      jy := (i * 16) div 256;
      s[4] := DateToChar(jy div 16);
      s[5] := DateToChar(jy mod 16);
      jy := (i * 16) mod 256;
      s[6] := DateToChar(jy div 16);
      s[7] := DateToChar(jy mod 16);
      s[8] := '0';
      s[9] := '0';
      jy := $10 + ((i * 16) div 256) + ((i * 16) mod 256);
      //fill data
      for j := 0 to 15 do
      begin
        jy := jy + DataBuf[i * 16 + j];
        s[j * 2 + 10] := DateToChar(DataBuf[i * 16 + j] div 16);
        s[j * 2 + 11] := DateToChar(DataBuf[i * 16 + j] mod 16);
      end;
      //校验
      jy := -jy;
      s[42] := DateToChar(jy div 16);
      s[43] := DateToChar(jy mod 16);
      s[44] := #13;
      s[45] := #10;
      stream.WriteBuffer(s[1], 45);
    end;
    stream.Position := 0;
    mmoHex.Lines.BeginUpdate;
    mmoHex.Clear;
    mmoHex.Lines.LoadFromStream(stream);
    mmoHex.Lines[mmoHex.Lines.Count - 1] := ':00000001FF';
    mmoHex.Lines.EndUpdate;
  finally
    stream.Free;
  end;
end;

//Log message

procedure TfrmMain.loginfo(Msg: string);
begin
  mmoLog.Lines.Append(Msg);
  //keep maxlines
  if mmoLog.Lines.Count > MAXLOGLINES then
  begin
    mmoLog.Lines.BeginUpdate;
    mmoLog.Lines.Delete(0);
    mmoLog.Lines.EndUpdate;
  end;
  mmoLog.TopLine := mmoLog.Lines.Count - mmoLog.LinesInWindow + 1;
end;

//Load language file

procedure TfrmMain.loadlanguage(langname: string);
var
  lng: TFastIniFile;
  i: Integer;
begin
  for i := 0 to pmLang.Items.Count - 1 do
    if CompareText(langname, pmLang.Items[i].Caption) = 0 then
    begin
      pmLang.Items[i].Checked := True;
      mnLanguage.Items[i].Checked := True;
    end;

  if CompareText(langname, 'Chinese') = 0 then
  begin
    pmLangChinese.ImageIndex := -1;
    mnLangChinese.ImageIndex := -1;
  end
  else
  begin
    pmLangChinese.ImageIndex := 12;
    mnLangChinese.ImageIndex := 12;
  end;

  if CompareText(langname, 'English') = 0 then
  begin
    pmLangEnglish.ImageIndex := -1;
    mnLangEnglish.ImageIndex := -1;
  end
  else
  begin
    pmLangEnglish.ImageIndex := 13;
    mnLangEnglish.ImageIndex := 13;
  end;

  if CompareText(langname, 'CHINESE') = 0 then
  begin
    //Main
    //menu
    mnFile.Caption := '文件';
    mnLoad.Caption := '载入';
    mnSave.Caption := '保存';
    mnExit.Caption := '退出';
    mnOperate.Caption := '操作';
    mnDownLoad.Caption := '下载';
    mnAutoCode.Caption := '自动生成配置代码';
    mnOption.Caption := '选项';
    mnSystem.Caption := '系统';
    mnComPortSetup.Caption := '串口';
    mnHelp.Caption := '帮助';
    mnAbout.Caption := '关于';
    mnContent.Caption := '使用方法';
    mnWeb.Caption := '访问主页';
    mnEmail.Caption := '发封邮件';
    mnGroup.Caption := '讨论组';
    btnMenuFile.Caption := mnFile.Caption;
    btnMenuOperate.Caption := mnOperate.Caption;
    btnMenuOption.Caption := mnOption.Caption;
    btnMenuHelp.Caption := mnHelp.Caption;
    //msg
    lang.msgError := '错误';
    lang.msgWarning := '警告';
    lang.msgOpenComportFail := '打开串口失败.';
    lang.msgSendRSTCmd := '发送复位命令';
    lang.msgStartDownload := '开始下载';
    lang.msgStopDownload := '停止下载';
    lang.msgStartConnect := '开始联机';
    lang.msgSendConnectKey := '发送联机密码';
    lang.msgConnectSuccess := '联机成功';
    lang.msgConnectFail := '联机失败';
    lang.msgLines := '行';
    lang.msgChecksum := '校验';
    lang.msgLength := '长度';
    lang.msgTooManyRetry := '重试次数太多！';
    lang.msgLoadFileFail := '载入文件失败.';
    lang.msgUnsupport32BitAddress := '不支持32位地址模式.';
    lang.msgLoadFileSuccess := '载入文件成功';
    lang.msgFileLength := '文件长度';
    lang.msgFlashSize := 'Flash空间大小';
    lang.msgBufferOverflow := '缓冲区溢出';
    lang.msgBufferSize := '缓冲区大小';
    lang.msgQueryClearStat := '你确定要清除统计结果吗?';
    lang.msgUpdateSuccess := '升级成功';
    lang.msgUpdateFail := '升级失败';
    lang.msgUpdateAbort := '升级中止';
    lang.msgSpeed := '速度';
    lang.msgTimeElapse := '用时';
    lang.msgPackag := '包';
    lang.msgData := '数据';
    lang.msgRetry := '重试';
    lang.msgFmtErr := '文件格式错误';
    lang.msgCrcErr := '数据校验错误';
    lang.msgKey := '密钥';
    lang.msgAlgorithm := '算法';
    lang.msgSetCryptOption := '你可以在自动代码功能中设置加密参数';
    //About
    with frmAbout do
    begin
      Caption := '关于';
      lbTranslator.Caption := '语言: 简体中文';
      lbTotalRun.Caption := '运行次数';
      lbTotalDown.Caption := '总共下载';
      lbTotalSucc.Caption := '成功次数';
      lbTotalFail.Caption := '失败次数';
      lbCurDown.Caption := '本次下载';
      lbCurSucc.Caption := '本次成功';
      lbCurFail.Caption := '本次失败';
      btnOk.Caption := '确定';
      btnClearStat.Caption := '清除';
    end;
    //option
    with frmOption do
    begin
      Caption := '选项';
      tsSystem.Caption := '系统';
      tsComport.Caption := '串口';
      btnFont.Caption := '字体';
      btnOk.Caption := '确定';
      btnCancel.Caption := '取消';
      btnHypertrm.Caption := '超级终端';
      btnImport.Caption := '导入';
      gpbConnectKey.Caption := '联机密码';
      lbFlashSize.Caption := 'Flash空间大小';
      lbFlashSizeBytes.Caption := '字节';
      lbCommProt.Caption := '通信协议';
      lbFrameLen.Caption := '数据帧长';
      lbBasicTime.Caption := '基本时间周期';
      lbBasicTimeMs.Caption := '毫秒';
      lbMaxConCnt.Caption := '最大联机次数';
      lbFrameLenBytes.Caption := lbFlashSizeBytes.Caption;
      lbComPort.Caption := '串口';
      lbBaudRate.Caption := '波特率';
      lbDataBits.Caption := '数据位';
      lbStopBits.Caption := '停止位';
      lbParity.Caption := '校验位';
      lbFlowCtrl.Caption := '流量控制';
      chkBeepEn.Caption := '扬声器声音提示';
      chkSmartMouse.Caption := '智能鼠标模式';
      chkReloadFile.Caption := '自动载入文件';
      lbFillUnused.Caption := '填充未用空间';
      lbSendRST.Caption := '发送复位命令';
      lbAutoSize.Caption := '自动判断下载数据长度';
      cbbProtocol.Items.Strings[0] := '标准XModem协议';
      cbbProtocol.Items.Strings[1] := '简化XModem协议';
    end;
    //AutoCode
    with frmAutoCode do
    begin
      Caption := '自动代码';
      btnHypertrm.Caption := '超级终端';
      btnCancel.Caption := '取消';
      btnSmallest.Caption := '最小代码';
      btnRecover.Caption := '恢复';
      btnRecommand.Caption := '推荐';
      btnMake.Caption := '创建';
      lbRS485PORT.Caption := '端口';
      lbRS485PIN.Caption := '引脚';
      lbLEDPORT.Caption := '端口';
      lbLEDPIN.Caption := '引脚';
      lbBOOTPORT.Caption := '端口';
      lbBOOTPIN.Caption := '引脚';
      lbBOOTLEVEL.Caption := '电平';
      cbbCRC.Items := frmOption.cbbProtocol.Items;
      cbbBootLevel.Items.Strings[0] := '低';
      cbbBootLevel.Items.Strings[1] := '高';
      cbbBootMode.Items.Strings[0] := '串口';
      cbbBootMode.Items.Strings[1] := '电平';
    end;
    //Hint
    btnMenuFile.Hint := '文件操作';
    btnMenuOperate.Hint := '用户操作';
    btnMenuOption.Hint := '系统参数设置';
    btnMenuHelp.Hint := '帮助信息';
    mnLoad.Hint := '载入目标文件到缓冲区';
    mnSave.Hint := '保存缓冲区数据到文件';
    mnExit.Hint := '退出软件';
    mnDownLoad.Hint := '下载数据到单片机';
    mnAutoCode.Hint := '自动产生用户配置文件';
    mnMCU.Hint := '设置单片机相关参数';
    mnSystem.Hint := '设置系统参数';
    mnComPortSetup.Hint := '设置串口参数';
    mnLanguage.Hint := '选择用户界面语言';
    mnAbout.Hint := '显示软件相关信息';
    mnHelp.Hint := '显示帮助文件';
    btnLoad.Hint := mnLoad.Hint;
    btnSave.Hint := mnSave.Hint;
    btnDownload.Hint := mnDownLoad.Hint;
    btnStop.Hint := '中止下载';
    btnOption.Hint := '设置软件参数';
    btnAutoCode.Hint := mnAutoCode.Hint;
    btnLanguage.Hint := mnLanguage.Hint;
    btnAbout.Hint := mnAbout.Hint;
    btnExit.Hint := mnExit.Hint;
    btnEncrypt.Hint := '加密';
    btnDecrypt.Hint := '解密';

    frmAbout.btnClearStat.Hint := '清除统计信息';
    frmOption.btnImport.Hint := '导入用户配置文件bootcfg.h';
    frmOption.btnHypertrm.Hint := '设置适合使用超级终端的参数';
    frmOption.btnCancel.Hint := '取消参数设置';
    frmOption.btnOk.Hint := '保存参数设置';
    frmOption.btnFont.Hint := '设置用户界面字体';
    frmOption.gpbConnectKey.Hint := '设置单片机的联机密码参数';
    frmOption.cbbFlash.Hint := '选择单片机的Flash容量';
    frmOption.chkFillUnused.Hint := '填充Flash中没有使用的空间，可以提高抗干扰能力';
    frmOption.lbFillUnused.Hint := frmOption.chkFillUnused.Hint;
    frmOption.edtFillUnused.Hint := '填充的数据以字(双字节)为单位';
    frmOption.chkSendRST.Hint := '发送复位命令，可以不用手工复位单片机。这个功能需要单片机程序支持';
    frmOption.lbSendRST.Hint := frmOption.chkSendRST.Hint;
    frmOption.edtSendRST.Hint := '复位命令';
    frmOption.chkAutoSize.Hint := '自动计算下载数据长度，可以减少下载时间';
    frmOption.lbAutoSize.Hint := frmOption.chkAutoSize.Hint;
    frmOption.cbbProtocol.Hint := '通信使用的协议，必需和bootcfg.h中定义的一致';
    frmOption.cbbFrameLen.Hint := '通信时数据包中数据的长度(不包括控制数据和校验数据)';
    frmOption.cbbSendTime.Hint := '发送数据间隔时间，也是数据超时时间';
    frmOption.cbbBootCnt.Hint := '发送联机命令最大次数';
    frmOption.chkBeepEn.Hint := '允许使用PC机扬声器进行状态提示';
    frmOption.chkSmartMouse.Hint := '允许智能鼠标模式，可以减少手指的疲劳程度';
    frmOption.chkReloadFile.Hint := '在文件修改后自动载入文件';
    frmAutoCode.btnRecover.Hint := '恢复参数成默认值';
    frmAutoCode.btnRecommand.Hint := '正式产品中推荐使用的参数';
    frmAutoCode.btnHypertrm.Hint := '适合于使用超级终端时使用的参数';
    frmAutoCode.btnSmallest.Hint := '产生最小的Bootloader代码时的参数设置';
    frmAutoCode.btnCancel.Hint := '退出自动代码功能';
    frmAutoCode.btnMake.Hint := '根据用户选择的参数产生配置文件';
    frmAutoCode.chkRS485En.Hint := '使用RS485/RS422模式';
    frmAutoCode.lbRS485En.Hint := frmAutoCode.chkRS485En.Hint;
    frmAutoCode.chkLED.Hint := '使用LED显示Bootloader的状态';
    frmAutoCode.lbLEDEn.Hint := frmAutoCode.chkLED.Hint;
    frmAutoCode.cbbMCU.Hint := '选择单片机型号';
    frmAutoCode.cbbFCPU.Hint := '单片机系统的时钟频率';
    frmAutoCode.cbbBaudRate.Hint := '选择串口通信使用的波特率';
    frmAutoCode.cbbComPortNo.Hint := '选择单片机使用的串口号';
    frmAutoCode.cbbBufferSize.Hint := '单片机串口缓冲区长度，与通信协议中数据帧长度对应';
    frmAutoCode.cbbTimeClk.Hint := '一次联机命令的超时时间，也是基本时间段';
    frmAutoCode.cbbBootCnt.Hint := '最大联机次数';
    frmAutoCode.cbbFileCnt.Hint := '等待上位机发送数据的超时时间，也是单片机发送数据C的次数';
    frmAutoCode.cbbInitDelay.Hint := '串口初始化后的延时周期数，部分早期单片机需要此参数';
    frmAutoCode.cbbBootMode.Hint := 'Bootloader触发模式，默认是串口。选择引脚电平是可以获得更小的代码';
    frmAutoCode.cbbBootStart.Hint := '单片机Boot区的起始地址, 设置为0将禁止此功能';
    frmAutoCode.cbbCRC.Hint := '通信数据的校验模式';
    frmAutoCode.cbbAlgorithm.Hint := '选择加密解密算法';
    frmAutoCode.cbbFlagAddr.Hint := '标志位在EEPROM中位置';
    frmAutoCode.chkChipCheck.Hint := '校验下载的数据，用于提高可靠性。强烈推荐使用这个选项(需要和BootStart一起使用才能有效)';
    frmAutoCode.chkWDG.Hint := '使用看门狗，可以提高系统的可靠性';
    frmAutoCode.chkVerbose.Hint := '显示更多的提示信息给用户，使用超级终端时需要选择此参数';
    frmAutoCode.chkDecrypt.Hint := '使用解密功能，同时需要选择加密算法和密码';
    frmAutoCode.chkSafeMode.Hint := '安全升级模式，只有完成升级后才能退出Bootloader';

    //Version
    mmoVer.Lines.Text :=
      'v4.5     2008.Octo' + #13#10 +
      '       + 安全升级模式' + #13#10 +
      '' + #13#10 +
      'v4.2     2008.Jun' + #13#10 +
      '       + 增加了AES加密/解密功能' + #13#10 +
      '       + 增加产生随机密钥功能' + #13#10 +
      '' + #13#10 +
      'v4.1     2008.May' + #13#10 +
      '       - 修正了下载缓冲区长度的错误' + #13#10 +
      '       - 修正了导入bootcfg.h参数的两处错误' + #13#10 +
      '' + #13#10 +
      'v4.0     2008.Apr' + #13#10 +
      '       + 增加PC1加密/解密功能' + #13#10 +
      '       + 自动重新载入文件功能' + #13#10 +
      '       - 修正一些错误' + #13#10 +
      '' + #13#10 +
      'v3.5     2008.Mar' + #13#10 +
      '       + 在自动代码功能创建配置文件后自动创建 avrub.bat' + #13#10 +
      '       + 增加了 byte/word 参数选择' + #13#10 +
      '' + #13#10 +
      'v3.1     2007.Octo' + #13#10 +
      '       - 修正了aub文件中一个缓冲区错误(感谢Stojan Obid)' + #13#10 +
      '       + 支持XP样式外观' + #13#10 +
      '       ^ 细节调整' + #13#10 +
      '' + #13#10 +
      'v3.0     2007.Jun.9' + #13#10 +
      '       - 应用户要求取消了皮肤，重新设计了界面' + #13#10 +
      '       + 增加导入配置功能，直接读取用户bootcfg.h的设置' + #13#10 +
      '       ^ 增强了智能鼠标模式' + #13#10 +
      '       ^ 改进了自动代码功能，增加了参数范围检查和波特率误差计算' + #13#10 +
      '' + #13#10 +
      'v2.0     2007.Apr.22' + #13#10 +
      '       + 使用AlphaControl5 Lite控件做皮肤，重新设计了界面，' + #13#10 +
      '       + 增加了智能鼠标模式' + #13#10 +
      '       + 支持多语言' + #13#10 +
      '' + #13#10 +
      'v1.2     2007.Apr.10' + #13#10 +
      '       ^ 修改了一些操作方式' + #13#10 +
      '       ^ 修正了几处小错误' + #13#10 +
      '' + #13#10 +
      'v1.1.0   2007.Apr.02' + #13#10 +
      '       + 支持大于64K的HEX文件' + #13#10 +
      '       + 自动生成配置代码文件Bootcfg.h' + #13#10 +
      '' + #13#10 +
      'v1.0.0   2007.Mar.22' + #13#10 +
      '       + 第一版，实现全部基本功能，并扩展了XModem协议(可变缓冲区长度和多种校验方式)以适合' + #13#10 +
      '         不同的应用' + #13#10 +
      '' + #13#10 +
      '特别感谢:' + #13#10 +
      '' + #13#10 +
      '  刘海涛, 徐延康, tda1552, 倪恩伟, 程翔, Stojan Obid, Luiz Francisco, seu gab 等' + #13#10 +
      ''
      ;
    Exit;
  end;

  //Hint
  btnMenuFile.Hint := 'File opreate';
  btnMenuOperate.Hint := 'user operate';
  btnMenuOption.Hint := 'set software''s parameters';
  btnMenuHelp.Hint := 'help information';
  mnLoad.Hint := 'load user''s file to buffer';
  mnSave.Hint := 'save buffer'' data to file';
  mnExit.Hint := 'quit AVRUBD';
  mnDownLoad.Hint := 'Download buffer''s data to AVR device';
  mnAutoCode.Hint := 'automatic make user configure file: bootcfg.h';
  mnMCU.Hint := 'set AVR device parameters';
  mnSystem.Hint := 'set system parameters';
  mnComPortSetup.Hint := 'set comport parameters';
  mnLanguage.Hint := 'choose language you will use';
  mnAbout.Hint := 'show software information';
  mnHelp.Hint := 'show help file';
  btnLoad.Hint := mnLoad.Hint;
  btnSave.Hint := mnSave.Hint;
  btnDownload.Hint := mnDownLoad.Hint;
  btnStop.Hint := 'abort download operation';
  btnOption.Hint := 'set AVRUBD''s parameters';
  btnAutoCode.Hint := mnAutoCode.Hint;
  btnLanguage.Hint := mnLanguage.Hint;
  btnAbout.Hint := mnAbout.Hint;
  btnExit.Hint := mnExit.Hint;
  btnEncrypt.Hint := 'Encrypt';
  btnDecrypt.Hint := 'Decrypt';

  frmAbout.btnClearStat.Hint := 'Delete download statistic data';
  frmOption.btnImport.Hint := 'import bootcfg.h''s configure';
  frmOption.btnHypertrm.Hint := 'set parameters fit Hypertrm';
  frmOption.btnCancel.Hint := 'return without save configure';
  frmOption.btnOk.Hint := 'save user configure';
  frmOption.btnFont.Hint := 'select font for user interface';
  frmOption.gpbConnectKey.Hint := 'set connect key';
  frmOption.cbbFlash.Hint := 'select microcontroller''s Flash size';
  frmOption.chkFillUnused.Hint := 'fill unused Flash space, will enhance anti-jamming ability';
  frmOption.lbFillUnused.Hint := frmOption.chkFillUnused.Hint;
  frmOption.edtFillUnused.Hint := 'filled data base on word(two byte)';
  frmOption.chkSendRST.Hint := 'send reset command, so you do not need reset AVR manual';
  frmOption.lbSendRST.Hint := frmOption.chkSendRST.Hint;
  frmOption.edtSendRST.Hint := 'reset command strings';
  frmOption.chkAutoSize.Hint := 'auto calculate download size, will reduce download time';
  frmOption.lbAutoSize.Hint := frmOption.chkAutoSize.Hint;
  frmOption.cbbProtocol.Hint := 'communication''s protocol, must same as define in bootcfg.h';
  frmOption.cbbFrameLen.Hint := 'communication package''s data length, not include control bytes and checksum bytes';
  frmOption.cbbSendTime.Hint := 'interval between two connection';
  frmOption.cbbBootCnt.Hint := 'maximum send connect command count';
  frmOption.chkBeepEn.Hint := 'use pc speaker to prompt current state';
  frmOption.chkSmartMouse.Hint := 'use smartmouse mode will reduce you finger click count';
  frmOption.chkReloadFile.Hint := 'Auto reload file if file have been modified';
  frmAutoCode.btnRecover.Hint := 'recover parameters to defaul';
  frmAutoCode.btnRecommand.Hint := 'recommand parameters in final product';
  frmAutoCode.btnHypertrm.Hint := 'parameters fit Hypertrm';
  frmAutoCode.btnSmallest.Hint := 'produce smallest size Bootloader code';
  frmAutoCode.btnCancel.Hint := 'quit autocode function';
  frmAutoCode.btnMake.Hint := 'use current parameters make user configure file: bootcfg.h';
  frmAutoCode.chkRS485En.Hint := 'Enable RS485/RS422 mode';
  frmAutoCode.lbRS485En.Hint := frmAutoCode.chkRS485En.Hint;
  frmAutoCode.chkLED.Hint := 'use LED indicate Bootloader state';
  frmAutoCode.lbLEDEn.Hint := frmAutoCode.chkLED.Hint;
  frmAutoCode.cbbMCU.Hint := 'Select MCU''s type';
  frmAutoCode.cbbFCPU.Hint := 'AVR device system clock frequency';
  frmAutoCode.cbbBaudRate.Hint := 'communication baudrate';
  frmAutoCode.cbbComPortNo.Hint := 'select microcontroller''s UART';
  frmAutoCode.cbbBufferSize.Hint := 'AVR uart buffer size';
  frmAutoCode.cbbTimeClk.Hint := 'timeout for one connect, it is also basic time interval';
  frmAutoCode.cbbBootCnt.Hint := 'maximum connect count';
  frmAutoCode.cbbFileCnt.Hint := 'timeout for waiting package from PC';
  frmAutoCode.cbbInitDelay.Hint := 'delay cycle number after UART initialization, som early part of AVR device need this';
  frmAutoCode.cbbBootMode.Hint := 'Bootloader toggle mode. Default is UART, select LEVEL to get smaller code size';
  frmAutoCode.cbbBootStart.Hint := 'Boot section address';
  frmAutoCode.cbbCRC.Hint := 'checksum method';
  frmAutoCode.cbbAlgorithm.Hint := 'Choice crypt algorithm';
  frmAutoCode.cbbFlagAddr.Hint := 'Flag position in EEPROM';
  frmAutoCode.chkChipCheck.Hint := 'verify data write to Flash, it will enhance download reliability, strong recommand choose it(need BootStart parameter too)';
  frmAutoCode.chkWDG.Hint := 'Enable watchdog will enhance system reliability';
  frmAutoCode.chkVerbose.Hint := 'show more tip information, check it when you use Hypertrm';
  frmAutoCode.chkDecrypt.Hint := 'Enable decrypt function';
  frmAutoCode.chkSafeMode.Hint := 'Safe update mode, it will not jump to user application till update successfully';

  //version
  mmoVer.Lines.Text :=
    'v4.5     2008.Octo' + #13#10 +
    '       + Add SafeMode' + #13#10 +
    '' + #13#10 +
    'v4.2     2008.Jun' + #13#10 +
    '       + Add AES encrypt/decrypt function' + #13#10 +
    '       + Add random key function' + #13#10 +
    '' + #13#10 +
    'v4.1     2008.May' + #13#10 +
    '       - Fix download buffer length bug' + #13#10 +
    '       - Fix two bugs in import bootcfg.h' + #13#10 +
    '' + #13#10 +
    'v4.0     2008.Apr' + #13#10 +
    '       + Add PC1 encrypt/decrypt function' + #13#10 +
    '       + Add auto reload file function' + #13#10 +
    '       - Fix some bugs' + #13#10 +
    '' + #13#10 +
    'v3.5     2008.Mar' + #13#10 +
    '       + Create batch file "avrub.bat" after autocode bootcfg.h' + #13#10 +
    '       + Add byte/word option' + #13#10 +
    '' + #13#10 +
    'v3.1     2007.Octo' + #13#10 +
    '       - fix a bug in aub format import(thank Stojan Obid)' + #13#10 +
    '       + support XP appearance' + #13#10 +
    '' + #13#10 +
    'v3.0     2007.Jun.9' + #13#10 +
    '       - removed skin, redesigned interface' + #13#10 +
    '       + add import function, direct read user''s bootcfg.h file configure' + #13#10 +
    '       ^ improve smartmouse mode' + #13#10 +
    '       ^ improve autocode function, add parameters''s range check' + #13#10 +
    '' + #13#10 +
    'v2.0     2007.Apr.22' + #13#10 +
    '       + rewrite interface, use AlphaControl5 lite component skin' + #13#10 +
    '       + add smart mouse mode' + #13#10 +
    '       + support multilanguage' + #13#10 +
    '' + #13#10 +
    'v1.2     2007.Apr.10' + #13#10 +
    '       ^ modify some operate mode' + #13#10 +
    '       ^ modify some bugs' + #13#10 +
    '' + #13#10 +
    'v1.1.0   2007.Apr.02' + #13#10 +
    '       + support HEX file bigger than 64K' + #13#10 +
    '       + auto generate config file Bootcfg.h.' + #13#10 +
    '' + #13#10 +
    'v1.0.0   2007.Mar.22' + #13#10 +
    '       + First version, all basic functions. And extend standard XModem protocol(variable' + #13#10 +
    '         buffer size and checksum mode)' + #13#10 +
    '' + #13#10 +
    'Special thanks to' + #13#10 +
    '' + #13#10 +
    ' Liu haitao, Xu yankang, tda1552, Ni enwei, Cheng xiang, Stojan Obid, Luiz Francisco, seu gab etc.' + #13#10 +
    ''
    ;

  if CompareText(langname, 'English') = 0 then
  begin
    //Main
    //menu
    mnFile.Caption := '&File';
    mnLoad.Caption := '&Load';
    mnSave.Caption := '&Save';
    mnExit.Caption := 'E&xit';
    mnOperate.Caption := '&Operate';
    mnDownLoad.Caption := '&Download';
    mnAutoCode.Caption := '&Auto Config Code';
    mnOption.Caption := '&Option';
    mnSystem.Caption := '&System';
    mnComPortSetup.Caption := '&Comport';
    mnHelp.Caption := '&Help';
    mnAbout.Caption := '&About';
    mnContent.Caption := '&Content';
    mnWeb.Caption := 'HomePage';
    mnEmail.Caption := 'Send email';
    mnGroup.Caption := 'discuss group';
    btnMenuFile.Caption := mnFile.Caption;
    btnMenuOperate.Caption := mnOperate.Caption;
    btnMenuOption.Caption := mnOption.Caption;
    btnMenuHelp.Caption := mnHelp.Caption;
    //msg
    lang.msgError := 'Error';
    lang.msgWarning := 'Warning';
    lang.msgOpenComportFail := 'Open comport fail.';
    lang.msgSendRSTCmd := 'Send reset command';
    lang.msgStartDownload := 'Start download';
    lang.msgStopDownload := 'Stop download';
    lang.msgStartConnect := 'Start connect';
    lang.msgSendConnectKey := 'Send connect key';
    lang.msgConnectSuccess := 'Connect success';
    lang.msgConnectFail := 'Connect fail';
    lang.msgLines := 'Lines';
    lang.msgChecksum := 'Checksum';
    lang.msgLength := 'Length';
    lang.msgTooManyRetry := 'Too many retry!';
    lang.msgLoadFileFail := 'Loading file fail';
    lang.msgUnsupport32BitAddress := 'This soft don''t support 32-bit HEX format.';
    lang.msgLoadFileSuccess := 'Loading file success';
    lang.msgFileLength := 'File length';
    lang.msgFlashSize := 'Flash size';
    lang.msgBufferOverflow := 'Buffer overflow';
    lang.msgBufferSize := 'Buffer size';
    lang.msgQueryClearStat := 'Do you really want to clear statistic data?';
    lang.msgUpdateSuccess := 'Update success';
    lang.msgUpdateFail := 'Update fail';
    lang.msgUpdateAbort := 'Update abort';
    lang.msgSpeed := 'speed';
    lang.msgTimeElapse := 'time elapsed';
    lang.msgPackag := 'Packag';
    lang.msgData := 'Data';
    lang.msgRetry := 'Retry';
    lang.msgFmtErr := 'File format error.';
    lang.msgCrcErr := 'Data checksum error.';
    lang.msgKey := 'Key';
    lang.msgAlgorithm := 'Algorithm';
    lang.msgSetCryptOption := 'You may set crypt value in [AutoCode]';
    //About
    with frmAbout do
    begin
      Caption := 'About';
      lbTranslator.Caption := 'language: English';
      lbTotalRun.Caption := 'Total run';
      lbTotalDown.Caption := 'Total download';
      lbTotalSucc.Caption := 'Total success';
      lbTotalFail.Caption := 'Total fail';
      lbCurDown.Caption := 'Current download';
      lbCurSucc.Caption := 'Current success';
      lbCurFail.Caption := 'Current fail';
      btnOk.Caption := 'Ok';
      btnClearStat.Caption := 'Clear';
    end;
    //option
    with frmOption do
    begin
      Caption := 'Option';
      tsSystem.Caption := 'System';
      tsComport.Caption := 'Comport';
      btnFont.Caption := '&Font';
      btnOk.Caption := '&Ok';
      btnCancel.Caption := '&Cancel';
      btnHypertrm.Caption := '&Hypertrm';
      btnImport.Caption := '&Import';
      gpbConnectKey.Caption := 'Connect key';
      lbFlashSize.Caption := 'Flash size';
      lbFlashSizeBytes.Caption := 'bytes';
      lbCommProt.Caption := 'Communication protocol';
      lbFrameLen.Caption := 'Data frame length';
      lbBasicTime.Caption := 'Basic time interval';
      lbBasicTimeMs.Caption := 'ms';
      lbMaxConCnt.Caption := 'Maximum connect count';
      lbFrameLenBytes.Caption := lbFlashSizeBytes.Caption;
      lbComPort.Caption := 'Comport';
      lbBaudRate.Caption := 'Baudrate';
      lbDataBits.Caption := 'Data bits';
      lbStopBits.Caption := 'Stop bits';
      lbParity.Caption := 'Parity';
      lbFlowCtrl.Caption := 'Flow control';
      chkBeepEn.Caption := 'PC speaker prompt';
      chkSmartMouse.Caption := 'Smart mouse mode';
      chkReloadFile.Caption := 'Auto reload file';
      lbFillUnused.Caption := 'Fill unused space';
      lbSendRST.Caption := 'Send reset command';
      lbAutoSize.Caption := 'Auto calculate data size';
      cbbProtocol.Items.Strings[0] := 'Standard XModem';
      cbbProtocol.Items.Strings[1] := 'Lite XModem';
    end;
    //AutoCode
    with frmAutoCode do
    begin
      Caption := 'Auto Config Code';
      btnHypertrm.Caption := '&Hypertrm';
      btnCancel.Caption := '&Cancel';
      btnSmallest.Caption := '&Smallest';
      btnRecover.Caption := '&Reset';
      btnRecommand.Caption := 'R&ecommend';
      btnMake.Caption := '&Make';
      lbRS485PORT.Caption := 'Port';
      lbRS485PIN.Caption := 'Pin';
      lbLEDPORT.Caption := 'Port';
      lbLEDPIN.Caption := 'Pin';
      lbBOOTPORT.Caption := 'Port';
      lbBOOTPIN.Caption := 'Pin';
      lbBOOTLEVEL.Caption := 'Level';
      cbbCRC.Items := frmOption.cbbProtocol.Items;
      cbbBootLevel.Items.Strings[0] := 'Low';
      cbbBootLevel.Items.Strings[1] := 'High';
      cbbBootMode.Items.Strings[0] := 'Comport';
      cbbBootMode.Items.Strings[1] := 'Level';
    end;
    Exit;
  end;

  lng := TFastIniFile.Create(langname + '.lng');
  try
    //Main
    //menu
    mnFile.Caption := lng.ReadString('Menu', 'File', '&File');
    mnLoad.Caption := lng.ReadString('Menu', 'Load', '&Load');
    mnSave.Caption := lng.ReadString('Menu', 'Save', '&Save');
    mnExit.Caption := lng.ReadString('Menu', 'Exit', 'E&xit');
    mnOperate.Caption := lng.ReadString('Menu', 'Operate', '&Operate');
    mnDownLoad.Caption := lng.ReadString('Menu', 'Download', '&Download');
    mnAutoCode.Caption := lng.ReadString('Menu', 'AutoCode', '&Auto Config Code');
    mnOption.Caption := lng.ReadString('Menu', 'Option', '&Option');
    mnSystem.Caption := lng.ReadString('Menu', 'System', '&System');
    mnComPortSetup.Caption := lng.ReadString('Menu', 'Comport', '&Comport');
    mnHelp.Caption := lng.ReadString('Menu', 'Help', '&Help');
    mnAbout.Caption := lng.ReadString('Menu', 'About', '&About');
    mnContent.Caption := lng.ReadString('Menu', 'Content', '&Content');
    mnWeb.Caption := lng.ReadString('Menu', 'Homepage', 'Homepage');
    mnEmail.Caption := lng.ReadString('Menu', 'SendEMail', 'Send email');
    mnGroup.Caption := lng.ReadString('Menu', 'DiscussGroup', 'Discuss group');
    btnMenuFile.Caption := mnFile.Caption;
    btnMenuOperate.Caption := mnOperate.Caption;
    btnMenuOption.Caption := mnOption.Caption;
    btnMenuHelp.Caption := mnHelp.Caption;
    //msg
    lang.msgError := lng.ReadString('Message', 'Error', 'Error');
    lang.msgWarning := lng.ReadString('Message', 'Warning', 'Warning');
    lang.msgOpenComportFail := lng.ReadString('Message', 'OpenComportfail', 'Open comport fail.');
    lang.msgSendRSTCmd := lng.ReadString('Message', 'SendRSTCmd', 'Send reset command');
    lang.msgStartDownload := lng.ReadString('Message', 'StartDwonload', 'Start download');
    lang.msgStopDownload := lng.ReadString('Message', 'StopDownload', 'Stop download');
    lang.msgStartConnect := lng.ReadString('Message', 'StartConnect', 'Start connect');
    lang.msgSendConnectKey := lng.ReadString('Message', 'SendConnectKey', 'Send connect key');
    lang.msgConnectSuccess := lng.ReadString('Message', 'ConnectSuccess', 'Connect success');
    lang.msgConnectFail := lng.ReadString('Message', 'ConnectFail', 'Connect fail');
    lang.msgLines := lng.ReadString('Message', 'Lines', 'Lines');
    lang.msgChecksum := lng.ReadString('Message', 'CheckSum', 'Checksum');
    lang.msgLength := lng.ReadString('Message', 'Length', 'Length');
    lang.msgTooManyRetry := lng.ReadString('Message', 'TooManyRetry', 'Too many retry!');
    lang.msgLoadFileFail := lng.ReadString('Message', 'LoadFileFail', 'Loading file fail');
    lang.msgUnsupport32BitAddress := lng.ReadString('Message', 'Unsupport32BitAddress', 'This soft don''t support 32-bit HEX format.');
    lang.msgLoadFileSuccess := lng.ReadString('Message', 'LoadFileSuccess', 'Loading file success');
    lang.msgFileLength := lng.ReadString('Message', 'FileLength', 'File length');
    lang.msgFlashSize := lng.ReadString('Message', 'FlashSize', 'Flash size');
    lang.msgBufferOverflow := lng.ReadString('Message', 'BufferOverFlow', 'Buffer overflow.');
    lang.msgBufferSize := lng.ReadString('Message', 'BufferSize', 'Buffer size');
    lang.msgQueryClearStat := lng.ReadString('Message', 'QueryClearStat', 'Do you really want to clear statistic data?');
    lang.msgUpdateSuccess := lng.ReadString('Message', 'UpdateSuccess', 'Update success');
    lang.msgUpdateFail := lng.ReadString('Message', 'UpdateFail', 'Update fail');
    lang.msgUpdateAbort := lng.ReadString('Message', 'UpdateAbort', 'Update abort');
    lang.msgSpeed := lng.ReadString('Message', 'Speed', 'speed');
    lang.msgTimeElapse := lng.ReadString('Message', 'TimeElapse', 'time elapsed');
    lang.msgPackag := lng.ReadString('Message', 'Packag', 'Packag');
    lang.msgData := lng.ReadString('Message', 'Data', 'Data');
    lang.msgRetry := lng.ReadString('Message', 'Retry', 'Retry');
    lang.msgFmtErr := lng.ReadString('Message', 'FmtErr', 'File format error.');
    lang.msgCrcErr := lng.ReadString('Message', 'CrcErr', 'Data checksum error.');
    lang.msgKey := lng.ReadString('Message', 'Key', 'Key');
    lang.msgAlgorithm := lng.ReadString('Message', 'Algorithm', 'Algorithm');
    lang.msgSetCryptOption := lng.ReadString('Message', 'SetCryptOption', 'You may set crypt value in [AutoCode]');
    //About
    with frmAbout do
    begin
      Caption := lng.ReadString('About', 'About', 'About');
      lbTranslator.Caption := lng.ReadString('Language', 'Language', 'English') + ' / ' + lng.ReadString('Language', 'Translator', 'English');
      lbTotalRun.Caption := lng.ReadString('About', 'TotolRun', 'Total run');
      lbTotalDown.Caption := lng.ReadString('About', 'TotalDownload', 'Total download');
      lbTotalSucc.Caption := lng.ReadString('About', 'TotalSuccess', 'Total success');
      lbTotalFail.Caption := lng.ReadString('About', 'TotalFail', 'Total fail');
      lbCurDown.Caption := lng.ReadString('About', 'CurrentDownload', 'Current download');
      lbCurSucc.Caption := lng.ReadString('About', 'CurrentSuccess', 'Current success');
      lbCurFail.Caption := lng.ReadString('About', 'CurrentFail', 'Current fail');
      btnOk.Caption := lng.ReadString('About', 'Ok', 'Ok');
      btnClearStat.Caption := lng.ReadString('About', 'Clear', 'Clear');
    end;
    //option
    with frmOption do
    begin
      Caption := lng.ReadString('Option', 'Option', 'Option');
      tsSystem.Caption := lng.ReadString('Option', 'System', 'System');
      tsComport.Caption := lng.ReadString('Option', 'Comport', 'Comport');
      btnFont.Caption := lng.ReadString('Option', 'Font', '&Font');
      btnOk.Caption := lng.ReadString('Option', 'Ok', '&Ok');
      btnCancel.Caption := lng.ReadString('Option', 'Cancel', '&Cancel');
      btnHypertrm.Caption := lng.ReadString('Option', 'Hypertrm', '&Hypertrm');
      btnImport.Caption := lng.ReadString('Option', 'Import', '&Import');
      gpbConnectKey.Caption := lng.ReadString('Option', 'ConnectKey', 'Connect key');
      lbFlashSize.Caption := lng.ReadString('Option', 'FlashSize', 'Flash size');
      lbFlashSizeBytes.Caption := lng.ReadString('Option', 'bytes', 'bytes');
      lbCommProt.Caption := lng.ReadString('Option', 'Communication protocol', 'Communication protocol');
      lbFrameLen.Caption := lng.ReadString('Option', 'FrameLength', 'Data frame length');
      lbBasicTime.Caption := lng.ReadString('Option', 'BasicTimeInterval', 'Basic time interval');
      lbBasicTimeMs.Caption := lng.ReadString('Option', 'ms', 'ms');
      lbMaxConCnt.Caption := lng.ReadString('Option', 'MaxConCnt', 'Maximum connect count');
      lbFrameLenBytes.Caption := lbFlashSizeBytes.Caption;
      lbComPort.Caption := lng.ReadString('Option', 'Comport', 'Comport');
      lbBaudRate.Caption := lng.ReadString('Option', 'Baudrate', 'Baudrate');
      lbDataBits.Caption := lng.ReadString('Option', 'DataBits', 'Data bits');
      lbStopBits.Caption := lng.ReadString('Option', 'StopBits', 'Stop bits');
      lbParity.Caption := lng.ReadString('Option', 'Parity', 'Parity');
      lbFlowCtrl.Caption := lng.ReadString('Option', 'Flow control', 'Flow control');
      chkBeepEn.Caption := lng.ReadString('Option', 'BeepEn', 'PC speaker prompt');
      chkSmartMouse.Caption := lng.ReadString('Option', 'SmartMouse', 'Smart mouse mode');
      chkReloadFile.Caption := lng.ReadString('Option', 'reload', 'Auto reload file');
      lbFillUnused.Caption := lng.ReadString('Option', 'FillUnusedSpace', 'Fill unused space');
      lbSendRST.Caption := lng.ReadString('Option', 'SendRST', 'Send reset command');
      lbAutoSize.Caption := lng.ReadString('Option', 'AutoSize', 'Auto calculate data size');
      cbbProtocol.Items.Strings[0] := lng.ReadString('Option', 'StandardProtocol', 'Standard XModem');
      cbbProtocol.Items.Strings[1] := lng.ReadString('Option', 'LiteProtocol', 'Lite XModem');
    end;
    //AutoCode
    with frmAutoCode do
    begin
      Caption := lng.ReadString('AutoCode', 'AutoCode', 'Auto Config Code');
      btnHypertrm.Caption := lng.ReadString('AutoCode', 'Hypertrm', '&Hypertrm');
      btnCancel.Caption := lng.ReadString('AutoCode', 'Cancel', '&Cancel');
      btnSmallest.Caption := lng.ReadString('AutoCode', 'Smallest', '&Smallest');
      btnRecover.Caption := lng.ReadString('AutoCode', 'Reset', '&Reset');
      btnRecommand.Caption := lng.ReadString('AutoCode', 'Recommend', 'R&ecommend');
      btnMake.Caption := lng.ReadString('AutoCode', 'Make', '&Make');
      lbRS485PORT.Caption := lng.ReadString('AutoCode', 'Port', 'Port');
      lbRS485PIN.Caption := lng.ReadString('AutoCode', 'Pin', 'Pin');
      lbLEDPORT.Caption := lbRS485PORT.Caption;
      lbLEDPIN.Caption := lbRS485PIN.Caption;
      lbBOOTPORT.Caption := lbRS485PORT.Caption;
      lbBOOTPIN.Caption := lbRS485PIN.Caption;
      lbBOOTLEVEL.Caption := lng.ReadString('AutoCode', 'Level', 'Level');
      cbbCRC.Items := frmOption.cbbProtocol.Items;
      cbbBootLevel.Items.Strings[0] := lng.ReadString('AutoCode', 'Low', 'Low');
      cbbBootLevel.Items.Strings[1] := lng.ReadString('AutoCode', 'High', 'High');
      cbbBootMode.Items.Strings[0] := lng.ReadString('AutoCode', 'Comport', 'Comport');
      cbbBootMode.Items.Strings[1] := lbBOOTLEVEL.Caption;
    end;
  finally
    lng.Free;
  end;
end;

//Disable Debug mode

procedure TfrmMain.btnDebugClick(Sender: TObject);
var
  X: Word;
begin
  X := VK_F11;
  FormKeyDown(Sender, X, [ssAlt]);
end;

//Form movable

procedure TfrmMain.tbMenuMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  ReleaseCapture;
  PostMessage(Handle, wm_SysCommand, $F012, 0);
end;

procedure TfrmMain.delay(dt: Integer);
var
  i: Integer;
begin
  for i := 1 to dt do
  begin
    HRTmr.wait(1000);
    Application.ProcessMessages;
  end;
end;

procedure TfrmMain.ComPortRxChar(Sender: TObject; Count: Integer);
var
  i, cnt: Integer;
  s, ss: string;
begin
  if Count > 12 then
    cnt := 12
  else
    cnt := Count;
  ComPort.Read(RxBuf, cnt);
  case DownFlag of
    1: //connect
      begin
        s := '';
        ss := '';
        for i := 0 to cnt - 1 do
        begin
          s := s + Format('%.02X ', [RxBuf[i]]);
          if (RxBuf[i] >= $20) then
            ss := ss + Chr(RxBuf[i])
          else
            ss := ss + '.';
        end;
        loginfo(Format(': %-40s%s', [s, ss]));
        if ((RxBuf[0] = Ord('C')) and (Count = 1)) then
        begin
          DownCnt := DownCnt + 1;
          if DownCnt = 2 then
          begin
            DownFlag := 2;
          end;
        end;
      end;
    3: //download
      begin
        if RxBuf[0] = XMODEM_ACK then
          ACK := XMODEM_ACK
        else
          ACK := XMODEM_NAK;
        if Debug then
        begin
          s := '';
          ss := '';
          for i := 0 to cnt - 1 do
          begin
            s := s + Format('%.02X ', [RxBuf[i]]);
            if (RxBuf[i] >= $20) then
              ss := ss + Chr(RxBuf[i])
            else
              ss := ss + '.';
          end;
          loginfo(Format(': %-40s%s', [s, ss]));
        end;
      end;
    4: //do nothing
      begin

      end;
  else
    //do nothing
  end;
end;

procedure TfrmMain.ImportHisClick(Sender: TObject);
var
  FileName: string;
begin
  FileName := frmMain.pmImportHis.Items[TMenuItem(Sender).MenuIndex].Caption;
  if FileExists(FileName) then
  begin
    frmOption.ImportCfg(FileName);
    frmOption.dlgOpen.FileName := FileName;
    frmOption.btnImport.SetFocus;
  end;
end;

procedure TfrmMain.clrSmart;
begin
  if cfg.SmartMouse then
  begin
    GetCursorPos(mp);
    msx := mp.X;
    msy := mp.Y;
    tmrSmartMouse.Tag := 0;
  end;
end;

procedure TfrmMain.mmoLogMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  if Shift = [ssLeft, ssAlt, ssShift] then
    mmoLog.Clear;
end;

//Because of bug of Hexedit, I made this procedure
//Otherwise Hexedit's scrollbar will be error first time
//you load file

procedure TfrmMain.pgctMainChange(Sender: TObject);
begin
  if pgctMain.ActivePageIndex = 1 then
    mmoBin.Align := alClient
  else
    mmoBin.Align := alNone;
end;

procedure TfrmMain.btnEncryptClick(Sender: TObject);
var
  Key: array[0..64] of Byte;
  s: string;
  i, len: Integer;
begin
  mmoBin.Tag := 12;

  try
    s := frmAutoCode.edtEncryptKey.Text;
    case frmAutoCode.cbbAlgorithm.ItemIndex of
      0, 2: len := 16 * 3;
      1, 3: len := 32 * 3;
    else
      Exit;
    end;
    if Length(s) > len then
      SetLength(s, len)
    else
      for i := Length(s) to len - 1 do
        s := s + ' ';
    s := FormatHexStr(s, Key);

    if Application.MessageBox(PChar(btnEncrypt.Hint + ' :  ' + lang.msgSetCryptOption + #13#10#13#10 +
      lang.msgAlgorithm + ':   ' + frmAutoCode.cbbAlgorithm.Items[frmAutoCode.cbbAlgorithm.ItemIndex] + #13#10 +
      lang.msgKey + ':   [' + s + ']               '),
      PChar(btnEncrypt.Hint), MB_YESNO + MB_ICONQUESTION) = IDNO then
    begin
      Exit;
    end;

    loginfo('');
    loginfo('# Encrypt key: ' + s);
    case frmAutoCode.cbbAlgorithm.ItemIndex of
      0: //PC1-128
        begin
          loginfo('> Encrypt algorithm: PC1 - 128');
          for i := 0 to 15 do
            Key[i] := CharToByte(s[i * 3 + 1], s[i * 3 + 2]);
          PC1SetKey128(Key);
          mmoBin.SaveToBuffer(DataBuf, mmoBin.DataSize);
          PC1Encrypt128(DataBuf, EDBuf, mmoBin.DataSize);
          mmoBin.LoadFromBuffer(EDBuf, mmoBin.DataSize);
        end;
      1: //PC1-256
        begin
          loginfo('> Encrypt algorithm: PC1 - 256');
          for i := 0 to 31 do
            Key[i] := CharToByte(s[i * 3 + 1], s[i * 3 + 2]);
          PC1SetKey256(Key);
          mmoBin.SaveToBuffer(DataBuf, mmoBin.DataSize);
          PC1Encrypt256(DataBuf, EDBuf, mmoBin.DataSize);
          mmoBin.LoadFromBuffer(EDBuf, mmoBin.DataSize);
        end;
      2: //AES-128
        begin
          loginfo('> Encrypt algorithm: AES - 128');
          for i := 0 to 15 do
            Key[i] := CharToByte(s[i * 3 + 1], s[i * 3 + 2]);
          mmoBin.SaveToBuffer(DataBuf, mmoBin.DataSize);
          aesEncBlock(@DataBuf, @Key, mmoBin.DataSize);
          mmoBin.LoadFromBuffer(DataBuf, mmoBin.DataSize);
        end;
      3: //AES 256
        begin
          loginfo('> Encrypt algorithm: AES - 256');
          for i := 0 to 31 do
            Key[i] := CharToByte(s[i * 3 + 1], s[i * 3 + 2]);
          mmoBin.SaveToBuffer(DataBuf, mmoBin.DataSize);
          aesEncBlock(@DataBuf, @Key, mmoBin.DataSize, 256);
          mmoBin.LoadFromBuffer(DataBuf, mmoBin.DataSize);
        end;
    else
      //unknow algorithm
      ShowMessage('In product...');
      Exit;
    end;
    UpdateHex;
  finally
    loginfo('> Encrypt finished.');
    mmoBin.Tag := 0;
  end;
end;

procedure TfrmMain.btnDecryptClick(Sender: TObject);
var
  Key: array[0..64] of Byte;
  s: string;
  i, len: Integer;
begin
  mmoBin.Tag := 12;

  try
    s := frmAutoCode.edtDecryptKey.Text;
    case frmAutoCode.cbbAlgorithm.ItemIndex of
      0, 2: len := 16 * 3;
      1, 3: len := 32 * 3;
    else
      Exit;
    end;
    if Length(s) > len then
      SetLength(s, len)
    else
      for i := Length(s) to len do
        s := s + ' ';
    s := FormatHexStr(s, Key);

    if Application.MessageBox(PChar(btnDecrypt.Hint + ' :  ' + lang.msgSetCryptOption + #13#10#13#10 +
      lang.msgAlgorithm + ':   ' + frmAutoCode.cbbAlgorithm.Items[frmAutoCode.cbbAlgorithm.ItemIndex] + #13#10 +
      lang.msgKey + ':   [' + s + ']               '),
      PChar(btnDecrypt.Hint), MB_YESNO + MB_ICONQUESTION) = IDNO then
    begin
      Exit;
    end;

    loginfo('');
    loginfo('# Decrypt key: ' + s);
    case frmAutoCode.cbbAlgorithm.ItemIndex of
      0: //PC1-128
        begin
          loginfo('> Decrypt algorithm: PC1 - 128');
          for i := 0 to 15 do
            Key[i] := CharToByte(s[i * 3 + 1], s[i * 3 + 2]);
          PC1SetKey128(Key);
          mmoBin.SaveToBuffer(DataBuf, mmoBin.DataSize);
          PC1Decrypt128(DataBuf, EDBuf, mmoBin.DataSize);
          mmoBin.LoadFromBuffer(EDBuf, mmoBin.DataSize);
        end;
      1: //PC1-256
        begin
          loginfo('> Decrypt algorithm: PC1 - 256');
          for i := 0 to 31 do
            Key[i] := CharToByte(s[i * 3 + 1], s[i * 3 + 2]);
          PC1SetKey256(Key);
          mmoBin.SaveToBuffer(DataBuf, mmoBin.DataSize);
          PC1Decrypt256(DataBuf, EDBuf, mmoBin.DataSize);
          mmoBin.LoadFromBuffer(EDBuf, mmoBin.DataSize);
        end;
      2: //AES-128
        begin
          loginfo('> Decrypt algorithm: AES - 128');
          for i := 0 to 15 do
            Key[i] := CharToByte(s[i * 3 + 1], s[i * 3 + 2]);
          mmoBin.SaveToBuffer(DataBuf, mmoBin.DataSize);
          aesDecBlock(@DataBuf, @Key, mmoBin.DataSize);
          mmoBin.LoadFromBuffer(DataBuf, mmoBin.DataSize);
        end;
      3: //AES 256
        begin
          loginfo('> Decrypt algorithm: AES - 256');
          for i := 0 to 31 do
            Key[i] := CharToByte(s[i * 3 + 1], s[i * 3 + 2]);
          mmoBin.SaveToBuffer(DataBuf, mmoBin.DataSize);
          aesDecBlock(@DataBuf, @Key, mmoBin.DataSize, 256);
          mmoBin.LoadFromBuffer(DataBuf, mmoBin.DataSize);
        end;
    else
      //unknow algorithm
      ShowMessage('In product...');
      Exit;
    end;
    UpdateHex;
  finally
    loginfo('> Decrypt finished.');
    mmoBin.Tag := 0;
  end;
end;

procedure TfrmMain.UpdateHex;
var
  i, j: Integer;
  jy: Byte;
  s: string;
  stream: TMemoryStream;
begin
  DataSize := mmoBin.DataSize;
  //fill unused cell
  mmoBin.SaveToBuffer(DataBuf, DataSize);
  //autoszie
  if not cfg.AutoSize then
  begin
    DataSize := FlashSize[cfg.FlashSize]
  end;
  //make buffer size is multiple frame size
  if (DataSize mod cfg.FrameLen) <> 0 then
  begin
    DataSize := ((DataSize + cfg.FrameLen - 1) div cfg.FrameLen) * cfg.FrameLen;
  end;
  //even frame
  if ((DataSize div cfg.FrameLen) mod 2) = 1 then
  begin
    DataSize := DataSize + cfg.FrameLen;
  end;
  mmoBin.LoadFromBuffer(DataBuf, DataSize);

  stream := TMemoryStream.Create;
  try
    stream.Clear;
    stream.Seek(0, soFromBeginning);
    for i := 0 to ((DataSize + 15) div 16) - 1 do
    begin
      if (i > 0) and (((i * 16) mod 65536) = 0) then
      begin
        //segment address
        s := ':02000002100000  ';
        j := (i * 16 * 16) div 65536;
        s[10] := DateToChar(j div 16);
        s[11] := DateToChar(j mod 16);
        jy := 4 + CharToByte(s[10], s[11]);
        jy := -jy;
        s[14] := DateToChar(jy div 16);
        s[15] := DateToChar(jy mod 16);
        s[16] := #13;
        s[17] := #10;
        stream.WriteBuffer(s[1], 17);
      end;
      SetLength(s, 45);
      s[1] := ':';
      s[2] := '1';
      s[3] := '0';
      jy := (i * 16) div 256;
      s[4] := DateToChar(jy div 16);
      s[5] := DateToChar(jy mod 16);
      jy := (i * 16) mod 256;
      s[6] := DateToChar(jy div 16);
      s[7] := DateToChar(jy mod 16);
      s[8] := '0';
      s[9] := '0';
      jy := $10 + ((i * 16) div 256) + ((i * 16) mod 256);
      //fill data
      for j := 0 to 15 do
      begin
        jy := jy + DataBuf[i * 16 + j];
        s[j * 2 + 10] := DateToChar(DataBuf[i * 16 + j] div 16);
        s[j * 2 + 11] := DateToChar(DataBuf[i * 16 + j] mod 16);
      end;
      //calc checksum
      jy := -jy;
      s[42] := DateToChar(jy div 16);
      s[43] := DateToChar(jy mod 16);
      s[44] := #13;
      s[45] := #10;
      stream.WriteBuffer(s[1], 45);
    end;
    s := ':00000001FF';
    stream.WriteBuffer(s[1], 11);
    stream.Position := 0;
    mmoHex.Lines.BeginUpdate;
    mmoHex.Clear;
    mmoHex.Lines.LoadFromStream(stream);
    mmoHex.Lines.EndUpdate;
    pgctMain.ActivePageIndex := 1; //Bin
    pgctMainChange(Self);
  finally
    stream.Free;
  end;
end;

function TfrmMain.UpdateBin(FileName: string): Boolean;
var
  i, j: Integer;
  addr, len, seg: Integer;
  jy: Byte;
  s: string;
  buf: CBuf;
  stream: TMemoryStream;
begin
  Result := False;

  seg := 0; //HEX's segment, when data size > 64K
  for i := 0 to mmoHex.Lines.Count - 1 do
  begin
    s := mmoHex.Lines[i];
    if Length(s) = 0 then
      continue;
    if s[1] <> ':' then
    begin
      mmoHex.TopLine := i + 1;
      Application.MessageBox(PChar(lang.msgLoadFileFail + ': [ ' + FileName + ' ]' + #13#10 + lang.msgLines + ': ' + IntToStr(i + 1)), PChar(lang.msgError), MB_OK + MB_ICONSTOP);
      loginfo('');
      loginfo('X ' + lang.msgLoadFileFail + ': [ ' + FileName + ' ]');
      loginfo('> ' + lang.msgLines + ': ' + IntToStr(i + 1));
      Exit;
    end;
    len := CharToByte(s[2], s[3]);
    if Length(s) <> (len * 2 + 11) then
    begin
      mmoHex.TopLine := i + 1;
      Application.MessageBox(PChar(lang.msgLoadFileFail + ': [ ' + FileName + ' ]' + #13#10 + lang.msgLines + ': ' + IntToStr(i + 1)), PChar(lang.msgError + ': ' + lang.msgLength), MB_OK + MB_ICONSTOP);
      loginfo('');
      loginfo('X ' + lang.msgLoadFileFail + ': [ ' + FileName + ' ]: ' + lang.msgLength);
      loginfo('> ' + lang.msgLines + ': ' + IntToStr(i + 1));
      Exit;
    end;
    jy := 0;
    for j := 0 to len + 4 do
    begin
      buf[j] := CharToByte(s[j + j + 2], s[j + j + 3]);
      jy := jy + buf[j];
    end;
    if jy <> 0 then
    begin
      mmoHex.TopLine := i + 1;
      Application.MessageBox(PChar(lang.msgLoadFileFail + ': [ ' + FileName + ' ]' + #13#10 + lang.msgLines + ': ' + IntToStr(i + 1)), PChar(lang.msgError + ': ' + lang.msgChecksum), MB_OK + MB_ICONSTOP);
      loginfo('');
      loginfo('X ' + lang.msgLoadFileFail + ': [ ' + FileName + ' ]: ' + lang.msgChecksum);
      loginfo('> ' + lang.msgLines + ': ' + IntToStr(i + 1));
      Exit;
    end;
    case CharToByte(s[8], s[9]) of
      0: //Data
        begin
          addr := CharToByte(s[4], s[5]) * 256 + CharToByte(s[6], s[7]) + seg;
          for j := 0 to len - 1 do
            DataBuf[addr + j] := buf[4 + j];
          if DataSize < (addr + len) then
          begin
            DataSize := addr + len;
            if DataSize > FlashSize[cfg.FlashSize] then
            begin
              mmoBin.DataSize := DataSize;
              Application.MessageBox(PChar(lang.msgBufferOverflow + ': [' + IntToStr(DataSize) + '] > [' + IntToStr(FlashSize[cfg.FlashSize]) + ']'), PChar(lang.msgError), MB_OK + MB_ICONSTOP);
              loginfo('X ' + lang.msgBufferOverflow + ': [' + IntToStr(DataSize) + '] > [' + IntToStr(FlashSize[cfg.FlashSize]) + ']');
              Exit;
            end
          end;
        end;
      1: //End
        begin

        end;
      2: //Segment
        begin
          seg := (CharToByte(s[10], s[11]) * 256 + CharToByte(s[12], s[13])) * (Longword(16));
        end;
      4: //32bit address
        begin
          mmoHex.TopLine := i + 1;
          Application.MessageBox(PChar(lang.msgLoadFileFail + ': [ ' + FileName + ' ]' + #13#10 + lang.msgLines + ': ' + IntToStr(i + 1)), PChar(lang.msgError + ': ' + lang.msgUnsupport32BitAddress), MB_OK + MB_ICONWARNING);
          loginfo('');
          loginfo('X ' + lang.msgLoadFileFail + ': [ ' + FileName + ' ]: ' + lang.msgUnsupport32BitAddress);
          loginfo('> ' + lang.msgLines + ': ' + IntToStr(i + 1));
          Exit;
        end;
    else //other

    end;
  end;
  //autoszie
  if not cfg.AutoSize then
  begin
    DataSize := FlashSize[cfg.FlashSize]
  end;

  //make buffer size is multiple frame size
  if (DataSize mod cfg.FrameLen) <> 0 then
  begin
    DataSize := ((DataSize + cfg.FrameLen - 1) div cfg.FrameLen) * cfg.FrameLen;
  end;

  //even frame
  if ((DataSize div cfg.FrameLen) mod 2) = 1 then
  begin
    DataSize := DataSize + cfg.FrameLen;
  end;

  mmoBin.LoadFromBuffer(DataBuf, DataSize);
  Result := True;
end;

procedure TfrmMain.ApplicationEventsActivate(Sender: TObject);
begin
  if cfg.reload and FileModify(sr) then
  begin
    loginfo('');
    loginfo('# ' + frmOption.chkReloadFile.Caption);
    OpenFile(sr.Name);
  end;
end;

procedure TfrmMain.mnWebClick(Sender: TObject);
begin
  ShellExecute(Handle, nil, 'http://avrubd.googlepages.com', nil, nil, SW_SHOW);
end;

procedure TfrmMain.mnEmailClick(Sender: TObject);
begin
  ShellExecute(Handle, nil, 'mailto:Shaoziyang@gmail.com?subject=about AVR BootLoader Downloader', nil, nil, SW_SHOW);
end;

procedure TfrmMain.mnGroupClick(Sender: TObject);
begin
  ShellExecute(Handle, nil, 'http://groups.google.com/group/avrub?hl=en', nil, nil, SW_SHOW);
end;

end.

