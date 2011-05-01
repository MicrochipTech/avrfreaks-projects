# AVR Butterfly Logger

Uploaded by NickLott on 2011-05-01 14:58:00 (rating 5 out of 5)

## Summary

AVR Butterfly Logger uses the Atmel AVR Butterfly hardware and open source tools to create a cost effective stand alone datalogger for logging light, temperature, voltage or any general sensor value. It is easily re-configured to suit your exact application.


Current inputs available:  

- 1 16 bit Counter input  

- Up to 5 ADC channels available.  

- Onboard Light Sensor  

- Onboard Thermistor Temperature Sensor  

- SHT75 Humidity and Temperature sensor  

- DS18B20 1-Wire temperature sensors  

- Full real time clock  

- KXPS5 3-axis Accelerometers ([www.kionix.com](http://www.kionix.com))  

- J/K Type Thermocouple via a MAX6675


Other Features:  

- Alarm clock. disable logging until a specified date and time  

- Full human readable text output or fast binary dump option.  

- Human usable serial interface.  

- Display Text on the LCD via the serial interface  

- Optional tick sound to let you know its logging (useful for short burst high frequency logging)  

- Logging frequency adjustable from 16Hz to once every 90 Hours  

- Logging dynamically based on change in sensor value. (Only log when signals are changing)


latest release is v0.31a which adds:  

- Dynamic logging based ADC readings  

- Bugfixes for LCD readout stops updating when flash is full  

- Imported some minor changes from Martin Thomas to LCD init.


[http://brokentoaster.com/butterf...](http://brokentoaster.com/butterflylogger/)

## Links

- [AVR Butterfly Logger](http://brokentoaster.com/butterflylogger/)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
