# RC Speed controller

Uploaded by trace on 2011-03-17 23:22:00 (rating 0 out of 5)

## Summary

RC Speed controller with low voltage cutoff and safe startup.  

It has a led that shows the status of the speed controller. 


The PCB layout is about 2 x 2 cm x 2 layers. 


HOW IT WORKS? 


First of all it's fantastic that the small tiny chips have built in PWM! Now it's possible to make a really small RC controller with some nice features. 


Actually the design and software is very simple but anyway, here is a small description on the works. 


HARDWARE:  

The hardware as I built it is based on an ATtiny2313 and one or more FET's recording to the current load. I use one of the PWM outputs in the Tiny to drive the FET and uses the analog comparator as the low voltage cutoff detector.  

R4 & R7 divides the battery voltage and is connected to the internal comparator. The other comparator input comes from the internal voltage reference. By changing the R4 & R7 it is possible to control exactly which voltage the cutoff should be triggered.  

The FET can be anyone desired, however it's important that one uses a FET with a very low RDSON resistance. ST has some very nice ones!


SOFTWARE:  

The software uses some of the internal features of the micro. The signal from the RC receiver is a pulse with modulation. I use the internal capture compare to measure the length of the pulse. The signal (taken from the original standard) varies from 1ms to 2 ms with about 20 ms apart.  

When the micro gets a measurement from the capture compare device it compares it to a lower and upper value. In this way it's possible to determine if the signal is real or garbage. 


Garbage Collector:  

The garbage collector is a way to determine the quality of the signal from the RC receiver. With the garbage collector, the motor will stop if the signal from the receiver disappears. It also prevents the motor from starting when turning on the power without you having turned the transmitter on.  

In the software you will find a "GoodCounter". This counter is a gauge that I use to determine the signal quality. It's a point system that awards a perfect signal with to points and takes one point from a bad signal. If the gauge is above 30 then we are ready to control the motor. However, if the gauge reaches 0 then we stop the motor. 


SafeStart:  

Safe start is a mechanism that prevents the motor from starting in full speed when power is applied to the RC speed controller. Before you are allowed to start the motor you have to move the stick on the transmitter to the lowest speed possible. Then you can move the stick to a higher speed that will start the motor. 


Low Voltage Cutoff:  

The low voltage cutoff uses the built in analog comparator to determine the voltage level on the battery. If the voltage drops below a certain level, the software shuts down the motor. Then the user has to move the stick on the transmitter to motor off position to gain control on the motor again. 


That's all for now! I will try to put some more comments into the software itself but for now this is what you get.

## Tags

- Complete code with HW
