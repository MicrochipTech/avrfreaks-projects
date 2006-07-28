# The Programmable Chip Electroencephalogram EEG

Uploaded by josh_eeg on 2006-07-27 19:27:00 (rating 0 out of 5)

## Summary

The Programmable Chip EEG is a Multi-channel electroencephalograph that could be a brain-computer interface.


What it is and what it can be used for:


The Programmable Chip EEG is a Modular Multi-channel electroencephalograph that could be used for a brain-computer interface, biofeedback....


The Programmable Chip EEG is programmable because it uses Digitally Programmable Potentiometers. They are used to set the Gain, frequency range, and can calibrate multiple analog boards for different EEG tests.


Status:


We currently have a (preliminary?) circuit design and a (prelimary) PCB layout. Eventually, kits could be sold, if there were interest. More info is available at the PCEEG SourceForge home page at <http://PCEEG.sourceforge.net>.


News:


The design is switching to EAGLE CAD, because the auto-router is excellent for open-source productivity. It will allow the design to evolve faster and with less work with a wider selection of PCB production houses to choose from.


The ideas and monkey to robot arm interface by Miguel Nicolelisis very inspiring. He used implanted electrodes to monitor and allow monkeys to control a robot arm as if it was their own. Could the PCEEG do the same with its electrodes on the scalp. Only the future will show.


Discription of How it Works:


The daughter signal prossesing board is responcible for filtering and digitising the signals from the body, then the signals are passed to the control board that is the motherboard. The motherboard then can pass the signal to a larger computer.


The differential analog signal is amplified by the instramentation amplifier. Then the signal is amplified and conditioned by high pass and low pass filters.


The common mode signal is inverted summed and sent back to the body to decrese common mode signal thsi is done by the driver right leg circuit.


Band reject filters may be added soon in the future.


Then the analog signals are simultaniously digitized and the digital values can be clocked to the controler motherboard.


The controler board that is the mother board of the system and the signal prossesing daughter boards plug into is the AVR Butterfly


Parts used in the PCEEG:


 * LT1168 Programmable-Gain Precision Instrumentation Amplifier is used to amplify the weak electric signals coming from the brain through electrodes attached to the scalp, etc.  

 * Several LT1114 Low Power Precision OP-Amps are used for amplification and filtering of the signal  

 * The Digitally Programmable Potentiometers AD5204 4-Channel Digitally Controlled Potentiometer is used to make calibration of this device easier. (Historically, a weak point of homebrew designs has been their sensitivity to component value; tolerance and drift have made them require constant tweaking for good results.) 


To reduce noise in the readout of the analog part of this circuit, a common-mode feedback is passed back into the body by the driver right leg circuit. Band reject filter may also be used (to reject, for example, 60Hz noise from nearby A/C power wiring).


The analog signal prossesing board will digitise the signal and pass the data to the control board. The control board is based on the AVR Butterfly which will, further process the signals, and provide an interface to a computer & lcd display.

## Links

- [The Programmable Chip EEG Sourceforge Website](http://pceeg.sourceforge.net/)
- [The Programmable Chip EEG Open Circuits Wiki Website](http://opencircuits.com/Main_Page)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
