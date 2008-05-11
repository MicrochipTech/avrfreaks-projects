Attribute VB_Name = "modDecToHex"
Option Explicit
'Fonction retournant Hexa X caractères à partir d'un byte
Public Function DecToHex(Decim As Long, Longueur_X As Byte, Symbole As String) As String
        DecToHex = Hex(Decim)
Pt_Test:
        If Len(DecToHex) < Longueur_X Then
            DecToHex = "0" & DecToHex
            GoTo Pt_Test
        End If
        DecToHex = Symbole & DecToHex
End Function
