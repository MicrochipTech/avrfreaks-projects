# Distribution Makefile for PhorseZip
# Rebuild the distribution using make -r PhorsePov.zip.

BASE = Makefile Readme.txt
DOCS = Docs/PhorsePOVGuide.pdf Docs/PovCharImages.paint Docs/COPYING.txt
FIRMWARE = Firmware/Makefile Firmware/PhorsePOV.* Firmware/PhorsePOV.bin.*
HARDWARE = Hardware/PhorsePOV_*.stripes Hardware/PostscriptFiles/*.*
TESTS = Tests/PhorsePOVTests.c

# This is all!
PhorsePov.zip : $(BASE) $(DOCS) $(SOURCES) $(FIRMWARE) $(HARDWARE) $(TESTS)
			zip PhorsePov.zip $(BASE) $(DOCS) $(SOURCES) $(FIRMWARE) $(HARDWARE) $(TESTS)
