{
  Project:  AVR Universal Bootloader Download
  File:     uAutoCode.pas
  Describe: Show autocode dialog, auto make bootcfg.h file
  Author:   Shaoziyang
  Date:     2007.6
  Version:  4.0
  Web:      http://shaoziyang.googlepages.com
        or  http://shaoziyang.bloger.com.cn (Chinese)
  Email:    shaoziyang@gmail.com
}

unit uAutoCode;

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
  ExtCtrls,
  StdCtrls,
  Buttons,
  ShellAPI,
  CPortCtl,
  ComCtrls;

type
  TfrmAutoCode = class(TForm)
    btnCancel: TBitBtn;
    btnHypertrm: TBitBtn;
    btnMake: TBitBtn;
    btnRecommand: TBitBtn;
    btnRecover: TBitBtn;
    btnSmallest: TBitBtn;
    cbbCportBaudRate: TComComboBox;
    imgDragon: TImage;
    Timer: TTimer;
    tmrErr: TTimer;
    dlgSave: TSaveDialog;
    Label2: TLabel;
    pgct: TPageControl;
    tsGeneral: TTabSheet;
    tsKey: TTabSheet;
    lbFCPU: TLabel;
    lbBaudRate: TLabel;
    lbComPortNo: TLabel;
    lbBUFFERSIZE: TLabel;
    lbtimeclk: TLabel;
    lbTimeOutCnt: TLabel;
    lbTimeOutCntC: TLabel;
    lbInitDelay: TLabel;
    lbLEVELMODE: TLabel;
    lbchecksum: TLabel;
    lbBootStart: TLabel;
    lbMCU: TLabel;
    Label5: TLabel;
    Label6: TLabel;
    Label7: TLabel;
    btnReload: TSpeedButton;
    GroupBox1: TGroupBox;
    lbLEDPORT: TLabel;
    lbLEDPIN: TLabel;
    cbbPortLED: TComboBox;
    cbbPINLED: TComboBox;
    GroupBox2: TGroupBox;
    lbRS485PORT: TLabel;
    lbRS485PIN: TLabel;
    cbbPortRS485: TComboBox;
    cbbPINRS485: TComboBox;
    Panel1: TPanel;
    lbLEDEn: TLabel;
    Panel2: TPanel;
    lbRS485En: TLabel;
    GroupBox3: TGroupBox;
    lbBOOTPORT: TLabel;
    lbBOOTPIN: TLabel;
    lbBOOTLEVEL: TLabel;
    cbbPortLevel: TComboBox;
    cbbPINLevel: TComboBox;
    cbbBootLevel: TComboBox;
    Panel3: TPanel;
    imgLED: TImage;
    cbbFCPU: TComboBox;
    cbbTimeClk: TComboBox;
    cbbBootCnt: TComboBox;
    cbbFileCnt: TComboBox;
    cbbInitDelay: TComboBox;
    cbbBootMode: TComboBox;
    cbbBootStart: TComboBox;
    cbbCRC: TComboBox;
    cbbBufferSize: TComboBox;
    cbbComPortNo: TComboBox;
    cbbBaudRate: TComboBox;
    Panel4: TPanel;
    Label1: TLabel;
    cbbMCU: TComboBox;
    cbbByteWord: TComboBox;
    lbAlgorithm: TLabel;
    cbbAlgorithm: TComboBox;
    lbKEY: TLabel;
    edtKeyHEX: TEdit;
    Label4: TLabel;
    Label8: TLabel;
    Image1: TImage;
    Image2: TImage;
    Image3: TImage;
    Label9: TLabel;
    chkRS485En: TCheckBox;
    chkLED: TCheckBox;
    GroupBox4: TGroupBox;
    chkChipCheck: TCheckBox;
    chkWDG: TCheckBox;
    chkDecrypt: TCheckBox;
    chkVerbose: TCheckBox;
    edtEncryptKey: TMemo;
    edtDecryptKey: TMemo;
    btnRandomKey: TSpeedButton;
    chkSafeMode: TCheckBox;
    lbFlagAddr: TLabel;
    cbbFlagAddr: TComboBox;
    procedure chkLEDClick(Sender: TObject);
    procedure edtKeyHEXChange(Sender: TObject);
    procedure edtKeyHEXClick(Sender: TObject);
    procedure FormMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure lbLEDEnClick(Sender: TObject);
    procedure lbRS485EnClick(Sender: TObject);
    procedure TimerTimer(Sender: TObject);
    procedure btnRecommandClick(Sender: TObject);
    procedure btnHypertrmClick(Sender: TObject);
    procedure btnSmallestClick(Sender: TObject);
    procedure btnMakeClick(Sender: TObject);
    procedure tmrErrTimer(Sender: TObject);
    procedure dlgSaveShow(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure cbbByteWordChange(Sender: TObject);
    procedure cbbMCUChange(Sender: TObject);
    procedure cbbTimeClkChange(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure btnRandomKeyClick(Sender: TObject);
  private
    { Private declarations }
    Idle: Boolean;
    procedure setedtErr(edt: TEdit);
    procedure setcbbErr(cbb: TComboBox);
  public
    { Public declarations }
    function getAVRType: Integer;
  end;

var
  frmAutoCode: TfrmAutoCode;
  cbbErr: TComboBox;
  edtErr: TEdit;

implementation

uses
  uMain;

{$R *.DFM}

procedure TfrmAutoCode.FormMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  //释放鼠标事件的对象
  ReleaseCapture;
  PostMessage(handle, wm_SysCommand, $F012, 0);
end;

procedure TfrmAutoCode.TimerTimer(Sender: TObject);
begin
  imgLED.Visible := not imgLED.Visible;
end;

procedure TfrmAutoCode.chkLEDClick(Sender: TObject);
begin
  cbbTimeClkChange(Sender);
  Timer.Enabled := chkLED.Checked;
  imgLED.Visible := chkLED.Checked;
end;

procedure TfrmAutoCode.lbRS485EnClick(Sender: TObject);
begin
  chkRS485En.Checked := not chkRS485En.Checked;
end;

procedure TfrmAutoCode.lbLEDEnClick(Sender: TObject);
begin
  chkLED.Checked := not chkLED.Checked;
  chkLEDClick(Sender);
end;

procedure TfrmAutoCode.edtKeyHEXChange(Sender: TObject);
begin
  if Sender is TEdit then
  begin
    if (Length(TEdit(Sender).Text) mod 3) = 0 then
    begin
      edtKeyHEXClick(Sender);
      PostMessage(TEdit(Sender).handle, WM_KEYDOWN, VK_END, 0);
    end;
  end;
end;

procedure TfrmAutoCode.edtKeyHEXClick(Sender: TObject);
var
  buf: CBuf;
begin
  if not (Sender is TEdit) then
    Exit;

  TEdit(Sender).Text := FormatHexStr(TEdit(Sender).Text, buf);
end;

procedure TfrmAutoCode.btnRecommandClick(Sender: TObject);
begin
  chkVerbose.Checked := False;
  chkWDG.Checked := True;
  chkChipCheck.Checked := True;
  edtKeyHEX.Text := '41 56 52 55 42';
  cbbTimeClk.Text := '200';
  cbbBootCnt.Text := '5';
  cbbFileCnt.Text := '20';
  cbbBootMode.ItemIndex := 0;
  chkLED.Checked := True;
  cbbBufferSize.Text := '128';
  chkLED.Checked := True;
  cbbCRC.ItemIndex := 0;
  chkSafeMode.Checked := TRUE;
end;

procedure TfrmAutoCode.btnHypertrmClick(Sender: TObject);
begin
  chkVerbose.Checked := True;
  chkLED.Checked := True;
  edtKeyHEX.Text := '64';
  cbbBufferSize.Text := '128';
  cbbTimeClk.Text := '500';
  cbbBootCnt.Text := '10';
  cbbFileCnt.Text := '100';
  cbbBootMode.ItemIndex := 0;
  cbbCRC.ItemIndex := 0;
end;

procedure TfrmAutoCode.btnSmallestClick(Sender: TObject);
begin
  chkRS485En.Checked := False;
  chkLED.Checked := False;
  cbbInitDelay.Text := '0';
  cbbBootMode.ItemIndex := 1;
  chkChipCheck.Checked := False;
  chkWDG.Checked := False;
  chkVerbose.Checked := False;
  cbbCRC.ItemIndex := 1;
  chkSafeMode.Checked := FALSE;
  chkDecrypt.Checked := FALSE;
end;

//

procedure TfrmAutoCode.dlgSaveShow(Sender: TObject);
begin
  HWD := FindWindow(nil, PChar(dlgSave.Title));
  frmMain.clrSmart;
end;

procedure TfrmAutoCode.btnMakeClick(Sender: TObject);
var
  f: TextFile;
  buf: CBuf;
  i: Integer;
  t: Double;
  b: Boolean;
  msg: string;
  s: string;
begin
  if Idle = False then
    Exit;
  try
    Idle := False;
    edtKeyHEXClick(Sender);

    if (cbbMCU.Text = '') or (StrLIComp(PChar(cbbMCU.Text), PChar('at'), 2) <> 0) then
    begin
      Application.MessageBox(PChar(lang.msgError + ': MCU'), PChar(lang.msgError), MB_OK + MB_ICONSTOP);
      setcbbErr(cbbMCU);
      Exit;
    end;

    //check value range
    if edtKeyHEX.Text = '' then
    begin
      Application.MessageBox(PChar(lang.msgError + ': KEY[]'), PChar(lang.msgError), MB_OK + MB_ICONSTOP);
      setedtErr(edtKeyHEX);
      Exit;
    end;

    try
      btnMake.Tag := StrToInt(cbbFCPU.Text);
    except
      Application.MessageBox(PChar(lang.msgError + ': F_CPU'), PChar(lang.msgError), MB_OK + MB_ICONSTOP);
      setcbbErr(cbbFCPU);
      Exit;
    end;

    try
      btnMake.Tag := StrToInt(cbbBufferSize.Text);
    except
      Application.MessageBox(PChar(lang.msgError + ': BUFFERSIZE'), PChar(lang.msgError), MB_OK + MB_ICONSTOP);
      setcbbErr(cbbBufferSize);
      Exit;
    end;

    try
      btnMake.Tag := StrToInt(cbbTimeClk.Text);
    except
      Application.MessageBox(PChar(lang.msgError + ': timeclk'), PChar(lang.msgError), MB_OK + MB_ICONSTOP);
      setcbbErr(cbbTimeClk);
      Exit;
    end;

    try
      btnMake.Tag := StrToInt(cbbBootCnt.Text);
    except
      Application.MessageBox(PChar(lang.msgError + ': TimeOutCnt'), PChar(lang.msgError), MB_OK + MB_ICONSTOP);
      setcbbErr(cbbBootCnt);
      Exit;
    end;

    try
      btnMake.Tag := StrToInt(cbbFileCnt.Text);
    except
      Application.MessageBox(PChar(lang.msgError + ': TimeOutCntC'), PChar(lang.msgError), MB_OK + MB_ICONSTOP);
      setcbbErr(cbbFileCnt);
      Exit;
    end;

    try
      btnMake.Tag := StrToInt(cbbInitDelay.Text);
    except
      Application.MessageBox(PChar(lang.msgError + ': InitDelay'), PChar(lang.msgError), MB_OK + MB_ICONSTOP);
      setcbbErr(cbbInitDelay);
      Exit;
    end;

    try
      btnMake.Tag := StrToInt(cbbBootStart.Text);
    except
      Application.MessageBox(PChar(lang.msgError + ': BootStart'), PChar(lang.msgError), MB_OK + MB_ICONSTOP);
      setcbbErr(cbbBootStart);
      Exit;
    end;

    //Cala baudrate error
    t := StrToInt(cbbFCPU.Text) / (StrToInt(cbbBaudRate.Text) * 16);
    i := Round(t);
    if (100 * abs(t - i) / t) > 2 then
    begin
      Application.MessageBox(PChar(lang.msgError + ': BaudRate error = [' + FloatToStrF(100 * abs(t - i) / t, ffFixed, 1, 1) + '%] > 2%   '), PChar(lang.msgError), MB_OK + MB_ICONSTOP);
      setcbbErr(cbbBaudRate);
      Exit;
    end;

    //Write to file
    dlgSave.Title := frmMain.mnSave.Caption + '         ';
    if dlgSave.Execute then
    begin
      try
        if CompareText(cfg.language, 'CHINESE') = 0 then
          b := True
        else
          b := False;
        AssignFile(f, dlgSave.FileName);
        Rewrite(f);
        Writeln(f, '/*');
        Writeln(f, '');
        Writeln(f, '                           e Y8b    Y8b YV4.08P888 88e');
        Writeln(f, '                          d8b Y8b    Y8b Y888P 888 888D');
        Writeln(f, '                         d888b Y8b    Y8b Y8P  888 88"');
        Writeln(f, '                        d8WuHan888b    Y8b Y   888 b,');
        Writeln(f, '                       d8888888b Y8b    Y8P    888 88b,');
        Writeln(f, '           8888 8888       ,e,                                  888');
        Writeln(f, '           8888 888820088e  " Y8b Y888P ,e e, 888,8, dP"Y ,"Y88b888');
        Writeln(f, '           8888 8888888 88b888 Y8b Y8P d88 88b888 " C88b "8" 888888');
        Writeln(f, '           8888 8888888 888888  Y8b "  888   ,888    Y88D,ee 888888');
        Writeln(f, '           ''Y88 88P''888 888888   Y8P    "YeeP"888   d,dP "88 888888');
        Writeln(f, '   888 88b,                    d8  888                     888');
        Writeln(f, '   888 88P'' e88 88e  e88 88e  d88  888 e88 88e  ,"Y88b e88 888 ,e e, 888,8,');
        Writeln(f, '   888 8K  d888 888bd888 8Shaoziyang88d888 888b"8" 888d888 888d88 88b888 "');
        Writeln(f, '   888 88b,Y888 888PY888 888P 888  888Y888 888P,ee 888Y888 888888   ,888');
        Writeln(f, '   888 88P'' "88 88"  "88 88"  888  888 "88 88" "88 888 "88 888 "YeeP"888');
        Writeln(f, '');
        Writeln(f, '');
        if b then
          Writeln(f, '  Project:       AVR 通用 BootLoader')
        else
          Writeln(f, '  Project:       AVR Universal BootLoader');
        Writeln(f, '  File:          bootcfg.h');
        if b then
          Writeln(f, '                 用户配置参数，请根据你系统的实际情况进行设置')
        else
          Writeln(f, '                 user''s configuration.');
        Writeln(f, '  Version:       4.5');
        Writeln(f, '');
        Writeln(f, '  Compiler:      WinAVR20071221 + AVR Studio 4.14 b589');
        Writeln(f, '');
        Writeln(f, '  Author:        Shaoziyang');
        Writeln(f, '                 Shaoziyang@gmail.com');
        Writeln(f, '                 http://avrubd.googlepages.com');
        Writeln(f, '                 http://groups.google.com/group/avrub?hl=en');
        Writeln(f, '');
        Writeln(f, '  Date:          2008.8');
        Writeln(f, '  Made by:       AVRUBD autocode, ' + DateTimeToStr(now));
        Writeln(f, '');
        Writeln(f, '  See readme.htm to get more information.');
        Writeln(f, '');
        Writeln(f, '*/');
        Writeln(f, '');
        Writeln(f, '#ifndef _BOOTCFG_H_');
        Writeln(f, '#define _BOOTCFG_H_        1');
        Writeln(f, '');
        if b then
          Writeln(f, '//定义串口接收缓冲区长度')
        else
          Writeln(f, '//define uart buffer''s length');
        Writeln(f, '#define BUFFERSIZE         ', cbbBufferSize.Text);
        Writeln(f, '');
        if b then
          Writeln(f, '//系统时钟(Hz)')
        else
          Writeln(f, '//system clock(Hz)');
        Writeln(f, '#ifndef F_CPU');
        Writeln(f, '#define F_CPU              ', cbbFCPU.Text, 'UL');
        Writeln(f, '#endif');
        Writeln(f, '');
        if b then
          Writeln(f, '//波特率')
        else
          Writeln(f, '//baudrate');
        Writeln(f, '#define BAUDRATE           ', cbbBaudRate.Text);
        Writeln(f, '');
        if b then
        begin
          Writeln(f, '//Boot区起始地址: 按字节计算');
          Writeln(f, '//如果定义为0或不定义BootStart，可以节约空间');
        end
        else
        begin
          Writeln(f, '//Boot section start address(byte)');
          Writeln(f, '//define BootStart to 0 will disable this function');
        end;
        if cbbByteWord.ItemIndex = 0 then
          Writeln(f, '#define BootStart          ', cbbBootStart.Text + 'UL')
        else
          Writeln(f, '#define BootStart          ', '2 * ' + cbbBootStart.Text + 'UL');
        Writeln(f, '');
        if b then
        begin
          Writeln(f, '//下载过程中进行数据校验');
          Writeln(f, '//ChipCheck和BootStart必须联合使用，校验才有效');
        end
        else
        begin
          Writeln(f, '//verify flash''s data while write');
          Writeln(f, '//ChipCheck will only take effect while BootStart enable also');
        end;
        if chkChipCheck.Checked then
          i := 1
        else
          i := 0;
        Writeln(f, '#define ChipCheck          ', i);
        Writeln(f, '');
        if b then
          Writeln(f, '//安全升级模式下，只有升级成功后才会退出安全模式，适合远程升级')
        else
          Writeln(f, '//In SafeMode, it will not jump to user application till update successfully');
        if chkSafeMode.Checked then
          i := 1
        else
          i := 0;
        Writeln(f, '#define SafeMode           ', i);
        Writeln(f, '');
        if b then
          Writeln(f, '//在安全升级模式下，将在EEPROM中设置一个标志位')
        else
          Writeln(f, '//In SafeMode, it will set a flag in EEPROM');
        Writeln(f, '#define FlagAddr           ', cbbFlagAddr.Text);
        Writeln(f, '');
        if b then
          Writeln(f, '//Bootloader触发模式  0:串口  1:电平')
        else
          Writeln(f, '//Bootloader launch  0:comport password  1:port level');
        Writeln(f, '#define LEVELMODE          ', cbbBootMode.ItemIndex);
        Writeln(f, '');
        Writeln(f, '#define LEVELPORT          ', cbbPortLevel.Text);
        Writeln(f, '#define LEVELPIN           P', cbbPortLevel.Text, cbbPINLevel.Text);
        if b then
          Writeln(f, '//触发电平 1:高  0:低')
        else
          Writeln(f, '//port level  1:High  0:Low');
        Writeln(f, '#define PINLEVEL           ', cbbBootLevel.ItemIndex);
        Writeln(f, '');
        if b then
        begin
          Writeln(f, '//等待密码的超时时间 = TimeOutCnt * timeclk');
          Writeln(f, '//超时次数');
        end
        else
        begin
          Writeln(f, '//max wait password time = TimeOutCnt * timeclk');
          Writeln(f, '//timeout count');
        end;
        Writeln(f, '#define TimeOutCnt         ', cbbBootCnt.Text);
        Writeln(f, '');
        if CompareText(cfg.language, 'CHINESE') = 0 then
          Writeln(f, '//同步时间间隔(ms)')
        else
          Writeln(f, '//basic timer interval(ms)');
        Writeln(f, '#define timeclk            ', cbbTimeClk.Text);
        Writeln(f, '');
        if b then
        begin
          Writeln(f, '//等待文件的超时时间 = TimeOutCntC * timeclk');
          Writeln(f, '//发送''C''的最大次数');
        end
        else
        begin
          Writeln(f, '//max wait data time = TimeOutCntC * timeclk');
          Writeln(f, '//send ''C'' command count');
        end;
        Writeln(f, '#define TimeOutCntC        ', cbbFileCnt.Text);
        Writeln(f, '');
        if b then
          Writeln(f, '//同步密码长度')
        else
          Writeln(f, '//password length');
        edtKeyHEX.Text := FormatHexStr(edtKeyHEX.Text, buf);
        Writeln(f, '#define CONNECTCNT         ', (Length(edtKeyHEX.Text) + 2) div 3);
        Writeln(f, '');
        if b then
          Writeln(f, '//同步密码')
        else
          Writeln(f, '//password');
        Writeln(f, '#if LEVELMODE == 0');
        Write(f, 'unsigned char ConnectKey[] = {');
        for i := 1 to ((Length(edtKeyHEX.Text) + 2) div 3) - 1 do
          Write(f, '0x', edtKeyHEX.Text[i * 3 - 2], edtKeyHEX.Text[i * 3 - 1], ', ');
        i := (Length(edtKeyHEX.Text) + 2) div 3;
        Write(f, '0x', edtKeyHEX.Text[i * 3 - 2], edtKeyHEX.Text[i * 3 - 1], '};');
        Writeln(f, '');
        Writeln(f, '#endif');
        Writeln(f, '');
        if b then
          Writeln(f, '//串口号: 0/1/2..')
        else
          Writeln(f, '//comport number: 0/1/2..');
        Writeln(f, '#define COMPORTNo          ', cbbComPortNo.Text);
        Writeln(f, '');
        if b then
          Writeln(f, '//看门狗使能')
        else
          Writeln(f, '//enable watchdog');
        if chkWDG.Checked then
          i := 1
        else
          i := 0;
        Writeln(f, '#define WDG_En             ', i);
        Writeln(f, '');
        if b then
          Writeln(f, '//使用RS485模式')
        else
          Writeln(f, '//enable RS485/RS422 mode');
        if chkRS485En.Checked then
          i := 1
        else
          i := 0;
        Writeln(f, '#define RS485              ', i);
        if b then
          Writeln(f, '//RS485/RS422控制端口和引脚')
        else
          Writeln(f, '//RS485/RS422 send control port');
        Writeln(f, '#define RS485PORT          ', cbbPortRS485.Text);
        Writeln(f, '#define RS485TXEn          P', cbbPortRS485.Text, cbbPINRS485.Text);
        Writeln(f, '');
        if b then
          Writeln(f, '//使用LED指示状态')
        else
          Writeln(f, '//enable LED indication');
        if chkLED.Checked then
          i := 1
        else
          i := 0;
        Writeln(f, '#define LED_En             ', i);
        if b then
          Writeln(f, '//LED控制端口和引脚')
        else
          Writeln(f, '//LED control port');
        Writeln(f, '#define LEDPORT            ', cbbPortLED.Text);
        Writeln(f, '#define LEDPORTNo          P', cbbPortLED.Text, cbbPINLED.Text);
        Writeln(f, '');
        if b then
          Writeln(f, '//某些旧型号的单片机在初始化后需要延时')
        else
          Writeln(f, '//some old kind of AVR need special delay after comport initialization');
        Writeln(f, '#define InitDelay          ', cbbInitDelay.Text);
        Writeln(f, '');
        if b then
          Writeln(f, '//校验方法  0:CRC16  1:累加和')
        else
          Writeln(f, '//communication checksum method   0:CRC16  1:add up');
        Writeln(f, '#define CRCMODE            ', cbbCRC.ItemIndex);
        Writeln(f, '');
        if b then
          Writeln(f, '//提示模式: 显示更多交互提示信息，不用可以节省空间')
        else
          Writeln(f, '//Verbose mode: display more prompt message');
        if chkVerbose.Checked then
          i := 1
        else
          i := 0;
        Writeln(f, '#define VERBOSE            ', i);
        Writeln(f, '');
        if b then
        begin
          Writeln(f, '//提示信息');
          Writeln(f, '#if VERBOSE');
          Writeln(f, '#if LEVELMODE');
          Writeln(f, 'const char msg6[] = "升级模式.";');
          Writeln(f, 'const char msg7[] = "正常启动.";');
          Writeln(f, '#else');
          Writeln(f, '//等待密码');
          Writeln(f, 'const char msg1[] = "等待接收密码.";');
          Writeln(f, '#endif');
          Writeln(f, '//超时');
          Writeln(f, 'const char msg2[] = "超时.";');
          Writeln(f, '//等待接收数据');
          Writeln(f, 'const char msg3[] = "等待接收数据.";');
          Writeln(f, '//升级成功');
          Writeln(f, 'const char msg4[] = "升级成功.";');
          Writeln(f, '//升级失败');
          Writeln(f, 'const char msg5[] = "升级失败.";');
          Writeln(f, '#endif');
        end
        else
        begin
          Writeln(f, '//prompt messages');
          Writeln(f, '#if VERBOSE');
          Writeln(f, '#if LEVELMODE');
          Writeln(f, 'const char msg6[] = "bootloader mode.";');
          Writeln(f, 'const char msg7[] = "application mode.";');
          Writeln(f, '#else');
          Writeln(f, '//waiting for password');
          Writeln(f, 'const char msg1[] = "waiting for password.";');
          Writeln(f, '#endif');
          Writeln(f, '//timeout');
          Writeln(f, 'const char msg2[] = "timeout.";');
          Writeln(f, '//waiting for data');
          Writeln(f, 'const char msg3[] = "waiting for data.";');
          Writeln(f, '//update success');
          Writeln(f, 'const char msg4[] = "update success.";');
          Writeln(f, '//update fail');
          Writeln(f, 'const char msg5[] = "update fail.";');
          Writeln(f, '#endif');
        end;
        Writeln(f, '');
        if b then
          Writeln(f, '//允许解密')
        else
          Writeln(f, '//enable decrypt buffer');
        if chkDecrypt.Checked then
          i := 1
        else
          i := 0;
        Writeln(f, '#define Decrypt            ', i);
        Writeln(f, '');
        if b then
        begin
          Writeln(f, '//加密算法');
          Writeln(f, '//PC1_128, 128位 PC1 加密算法');
          Writeln(f, '#define PC1_128            1');
          Writeln(f, '//PC1_256, 256位 PC1 加密算法');
          Writeln(f, '#define PC1_256            2');
          Writeln(f, '//AES_128, 128位 AES 加密算法');
          Writeln(f, '#define AES_128            3');
          Writeln(f, '//AES_256, 256位 AED 加密算法');
          Writeln(f, '#define AES_256            4');
        end
        else
        begin
          Writeln(f, '//crypt algorithm');
          Writeln(f, '//PC1_128, PC1 Cipher Algorithm with 128 bits key');
          Writeln(f, '#define PC1_128            1');
          Writeln(f, '//PC1_256, PC1 Cipher Algorithm with 256 bits key');
          Writeln(f, '#define PC1_256            2');
          Writeln(f, '//AES_128, AES Cipher Algorithm with 128 bits key');
          Writeln(f, '#define AES_128            3');
          Writeln(f, '//AES_256, AES Cipher Algorithm with 256 bits key');
          Writeln(f, '#define AES_256            4');
        end;
        Writeln(f, '');
        Writeln(f, '#define Algorithm          ', cbbAlgorithm.Text);
        Writeln(f, '');
        Writeln(f, '#if Decrypt');
        Writeln(f, '');
        Writeln(f, '#if (Algorithm == PC1_128) || (Algorithm == AES_128)');
        if b then
          Writeln(f, '//定义解密密钥: 128位')
        else
          Writeln(f, '//Define decrypt key: 128b');
        Writeln(f, 'unsigned char DecryptKey[16] ={');
        for i := 0 to 16 do
          buf[i] := 0;
        s := edtDecryptKey.Text;
        if Length(s) > 16 * 3 then
          SetLength(s, 16 * 3);
        for i := 0 to (Length(s) + 2) div 3 do
          buf[i] := CharToByte(s[i * 3 + 1], s[i * 3 + 2]);
        for i := 0 to 15 do
        begin
          if (i mod 8) = 0 then
            Write(f, ' ');
          Write(f, ' 0x', IntToHex(buf[i], 2));
          if i <> 15 then
            Write(f, ',');
          if (i mod 8) = 7 then
            Writeln(f);
        end;
        Writeln(f, '};');
        Writeln(f, '#elif (Algorithm == PC1_256) || (Algorithm == AES_256)');
        if b then
          Writeln(f, '//定义解密密钥: 256位')
        else
          Writeln(f, '//Define decrypt key: 256b');
        Writeln(f, 'unsigned char DecryptKey[32] ={');
        for i := 0 to 31 do
          buf[i] := 0;
        s := edtDecryptKey.Text;
        if Length(s) > 32 * 3 then
          SetLength(s, 32 * 3);
        for i := 0 to (Length(s) + 2) div 3 do
          buf[i] := CharToByte(s[i * 3 + 1], s[i * 3 + 2]);
        for i := 0 to 31 do
        begin
          if (i mod 8) = 0 then
            Write(f, ' ');
          Write(f, ' 0x', IntToHex(buf[i], 2));
          if i <> 31 then
            Write(f, ',');
          if (i mod 8) = 7 then
            Writeln(f);
        end;
        Writeln(f, '};');
        Writeln(f, '');
        Writeln(f, '#endif');
        Writeln(f, '');
        Writeln(f, '#endif');
        Writeln(f, '');
        Writeln(f, '#endif');
        Writeln(f, '');
        Writeln(f, '//End of file: bootcfg.h');
      finally
        CloseFile(f);
      end;

      try
        AssignFile(f, ExtractFilePath(dlgSave.FileName) + 'avrub.bat');
        Rewrite(f);
        Writeln(f, '@rem auto create by AVRUBD at ' + DateTimeToStr(now));
        Writeln(f, 'avr-gcc.exe  -mmcu=' + lowercase(cbbMCU.Text) + ' -Wall -gdwarf-2  -Os -fsigned-char -MD -MP  -c  bootldr.c');
        if cbbByteWord.ItemIndex = 0 then
          i := StrToInt(cbbBootStart.Text)
        else
          i := StrToInt(cbbBootStart.Text) * 2;
        Writeln(f, 'avr-gcc.exe -mmcu=' + lowercase(cbbMCU.Text) + '  -Wl,-section-start=.text=0x' + IntToHex(i, 4) + ' bootldr.o     -o Bootldr.elf');
        Writeln(f, 'avr-objcopy -O ihex -R .eeprom  Bootldr.elf Bootldr.hex');
        Writeln(f, '@pause');
      finally
        CloseFile(f);
      end;

      if b then
        msg := '成功创建参数配置文件. 立即编译 bootloader 吗?   ' + #13#10#13#10 + '你可以在任何时候运行 "avrub.bat" 进行编译.' + #13#10
      else
        msg := 'Create configuration file success. Compile bootloader right now?   ' + #13#10#13#10 + 'You may run "avrub.bat" to complie bootloader at any time.' + #13#10;
      if Application.MessageBox(PChar(msg), '?', MB_YESNO + MB_ICONQUESTION) = IDYES then
      begin
        ShellExecute(handle, nil, PChar('"' + ExtractFilePath(dlgSave.FileName) + 'avrub.bat"'), nil, nil, SW_SHOW);
      end;

    end;
    Close;
  finally
    Idle := True;
    HWD := handle;
  end;
end;

procedure TfrmAutoCode.tmrErrTimer(Sender: TObject);
begin
  if tmrErr.Tag > 0 then
  begin
    if (tmrErr.Tag mod 2) = 0 then
    begin
      if edtErr <> nil then
        edtErr.Color := clWindow;
      if cbbErr <> nil then
        cbbErr.Color := clWindow;
    end
    else
    begin
      if edtErr <> nil then
        edtErr.Color := clRed;
      if cbbErr <> nil then
        cbbErr.Color := clRed;
    end;
    tmrErr.Tag := tmrErr.Tag - 1;
  end
  else
  begin
    if edtErr <> nil then
    begin
      edtErr.Color := clWindow;
      edtErr := nil;
    end;
    if cbbErr <> nil then
    begin
      cbbErr.Color := clWindow;
      cbbErr := nil;
    end;
    tmrErr.Enabled := False;
  end;
end;

procedure TfrmAutoCode.setcbbErr(cbb: TComboBox);
begin
  cbbErr := cbb;
  tmrErr.Tag := 6;
  tmrErr.Enabled := True;
end;

procedure TfrmAutoCode.setedtErr(edt: TEdit);
begin
  edtErr := edt;
  tmrErr.Tag := 6;
  tmrErr.Enabled := True;
end;

procedure TfrmAutoCode.FormShow(Sender: TObject);
var
  buf: CBuf;
  i: Integer;
begin
  Idle := True;
  edtErr := nil;
  cbbErr := nil;

  //cbbMCU.Sorted := True;
  case cfg.FlashSize of
    0, 1: cbbMCU.Text := '';
    2: cbbMCU.Text := 'ATmega88';
    3: cbbMCU.Text := 'ATmega168';
    4: cbbMCU.Text := 'ATmega32';
    5: cbbMCU.Text := 'ATmega64';
    6: cbbMCU.Text := 'ATmega128';
    7: cbbMCU.Text := 'ATmega2560';
  end;

  cbbCportBaudRate.Refresh;
  cbbBaudRate.Items := cbbCportBaudRate.Items;
  cbbBaudRate.ItemIndex := Ord(frmMain.ComPort.BaudRate);
  if cbbBaudRate.ItemIndex = -1 then
    cbbBaudRate.ItemIndex := 9;

  cbbComPortNo.ItemIndex := 0;
  cbbBufferSize.Text := IntToStr(cfg.FrameLen);

  cbbCRC.ItemIndex := cfg.Protocol;
  if cbbCRC.ItemIndex = -1 then
    cbbCRC.ItemIndex := 0;

  cbbBootLevel.ItemIndex := 0;
  cbbBootMode.ItemIndex := 0;

  cbbPINRS485.ItemIndex := 5;
  cbbPINLED.ItemIndex := 6;
  cbbPINLevel.ItemIndex := 7;

  cbbBufferSize.Text := IntToStr(cfg.FrameLen);
  cbbTimeClk.Text := IntToStr(cfg.SendTime);
  cbbBootCnt.Text := IntToStr(cfg.BootCnt);

  if FlashSize[cfg.FlashSize] > 4096 then
    cbbBootStart.Text := '0x' + IntToHex((FlashSize[cfg.FlashSize] - 2048) div 2, 4);
  cbbByteWord.Tag := 1;
  cbbByteWord.ItemIndex := 1;
  cbbByteWordChange(Sender);
  cbbMCUChange(Sender);

  edtKeyHEX.Text := FormatHexStr(cfg.keyHEX, buf);

  Font.Name := cfg.FontName;
  Font.Size := cfg.FontSize;

  chkLED.Checked := True;
  chkLEDClick(Sender);

  lbMCU.Hint := cbbMCU.Hint;
  lbBootStart.Hint := cbbBootStart.Hint;
  lbFCPU.Hint := cbbFCPU.Hint;
  lbBaudRate.Hint := cbbBaudRate.Hint;
  lbComPortNo.Hint := cbbComPortNo.Hint;
  lbBUFFERSIZE.Hint := cbbBufferSize.Hint;
  lbtimeclk.Hint := cbbTimeClk.Hint;
  lbTimeOutCnt.Hint := cbbBootCnt.Hint;
  lbTimeOutCntC.Hint := cbbFileCnt.Hint;
  lbInitDelay.Hint := cbbInitDelay.Hint;
  lbLEVELMODE.Hint := cbbBootMode.Hint;
  lbchecksum.Hint := cbbCRC.Hint;
  lbAlgorithm.Hint := cbbAlgorithm.Hint;
  lbFlagAddr.Hint := cbbFlagAddr.Hint;

  pgct.ActivePageIndex := 0;

  if cbbAlgorithm.ItemIndex = -1 then
    cbbAlgorithm.ItemIndex := 0;

  for i := 0 to ComponentCount - 1 do
    if Components[i] is TComboBox then
    begin
      TComboBox(Components[i]).SelLength := 0;
    end;
end;

procedure TfrmAutoCode.cbbByteWordChange(Sender: TObject);
var
  s: string;
  i: Integer;
  t: Integer;
begin
  if cbbByteWord.ItemIndex <> cbbByteWord.Tag then
  begin
    s := cbbBootStart.Text;
    for i := 0 to cbbBootStart.Items.Count - 1 do
    begin
      t := StrToInt(cbbBootStart.Items.Strings[i]);
      if cbbByteWord.ItemIndex = 0 then
        t := t * 2
      else
        t := t div 2;
      cbbBootStart.Items.Strings[i] := '0x' + IntToHex(t, 4);
    end;
    cbbByteWord.Tag := cbbByteWord.ItemIndex;
    try
      t := StrToInt(s);
      if cbbByteWord.ItemIndex = 0 then
        t := t * 2
      else
        t := t div 2;
      cbbBootStart.Text := '0x' + IntToHex(t, 4);
    except

    end;
  end;
end;

procedure TfrmAutoCode.cbbMCUChange(Sender: TObject);
begin
  case getAVRType of
    1:
      begin
        if cbbByteWord.ItemIndex = 0 then
        begin
          cbbBootStart.Items.Text := '0x1800' + #13#10 + '0x1C00' + #13#10 + '0x1E00' + #13#10 + '0x1F00';
        end
        else
        begin
          cbbBootStart.Items.Text := '0x0C00' + #13#10 + '0x0E00' + #13#10 + '0x0F00' + #13#10 + '0x0F80';
        end;
      end;
    2:
      begin
        if cbbByteWord.ItemIndex = 0 then
        begin
          cbbBootStart.Items.Text := '0x3800' + #13#10 + '0x3C00' + #13#10 + '0x3E00' + #13#10 + '0x3F00';
        end
        else
        begin
          cbbBootStart.Items.Text := '0x1C00' + #13#10 + '0x1E00' + #13#10 + '0x1F00' + #13#10 + '0x1F80';
        end;
      end;
    3:
      begin
        if cbbByteWord.ItemIndex = 0 then
        begin
          if CompareText(cbbMCU.Text, 'AT90CAN32') = 0 then
            cbbBootStart.Items.Text := '0x6000' + #13#10 + '0x7000' + #13#10 + '0x7800' + #13#10 + '0x7C00'
          else
            cbbBootStart.Items.Text := '0x7000' + #13#10 + '0x7800' + #13#10 + '0x7C00' + #13#10 + '0x7E00';
        end
        else
        begin
          if CompareText(cbbMCU.Text, 'AT90CAN32') = 0 then
            cbbBootStart.Items.Text := '0x3000' + #13#10 + '0x3800' + #13#10 + '0x3C00' + #13#10 + '0x3E00'
          else
            cbbBootStart.Items.Text := '0x3800' + #13#10 + '0x3C00' + #13#10 + '0x3E00' + #13#10 + '0x3F00';
        end;
      end;
    4:
      begin
        if cbbByteWord.ItemIndex = 0 then
        begin
          cbbBootStart.Items.Text := '0x9000' + #13#10 + '0x9800' + #13#10 + '0x9C00' + #13#10 + '0x9E00';
        end
        else
        begin
          cbbBootStart.Items.Text := '0x4800' + #13#10 + '0x4C00' + #13#10 + '0x4E00' + #13#10 + '0x4F00';
        end;
      end;
    5:
      begin
        if cbbByteWord.ItemIndex = 0 then
        begin
          cbbBootStart.Items.Text := '0xE000' + #13#10 + '0xF000' + #13#10 + '0xF800' + #13#10 + '0xFC00';
        end
        else
        begin
          cbbBootStart.Items.Text := '0x7000' + #13#10 + '0x7800' + #13#10 + '0x7C00' + #13#10 + '0x7E00';
        end;
      end;
    6:
      begin
        if cbbByteWord.ItemIndex = 0 then
        begin
          cbbBootStart.Items.Text := '0x1E000' + #13#10 + '0x1F000' + #13#10 + '0x1F800' + #13#10 + '0x1FC00';
        end
        else
        begin
          cbbBootStart.Items.Text := '0xF000' + #13#10 + '0xF800' + #13#10 + '0xFC00' + #13#10 + '0xFE00';
        end;
      end;
    7:
      begin
        if cbbByteWord.ItemIndex = 0 then
        begin
          cbbBootStart.Items.Text := '0x3E000' + #13#10 + '0x3F000' + #13#10 + '0x3F800' + #13#10 + '0x3FC00';
        end
        else
        begin
          cbbBootStart.Items.Text := '0x1F000' + #13#10 + '0x1F800' + #13#10 + '0x1FC00' + #13#10 + '0x1FE00';
        end;
      end;
  else
    if cbbByteWord.ItemIndex = 0 then
    begin
      cbbBootStart.Items.Text := '0x1800' + #13#10 + '0x1C00' + #13#10 + '0x1E00' + #13#10 + '0x1F00' + #13#10 +
        '0x3800' + #13#10 + '0x3C00' + #13#10 + '0x3E00' + #13#10 + '0x3F00' + #13#10 +
        '0x6000' + #13#10 + '0x7000' + #13#10 + '0x7800' + #13#10 + '0x7C00' + #13#10 + '0x7E00' + #13#10 +
        '0x9000' + #13#10 + '0x9800' + #13#10 + '0x9C00' + #13#10 + '0x9E00' + #13#10 +
        '0xE000' + #13#10 + '0xF000' + #13#10 + '0xF800' + #13#10 + '0xFC00' + #13#10 +
        '0x1E000' + #13#10 + '0x1F000' + #13#10 + '0x1F800' + #13#10 + '0x1FC00' + #13#10 +
        '0x3E000' + #13#10 + '0x3F000' + #13#10 + '0x3F800' + #13#10 + '0x3FC00';
    end
    else
    begin
      cbbBootStart.Items.Text := '0x0C00' + #13#10 + '0x0E00' + #13#10 + '0x0F00' + #13#10 + '0x0F80' + #13#10 +
        '0x1C00' + #13#10 + '0x1E00' + #13#10 + '0x1F00' + #13#10 + '0x1F80' + #13#10 +
        '0x3000' + #13#10 + '0x3800' + #13#10 + '0x3C00' + #13#10 + '0x3E00' + #13#10 + '0x3F00' + #13#10 +
        '0x4800' + #13#10 + '0x4C00' + #13#10 + '0x4E00' + #13#10 + '0x4F00' + #13#10 +
        '0x7000' + #13#10 + '0x7800' + #13#10 + '0x7C00' + #13#10 + '0x7E00' + #13#10 +
        '0xF000' + #13#10 + '0xF800' + #13#10 + '0xFC00' + #13#10 + '0xFE00' + #13#10 +
        '0x1F000' + #13#10 + '0x1F800' + #13#10 + '0x1FC00' + #13#10 + '0x1FE00';
    end;
  end;
  cbbBootStart.ItemIndex := 0;
end;

function TfrmAutoCode.getAVRType: Integer;
begin
  Result := 0; //unknow

  //8K: 10
  if (CompareText(cbbMCU.Text, 'ATmega8') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega88') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega88P') = 0) or
    (CompareText(cbbMCU.Text, 'AT90PWM2') = 0) or
    (CompareText(cbbMCU.Text, 'AT90PWM2B') = 0) or
    (CompareText(cbbMCU.Text, 'AT90PWM3') = 0) or
    (CompareText(cbbMCU.Text, 'AT90PWM324') = 0) or
    (CompareText(cbbMCU.Text, 'AT90PWM3B') = 0) or
    (CompareText(cbbMCU.Text, 'ATMega8515') = 0) or
    (CompareText(cbbMCU.Text, 'ATMega8535') = 0)
    then
  begin
    Result := 1;
    Exit;
  end;

  //16K: 14
  if (CompareText(cbbMCU.Text, 'ATmega16') = 0) or
    (CompareText(cbbMCU.Text, 'AT90PWM216') = 0) or
    (CompareText(cbbMCU.Text, 'AT90PWM316') = 0) or
    (CompareText(cbbMCU.Text, 'ATMega162') = 0) or
    (CompareText(cbbMCU.Text, 'ATMega163') = 0) or
    (CompareText(cbbMCU.Text, 'ATMega164P') = 0) or
    (CompareText(cbbMCU.Text, 'ATMega165') = 0) or
    (CompareText(cbbMCU.Text, 'ATMega165P') = 0) or
    (CompareText(cbbMCU.Text, 'ATMega168') = 0) or
    (CompareText(cbbMCU.Text, 'ATMega168P') = 0) or
    (CompareText(cbbMCU.Text, 'ATMega169') = 0) or
    (CompareText(cbbMCU.Text, 'ATMega169P') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega328P') = 0)
    then
  begin
    Result := 2;
    Exit;
  end;

  //32K: 12
  if (CompareText(cbbMCU.Text, 'ATmega32') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega323') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega324P') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega325') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega3250') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega3250P') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega329') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega3290') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega3290P') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega329P') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega32HVB') = 0) or
    (CompareText(cbbMCU.Text, 'AT90CAN32') = 0)
    then
  begin
    Result := 3;
    Exit;
  end;

  //40K: 1
  if (CompareText(cbbMCU.Text, 'ATmega406') = 0)
    then
  begin
    Result := 4;
    Exit;
  end;

  //64K: 9
  if (CompareText(cbbMCU.Text, 'ATmega64') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega640') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega644') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega644P') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega645') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega6450') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega649') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega6490') = 0) or
    (CompareText(cbbMCU.Text, 'AT90CAN64') = 0)
    then
  begin
    Result := 5;
    Exit;
  end;

  //128K: 5
  if (CompareText(cbbMCU.Text, 'ATmega128') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega1280') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega1281') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega1284P') = 0) or
    (CompareText(cbbMCU.Text, 'AT90CAN128') = 0) then
  begin
    Result := 6;
    Exit;
  end;

  //256K: 2
  if (CompareText(cbbMCU.Text, 'ATmega2560') = 0) or
    (CompareText(cbbMCU.Text, 'ATmega2561') = 0)
    then
  begin
    Result := 7;
    Exit;
  end;
end;

procedure TfrmAutoCode.cbbTimeClkChange(Sender: TObject);
begin
  try
    Timer.Interval := StrToInt(cbbTimeClk.Text);
    if Timer.Interval < 100 then
      Timer.Interval := 100;
  except
    Timer.Interval := 200;
  end;
end;

procedure TfrmAutoCode.FormCreate(Sender: TObject);
begin
  edtEncryptKey.Text := 'D0 94 3F 8C 29 76 15 D8 20 40 E3 27 45 D8 48 AD EA 8B 2A 73 16 E9 B0 49 45 B3 39 28 0A C3 28 3C';
  edtDecryptKey.Text := 'D0 94 3F 8C 29 76 15 D8 20 40 E3 27 45 D8 48 AD EA 8B 2A 73 16 E9 B0 49 45 B3 39 28 0A C3 28 3C';
end;

procedure TfrmAutoCode.btnRandomKeyClick(Sender: TObject);
var
  i: Integer;
  s: string;
begin
  Randomize;
  s := '';
  for i := 0 to 31 do
  begin
    s := s + IntToHex(Random(256), 2) + ' ';
  end;
  edtEncryptKey.Text := s;
  edtDecryptKey.Text := s;
end;

end.

