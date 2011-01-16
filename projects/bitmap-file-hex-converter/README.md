# Bitmap file to hex converter

Uploaded by Kimmeridgien on 2011-01-15 17:17:00 (rating 0 out of 5)

## Summary

This is an MS Excel 2003 tool for generating hex data for the Nokia 3310/5110 displays from an 84*48 pixel monochrome (black/white) bitmap file or 3410 display from a 96*65 bitmap. The 3410 download is a contribution from Joey (LiquidSmoke) as included in the v4.0 download in one of my other projects.


The application lets the user navigate to and select a bitmap file, which is then automatically imported and converted into an MS Excel sheet. The sheet can also be edited manually, so you can start from scratch in Excel. The sheet has a set of formulas that generate the hex data statements in BASCOM-AVR format. These data lines should simply be copy/pasted into your project.


There are rudimentary instructions in menu item "BMPMacro" under "Info/about". Please read them, especially the section that instructs you how to install the Analysis toolpak. You must have security setting medium (or lower (not advisable...)) in order to be able to run macros. The code is visible and editable in the VB editor.


If you are running on a slow computer or have too little RAM, you may want to add the following to the beginning of sub MainProgram:  

Application.ScreenUpdating = False  

At the end of sub MainProgram:  

Application.ScreenUpdating = True


Please notice that this app is written for the Nokia 3310 84*48 format in BASCOM-AVR grammar. It should be very simple to modify for other displays or compilers. Make sure that the bitmap file is indeed monochrome and exactly 84*48 pixels, or the app might misbehave.


The VBA code has a comment section with brief information about the bitmap format. You need to read and understand this info to be able to modify the VB code for other displays or resolutions. For more information about the bitmap format or VBA programming, please google or consult with a friend.


I have now added support for bitmap image printing to my other project "Nokia 3310 display library" using this data. This means that this bmp-to-hex converter is now known to function properly in the first published version.


The reason why I choose to publish this MS Excel app as a separate project is because I hope to add support for other displays later on. Please send me feedback (or even a modified app for your favourite display) and I will improve/add to the project.


Enjoy!


/Niclas aka Kimmeridgien

## Compilers

- BASCOM-AVR

## Tags

- Part-specific Lib.functions
- BASCOM-AVR
