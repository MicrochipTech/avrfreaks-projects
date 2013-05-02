# Espion RS232

Uploaded by ElectroGege on 2013-05-01 18:01:00 (rating 0 out of 5)

## Summary

J'ai utilisÃ© ici une Arduino2560 car cette carte est super complÃ¨te et intÃ©gre un des rares Âµc intÃ©grant + de 2 interfaces sÃ©ries (il m'en fallait 3 pour ce projet). Le luxe c'est que cette carte intÃ©gre Ã©galement un convertisseur RS232 vers USB. Toutefois, Ã©tant partisan acharnÃ© de l'assembleur, je n'ai pas utilisÃ© le langage Arduino, mais bien le pur assembleur sur cette carte. Ce projet rÃ©cupÃ©re donc les donnÃ©es circulant dans les 2 sens de communication via les pins RX1 et RX2 et les envoie vers TX0 aprÃ¨s mÃ©morisation et conversion Ã©ventuelle en hexa et la carte Arduino envoie ce contenu format sÃ©rie vers l'USB.  

La carte est alimentÃ©e par l'USB et les donnÃ©es espionnÃ©es peuvent circuler Ã  diverses vitesses (format 7 ou 8 bits), alors que le rÃ©sultat de l'espionnage est toujours envoyÃ© vers le PC Ã  115200 bauds.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
