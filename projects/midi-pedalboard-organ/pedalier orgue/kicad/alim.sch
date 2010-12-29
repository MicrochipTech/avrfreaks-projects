EESchema Schematic File Version 1
LIBS:power,device,transistors,conn,linear,regul,74xx,cmos4000,adc-dac,memory,xilinx,special,microcontrollers,dsp,microchip,analog_switches,motorola,texas,intel,audio,interface,digital-audio,philips,display,cypress,siliconi,contrib,valves,Atmel,.\pedalier.cache
EELAYER 23  0
EELAYER END
$Descr A4 11700 8267
Sheet 3 3
Title ""
Date "24 mar 2008"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L GND #PWR022
U 1 1 47E396DF
P 7650 2950
F 0 "#PWR022" H 7650 2950 30  0001 C C
F 1 "GND" H 7650 2880 30  0001 C C
	1    7650 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	7650 2950 7650 2700
Wire Wire Line
	7650 2700 2250 2700
Connection ~ 7100 2700
Wire Wire Line
	7100 2700 7100 2800
Wire Wire Line
	6100 2700 6100 2550
Connection ~ 3850 2700
Wire Wire Line
	3000 2700 3000 2550
Wire Wire Line
	2250 2700 2250 2200
Wire Wire Line
	2250 2200 1800 2200
Wire Wire Line
	6100 2000 6100 2150
Connection ~ 3000 2000
Wire Wire Line
	3850 2000 3850 2150
Wire Wire Line
	3000 2150 3000 2000
Wire Wire Line
	4500 2000 1800 2000
Connection ~ 3850 2000
Connection ~ 6100 2000
Wire Wire Line
	3850 2700 3850 2550
Connection ~ 3000 2700
Wire Wire Line
	4900 2700 4900 2300
Connection ~ 4900 2700
Connection ~ 6100 2700
Wire Wire Line
	7150 2000 7150 1950
Connection ~ 7150 2000
Wire Wire Line
	5300 2000 7650 2000
Wire Wire Line
	7650 2000 7650 1850
$Comp
L +5V #PWR023
U 1 1 47E39557
P 7650 1850
F 0 "#PWR023" H 7650 1940 20  0001 C C
F 1 "+5V" H 7650 1940 30  0000 C C
	1    7650 1850
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG024
U 1 1 47DFF149
P 7150 1950
F 0 "#FLG024" H 7150 2220 30  0001 C C
F 1 "PWR_FLAG" H 7150 2180 30  0000 C C
	1    7150 1950
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG025
U 1 1 47DFF105
P 7100 2800
F 0 "#FLG025" H 7100 3070 30  0001 C C
F 1 "PWR_FLAG" H 7100 3030 30  0000 C C
	1    7100 2800
	1    0    0    1   
$EndComp
$Comp
L C C6
U 1 1 47D7BDCD
P 6100 2350
F 0 "C6" H 6150 2450 50  0000 L C
F 1 "100n" H 6150 2250 50  0000 L C
	1    6100 2350
	1    0    0    -1  
$EndComp
$Comp
L C C5
U 1 1 47D7BDC7
P 3850 2350
F 0 "C5" H 3900 2450 50  0000 L C
F 1 "100n" H 3900 2250 50  0000 L C
	1    3850 2350
	1    0    0    -1  
$EndComp
$Comp
L CONN_2 P3
U 1 1 47D7BC1A
P 1450 2100
F 0 "P3" V 1400 2100 40  0000 C C
F 1 "CONN_2" V 1500 2100 40  0000 C C
	1    1450 2100
	-1   0    0    -1  
$EndComp
$Comp
L CP C4
U 1 1 47D7A76D
P 3000 2350
F 0 "C4" H 3050 2450 50  0000 L C
F 1 "4700u" H 3050 2250 50  0000 L C
	1    3000 2350
	1    0    0    -1  
$EndComp
$Comp
L 7805 U6
U 1 1 47D7A6C9
P 4900 2050
F 0 "U6" H 5050 1854 60  0000 C C
F 1 "7805" H 4900 2250 60  0000 C C
	1    4900 2050
	-1   0    0    -1  
$EndComp
$EndSCHEMATC
