# The Clapper 2.0

Uploaded by dhahaj on 2012-09-03 17:48:00 (rating 0 out of 5)

## Summary

Like the original clapper, this is designed to trigger an output voltage when a clapping sequence is heard. But unlike the original, the clapping sequence is programmable via a button and is stored in non-volatile memory so that the sequence is not lost on reset. The programming button also doubles as a toggle switch when held for a programmed amount of time. 


For the microphone circuit, a simple small-signal amp would be sufficient. I attached a picture of a sample circuit that could be used. Modify the bias resistors accordingly. The sensitivity is also "soft" adjustable via a pot connected to the ADC.


You must use a relay with contacts rated for high voltage and logic-level control. Take care to provide proper isolation between the high and low voltages. It is also wise to fuse the line voltage in case of failure.

## Compilers

- AVR Studio 5

## Tags

- Complete code
- AVR Studio 5
