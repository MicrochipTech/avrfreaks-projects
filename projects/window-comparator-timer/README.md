# Window comparator + timer

Uploaded by laurentppol on 2013-07-06 17:19:00 (rating 0 out of 5)

## Summary

In Poland You may opt for G21 "grid" tariff: cheap 15-17 (summer), 13-15 (winter), 22-6 (all year), and costly other time (2x)...  

So I designed a window comparator, to switch on/off inverter of my hybrid (solar + wind) "green" power station, according to battery voltage and time.  

During "cheap" tariff, I want to mainly charge batteries, while during "costly" tariff, mainly to drive the load.  

Inverter is 500W, wind turbine is 500W, and solar panels are 360W (nominal).  

Batteries are 170Ah / 24V (Trojan 8V x 3).


Hardware config is simplest it can be: 1x uC, 1x relay, 2x LEDs, 1x jumper, 1x 7805. (attached)


Program is written in Bascom BASIC :( sorry... [simple for start]


It is incomplete, missing winter/summer switching.

## Compilers

- BASCOM-AVR

## Tags

- Complete code with HW
- BASCOM-AVR
