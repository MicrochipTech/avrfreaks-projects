# Speed Autobaud

Uploaded by boseji on 2008-02-24 05:19:00 (rating 0 out of 5)

## Summary

There have been many attempts to make fast Auto-bauding algorithms.  

The basic requirement arises from the bootloaders and customized User interfaces using the Serial Port of the Microcontroller.  

Here is my attempt to do the same.


In this code I am using a ATmega16 clocked at 4MHz.  

The Timer1 of this AVR MCU is cable of Output compare.  

For the Autobauding I transmit the English character "U" which is actually 0x55 in ASCII.  

Here is how its done:  

************  

The "calb\_uart" routine looks for the First lower edge on the PD0 line and then start Timer1 in Normal mode. This is basically the Start Bit of RS-232 Communication.


The next step is to stop the timer as and when the first rising edge of the Data bit 0 from the 0x55 pattern sent. Then Stops the Timer1 and loads the value to the OC1A.


Now the Timer1 is configured for Clear-on-Compare-Match mode (CTC).When the Timer1 is started now it will get cleared when the value matches OC1A. 


The counter is initialized to load up 8 bits from the serial stream. 


The counter is started and the bits are loaded by RIGHT shifting the DATA register and then or-ing the Highest Bit in case PD0 of PIND reads a 1.


Then Wait for the OC1A compare match event to occur and clear the Flags and then restart the Timer.


Continue the above for two steps for all the 8 bits.


Now after all the 8 bits are received you get the Data from the Serial Stream. Compare it if its the "U" that was transmitted if its okay you have the Serial AUTO Baud done else continue the whole process again.  

***********  

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  

3,Feb,2008  

------------  

This was the OLD Algorithm. I have now devised a new faster algorithm. The Code for the Same has been updated in the URL link.  

Here is it:  

>>>************


1. The "calb\_uart" routine looks for the First lower edge on the PD0 line and then start Timer1 in Normal mode. This is basically the Start Bit of RS-232 Communication.


2. The next step is to stop the timer as and when the first rising edge of the Data bit 0 from the 0x55 pattern sent. Then Stops the Timer1.


3. The Timer count is loaded into a temporary location say Temp1.  

The calculation for the actual baud rate to be loaded into the baud rate generator register UBRR is done by the following calculation:  

 We know that,  

 Fbaud = Fosc / (16 * (UBRR + 1)) ->In Normal Async Mode --EQ1


 Also for the Timer Value in Temp1 for the Start Bit Duration,  

 Tbit = (1/Fosc) * Temp1 * (Prescalar Value) --EQ2


 However the Prescalar used in this case is 1.  

 Hence,  

 Fosc = Temp1 / Tbit [From EQ2] --EQ3


 Now,  

 Fbaud = 1/Tbit [By definition of Baud Rate] --EQ4


 Substituting in EQ3: - Assuming that we have go the correct timing  

 Fosc = Temp1 * Fbaud --EQ5


 Now substituting in EQ1,  

 Fbaud = Fbaud * Temp1 / (16 * (UBRR + 1))  

 or, Temp1 = 16 * (UBRR + 1)


 Hence We have::


 UBRR = (Temp1 / 16) - 1 [Our Final Relation]


 So now we know how the Timer Value Temp1 is related to the UBRR.  

 But in practice when all these calculations are done there is some inherent delay that is compensated using the following Equation to get the final UBRR value.


 UBRR = ((Temp1 + 3) >> 4) - 1;


4. Now this UBRR value is loaded to the Baud Rate Registers


5. Check for the Reception of a proper 'U' for 4 times


6. If it is there then Exit  

 Else If Dual Speed mode Feature is Enabled continue  

 Else Goto Step 1 and Reinit all the parameter try again.


7. Calculate UBRR value as: UBRR = ((Temp1 + 3) >> 3) - 1;


8. Set the DOUBLE SPEED ENABLE bit U2X and load the UBRR register


9. Check for the Reception of a proper 'U' for 4 times


10. If it is there then Exit  

 Else goto Step1


************<<<


Hope that this would be helpful many people.  

Please feel free to suggest me all the improvements that might be necessary or if you have any doubts in the algo.


AVR RulZ !!! ;-)  

\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_\_


Warm Regards,  

Boseji

## Links

- [C File containing the Actual Code for Speed AutoBaud](http://prog.ic.googlepages.com/serial_a.c)
- [Link to My Web Page Describing this Algorithm](http://prog.ic.googlepages.com/a_serial)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code
- AVR32 GNU Toolchain
