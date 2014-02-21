unit HexEditReg;

{$I Ver.inc}

interface

uses
  Windows, Messages, SysUtils, Classes, Controls, Forms, Dialogs, Graphics;

procedure Register;

implementation

uses
  HexEdit,
{$IFDEF VER_COMPILER_6_UP}
  DesignIntf, DesignEditors;
{$ELSE}
  DsgnIntf;
{$ENDIF}

type

{ Property Editor }

  THexEditFontProperty = class(TClassProperty)
  public
    function GetAttributes: TPropertyAttributes; override;
    procedure Edit; override;
  end;

procedure Register;
begin
  RegisterComponents('DayDream', [THexEdit]);
  RegisterPropertyEditor(TypeInfo(TFont), THexEdit, 'Font', THexEditFontProperty);
end;

{ THexEditFontProperty }

function THexEditFontProperty.GetAttributes: TPropertyAttributes;
begin
  Result := [paDialog, paReadOnly];
end;

procedure THexEditFontProperty.Edit;
var
  FontDialog: TFontDialog;
  AHexEdit: THexEdit;
begin
  AHexEdit := GetComponent(0) as THexEdit;
  FontDialog := TFontDialog.Create(Application);
  try
    FontDialog.Font := AHexEdit.Font;
    FontDialog.Options := FontDialog.Options + [fdFixedPitchOnly];
    if FontDialog.Execute then
    begin
      AHexEdit.Font := FontDialog.Font;
      AHexEdit.Invalidate;
      Modified;
    end;
  finally
    FontDialog.Free;
  end;
end;

end.
