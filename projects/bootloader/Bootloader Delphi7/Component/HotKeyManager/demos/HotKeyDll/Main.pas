unit Main;

interface

uses
  Windows, Messages, Classes, Controls, Forms, StdCtrls;

type
  TForm1 = class(TForm)
    Button1: TButton;
    Label1: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure Button1Click(Sender: TObject);
  end;

const
  HotKeyDll = 'hotkeydll.dll';

var
  Form1: TForm1;

procedure InitDll; external HotKeyDll;

implementation

{$R *.dfm}

procedure TForm1.FormCreate(Sender: TObject);
begin
  InitDll;
end;

procedure TForm1.Button1Click(Sender: TObject);
begin
  Close;
end;

end.

