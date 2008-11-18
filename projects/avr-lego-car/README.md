# AVR - Lego Car

Uploaded by Listen on 2008-11-18 11:23:00 (rating 0 out of 5)

## Summary

Here is my first project!!! I made this car in order to learn and practice on AVR. I would really appreciate your comments and advises cause I am new in this thing and I am looking for good information from experienced people. The mechanical part is made with Lego. 


I have made it a photovore car, in order to test it, but it can easily be programmed to do something else. The big work was to write the routines for moving, turning etc. 


It has three motors:


The first moves the whole car in both directions (Wheel motor). We can adjust its speed using PWM method. We can also measure the distance that the car covers with a pulse generator which is connected to AVR. This generator is made by an IR led and an IR detector. Between these there is a wheel with a hole. Every time the hole pass between the IR led and IR detector we have a pulse. This happens when the wheel completes a full rotation.


The second motor turns the front wheels (front wheels can have any direction we want cause there is a potentiometer connected to the wheels and supplying the AVR ADC for feedback).


The third motor rotates the "hut" of the car. The hut is a circle pad where we can place the sensors we want (i have place a photoresistor) or a camera or something else. We can control exactly the angle of this pad with a Pulse generator just like the way i described before but we have to initialize hut position first. That's why there is a contact (tow screws) which tells the microcontroller if the hut has achieved the limit of his route.


We can also detect a crash of the car cause there is a contact at the middle of the car witch is released every time the front part of the car hits an object. Unfortunately I haven't use this capability in my code.


All the moves are time limited. If a motor is blocked due to mechanical reasons and can't complete its move, it will stop moving in order to save the mechanical part and the transistors. 


Finally, the car can be switched in pc control mode if we connect a jack at the USART connection point. Then we can do what we want with a PC program.


Here are the main routines:


START\_MOVING: Moves the car. The speed is number from 0 to 255 and we store it in a register which is defined as "speed". The direction is control by register "direction". Values "front" and "rear" are defined.


STOP\_MOVING: Easy to understand...


MOVE\_DISTANCE: Moves the car at the speed, direction and distance we want. The desired distance is placed on "distance register" and it represents wheel full rotations.


TURN: Turns the front wheels at the position we demand in "steering" register. 0 is full left position and 255 full right position.


INIT\_HUT: Initializes the hut position.


MOVE\_HUT: Turn the hut to the position we demand in "angle" register. Angle 0 is full left position and angle 48 full right.


WAIT: Delay 10ms * wait\_x\_10ms register.


I really sorry for my English...


Michalis

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
