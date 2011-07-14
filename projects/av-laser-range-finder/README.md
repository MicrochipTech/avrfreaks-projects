# AV Laser Range Finder

Uploaded by solit_lv on 2011-07-14 06:19:00 (rating 0 out of 5)

## Summary

This project is a reverse engineering of the BOSCH DLE50 Laser Range Finder. The distance measurement is based on a phase shift method using AM modulated laser beam. Originally, the device is able to perform single or continuous distance measurement and display results on the screen.


In that project, the original firmware of the device's controller (ATMega169P) was replaced by the new one and now the device is working as a continuous laser range finder with external control via UART interface. 


Main parameters are as the following:  

Measurement rate â€“ 40Hz (25ms)  

Measurement range â€“ 0â€¦8Ð¼  

Measurement accuracy â€“ +/-5..10Ð¼Ð¼ (depends on conditions)  

External interface â€“ TTL UART 76800 8N1  

Power supply â€“ 4 x 1.5 V LR03


The two short movies about "how it works" and "reverse engineering" are posted on youtube.  

 Part I [http://www.youtube.com/watch?v=P...](http://www.youtube.com/watch?v=PsnzorA0WSs)  

 Part IV [http://www.youtube.com/watch?v=Y...](http://www.youtube.com/watch?v=Yz49Dr54nUc)


This project is an extension of the previous project "DIY Laser Ranger Finder". In the previous project the user was responsible for final distance calculation and some other control actions.  

This project is fully standalone and doesn't require any external calculations.


Change log:  

--- 14/07/2011 ---  

CMD\_LINE library is fixed in Control Software (PC based) package.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
