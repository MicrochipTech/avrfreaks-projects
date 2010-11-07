# thermostat_atemga16

Uploaded by admin on 2010-11-07 11:19:00 (rating 0 out of 5)

## Summary

'Termostat pokojowy do Pieca C.O.


$regfile = "m16def.DAT"  

$crystal = 16000000  

 Config Lcdpin = Pin , Rs = Portc.2 , E = Portc.3 , Db4 = Portc.4 , Db5 = Portc.5 , Db6 = Portc.6 , Db7 = Portc.7  

Config Lcd = 16 * 2


Config Pina.0 = Output  

Config Pina.1 = Input  

Config Pina.2 = Input  

Config Pina.3 = Input


Config 1wire = Pind.4


Dim Temp As Integer , Ss As String * 6  

Dim Tzad As Integer , Tzadana As String * 6  

Dim Dsid As Byte  

Dim Hist As Integer


Porta.1 = 1 : Plus Alias Pina.1 'Podci?gni?cie portÃ³w do 1 dla obs?ugi klawitury  

Porta.2 = 1 : Minus Alias Pina.2  

Porta.3 = 1 : Menu Alias Pina.3


Pk Alias Porta.0


Tzad = 220


Writeeeprom Tzad , 0


Cursor Off  

Cls


Lcd " sterownik "  

Lowerline  

Lcd " do pieca C.O. "  

Wait 1  

Cls


Pomiar:


Do


Hist = Temp + 10 'ustawianie histerezy temperatury 10 to 1stopien c'


 1wreset 'organizacja trybu 1 wire'


 1wreset  

 1wwrite &HCC  

 1wwrite &H44


 Wait 1


 1wreset  

 1wwrite &HCC  

 1wwrite &HBE


 Temp = 1wread(2)


 Temp = Temp * 10  

 Temp = Temp / 16  

 Ss = Str(temp)  

 Ss = Format(ss , "0.0")


 Tzadana = Str(tzad)  

 Tzadana = Format(tzadana , "0.0")


 Readeeprom Tzad , 0


 Cls


 Locate 1 , 1


 Lcd "Temp :" ; Ss


 Locate 1 , 10


 Locate 2 , 11


 Lcd ; Chr(1) ; ":"  

 Locate 2 , 10  

 Lcd ; Tzadana


 If Menu = 0 Then  

 Wait 1  

 If Menu = 0 Then  

 Readeeprom Tzad , 0  

 Pk = 1  

 Goto Ustaw\_t  

 End If  

 End If


 Readeeprom Tzad , 0


 If Tzad >= Hist Then 'jesli temp zadana wieksza od histerray PK on '  

 Pk = 1  

 Locate 2 , 1  

 Lcd "Grzanie"  

 End If


 If Tzad < Temp Then  

 Pk = 0  

 Locate 2 , 1  

 Lcd "temp OK "  

 End If


Loop 'P?tla (koniec programu)


Ustaw\_t:


 Cls  

 Locate 1 , 1  

 Lcd " Ustaw temp "


 Locate 2 , 1  

 Lcd " -/+ "


 Tzadana = Str(tzad)  

 Tzadana = Format(tzadana , "0.0")


 Locate 2 , 8


 Lcd Tzadana


 Waitms 300  

 If Minus = 0 Then  

 Tzad = Tzad - 10  

 If Tzad = 150 Then  

 Tzad = 400  

 End If  

 End If


 Waitms 300  

 If Plus = 0 Then  

 Tzad = Tzad + 10  

 If Tzad = 400 Then  

 Tzad = 150  

 End If  

 End If  

 Waitms 300  

 If Menu = 0 Then  

 Writeeeprom Tzad , 0  

 Goto Pomiar  

 End If


Goto Ustaw\_t

## Compilers

- BASCOM-AVR

## Tags

- Complete code
- BASCOM-AVR
