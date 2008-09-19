# ADC using capacitor

Uploaded by Armor on 2008-09-18 19:50:00 (rating 0 out of 5)

## Summary

This project reads an approximate value of an analog voltage applied to a voltage dependent resistor(VDR) and determines the level of this voltage(level 0,level 1, level 2,....). These level can be adjusted by the designer as reauired.


The idea of this project is similar to the single-slope(integrating) ADC,depends on measuring the voltage on a capacitor after a fixedtime quantum


Time quantum is estimated by on-chip timer


Notes:  

1.The results are not accurate, so it is only suitable for determining the level of the input frequency, and never be used for accurate results, for accurate results you may use the on-chip ADC or an external ADC


2.The idea of this projet is excerpted from John Main's project "ADC with no ADC"

## Links

- [Varistor](http://en.wikipedia.org/wiki/Varistor)
- [Integrating ADC](http://www.allaboutcircuits.com/vol_4/chpt_13/8.html)

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code with HW
- CodeVisionAVR-Standard
