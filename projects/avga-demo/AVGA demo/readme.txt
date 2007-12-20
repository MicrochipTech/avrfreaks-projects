=================================================================================
# AVGA demo
# Author: Jerry981 (Jaromir Dvorak, mailto: md@unicode.cz)
=================================================================================

This demo runs on ATMEGA168 @ 19.6608MHz and is configured for PAL standard with
final resolution 192*144. Almost 3MCycles per second are available for user code 
when showing full-screen graphics. Simple 9-resistor DAC is used to convert the 
bitstream to R, G and B signal with classic 16color VGA pallette. See the
schematics. Theese signals are connected to SCART connector. Alternatively, a RGB 
to composite video converter IC can be used.


