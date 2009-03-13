# Yamaha TX81Z MIDI programmer

Uploaded by Simonetta on 2009-03-12 18:28:00 (rating 0 out of 5)

## Summary

The Yamaha TX81Z MIDI tone module is the world's most popular FM synthesizer module. It has a wide range of programmable parameters to change its internal sounds. But it is very awkward to program through the buttons on the front panel. There used to be many editor programs available for the Atari ST, Mac and PC, but they are now all obsolete of require ancient hardware interfaces.


 The Yamaha TX81Z is always available on eBay in the $40~$70 range. It is the last remaining inexpensive real synthesizer available. All the other modules play recorded samples of actual instrument waveforms. Sweet sounding, but boring. The Yamaha FM synths make both New Age ambient sounds, near-orchestral instrument sounds, and also the most distorted, grinding, industrial-wasteland noise-tones imaginable. But they are difficult to program without a good editor. The desktop PC editors were useful, but they were not interactive with the synth. Besides, those programs are all gone now, but the synths remain around. Hence, this programmer/editor needed to be created.


 This project uses two potentiometers, a 16x2 LCD, and a Mega8 AVR to edit in real-time the most active TX sound parameters. There is also a PS2 QWERTY keyboard interface that acts like a live 'piano'-style MIDI note controller. One pot selects the parameter, such as Attack Rate or Operator Frequency. The other pot changes the value as the sound is playing. The LCD displays the VoiceName, the parameter, and the new and old values. Operator frequencies are shown as ratios, such as 1.50 or 5.41. Edited voices can be written directly to the TX memory.


 The 2500+ lines of source code are written is AVR assembler and is extensively commented. A GIF file shows the complete schematic.

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
