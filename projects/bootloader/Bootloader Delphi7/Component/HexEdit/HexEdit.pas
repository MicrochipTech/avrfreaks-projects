
{**********************************************************}
{                                                          }
{  HexEdit Component Version 1.2                           }
{                                                          }
{  Author: DayDream Studio                                 }
{  Email: haoxg@21cn.com                                   }
{  URL: http://haoxg.yeah.net                              }
{  Last Modify Date: 2003-12-25                            }
{                                                          }
{**********************************************************}

unit HexEdit;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Clipbrd;

const
  BookmarkCount = 10;
  DefUndoLimit = 10;
  MaxUndoLimit = 100;

  SHEDeleteError = 'Cannot delete whole buffer';
  SHEFontPitchError = 'Font must have the same width';
  
type

{ Exception classes }

  EHexEdit = class(Exception);

{ classes }

  TCustomHexEdit = class;  

{ records }

  TCharAttr = record
    FColor: TColor;
    BColor: TColor;
  end;

  TSelection = record
    Active: Boolean;
    StartOffset: Integer;      //based 0
    EndOffset: Integer;
  end;

  TBookmark = record
    Active: Boolean;
    DLen: Integer;
    CurInHex: Boolean;
  end;

{ THexEditActsMgr }

  //动作类型
  THexEditActType = (atNone, atModify, atInsert, atDelete);

  PHexEditAct = ^THexEditAct;
  THexEditAct = record             //动作记录
    Enabled: Boolean;              //该动作是否能用
    ActType: THexEditActType;      //动作类型
    Buf: string;                   //缓冲区
    Offset: Integer;               //偏移
    Count: Integer;                //字节数
    CurPos: Integer;               //光标位置
    Prev: PHexEditAct;             //Prev指针
    Next: PHexEditAct;             //Next指针
  end;

  THexEditActs = array of THexEditAct;

  THexEditActsMgr = class
  private
    FUndoActs: THexEditActs;       //Undo队列
    FUndoHead: PHexEditAct;        //队列头指针
    FUndoTail: PHexEditAct;        //队列尾指针
    FRedoActs: THexEditActs;       //Redo队列
    FRedoHead: PHexEditAct;        //队列头指针
    FRedoTail: PHexEditAct;        //队列尾指针
    FUndoLimit: Integer;           //最大Undo次数

    procedure SetUndoLimit(Value: Integer);
    procedure InitAct(var Act: THexEditAct);
  public
    constructor Create(AUndoLimit: Integer);
    destructor Destroy; override;

    property UndoLimit: Integer read FUndoLimit write SetUndoLimit;

    function AddUndoItem: PHexEditAct;
    function AddRedoItem: PHexEditAct;
    function Undo: PHexEditAct;
    function Redo: PHexEditAct;
    function CanUndo: Boolean;
    function CanRedo: Boolean;
  end;

{ THexEditColors }

  THexEditColors = class(TPersistent)
  private
    FOwner: TCustomHexEdit;
    FAddressColor: TCharAttr;     //地址区颜色
    FHexColor: TCharAttr;         //Hex区颜色
    FCharColor: TCharAttr;        //字符区颜色
    FCaretColor: TCharAttr;       //光标颜色
    FSelectionColor: TCharAttr;   //块颜色

    function GetAddressColor(const Index: Integer): TColor;
    function GetHexColor(const Index: Integer): TColor;
    function GetCharColor(const Index: Integer): TColor;
    function GetCaretColor(const Index: Integer): TColor;
    function GetSelectionColor(const Index: Integer): TColor;

    procedure SetAddressColor(const Index: Integer; const Value: TColor);
    procedure SetHexColor(const Index: Integer; const Value: TColor);
    procedure SetCharColor(const Index: Integer; const Value: TColor);
    procedure SetCaretColor(const Index: Integer; const Value: TColor);
    procedure SetSelectionColor(const Index: Integer; const Value: TColor);
  public
    constructor Create(AOwner: TCustomHexEdit); virtual;
    destructor Destroy; override;

    property AddressColor: TCharAttr read FAddressColor;
    property HexColor: TCharAttr read FHexColor;
    property CharColor: TCharAttr read FCharColor;
    property CaretColor: TCharAttr read FCaretColor;
    property SelectionColor: TCharAttr read FSelectionColor;
  published
    property AddressBackColor: TColor index 0 read GetAddressColor write SetAddressColor default clWhite;
    property AddressForeColor: TColor index 1 read GetAddressColor write SetAddressColor default clBlue;
    property HexBackColor: TColor index 0 read GetHexColor write SetHexColor default clWhite;
    property HexForeColor: TColor index 1 read GetHexColor write SetHexColor default clBlack;
    property CharBackColor: TColor index 0 read GetCharColor write SetCharColor default clWhite;
    property CharForeColor: TColor index 1 read GetCharColor write SetCharColor default clBlack;
    property CaretBackColor: TColor index 0 read GetCaretColor write SetCaretColor default clBlue;
    property CaretForeColor: TColor index 1 read GetCaretColor write SetCaretColor default clWhite;
    property SelectionBackColor: TColor index 0 read GetSelectionColor write SetSelectionColor default clNavy;
    property SelectionForeColor: TColor index 1 read GetSelectionColor write SetSelectionColor default clWhite;
  end;

{ TCustomHexEdit }

  TScrollKind = (skHorizontal, skVertical);
  //字符显示方式(常规、特殊字符用点表示、显示双字节字符)
  TDrawCharStyle = (dcGeneral, dcDotForSpec, dcDblByteChar);
  //编辑器选项
  THexEditOption = (hoEditing, hoShowCaret, hoAutoHideSelection, hoAllowSelect);
  THexEditOptions = set of THexEditOption;
  //移动光标的方式
  TMoveCaretMode = (mcStart, mcEnd, mcSelStart, mcSelEnd, mcLineStart, mcLineEnd);

  TIntegerType = (itShortInt, itSmallInt, itLongInt, itInt64, itByte, itWord, itLongWord);
  TFloatType = (ftSingle, ftDouble, ftReal48, ftExtended);
  //查找文本选项
  TSearchTextOptions = record
    MatchCase: Boolean;
    WholeWord: Boolean;
    Unicode: Boolean;
    OnlyBlock: Boolean;
    UseWild: Boolean;
    WildChar: Char;
    WildCount: Integer;
  end;
  //查找十六进制串选项
  TSearchHexOptions = record
    OnlyBlock: Boolean;
    UseWild: Boolean;
    WildChar: Char;
    WildCount: Integer;
  end;
  //查找整型数选项
  TSearchIntOptions = record
    IntegerType: TIntegerType;
    OnlyBlock: Boolean;
  end;
  //查找浮点数选项
  TSearchFloatOptions = record
    FloatType: TFloatType;
    Blur: Boolean;
    BlurValue: Extended;
    OnlyBlock: Boolean;
  end;

  TCustomHexEdit = class(TCustomControl)
  private
    FStream: TMemoryStream;       //缓冲区
    FRowCount: Integer;           //总行数
    FTopRow: Integer;             //顶行的绝对行号(based 0)
    FLeftCol: Integer;            //最左行的绝对列号(based 0)
    FCurRow: Integer;             //光标所在的绝对行号(based 0)
    FCurCol: Integer;             //光标所在的列号(based 0)
    FVisRowCount: Integer;        //窗口所能容纳的行数
    FCurInHex: Boolean;           //光标是否在HEX区域
    FCurInHigh: Boolean;          //光标是否在高4位区域
    FCtrlKPressed: Boolean;       //Ctrl + K 是否按下
    FCtrlQPressed: Boolean;       //Ctrl + Q 是否按下
    FMouseDownFlag: Boolean;      //Mouse是否按下
    FMouseDblClkFlag: Boolean;    //Mouse是否Double Click 
    FMsDownOffset: Integer;       //Mouse按下时指针所在位置的偏移值
    FMsMoveSameItem: Boolean;     //Mouse拖动定义块时，是否还在按下时的字节上
    FSelection: TSelection;       //选中区域
    FBookmark: array[0..BookmarkCount-1] of TBookmark;   //书签
    FModified: Boolean;           //是否被修改
    FCharWidth: Integer;          //字符宽度
    FCharHeight: Integer;         //字符高度
    FFocused: Boolean;            //当前是否得到焦点
    FActsMgr: THexEditActsMgr;    //管理 Undo Redo

    FBorderStyle: TBorderStyle;
    FScrollBars: TScrollStyle;
    FColors: THexEditColors;      //背景和字符的颜色
    FDrawCharStyle: TDrawCharStyle; //显示字符的方式
    FMargin: Integer;             //边界宽度
    FOptions: THexEditOptions;    //编辑器选项

    FOnChange: TNotifyEvent;
    FOnSelectionChange: TNotifyEvent;
    FOnCaretMove: TNotifyEvent;

    function GetDataAddr: PChar;
    function GetDataSize: Integer;
    function GetRowCount(Size: Integer): Integer;
    function GetRowPointer(AbsRow: Integer): PChar;
    function GetRowColPointer(AbsRow, Col: Integer): PChar;
    function GetOffset(AbsRow, Col: Integer): Integer; overload;
    function GetOffset: Integer; overload;
    function GetOffsetForProperty: Integer;
    function GetDLen(AbsRow, Col: Integer; CurInHigh: Boolean): Integer; overload;
    function GetDLen: Integer; overload;
    function GetRowTotalWidth: Integer;
    function GetAddrAreaLeft: Integer;
    function GetHexAreaLeft: Integer;
    function GetChrAreaLeft: Integer;
    function GetCharAttrByRowCol(AbsRow, Col: Integer; CurInHex: Boolean): TCharAttr;
    function GetOffsetByMove(DLenOffset: Integer): Integer;
    function GetOffsetByMoveTo(DLen: Integer): Integer;
    function GetHexAreaStr(AbsRow: Integer): string;
    function GetChrAreaStr(AbsRow: Integer): string;
    function GetStrAtPos(Offset, Count: Integer; var S: string): Boolean;
    function GetUndoLimit: Integer;
    function GetFont: TFont;
    function GetSelStart: Integer;
    function GetSelLength: Integer;
    function GetSelData: string;

    procedure SetDataSize(Value: Integer);
    procedure SetOffset(Value: Integer);
    procedure SetTopRow(Value: Integer);
    procedure SetBorderStyle(Value: TBorderStyle);
    procedure SetScrollBars(Value: TScrollStyle);
    procedure SetDrawCharStyle(Value: TDrawCharStyle);
    procedure SetUndoLimit(Value: Integer);
    procedure SetOptions(Value: THexEditOptions);
    procedure SetFont(Value: TFont);
    procedure SetSelStart(Value: Integer);
    procedure SetSelLength(Value: Integer);
    procedure SetSelData(const S: string);

    procedure DataLoaded;
    procedure InitCharSize;
    procedure InitFont;
    procedure InitSearchOptions;

    function DLenToOffset(DLen: Integer): Integer;
    function OffsetToDLen(AOffset: Integer): Integer;
    procedure PosToDLen(var DLen: Integer; AbsRow, Col: Integer; CurInHigh: Boolean);
    procedure DLenToPos(var AbsRow, Col: Integer; var CurInHigh: Boolean; DLen: Integer);
    function  XYToRowCol(var OffRow, Col: Integer;
      var CurInHex, CurInHigh: Boolean; X, Y: Integer): Boolean;

    procedure MoveCaret(DLenOffset: Integer; Force: Boolean = False); overload;
    procedure MoveCaretTo(DLen: Integer; Force: Boolean = False);
    procedure RestrictCaret;
    function CheckCaretInWindow: Boolean;

    procedure AdjustSelectionA(Offset1, Offset2: Integer);
    procedure AdjustSelectionB(Offset1, Offset2: Integer);
    procedure SetSelection(StartOffset, EndOffset: Integer; Redraw: Boolean = False);
    procedure SetSelectionStart(StartOffset: Integer; Redraw: Boolean = False);
    procedure SetSelectionEnd(EndOffset: Integer; Redraw: Boolean = False);
    procedure SetSelectionVisible(Value: Boolean = True);
    procedure CancelSelection(Redraw: Boolean = False);
    procedure CancelSelByUser(Redraw: Boolean = False);
    procedure RestrictSelection;
    function CheckInSelection(Offset: Integer): Boolean;
    procedure GetOffsetRange(var StartOffset, EndOffset: Integer; OnlyBlock, FromCursor: Boolean);

    procedure UpdateVertScrollBar;
    procedure UpdateHorzScrollBar;
    procedure UpdateScrollPage(AScrollKind: TScrollKind; APage: Integer);
    procedure UpdateScrollMax(AScrollKind: TScrollKind; AMax: Integer);
    procedure UpdateScrollPos(AScrollKind: TScrollKind; APos: Integer);

    procedure DrawText(X, Y: Integer; S: string; CharAttr: TCharAttr);
    procedure DrawAddrRow(OffRow, AbsRow: Integer);
    procedure DrawHexRow(OffRow, AbsRow: Integer);
    procedure DrawChrRow(OffRow, AbsRow: Integer);
    procedure DrawStrForChrRow(S: string; OffRow, Col: Integer; CharAttr: TCharAttr);
    procedure DrawOneRow(OffRow, AbsRow: Integer);
    procedure DrawSpcRow(OffRow: Integer);
    procedure DrawHexItem(OffRow, Col: Integer; HexCharAttr: TCharAttr);
    procedure DrawChrItem(OffRow, Col: Integer; ChrCharAttr: TCharAttr);
    procedure DrawItem(OffRow, Col: Integer; HexCharAttr, ChrCharAttr: TCharAttr);
    procedure DrawCurItem(Show: Boolean = True);
    procedure DrawCurItemHighlight(Show: Boolean = True);
    procedure DrawAllRow;

    procedure DoUpKey(ShiftPressed: Boolean);
    procedure DoDownKey(ShiftPressed: Boolean);
    procedure DoLeftKey(ShiftPressed: Boolean);
    procedure DoRightKey(ShiftPressed: Boolean);
    procedure DoPageUpKey(ShiftPressed: Boolean);
    procedure DoPageDownKey(ShiftPressed: Boolean);
    procedure DoHomeKey(ShiftPressed: Boolean);
    procedure DoEndKey(ShiftPressed: Boolean);
    procedure DoTabKey;
    procedure DoCtrlJKey;
    procedure DoCtrlKKey(Key: Word);
    procedure DoCtrlQKey(Key: Word);
    procedure DoCtrlKNumKey(Key: Word);
    procedure DoCtrlKBKey(Key: Word);
    procedure DoCtrlKKKey(Key: Word);
    procedure DoCtrlKHKey(Key: Word);
    procedure DoCtrlKWKey(Key: Word);
    procedure DoCtrlQNumKey(Key: Word);
    procedure DoCtrlUpKey(MoveCaret: Boolean = True);
    procedure DoCtrlDownKey(MoveCaret: Boolean = True);
    procedure DoCtrlLeftKey;
    procedure DoCtrlRightKey;
    procedure DoCtrlPageUpKey(ShiftPressed: Boolean);
    procedure DoCtrlPageDownKey(ShiftPressed: Boolean);
    procedure DoCharKey(Key: Char);
    procedure DoCharKeyForHexArea(Key: Char);
    procedure DoCharKeyForChrArea(Key: Char);

    function CreateUndoForModify(Offset, OldCount, NewCount: Integer): Boolean;
    function CreateUndoForInsert(Offset, Count: Integer): Boolean;
    function CreateUndoForDelete(Offset, Count: Integer): Boolean;
    function CreateRedoForModify(Offset, OldCount, NewCount: Integer): Boolean;
    function CreateRedoForInsert(Offset, Count: Integer): Boolean;
    function CreateRedoForDelete(Offset, Count: Integer): Boolean;

    procedure DoCreateCaret;
    procedure DoMouseRoll;
    procedure DoPaint;
    procedure DoResize;
    function  DoScroll(AScrollKind: TScrollKind; Msg: TWMScroll): Integer;
    procedure DoOnChange;
    procedure DoOnSelectionChange;
    procedure DoOnCaretMove;

    procedure InternalInsertData(Offset: Integer; const S: string);
    procedure InternalDeleteData(Offset, Count: Integer);
    procedure InternalModifyData(Offset: Integer; const S: string); overload;
    procedure InternalModifyData(Offset, Count: Integer; const S: string); overload;

    procedure CMFontChanged(var Message: TMessage); message CM_FONTCHANGED;
    procedure WMGetDlgCode(var Msg: TWMGetDlgCode); message WM_GETDLGCODE;
    procedure WMHScroll(var Msg: TWMHScroll); message WM_HSCROLL;
    procedure WMVScroll(var Msg: TWMVScroll); message WM_VSCROLL;
    procedure WMSetFocus(var Message: TWMSetFocus); message WM_SETFOCUS;
    procedure WMKillFocus(var Message: TWMSetFocus); message WM_KILLFOCUS;
  protected
    procedure CreateParams(var Params: TCreateParams); override;
    procedure Paint; override;
    procedure DoEnter; override;
    procedure DoExit; override;
    procedure KeyDown(var Key: Word; Shift: TShiftState); override;
    procedure KeyUp(var Key: Word; Shift: TShiftState); override;
    procedure KeyPress(var Key: Char); override;
    procedure MouseDown(Button: TMouseButton; Shift: TShiftState; X, Y: Integer); override;
    procedure MouseMove(Shift: TShiftState; X, Y: Integer); override;
    procedure MouseUp(Button: TMouseButton; Shift: TShiftState; X, Y: Integer); override;
    function DoMouseWheel(Shift: TShiftState; WheelDelta: Integer; MousePos: TPoint): Boolean; override;
    function DoMouseWheelDown(Shift: TShiftState; MousePos: TPoint): Boolean; override;
    function DoMouseWheelUp(Shift: TShiftState; MousePos: TPoint): Boolean; override;
    procedure Click; override;
    procedure DblClick; override;
    procedure Resize; override;
    procedure Loaded; override;
    procedure AdjustSize; override;
  public
    SearchTextOptions: TSearchTextOptions;
    SearchHexOptions: TSearchHexOptions;
    SearchIntOptions: TSearchIntOptions;
    SearchFloatOptions: TSearchFloatOptions;
  public
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;

    function CanUndo: Boolean;
    function CanRedo: Boolean;
    function Undo: Boolean;
    function Redo: Boolean;
    function CanPaste: Boolean;
    procedure CutToClipboard;
    procedure CopyToClipboard;
    procedure PasteFromClipboard;
    procedure SelectAll;
    function IsEmpty: Boolean;
    procedure MoveCaret(MoveMode: TMoveCaretMode); overload;
    procedure SetBookmark(BkmkIndex: Integer);
    procedure GotoBookmark(BkmkIndex: Integer);
    function BookmarkExists(BkmkIndex: Integer): Boolean;

    procedure LoadFromFile(const FileName: string);
    procedure SaveToFile(const FileName: string);
    procedure LoadFromStream(Stream: TStream);
    procedure SaveToStream(Stream: TStream);
    procedure LoadFromBuffer(const Buffer; Count: Longint);
    function SaveToBuffer(var Buffer; Count: Longint): Longint;
    procedure GetEditorText(Lines: TStrings; IncludeAddr, OnlyBlock: Boolean);

    procedure InsertData(Offset: Integer; const Buffer; BufferSize: Integer;
      AllowUndo: Boolean = False); overload;
    procedure DeleteData(Offset, Count: Integer; AllowUndo: Boolean = False);
    procedure DeleteSelection(AllowUndo: Boolean = False);
    procedure ModifyData(Offset: Integer; const Buffer; BufferSize: Integer;
      AllowUndo: Boolean = False); overload;
    procedure ModifyData(Offset, Count: Integer; const Buffer; BufferSize: Integer;
      AllowUndo: Boolean = False); overload;

    function SearchText(const Text: string; FromCursor: Boolean): Boolean;
    function SearchHex(const HexStr: string; FromCursor: Boolean): Boolean;
    function SearchInt(Value: Int64; FromCursor: Boolean): Boolean;
    function SearchFloat(Value: Extended; FromCursor: Boolean): Boolean;

    property DataSize: Integer read GetDataSize write SetDataSize;
    property DataAddr: PChar read GetDataAddr;
    property Offset: Integer read GetOffsetForProperty write SetOffset;
    property TopRow: Integer read FTopRow write SetTopRow;
    property RowCount: Integer read FRowCount;
    property Modified: Boolean read FModified write FModified;
    property SelStart: Integer read GetSelStart write SetSelStart;
    property SelLength: Integer read GetSelLength write SetSelLength;
    property SelData: string read GetSelData write SetSelData;

    property BorderStyle: TBorderStyle read FBorderStyle write SetBorderStyle default bsSingle;
    property ScrollBars: TScrollStyle read FScrollBars write SetScrollBars default ssBoth;
    property DrawCharStyle: TDrawCharStyle read FDrawCharStyle write SetDrawCharStyle default dcDblByteChar;
    property UndoLimit: Integer read GetUndoLimit write SetUndoLimit default DefUndoLimit;
    property Colors: THexEditColors read FColors write FColors;
    property Options: THexEditOptions read FOptions write SetOptions
      default [hoEditing, hoShowCaret, hoAutoHideSelection, hoAllowSelect];
    property Font: TFont read GetFont write SetFont;

    property OnChange: TNotifyEvent read FOnChange write FOnChange;
    property OnSelectionChange: TNotifyEvent read FOnSelectionChange write FOnSelectionChange;
    property OnCaretMove: TNotifyEvent read FOnCaretMove write FOnCaretMove;
  end;

{ THexEdit }

  THexEdit = class(TCustomHexEdit)
  published
    property BorderStyle;
    property ScrollBars;
    property TabStop default True;
    property Align;
    property Anchors;
    property BiDiMode;
    property Color;
    property Constraints;
    property Ctl3D;
    property DragCursor;
    property DragKind;
    property DragMode;
    property Enabled;
    property Font;
    property ParentBiDiMode;
    property ParentColor default False;
    property ParentCtl3D;
    property ParentFont default False;
    property ParentShowHint;
    property PopupMenu;
    property ShowHint;
    property TabOrder;
    property Visible;

    property DrawCharStyle;
    property UndoLimit;
    property Colors;
    property Options;

    property OnChange;
    property OnSelectionChange;
    property OnCaretMove;

    property OnClick;
    property OnContextPopup;
    property OnDblClick;
    property OnDragDrop;
    property OnDragOver;
    property OnEndDock;
    property OnEndDrag;
    property OnEnter;
    property OnExit;
    property OnKeyDown;
    property OnKeyPress;
    property OnKeyUp;
    property OnMouseDown;
    property OnMouseMove;
    property OnMouseUp;
    property OnMouseWheelDown;
    property OnMouseWheelUp;
    property OnStartDock;
    property OnStartDrag;
  end;

var
  CF_HEXEDITDATA: Word;

function GetCharAttr(FColor, BColor: TColor): TCharAttr;

implementation

{ Misc routines }

function GetCharAttr(FColor, BColor: TColor): TCharAttr;
begin
  Result.FColor := FColor;
  Result.BColor := BColor;
end;

procedure SwapInt(var V1, V2: Integer);
var
  Temp: Integer;
begin
  Temp := V1;
  V1 := V2;
  V2 := Temp;
end;

function CharUpperCase(Ch: Char): Char;
begin
  if (Ch >= 'a') and (Ch <= 'z') then Dec(Ch, 32);
  Result := Ch;
end;

function IsHexChar(C: Char): Boolean;
begin
  Result := (C >= '0') and (C <= '9') or
            (C >= 'A') and (C <= 'F') or
            (C >= 'a') and (C <= 'f');
end;

function IsAlphaChar(C: Char): Boolean;
begin
  Result := (C >= 'A') and (C <= 'Z') or
            (C >= 'a') and (C <= 'z');
end;

function IsDigitChar(Ch: Char): Boolean;
begin
  Result := (Ch >= '0') and (Ch <= '9');
end;

function IsSpecChar(C: Char): Boolean;
begin
  Result := (Ord(C) < 32) or (Ord(C) > 126);
end;

function IsBorderChar(Ch: Char): Boolean;
begin
  Result := not IsAlphaChar(Ch) and not IsDigitChar(Ch);
end;

function IsHexStr(S: string): Boolean;
var
  I: Integer;
begin
  for I := 1 to Length(S) do
  begin
    if not IsHexChar(S[I]) then
    begin
      Result := False;
      Exit;
    end;
  end;
  Result := True;
end;

function StrToIntAsHex(var V: Cardinal; S: string): Boolean;
var
  I, J: Cardinal;
begin
  Result := IsHexStr(S);
  if not Result then Exit;

  V := 0;
  S := UpperCase(S);
  for I := 1 to Length(S) do
  begin
    if IsDigitChar(S[I]) then J := Ord(S[I]) - Ord('0')
    else J := Ord(S[I]) - Ord('A') + 10;
    V := (V shl 4) + J;
  end;
  Result := True;
end;

{ THexEditActsMgr }

constructor THexEditActsMgr.Create(AUndoLimit: Integer);
begin
  SetUndoLimit(AUndoLimit);
end;

destructor THexEditActsMgr.Destroy;
begin
  SetLength(FUndoActs, 0);
  SetLength(FRedoActs, 0);
end;

procedure THexEditActsMgr.SetUndoLimit(Value: Integer);
var
  I: Integer;
begin
  FUndoLimit := Value;
  SetLength(FUndoActs, FUndoLimit + 1);
  for I := 0 to FUndoLimit do
  begin
    if I = FUndoLimit then
      FUndoActs[I].Next := @FUndoActs[0]
    else
      FUndoActs[I].Next := @FUndoActs[I+1];
    if I = 0 then
      FUndoActs[I].Prev := @FUndoActs[FUndoLimit]
    else
      FUndoActs[I].Prev := @FUndoActs[I-1];
    FUndoActs[I].Enabled := False;
  end;
  FUndoHead := @FUndoActs[0];
  FUndoTail := @FUndoActs[0];

  SetLength(FRedoActs, FUndoLimit + 1);
  for I := 0 to FUndoLimit do
  begin
    if I = FUndoLimit then
      FRedoActs[I].Next := @FRedoActs[0]
    else
      FRedoActs[I].Next := @FRedoActs[I+1];
    if I = 0 then
      FRedoActs[I].Prev := @FRedoActs[FUndoLimit]
    else
      FRedoActs[I].Prev := @FRedoActs[I-1];
    FRedoActs[I].Enabled := False;
  end;
  FRedoHead := @FRedoActs[0];
  FRedoTail := @FRedoActs[0];
end;

procedure THexEditActsMgr.InitAct(var Act: THexEditAct);
begin
  with Act do
  begin
    Enabled := True;
    Buf := '';
    Offset := 0;
    Count := 0;
    CurPos := 0;
  end;
end;

function THexEditActsMgr.AddUndoItem: PHexEditAct;
begin
  InitAct(FUndoHead^);
  Result := FUndoHead;

  if FUndoHead^.Next = FUndoTail then
    FUndoTail := FUndoTail^.Next;
  FUndoHead := FUndoHead^.Next;
  FUndoHead^.Enabled := False;
end;

function THexEditActsMgr.AddRedoItem: PHexEditAct;
begin
  InitAct(FRedoHead^);
  Result := FRedoHead;

  if FRedoHead^.Next = FRedoTail then
    FRedoTail := FRedoTail^.Next;
  FRedoHead := FRedoHead^.Next;
  FRedoHead^.Enabled := False;
end;

function THexEditActsMgr.Undo: PHexEditAct;
begin
  if not CanUndo then
  begin
    Result := nil;
    Exit;
  end;
  FUndoHead := FUndoHead^.Prev;
  FRedoHead := FRedoHead^.Prev;
  Result := FUndoHead;
end;

function THexEditActsMgr.Redo: PHexEditAct;
begin
  if not CanRedo then
  begin
    Result := nil;
    Exit;
  end;
  Result := FRedoHead;
  FRedoHead := FRedoHead^.Next;
  FUndoHead := FUndoHead^.Next;
end;

function THexEditActsMgr.CanUndo: Boolean;
begin
  Result := (FUndoHead <> FUndoTail);
end;

function THexEditActsMgr.CanRedo: Boolean;
begin
  Result := FRedoHead^.Enabled;
end;

{ THexEditColors }

constructor THexEditColors.Create(AOwner: TCustomHexEdit);
begin
  inherited Create;
  FOwner := AOwner;

  FAddressColor := GetCharAttr(clBlue, clWhite);
  FHexColor := GetCharAttr(clBlack, clWhite);
  FCharColor := GetCharAttr(clBlack, clWhite);
  FCaretColor := GetCharAttr(clWhite, clBlue);
  FSelectionColor := GetCharAttr(clWhite, clNavy);
end;

destructor THexEditColors.Destroy;
begin
  inherited;
end;

function THexEditColors.GetAddressColor(const Index: Integer): TColor;
begin
  if Index = 0 then Result := FAddressColor.BColor
  else Result := FAddressColor.FColor;
end;

function THexEditColors.GetHexColor(const Index: Integer): TColor;
begin
  if Index = 0 then Result := FHexColor.BColor
  else Result := FHexColor.FColor;
end;

function THexEditColors.GetCharColor(const Index: Integer): TColor;
begin
  if Index = 0 then Result := FCharColor.BColor
  else Result := FCharColor.FColor;
end;

function THexEditColors.GetCaretColor(const Index: Integer): TColor;
begin
  if Index = 0 then Result := FCaretColor.BColor
  else Result := FCaretColor.FColor;
end;

function THexEditColors.GetSelectionColor(const Index: Integer): TColor;
begin
  if Index = 0 then Result := FSelectionColor.BColor
  else Result := FSelectionColor.FColor;
end;

procedure THexEditColors.SetAddressColor(const Index: Integer; const Value: TColor);
begin
  if Index = 0 then FAddressColor.BColor := Value
  else FAddressColor.FColor := Value;
  FOwner.DoPaint;
end;

procedure THexEditColors.SetHexColor(const Index: Integer; const Value: TColor);
begin
  if Index = 0 then FHexColor.BColor := Value
  else FHexColor.FColor := Value;
  FOwner.DoPaint;
end;

procedure THexEditColors.SetCharColor(const Index: Integer; const Value: TColor);
begin
  if Index = 0 then FCharColor.BColor := Value
  else FCharColor.FColor := Value;
  FOwner.DoPaint;
end;

procedure THexEditColors.SetCaretColor(const Index: Integer; const Value: TColor);
begin
  if Index = 0 then FCaretColor.BColor := Value
  else FCaretColor.FColor := Value;
  FOwner.DoPaint;
end;

procedure THexEditColors.SetSelectionColor(const Index: Integer; const Value: TColor);
begin
  if Index = 0 then FSelectionColor.BColor := Value
  else FSelectionColor.FColor := Value;
  FOwner.DoPaint;
end;

{ TCustomHexEdit }

constructor TCustomHexEdit.Create(AOwner: TComponent);
const
  EditStyle = [csClickEvents, csSetCaption, csCaptureMouse,
    csDoubleClicks, csFixedHeight];
var
  I: Integer;
begin
  inherited;
  InitSearchOptions;
  if NewStyleControls then
    ControlStyle := EditStyle else
    ControlStyle := EditStyle + [csFramed];
  FScrollBars := ssBoth;
  FBorderStyle := bsSingle;
  Color := clWindow;
  ParentColor := False;
  ParentFont := False;
  TabStop := True;
  Width := 195;
  Height := 90;

  FActsMgr := THexEditActsMgr.Create(DefUndoLimit);
  FStream := TMemoryStream.Create;
  CancelSelection(False);
  for I := 0 to BookmarkCount - 1 do
    FBookmark[I].Active := False;
  FRowCount := 0;
  FTopRow := 0;
  FLeftCol := 0;
  FCurRow := 0;
  FCurCol := 0;
  FVisRowCount := 0;
  FCurInHex := False;
  FCurInHigh := False;
  FCtrlKPressed := False;
  FCtrlQPressed := False;
  FModified := False;

  FColors := THexEditColors.Create(Self);
  FDrawCharStyle := dcDblByteChar;
  FMargin := 5;
  FOptions := [hoEditing, hoShowCaret, hoAutoHideSelection, hoAllowSelect];
end;

destructor TCustomHexEdit.Destroy;
begin
  FColors.Free;
  FActsMgr.Free;
  FStream.Free;
  inherited;
end;

function TCustomHexEdit.GetDataAddr: PChar;
begin
  Result := PChar(FStream.Memory);
end;

function TCustomHexEdit.GetDataSize: Integer;
begin
  Result := FStream.Size;
end;

function TCustomHexEdit.GetRowCount(Size: Integer): Integer;
begin
  Result := Size div 16;
  if Size mod 16 <> 0 then Inc(Result);
end;

function TCustomHexEdit.GetRowPointer(AbsRow: Integer): PChar;
begin
  Result := GetDataAddr + AbsRow * 16;
end;

function TCustomHexEdit.GetRowColPointer(AbsRow, Col: Integer): PChar;
begin
  Result := GetDataAddr + AbsRow * 16 + Col;
end;

function TCustomHexEdit.GetOffset(AbsRow, Col: Integer): Integer;
begin
  Result := AbsRow * 16 + Col;
end;

function TCustomHexEdit.GetOffset: Integer;
begin
  Result := FCurRow * 16 + FCurCol;
end;

function TCustomHexEdit.GetOffsetForProperty: Integer;
begin
  Result := GetOffset;
end;

function TCustomHexEdit.GetDLen(AbsRow, Col: Integer; CurInHigh: Boolean): Integer;
begin
  PosToDLen(Result, AbsRow, Col, CurInHigh);
end;

function TCustomHexEdit.GetDLen: Integer;
begin
  PosToDLen(Result, FCurRow, FCurCol, FCurInHigh);
end;

function TCustomHexEdit.GetRowTotalWidth: Integer;
begin
  Result := FCharWidth * 74 + FMargin * 2;
end;

function TCustomHexEdit.GetAddrAreaLeft: Integer;
begin
  Result := FMargin;
end;

function TCustomHexEdit.GetHexAreaLeft: Integer;
begin
  Result := GetAddrAreaLeft + FCharWidth * 10;
end;

function TCustomHexEdit.GetChrAreaLeft: Integer;
begin
  Result := GetHexAreaLeft + FCharWidth * 48;
end;

function TCustomHexEdit.GetCharAttrByRowCol(AbsRow, Col: Integer; CurInHex: Boolean): TCharAttr;
var
  Offset: Integer;
begin
  if FSelection.Active then
  begin
    Offset := GetOffset(AbsRow, Col);
    if (Offset >= FSelection.StartOffset) and (Offset <= FSelection.EndOffset) then
      Result := FColors.SelectionColor
    else
      Result := FColors.HexColor;
  end else
  begin
    if CurInHex then Result := FColors.HexColor
    else Result := FColors.CharColor;
  end;
end;

function TCustomHexEdit.GetOffsetByMove(DLenOffset: Integer): Integer;
var
  DLen, AbsRow, Col: Integer;
  CurInHigh: Boolean;
begin
  PosToDLen(DLen, FCurRow, FCurCol, FCurInHigh);
  DLen := DLen + DLenOffset;
  if DLen < 1 then DLen := 1
  else if DLen > GetDataSize * 2 then DLen := GetDataSize * 2;
  DLenToPos(AbsRow, Col, CurInHigh, DLen);
  Result := AbsRow * 16 + Col;
end;

function TCustomHexEdit.GetOffsetByMoveTo(DLen: Integer): Integer;
var
  AbsRow, Col: Integer;
  CurInHigh: Boolean;
begin
  if DLen < 1 then DLen := 1
  else if DLen > GetDataSize * 2 then DLen := GetDataSize * 2;
  DLenToPos(AbsRow, Col, CurInHigh, DLen);
  Result := AbsRow * 16 + Col;
end;

function TCustomHexEdit.GetHexAreaStr(AbsRow: Integer): string;
var
  S: string;
  RowPointer: PChar;
  I, ColCount: Integer;
begin
  RowPointer := GetRowPointer(AbsRow);
  S := '';

  if (AbsRow < FRowCount -1) then
    ColCount := 16
  else begin
    if (GetDataSize mod 16 > 0) then ColCount := GetDataSize mod 16
    else ColCount := 16;
  end;

  for I := 0 to ColCount - 1 do
  begin
    S := S + IntToHex(Integer(RowPointer^), 2);
    if I <> 7 then S := S + ' '
    else S := S + '-';
    Inc(RowPointer);
  end;
  for I := ColCount to 15 do
    S := S + '   ';
  Result := S;
end;

function TCustomHexEdit.GetChrAreaStr(AbsRow: Integer): string;
var
  S: string;
  RowPointer: PChar;
  I, ColCount: Integer;
begin
  RowPointer := GetRowPointer(AbsRow);
  S := '';

  if (AbsRow < FRowCount -1) then
    ColCount := 16
  else begin
    if (GetDataSize mod 16 > 0) then ColCount := GetDataSize mod 16
    else ColCount := 16;
  end;

  for I := 0 to ColCount - 1 do
  begin
    S := S + RowPointer^;
    Inc(RowPointer);
  end;
  for I := ColCount to 15 do
    S := S + ' ';

  Result := S;
end;

function TCustomHexEdit.GetStrAtPos(Offset, Count: Integer; var S: string): Boolean;
begin
  SetString(S, GetDataAddr + Offset, Count);
  Result := True;
end;

function TCustomHexEdit.GetUndoLimit: Integer;
begin
  Result := FActsMgr.UndoLimit;
end;

function TCustomHexEdit.GetFont: TFont;
begin
  Result := inherited Font;
end;

function TCustomHexEdit.GetSelStart: Integer;
begin
  if FSelection.Active then
    Result := FSelection.StartOffset
  else
    Result := GetOffset;
end;

function TCustomHexEdit.GetSelLength: Integer;
begin
  if FSelection.Active then
  begin
    Result := FSelection.EndOffset - FSelection.StartOffset + 1;
    if Result < 0 then Result := -Result;
  end else
    Result := 0;
end;

function TCustomHexEdit.GetSelData: string;
begin
  if FSelection.Active then
  begin
    GetStrAtPos(FSelection.StartOffset, GetSelLength, Result);
  end else
  begin
    Result := '';
  end;
end;

procedure TCustomHexEdit.SetDataSize(Value: Integer);
var
  OldSize: Integer;
begin
  if GetDataSize <> Value then
  begin
    OldSize := FStream.Size;
    FStream.SetSize(Value);
    FillChar((PChar(FStream.Memory) + OldSize)^, Value - OldSize, 0);

    FRowCount := GetRowCount(GetDataSize);
    if FTopRow > FRowCount - FVisRowCount then
      FTopRow := FRowCount - FVisRowCount;
    if FTopRow < 0 then FTopRow := 0;

    UpdateVertScrollBar;
    FModified := True;
    DoResize;
    DoPaint;
    DoOnChange;
  end;
end;

procedure TCustomHexEdit.SetOffset(Value: Integer);
begin
  if (Value >= 0) and (Value < GetDataSize - 1) then
  begin
    MoveCaretTo(Value * 2 + 1);
    //CancelSelection(True);
  end;
end;

procedure TCustomHexEdit.SetTopRow(Value: Integer);
begin
  if FTopRow <> Value then
  begin
    if (Value >= 0) and (Value < FRowCount) then
    begin
      FTopRow := Value;
      DrawAllRow;
    end;
  end;
end;

procedure TCustomHexEdit.SetBorderStyle(Value: TBorderStyle);
begin
  if FBorderStyle <> Value then
  begin
    FBorderStyle := Value;
    RecreateWnd;
  end;
end;

procedure TCustomHexEdit.SetScrollBars(Value: TScrollStyle);
begin
  if FScrollBars <> Value then
  begin
    FScrollBars := Value;
    RecreateWnd;
    DoResize;
  end;
end;

procedure TCustomHexEdit.SetDrawCharStyle(Value: TDrawCharStyle);
begin
  if FDrawCharStyle <> Value then
  begin
    FDrawCharStyle := Value;
    DoPaint;
  end;
end;

procedure TCustomHexEdit.SetUndoLimit(Value: Integer);
begin
  if Value < 0 then Value := 0;
  if Value > MaxUndoLimit then Value := MaxUndoLimit;

  if FActsMgr.UndoLimit <> Value then
    FActsMgr.UndoLimit := Value;
end;

procedure TCustomHexEdit.SetOptions(Value: THexEditOptions);
var
  Temp: THexEditOptions;
begin
  if FOptions <> Value then
  begin
    Temp := Value;
    if (hoEditing in Value) and not (hoEditing in FOptions) then
    begin
      Include(Temp, hoShowCaret);
      Include(Temp, hoEditing);
    end;
    if not (hoShowCaret in Value) and (hoShowCaret in FOptions) then
    begin
      Exclude(Temp, hoEditing);
      Exclude(Temp, hoShowCaret);
    end;
    FOptions := Temp;
  end;
end;

procedure TCustomHexEdit.SetFont(Value: TFont);
var
  T: TTextMetric;
begin
  { TODO : This does not work fine. }
  GetTextMetrics(Handle, T);
  if (T.tmPitchAndFamily and TMPF_FIXED_PITCH) > 0	then
    raise EHexEdit.Create(SHEFontPitchError);

  inherited Font := Value;
end;

procedure TCustomHexEdit.SetSelStart(Value: Integer);
begin
  CancelSelection(False);
  Offset := Value;
end;

procedure TCustomHexEdit.SetSelLength(Value: Integer);
begin
  if Value > 0 then
  begin
    SetSelection(GetOffset, GetOffset + Value - 1, True);
  end else
  begin
    if FSelection.Active then
      CancelSelection(True)
    else
      DoPaint;
  end;
end;

procedure TCustomHexEdit.SetSelData(const S: string);
var
  SelLen, NewLen, AOffset: Integer;
begin
  NewLen := Length(S);
  if FSelection.Active then
  begin
    SelLen := GetSelLength;
    AOffset := FSelection.StartOffset;
    CreateUndoForModify(AOffset, SelLen, NewLen);
    InternalModifyData(AOffset, SelLen, S);
    CreateRedoForModify(AOffset, SelLen, NewLen);
    if NewLen > 0 then
      SetSelection(AOffset, AOffset + NewLen - 1, True)
    else
      CancelSelection(True);
    MoveCaretTo(OffsetToDLen(AOffset));
  end else
  begin
    if NewLen > 0 then
    begin
      AOffset := GetOffset;
      CreateUndoForModify(AOffset, NewLen, NewLen);
      InternalModifyData(AOffset, S);
      CreateRedoForModify(AOffset, NewLen, NewLen);
      SetSelection(AOffset, AOffset + NewLen - 1, True);
    end;
  end;
end;

procedure TCustomHexEdit.DataLoaded;
begin
  FRowCount := GetRowCount(GetDataSize);
  FCurRow := 0;
  FCurCol := 0;
  FTopRow := 0;
  FLeftCol := 0;
  FCurInHex := True;
  FCurInHigh := True;
  UpdateVertScrollBar;
  UpdateHorzScrollBar;
  DoResize;
  DoPaint;
end;

procedure TCustomHexEdit.InitCharSize;
begin
  FCharWidth := Canvas.TextWidth('H');
  FCharHeight := Canvas.TextHeight('H');
end;

procedure TCustomHexEdit.InitFont;
var
  TempFont: TFont;
begin
  TempFont := TFont.Create;
  try
    with TempFont do
    begin
      Name := 'Fixedsys';
      Size := 12;
      Height := -16;
      Pitch := fpDefault;
      Style := [];
      Charset := DEFAULT_CHARSET;
      Color := clBlack;
    end;
    inherited Font := TempFont;
  finally
    TempFont.Free;
  end;
end;

procedure TCustomHexEdit.InitSearchOptions;
begin
  with SearchTextOptions do
  begin
    MatchCase := False;
    WholeWord := True;
    Unicode := False;
    OnlyBlock := False;
    UseWild := False;
    WildChar := '?';
    WildCount := 1;
  end;
  with SearchHexOptions do
  begin
    OnlyBlock := False;
    UseWild := False;
    WildChar := '?';
    WildCount := 1;
  end;
  with SearchIntOptions do
  begin
    OnlyBlock := False;
    IntegerType := itLongword;
  end;
  with SearchFloatOptions do
  begin
    FloatType := ftDouble;
    Blur := True;
    BlurValue := 0.001;
    OnlyBlock := False;
  end;
end;

function TCustomHexEdit.DLenToOffset(DLen: Integer): Integer;
var
  I: Integer;
begin
  if DLen > 0 then I := DLen - 1
  else I := DLen;
  Result := I div 2;
end;

function TCustomHexEdit.OffsetToDLen(AOffset: Integer): Integer;
begin
  Result := AOffset * 2 + 1;
end;

procedure TCustomHexEdit.PosToDLen(var DLen: Integer; AbsRow, Col: Integer; CurInHigh: Boolean);
begin
  DLen := (AbsRow * 16 + Col) * 2 + 1;
  if not CurInHigh then Inc(DLen);
end;

procedure TCustomHexEdit.DLenToPos(var AbsRow, Col: Integer; var CurInHigh: Boolean; DLen: Integer);
var
  Offset: Integer;
begin
  Offset := DLenToOffset(DLen);
  AbsRow := Offset div 16;
  Col := Offset mod 16;
  if (DLen mod 2) <> 0 then CurInHigh := True
  else CurInHigh := False;
end;

function  TCustomHexEdit.XYToRowCol(var OffRow, Col: Integer;
  var CurInHex, CurInHigh: Boolean; X, Y: Integer): Boolean;
var
  I: Integer;
  X1, Y1, X2, Y2: Integer;
  CharW, CharH: Integer;
  HexAreaLeft, ChrAreaLeft: Integer;
begin
  if IsEmpty then
  begin
    Result := False;
    Exit;
  end;

  CharW := FCharWidth;
  CharH := FCharHeight;
  HexAreaLeft := GetHexAreaLeft;
  ChrAreaLeft := GetChrAreaLeft;
  OffRow := -1;
  Col := -1;
  for I := 0 to FVisRowCount - 1 do
  begin
    Y1 := CharH * I;
    Y2 := Y1 + CharH;
    if (Y >= Y1) and (Y <= Y2) then OffRow := I;
  end;
  for I := 0 to 15 do
  begin
    X1 := HexAreaLeft + CharW * I * 3;
    X2 := X1 + CharW * 3;
    if (X >= X1) and (X <= X2) then
    begin
      Col := I;
      CurInHex := True;
      CurInHigh := (X <= X1 + CharW);
      Break;
    end;
    X1 := ChrAreaLeft + CharW * I;
    X2 := X1 + CharW;
    if (X >= X1) and (X <= X2) then
    begin
      Col := I;
      CurInHex := False;
      CurInHigh := False;
      Break;
    end;
  end;
  Result := (OffRow <> -1) and (Col <> -1) and
    (GetDLen(FTopRow + OffRow, Col, CurInHigh) <= GetDataSize * 2);
end;

procedure TCustomHexEdit.MoveCaret(DLenOffset: Integer; Force: Boolean);
var
  OldDLen, DLen, AbsRow, OffRow, Col: Integer;
  OldTopRow: Integer;
  CurInHigh: Boolean;
begin
  OldTopRow := FTopRow;
  PosToDLen(DLen, FCurRow, FCurCol, FCurInHigh);
  OldDLen := DLen;
  DLen := DLen + DLenOffset;
  if DLen < 1 then DLen := 1
  else if DLen > GetDataSize * 2 then DLen := GetDataSize * 2;
  if (DLen = OldDLen) and not Force then Exit;

  DLenToPos(AbsRow, Col, CurInHigh, DLen);
  OffRow := AbsRow - FTopRow;

  DrawCurItemHighlight(False);
  DrawCurItem(False);

  FCurRow := AbsRow;
  FCurCol := Col;
  FCurInHigh := CurInHigh;

  if OffRow < 0 then
  begin
    FTopRow := AbsRow;
  end
  else if OffRow >= FVisRowCount then
  begin
    FTopRow := AbsRow - FVisRowCount + 1;
  end;

  DrawCurItem(True);
  DrawCurItemHighlight(True);

  if FTopRow <> OldTopRow then DrawAllRow;
  UpdateScrollPos(skVertical, FTopRow);
  DoOnCaretMove;
end;

procedure TCustomHexEdit.MoveCaretTo(DLen: Integer; Force: Boolean);
var
  CurDLen: Integer;
begin
  if DLen < 1 then DLen := 1;
  if DLen > GetDataSize * 2 then DLen := GetDataSize * 2;
  CurDLen := GetDLen;
  MoveCaret(DLen - CurDLen, Force);
end;

procedure TCustomHexEdit.RestrictCaret;
var
  DLen, AbsRow, Col: Integer;
  CurInHigh: Boolean;
begin
  if GetOffset > GetDataSize - 1 then
  begin
    DLen := OffsetToDLen(GetDataSize - 1);
    DLenToPos(AbsRow, Col, CurInHigh, DLen);
    FCurRow := AbsRow;
    FCurCol := Col;
  end;
end;

function TCustomHexEdit.CheckCaretInWindow: Boolean;
begin
  Result := (FCurRow >= FTopRow) and (FCurRow <= FTopRow + FVisRowCount - 1);
end;

procedure TCustomHexEdit.AdjustSelectionA(Offset1, Offset2: Integer);
begin
  if not (hoAllowSelect in FOptions) or
    not (hoShowCaret in FOptions) then Exit;

  if not FSelection.Active then
  begin
    SetSelection(Offset1, Offset2, False);
  end else
  begin
    if FSelection.StartOffset = Offset1 then
      SetSelectionStart(Offset2, False)
    else if FSelection.EndOffset = Offset1 then
      SetSelectionEnd(Offset2, False)
    else if Offset2 >= 0 then
      SetSelection(Offset2, Offset1, False);
  end;
end;

procedure TCustomHexEdit.AdjustSelectionB(Offset1, Offset2: Integer);
begin
  if not (hoAllowSelect in FOptions) or
    not (hoShowCaret in FOptions) then Exit;

  if not FSelection.Active then
  begin
    SetSelection(Offset1, Offset2, False);
  end else
  begin
    if FSelection.EndOffset = Offset1 then
      SetSelectionEnd(Offset2, False)
    else if  FSelection.StartOffset = Offset1 then
      SetSelectionStart(Offset2, False)
    else if Offset2 < GetDataSize then
      SetSelection(Offset1, Offset2, False);
  end;
end;

procedure TCustomHexEdit.SetSelection(StartOffset, EndOffset: Integer; Redraw: Boolean);
var
  OldSelection: TSelection;
begin
  OldSelection := FSelection;
  FSelection.Active := True;
  FSelection.StartOffset := StartOffset;
  FSelection.EndOffset := EndOffset;
  RestrictSelection;
  if Redraw then DoPaint;
  if not CompareMem(Pointer(@OldSelection), Pointer(@FSelection), SizeOf(TSelection)) then
    DoOnSelectionChange;
end;

procedure TCustomHexEdit.SetSelectionStart(StartOffset: Integer; Redraw: Boolean);
begin
  if StartOffset < 0 then StartOffset := 0;
  if StartOffset > GetDataSize - 1 then StartOffset := GetDataSize - 1;

  FSelection.StartOffset := StartOffset;
  if FSelection.StartOffset > FSelection.EndOffset then
    FSelection.Active := False
  else
    FSelection.Active := True;
  if Redraw then DoPaint;
  DoOnSelectionChange;
end;

procedure TCustomHexEdit.SetSelectionEnd(EndOffset: Integer; Redraw: Boolean);
begin
  if EndOffset < 0 then EndOffset := 0;
  if EndOffset > GetDataSize - 1 then EndOffset := GetDataSize - 1;

  FSelection.EndOffset := EndOffset;
  if FSelection.StartOffset > FSelection.EndOffset then
    FSelection.Active := False
  else
    FSelection.Active := True;
  if Redraw then DoPaint;
  DoOnSelectionChange;
end;

procedure TCustomHexEdit.SetSelectionVisible(Value: Boolean);
begin
  if FSelection.Active <> Value then
  begin
    FSelection.Active := Value;
    RestrictSelection;
    DoPaint;
    DoOnSelectionChange;
  end;
end;

procedure TCustomHexEdit.CancelSelection(Redraw: Boolean);
begin
  if FSelection.Active then
  begin
    FSelection.Active := False;
    if Redraw then DoPaint;
    DoOnSelectionChange;
  end;
end;

procedure TCustomHexEdit.CancelSelByUser(Redraw: Boolean);
begin
  if hoAutoHideSelection in FOptions then
    CancelSelection(Redraw);
end;

procedure TCustomHexEdit.RestrictSelection;
begin
  if FSelection.Active then
  begin
    if FSelection.StartOffset > FSelection.EndOffset then
      SwapInt(FSelection.StartOffset, FSelection.EndOffset);
    if FSelection.StartOffset < 0 then
      FSelection.StartOffset := 0;
    if FSelection.EndOffset > GetDataSize - 1 then
      FSelection.EndOffset := GetDataSize - 1;
    if FSelection.StartOffset > FSelection.EndOffset then
      FSelection.Active := False;
  end;
end;

function TCustomHexEdit.CheckInSelection(Offset: Integer): Boolean;
begin
  Result := FSelection.Active;
  if Result then
    Result := (Offset >= FSelection.StartOffset) and (Offset <= FSelection.EndOffset);
end;

procedure TCustomHexEdit.GetOffsetRange(var StartOffset, EndOffset: Integer;
  OnlyBlock, FromCursor: Boolean);
var
  CurOffset: Integer;
begin
  CurOffset := GetOffset;
  if OnlyBlock then
  begin
    if FSelection.Active then
    begin
      if FromCursor then
      begin
        if (CurOffset >= FSelection.StartOffset) and
          (CurOffset <= FSelection.EndOffset) then
        begin
          StartOffset := CurOffset + 1;
          EndOffset := FSelection.EndOffset;
        end else
        begin
          StartOffset := FSelection.StartOffset;
          EndOffset := FSelection.EndOffset;
        end;
      end else
      begin
        StartOffset := FSelection.StartOffset;
        EndOffset := FSelection.EndOffset;
      end;
    end else
    begin
      StartOffset := 0;
      EndOffset := -1;
    end;
  end else
  begin
    if FromCursor then
      StartOffset := CurOffset + 1
    else
      StartOffset := 0;
    EndOffset := GetDataSize - 1;
  end;
end;

procedure TCustomHexEdit.UpdateVertScrollBar;
var
  Max, Page: Integer;
begin
  if (ScrollBars in [ssNone, ssHorizontal]) or
    not HandleAllocated or not Showing then Exit;

  if csDesigning in ComponentState then
  begin
    UpdateScrollMax(skVertical, 1);
    UpdateScrollPage(skVertical, 0);
  end else
  begin
    Max := FRowCount - 2;
    if Max < 1 then Max := 1;
    UpdateScrollMax(skVertical, Max);

    Page := FVisRowCount - 1;
    if Page < 1 then Page := 1;
    UpdateScrollPage(skVertical, Page);
  end;
end;

procedure TCustomHexEdit.UpdateHorzScrollBar;
var
  Max: Integer;
begin
  if (ScrollBars in [ssNone, ssVertical]) or
    not HandleAllocated or not Showing then Exit;

  if csDesigning in ComponentState then
  begin
    UpdateScrollMax(skHorizontal, 1);
    UpdateScrollPage(skHorizontal, 0);
  end else
  begin
    if ClientWidth < GetRowTotalWidth then
      Max := GetRowTotalWidth div FCharWidth - 1
    else
      Max := 0;
    UpdateScrollMax(skHorizontal, Max);
    UpdateScrollPage(skHorizontal, ClientWidth div FCharWidth - 1);
  end;
end;

procedure TCustomHexEdit.UpdateScrollPage(AScrollKind: TScrollKind; APage: Integer);
var
  ScrInfo: TScrollInfo;
  BarFlag: Integer;
begin
  if AScrollKind = skHorizontal then
    BarFlag := SB_HORZ
  else
    BarFlag := SB_VERT;

  if APage < 0 then APage := 0;

  FillChar(ScrInfo, SizeOf(ScrInfo), 0);
  ScrInfo.cbSize := SizeOf(ScrInfo);
  ScrInfo.fMask := SIF_DISABLENOSCROLL or SIF_PAGE;
  ScrInfo.nPage := APage;

  SetScrollInfo(Handle, BarFlag, ScrInfo, True);
end;

procedure TCustomHexEdit.UpdateScrollMax(AScrollKind: TScrollKind; AMax: Integer);
var
  BarFlag: Integer;
begin
  if AScrollKind = skHorizontal then
    BarFlag := SB_HORZ
  else
    BarFlag := SB_VERT;

  if AMax < 0 then AMax := 0;
  SetScrollRange(Handle, BarFlag, 0, AMax, True);
end;

procedure TCustomHexEdit.UpdateScrollPos(AScrollKind: TScrollKind; APos: Integer);
var
  BarFlag: Integer;
begin
  if AScrollKind = skHorizontal then
    BarFlag := SB_HORZ
  else
    BarFlag := SB_VERT;

  if APos < 0 then APos := 0;
  SetScrollPos(Handle, BarFlag, APos, True);
end;

procedure TCustomHexEdit.DrawText(X, Y: Integer; S: string; CharAttr: TCharAttr);
begin
  Canvas.Brush.Color := CharAttr.BColor;
  Canvas.Font.Color := CharAttr.FColor;
  Canvas.TextOut(X - FCharWidth * FLeftCol, Y, S);
end;

procedure TCustomHexEdit.DrawAddrRow(OffRow, AbsRow: Integer);
var
  S: string;
  X, Y: Integer;
begin
  X := GetAddrAreaLeft;
  Y := OffRow * FCharHeight;

  Canvas.Brush.Color := FColors.AddressColor.BColor;
  Canvas.FillRect(Rect(0, Y, FMargin, Y + FCharHeight));

  S := IntToHex(AbsRow * 16, 8) + ': ';
  DrawText(X, Y, S, FColors.AddressColor);
end;

procedure TCustomHexEdit.DrawHexRow(OffRow, AbsRow: Integer);
var
  S, S1: string;
  X, Y: Integer;
  I, Offset: Integer;
  CharAttr: TCharAttr;
begin
  X := GetHexAreaLeft;
  Y := OffRow * FCharHeight;
  S := GetHexAreaStr(AbsRow);

  for I := 0 to 15 do
  begin
    Offset := GetOffset(AbsRow, I);
    CharAttr := GetCharAttrByRowCol(AbsRow, I, True);
    if (Offset <> FSelection.EndOffset) and (I <> 15) then
    begin
      S1 := Copy(S, I*3+1, 3);
      DrawText(X+I*FCharWidth*3, Y, S1, CharAttr);
    end else
    begin
      S1 := Copy(S, I*3+1, 2);
      DrawText(X+I*FCharWidth*3, Y, S1, CharAttr);
      S1 := Copy(S, I*3+3, 1);
      DrawText(X+I*FCharWidth*3+FCharWidth*2, Y, S1, FColors.HexColor);
    end;
  end;
end;

procedure TCustomHexEdit.DrawChrRow(OffRow, AbsRow: Integer);
var
  S: string;
  S1: array[1..3] of string;
  I, J, Offset: Integer;
  DrawSelection, InSelection: Boolean;
  CharAttr: TCharAttr;
begin
  S := GetChrAreaStr(AbsRow);

  //画右边的空白区域
  Canvas.Brush.Color := FColors.CharColor.BColor;
  Canvas.FillRect(Rect(GetChrAreaLeft + FCharWidth * (Length(S) - FLeftCol),
    OffRow * FCharHeight, ClientWidth, (OffRow+1) * FCharHeight));

  if not FSelection.Active then DrawSelection := False
  else
  begin
    if (GetOffset(AbsRow, 0) <= FSelection.EndOffset) and
       (GetOffset(AbsRow, 15) >= FSelection.StartOffset) then
         DrawSelection := True
    else
         DrawSelection := False;
  end;

  if DrawSelection then
  begin
    J := 1;
    S1[1] := '';    S1[2] := '';    S1[3] := '';
    for I := 0 to 15 do
    begin
      Offset := GetOffset(AbsRow, I);
      InSelection := (Offset <= FSelection.EndOffset) and (Offset >= FSelection.StartOffset);
      if InSelection then J := 2;
      if Offset > FSelection.EndOffset then J := 3;
      S1[J] := S1[J] + S[I + 1];
    end;
    J := 0;
    for I := 1 to 3 do
    begin
      if I = 2 then CharAttr := FColors.SelectionColor
      else CharAttr := FColors.CharColor;
      DrawStrForChrRow(S1[I], OffRow, J, CharAttr);
      J := J + Length(S1[I]);
    end;
  end else
    DrawStrForChrRow(S, OffRow, 0, FColors.CharColor);
end;

procedure TCustomHexEdit.DrawStrForChrRow(S: string; OffRow, Col: Integer; CharAttr: TCharAttr);
var
  I, J, X, Y: Integer;
  S1: string;
begin
  if S = '' then Exit;
  X := GetChrAreaLeft + FCharWidth * Col;
  Y := OffRow * FCharHeight;
  case FDrawCharStyle of
    dcGeneral:
      for I := 1 to Length(S) do
      begin
        DrawText(X+FCharWidth*(I-1), Y, S[I], CharAttr);
      end;
    dcDotForSpec:
      for I := 1 to Length(S) do
      begin
        if IsSpecChar(S[I]) then
          S1 := '.'
        else
          S1 := S[I];
        DrawText(X+FCharWidth*(I-1), Y, S1, CharAttr);
      end;
    dcDblByteChar:
      begin
        I := 1;  J := 0;
        while I <= Length(S) do
        begin
          S1 := S[I];
          if I < Length(S) then
          begin
            S1 := S1 + S[I + 1];
            Inc(I);
          end;
          DrawText(X+FCharWidth*J, Y, StringOfChar(' ', Length(S1)), CharAttr);
          DrawText(X+FCharWidth*J, Y, S1, CharAttr);
          Inc(I);
          Inc(J, 2);
        end;
      end;
      //如果用一句DrawText画出所有字符，则有可能被截断
      //DrawText(X, Y, S, CharAttr);
  end;
end;

procedure TCustomHexEdit.DrawOneRow(OffRow, AbsRow: Integer);
begin
  DrawAddrRow(OffRow, AbsRow);
  DrawHexRow(OffRow, AbsRow);
  DrawChrRow(OffRow, AbsRow);
end;

procedure TCustomHexEdit.DrawSpcRow(OffRow: Integer);
var
  X, Y: Integer;
begin
  X := GetAddrAreaLeft;
  Y := OffRow * FCharHeight;
  DrawText(X, Y, StringOfChar(' ', 74), FColors.HexColor);
end;

procedure TCustomHexEdit.DrawHexItem(OffRow, Col: Integer; HexCharAttr: TCharAttr);
var
  X, Y: Integer;
  RowPointer: PChar;
begin
  X := GetHexAreaLeft + FCharWidth * (Col*3);
  Y := OffRow * FCharHeight;
  RowPointer := GetRowColPointer(FTopRow + OffRow, Col);
  if GetOffset(FTopRow + OffRow, Col) < GetDataSize then
    DrawText(X, Y, IntToHex(Integer(RowPointer^), 2), HexCharAttr)
  else
    DrawText(X, Y, '  ', HexCharAttr);
end;

procedure TCustomHexEdit.DrawChrItem(OffRow, Col: Integer; ChrCharAttr: TCharAttr);
var
  RowPointer: PChar;
begin
  RowPointer := GetRowColPointer(FTopRow + OffRow, Col);
  if GetOffset(FTopRow + OffRow, Col) < GetDataSize then
    DrawStrForChrRow(RowPointer^, OffRow, Col, ChrCharAttr)
  else
    DrawStrForChrRow(' ', OffRow, Col, ChrCharAttr);
end;

procedure TCustomHexEdit.DrawItem(OffRow, Col: Integer; HexCharAttr, ChrCharAttr: TCharAttr);
var
  X, Y: Integer;
  RowPointer: PChar;
begin
  X := GetHexAreaLeft + FCharWidth * (Col*3);
  Y := OffRow * FCharHeight;
  RowPointer := GetRowColPointer(FTopRow + OffRow, Col);
  if GetOffset(FTopRow + OffRow, Col) < GetDataSize then
  begin
    DrawText(X, Y, IntToHex(Integer(RowPointer^), 2), HexCharAttr);
    DrawStrForChrRow(RowPointer^, OffRow, Col, ChrCharAttr);
  end else
  begin
    DrawText(X, Y, '  ', HexCharAttr);
    DrawStrForChrRow(' ', OffRow, Col, ChrCharAttr);
  end;
end;

procedure TCustomHexEdit.DrawCurItem(Show: Boolean);
var
  OffRow: Integer;
  X: Integer;
begin
  if not FFocused or not (hoShowCaret in FOptions) then Exit;

  OffRow := FCurRow - FTopRow;
  if Show then
  begin
    if FCurInHex then
    begin
      DrawChrItem(OffRow, FCurCol, FColors.CaretColor);
      X := (FCurCol*3 - FLeftCol) * FCharWidth;
      if not FCurInHigh then Inc(X, FCharWidth);
      SetCaretPos(GetHexAreaLeft + X, OffRow * FCharHeight);
      ShowCaret(Handle);
    end else
    begin
      DrawHexItem(OffRow, FCurCol, FColors.CaretColor);
      SetCaretPos(GetChrAreaLeft + (FCurCol - FLeftCol) * FCharWidth, OffRow * FCharHeight);
      ShowCaret(Handle);
    end;
  end else
  begin
    HideCaret(Handle);
    DrawItem(OffRow, FCurCol, GetCharAttrByRowCol(FCurRow, FCurCol, True),
      GetCharAttrByRowCol(FCurRow, FCurCol, True));
  end;
end;

procedure TCustomHexEdit.DrawCurItemHighlight(Show: Boolean);
var
  OffRow: Integer;
begin
  if FFocused or not (hoShowCaret in FOptions) then Exit;

  OffRow := FCurRow - FTopRow;
  if Show then
  begin
    DrawChrItem(OffRow, FCurCol, FColors.CaretColor);
    DrawHexItem(OffRow, FCurCol, FColors.CaretColor);
  end else
  begin
    DrawItem(OffRow, FCurCol, GetCharAttrByRowCol(FCurRow, FCurCol, True),
      GetCharAttrByRowCol(FCurRow, FCurCol, True));
  end;
end;

procedure TCustomHexEdit.DrawAllRow();
var
  AbsRow: Integer;
  I: Integer;
begin
  DrawCurItemHighlight(False);
  DrawCurItem(False);
  
  for I := 0 to FVisRowCount do  //本来是FVisRowCount-1, 但底下要多画一行
  begin
    AbsRow := FTopRow + I;
    if AbsRow < FRowCount then
      DrawOneRow(I, AbsRow)
    else
      DrawSpcRow(I);
  end;

  DrawCurItem(True);
  DrawCurItemHighlight(True);
end;

procedure TCustomHexEdit.DoUpKey(ShiftPressed: Boolean);
begin
  if hoShowCaret in FOptions then
  begin
    if ShiftPressed then
      AdjustSelectionA(GetOffset, GetOffsetByMove(-32))
    else
      CancelSelByUser(True);
    MoveCaret(-32);
    if ShiftPressed then DrawAllRow;
  end else
    DoCtrlUpKey(False);
end;

procedure TCustomHexEdit.DoDownKey(ShiftPressed: Boolean);
begin
  if hoShowCaret in FOptions then
  begin
    if ShiftPressed then
      AdjustSelectionB(GetOffset, GetOffsetByMove(32))
    else
      CancelSelByUser(True);
    MoveCaret(32);
    if ShiftPressed then DrawAllRow;
  end else
    DoCtrlDownKey(False);
end;

procedure TCustomHexEdit.DoLeftKey(ShiftPressed: Boolean);
begin
  if not (hoShowCaret in FOptions) then Exit;

  if FCurInHex then
  begin
    if ShiftPressed then
      AdjustSelectionA(GetOffset, GetOffsetByMove(-1))
    else
      CancelSelByUser(True);
    MoveCaret(-1);
  end else
  begin
    if ShiftPressed then
      AdjustSelectionA(GetOffset, GetOffsetByMove(-2))
    else
      CancelSelByUser(True);
    MoveCaret(-2);
  end;
  if ShiftPressed then DrawAllRow;
end;

procedure TCustomHexEdit.DoRightKey(ShiftPressed: Boolean);
begin
  if not (hoShowCaret in FOptions) then Exit;

  if FCurInHex then
  begin
    if ShiftPressed then
      AdjustSelectionB(GetOffset, GetOffsetByMove(1))
    else
      CancelSelByUser(True);
    MoveCaret(1);
  end else
  begin
    if ShiftPressed then
      AdjustSelectionB(GetOffset, GetOffsetByMove(2))
    else
      CancelSelByUser(True);
    MoveCaret(2);
  end;
  if ShiftPressed then DrawAllRow;
end;

procedure TCustomHexEdit.DoPageUpKey(ShiftPressed: Boolean);
var
  OffRow: Integer;
  Offset1, Offset2: Integer;
begin
  if FRowCount <= FVisRowCount then Exit;
  if (FTopRow = 0) and CheckCaretInWindow then Exit;

  Offset1 := GetOffset;

  if CheckCaretInWindow then
  begin
    OffRow := FCurRow - FTopRow;
    FTopRow := FTopRow - (FVisRowCount - 1);
    if FTopRow < 0 then FTopRow := 0;
    FCurRow := FTopRow + OffRow;
  end
  else if FCurRow > FTopRow then
  begin
    FCurRow := FCurRow - (FVisRowCount - 1);
    FTopRow := FCurRow - (FVisRowCount - 1);
    if FTopRow < 0 then FTopRow := 0;
    FCurRow := FTopRow + (FVisRowCount - 1);
  end
  else if FCurRow < FTopRow then
  begin
    FCurRow := FCurRow - (FVisRowCount - 1);
    if FCurRow < 0 then FCurRow := 0;
    FTopRow := FCurRow;
  end;

  Offset2 := GetOffset;
  if ShiftPressed then
    AdjustSelectionA(Offset1, Offset2)
  else
    CancelSelByUser(True);

  UpdateScrollPos(skVertical, FTopRow);
  DrawAllRow;
end;

procedure TCustomHexEdit.DoPageDownKey(ShiftPressed: Boolean);
var
  OffRow: Integer;
  Offset1, Offset2: Integer;
begin
  if FRowCount <= FVisRowCount then Exit;
  if (FTopRow+FVisRowCount-1 = FRowCount-1) and CheckCaretInWindow then Exit;

  Offset1 := GetOffset;

  if CheckCaretInWindow then
  begin
    OffRow := FCurRow - FTopRow;
    FTopRow := FTopRow + (FVisRowCount - 1);
    if FTopRow + FVisRowCount - 1 > FRowCount - 1 then
      FTopRow := FRowCount - FVisRowCount;
    FCurRow := FTopRow + OffRow;
    While GetOffset >= GetDataSize do
      Dec(FCurRow);
  end
  else if FCurRow > FTopRow then
  begin
    FCurRow := FCurRow + (FVisRowCount - 1);
    if FCurRow + FVisRowCount - 1 > FRowCount - 1 then
      FCurRow := FRowCount - FVisRowCount;
    FTopRow := FCurRow - (FVisRowCount - 1);
  end
  else if FCurRow < FTopRow then
  begin
    FCurRow := FCurRow + (FVisRowCount - 1);
    FTopRow := FCurRow;
    if FTopRow + FVisRowCount - 1 > FRowCount - 1 then
      FTopRow := FRowCount - FVisRowCount;
    FCurRow := FTopRow;
  end;

  Offset2 := GetOffset;
  if ShiftPressed then
    AdjustSelectionB(Offset1, Offset2)
  else
    CancelSelByUser(True);

  UpdateScrollPos(skVertical, FTopRow);
  DrawAllRow;
end;

procedure TCustomHexEdit.DoHomeKey(ShiftPressed: Boolean);
var
  Offset1, Offset2: Integer;
begin
  if hoShowCaret in FOptions then
  begin
    if (FCurCol > 0) or not FCurInHigh then
    begin
      Offset1 := GetOffset;
      FCurCol := 0;
      FCurInHigh := True;
      Offset2 := GetOffset;
      if ShiftPressed then
        AdjustSelectionA(Offset1, Offset2)
      else
        CancelSelByUser(False);
      DrawAllRow;
    end;
  end else
    DoCtrlPageUpKey(False);
end;

procedure TCustomHexEdit.DoEndKey(ShiftPressed: Boolean);
var
  Offset1, Offset2, CurDLen: Integer;
begin
  if hoShowCaret in FOptions then
  begin
    CurDLen := GetDLen;
    if (CurDlen mod 32 <> 0) and (CurDLen <> GetDataSize * 2) then
    begin
      Offset1 := GetOffset;
      if FCurRow = FRowCount - 1 then FCurCol := (GetDataSize-1) mod 16
      else FCurCol := 15;
      FCurInHigh := False;
      Offset2 := GetOffset;
      if ShiftPressed then
        AdjustSelectionB(Offset1, Offset2)
      else
        CancelSelByUser(False);
      DrawAllRow;
    end;
  end else
    DoCtrlPageDownKey(False);
end;

procedure TCustomHexEdit.DoTabKey;
begin
  DrawCurItem(False);
  FCurInHex := not FCurInHex;
  DrawCurItem(True);
end;

procedure TCustomHexEdit.DoCtrlJKey;
begin
  if DrawCharStyle = High(TDrawCharStyle) then
    DrawCharStyle := Low(TDrawCharStyle)
  else
    DrawCharStyle := Succ(FDrawCharStyle);
end;

procedure TCustomHexEdit.DoCtrlKKey(Key: Word);
begin
  if (Key >= Ord('0')) and (Key <= Ord('9')) then DoCtrlKNumKey(Key)
  else if (Key = Ord('B')) or (Key = Ord('b')) then DoCtrlKBKey(Key)
  else if (Key = Ord('K')) or (Key = Ord('k')) then DoCtrlKKKey(Key)
  else if (Key = Ord('H')) or (Key = Ord('h')) then DoCtrlKHKey(Key)
  else if (Key = Ord('W')) or (Key = Ord('w')) then DoCtrlKWKey(Key);
end;

procedure TCustomHexEdit.DoCtrlQKey(Key: Word);
begin
  if (Key >= Ord('0')) and (Key <= Ord('9')) then DoCtrlQNumKey(Key);
end;

procedure TCustomHexEdit.DoCtrlKNumKey(Key: Word);
var
  Idx: Integer;
begin
  Idx := Key - Ord('0');
  if Idx >= BookmarkCount then Exit;

  FBookmark[Idx].Active := True;
  FBookmark[Idx].DLen := GetDLen;
  FBookmark[Idx].CurInHex := FCurInHex;
end;

procedure TCustomHexEdit.DoCtrlKBKey(Key: Word);
begin
  if (hoAllowSelect in FOptions) and (hoShowCaret in FOptions) then
    SetSelectionStart(GetOffset, True);
end;

procedure TCustomHexEdit.DoCtrlKKKey(Key: Word);
begin
  if (hoAllowSelect in FOptions) and (hoShowCaret in FOptions) then
    SetSelectionEnd(GetOffset, True);
end;

procedure TCustomHexEdit.DoCtrlKHKey(Key: Word);
begin
  SetSelectionVisible(not FSelection.Active);
end;

procedure TCustomHexEdit.DoCtrlKWKey(Key: Word);
begin
  //{ TODO : DoCtrlKWKey }
end;

procedure TCustomHexEdit.DoCtrlQNumKey(Key: Word);
var
  Idx: Integer;
begin
  Idx := Key - Ord('0');
  if Idx >= BookmarkCount then Exit;
  if not FBookmark[Idx].Active then Exit;

  FCurInHex := FBookmark[Idx].CurInHex;
  MoveCaretTo(FBookmark[Idx].DLen);
end;

procedure TCustomHexEdit.DoCtrlUpKey(MoveCaret: Boolean);
begin
  if FTopRow <= 0 then Exit;
  Dec(FTopRow);
  if MoveCaret then
  begin
    if FCurRow < FTopRow then FCurRow := FTopRow;
    if FCurRow > FTopRow + FVisRowCount - 1 then
      FCurRow := FTopRow + FVisRowCount -1;
  end;

  UpdateScrollPos(skVertical, FTopRow);
  DrawAllRow;
end;

procedure TCustomHexEdit.DoCtrlDownKey(MoveCaret: Boolean);
begin
  if FTopRow >= FRowCount - FVisRowCount then Exit;
  Inc(FTopRow);
  if MoveCaret then
  begin
    if FCurRow < FTopRow then FCurRow := FTopRow;
    if FCurRow > FTopRow + FVisRowCount - 1 then
      FCurRow := FTopRow + FVisRowCount -1;
  end;

  UpdateScrollPos(skVertical, FTopRow);
  DrawAllRow;
end;

procedure TCustomHexEdit.DoCtrlLeftKey;
begin
  MoveCaret(-2);
end;

procedure TCustomHexEdit.DoCtrlRightKey;
begin
  MoveCaret(2);
end;

procedure TCustomHexEdit.DoCtrlPageUpKey(ShiftPressed: Boolean);
var
  Offset1, Offset2: Integer;
begin
  Offset1 := GetOffset;
  if Offset1 = 0 then Exit;
  MoveCaretTo(1);
  Offset2 := GetOffset;
  if ShiftPressed then
    AdjustSelectionA(Offset1, Offset2)
  else
    CancelSelByUser(False);
  DrawAllRow;
end;

procedure TCustomHexEdit.DoCtrlPageDownKey(ShiftPressed: Boolean);
var
  Offset1, Offset2: Integer;
begin
  Offset1 := GetOffset;
  if Offset1 = GetDataSize - 1 then Exit;
  MoveCaretTo(GetDataSize * 2 - 1);
  Offset2 := GetOffset;
  if ShiftPressed then
    AdjustSelectionB(Offset1, Offset2)
  else
    CancelSelByUser(False);
  DrawAllRow;
end;

procedure TCustomHexEdit.DoCharKey(Key: Char);
begin
  if hoEditing in FOptions then
  begin
    if FCurInHex then
      DoCharKeyForHexArea(Key)
    else
      DoCharKeyForChrArea(Key);
  end;
end;

procedure TCustomHexEdit.DoCharKeyForHexArea(Key: Char);
var
  P: PByte;
  B: Byte;
  Offset: Integer;
begin
  if not IsHexChar(Key) then Exit;

  Offset := GetOffset;
  CreateUndoForModify(Offset, 1, 1);
  
  P := PByte(GetRowColPointer(FCurRow, FCurCol));
  if IsAlphaChar(Key) then
    B := Ord(UpperCase(Key)[1]) - Ord('A') + 10
  else
    B := Ord(Key) - Ord('0');

  if FCurInHigh then
  begin
    P^ := P^ and $0F;
    P^ := P^ or (B shl 4);
  end else
  begin
    P^ := P^ and $F0;
    P^ := P^ or B;
  end;
  DrawCurItem(False);
  DrawCurItem(True);

  FModified := True;
  DoRightKey(False);
  CreateRedoForModify(Offset, 1, 1);
  DoOnChange;
end;

procedure TCustomHexEdit.DoCharKeyForChrArea(Key: Char);
var
  P: PChar;
  Offset: Integer;
begin
  if Ord(Key) < 32 then Exit;

  Offset := GetOffset;
  CreateUndoForModify(Offset, 1, 1);

  P := GetRowColPointer(FCurRow, FCurCol);
  P^ := Key;

  DrawCurItem(False);
  DrawCurItem(True);

  FModified := True;
  DoRightKey(False);
  CreateRedoForModify(Offset, 1, 1);
  DoOnChange;
end;

function TCustomHexEdit.CreateUndoForModify(Offset, OldCount, NewCount: Integer): Boolean;
var
  ActPtr: PHexEditAct;
begin
  ActPtr := FActsMgr.AddUndoItem;
  ActPtr^.ActType := atModify;
  GetStrAtPos(Offset, OldCount, ActPtr^.Buf);
  ActPtr^.Offset := Offset;
  ActPtr^.Count := NewCount;
  ActPtr^.CurPos := GetDLen;
  Result := True;
end;

function TCustomHexEdit.CreateUndoForInsert(Offset, Count: Integer): Boolean;
var
  ActPtr: PHexEditAct;
begin
  ActPtr := FActsMgr.AddUndoItem;
  ActPtr^.ActType := atInsert;
  ActPtr^.Buf := '';
  ActPtr^.Offset := Offset;
  ActPtr^.Count := Count;
  ActPtr^.CurPos := GetDLen;
  Result := True;
end;

function TCustomHexEdit.CreateUndoForDelete(Offset, Count: Integer): Boolean;
var
  ActPtr: PHexEditAct;
begin
  ActPtr := FActsMgr.AddUndoItem;
  ActPtr^.ActType := atDelete;
  GetStrAtPos(Offset, Count, ActPtr^.Buf);
  ActPtr^.Offset := Offset;
  ActPtr^.Count := Count;
  ActPtr^.CurPos := GetDLen;
  Result := True;
end;

function TCustomHexEdit.CreateRedoForModify(Offset, OldCount, NewCount: Integer): Boolean;
var
  ActPtr: PHexEditAct;
begin
  ActPtr := FActsMgr.AddRedoItem;
  ActPtr^.ActType := atModify;
  GetStrAtPos(Offset, NewCount, ActPtr^.Buf);
  ActPtr^.Offset := Offset;
  ActPtr^.Count := OldCount;
  ActPtr^.CurPos := GetDLen;
  Result := True;
end;

function TCustomHexEdit.CreateRedoForInsert(Offset, Count: Integer): Boolean;
var
  ActPtr: PHexEditAct;
begin
  ActPtr := FActsMgr.AddRedoItem;
  ActPtr^.ActType := atInsert;
  GetStrAtPos(Offset, Count, ActPtr^.Buf);
  ActPtr^.Offset := Offset;
  ActPtr^.Count := Count;
  ActPtr^.CurPos := GetDLen;
  Result := True;
end;

function TCustomHexEdit.CreateRedoForDelete(Offset, Count: Integer): Boolean;
var
  ActPtr: PHexEditAct;
begin
  ActPtr := FActsMgr.AddRedoItem;
  ActPtr^.ActType := atDelete;
  ActPtr^.Buf := '';
  ActPtr^.Offset := Offset;
  ActPtr^.Count := Count;
  ActPtr^.CurPos := GetDLen;
  Result := True;
end;

procedure TCustomHexEdit.DoCreateCaret;
begin
  CreateCaret(Handle, 0, FCharWidth, FCharHeight);
end;

procedure TCustomHexEdit.DoMouseRoll;
var
  MsPos: TPoint;
  DLen, AOffset: Integer;
begin
  GetCursorPos(MsPos);
  MsPos := ScreenToClient(MsPos);

  while FMouseDownFlag and (MsPos.y < 0) do
  begin
    if FTopRow > 0 then
    begin
      Dec(FTopRow);
      DLen := GetDLen(FTopRow, FCurCol, True);
      AOffset := GetOffsetByMoveTo(DLen);

      MoveCaretTo(DLen);
      SetSelection(FMsDownOffset, AOffset, True);
    end;

    GetCursorPos(MsPos);
    MsPos := ScreenToClient(MsPos);
    Sleep(30);
    //Application.ProcessMessages;
  end;

  while FMouseDownFlag and (MsPos.y > ClientHeight) do
  begin
    if FTopRow + FVisRowCount < FRowCount then
    begin
      Inc(FTopRow);
      DLen := GetDLen(FTopRow + FVisRowCount - 1, FCurCol, True);
      AOffset := GetOffsetByMoveTo(DLen);

      MoveCaretTo(DLen);
      SetSelection(FMsDownOffset, AOffset, True);
    end;

    GetCursorPos(MsPos);
    MsPos := ScreenToClient(MsPos);
    Sleep(30);
    //Application.ProcessMessages;
  end;
end;

procedure TCustomHexEdit.DoPaint;
begin
  if not IsEmpty then DrawAllRow;
end;

procedure TCustomHexEdit.DoResize;
begin
  if not HandleAllocated or (FCharHeight = 0) then Exit;
  
  FVisRowCount := ClientHeight div FCharHeight;
  if FLeftCol > 0 then
  begin
    if ClientWidth >= GetRowTotalWidth - FLeftCol * FCharWidth then
    begin
      FLeftCol := (GetRowTotalWidth - ClientWidth) div FCharWidth;
      if FLeftCol < 0 then FLeftCol := 0;
    end;
  end;
  UpdateVertScrollBar;
  UpdateHorzScrollBar;
  DoPaint;
end;

//处理滚动条消息
//返回：  滚动条的Position
function TCustomHexEdit.DoScroll(AScrollKind: TScrollKind; Msg: TWMScroll): Integer;
var
  BarFlag: Integer;
  MaxPos: Integer;
  ScrInfo: TScrollInfo;
begin
  if AScrollKind = skHorizontal then
    BarFlag := SB_HORZ
  else
    BarFlag := SB_VERT;

  FillChar(ScrInfo, SizeOf(ScrInfo), 0);
  ScrInfo.cbSize := SizeOf(ScrInfo);
  ScrInfo.fMask := SIF_ALL;
  GetScrollInfo(Handle, BarFlag, ScrInfo);

  MaxPos := ScrInfo.nMax - Integer(ScrInfo.nPage) + 1;
  case Msg.ScrollCode of
    SB_LINEUP:
      if ScrInfo.nPos > ScrInfo.nMin then
      begin
        Dec(ScrInfo.nPos);
      end;
    SB_LINEDOWN:
      if ScrInfo.nPos < MaxPos then
      begin
        Inc(ScrInfo.nPos);
      end;
    SB_PAGEUP:
      if ScrInfo.nPos > ScrInfo.nMin then
      begin
        Dec(ScrInfo.nPos, ScrInfo.nPage);
      end;
    SB_PAGEDOWN:
      if ScrInfo.nPos < MaxPos then
      begin
        Inc(ScrInfo.nPos, ScrInfo.nPage);
      end;
    SB_THUMBPOSITION,
    SB_THUMBTRACK:
      begin
        //ScrInfo.nPos := Msg.Pos;   //Msg.Pos是16-bits的, 行数太多时会溢出
        ScrInfo.nPos := ScrInfo.nTrackPos;
      end;
    SB_LEFT:
      if ScrInfo.nPos <> ScrInfo.nMin then
      begin
        ScrInfo.nPos := ScrInfo.nMin;
      end;
    SB_RIGHT:
      if ScrInfo.nPos < MaxPos then
      begin
        ScrInfo.nPos := MaxPos;
      end;
    SB_ENDSCROLL:
      begin
        ScrInfo.nPos := ScrInfo.nPos;
      end;
  end;

  if ScrInfo.nPos < ScrInfo.nMin then ScrInfo.nPos := ScrInfo.nMin;
  if ScrInfo.nPos > MaxPos then ScrInfo.nPos := MaxPos;
  UpdateScrollPos(AScrollKind, ScrInfo.nPos);
  Result := ScrInfo.nPos;
end;

procedure TCustomHexEdit.DoOnChange;
begin
  if Assigned(FOnChange) then
    FOnChange(Self);
end;

procedure TCustomHexEdit.DoOnSelectionChange;
begin
  if Assigned(FOnSelectionChange) then
    FOnSelectionChange(Self);
end;

procedure TCustomHexEdit.DoOnCaretMove;
begin
  if Assigned(FOnCaretMove) then
    FOnCaretMove(Self);
end;

procedure TCustomHexEdit.InternalInsertData(Offset: Integer; const S: string);
var
  Temp: string;
begin
  if Length(S) > 0 then
    InsertData(Offset, S[1], Length(S), False)
  else begin
    Temp := ' ';
    InsertData(Offset, Temp[1], 0, False);
  end;
end;

procedure TCustomHexEdit.InternalDeleteData(Offset, Count: Integer);
begin
  DeleteData(Offset, Count, False);
end;

procedure TCustomHexEdit.InternalModifyData(Offset: Integer; const S: string);
var
  Temp: string;
begin
  if Length(S) > 0 then
    ModifyData(Offset, S[1], Length(S), False)
  else begin
    Temp := ' ';
    ModifyData(Offset, Temp[1], 0, False);
  end;
end;

procedure TCustomHexEdit.InternalModifyData(Offset, Count: Integer; const S: string);
var
  Temp: string;
begin
  if Length(S) > 0 then
    ModifyData(Offset, Count, S[1], Length(S), False)
  else begin
    Temp := ' ';
    ModifyData(Offset, Count, Temp[1], 0, False);
  end;
end;

procedure TCustomHexEdit.CMFontChanged(var Message: TMessage);
begin
  inherited;
  if Canvas <> nil then
  begin
    Canvas.Font := Font;
    InitCharSize;
    DoResize;
  end;
end;

procedure TCustomHexEdit.WMGetDlgCode(var Msg: TWMGetDlgCode);
begin
  Msg.Result := DLGC_WANTARROWS or DLGC_WANTTAB or DLGC_WANTCHARS;
end;

procedure TCustomHexEdit.WMHScroll(var Msg: TWMHScroll);
var
  Pos: Integer;
begin
  Pos := DoScroll(skHorizontal, Msg);
  if Pos <> FLeftCol then
  begin
    FLeftCol := Pos;
    DrawAllRow;
  end;
end;

procedure TCustomHexEdit.WMVScroll(var Msg: TWMVScroll);
var
  Pos: Integer;
begin
  Pos := DoScroll(skVertical, Msg);
  if Pos <> FTopRow then
  begin
    FTopRow := Pos;
    DrawAllRow;
  end;
end;

procedure TCustomHexEdit.WMSetFocus(var Message: TWMSetFocus);
begin
  inherited;
  DrawCurItemHighlight(False);
  FFocused := True;
  DoCreateCaret;
  DrawCurItem(True);
end;

procedure TCustomHexEdit.WMKillFocus(var Message: TWMSetFocus);
begin
  inherited;
  DrawCurItem(False);
  DestroyCaret;
  FFocused := False;
  DrawCurItemHighlight(True);
end;

procedure TCustomHexEdit.CreateParams(var Params: TCreateParams);
begin
  inherited CreateParams(Params);
  with Params do
  begin
    Style := Style or WS_TABSTOP;
    if FScrollBars in [ssVertical, ssBoth] then Style := Style or WS_VSCROLL;
    if FScrollBars in [ssHorizontal, ssBoth] then Style := Style or WS_HSCROLL;
    WindowClass.style := CS_DBLCLKS;
    if FBorderStyle = bsSingle then
      if NewStyleControls and Ctl3D then
      begin
        Style := Style and not WS_BORDER;
        ExStyle := ExStyle or WS_EX_CLIENTEDGE;
      end
      else
        Style := Style or WS_BORDER;
  end;
end;

procedure TCustomHexEdit.Paint;
begin
  inherited;
  DoPaint;
end;

procedure TCustomHexEdit.DoEnter;
begin
  inherited;
end;

procedure TCustomHexEdit.DoExit;
begin
  inherited;
end;

procedure TCustomHexEdit.KeyDown(var Key: Word; Shift: TShiftState);
var
  ShiftPressed: Boolean;
begin
  inherited;
  if IsEmpty then Exit;

  ShiftPressed := ssShift in Shift;
  Shift := Shift - [ssShift];

  if Shift = [] then
  begin
    case Key of
      VK_UP:         DoUpKey(ShiftPressed);
      VK_DOWN:       DoDownKey(ShiftPressed);
      VK_LEFT:       DoLeftKey(ShiftPressed);
      VK_RIGHT:      DoRightKey(ShiftPressed);
      VK_PRIOR:      DoPageUpKey(ShiftPressed);
      VK_NEXT:       DoPageDownKey(ShiftPressed);
      VK_HOME:       DoHomeKey(ShiftPressed);
      VK_END:        DoEndKey(ShiftPressed);
      VK_TAB:        DoTabKey;
    end;
  end
  else if Shift = [ssCtrl] then
  begin
    case Key of
      VK_UP:         DoCtrlUpKey;
      VK_DOWN:       DoCtrlDownKey;
      VK_LEFT:       DoCtrlLeftKey;
      VK_RIGHT:      DoCtrlRightKey;
      VK_HOME,
      VK_PRIOR:      DoCtrlPageUpKey(ShiftPressed);
      VK_END,
      VK_NEXT:       DoCtrlPageDownKey(ShiftPressed);
      Ord('J'):      DoCtrlJKey;
      Ord('Z'):      Undo;
      Ord('Y'):      Redo;
      Ord('X'):      CutToClipboard;
      Ord('C'):      CopyToClipboard;
      Ord('V'):      PasteFromClipboard;
      Ord('A'):      SelectAll;
      Ord('0')..Ord('9'):
        begin
          if ShiftPressed then DoCtrlKKey(Key)
          else DoCtrlQKey(Key);
        end;
    end;
  end
  else if Shift = [ssAlt] then
  begin
  end;
end;

procedure TCustomHexEdit.KeyUp(var Key: Word; Shift: TShiftState);
begin
  inherited;
end;

procedure TCustomHexEdit.KeyPress(var Key: Char);
begin
  inherited;
  if IsEmpty then Exit;

  if Key = #11 then  //Ctrl+K
  begin
    FCtrlKPressed := True;
    Exit;
  end;
  if Key = #17 then  //Ctrl+Q
  begin
    FCtrlQPressed := True;
    Exit;
  end;

  if FCtrlKPressed then
  begin
    DoCtrlKKey(Ord(Key));
    FCtrlKPressed := False;
    Exit;
  end
  else if FCtrlQPressed then
  begin
    DoCtrlQKey(Ord(Key));
    FCtrlQPressed := False;
    Exit;
  end;

  DoCharKey(Key);
end;

procedure TCustomHexEdit.MouseDown(Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
var
  OldOffset: Integer;
  OffRow, Col, DLen: Integer;
  CurInHex, CurInHigh: Boolean;
begin
  inherited;
  SetFocus;

  if FMouseDblClkFlag then
  begin
    FMouseDblClkFlag := False;
    Exit;
  end;

  if XYToRowCol(OffRow, Col, CurInHex, CurInHigh, X, Y) then
  begin
    OldOffset := GetOffset;
    FCurInHex := CurInHex;
    DLen := GetDLen(FTopRow + OffRow, Col, CurInHigh);
    MoveCaretTo(DLen, True);

    if Button = mbLeft then
    begin
      FMouseDownFlag := True;
      FMsMoveSameItem := True;
      if ssShift in Shift then
      begin
        if SelLength > 0 then
        begin
          if OldOffset = SelStart then
            OldOffset := SelStart + SelLength - 1
          else
            OldOffset := SelStart;
        end;
        SetSelection(OldOffset, GetOffset, True);
      end else
        CancelSelByUser(True);
    end else
    begin
      if not CheckInSelection(DLenToOffset(DLen)) then
        CancelSelByUser(True);
    end;
    FMsDownOffset := GetOffset;
  end;
end;

{$WRITEABLECONST ON}
procedure TCustomHexEdit.MouseMove(Shift: TShiftState; X, Y: Integer);
const
  OldOffRow: Integer = -1;
  OldCol: Integer = -1;
var
  OffRow, Col, DLen: Integer;
  CurInHex, CurInHigh: Boolean;
begin
  inherited;
  if FMouseDownFlag and (hoAllowSelect in FOptions) then
  begin
    if XYToRowCol(OffRow, Col, CurInHex, CurInHigh, X, Y) then
    begin
      if FMsMoveSameItem and (FMsDownOffset = GetOffset(FTopRow + OffRow, Col)) then Exit;
      FMsMoveSameItem := False;
      DLen := GetDLen(FTopRow + OffRow, Col, True);
      MoveCaretTo(DLen);
      if (OldOffRow <> OffRow) or (OldCol <> Col) then
      begin
        SetSelection(FMsDownOffset, GetOffset, True);
        OldOffRow := OffRow;
        OldCol := Col;
      end;
    end else
      DoMouseRoll;
  end;
end;
{$WRITEABLECONST OFF}

procedure TCustomHexEdit.MouseUp(Button: TMouseButton; Shift: TShiftState;
  X, Y: Integer);
begin
  inherited;
  FMouseDownFlag := False;
end;

function TCustomHexEdit.DoMouseWheel(Shift: TShiftState; WheelDelta: Integer; MousePos: TPoint): Boolean;
begin
  Result := inherited DoMouseWheel(Shift, WheelDelta, MousePos);
end;

function TCustomHexEdit.DoMouseWheelDown(Shift: TShiftState; MousePos: TPoint): Boolean;
begin
  Result := inherited DoMouseWheelDown(Shift, MousePos);
  DoCtrlDownKey(False);
end;

function TCustomHexEdit.DoMouseWheelUp(Shift: TShiftState; MousePos: TPoint): Boolean;
begin
  Result := inherited DoMouseWheelUp(Shift, MousePos);
  DoCtrlUpKey(False);
end;

procedure TCustomHexEdit.Click;
begin
  inherited;
end;

procedure TCustomHexEdit.DblClick;
begin
  inherited;
  FMouseDblClkFlag := True;
end;

procedure TCustomHexEdit.Resize;
begin
  inherited;
  DoResize;
end;

procedure TCustomHexEdit.Loaded;
begin
  inherited;
  InitFont;
  InitCharSize;
end;

procedure TCustomHexEdit.AdjustSize;
begin
  inherited;
end;

function TCustomHexEdit.CanUndo: Boolean;
begin
  Result := FActsMgr.CanUndo;
end;

function TCustomHexEdit.CanRedo: Boolean;
begin
  Result := FActsMgr.CanRedo;
end;

function TCustomHexEdit.Undo: Boolean;
var
  ActPtr: PHexEditAct;
begin
  Result := FActsMgr.CanUndo;
  if not Result then Exit;

  ActPtr := FActsMgr.Undo;
  case ActPtr^.ActType of
    atModify:
      begin
        InternalModifyData(ActPtr^.Offset, ActPtr^.Count, ActPtr^.Buf);
        MoveCaretTo(ActPtr^.CurPos);
      end;
    atInsert:
      begin
        InternalDeleteData(ActPtr^.Offset, ActPtr^.Count);
        MoveCaretTo(ActPtr^.CurPos);
      end;
    atDelete:
      begin
        InternalInsertData(ActPtr^.Offset, ActPtr^.Buf);
        MoveCaretTo(ActPtr^.CurPos);
      end;
  end;
end;

function TCustomHexEdit.Redo: Boolean;
var
  ActPtr: PHexEditAct;
begin
  Result := FActsMgr.CanRedo;
  if not Result then Exit;

  ActPtr := FActsMgr.Redo;
  case ActPtr^.ActType of
    atModify:
      begin
        InternalModifyData(ActPtr^.Offset, ActPtr^.Count, ActPtr^.Buf);
        MoveCaretTo(ActPtr^.CurPos);
      end;
    atInsert:
      begin
        InternalInsertData(ActPtr^.Offset, ActPtr^.Buf);
        MoveCaretTo(ActPtr^.CurPos);
      end;
    atDelete:
      begin
        InternalDeleteData(ActPtr^.Offset, ActPtr^.Count);
        MoveCaretTo(ActPtr^.CurPos);
      end;
  end;
end;

function TCustomHexEdit.CanPaste: Boolean;
var
  Data: THandle;
begin
  if Clipboard.HasFormat(CF_HEXEDITDATA) then
  begin
    Clipboard.Open;
    try
      Data := Clipboard.GetAsHandle(CF_HEXEDITDATA);
      Result := GlobalSize(Data) > 0;
    finally
      Clipboard.Close;
    end;
  end else
  begin
    Result := Length(Clipboard.AsText) > 0; 
  end;
end;

procedure TCustomHexEdit.CutToClipboard;
var
  Size, BakPos: Integer;
begin
  if not FSelection.Active then Exit;
  if hoEditing in FOptions then
  begin
    Size := GetSelLength;
    BakPos := FSelection.StartOffset;
    if Size = GetDataSize then
      raise EHexEdit.Create(SHEDeleteError);

    CreateUndoForDelete(BakPos, Size);
    CreateRedoForDelete(BakPos, Size);

    CopyToClipboard;
    DeleteSelection;
    MoveCaretTo(OffsetToDLen(BakPos));
  end;
end;

procedure TCustomHexEdit.CopyToClipboard;
var
  S: string;
  Size: Integer;
  Data: THandle;
  DataPtr: Pointer;
begin
  Size := GetSelLength;
  if Size = 0 then Exit;

  Clipboard.Open;
  try
    // Copy as CF_TEXT format
    SetString(S, GetDataAddr + FSelection.StartOffset, Size);
    Clipboard.AsText := S;

    // Copy as CF_HEXEDITDATA format
    Data := GlobalAlloc(GMEM_MOVEABLE + GMEM_DDESHARE, Size);
    try
      DataPtr := GlobalLock(Data);
      try
        Move((GetDataAddr + FSelection.StartOffset)^, DataPtr^, Size);
        Clipboard.SetAsHandle(CF_HEXEDITDATA, Data);
      finally
        GlobalUnlock(Data);
      end;
    except
      GlobalFree(Data);
      raise;
    end;
  finally
    Clipboard.Close;
  end;
end;

procedure TCustomHexEdit.PasteFromClipboard;
var
  S: string;
  Offset, Size: Integer;
  Data: THandle;
  DataPtr: Pointer;
begin
  if hoEditing in FOptions then
  begin
    S := '';
    if Clipboard.HasFormat(CF_HEXEDITDATA) then
    begin
      Data := Clipboard.GetAsHandle(CF_HEXEDITDATA);
      DataPtr := GlobalLock(Data);
      try
        Size := GlobalSize(Data);
        if Size > 0 then
          SetString(S, PChar(DataPtr), Size);
      finally
        GlobalUnlock(Data);
      end;
    end else
    begin
      S := Clipboard.AsText;
      Size := Length(S);
    end;

    if Length(S) > 0 then
    begin
      Offset := GetOffset;
      CreateUndoForInsert(Offset, Size);
      InternalInsertData(GetOffset, S);
      CreateRedoForInsert(Offset, Size);
    end;
  end;
end;

procedure TCustomHexEdit.SelectAll;
begin
  SetSelection(0, GetDataSize - 1, True);
end;

function TCustomHexEdit.IsEmpty: Boolean;
begin
  Result := (GetDataSize = 0);
end;

procedure TCustomHexEdit.MoveCaret(MoveMode: TMoveCaretMode);
begin
  case MoveMode of
    mcStart:     DoCtrlPageUpKey(False);
    mcEnd:       DoCtrlPageDownKey(False);
    mcLineStart: DoHomeKey(False);
    mcLineEnd:   DoEndKey(False);
    mcSelStart:
      if SelLength > 0 then
        SetOffset(SelStart);
    mcSelEnd:
      if SelLength > 0 then
        SetOffset(SelStart + SelLength - 1);
  end;
end;

procedure TCustomHexEdit.SetBookmark(BkmkIndex: Integer);
begin
  DoCtrlKNumKey(Ord('0') + BkmkIndex);
end;

procedure TCustomHexEdit.GotoBookmark(BkmkIndex: Integer);
begin
  DoCtrlQNumKey(Ord('0') + BkmkIndex);
end;

function TCustomHexEdit.BookmarkExists(BkmkIndex: Integer): Boolean;
begin
  if (BkmkIndex < 0) or (BkmkIndex >= BookmarkCount) then
    Result := False
  else
    Result := FBookmark[BkmkIndex].Active;
end;

procedure TCustomHexEdit.LoadFromFile(const FileName: string);
var
  FileStream: TFileStream;
begin
  FileStream := TFileStream.Create(FileName, fmOpenRead or fmShareDenyNone);
  try
    FStream.LoadFromStream(FileStream);
    if GetDataSize = 0 then
    begin
      FStream.SetSize(1);
      ZeroMemory(FStream.Memory, 1);
    end;
    DataLoaded;
  finally
    FileStream.Free;
  end;
end;

procedure TCustomHexEdit.SaveToFile(const FileName: string);
begin
  FStream.SaveToFile(FileName);
end;

procedure TCustomHexEdit.LoadFromStream(Stream: TStream);
begin
  FStream.LoadFromStream(Stream);
  if GetDataSize = 0 then
  begin
    FStream.SetSize(1);
    ZeroMemory(FStream.Memory, 1);
  end;
  DataLoaded;
end;

procedure TCustomHexEdit.SaveToStream(Stream: TStream);
begin
  FStream.SaveToStream(Stream);
end;

procedure TCustomHexEdit.LoadFromBuffer(const Buffer; Count: Integer);
begin
  FStream.Clear;
  FStream.WriteBuffer(Buffer, Count);
  if GetDataSize = 0 then
  begin
    FStream.SetSize(1);
    ZeroMemory(FStream.Memory, 1);
  end;
  DataLoaded;
end;

function TCustomHexEdit.SaveToBuffer(var Buffer; Count: Integer): Longint;
begin
  FStream.Position := 0;
  Result := FStream.Read(Buffer, Count);
end;

procedure TCustomHexEdit.GetEditorText(Lines: TStrings; IncludeAddr, OnlyBlock: Boolean);
var
  AddrStr, HexStr, ChrStr, RowStr: string;
  StartRow, EndRow: Integer;
  I, J: Integer;
begin
  Lines.BeginUpdate;
  Lines.Clear;
  try
    if OnlyBlock then
    begin
      if SelLength = 0 then Exit;
      StartRow := SelStart div 16;
      EndRow := (SelStart + SelLength) div 16;
    end else
    begin
      StartRow := 0;
      EndRow := FRowCount - 1;
    end;

    for I := StartRow to EndRow do
    begin
      AddrStr := IntToHex(I * 16, 8) + ': ';
      HexStr := GetHexAreaStr(I);
      ChrStr := GetChrAreaStr(I);

      for J := 1 to Length(ChrStr) do
        if IsSpecChar(ChrStr[J]) then ChrStr[J] := '.';

      if OnlyBlock then    //处理块首块尾的空格
      begin
        if I = StartRow then
        begin
          for J := 1 to (SelStart mod 16) do
          begin
            HexStr[(J-1)*3+1] := ' ';
            HexStr[(J-1)*3+2] := ' ';
            HexStr[(J-1)*3+3] := ' ';
            ChrStr[J] := ' ';
          end;
        end;
        if I = EndRow then
        begin
          for J := ((SelStart + SelLength) mod 16) + 2 to 16 do
          begin
            HexStr[(J-1)*3+1] := ' ';
            HexStr[(J-1)*3+2] := ' ';
            HexStr[(J-1)*3+3] := ' ';
            ChrStr[J] := ' ';
          end;
        end;
      end;
      if IncludeAddr then
        RowStr := AddrStr + HexStr + ChrStr
      else
        RowStr := HexStr + ChrStr;

      Lines.Add(RowStr);
    end;
  finally
    Lines.EndUpdate;
  end;
end;

procedure TCustomHexEdit.InsertData(Offset: Integer;
  const Buffer; BufferSize: Integer; AllowUndo: Boolean);
var
  CurPtr: PChar;
  OldDataSize: Integer;
begin
  if (Offset >= 0) and (Offset <= GetDataSize) and (BufferSize > 0) then
  begin
    OldDataSize := GetDataSize;
    if AllowUndo then CreateUndoForInsert(Offset, BufferSize);
    FStream.SetSize(GetDataSize + BufferSize);
    CurPtr := PChar(FStream.Memory) + Offset;
    Move(CurPtr^, (CurPtr + BufferSize)^, OldDataSize - Offset);
    Move(Buffer, CurPtr^, BufferSize);
    if AllowUndo then CreateRedoForInsert(Offset, BufferSize);

    FRowCount := GetRowCount(GetDataSize);
    UpdateVertScrollBar;
    FModified := True;
    DoPaint;
    DoOnChange;
  end;
end;

procedure TCustomHexEdit.DeleteData(Offset, Count: Integer; AllowUndo: Boolean);
var
  CurPtr: PChar;
begin
  if Offset + Count > GetDataSize then
    Count := GetDataSize - Offset;
  if (Offset >= 0) and (Count > 0) then
  begin
    if GetDataSize <= Count then
      raise EHexEdit.Create(SHEDeleteError);

    if AllowUndo then
    begin
      CreateUndoForDelete(Offset, Count);
      CreateRedoForDelete(Offset, Count);
    end;
    CurPtr := PChar(FStream.Memory) + Offset;
    Move((CurPtr + Count)^, CurPtr^, GetDataSize - Offset - Count);
    FStream.SetSize(GetDataSize - Count);

    FRowCount := GetRowCount(GetDataSize);
    RestrictCaret;
    RestrictSelection;
    UpdateVertScrollBar;
    FModified := True;
    DoPaint;
    DoOnChange;
  end;
end;

procedure TCustomHexEdit.DeleteSelection(AllowUndo: Boolean);
var
  StartOffset: Integer;
begin
  if FSelection.Active then
  begin
    StartOffset := SelStart;
    DeleteData(FSelection.StartOffset, GetSelLength, AllowUndo);
    CancelSelection(True);
    Offset := StartOffset;
  end;
end;

procedure TCustomHexEdit.ModifyData(Offset: Integer;
  const Buffer; BufferSize: Integer; AllowUndo: Boolean);
var
  CurPtr: PChar;
begin
  if Offset + BufferSize > GetDataSize then
    BufferSize := GetDataSize - Offset;
  if (Offset >= 0) and (BufferSize > 0) then
  begin
    if AllowUndo then CreateUndoForModify(Offset, BufferSize, BufferSize);
    CurPtr := PChar(FStream.Memory) + Offset;
    Move(Buffer, CurPtr^, BufferSize);
    if AllowUndo then CreateRedoForModify(Offset, BufferSize, BufferSize);
    FModified := True;
    DoPaint;
    DoOnChange;
  end;
end;

//把偏移为Offset处的Count个字节替换为Buffer.
procedure TCustomHexEdit.ModifyData(Offset, Count: Integer;
  const Buffer; BufferSize: Integer; AllowUndo: Boolean);
var
  CurPtr: PChar;
  NewCount, NewBufSize: Integer;
begin
  if Offset + Count > GetDataSize then
    Count := GetDataSize - Offset;
  if (Offset >= 0) and (Count >= 0) then
  begin
    NewCount := BufferSize;
    NewBufSize := GetDataSize + NewCount - Count;
    if NewBufSize <= 0 then
      raise EHexEdit.Create(SHEDeleteError);

    if AllowUndo then CreateUndoForModify(Offset, Count, NewCount);
    CurPtr := PChar(FStream.Memory) + Offset;
    if NewCount > Count then
    begin
      FStream.SetSize(NewBufSize);
      Move((CurPtr+Count)^, (CurPtr+NewCount)^, GetDataSize - Offset - Count);
    end else
    begin
      Move((CurPtr+Count)^, (CurPtr+NewCount)^, GetDataSize - Offset - Count);
      if NewCount <> Count then FStream.SetSize(NewBufSize);
    end;
    Move(Buffer, CurPtr^, NewCount);
    if AllowUndo then CreateRedoForModify(Offset, Count, NewCount);

    FRowCount := GetRowCount(GetDataSize);
    RestrictCaret;
    RestrictSelection;
    UpdateVertScrollBar;
    FModified := True;
    DoPaint;
    DoOnChange;
  end;
end;

function TCustomHexEdit.SearchText(const Text: string; FromCursor: Boolean): Boolean;
var
  I, J, K, Len: Integer;
  StartOffset, EndOffset: Integer;
  FindStr, S: string;
  FindStrW: WideString;
  Same: Boolean;
  Ch: Char;
begin
  if SearchTextOptions.MatchCase then
    FindStr := Text
  else
    FindStr := UpperCase(Text);
  if SearchTextOptions.UseWild then
  begin
    I := 1;
    while I <= Length(FindStr) do
    begin
      if FindStr[I] = SearchTextOptions.WildChar then
      begin
        for J := 1 to SearchTextOptions.WildCount - 1 do
          Insert(SearchTextOptions.WildChar, FindStr, I);
        Inc(I, SearchTextOptions.WildCount - 1);
      end;
      Inc(I, 1);
    end;
  end;
  if SearchTextOptions.Unicode then
  begin
    S := FindStr;
    FindStrW := WideString(S);
    Len := (Length(FindStrW)+1)*2;
    SetLength(FindStr, Len);
    StringToWideChar(S, @FindStr[1], Len);
    SetLength(FindStr, Length(FindStrW)*2);
    if SearchTextOptions.UseWild then
    begin
      I := 1;
      while I < Length(FindStr) do
      begin
        if (FindStr[I] = SearchTextOptions.WildChar) and (FindStr[I+1] = #0) then
          Delete(FindStr, I+1, 1)
        else
          Inc(I);
      end;
    end;
  end;

  GetOffsetRange(StartOffset, EndOffset, SearchTextOptions.OnlyBlock, FromCursor);
  Len := Length(FindStr);
  for I := StartOffset to EndOffset - Len + 1 do
  begin
    Same := True;

    if SearchTextOptions.WholeWord then
    begin
      K := I - 1;
      if K >= 0 then
        if not IsBorderChar(GetDataAddr[K]) then
          Same := False;
      K := I + Len;
      if K < GetDataSize then
        if not IsBorderChar(GetDataAddr[K]) then
          Same := False;
    end;
    if not Same then Continue;

    for J := 0 to Len - 1 do
    begin
      if SearchTextOptions.UseWild then
      begin
        if FindStr[J+1] = SearchTextOptions.WildChar then Continue;
      end;
      Ch := GetDataAddr[I+J];
      if not SearchTextOptions.MatchCase then Ch := CharUpperCase(Ch);

      if FindStr[J+1] <> Ch then
      begin
        Same := False;
        Break;
      end;
    end;

    if Same then
    begin
      SetOffset(I);
      Result := True;
      Exit;
    end;
  end; //for I

  Result := False;
end;

function TCustomHexEdit.SearchHex(const HexStr: string; FromCursor: Boolean): Boolean;

  function HexStrToValStr(HexStr: string): string;
  var
    I: Integer;
    HexValue: Cardinal;
    S: string;
  begin
    Result := '';
    if Length(HexStr) mod 2 <> 0 then
      Insert('0', HexStr, Length(HexStr));

    for I := 0 to Length(HexStr) div 2 - 1 do
    begin
      S := Copy(HexStr, I*2+1, 2);
      HexValue := 0;
      StrToIntAsHex(HexValue, S);
      Result := Result + Chr(HexValue);
    end;
  end;

var
  I, J, Len: Integer;
  StartOffset, EndOffset: Integer;
  FindStr: string;
  Same: Boolean;
begin
  FindStr := HexStrToValStr(HexStr);

  if SearchHexOptions.UseWild then
  begin
    I := 1;
    while I <= Length(FindStr) do
    begin
      if FindStr[I] = SearchHexOptions.WildChar then
      begin
        for J := 1 to SearchHexOptions.WildCount - 1 do
          Insert(SearchHexOptions.WildChar, FindStr, I);
        Inc(I, SearchHexOptions.WildCount - 1);
      end;
      Inc(I, 1);
    end;
  end;

  GetOffsetRange(StartOffset, EndOffset, SearchHexOptions.OnlyBlock, FromCursor);
  Len := Length(FindStr);
  for I := StartOffset to EndOffset - Len + 1 do
  begin
    Same := True;

    for J := 0 to Len - 1 do
    begin
      if SearchHexOptions.UseWild then
      begin
        if FindStr[J+1] = SearchHexOptions.WildChar then Continue;
      end;
      if FindStr[J+1] <> GetDataAddr[I+J] then
      begin
        Same := False;
        Break;
      end;
    end;

    if Same then
    begin
      SetOffset(I);
      Result := True;
      Exit;
    end;
  end; //for I

  Result := False;
end;

function TCustomHexEdit.SearchInt(Value: Int64; FromCursor: Boolean): Boolean;

  function GetIntLen(IntType: TIntegerType): Integer;
  begin
    case IntType of
      itByte:      Result := 1;
      itWord:      Result := 2;
      itLongWord:  Result := 4;
      itShortInt:  Result := 1;
      itSmallInt:  Result := 2;
      itLongInt:   Result := 4;
      itInt64:     Result := 8;
      else         Result := 0;
    end;
  end;

var
  I, Len: Integer;
  StartOffset, EndOffset: Integer;
  Same: Boolean;
begin
  Len := GetIntLen(SearchIntOptions.IntegerType);

  GetOffsetRange(StartOffset, EndOffset, SearchIntOptions.OnlyBlock, FromCursor);
  for I := StartOffset to EndOffset - Len + 1 do
  begin
    Same := True;

    case SearchIntOptions.IntegerType of
      itByte:      Same := (Value = PByte(GetDataAddr + I)^);
      itWord:      Same := (Value = PWord(GetDataAddr + I)^);
      itLongWord:  Same := (Value = PLongWord(GetDataAddr + I)^);
      itShortInt:  Same := (Value = PShortInt(GetDataAddr + I)^);
      itSmallInt:  Same := (Value = PSmallInt(GetDataAddr + I)^);
      itLongInt:   Same := (Value = PLongInt(GetDataAddr + I)^);
      itInt64:     Same := (Value = PInt64(GetDataAddr + I)^);
    end;

    if Same then
    begin
      SetOffset(I);
      Result := True;
      Exit;
    end;
  end; //for I

  Result := False;
end;

function TCustomHexEdit.SearchFloat(Value: Extended; FromCursor: Boolean): Boolean;

  function GetFloatLen(FloatType: TFloatType): Integer;
  begin
    case FloatType of
      ftSingle:    Result := 4;
      ftDouble:    Result := 8;
      ftReal48:    Result := 6;
      ftExtended:  Result := 10;
      else         Result := 4;
    end;
  end;

type
  PReal48 = ^Real48;
  PExtended = ^Extended;
var
  I, Len: Integer;
  StartOffset, EndOffset: Integer;
  Same: Boolean;
  BlurValue: Extended;
begin
  Len := GetFloatLen(SearchFloatOptions.FloatType);
  if SearchFloatOptions.Blur then
    BlurValue := SearchFloatOptions.BlurValue
  else
    BlurValue := 0;

  GetOffsetRange(StartOffset, EndOffset, SearchFloatOptions.OnlyBlock, FromCursor);
  for I := StartOffset to EndOffset - Len + 1 do
  begin
    Same := True;

    case SearchFloatOptions.FloatType of
      ftSingle:   Same := Abs(Value - PSingle(GetDataAddr + I)^) <= BlurValue;
      ftDouble:   Same := Abs(Value - PDouble(GetDataAddr + I)^) <= BlurValue;
      ftReal48:   Same := Abs(Value - PReal48(GetDataAddr + I)^) <= BlurValue;
      ftExtended: Same := Abs(Value - PExtended(GetDataAddr + I)^) <= BlurValue;
    end;

    if Same then
    begin
      SetOffset(I);
      Result := True;
      Exit;
    end;
  end; //for I

  Result := False;
end;

initialization
  CF_HEXEDITDATA := RegisterClipboardFormat('HexEdit Clipboard Format');  { Do not localize }

end.
