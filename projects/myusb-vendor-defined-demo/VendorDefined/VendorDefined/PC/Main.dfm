object HIDForm: THIDForm
  Left = 273
  Top = 185
  Width = 640
  Height = 480
  Caption = 'MyUSB Vendor defined HID Demo'
  Color = clBtnFace
  Constraints.MinHeight = 480
  Constraints.MinWidth = 640
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnShow = FormShow
  DesignSize = (
    632
    453)
  PixelsPerInch = 96
  TextHeight = 13
  object AvrMemo: TMemo
    Left = 5
    Top = 5
    Width = 621
    Height = 291
    Anchors = [akLeft, akTop, akRight, akBottom]
    Lines.Strings = (
      'AvrMemo')
    TabOrder = 0
  end
  object GroupBox1: TGroupBox
    Left = 5
    Top = 300
    Width = 621
    Height = 50
    Anchors = [akLeft, akRight, akBottom]
    Caption = 'Port IO'
    TabOrder = 1
    object Port: TLabel
      Left = 10
      Top = 25
      Width = 19
      Height = 13
      Caption = 'Port'
    end
    object Label1: TLabel
      Left = 90
      Top = 25
      Width = 20
      Height = 13
      Caption = 'Reg'
    end
    object Label2: TLabel
      Left = 190
      Top = 25
      Width = 27
      Height = 13
      Caption = 'Value'
    end
    object PortCombo: TComboBox
      Left = 35
      Top = 20
      Width = 46
      Height = 21
      ItemHeight = 13
      TabOrder = 1
      Text = 'PortCombo'
    end
    object RegCombo: TComboBox
      Left = 115
      Top = 20
      Width = 66
      Height = 21
      ItemHeight = 13
      TabOrder = 0
      Text = 'RegCombo'
      Items.Strings = (
        'IN'
        'DDR'
        'OUT')
    end
    object Value: TEdit
      Left = 225
      Top = 20
      Width = 26
      Height = 21
      TabOrder = 2
      Text = '00'
    end
    object ReadPortBtn: TButton
      Left = 260
      Top = 20
      Width = 40
      Height = 21
      Caption = 'Read'
      TabOrder = 3
      OnClick = ReadPortBtnClick
    end
    object WritePortBtn: TButton
      Left = 305
      Top = 20
      Width = 40
      Height = 21
      Caption = 'Write'
      TabOrder = 4
      OnClick = WritePortBtnClick
    end
    object Button1: TButton
      Left = 365
      Top = 15
      Width = 75
      Height = 25
      Caption = 'Button1'
      TabOrder = 5
      OnClick = Button1Click
    end
  end
  object GroupBox2: TGroupBox
    Left = 5
    Top = 350
    Width = 306
    Height = 50
    Anchors = [akLeft, akBottom]
    Caption = 'EEPROM'
    TabOrder = 2
    object Label3: TLabel
      Left = 10
      Top = 25
      Width = 38
      Height = 13
      Caption = 'Address'
    end
    object Label4: TLabel
      Left = 120
      Top = 25
      Width = 27
      Height = 13
      Caption = 'Value'
    end
    object EEAddress: TEdit
      Left = 55
      Top = 20
      Width = 56
      Height = 21
      TabOrder = 0
      Text = '0'
    end
    object EEValue: TEdit
      Left = 155
      Top = 20
      Width = 26
      Height = 21
      TabOrder = 1
      Text = '00'
    end
    object EERead: TButton
      Left = 185
      Top = 20
      Width = 40
      Height = 21
      Caption = 'Read'
      TabOrder = 2
      OnClick = EEReadClick
    end
    object EEWrite: TButton
      Left = 230
      Top = 20
      Width = 40
      Height = 21
      Caption = 'Write'
      TabOrder = 3
      OnClick = EEWriteClick
    end
  end
  object GroupBox3: TGroupBox
    Left = 320
    Top = 350
    Width = 306
    Height = 50
    Anchors = [akLeft, akRight, akBottom]
    Caption = 'RAM'
    TabOrder = 3
    object Label5: TLabel
      Left = 10
      Top = 25
      Width = 38
      Height = 13
      Caption = 'Address'
    end
    object Label6: TLabel
      Left = 120
      Top = 25
      Width = 27
      Height = 13
      Caption = 'Value'
    end
    object RAMAddress: TEdit
      Left = 55
      Top = 20
      Width = 56
      Height = 21
      TabOrder = 0
      Text = '0'
    end
    object RAMValue: TEdit
      Left = 155
      Top = 20
      Width = 26
      Height = 21
      TabOrder = 1
      Text = '00'
    end
    object RAMRead: TButton
      Left = 185
      Top = 20
      Width = 40
      Height = 21
      Caption = 'Read'
      TabOrder = 2
      OnClick = RAMReadClick
    end
    object RAMWrite: TButton
      Left = 230
      Top = 20
      Width = 40
      Height = 21
      Caption = 'Write'
      TabOrder = 3
      OnClick = RAMWriteClick
    end
  end
  object GroupBox4: TGroupBox
    Left = 5
    Top = 400
    Width = 621
    Height = 50
    Anchors = [akLeft, akRight, akBottom]
    Caption = 'Feature'
    TabOrder = 4
    object GetFeatureBtn: TButton
      Left = 10
      Top = 15
      Width = 75
      Height = 25
      Caption = 'Get Feature'
      TabOrder = 0
      OnClick = GetFeatureBtnClick
    end
    object SetFeatureBtn: TButton
      Left = 90
      Top = 15
      Width = 75
      Height = 25
      Caption = 'Set Feature'
      TabOrder = 1
      OnClick = SetFeatureBtnClick
    end
  end
  object HidCtrl: TJvHidDeviceController
    OnArrival = HidCtrlArrival
    OnDeviceChange = HidCtrlDeviceChange
    OnRemoval = HidCtrlRemoval
    Left = 600
  end
end
