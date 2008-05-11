Attribute VB_Name = "modHexToDec"
Option Explicit
'Fonction retournant valeur décimale d'un Hexa 2caractères
Public Function HexToDec(HexValue As String) As Long
        If Len(HexValue) > 3 Then
            HexToDec = Val("&H" + Right(HexValue, 3))
            HexToDec = HexToDec + (4096 * (Val("&H" + Left(HexValue, (Len(HexValue) - 3)))))
        End If
        If Len(HexValue) <= 3 Then HexToDec = Val("&H" + HexValue)
End Function

'Public Function HexToDec(HexValue As String) As Long
'    On Error Resume Next
'    If UCase$(Left$(HexValue, 2)) = "0X" Then
'        HexToDec = Val("&H" & Mid$(HexValue, 3))
'    ElseIf UCase$(Left$(HexValue, 2)) <> "&H" Then
'        HexToDec = Val("&H" & HexValue)
'    Else
'        HexToDec = Val(HexValue)
'    End If
'End Function

