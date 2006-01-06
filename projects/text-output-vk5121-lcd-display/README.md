# Text output to a VK5121 LCD display

Uploaded by barnacle on 2006-01-06 10:44:00 (rating 0 out of 5)

## Summary

Does what it says on the label, basically...


The VK5121 graphics display keeps popping up on eBay and end-of-line places. Basically it's horrible, but I foolishly designed it into a short-run hardware widget and needed to drive both text and graphical elements.


The graphical parts are highly specific to me and not worth discussing, but this demonstrates a way of using the display as a 4 line 20 character output.


Note that the routine 'lcd\_print(char)' does not scroll the display and nor does it understand cr/lf/tab etc; it simply prints from top right to bottom left and then starts again.


Cheers,


Neil

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
