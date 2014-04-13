# VERY ADVANCED CLAP SWITCH

Uploaded by SHARANYADAS on 2014-04-12 22:24:00 (rating 0 out of 5)

## Summary

Hi everybody....This is my 2nd post here after Voltmeter+Thermometer.This circuit as the name signifies that it is an advanced clap switch.Basically you can turn on/off different loads using different no of claps...Let me clarify a little bit more..Generally clap switches available on the web are designed for single load to on/off using single/double clap.But this circuit is little bit different.Here you are given a 4s time window.After the First clap is made,the cpu starts to count.If within the 4s after the first clap,you clap once more(i.e 2nd clap),a LED will turn on.Simply to turn the LED off,you have to clap twice WITHIN 4S.Same 3 claps are needed to turn the 2nd LED on and simple 3 claps are needed to turn it off.4 claps are needed to turn both LEDs off if you turn on both LEDs.Single clap is avoided due to accidental turn on of LEDs. The circuit is working 100% in real time. First the analog signal from mic is fed to a non inverting amplifier input of lm324 opamp1.Then the amplified signal is fed to the comparator input made of opamp2.RV1 and RV2 are for sensitivity adjustment.To avoid any type of distortion/false trigger,now the signal is fed to a 555 in monostable mode via a transistor darlington.The monostable output then fed to the micro.Whenever the micro gets an interrupt(i.e logic 1),it starts to count in the main function and after completing the count,it processes the commands depending on the no of interrupts.Sigle clap have no effect on the circuit to avoid false triggering.If second clap/interrupt is detected during the counting,then after finishing the counting it performs the desired action.several global flags are used to notify the micro what to do.Just go through the code and you will understand. Proteus screenshot is given as the circuit diagram.Mic+ and Mic- are the condenser microphone +/-.All other parts value are given in the diagram.Also hex and c code is attached.My prototype pic is also attached. Please add optocouplers to drive relays and increase no of claps and sensing time(here 4s) in the code to control more than two loads. If any problem occurs while building the circuit/understanding,please feel free to contact me.Also if you like the circuit/idea please rate the post. Rgds//Sharanya

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
