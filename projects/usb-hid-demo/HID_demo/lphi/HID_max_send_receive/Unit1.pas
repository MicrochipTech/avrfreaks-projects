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
    StringGrid1: TStringGrid;
    Memo1: TMemo;
    ListBox1: TListBox;
    Button2: TButton;
    Button4: TButton;
    Button5: TButton;
    function HidCtlEnumerate(HidDev: TJvHidDevice;
      const Idx: Integer): Boolean;
    procedure HidCtlDeviceChange(Sender: TObject);
    procedure StringGrid1KeyPress(Sender: TObject; var Key: Char);
    procedure StringGrid1SelectCell(Sender: TObject; ACol, ARow: Integer;
      var CanSelect: Boolean);
    procedure FormActivate(Sender: TObject);
    procedure ListBox1DblClick(Sender: TObject);
    procedure ComboBox1Select(Sender: TObject);
    procedure HidCtlDeviceDataError(HidDev: TJvHidDevice; Error: Cardinal);
    procedure HidCtlArrival(HidDev: TJvHidDevice);
    procedure HidCtlRemoval(HidDev: TJvHidDevice);
    procedure Button2Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
    procedure Button5Click(Sender: TObject);
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


procedure TForm1.StringGrid1SelectCell(Sender: TObject; ACol,ARow: Integer; var CanSelect: Boolean);begin AC:=ACol;RC:=ARow;end;
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

procedure TForm1.StringGrid1KeyPress(Sender: TObject; var Key: Char);
begin
Key:=UpCase(Key);
if (Key in ['A','B','C','D','E','F','0'..'9',char(VK_BACK)]) then
  Begin
   if (Length(StringGrid1.Cells[AC,RC]) > 1) then if (Key<>char(VK_BACK)) then Key:=char(0);
  end
else Key:=char(0);
end;


procedure TForm1.FormActivate(Sender: TObject);
var
i,c:byte;
begin
StringGrid1.Cells[0,0]:='HEX';
for i:=1 to 9 do StringGrid1.Cells[i,0]:=inttostr(i-1);c:=0;
for i:=1 to 9 do begin StringGrid1.Cells[0,i]:=inttostr(c);c:=c+8;end;
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
  Button2.Enabled := false; // GetHex
  Button4.Enabled := false; // SetHex
  Button5.Enabled := false; // SetStr

  if (ComboBox1.Items.Count > 0) and (ComboBox1.ItemIndex >= 0) then
  begin
    Dev := TJvHidDevice(ComboBox1.Items.Objects[ComboBox1.ItemIndex]);
    // start reader thread
    Dev.OnData := ShowRead;
    CurrentDevice := Dev;
    Button4.Enabled := (Dev.Caps.InputReportByteLength <> 0) and Dev.HasReadWriteAccess;
    Button5.Enabled := (Dev.Caps.InputReportByteLength <> 0) and Dev.HasReadWriteAccess;
    Button2.Enabled := (Dev.Caps.OutputReportByteLength <> 0) and Dev.HasReadWriteAccess;
    ListBox1.Items.Append('Select of:'+ DeviceName(CurrentDevice)+ ' 0x'+ InttoHex(Dev.Attributes.VendorID,4) + ' 0x' + IntToHex(Dev.Attributes.ProductID,4));
  end;
end;

procedure TForm1.ShowRead(HidDev: TJvHidDevice; ReportID: Byte; const Data: Pointer; Size: Word);
var
  temp:string;
  b,c:byte;
  Buf:array [0..64] of Byte;
begin
  move(Data^,Buf[0],Size);

     temp:='';
      Memo1.Lines.Clear;
     for b:=1 to 8 do
      begin
      for c:= 1 to 8 do
        begin
        StringGrid1.Cells[c,b] := IntToHex(Buf[((b-1)*8)+(c-1)],2);
        if (Buf[((b-1)*8)+(c-1)] > 31) and (Buf[((b-1)*8)+(c-1)] < 127) then temp:= temp+ char(Buf[((b-1)*8)+(c-1)]) else temp:=temp+'*';
        end;
      end;
    Memo1.Lines.Append(temp);

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

procedure TForm1.Button2Click(Sender: TObject);
var
  Buf: array [0..64] of Byte;
  Err: DWORD;
  c,b:byte;
  temp:string;

begin
  if Assigned(CurrentDevice) then
  begin
    FillChar(Buf[0], SizeOf(Buf), 0);
    Buf[0] := 0;
    if CurrentDevice.GetInputReport(Buf[0], SizeOf(Buf)) then
    begin
     ListBox1.Items.Append('Required data from devie');
     temp:='';
      Memo1.Lines.Clear;
     for b:=1 to 8 do
      begin
      for c:= 1 to 8 do
        begin
        StringGrid1.Cells[c,b] := IntToHex(Buf[(((b-1)*8)+(c-1))+1],2);
        if (Buf[(((b-1)*8)+(c-1))+1] > 31) and (Buf[(((b-1)*8)+(c-1))+1] < 127) then temp:= temp+ char(Buf[(((b-1)*8)+(c-1))+1]) else temp:=temp+'*';
        end;
      end;
    Memo1.Lines.Append(temp);
    end
    else
    begin
      Err := GetLastError;
      ListBox1.Items.Append(Format('GET REPORT ERROR: %s (%x)', [SysErrorMessage(Err), Err]));
    end;
  end;
end;

procedure TForm1.Button4Click(Sender: TObject);
var
  Buf: array [0..64] of Byte;
  Err: DWORD;
  temp:string;
  b,c:byte;
begin

  for b:=1 to 8 do begin
      for c:= 1 to 8 do begin
      temp := StringGrid1.Cells[c,b]; if (Length(temp) > 0) then Buf[(((b-1)*8)+(c-1))+1]:= strtoint('$'+temp) else Buf[(((b-1)*8)+(c-1))+1]:= strtoint('$00'); end;
    end;

  if Assigned(CurrentDevice) then
  begin
    Buf[0] := 0;
    if not CurrentDevice.SetOutputReport(Buf[0], CurrentDevice.Caps.OutputReportByteLength) then
    begin
      Err := GetLastError;
      ListBox1.Items.Append(Format('SET REPORT ERROR: %s (%x)', [SysErrorMessage(Err), Err]));
    end
    else
    begin
      ListBox1.Items.Append('Data are send');
    end;
  end;
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
    end;
  end;
end;

end.
