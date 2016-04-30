# MEGA TETRIS board (ATmega RGB LED display with pack of classic games and UART control)

Uploaded by dragondreamer on 2016-04-30 09:41:00 (rating 5 out of 5)

## Summary

Here's my new hobby project: ATmega RGB LED display with pack of classic games and UART control (MEGA TETRIS board).


 


**What's this**


This project is RGB LED display (16x10) based on WS2812B LEDs. Its core is ATmega644pa controller running on 16 MHz frequency. The board has several buttons (up, down, left, right, forward, back, reset) and ADXL345 accelerometer. It also has 5-digit numeric display. There're also UART interface (MAX232A-based) and SPI programming interface for USBASP or similar programmer.


 


**This project contains:**


* Board schematic
* PCB
* C++ firmware code for ATmega644p(a) (uses C++11 features, compiles in Atmel Studio 6.2 or later)
	+ Animated menu
	+ Games (support buttons or accelerometer control and high scores)
		- Snake
		- Tetris
		- Space invaders
		- Asteroids
		- Maze
	+ UART-based control protocol
	+ Debug mode
* [Winamp player](http://www.winamp.com/) visualization plugin
	+ Three different visualizations
* Board Debug Console C# code for Windows

 


Full source code for all parts of this project along with some screenshots and pictures can be found on [GitHub project page](https://github.com/dragon-dreamer/RgbTetris).

## Links

- [GitHub project page](https://github.com/dragon-dreamer/RgbTetris)
- [Demo video](https://www.youtube.com/watch?v=IZfsuTzZs8U)

## Compilers

- Atmel Studio 6

## Tags

- Complete code with HW
- Atmel Studio 6
- WS2812B
- ATmega644PA
- Home Entertainment
- RGB LED
- Tetris
- Snake
- Space invaders
- Maze
- Equalizer
