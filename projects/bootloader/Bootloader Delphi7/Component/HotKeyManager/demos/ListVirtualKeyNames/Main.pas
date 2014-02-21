unit Main;

interface

uses
  Windows, Messages, SysUtils, Classes, Controls, Forms, StdCtrls, HotKeyManager;

type
  TForm1 = class(TForm)
    ListBox1: TListBox;
    Button1: TButton;
    procedure FormCreate(Sender: TObject);
    procedure Button1Click(Sender: TObject);
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}

procedure TForm1.FormCreate(Sender: TObject);
var
  I: Integer;
begin
  Screen.Cursor := crHourGlass;
  for I := $08 to $FF do
    ListBox1.Items.Add(IntToStr(I) + '  -  $' + IntToHex(I, 2) + '  -  ' +
                       HotKeyToText(I, True) + '  -  ' + HotKeyToText(I, False));
  Screen.Cursor := crDefault;
end;

procedure TForm1.Button1Click(Sender: TObject);
begin
  Close;
end;

end.

