# rs 484 communication

Uploaded by marifin.ardiansyah on 2010-05-15 10:46:00 (rating 0 out of 5)

## Summary

rs485 is a old comunication system, but still exist at this time. it's good for long distance wire communiaction because the concept of data transver use 2 line with complementer voltage. 


in this project, i want to make system in weighing for ship load. i use 3 "slave" (i don't know can it be said as slave), and master that collecting data and save it indata base. the process of data aquisition concept can be shown like this:


push button (slave) >>>>> slave send permision signal to slave and then waiting green light >>>> master gto the signal and make decision can slave send data know, if oke master send ack signal >>>> slave get it, and then send data signal (5 byte data ).


i use 10 bit adc and use simple data process to make simple data packet (delimeter, slave addres, datahigh, datalow, terminator.) 


[rs 485 communication.rar](http://www.4shared.com/file/puX5Cn6s/rs_485_communication.html)

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code with HW
- CodeVisionAVR-Standard
