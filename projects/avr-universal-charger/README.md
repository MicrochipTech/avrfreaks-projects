# AVR Universal Charger

Uploaded by martind on 2016-09-05 20:00:00 (rating 5 out of 5)

## Summary

Hello everybody,


I just wanted to share with you my most recent project, an AVR Multichemistry charger. There has been a lot of fuzz about charging batteries with AVR, but there are not many complete schematics and/or projects on the internet.


 


**Overall**


The charger is based on Atmega 328 (because it is dirt cheap) and utilizes its PWM to drive a FET, used as buck converter. There is also an 5110 LCD (again, less than 2 USD on Ebay) to show some basic menus and stuff. Most of the settings are adjustable on the go, so once you build and program it, you can use it without further reprogramming for all battery types and so on.


 


**Batteries:**


The charger supports all of the most used battery types - that is in my opinion NiCd, NiMH, Li-Ion and SLA. Li-Ion and SLA are charged in constant current-constant voltage mode, while Nickel based batteries are charged with constant current and terminated by about 5 methods (time out, overvoltage, overtemperature, -dV/dT, user termination). I have been using the charger for about a month before posting it here (tested everything except SLA, don’t have those on hand) and everything seems to work seamlessly. Maximum charging current is 3A, maximum charging voltage is almost Vin (that’s about 18V in my setup).


 


**Discharge**


The charger also comes with a discharge circuitry, used to:


1. Measure battery capacity
2. Discharge batteries before charging (useful with NiMH batteries to neutralize their memory effect)

The discharging current is fully adjustable from 0 to 3 amps, with undervoltage protection (again, fully adjustable). There is a small fan, which is automatically switched on when thermal power dissipation is bigger than 2 watts (you can change that, it depends on the size of your heatsink).


 


**Power supply**


Also in the menu you can find "constant C", "constant V" and "power supply" options. They provide you with simple power supply, useful for SOME applications. Because of the MCU limitations, the transient response time is relatively long, but on loads that do not change quickly, the MCU will react quickly enough. Maximum output voltage of the power supply is almost Vin and output current 3 A.


 


**Project**


This started as my own personal build, but since I spent a lot of time on it, I decided to publish it. For me, it works well, even for precise charging lithium batteries.


It is inspired by AVR450, but 99% of the code is my work. I also used 5110 LCD driver library from Radu Motisan, see the 5110 files for credit.


I would like to hear some feedback on this project. What do you think about it? The total cost was less than 10 USD, which, I believe, is dirt cheap. Also, if there will be interest, I am thinking about porting this to STM32, as they are superior to 8-bit AVR and still very cheap. I would also include balancing circuitry. What do you think about that?


 


**Images:** see attachments

## Links

- [GitHub](https://github.com/MartinD-CZ/batteryCharger)

## Compilers

- Atmel Studio 6

## Tags

- Complete code with HW
- Atmel Studio 6
- #Atmega328P
- Battery Management
- #charging #battery #liion #sla #nimh #nicd
