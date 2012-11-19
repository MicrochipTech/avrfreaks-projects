object Form1: TForm1
  Left = 708
  Top = 324
  Width = 313
  Height = 402
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'HID usart'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
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
  object Memo1: TMemo
    Left = 8
    Top = 280
    Width = 289
    Height = 49
    MaxLength = 62
    TabOrder = 1
    OnKeyPress = Memo1KeyPress
  end
  object ListBox1: TListBox
    Left = 8
    Top = 32
    Width = 289
    Height = 73
    ItemHeight = 13
    TabOrder = 2
    OnDblClick = ListBox1DblClick
  end
  object Button5: TButton
    Left = 240
    Top = 336
    Width = 49
    Height = 25
    Caption = 'SetStr'
    Enabled = False
    TabOrder = 3
    OnClick = Button5Click
  end
  object Memo2: TMemo
    Left = 8
    Top = 112
    Width = 289
    Height = 161
    ScrollBars = ssBoth
    TabOrder = 4
    OnDblClick = Memo2DblClick
  end
  object CheckBox1: TCheckBox
    Left = 16
    Top = 344
    Width = 105
    Height = 17
    Caption = '#13 #10 append'
    Checked = True
    State = cbChecked
    TabOrder = 5
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
