# Playing music with Atmega328 PWM

Uploaded by Rivello on 2011-08-27 23:08:00 (rating 5 out of 5)

## Summary

Videos of the projects:  

[http://www.youtube.com/watch?v=d...](http://www.youtube.com/watch?v=dWjsupWdNiI) (Brasileirinho - Guitar)  

[http://www.youtube.com/watch?v=v...](http://www.youtube.com/watch?v=v3vlXswfGGs) (Turkish March - Piano)


Topic in the forums (with source files attached):  

[https://www.avrfreaks.net/index.p...](https://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=111135&highlight=)


In the program memory is stored an array of 10,000 bytes which represents the waveform of the note C4 of a piano.  

Outputting these bytes with PWM to a speaker will reproduce the sound of a piano note.


Every byte should stay in the PWM output for a specific period of time. Changing this time you are changing the frequency of the timbre, and so changing the tone either.  

Playing faster you increase the frequency making higher notes and the opposite is true.


(I said piano, but the current timbre of the project is of a guitar)


What we do, is store in the program memory the sequence of notes to be played and the time that each should be played for.  

The "Periodo" array stores the notes while the "Tempo" array stores the times.  

As it can play two notes simultaneously, there are 2 of each array.


It uses timers 1 and 2 to control how much time every byte of the waveform should stay in the PWM output. Timer 0 is used for the PWM.  

When the timer runs out, the interruption is called and it loads the next byte of the waveform array. When the waveform array ends, it loads the next note from "Periodo" array and play the waveform again in the time according to the note.


The code may be kind hard to understand, but as it is short, it's not so difficult.


Any question, suggestion and criticism, please mail-me.


NOTE: As I can't manage to upload the files please, if you'd like to, download it from here:  

[http://code.google.com/p/playing...](http://code.google.com/p/playing-music-with-atmega328-pwm/downloads/detail?name=C%20Brasileirinho.rar&can=2&q=)

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
