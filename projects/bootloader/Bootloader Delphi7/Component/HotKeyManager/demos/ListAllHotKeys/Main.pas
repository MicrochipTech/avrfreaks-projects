unit Main;

interface

uses
  Windows, Messages, Classes, Controls, Forms, StdCtrls, HotKeyManager;

type
  TForm1 = class(TForm)
    ListBox1: TListBox;
    Button1: TButton;
    Button2: TButton;
    Label1: TLabel;
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
  private
    procedure GetCombination(Modifiers: Word);
    procedure ScanHotKeys;
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}

const
  LOCALIZED_KEYNAMES = True;

procedure TForm1.Button1Click(Sender: TObject);
begin
  ScanHotKeys;
end;


procedure TForm1.Button2Click(Sender: TObject);
begin
  Close;
end;


procedure TForm1.GetCombination(Modifiers: Word);
var
  I: Integer;
  HotKey: Cardinal;
begin
//  for I := $08 to $FF do
  for I := $08 to $E6 do
  begin
    HotKey := GetHotKey(Modifiers, I);
    if not HotKeyAvailable(HotKey) then
      ListBox1.Items.Add({IntToStr(I) + ' - ' +} HotKeyToText(HotKey, LOCALIZED_KEYNAMES));
  end;
end;


procedure TForm1.ScanHotKeys;
begin
  Screen.Cursor := crHourGlass;
  ListBox1.Clear;

  GetCombination(MOD_WIN);
  GetCombination(MOD_WIN + MOD_SHIFT);
  GetCombination(MOD_WIN + MOD_CONTROL);
  GetCombination(MOD_WIN + MOD_ALT);
  GetCombination(MOD_WIN + MOD_SHIFT + MOD_CONTROL);
  GetCombination(MOD_WIN + MOD_SHIFT + MOD_ALT);
  GetCombination(MOD_WIN + MOD_CONTROL + MOD_ALT);
  GetCombination(MOD_WIN + MOD_SHIFT + MOD_CONTROL + MOD_ALT);

  GetCombination(MOD_SHIFT);
  GetCombination(MOD_SHIFT + MOD_CONTROL);
  GetCombination(MOD_SHIFT + MOD_ALT);
  GetCombination(MOD_SHIFT + MOD_CONTROL + MOD_ALT);

  GetCombination(MOD_CONTROL);
  GetCombination(MOD_CONTROL + MOD_ALT);

  GetCombination(MOD_ALT);

  GetCombination(0);

  Screen.Cursor := crDefault;
end;

end.

