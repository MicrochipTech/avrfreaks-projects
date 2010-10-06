# Portable Off-Grid Solar Tracker

Uploaded by scottfromscott on 2010-10-05 18:37:00 (rating 0 out of 5)

## Summary

20W soar panel solar tracker controlled by Atmega 328p. Includes 400W inverter/battery/charge controller. C code developed for use with Pololu Baby Orangutan B-328, but could be ported with some modifications to other AVR's.


Hardware list:


12V, 20W Lavie solar panel and 12V-24V, 10A Wellsee charge controller: [http://cgi.ebay.com/Solar-system...](http://cgi.ebay.com/Solar-system-solar-panel-controller-cable-connectors-/270581575997?pt=LH_DefaultDomain_0&var=&hash=item84b6ee2c1f)  

Baby O 328B robot controller: [http://www.pololu.com/catalog/pr...](http://www.pololu.com/catalog/product/1220)  

HMC6352 compass module-tells panel azimuth/compass direction-inserted in breadboard: [http://www.parallax.com/Store/Se...](http://www.parallax.com/Store/Sensors/CompassGPS/tabid/173/ProductID/596/List/0/Default.aspx?SortField=ProductName,ProductName)  

DE-ACCM2G2 2-axis accelerometer-tells panel altitude/vertical tilt: [http://www.dimensionengineering....](http://www.dimensionengineering.com/DE-ACCM2G2.htm)  

Parallax GPS receiver-gets data needed by the sun position algorithm: [http://www.parallax.com/Store/Se...](http://www.parallax.com/Store/Sensors/CompassGPS/tabid/173/ProductID/396/List/0/Default.aspx?SortField=ProductName,ProductName)  

2 - 12V, 1RPM gear motors: [http://www.servocity.com/html/1\_...](http://www.servocity.com/html/1_rpm_gear_motor.html)  

gear motor end caps: [http://www.servocity.com/html/ge...](http://www.servocity.com/html/gearmotor_end_caps.html)  

I used the clamping hubs below, but had to tap them:  

2 - clamping hubs-attach wheel to azimuth motor,lid-hinge to altitude motor: [http://www.servocity.com/html/cl...](http://www.servocity.com/html/clamping_hubs.html)  

I think these would work better:  

2 - 1/4" bore set-screw hubs-attach wheel to azimuth motor,lid-hinge to altitude motor: [http://www.servocity.com/html/se...](http://www.servocity.com/html/set_screw_hubs.html)  

foam wheel-drives panel horizontally: [http://www.servocity.com/html/2\_...](http://www.servocity.com/html/2_00__foam_wheels.html)  

wheel adaptors attach clamping hubs to wheel: [http://www.servocity.com/html/wh...](http://www.servocity.com/html/wheel_adaptors__wad5130_.html)  

2 - limit switches-to protect altitude motor from stripping gears: [http://www.pololu.com/catalog/pr...](http://www.pololu.com/catalog/product/1403)  

6in. lid hinge (local hardware store)  

Cobra 400W power inverter: [http://www.amazon.com/gp/product...](http://www.amazon.com/gp/product/B001RNOHBC/ref=oss_product)  

12V, 14AH AGM battery: [http://www.batteryspec.com/cgi-b...](http://www.batteryspec.com/cgi-bin/cart.cgi?action=link&product=223)  

breadboard, 5A fuse, 3A diode, 7805 & 7812 regulators, 22GA hookup wire, 4.7K resistors, 4-conductor m/f plugs,  

pack of colored electrical tape-to identify wire functions (Radio Shack)  

Sheet metal base and panel support is 1/8" scrap aluminum (local recycler)  

#8 nuts, bolts, small angle brackets, teflon tape, washers, 6" lid hinge-drives panel up an down vertically,  

(#6-32 hardened allen screws for tapped clamping hubs) (local hardware store)  

6" lazy-suzan turntable (Home Depot)


Software:


For the compass drivers, Jim Remington has a zip file containing Peter Fleury's ic2master.c and i2cmaster.h which works with Baby O at:  

[http://www.uoxray.uoregon.edu/or...](http://www.uoxray.uoregon.edu/orangutan/i2c.zip)  

The rest of the required software is available in the attached zip files.  

Read the comment sections for explanations of how each function works and examine the pinout diagram for each sensor.  

The \_myincludes.h should be the only #include needed as it contains all needed pololu .h references.  

I stored \_myincludes.h in the avr/../pololu/ subdirectory and call it from the main program with #include  


For AVR Studio 4 users:  

 Under Project, Configuration Options, Libraries, add libpololu\_atmega328p.a, libc.a (to build without errors)  

 Under Custom Options, [linker options], add -Wl,-gc-sections (builds only library functions used by your program)


For those of you who wish to customize the declination() function in \_myincludes.h, I have included the Declination Table Development Kit


Tools:  

screwdriver, sabre saw w/ metal-cutting blade, pliers, drill, drill bits, (#6-32 NC tap), scrap plastic 


Finally, I have a blog which describes the parts of the system and includes some videos and photos here:  

Project overview: [http://scottfromscott.blogspot.c...](http://scottfromscott.blogspot.com/2010/06/portable-solar-pv-panel-tracker.html)  

software overviews:  

\_myincludes.h overview: [http://scottfromscott.blogspot.c...](http://scottfromscott.blogspot.com/2010/09/portable-solar-pv-tracker-cc-library.html)  

main functions overview: [http://scottfromscott.blogspot.c...](http://scottfromscott.blogspot.com/2010/09/portable-solar-pv-tracker-main.html)  

hardware overview: [http://scottfromscott.blogspot.c...](http://scottfromscott.blogspot.com/2010/09/portable-solar-pv-tracker-hardware.html)

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
