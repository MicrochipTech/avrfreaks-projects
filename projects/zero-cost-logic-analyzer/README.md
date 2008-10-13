# zero cost Logic Analyzer

Uploaded by heinrichs.hj on 2008-10-12 18:51:00 (rating 0 out of 5)

## Summary

Allthough not an AVR project, I would like to make this available to the AVR Freaks, hoping it will be usefull for many of the forum users.  

Please provide feedback to: [heinrichs.hj@web.de](mailto:heinrichs.hj@web.de)


lanaCOM is:


· Logic Analyzer  

lanaCOM is a command line tool implementing a logic analyzer (a logic analyzer is an electronic instrument that displays signals in a digital circuit). 


· VCD Output format  

Any VCD waveform viewer can be used to view the results. LanaCOM generates its output file in VCD-format. This is a standard format which can be read with any VCD-waveform viewer. ‘GTKwave' is a pretty good and free VCD- waveform viewer. 


· I2C, SPI and V24 Protocol Analyzer  

vcdTrans is a postprocessing command line tool which takes lanaCOMs output file and allows for analyzing I2C, SPI and V24 protocols. 


· Zero hardware cost  

No additional hardware is required. The four flow-control and modem signals of the PC serial COM port, CTS, DCD, DSR and RI are used for probes the signals. According to RS232 specification signals from +/-25V can be applied with no additional buffering required. 


· LanaCOM software is free  

Permission is granted to copy, distribute and/or modify the software and this document under the terms of the GNU Free Documentation License, Version 1.1 or any later. 


· 400 kilo samples per second  

The sample rate depends on the actual PC speed. About 400 kSamples/s are common. 


· Huge trace buffer space  

The PC-DRAM is used to store the samples during acquisition. File output is created during a post-processing step. About 1MB of DRAM is required per sampling second. With a 1GB of DRAM on common PC hardware buffering of 500 seconds and more at max. resolution will not be a problem. 


· Indication of potential inaccuracies  

As lanaCOM runs in a multitasking OS environment, the sampling process might be interrupted due to higher priority OS tasks. LanaCOM indicates whenever the sampling process is interrupted - this includes any hardware related interrupts like DRAM refreshes etc. 


· Trigger counter  

A virtual signal counts the number of trigger conditions to support orientation within a huge trace buffer.

## Tags

- Complete code
