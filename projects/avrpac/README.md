# AVRPAC

Uploaded by admin on 2005-08-11 05:53:00 (rating 0 out of 5)

## Summary

AVRPAC is a version of the classic Pacman video game, for any AVR device with a UART/USART, connected to a dummy terminal through an RS232 link.


The screen looks like this:



```

Score: 0  
###################
#o...............o#
#G#####.....#####.#
#.#.............#.#
#.#..####.####..#.#
#....#...@...#....#
#.#..####.####..#.#
#.#.............#.#
#.#####.....#####.#
#o...............o#
###################

```

\@\ is the player, who can move around using these keyboard keys: A (left), S (right), D (down), E (up)  

'#' are walls that cannot be moved through.  

'G' is a ghost, which moves around and will kill the player if they meet. The player only has one life, so watch out!  

'o' are super-pills, which make ghosts edible for the player for a certain amount of time. Eating a ghost scores 20 points. When ghosts are edible, they are marked with lower case 'g'.  

'.' are food to be eaten. Eating one food pill scores 1 point. The level is complete when all the food is eaten. The next level starts immediately, with the same screen filled up with food, and one more ghost added (max 4).


The objective of the game is to eat as much food as possible and kill as many ghosts as possible, thereby achieving the highest score.

## Compilers

- IAR Embedded Workbench for AVR

## Tags

- Complete code
- IAR Embedded Workbench for AVR
