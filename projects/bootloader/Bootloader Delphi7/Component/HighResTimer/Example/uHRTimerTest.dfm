object Form1: TForm1
  Left = 192
  Top = 107
  Width = 326
  Height = 159
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 64
    Width = 70
    Height = 13
    Caption = 'Breaktime (us):'
  end
  object Label2: TLabel
    Left = 8
    Top = 88
    Width = 72
    Height = 13
    Caption = 'Measured time:'
  end
  object MeasureResult: TLabel
    Left = 88
    Top = 88
    Width = 71
    Height = 13
    Caption = 'MeasureResult'
  end
  object Label3: TLabel
    Left = 184
    Top = 8
    Width = 84
    Height = 13
    Caption = 'Count Frequency:'
  end
  object lblCountFrequency: TLabel
    Left = 184
    Top = 24
    Width = 88
    Height = 13
    Caption = 'lblCountFrequency'
  end
  object Edit1: TEdit
    Left = 88
    Top = 56
    Width = 65
    Height = 21
    TabOrder = 0
    Text = '10000000'
  end
  object RunTest: TButton
    Left = 168
    Top = 56
    Width = 75
    Height = 25
    Caption = 'Run Test'
    Default = True
    TabOrder = 1
    OnClick = RunTestClick
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 113
    Width = 318
    Height = 19
    Panels = <>
    SimplePanel = True
  end
  object rbUseTimeStampCounter: TRadioButton
    Left = 8
    Top = 8
    Width = 161
    Height = 17
    Caption = 'Time Stamp Counter'
    TabOrder = 3
    OnClick = rbUseTimeStampCounterClick
  end
  object rbUseHPCounter: TRadioButton
    Left = 8
    Top = 32
    Width = 161
    Height = 17
    Caption = 'High Performance Counter'
    TabOrder = 4
    OnClick = rbUseTimeStampCounterClick
  end
  object HighResTimer1: THighResTimer
    Accuracy = 0
    Resolution = 5
    ThreadPriority = tpHighest
    UseTSC = True
    Left = 256
    Top = 64
  end
end
