object TestHotKeyService: TTestHotKeyService
  OldCreateOrder = False
  OnCreate = ServiceCreate
  DisplayName = 'HotKeyManager Service'
  StartType = stManual
  AfterInstall = ServiceAfterInstall
  OnExecute = ServiceExecute
  Left = 165
  Top = 548
  Height = 285
  Width = 561
  object HotKeyManager1: THotKeyManager
    Left = 40
    Top = 16
  end
end
