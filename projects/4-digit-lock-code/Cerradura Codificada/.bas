'===========================================
'Integrantes:
'Echenagucia, Jarim C.I. 18.010.052
'Figueroa, Felix    C.I. 14.999.887
'Cerradura Codificada 04/10/2008
'===========================================

$regfile = "m8def.dat"
$crystal = 6000000

'---------------------------
'configuracion para teclado
'---------------------------
Config Portd.0 = Output , Portd.1 = Output , Portd.2 = Output , Portd.3 = Output,
Config Pinc.0 = Input , Pinc.1 = Input , Pinc.2 = Input
Config Portc.5 = Output , Portc.4 = Output
'PD0= filaA, PD1= FilaB, PD2=FilaC, PD3=FilaD
'PC0=C1, PC1=C2, PC2=C3
'-----------------------
'configuracion para LCD
'-----------------------
Config Lcdpin = Pin , Db4 = Portb.3 , Db5 = Portb.2 , Db6 = Portb.1 , Db7 = Portb.0 , E = Portb.4 , Rs = Portb.5
Config Lcd = 16 * 2

Dim Conte As Byte
Dim Num As Byte
Dim X As Word
Dim Pot As Long
Dim N1 As Byte
Dim Nbloque As Long
Dim Numb As Long
Dim N2 As Byte
Dim L As Byte

N2 = 0
L = 0
Num = 0
Conte = 0
Numb = 0
Nbloque = 0
Pot = 10

Deflcdchar 0 , 1 , 3 , 6 , 12 , 12 , 28 , 28 , 28
Deflcdchar 1 , 16 , 24 , 12 , 6 , 6 , 7 , 7 , 7
Deflcdchar 2 , 31 , 31 , 31 , 30 , 30 , 31 , 31 , 31
Deflcdchar 3 , 31 , 31 , 31 , 15 , 15 , 31 , 31 , 31

Cls
Lcd "#Cerradura#"
Waitms 500
Lowerline
Lcd "#Codificada#"
Waitms 500


'==========================
'Figura del candado abierto
'==========================
Cls
Locate 1 , 2
Lcd Chr(0)
Locate 1 , 3
Lcd Chr(1)
Locate 2 , 1
Lcd Chr(2)
Locate 2 , 2
Lcd Chr(3)
Waitms 100

Locate 1 , 13
Set Portc.5

Ciclo_teclado:
'-------------------------------------
'este es para activar la primera fila
'-------------------------------------
                                                 'PD0= filaA, PD1= FilaB, PD2=FilaC, PD3=FilaD
Reset Portd.0                                               'PC0=C1, PC1=C2, PC2=C3
Set Portd.1
Set Portd.2
Set Portd.3
If Pinc.0 = 0 Then
Conte = Conte + 1
Num = 1
Goto Espacio_lcd
End If
If Pinc.1 = 0 Then
Conte = Conte + 1
Num = 2
Goto Espacio_lcd
End If
If Pinc.2 = 0 Then
Conte = Conte + 1
Num = 3
Goto Espacio_lcd
End If

'--------------------------------------
'este es para activar la segunda fila
'--------------------------------------

Set Portd.0
Reset Portd.1
Set Portd.2
Set Portd.3
If Pinc.0 = 0 Then
Conte = Conte + 1
Num = 4
Goto Espacio_lcd
End If
If Pinc.1 = 0 Then
Conte = Conte + 1
Num = 5
Goto Espacio_lcd
End If
If Pinc.2 = 0 Then
Conte = Conte + 1
Num = 6
Goto Espacio_lcd
End If

'-------------------------------------
'este es para activar la tercera fila
'-------------------------------------
Set Portd.0
Set Portd.1
Reset Portd.2
Set Portd.3
If Pinc.0 = 0 Then
Conte = Conte + 1
Num = 7
Goto Espacio_lcd
End If
If Pinc.1 = 0 Then
Conte = Conte + 1
Num = 8
Goto Espacio_lcd
End If
If Pinc.2 = 0 Then
Conte = Conte + 1
Num = 9
Goto Espacio_lcd
End If


'------------------------------------
'este es para activar la cuarta fila
'------------------------------------
Set Portd.0
Set Portd.1
Set Portd.2
Reset Portd.3
If Pinc.0 = 0 Then
End If
If Pinc.1 = 0 Then
Conte = Conte + 1
Num = 10
Goto Espacio_lcd
End If
If Pinc.2 = 0 Then
End If
Goto Ciclo_teclado

'------------------------------------
'sub-rutina de Escritura de Numero
'------------------------------------

Espacio_lcd:
If Conte = 1 Then
Locate 1 , 13
End If

'-----------------------------
'Libreria
'-----------------------------
Libreria:
If L = 0 Then

If Num = 1 Then
Lcd "1"
Waitms 200
Gosub Numero
End If

If Num = 2 Then
Lcd "2"
Waitms 200
Gosub Numero
End If

If Num = 3 Then
Lcd "3"
Waitms 200
Gosub Numero
End If

If Num = 4 Then
Lcd "4"
Waitms 200
Gosub Numero
End If

If Num = 5 Then
Lcd "5"
Waitms 200
Gosub Numero
End If

If Num = 6 Then
Lcd "6"
Waitms 200
Gosub Numero
End If

If Num = 7 Then
Lcd "7"
Waitms 200
Gosub Numero
End If

If Num = 8 Then
Lcd "8"
Waitms 200
Gosub Numero
End If

If Num = 9 Then
Lcd "9"
Waitms 200
Gosub Numero
End If

If Num = 10 Then
Lcd "0"
Waitms 200
Gosub Numero
End If
End If

If L = 1 Then
Lcd "*"
Waitms 200
Gosub Numero
End If


If Conte = 4 Then
Gosub Limite
End If
Goto Ciclo_teclado

'=================
'Numero de Bloqueo
'=================
Numero:
If N1 = 0 Then
Nbloque = Nbloque * Pot
Nbloque = Nbloque + Num
End If

If N1 = 1 Then
Numb = Numb * Pot
Numb = Numb + Num
End If
Return

'==================
'Limite de digitos
'==================

Limite:
Set Portd.0
Set Portd.1
Set Portd.2
Reset Portd.3

If N2 = 0 Then
Locate 2 , 5
Lcd "# Acp * Rcz"
Limite1:
If Pinc.2 = 0 Then
L = 1
N2 = 1
Goto Aceptar
End If

If Pinc.0 = 0 Then
Nbloque = 0
Conte = 0
Cls
Gosub Abierto
Goto Ciclo_teclado
End If
Goto Limite1
End If

Locate 2 , 12
Lcd "# Acp"
Bitwait Pinc.2 , Reset

Aceptar:
If N1 = 1 Then
If Numb = Nbloque Then
Conte = 0
Numb = 0
Cls
Gosub Abierto
Locate 1 , 5
Lcd "menu"
Locate 2 , 5
Lcd "1 Cer 2 Cab"
Reset Portd.0
Set Portd.1
Set Portd.2
Set Portd.3
Menu:
If Pinc.0 = 0 Then
Cls
Gosub Cerrado
Waitms 200
Goto Ciclo_teclado
End If
If Pinc.1 = 0 Then
L = 0
N1 = 0
N2 = 0
Nbloque = 0
Cls
Gosub Abierto
Waitms 200
Goto Ciclo_teclado
End If
Goto Menu

Else
Numb = 0
Conte = 0
Cls
Gosub Cerrado
Goto Ciclo_teclado
End If
End If

If N1 = 0 Then
N1 = 1
Conte = 0
Cls
Goto Cerrado
End If

Return

'==========================
'Figura del candado cerrado
'==========================

Cerrado:
Locate 1 , 1
Lcd Chr(0)
Locate 1 , 2
Lcd Chr(1)
Locate 2 , 1
Lcd Chr(2)
Locate 2 , 2
Lcd Chr(3)
Locate 1 , 13

Reset Portc.5
Set Portc.4
Return

'==========================
'Figura del candado abierto
'==========================

Abierto:
Locate 1 , 2
Lcd Chr(0)
Locate 1 , 3
Lcd Chr(1)
Locate 2 , 1
Lcd Chr(2)
Locate 2 , 2
Lcd Chr(3)
Locate 1 , 13

Set Portc.5
Reset Portc.4
Return