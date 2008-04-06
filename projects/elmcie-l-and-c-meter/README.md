# Elmcie L- and C-meter

Uploaded by Plons on 2008-04-06 12:14:00 (rating 0 out of 5)

## Summary

April 2008: from field feedback I learned that it's not simple to build an Elmcie with components that are different from the ones I used. There are several references in the program that refer to the exact value of the used components, and those references are in constants. I had to do that to make the code fit into 8kb, and to ensures that your calibration- and null-values are safe, even when you do something stupid during cal or nulling. But the price is high: it takes a lot of tweaking from your side to get Elmcie going.  

I have to think about what to do about this.  

For now: be aware and prepared to do a lot of hard work to get your Elmcie on the road.


August29 2007: Updated package


Elmcie is a stand-alone inductance and capacitance measurement device  

Its name is a tribute to the original designs/designers


Elmcie is based on Elsie (or Elcie), as showed here:  

<http://www.amqrp.org/kits/elsie/>  

but it can be found on several other webpages as well.  

With thanks to the (unknown) original designer


The second part of Elmcie is based on the capacitance-meter as created by Elm-Chan:  

[http://elm-chan.org/works/cmc/re...](http://elm-chan.org/works/cmc/report.html)


All rights are reserved for the genuine designers


I merged both methods into the Elmcie design:  

Elsie's mechanism is great for small capacitances and inductors,  

where Elm-Chan's capacitance meter is more suitable for large capacitors,  

specifically electrolytes, upto 2200 uF.


Hardware based on ATmega8 @ 16 MHz


Implementation of algorythms are all genuine Plonswerk :-)  

This software is published under GNU General Public License (enclosed in the package)


Plons on AVRfreaks.net (Nard Awater), August 27, 2007

## Compilers

- BASCOM-AVR

## Tags

- Complete code with HW
- BASCOM-AVR
