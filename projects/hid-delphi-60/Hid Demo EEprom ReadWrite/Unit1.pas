unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, JvHidControllerClass, StdCtrls;

type
  TForm1 = class(TForm)
    Button1: TButton;
    Button2: TButton;
    Edit1: TEdit;
    JvHidDeviceController1: TJvHidDeviceController;
    ListBox1: TListBox;
    procedure Button2Click(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure JvHidDeviceController1DeviceData(HidDev: TJvHidDevice;
      ReportID: Byte; const Data: Pointer; Size: Word);
    procedure JvHidDeviceController1Arrival(HidDev: TJvHidDevice);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;
  Dev: TJvHidDevice;
  Raw:array[0..199] of byte;
  Devs:array[0..63] of TJvHidDevice;
  res:boolean;

implementation

{$R *.dfm}

{
 if (HidDev.Attributes.VendorID=$16C0) and
    (HidDev.Attributes.ProductID=$05DF) and
    (HidDev.VendorName = 'www.cortexbs.sk') and
    (HidDev.ProductName = 'Loader-3G &@X') and
    (HidDev.Attributes.VersionNumber = $B0A0) then

}
procedure ClearDev;
var i:byte;
begin
for i:=0 to 63 do Devs[i]:=nil;
end;

procedure TForm1.Button2Click(Sender: TObject);
var   i:string;
  b:integer;
begin
 if Dev=nil then exit;
 fillchar(Raw,length(RAW),0);
 Raw[0]:=$77;  //ReportID must be 0 !!!

 i:= Edit1.Text;
  if length(i)>1 then
  begin
 for b:=1 to length(i)-1 do
  Begin
   Raw[b]:= ord(i[b]);
  end;
 end;

 res:=Dev.SetFeature(Raw,200);
 if not res then ShowMessage(SysErrorMessage(GetLastError))
 else ListBox1.Items.Append('Send data');
end;

procedure TForm1.Button1Click(Sender: TObject);
var   i:string;
      b:integer;
begin
 if Dev=nil then exit;
 fillchar(Raw,length(RAW),0);
 Raw[0]:=$77; //ReportID must be 0 !!!
 res:=Dev.GetFeature(Raw,200);
 if not res then ShowMessage(SysErrorMessage(GetLastError))
 else
 begin
 ListBox1.Items.Append('Read data');
  for b:=1 to 199 do
  Begin
   i:=i+ char(Raw[b]);
  end;
  ListBox1.Items.Append(i);
 end;
end;

procedure TForm1.JvHidDeviceController1DeviceData(HidDev: TJvHidDevice;
  ReportID: Byte; const Data: Pointer; Size: Word);
var tSize:DWORD;
    tReportID:DWORD;
begin
tSize:=Size;
tReportID:=ReportID;
showmessage('receive size:'+inttostr(tsize)+' reportID:'+inttostr(tReportID));
end;

procedure TForm1.JvHidDeviceController1Arrival(HidDev: TJvHidDevice);
begin
 Dev:=nil;
 ListBox1.Items.Append( format('%s %4X/%4X',[HidDev.ProductName, HidDev.Attributes.VendorID,HidDev.Attributes.ProductID]) );
 if (HidDev.Attributes.VendorID=$16C0) and (HidDev.Attributes.ProductID=$05DF) then begin
  Dev:=HidDev;
  Dev.CheckOut;
end;

end;

end.
