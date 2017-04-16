# SAMB11

Uploaded by mvdzwaan on 2017-04-16 07:43:00 (rating 5 out of 5)

## Summary

This project contains some lessons learned for the Atmel/Microchip SAM B11.


 


The SAMB11 documentation and samples are not complete at this moment and I have learned a couple of things about this MCU the hard way.


 


* [Startup time](#startup)
* [Watchdog timer reset](#watchdog)
* [ULP mode](#ulp)
* [Unique ID](#uniqueid)
* [Power saving](#powersaving)
* [Return from ULP](#returnULP)
* [Bit banging](#bitbang)
* [OTAU](#otau)
* [Systick](#systick)
* [Timer](#timer)

 


Startup time


 


The SAMB11 is several chips on one pcb. A Bluetooth chip (BTLC1000), a Cortex M0 mcu and spi flash memory. On startup the MCU needs to copy the application firmware from SPI flash to its internal memory and then start the actual firmware. This means the SAMB11 has a startup time of 3~4 seconds which is quite different from other MCU's which have almost instant startup


 


Watchdog timer reset


 


The watchdog auto reset does not seem to be working when using the WDT ISR. The ASF libraries register this ISR even when not using callbacks for the early warning system


 


wdt\_set\_config contains


 



```

system\_register\_isr(RAM\_ISR\_TABLE\_NMI\_INDEX, (uint32\_t)wdt\_isr\_handler);
```
 


and the wdt\_isr\_handler checks the existence of the callback.


 


There are 2 solutions


1) Use the early warning callback as reset and perform   platform\_chip\_reset(); 


2) remove the isr and use the wdt reset function


 


ULP mode


 


To use ULP mode at the moment it's easiest to use the ASF functions. They perform certain kernel post messages which make the SAMB11 actually enter ULP mode.


 


To enter ULP mode there are 2 conditions which need to be met


1) Any AO GPIO pin with wakeup enabled needs to be 'low'


2) The sleep lock needs to be released. This can be done use the platform release\_sleep\_lock(), or using ble\_set\_ulp\_mode(BLE\_ULP\_MODE\_CLEAR); (which just make the next functions call this platform function)


 


After these conditions are met, ulp mode can be entered using



```

platform\_event\_wait(0);
or
ble\_event\_task(BLE\_EVENT\_TIMEOUT);
```
 


This also means ULP can only be left when calling


 


send\_plf\_int\_msg\_ind() or [lookup post ble user event]


 


Wake from AON timer can be done using the sample (WAKEUP MODE ARM) and is working fine


Wake from AON GPIO does not seem to work. The MCU wakes (as can be seen from its power consumption) but the platform message does not get send. I have worked around this using a AON timer with a counter every 0.5 sec. When the counter reaches 120 (2 minutes) I wake for my timer event. At every callback I check the AO GPIO (these do not need re-init in this ISR) and wakeup with some latency.


 


Unique ID


 


To get the unqiue MAC ID (also printed on the module)


 



```

at\_ble\_addr\_t BLE\_address = {AT\_BLE\_ADDRESS\_PUBLIC, {0xAB, 0xCD, 0xEF, 0xAB, 0xCD, 0xEF}};
ble\_device\_init(NULL);
at\_ble\_addr\_get(&BLE\_address);
```
 


Setting a custom MAC ID can only be done using the ble\_device\_init and thus it's not possible to read the MAC ID (and use it for device specific keys etc) and then change it to not have your secret made public.


 


This can be solved by using the NVDS area on the spi flash. Note: On first boot the MAC ID is read and saved and a reset is done. So first reset is ~10 seconds.



```

    spi\_flash\_turn\_on();
    spi\_flash\_init();
    spi\_flash\_read((unsigned char *)BLE\_address.addr, (unsigned long)FLASH\_NVDS\_START\_ADDRESS,6);
    if (BLE\_address.addr[5]==0xf8 && BLE\_address.addr[4]==0xf0 && BLE\_address.addr[3]==0x05) {
        spi\_flash\_turn\_off();
    } else {
        ble\_device\_init(NULL);
        at\_ble\_addr\_get(&BLE\_address);
        spi\_flash\_turn\_off();

        spi\_flash\_write((unsigned char *)BLE\_address.addr, (unsigned long)FLASH\_NVDS\_START\_ADDRESS,6);
        platform\_chip\_reset(); //reset mcu
    }
```
When calling ble\_init instead of platform\_init it should be noted it's best to use the ble function for entering ULP !


 


 


Power saving


 


To save some extra power, especially when using ULP do not forget to turn of power to the SPI flash.


 



```

spi\_flash\_turn\_off();
```
On my board (with canbus, i2c expander, rtc, quectel gps, microchip lora module, 3.3V and 5V regulators, accelerometer, fuel gauge) I can get to 110~130uA 


 


Crashes


 


I can get the SAMB11 to crash (crash meaning no code seems to be running anymore and the WDT is not resetting the MCU)


 


* Using ULP for 3~4 days (should be firmware specific)
* Changing gpio's from input/output and pulldown settings. Especially when entering ULP.
* Using TIMER, I've switched to DUALTIMER (at the start even without using both) and have had no problems at all
* Manipulating GPIO which are used in modules

 


Return from ULP


 


After returning from ULP all modules have been reset. So UART/I2C/SPI etc. all have to be reinit. You can use the resume callback for this mechanism.


 


I also init all GPIO again, bot the config and the state. Documenation is not clear about the GPIO (only that state is maintained using latches)


 


Bitbanging


 


Because of my crashes (both manipulating gpio from modules and ULP) and the need to set some ports low to save power (one of my chips draws power from my UART when put into sleep so I want to set the GPIO level to low on the SAMB11 to stop this leakage) I have implemented bitbanging libs for the SAMB11 for


 


* I2C (no clock stretching support)
* SPI (only 1 mode, with SCK low)
* UART (one at a time, limited to 9600 at 4x sampling, but should be able to go faster)

 


Other advantages are that my total firmware went from 74Kb to 63Kb when not using ASF for these modules, and I can use any GPIO.


 


Note it did not help with my ULP crashes (after 3~4 days).


 


OTAU


 


I have succesfully integrated the OTAU sample in my own firmware.


 


The OTAU sample does not working when compiling in any other setting than -O0 or -O1. At O2/Os the sample does not work


 


I have integrated the OTAU using the exact same code as the sample and calling it main function OTAU\_main. At startup I check a certain condition and enter OTAU\_main. So the OTAU module runs completely separate from my own firmware. My firmware also contains the option of resetting the mcu using keypad to enter startup.


 


I run the OTAU code without ULP.


 


After about 10 minutes in OTAU mode using the sample code the SAMB11 crashes. If you have no means of resetting your mcu externally this can be a big problem


 


You need to disable any dualtimer in use before entering OTAU, or else it's not working ok.


 


The OtauImageCreator.exe is missing from the BlueSDK 5.2 release for Atmel. It can be found in the Keil release.


 


 


 


Systick


 


Some ASF examples contain :


 



```

	/**
       * For make this QS work, disable the systick to stop task switch.
       * Should not do it if you want the BLE functions.
       */
    SysTick->CTRL &= ~SysTick\_CTRL\_ENABLE\_Msk;
```
I have found that the SysTick needs to be disabled if you have time critical ISR's. 


 


So I needed to disable it when using my UART bitbang implementation. Otherwise I would get a corrupted character every ~200 transfers at 9600 baud.


 


Timer


 


Using TIMER and having issues, switch to using DUALTIMER and see if it works.

## Compilers

- Atmel Studio 7

## Tags

- Part-specific Lib.functions
- Atmel Studio 7
