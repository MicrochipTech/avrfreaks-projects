object Form1: TForm1
  Left = 708
  Top = 324
  Width = 313
  Height = 402
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'HID read write..'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnActivate = FormActivate
  PixelsPerInch = 96
  TextHeight = 13
  object ComboBox1: TComboBox
    Left = 8
    Top = 8
    Width = 289
    Height = 21
    ItemHeight = 13
    TabOrder = 0
    OnSelect = ComboBox1Select
  end
  object StringGrid1: TStringGrid
    Left = 8
    Top = 112
    Width = 289
    Height = 161
    ColCount = 9
    DefaultColWidth = 30
    DefaultRowHeight = 16
    RowCount = 9
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing, goAlwaysShowEditor]
    TabOrder = 1
    OnKeyPress = StringGrid1KeyPress
    OnSelectCell = StringGrid1SelectCell
  end
  object Memo1: TMemo
    Left = 8
    Top = 280
    Width = 289
    Height = 49
    MaxLength = 64
    TabOrder = 2
  end
  object ListBox1: TListBox
    Left = 8
    Top = 32
    Width = 289
    Height = 73
    ItemHeight = 13
    TabOrder = 3
    OnDblClick = ListBox1DblClick
  end
  object Button2: TButton
    Left = 8
    Top = 336
    Width = 49
    Height = 25
    Caption = 'GetHex'
    Enabled = False
    TabOrder = 4
    OnClick = Button2Click
  end
  object Button4: TButton
    Left = 184
    Top = 336
    Width = 49
    Height = 25
    Caption = 'SetHex'
    Enabled = False
    TabOrder = 5
    OnClick = Button4Click
  end
  object Button5: TButton
    Left = 240
    Top = 336
    Width = 49
    Height = 25
    Caption = 'SetStr'
    Enabled = False
    TabOrder = 6
    OnClick = Button5Click
  end
  object HidCtl: TJvHidDeviceController
    OnArrival = HidCtlArrival
    OnEnumerate = HidCtlEnumerate
    OnDeviceChange = HidCtlDeviceChange
    OnDeviceDataError = HidCtlDeviceDataError
    OnRemoval = HidCtlRemoval
    Left = 224
    Top = 72
  end
end
