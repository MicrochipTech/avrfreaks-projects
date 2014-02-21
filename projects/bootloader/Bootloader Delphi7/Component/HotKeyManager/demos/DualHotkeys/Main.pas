unit Main;

interface

uses
  Windows, Messages, SysUtils, Classes, Controls, Forms, Dialogs, StdCtrls,
  ComCtrls, HotKeyManager;

type
  TMainForm = class(TForm)
    HotKey1: THotKey;
    HotKey2: THotKey;
    Label1: TLabel;
    Label2: TLabel;
    Button1: TButton;
    Button2: TButton;
    Button3: TButton;
    HotKeyManager1: THotKeyManager;
    Label3: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure HotKeyManager1HotKeyPressed(HotKey: Cardinal; Index: Word);
  private
    HotKeyIndex1, HotKeyIndex2: Word;
  end;

var
  MainForm: TMainForm;

implementation

{$R *.dfm}

const
  LOCALIZED_KEYNAMES = True;

procedure TMainForm.FormCreate(Sender: TObject);
begin
  HotKeyIndex1 := HotKeyManager1.AddHotKey(HotKey1.HotKey);
  HotKeyIndex2 := HotKeyManager1.AddHotKey(HotKey2.HotKey);
end;


procedure TMainForm.Button1Click(Sender: TObject);
begin
  HotKeyIndex1 := HotKeyManager1.ChangeHotKey(HotKeyIndex1, HotKey1.HotKey);
end;


procedure TMainForm.Button2Click(Sender: TObject);
begin
  HotKeyIndex2 := HotKeyManager1.ChangeHotKey(HotKeyIndex2, HotKey2.HotKey);
end;


procedure TMainForm.Button3Click(Sender: TObject);
begin
  Close;
end;


procedure TMainForm.HotKeyManager1HotKeyPressed(HotKey: Cardinal; Index: Word);
begin
  SetForegroundWindow(Application.Handle);
  if Index = HotKeyIndex1 then
    ShowMessage('Hotkey 1 (' + HotKeyToText(HotKey, LOCALIZED_KEYNAMES) + ') was pressed.')
  else if Index = HotKeyIndex2 then
    ShowMessage('Hotkey 2 (' + HotKeyToText(HotKey, LOCALIZED_KEYNAMES) + ') was pressed.')
  else
    ShowMessage('Unknown hotkey was pressed. This should not happen.');
end;

end.

