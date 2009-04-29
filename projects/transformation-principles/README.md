# Transformation principles

Uploaded by ankarsvik on 2009-04-28 17:03:00 (rating 0 out of 5)

## Summary

From time to time, I?m asked to explain how to transform from a coordinate system to another. Now when Sweden is converting from RT90 to a new planar system, SweRef99 TM, it?s time once again. It?s plenty of theoretical papers at [www.lantmateriet.se](http://www.lantmateriet.se) for this subject.


I've cooked the essence of this into one Excel-file and one C-file.  

The Excel-file seldom miscalculate more then a few centimetres.  

The C-file adoption for AVR-processors can miscalculate a metre or so from time to time.  

This is most because of limitations in SIN() and COS() functions in math.lib.  

I added atanh directly in the mainfile.


The package contains a few PDF documents from [www.lantmateriet.se](http://www.lantmateriet.se) .


Software is written and tested on an ATMega2561V and STK500.  

It loops through 15 control points with a delay of 5 sec.  

A single transformation takes approx 6 msec.  

A 20x4 LCD display is also attached to PortC.


Update:


I went curious about the UTM international transformation.  

The fact that Swedish "new" grid is based upon it..


However a search at the internet gave me the fealing that even profesors at Green Bay, US where members of "Flat-Earth-Society"....


I went to a dozen homepages, all members of F-E-S.  

But then, I found Alan Morton's page <http://www.dmap.co.uk/utmworld.htm>


In main.c you'll find a few coords all calculated with utms.exe except the fact the prog is also a member of FES.


But still, please have fun!


Oops, today it's impossible to upload an extra zip-file....  

This is an alternative link


[http://www.ankarsvik.nu/download...](http://www.ankarsvik.nu/download/WGS84_to_UTM.zip)


Plz be patient.


/Thomas J  

Ankarsvik

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code
- CodeVisionAVR-Standard
