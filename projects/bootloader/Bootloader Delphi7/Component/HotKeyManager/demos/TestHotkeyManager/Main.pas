unit Main;

interface

uses
  Windows, Messages, SysUtils, Classes, Controls, Forms, Dialogs, StdCtrls,
  ComCtrls, HotKeyManager, CoolTrayIcon;

type
  TMainForm = class(TForm)
    Label1: TLabel;
    GroupBox1: TGroupBox;
    GroupBox2: TGroupBox;
    GroupBox3: TGroupBox;
    HotKey1: THotKey;
    BtnAdd: TButton;
    BtnGetHotKey: TButton;
    BtnTextToHotKey: TButton;
    Edit1: TEdit;
    GroupBox4: TGroupBox;
    BtnClear: TButton;
    ListBox1: TListBox;
    BtnRemove: TButton;
    BtnExit: TButton;
    CheckBox1: TCheckBox;
    CheckBox2: TCheckBox;
    CheckBox3: TCheckBox;
    CheckBox4: TCheckBox;
    Label2: TLabel;
    BtnTest: TButton;
    HotKeyManager1: THotKeyManager;
    ComboBox1: TComboBox;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure BtnAddClick(Sender: TObject);
    procedure BtnGetHotKeyClick(Sender: TObject);
    procedure BtnTextToHotKeyClick(Sender: TObject);
    procedure BtnTestClick(Sender: TObject);
    procedure BtnRemoveClick(Sender: TObject);
    procedure BtnClearClick(Sender: TObject);
    procedure BtnExitClick(Sender: TObject);
    procedure HotKeyManager1HotKeyPressed(HotKey: Cardinal; Index: Word);
  private
    procedure AddHotKey(HotKey: Cardinal);
    procedure GetPotentialKeys;
  end;

var
  MainForm: TMainForm;
  ToShow: Boolean = False;

implementation

{$R *.DFM}

const
  LOCALIZED_KEYNAMES = True;

type
  THotKeyEntry = class
    HotKey: Cardinal;
    constructor Create(iHotKey: Cardinal);
  end;

  TPotentialKey = class
    Key: Word;
    constructor Create(iKey: Word);
  end;

constructor THotKeyEntry.Create(iHotKey: Cardinal);
begin
  inherited Create;
  HotKey := iHotKey;
end;

constructor TPotentialKey.Create(iKey: Word);
begin
  inherited Create;
  Key := iKey;
end;

{--------------------- TMainForm ----------------------}

procedure TMainForm.AddHotKey(HotKey: Cardinal);
begin
  if HotKeyManager1.AddHotKey(HotKey) <> 0 then
  begin
    ListBox1.Items.AddObject(HotKeyToText(HotKey, LOCALIZED_KEYNAMES), THotKeyEntry.Create(HotKey));
    HotKey1.HotKey := 0;     // Just a nice touch
  end
  else
    MessageDlg(HotKeyToText(HotKey, LOCALIZED_KEYNAMES) + ' couldn''t be assigned to a hotkey.',
               mtWarning, [mbOk], 0);
end;


procedure TMainForm.FormCreate(Sender: TObject);
begin
  GetPotentialKeys;
end;


procedure TMainForm.FormDestroy(Sender: TObject);
var
  I: Integer;
begin
  for I := ComboBox1.Items.Count -1 downto 0 do
    ComboBox1.Items.Objects[I].Free;
end;


procedure TMainForm.BtnAddClick(Sender: TObject);
var
  HotKeyVar: Cardinal;
begin
  HotKeyVar := HotKey1.HotKey;
  if HotKeyVar = 0 then
    MessageDlg('No hotkey specified.', mtWarning, [mbOk], 0)
  else
    AddHotKey(HotKeyVar);
end;


procedure TMainForm.BtnGetHotKeyClick(Sender: TObject);
var
  HotKeyVar: Cardinal;
  Modifiers: Word;
  PotentialKey: TPotentialKey;
begin
  Modifiers := 0;
  if CheckBox1.Checked then
    Modifiers := Modifiers or MOD_CONTROL;
  if CheckBox2.Checked then
    Modifiers := Modifiers or MOD_SHIFT;
  if CheckBox3.Checked then
    Modifiers := Modifiers or MOD_ALT;
  if CheckBox4.Checked then
    Modifiers := Modifiers or MOD_WIN;
  if ComboBox1.ItemIndex <> -1 then
  begin
    PotentialKey := (ComboBox1.Items.Objects[ComboBox1.ItemIndex] as TPotentialKey);
    HotKeyVar := GetHotKey(Modifiers, PotentialKey.Key);
    AddHotKey(HotKeyVar);
  end
  else
    MessageDlg('No key selected from the list.', mtWarning, [mbOk], 0);
end;


procedure TMainForm.BtnTextToHotKeyClick(Sender: TObject);
var
  HotKeyVar: Cardinal;
begin
  HotKeyVar := TextToHotKey(Edit1.Text, LOCALIZED_KEYNAMES);
  if HotKeyVar <> 0 then
    AddHotKey(HotKeyVar)
  else
    MessageDlg(Edit1.Text + ' doesn''t appear to be a hotkey.', mtWarning, [mbOk], 0);
end;


procedure TMainForm.BtnTestClick(Sender: TObject);
var
  HotKeyVar: Cardinal;
  S1: String;
begin
  HotKeyVar := TextToHotKey(Edit1.Text, LOCALIZED_KEYNAMES);
  if HotKeyVar <> 0 then
  begin
    S1 := '';
    if not HotKeyAvailable(HotKeyVar) then
      S1 := 'NOT ';
    MessageDlg(HotKeyToText(HotKeyVar, LOCALIZED_KEYNAMES) + ' is ' + S1 +
               'available for registration.', mtInformation, [mbOk], 0);
  end
  else
    MessageDlg(Edit1.Text + ' doesn''t appear to be a hotkey.', mtWarning, [mbOk], 0);
end;


procedure TMainForm.BtnRemoveClick(Sender: TObject);
var
  HotKeyEntry: THotKeyEntry;
begin
  if ListBox1.ItemIndex > -1 then
  begin
    HotKeyEntry := (ListBox1.Items.Objects[ListBox1.ItemIndex] as THotKeyEntry);
    if HotKeyManager1.RemoveHotKey(HotKeyEntry.HotKey) then
    begin
      HotKeyEntry.Free;
      ListBox1.Items.Delete(ListBox1.ItemIndex);
    end
    else
      MessageDlg(HotKeyToText(HotKeyEntry.HotKey, LOCALIZED_KEYNAMES) +
                 ' couldn''t be removed.', mtWarning, [mbOk], 0);
  end;
end;


procedure TMainForm.BtnClearClick(Sender: TObject);
var
  I: Integer;
begin
  HotKeyManager1.ClearHotKeys;
  HotKey1.HotKey := 0;
  for I := 0 to ListBox1.Items.Count -1 do
    (ListBox1.Items.Objects[I] as THotKeyEntry).Free;
  ListBox1.Items.Clear;
end;


procedure TMainForm.BtnExitClick(Sender: TObject);
begin
  Close;
end;

procedure ShowDesktop(const YesNo : boolean) ;
var
  h : THandle;
  rc: boolean;
begin
  h := FindWindow('ProgMan', nil) ;
  h := GetWindow(h, GW_CHILD) ;
  if YesNo = True then
    rc := ShowWindow(h, SW_SHOW)
  else
    rc := ShowWindow(h, SW_HIDE) ;
  if rc then
    messagebeep(0);

end;


procedure TMainForm.HotKeyManager1HotKeyPressed(HotKey: Cardinal; Index: Word);
begin
  SetForegroundWindow(Application.Handle);
  MessageDlg('Hotkey ' + HotKeyToText(HotKey, LOCALIZED_KEYNAMES) + ' pressed.', mtInformation, [mbOk], 0);
end;


procedure TMainForm.GetPotentialKeys;

  procedure AddKeys(Min, Max: Word);
  var
    I: Integer;
    KeyName: String;
  begin
    for I := Min to Max do
    begin
      KeyName := HotKeyToText(I, LOCALIZED_KEYNAMES);
      if KeyName <> '' then
        ComboBox1.Items.AddObject(KeyName, TPotentialKey.Create(I));
    end;
  end;

begin
  // Add standard keys
  AddKeys($08, $09);
  AddKeys($0D, $0D);
  AddKeys($14, $91);
  AddKeys($BA, $FF);
  // Add extended keys
  AddKeys(_VK_BROWSER_BACK, _VK_LAUNCH_APP2);
  if ComboBox1.Items.Count > 0 then
    ComboBox1.ItemIndex := 0;
end;

end.

