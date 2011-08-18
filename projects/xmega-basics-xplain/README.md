# XMEGA Basics for Xplain

Uploaded by valleyman on 2011-08-18 11:27:00 (rating 0 out of 5)

## Summary

Atmel AVR1512 is a group of training projects designed for XMEGA A1 Xplained (blue) board. For the purpose of the training, this blue board and Xplain (green) board are different in port connections. As my first foray into learning AVR programming, I made changes in board.h and some code so they will work for Xplain board. Because Xplain connects all soft switches to a single port, unfortunately, one ingenious example of using group masks has to be reduced to unnecessary (ab)use of group mask.


The original code is packaged in AVR Studio 5. Because Freaks projects hasn't added this as a choice, I'm temporarily parking this under WinAVR because the code is said to work "for both the GCC and the IAR compiler." I included makefiles produced by Studio 5 but you probably don't need one. Every project is a single .c file. Of course, you can use AVR Studio 5 to open xmega\_basics.avrsln. (Studio 4 format is totally different from Studio 5's.)


To reduce confusion, the original header file board.h is removed. The projects use a distinct header xplain.h, which is a merger between xplain.h supplied by Studio 5's Xplain template and board.h from AVR1512. As a result, there are many elements unrelated to this training.


The application notes are in [http://atmel.com/dyn/resources/p...](http://atmel.com/dyn/resources/prod_documents/doc8398.pdf). Copyrights in this project remain with Atmel, leave copyright notice intact, blah, blah.

## Compilers

- AVR Studio 5

## Tags

- Complete code
- AVR Studio 5
