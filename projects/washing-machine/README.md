# Washing machine

Uploaded by silicium628 on 2008-12-04 17:41:00 (rating 0 out of 5)

## Summary

Sorry for online translation:


Preliminary warning!


This realization includes|understands two parts:  

 * a low card tension gathering the microprocessor and some components.  

 * one high card tension connected to the sector 230 V including|understanding triacs, but also of the circuits of galvanic insulation between the sector and the part low tension, that is to say of the optotriacs (MOC3043), intermediaries, as|like a transformer of feeding. De ce fait this card must be packed and to be realized with the greatest care in order to it that the part low tension in no case can enter in contact with the sector 230 V.


For this reason I will not publish the let's bring out the features of realization of the cards but only the theoretical diagrams. From the same diagram, an establishment of the components or a badly made realization can lead you the cemetery straight ahead! If you are not accustomed to the assemblies working on the sector, I advise against you lively of realizing this timer.  

I clarify that I do not sell anything, I do not realize anything for the others. I content with displaying what I make for me, à titre d'exemple not to follow, in order to illustrate the extraordinary ones possibilities of the microcontrollers.  

On the other hand I supply all my sorces codes in Pascal, commented on.


Principle  

The microcontrôleur ATmega32 controls the unity|whole of the organs.


1) In being taken out:


 * walk|step - arret, speed and sense|way of rotation of the motor (motor "universal type" at brooms and coals)  

 * electrogates of admission of water  

 * pump of drain  

 * resistance of heating  

 * Billsticker LCD 4lignes of 20 characters (blue|bruise).  

 * Some LED of all the colors


2) In entry:


 * sensor of level of water (that of origin, to contacts, I plan|foresee to replace it|her with a sensor of pressure)  

 * dynamo tachymètrique on the axis of the motor (of origin)  

 * probe of résistive temperature (of origin)  

 * signal of reference of stage of the 50 Hz sector.  

 * recepteur of télécommmande IR  

 * push buttons.


The speed of rotation of the motor is subjugated by the microcontrôleur. His|Her|Its tension of feeding 230 V is cut by a triac ordered|commanded by the ATmega. Indeed, directly fed on 230 V the motor turns to the speed of wringing. It is needed to slow therefore it|him down for the washing. But the enslavement of speed is made necessary of the fact that the couple varies ENORMEMENT during every tower of the drum of the to the distribution|allotment and to the movements from the linen in this drum. Of where the presence of the tachymètrique dynamo on the axis of the motor. the ATmega measures permanently the real speed of the drum (by a programmed frequency meter duty|function) and compares it|her with some orders. The result is used to settle the corner of conduction of the triac in order to maintain the constant speed (enslavement of speed). It is one of the most delicate points of the program.


The sense|way of rotation of the motor is chosen by permutation of the connection of the stator in relation to the rotor, by two intermediaries.  

The pump and the electrogates are ordered|commanded by triacs.  

The resistance of heating is ordered|commanded by an intermediary 230 V - 10 A.


The microcontrôleur and the circuits low tension (5 V) are isolated of the sector 230 V:  

 * the feeding by a transformer 230 V - 2x6V  

 * the triacs by opto-triacs to detection of passage at zero MOC3043.  

 * the triac of carving|cut-out|division of the tension of the motor by an opto-triac WITHOUT detection of passage at zero MOC3023 (instant reply)


The all is managed to distance by a remote control for TV! (that avoids touching to which that it is during the working of the machine)

## Links

- [Original French site:](http://www.silicium628.fr/electron/lavelinge/lavelinge.html)
- [Translated online:](http://64.233.179.104/translate_c?hl=en&langpair=auto%7Cen&u=http%3A//www.silicium628.fr/electron/lavelinge/lavelinge.html)

## Compilers

- AVRco

## Tags

- Complete code with HW
- AVRco
