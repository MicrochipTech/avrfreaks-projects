Attribute VB_Name = "Module1"
Public Declare Function GetTickCount Lib "kernel32" () As Long
Public Declare Function QueryPerformanceCounter Lib "kernel32" (ByRef lpPerformanceCount As Currency) As Long
Public Declare Function QueryPerformanceFrequency Lib "kernel32" (ByRef lpFrequency As Currency) As Long
Dim iStart As Currency, iEnd As Currency, iFrq As Currency, lRet As Long




'延时子程序 单位ms
Sub TimeDelay(TT As Single)
    Dim t As Long
    t = GetTickCount()
    Do
        DoEvents
    Loop Until GetTickCount - t >= TT
End Sub

'延时子程序ms(极高精度) 单位ms
Sub TimeDelay2(TT As Single)
    lRet = QueryPerformanceFrequency(iFrq)
    iFrq = iFrq / 1000
    lRet = QueryPerformanceCounter(iStart)
    Do
        lRet = QueryPerformanceCounter(iEnd)
        DoEvents
    Loop Until ((iEnd - iStart) / iFrq) >= TT
End Sub
