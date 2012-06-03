{
  Project:  AVR Universal Bootloader Download
  File:     uOption.pas
  Describe: Show option dialog, import bootcfg.h config
            set config value
  Author:   Shaoziyang
  Date:     2007.6
  Version:  3.0
  Web:      http://shaoziyang.googlepages.com
        or  http://shaoziyang.bloger.com.cn (Chinese)
  Email:    shaoziyang@gmail.com
}

unit uOption;

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
  ComCtrls,
  StdCtrls,
  Buttons,
  ExtCtrls,
  CPort,
  CPortCtl,
  Menus,
  ImgList;

type
  TfrmOption = class(TForm)
    btnCancel: TBitBtn;
    btnFont: TBitBtn;
    btnHypertrm: TBitBtn;
    btnImport: TBitBtn;
    btnImportHis: TBitBtn;
    btnOk: TBitBtn;
    cbbBootCnt: TComboBox;
    cbbComPort: TComComboBox;
    cbbComPortBaudRate: TComboBox;
    cbbComPortDataBits: TComboBox;
    cbbComPortFlowControl: TComboBox;
    cbbComPortParity: TComboBox;
    cbbComPortPort: TComboBox;
    cbbComPortStopBits: TComboBox;
    cbbFlash: TComboBox;
    cbbFrameLen: TComboBox;
    cbbProtocol: TComboBox;
    cbbSendTime: TComboBox;
    chkAutoSize: TCheckBox;
    chkBeepEn: TCheckBox;
    chkComPortDTR: TCheckBox;
    chkComPortRTS: TCheckBox;
    chkFillUnused: TCheckBox;
    chkSendRST: TCheckBox;
    chkSmartMouse: TCheckBox;
    dlgFont: TFontDialog;
    dlgOpen: TOpenDialog;
    edtFillUnused: TEdit;
    edtKeyASC: TEdit;
    edtKeyHEX: TEdit;
    edtSendRST: TEdit;
    gpbConnectKey: TGroupBox;
    Image1: TImage;
    Image2: TImage;
    Image3: TImage;
    lbAutoSize: TLabel;
    lbBasicTime: TLabel;
    lbBasicTimeMs: TLabel;
    lbBaudRate: TLabel;
    lbCommProt: TLabel;
    lbComPort: TLabel;
    lbDataBits: TLabel;
    lbFillUnused: TLabel;
    lbFlashSize: TLabel;
    lbFlashSizeBytes: TLabel;
    lbFlowCtrl: TLabel;
    lbFont: TLabel;
    lbFrameLen: TLabel;
    lbFrameLenBytes: TLabel;
    lbMaxConCnt: TLabel;
    lbParity: TLabel;
    lbSendRST: TLabel;
    lbStopBits: TLabel;
    pgctOption: TPageControl;
    pnlBtn: TPanel;
    pnlpgctOption: TPanel;
    rbASC: TRadioButton;
    rbHEX: TRadioButton;
    tmrErr: TTimer;
    tsAVR: TTabSheet;
    tsComport: TTabSheet;
    tsSystem: TTabSheet;
    chkReloadFile: TCheckBox;
    Image4: TImage;
    procedure btnFontClick(Sender: TObject);
    procedure btnHypertrmClick(Sender: TObject);
    procedure btnImportClick(Sender: TObject);
    procedure btnImportHisClick(Sender: TObject);
    procedure btnOkClick(Sender: TObject);
    procedure cbbFrameLenChange(Sender: TObject);
    procedure dlgFontShow(Sender: TObject);
    procedure dlgOpenShow(Sender: TObject);
    procedure edtFillUnusedChange(Sender: TObject);
    procedure edtKeyHEXChange(Sender: TObject);
    procedure edtKeyHEXClick(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure lbAutoSizeClick(Sender: TObject);
    procedure lbSendRSTClick(Sender: TObject);
    procedure pnlpgctOptionMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure tmrErrTimer(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure lbFillUnusedClick(Sender: TObject);
  private
    { Private declarations }
    Modify: Boolean;
    procedure setcbbErr(cbb: TComboBox);
  public
    { Public declarations }
    procedure ImportCfg(FileName: string);
  end;

var
  frmOption: TfrmOption;
  cbbErr: TComboBox;
  IdleOpt: Boolean = True;

implementation

uses
  uMain,
  uAbout,
  uAutoCode;

{$R *.DFM}

procedure TfrmOption.FormShow(Sender: TObject);
var
  buf: CBuf;
begin
  btnOk.Enabled := True;
  pgctOption.Left := -1;
  pgctOption.Top := 0;
  pgctOption.Width := pnlpgctOption.Width + 4;
  pgctOption.Height := pnlpgctOption.Height + 3;

  //update Comport properties
  cbbComPort.ComProperty := cpPort;
  cbbComPort.Refresh;
  cbbComPortPort.Items := cbbComPort.Items;
  cbbComPortPort.ItemIndex := cbbComPortPort.Items.IndexOf(frmMain.ComPort.Port);
  if cbbComPortPort.ItemIndex = -1 then
    cbbComPortPort.ItemIndex := 0;

  cbbComPort.ComProperty := cpBaudRate;
  cbbComPort.Refresh;
  cbbComPortBaudRate.Items := cbbComPort.Items;
  cbbComPortBaudRate.ItemIndex := Ord(frmMain.ComPort.BaudRate);

  cbbComPort.ComProperty := cpDataBits;
  cbbComPort.Refresh;
  cbbComPortDataBits.Items := cbbComPort.Items;
  cbbComPortDataBits.ItemIndex := Ord(frmMain.ComPort.DataBits);

  cbbComPort.ComProperty := cpStopBits;
  cbbComPort.Refresh;
  cbbComPortStopBits.Items := cbbComPort.Items;
  cbbComPortStopBits.ItemIndex := Ord(frmMain.ComPort.StopBits);

  cbbComPort.ComProperty := cpParity;
  cbbComPort.Refresh;
  cbbComPortParity.Items := cbbComPort.Items;
  cbbComPortParity.ItemIndex := Ord(frmMain.ComPort.Parity.Bits);

  cbbComPort.ComProperty := cpFlowControl;
  cbbComPort.Refresh;
  cbbComPortFlowControl.Items := cbbComPort.Items;
  cbbComPortFlowControl.ItemIndex := Ord(frmMain.ComPort.FlowControl.FlowControl);

  chkComPortRTS.Checked := cfg.RTS;
  chkComPortDTR.Checked := cfg.DTR;

  //Load config data
  edtKeyASC.Text := cfg.keyASC;
  edtKeyHEX.Text := FormatHexStr(cfg.keyHEX, buf);
  if cfg.keyMode = 0 then
    rbASC.Checked := True
  else
    rbHEX.Checked := True;

  chkAutoSize.Checked := cfg.AutoSize;
  chkSendRST.Checked := cfg.SendRST;
  edtSendRST.Text := FormatHexStr(cfg.CmdRST, buf);
  chkFillUnused.Checked := cfg.fillunused;
  edtFillUnused.Text := FormatHexStr(cfg.filldata, buf);

  cbbProtocol.ItemIndex := cfg.Protocol;
  cbbBootCnt.Text := IntToStr(cfg.BootCnt);
  cbbSendTime.Text := IntToStr(cfg.SendTime);
  cbbFlash.ItemIndex := cfg.FlashSize;
  if cbbFlash.ItemIndex = -1 then
    cbbFlash.ItemIndex := 2;
  cbbFrameLen.Text := IntToStr(cfg.FrameLen);
  Modify := False;

  chkBeepEn.Checked := cfg.Beep;
  chkSmartMouse.Checked := cfg.SmartMouse;
  chkReloadFile.Checked := cfg.reload;

  lbFont.Font.Name := cfg.FontName;
  lbFont.Font.Size := cfg.FontSize;
  lbFont.Caption := lbFont.Font.Name + ' ' + IntToStr(lbFont.Font.Size);
  Font := lbFont.Font;
  //On Font change, TComboBox will select text automatically
  cbbFrameLen.SelLength := 0;
  cbbSendTime.SelLength := 0;
  cbbBootCnt.SelLength := 0;
end;

procedure TfrmOption.lbFillUnusedClick(Sender: TObject);
begin
  chkFillUnused.Checked := not chkFillUnused.Checked;
  Modify := True;
end;

procedure TfrmOption.lbSendRSTClick(Sender: TObject);
begin
  chkSendRST.Checked := not chkSendRST.Checked;
end;

procedure TfrmOption.lbAutoSizeClick(Sender: TObject);
begin
  chkAutoSize.Checked := not chkAutoSize.Checked;
  Modify := True
end;

procedure TfrmOption.btnFontClick(Sender: TObject);
begin
  dlgFont.Font := lbFont.Font;
  if dlgFont.Execute then
  begin
    lbFont.Caption := dlgFont.Font.Name + ' ' + IntToStr(dlgFont.Font.Size);
    lbFont.Font := dlgFont.Font;
    if lbFont.Font.Size < 8 then
      lbFont.Font.Size := 8;
    if lbFont.Font.Size > 24 then
      lbFont.Font.Size := 24;
  end;
  HWD := Handle;
end;

procedure TfrmOption.dlgFontShow(Sender: TObject);
begin
  HWD := dlgFont.Handle;
  frmMain.clrSmart;
end;

procedure TfrmOption.btnOkClick(Sender: TObject);
var
  buf: CBuf;
begin
  //avoid reiteration event
  if IdleOpt then
  begin
    IdleOpt := False;
    try
      //comport
      cfg.RTS := chkComPortRTS.Checked;
      cfg.DTR := chkComPortDTR.Checked;

      with frmMain.ComPort do
      begin
        BeginUpdate;
        Port := cbbComPortPort.Items[cbbComPortPort.ItemIndex];
        BaudRate := TBaudRate(cbbComPortBaudRate.ItemIndex);
        DataBits := TDataBits(cbbComPortDataBits.ItemIndex);
        StopBits := TStopBits(cbbComPortStopBits.ItemIndex);
        Parity.Bits := TParityBits(cbbComPortParity.ItemIndex);
        FlowControl.FlowControl := TFlowControl(cbbComPortFlowControl.ItemIndex);
        EndUpdate;
      end;

      edtKeyHEXClick(Sender);
      cfg.keyASC := edtKeyASC.Text;
      cfg.keyHEX := edtKeyHEX.Text;
      if rbASC.Checked then
        cfg.keyMode := 0
      else
        cfg.keyMode := 1;

      cfg.Protocol := cbbProtocol.ItemIndex;
      cfg.FlashSize := cbbFlash.ItemIndex;
      cfg.fillunused := chkFillUnused.Checked;
      cfg.filldata := edtFillUnused.Text;

      cfg.reload := chkReloadFile.Checked;
      cfg.Beep := chkBeepEn.Checked;
      cfg.SmartMouse := chkSmartMouse.Checked;
      frmMain.tmrSmartMouse.Enabled := cfg.SmartMouse;

      cfg.AutoSize := chkAutoSize.Checked;
      cfg.SendRST := chkSendRST.Checked;
      cfg.CmdRST := FormatHexStr(edtSendRST.Text, buf);

      try
        cfg.BootCnt := StrToInt(cbbBootCnt.Text);
        if (cfg.BootCnt < 2) then
          cfg.BootCnt := 2
        else
          if (cfg.BootCnt > 100) then
            cfg.BootCnt := 100;
      except
        Application.MessageBox(PChar(lang.msgError + ': [' + lbMaxConCnt.Caption + ']'), PChar(lang.msgError), MB_OK + MB_ICONSTOP);
        setcbbErr(cbbBootCnt);
        Exit;
      end;
      try
        cfg.SendTime := StrToInt(cbbSendTime.Text);
        if (cfg.SendTime < 100) then
          cfg.SendTime := 100
        else
          if (cfg.SendTime > 4096) then
            cfg.SendTime := 4096;
      except
        Application.MessageBox(PChar(lang.msgError + ': [' + lbBasicTime.Caption + ']'), PChar(lang.msgError), MB_OK + MB_ICONSTOP);
        setcbbErr(cbbSendTime);
        Exit;
      end;
      try
        cfg.FrameLen := StrToInt(cbbFrameLen.Text);
        if (cfg.FrameLen < 16) or (cfg.FrameLen > 4096) then
          cfg.FrameLen := 128;
      except
        Application.MessageBox(PChar(lang.msgError + ': [' + lbFrameLen.Caption + ']'), PChar(lang.msgError), MB_OK + MB_ICONSTOP);
        setcbbErr(cbbFrameLen);
        Exit;
      end;

      if cfg.SendTime < 100 then
        cfg.SendTime := 100;
      if cfg.SendTime > 2000 then
        cfg.SendTime := 2000;
      if cfg.BootCnt < 2 then
        cfg.BootCnt := 2;
      if cfg.BootCnt > 100 then
        cfg.BootCnt := 100;

      cfg.FontName := lbFont.Font.Name;
      cfg.FontSize := lbFont.Font.Size;
      frmMain.Font := lbFont.Font;
      frmMain.mmoHex.Font := lbFont.Font;
      frmMain.mmoLog.Font := lbFont.Font;
      frmMain.mmoVer.Font := lbFont.Font;

      //reload file
      if Modify and (frmMain.dlgOpen.FileName <> '') then
        frmMain.OpenFile(frmMain.dlgOpen.FileName);

      Close;
    finally
      IdleOpt := True;
    end;
  end;

end;

procedure TfrmOption.btnHypertrmClick(Sender: TObject);
begin
  //avoid reiteration event
  if IdleOpt then
  begin
    IdleOpt := False;
    try
      edtKeyASC.Text := 'd';
      edtKeyHEX.Text := '64';
      edtSendRST.Text := '7E';
      chkSendRST.Checked := True;
      cbbProtocol.ItemIndex := 0;
      cbbFrameLen.Text := '128';
      cbbBootCnt.Text := '10';
      cbbSendTime.Text := '500';
      edtFillUnused.Text := 'FF CF';
      chkFillUnused.Checked := False;
      rbASC.Checked := True;
      if pgctOption.ActivePageIndex <> 1 then
        pgctOption.ActivePageIndex := 1
      else
        pgctOption.ActivePageIndex := 0;
    finally
      IdleOpt := True;
    end;
  end;
end;

procedure TfrmOption.btnImportClick(Sender: TObject);
begin
  //avoid reiteration event
  if IdleOpt then
  begin
    IdleOpt := False;
    try
      dlgOpen.Title := btnImport.Caption + '           ';
      if dlgOpen.Execute then
      begin
        ImportCfg(dlgOpen.FileName);
      end;
    finally
      HWD := frmOption.Handle;
      IdleOpt := True;
    end;
  end;
end;

procedure TfrmOption.edtKeyHEXClick(Sender: TObject);
var
  buf: CBuf;
begin
  if not (Sender is TEdit) then
    Exit;

  TEdit(Sender).Text := FormatHexStr(TEdit(Sender).Text, buf);
end;

procedure TfrmOption.pnlpgctOptionMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  //释放鼠标事件的对象
  ReleaseCapture;
  PostMessage(Handle, wm_SysCommand, $F012, 0);
end;

procedure TfrmOption.edtKeyHEXChange(Sender: TObject);
begin
  if Sender is TEdit then
  begin
    if (Length(TEdit(Sender).Text) mod 3) = 0 then
    begin
      edtKeyHEXClick(Sender);
      PostMessage(TEdit(Sender).Handle, WM_KEYDOWN, VK_END, 0);
    end;
  end;
end;

procedure TfrmOption.edtFillUnusedChange(Sender: TObject);
begin
  edtKeyHEXChange(Sender);
  Modify := True;
end;

procedure TfrmOption.tmrErrTimer(Sender: TObject);
begin
  if tmrErr.Tag > 0 then
  begin
    if (tmrErr.Tag mod 2) = 0 then
      cbbErr.Color := clWindow
    else
      cbbErr.Color := clRed;
    tmrErr.Tag := tmrErr.Tag - 1;
  end
  else
  begin
    cbbErr.Color := clWindow;
    tmrErr.Enabled := False;
  end;
end;

//we must reload HEX/BIN file when this value changed

procedure TfrmOption.cbbFrameLenChange(Sender: TObject);
begin
  Modify := True;
end;

procedure TfrmOption.setcbbErr(cbb: TComboBox);
begin
  cbbErr := cbb;
  pgctOption.ActivePage := TTabSheet(cbb.Parent);
  tmrErr.Tag := 6;
  tmrErr.Enabled := True;
end;

procedure TfrmOption.btnImportHisClick(Sender: TObject);
begin
  //avoid reiteration event
  if IdleOpt then
  begin
    IdleOpt := False;
    try
      GetCursorPos(mp);
      frmMain.pmImportHis.Popup(mp.X, mp.Y);
    finally
      IdleOpt := True;
    end;
  end;
end;

procedure TfrmOption.ImportCfg(FileName: string);
var
  f: TextFile;
  lns, wds: string;
  i, len: Integer;
  klen: Integer;
  hf: TMenuItem;
begin
  try
    btnOk.Enabled := False;
    AssignFile(f, FileName);
    Reset(f);
    klen := -1;
    while not Eof(f) do
    begin
      Readln(f, lns);
      wds := TokenStr(lns);
      if wds = 'unsigned' then
      begin
        //KEY[]
        if TokenStr(lns) = 'char' then
          if TokenStr(lns) = 'ConnectKey[]' then
          begin
            if klen = -1 then
            begin
              frmMain.loginfo('X ' + btnImport.Caption + lang.msgError);
              frmMain.loginfo('> Macro ''CONNECTCNT'' must be define before ''ConnectKey[]'' !');
              Application.MessageBox('Macro ''CONNECTCNT'' must be define before ''KEY[]'' !', PChar(lang.msgWarning), MB_OK + MB_ICONWARNING);
              Exit;
            end;

            rbHEX.Checked := True;
            edtKeyHEX.Text := '';
            for i := 1 to klen do
            begin
              wds := TokenStr(lns);
              if wds = '' then
              begin
                frmMain.loginfo('X ' + btnImport.Caption + lang.msgError);
                frmMain.loginfo('> ' + lang.msgError + ': ' + gpbConnectKey.Caption + '(CONNECTCNT/KEY[])');
                Application.MessageBox(PChar(lang.msgError + ': ' + gpbConnectKey.Caption + '(CONNECTCNT/KEY[])'), PChar(lang.msgError), MB_OK + MB_ICONSTOP);
                Exit;
              end;
              if wds[1] = '''' then //Char: 'd'
              begin
                edtKeyHEX.Text := edtKeyHEX.Text + Format('%.02X ', [Ord(wds[2])]);
              end
              else
              begin //Number:0x10, 20
                try
                  len := StrToInt(wds) mod 256;
                except
                  len := 0;
                end;
                edtKeyHEX.Text := edtKeyHEX.Text + Format('%.02X ', [len]);
              end;
            end;
          end;
        Continue;
      end;

      if wds = '#define' then
      begin
        wds := TokenStr(lns);

        if wds = 'BUFFERSIZE' then
        begin
          wds := TokenStr(lns);
          if wds <> '' then
            cbbFrameLen.Text := wds;
          Continue;
        end;

        if wds = 'BAUDRATE' then
        begin
          try
            wds := TokenStr(lns);
            if Length(wds) > 0 then
            begin
              for i := 1 to Length(wds) do
              begin
                if (Ord(wds[i]) > Ord('9')) or (Ord(wds[i]) < Ord('0')) then
                  Break;
              end;
              SetLength(wds, i - 1);
            end;
            if cbbComPortBaudRate.Items.IndexOf(wds) = -1 then
            begin
              frmMain.loginfo('X ' + btnImport.Caption + lang.msgError);
              frmMain.loginfo('> ' + lang.msgError + ': BaudRate(BAUDRATE).');
              Application.MessageBox(PChar(lang.msgError + ': BaudRate(BAUDRATE).'), PChar(lang.msgError), MB_OK + MB_ICONSTOP);
              setcbbErr(cbbComPortBaudRate);
              Exit;
            end;
            cbbComPortBaudRate.ItemIndex := cbbComPortBaudRate.Items.IndexOf(wds);
            Continue;
          except
            setcbbErr(cbbComPortBaudRate);
            Exit;
          end;
        end;

        if wds = 'timeclk' then
        begin
          wds := TokenStr(lns);
          if wds <> '' then
            cbbSendTime.Text := wds;
          Continue;
        end;

        if wds = 'TimeOutCnt' then
        begin
          wds := TokenStr(lns);
          cbbBootCnt.Text := wds;
          try
            cbbBootCnt.Text := IntToStr(StrToInt(wds) + 10);
          except
            frmMain.loginfo('X ' + btnImport.Caption + lang.msgError);
            frmMain.loginfo('> ' + lang.msgError + ': [' + lbMaxConCnt.Caption + '](TimeOutCnt)');
            setcbbErr(cbbBootCnt);
            Exit;
          end;
          Continue;
        end;

        if wds = 'BootStart' then
        begin
          wds := TokenStr(lns);
          try
            len := StrToInt(wds);
            if len = 2 then
            begin
              wds := TokenStr(lns);
              if wds = '*' then
              begin
                wds := TokenStr(lns);
                if Length(wds) > 0 then
                begin
                  for i := Length(wds) downto 1 do
                  begin
                    if (Ord(wds[i]) <= Ord('9')) and (Ord(wds[i]) >= Ord('0')) then
                      Break;
                  end;
                  SetLength(wds, i);
                  if i > 0 then
                    len := StrToInt(wds) * 2;
                end;
              end;
            end;
            if (len < 0) or (len > FlashSize[7]) then
            begin
              frmMain.loginfo('X ' + btnImport.Caption + lang.msgError);
              frmMain.loginfo('> ' + lang.msgError + ': [' + lbFlashSize.Caption + '](BootStart)');
              setcbbErr(cbbFlash);
              Exit;
            end;
            if len = 0 then
              Exit;
            i := 0;
            while len >= FlashSize[i] do
              i := i + 1;
            cbbFlash.ItemIndex := i;
          except
            setcbbErr(cbbFlash);
            Exit;
          end;
          Continue;
        end;

        if wds = 'CONNECTCNT' then
        begin
          try
            klen := StrToInt(TokenStr(lns));
            if klen < 1 then
            begin
              frmMain.loginfo('X ' + btnImport.Caption + lang.msgError);
              frmMain.loginfo('> ' + lang.msgError + ': CONNECTCNT.');
              Application.MessageBox(PChar(lang.msgError + ': CONNECTCNT.'), PChar(lang.msgError), MB_OK + MB_ICONERROR);
              Exit;
            end;
          except
            frmMain.loginfo('X ' + btnImport.Caption + lang.msgError);
            frmMain.loginfo('> ' + lang.msgError + ': CONNECTCNT.');
            Application.MessageBox(PChar(lang.msgError + ': CONNECTCNT.'), PChar(lang.msgError), MB_OK + MB_ICONERROR);
            Exit;
          end;
          Continue;
        end;

        if wds = 'CRCMODE' then
        begin
          wds := TokenStr(lns);
          try
            cbbProtocol.ItemIndex := StrToInt(wds);
            if StrToInt(wds) >= cbbProtocol.Items.Count then
            begin
              frmMain.loginfo('X ' + btnImport.Caption + lang.msgError);
              frmMain.loginfo('> ' + lang.msgError + ': ' + lbCommProt.Caption);
              Application.MessageBox(PChar(lang.msgError + ': ' + lbCommProt.Caption), PChar(lang.msgError), MB_OK + MB_ICONERROR);
              setcbbErr(cbbProtocol);
              Exit;
            end;
          except
            frmMain.loginfo('X ' + btnImport.Caption + lang.msgError);
            frmMain.loginfo('> ' + lang.msgError + ': ' + lbCommProt.Caption);
            Application.MessageBox(PChar(lang.msgError + ': ' + lbCommProt.Caption), PChar(lang.msgError), MB_OK + MB_ICONERROR);
            setcbbErr(cbbProtocol);
            Exit;
          end;
          Continue;
        end;
      end; //end of token
    end; //end of while
    btnOk.Enabled := True;
    frmMain.loginfo('V Import file success: [' + FileName + ']');
    Application.MessageBox(PChar('Import file success: ' + FileName + '.   '), 'V', MB_OK + MB_ICONINFORMATION);

    for i := 0 to frmMain.pmImportHis.Items.Count - 1 do
      frmMain.pmImportHis.Items[i].Checked := False;
    if frmMain.pmImportHis.Items.Find(FileName) = nil then
    begin
      //History file list
      if frmMain.pmImportHis.Items.Count > MaxHisFileCount then
        frmMain.pmImportHis.Items.Delete(frmMain.pmImportHis.Items.Count - 1);
      hf := TMenuItem.Create(Self);
      hf.OnClick := frmMain.ImportHisClick;
      hf.Caption := FileName;
      hf.RadioItem := True;
      hf.Checked := True;
      frmMain.pmImportHis.Items.Insert(0, hf);
    end
    else
    begin
      frmMain.pmImportHis.Items.Find(FileName).Checked := True;
    end;
  finally
    CloseFile(f);
  end;
end;

procedure TfrmOption.dlgOpenShow(Sender: TObject);
begin
  HWD := FindWindow(nil, PChar(dlgOpen.Title));
  frmMain.clrSmart;
end;

procedure TfrmOption.FormCreate(Sender: TObject);
begin
  pgctOption.ActivePageIndex := 0;
end;

end.

