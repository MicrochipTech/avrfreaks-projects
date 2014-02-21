unit HotKeyUnit;

interface

uses
  Classes;

procedure InitDll;


implementation

uses
  SysUtils, HotKeyManager, Windows, Dialogs;

type
  TContainer = class    // A dummy class that allows us to use event methods
    hkm: THotKeyManager;
    procedure HotKeyPressed(HotKey: Cardinal; Index: Word);
  end;

var
  Container: TContainer;

procedure TContainer.HotKeyPressed(HotKey: Cardinal; Index: Word);
begin
  ShowMessage(HotKeyToText(HotKey, True) + ' pressed.');
end;

procedure InitDll;
var
  HotKey: Cardinal;
  HotKeyIndex: Word;
begin
  Container := TContainer.Create;
  Container.hkm := THotKeyManager.Create(nil);
  Container.hkm.OnHotKeyPressed := Container.HotKeyPressed;
  HotKey := GetHotKey(MOD_ALT, VK_HOME);
  if HotKey = 0 then
    ShowMessage('Invalid hotkey.');
  HotKeyIndex := Container.hkm.AddHotKey(HotKey);
  if HotKeyIndex = 0 then
    ShowMessage('Could not register hotkey.');
end;


initialization

finalization
  if Container <> nil then
  begin
    Container.hkm.Free;
    Container.Free;
  end;
end.

