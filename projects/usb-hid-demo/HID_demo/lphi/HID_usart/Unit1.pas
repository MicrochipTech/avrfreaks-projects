unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, JvHidControllerClass, Grids;


type

  TReport = packed record
    ReportID: Byte;
    Bytes: array [0..63] of Byte;
  end;


  TForm1 = class(TForm)
    HidCtl: TJvHidDeviceController;
    ComboBox1: TComboBox;
    Memo1: TMemo;
    ListBox1: TListBox;
    Button5: TButton;
    Memo2: TMemo;
    CheckBox1: TCheckBox;
    function HidCtlEnumerate(HidDev: TJvHidDevice;
      const Idx: Integer): Boolean;
    procedure HidCtlDeviceChange(Sender: TObject);
    procedure ListBox1DblClick(Sender: TObject);
    procedure ComboBox1Select(Sender: TObject);
    procedure HidCtlDeviceDataError(HidDev: TJvHidDevice; Error: Cardinal);
    procedure HidCtlArrival(HidDev: TJvHidDevice);
    procedure HidCtlRemoval(HidDev: TJvHidDevice);
    procedure Button5Click(Sender: TObject);
    procedure Memo2DblClick(Sender: TObject);
    procedure Memo1KeyPress(Sender: TObject; var Key: Char);
  private
    { Private declarations }
  public
    { Public declarations }
    Activated: Boolean;
    Edits: array [0..63] of TEdit;
    CurrentDevice: TJvHidDevice;
    function DeviceName(HidDev: TJvHidDevice): string;
    procedure ShowRead(HidDev: TJvHidDevice; ReportID: Byte; const Data: Pointer; Size: Word);

  end;

var
  Form1: TForm1;
  AC,RC:byte;
  Buff: array [0..63] of Byte;

implementation

{$R *.dfm}
{
var
  i,b:byte;
  temp:string;
begin
  for b:=1 to 8 do begin
      for i:= 1 to 8 do begin
      temp := StringGrid1.Cells[i,b]; if (Length(temp) > 0) then Buff[((b-1)*8)+(i-1)]:= strtoint('$'+temp) else Buff[((b-1)*8)+(i-1)]:= strtoint('$00');        end;
    end;
}


function TForm1.DeviceName(HidDev: TJvHidDevice): string;
begin
  if HidDev.ProductName <> '' then
    Result := HidDev.ProductName
  else
    Result := Format('Device VID=%.4x PID=%.4x',
      [HidDev.Attributes.VendorID, HidDev.Attributes.ProductID]);
  if HidDev.SerialNumber <> '' then
    Result := Result + Format(' (Serial=%s)', [HidDev.SerialNumber]);
end;

function TForm1.HidCtlEnumerate(HidDev: TJvHidDevice;const Idx: Integer): Boolean;
var
  N: Integer;
  Dev: TJvHidDevice;
begin
  N := ComboBox1.Items.Add(DeviceName(HidDev));
  HidCtl.CheckOutByIndex(Dev, Idx);
  Dev.NumInputBuffers := 64;
  Dev.NumOverlappedBuffers := 64;
  ComboBox1.Items.Objects[N] := Dev;
  Result := True;
end;

procedure TForm1.HidCtlDeviceChange(Sender: TObject);
var
  Dev: TJvHidDevice;
  I, N: Integer;
begin
  for I := 0 to ComboBox1.Items.Count - 1 do
  begin
    Dev := TJvHidDevice(ComboBox1.Items.Objects[I]);
    Dev.Free;
  end;
  ComboBox1.Items.Clear;

  while HidCtl.CheckOut(Dev) do
  begin
    N := ComboBox1.Items.Add(DeviceName(Dev));
    Dev.NumInputBuffers := 64;
    Dev.NumOverlappedBuffers := 64;
    ComboBox1.Items.Objects[N] := Dev;
  end;
end;

procedure TForm1.ListBox1DblClick(Sender: TObject);
begin
ListBox1.Items.Clear;
end;

procedure TForm1.ComboBox1Select(Sender: TObject);
var
  Dev: TJvHidDevice;
begin
  // stop reader thread
  if Assigned(CurrentDevice) then CurrentDevice.OnData := nil;
  CurrentDevice := nil;
  Button5.Enabled := false; // SetStr

  if (ComboBox1.Items.Count > 0) and (ComboBox1.ItemIndex >= 0) then
  begin
    Dev := TJvHidDevice(ComboBox1.Items.Objects[ComboBox1.ItemIndex]);
    // start reader thread
    Dev.OnData := ShowRead;
    CurrentDevice := Dev;
    Button5.Enabled := (Dev.Caps.InputReportByteLength <> 0) and Dev.HasReadWriteAccess;
    ListBox1.Items.Append('Select of:'+ DeviceName(CurrentDevice)+ ' 0x'+ InttoHex(Dev.Attributes.VendorID,4) + ' 0x' + IntToHex(Dev.Attributes.ProductID,4));
  end;
end;

procedure TForm1.ShowRead(HidDev: TJvHidDevice; ReportID: Byte; const Data: Pointer; Size: Word);
var
  temp:string;
  b:byte;
  Buf:array [0..64] of Byte;
begin
  move(Data^,Buf[0],Size);

     temp:='';
     for b:=0 to 63 do
      begin
       if (Buf[b] > 31) and (Buf[b] < 127) or (Buf[b] = 13) or (Buf[b] = 10) then temp:= temp+ char(Buf[b]);
      end;
    Memo2.Lines.Text := Memo2.Lines.Text + temp;
  SendMessage(Memo2.Handle, EM_LINESCROLL, 0,Memo2.Lines.Count);
  ListBox1.Items.Append('Read data from device');
end;


procedure TForm1.HidCtlDeviceDataError(HidDev: TJvHidDevice; Error: Cardinal);
begin
ListBox1.Items.Append(Format('READ ERROR: %s (%x)', [SysErrorMessage(Error), Error]));
end;

procedure TForm1.HidCtlArrival(HidDev: TJvHidDevice);
begin
ListBox1.Items.Append('Arrival of ' + DeviceName(HidDev));
end;

procedure TForm1.HidCtlRemoval(HidDev: TJvHidDevice);
begin
ListBox1.Items.Append('Removal of ' + DeviceName(HidDev));
end;

procedure TForm1.Button5Click(Sender: TObject);
var
  Buf: array [0..64] of Byte;
  Err: DWORD;
  temp:string;
  b:byte;
begin
  FillChar(Buf[0], SizeOf(Buf), 0);
  temp:=memo1.Lines.Text;
  if length(temp)<1 then exit;
  for b:=1 to length(temp)+1 do Buf[b]:= ord(temp[b]);
  if Assigned(CurrentDevice) then
  begin
    if not CurrentDevice.SetOutputReport(Buf[0], CurrentDevice.Caps.OutputReportByteLength) then
    begin
      Err := GetLastError;
      ListBox1.Items.Append(Format('SET REPORT ERROR: %s (%x)', [SysErrorMessage(Err), Err]));
    end
    else
    begin
      ListBox1.Items.Append('Data are send');
      memo1.Lines.Clear;
    end;
  end;
end;

procedure TForm1.Memo2DblClick(Sender: TObject);
begin
memo2.Lines.Clear;
end;

procedure TForm1.Memo1KeyPress(Sender: TObject; var Key: Char);
begin
if (Key = char(13)) then
  begin
    if checkbox1.Checked then memo1.Lines.Text:= memo1.Lines.Text+chr(13)+chr(10);
    button5.Click;
    Key:=char(0);
  end;
end;

end.
