# SLINK

Uploaded by dejan on 2001-09-09 08:56:00 (rating 0 out of 5)

## Summary

SLINK is assembled code implementing simple parallel-serial-parallel link which enables sending 8 bits of  

data over single wire.Two chips can be cascaded in master-slave fashion, giving total width of 16 bits. The packet data rate depends on the clock frequency of the transmitter, but receiver self-adjusts to the incoming data stream , allowing simple RC oscillators for cheap solutions. The code can be implemented on all AVR devices.In the future, code for Tiny devices with A/D-D/A converters will be done. The code for RX/TX is imlpemented in one module and RX/TX selection is done via single pin. The outputs of the receiver are configured as open-drain. When no data is present at the input, all outputs go off-state. Pin-selectable function for reliable operation requires four identical data packets to be received for output to change state.

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
