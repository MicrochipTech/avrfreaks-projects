Attribute VB_Name = "Module1"
Declare Sub Sleep Lib "Kernel32" (ByVal dwMilliseconds As Long)

Global Const AVRmhz = 10#
Global Const IRpwLSBuSec = 25.6              ' AVR reports pulse widths with this LSB resolution

Type learned
    name As String
    cs As Byte
    duration As Long
    learned() As Byte
End Type
    
    

