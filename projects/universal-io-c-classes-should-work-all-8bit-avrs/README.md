# Universal I/O C++ classes that should work with all 8bit AVRs

Uploaded by jvalencia on 2016-04-24 11:23:00 (rating 0 out of 5)

## Summary

I've finished these 2 classes to manage I/O pins. I wanted 3 main things with these libs:


 


- Abstraction, so they work with every 8bit AVR mcu.


- To manage different active levels for each pin (useful for pullup pins and such).


- Faster than arduino libs.


 


A sample blink.cpp program looks like this:



```

int main() {
    Pin led(Port::B, 5, OUTPUT);
    
    while(true) {
        led.high();
        delay(1000);
        led.low();
        delay(1000);
    }
}
```
 


If you want to invert the output of that pin, pass the active level to the constructor as this (or use the "mode()" function):



```

int main() {
    Pin led(Port::B, 5, OUTPUT, LOW);
    
    while(true) {
        led.high();
        delay(1000);
        led.low();
        delay(1000);
    }
}
```
 


That way, when issuing a:



```

led.high()
```
the output will be 0 volts accordingly.


 


Another example, reading the state of a button:


 



```

int main() {
    Pin led(Port::B, 5, OUTPUT);
    Pin button(Port::D, 2, INPUT\_PULLUP, LOW);
    
    while(true) {
        if (button.active()) {
            led.toggle();
            delay(200);
        }
    }
}

```
 


For reading a pin we use the "active()" function. This function also understands different activation levels.


 


In this case we used a internal pull up resistor for the button, so 0 volts means the button is pressed. That's why the button object has LOW in the active level parameter.


 


Other functions are:


 


- Pin::high() -> sets that pin to a HIGH "logical" value.


- Pin::low() -> sets that pin to a LOW "logical" value.


- Pin::toggle() -> toggle the state of an output pin.


- Pin::active() -> returns a boolean indicating if that input pin is HIGH or LOW depending on the activation level.


- Pin::mode() -> change the pin mode and activation level.


 


The Port class shouldn't be used directly, but you can.


 


The different Port variables (A, B, C...) are automatically declared if the assosiated PORTX is defined. I've added up to PORTG (atmega256). I think this is the maximum port for any 8bit AVR device.


 


These libs are GPL v2.


 


Hope you find the code useful.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
- atmega
- Attiny
- Library
- I/O port
