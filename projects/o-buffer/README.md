# O Buffer

Uploaded by eanwar on 2010-02-28 13:53:00 (rating 0 out of 5)

## Summary

AVR101


Having a system that regularly writes parameters to  

the EEPROM can wear out the EEPROM, since it is only  

guaranteed to endure 100 k erase/write cycles. Writing  

the parameters to a circular buffer in EEPROM where  

each of the elements in the buffer can endure 100 k  

erase/write cycles can circumvent this. However, if  

the system is exposed to RESET conditions, such as power  

failures, the system needs to be able to identify the  

correct position in the circular buffer again 


see all my projects at  

<http://www.ehab.1free.ws/index.html>

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code
- CodeVisionAVR-Standard
