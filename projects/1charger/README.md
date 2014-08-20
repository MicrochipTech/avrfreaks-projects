# 1charger

Uploaded by swe_jb on 2014-08-19 20:14:00 (rating 0 out of 5)

## Summary

![](https://www.avrfreaks.net/sites/default/files/IMG_4883_0.jpg)


![](https://www.avrfreaks.net/sites/default/files/3612511409483821383_0.jpg)


 


**OneCharger to rule them all; solar-panel, TEG, dynamo...**


 


 


**Features:**


* multiple power sources ( Vin = 4-30V peak 36V )


* multiple battery voltages ( CV / Vout 1.7-25V )


* multiple charge currents ( CC = 0.3 - 5A peak 10A )


* multiple batteries in parallel simultaneous ( with "or-ing diodes" )


* multifunction ( future add-ons )


 


**Wattmeter design:**


The heart of the wattmeter is a [Attiny861](http://www.atmel.com/Images/Atmel-2588-8-bit-AVR-Microcontrollers-tinyAVR-ATtiny261-ATtiny461-ATtiny861_Datasheet.pdf) which has differential gain x1, x8, x20, x32. This means there is no need for an op-amp. Attiny861 also has four different Voltage reference sections ( 1.1V, 2.56V, Vcc (3.3V),Aref ). This gives a lot of options. The 3.3V (buck-boost) voltage supply to the wattmeter is a gutted [ebay voltmeter](http://www.ebay.com/itm/DC-1-7-25V-Voltmeter-waterproof-Digital-Voltmeter-Panel-for-AUTO-CAR-MOTORCYCLE-/231128687224?pt=LH_DefaultDomain_0&hash=item35d057e278). It also has a voltage divider. The wattmeter is supplied from the battery ( Vout ). The buck-boost circuit ensure 3.3V when battery voltage varies between 1.7-25V. And finally a low power [Nokia 5110](http://thegaragelab.com/posts/nokia-5110-lcd-displays-on-the-attiny.html) is used as display.


 


**Software :**


The software has two modes, calibration and measure watt. The measure\_watt calls adc\_read routine that reads current and voltage. Each second 1500 ADC is read. Voltage and current is averaged in twelve measurement steps and for each step watt is calculated and finally the wattmeter is increased. The display is updated with 2Hz. Fix-point arithmetic with two decimals is used. Voltage and current has two ranges each. During calibration current and voltage is linearized to y = k*ADC + m. Where the constant k is represented as k=k1/k2 ( k1,k2 are uint32 ). The [software UART](http://martinsant.net/?page_id=1689) uses a timer (also used to measure time). The charging technique is [simple stupid CV & CC](http://www.prodctodc.com/dc-adjustable-430v-to-1230v-buck-converts-constant-current-car-laptop-cellphone-power-supply-p-214.html#.U8gGOkCqYZ8) and I threat the LiFePO4 as if they were capacitors. The power source may be TEG ( like the [powerpot](https://www.kickstarter.com/projects/david-toledo/the-powerpot-x-most-reliable-10-watt-portable-gene) ), solar panels or a [dynamo](http://www.sunshine-zh.com/en/product/product_info_94.html). It can also fast charge directly from a car battery. The supply current for the wattmeter is 7.5mA It is not (yet) a [MPPT](http://www.eetimes.com/document.asp?doc_id=1279330) charger even though the wattmeter circuit is a good base for taking that step in the future. Extra feature, "or-ing diodes" By connecting several diodes ( as many as you wish ) in parallel to the primary blocking diode, it is possible to charge multiple batteries at once. But the most discharged battery should be placed after the "primary-diode"


 


**Usage :**


Adjust CV (Vout) on step-down to battery float voltage


Adjust CC ( current limit ) for smaller batteries ( or use 5A to protect charger )


Estimate min Vin needed by step-down = battery float voltage (Vout) + 1V


Connect your power source in serial or parallel to achieve sufficient Vin


Suggested open voltage TEG = 2xVin ( [MPPT](http://www.eetimes.com/document.asp?doc_id=1279330) )


Suggested open voltage solar panels = 1.25xVin ( [MPPT](http://www.eetimes.com/document.asp?doc_id=1279330) )


Protect your TEG and solar panels with reverse blocking diodes When charging cells in series use balancing circuit


 


**Example**


Single cell LiFePO4 8Ah 2x [5W Solar panels (5.5V)](http://www.xscyz.com/index.php?main_page=product_info&cPath=65&products_id=391) in parallel or 2xTEG (12V) in parallel [PPX(special)](http://www.powerpractical.com/power-pot-x?ref=cat_img) CV=3.55V ( single cell LiFePo4 ) CC=5A ( to protect circuit when charging from car battery ) reverse blocking diodes in cables ( solar panels and TEG ) Since most of the energy is between 3-3.4V I prefer to set float voltage to 3.55V


[Projectpage hackaday-1charger](https://hackaday.io/project/1936-1charger)


[Solar charging demo](http://youtu.be/B_QaIIPeN1Y) [Make a charge curve with plotly](http://hackaday.io/project/1936/log/6202-connectedish-to-plotly)


[Charge curves](https://plot.ly/~mr.jb.swe/)


 


[![Multicharger](https://static.hackaday.io/images/802371407682412022.png)](https://plot.ly/~mr.jb.swe/17)



[source github](https://github.com/MrJBSwe/1charger)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
