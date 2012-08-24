# AVR Sonar

Uploaded by ksvitale on 2012-08-24 02:33:00 (rating 0 out of 5)

## Summary

AVR Sonar - V1.0  

-----------------  

2012-0822: After many years of fussing with the file upload problem and my many requests for investigation being ignored, I tried renaming the ZIP file to PNG and the source code ZIP file uploaded without error. Odd thing is, it shows in the file list with a "ZIP" icon. Be sure to remove the PNG extension when saving the file so your archiving program will recognize it.


2011-0128: Okay, this is really aggravating. The file posting two days ago was the wrong file. I deleted it and attempted to upload the correct file. No joy! Still no files! Others seem to have no problems with uploading their files but not I. I can't get any responses from "support" folks here at AVRFREAKS.NET. Sorry again folks. 


2011-0126: Hmm... Project files have been successfully uploaded. Seems that once you upload a file, have to go back to your project -> edit page and add the file from the list. I don't recall it ever being that way in the past but never-the-less, there they are!


THE STORY:  

Ten years ago, My family and I moved into a house with a two-car garage. I was now able to park my vehicle in a garage for the night but figuring out if the vehicle had cleared the garage door or not was always an issue. I thought about the tennis ball on a string trick but thought "how hick". LOL I remembered that I had acquired some Polaroid 6500 sonar board samples a few years earlier and thought "how glorious!". So I set out on this project about five years ago using a PIC-STAMP. I never was able to get the STAMP to work properly (at all) with the Polaroid 6500 board so I ventured into the AVR2313. Life was busier then and I never had a chance to get any further on the project other than to place a socket, crystal and ISP header on the perf-board and mount the SONAR board in a case.


Recently, I pulled the project from the shelf and dusted it off. I plugged in the sonar board to my pulse generator and o-scope and voila! It worked! My original intent was to have a large red and green LED for "go/no go" indication but quickly realized that would require specific calibration for ONE particular vehicle so I opted for two multiplexed 7-segment displays to give me a readout of distance. All this would fit quaintly on an AVR2313 device. After some more evaluation, I came to the conclusion that I needed to have large (and bright) enough displays to be seen from a distance of say 6 feet and in subdued sunlight. My solution was a "gutted" LED clock module from the 80's. After some research, I found that in those days, clock chips (namely NATIONAL SEMI) used "Duplex Displays". Well, heck! I really had no idea what a "duplex display" was. I was planning to MULTIPLEX my displays! Well, I found out what a "duplex display" was and more importantly, how to interface to one. Not difficult in hardware but the character lookup table combinations proved to be mistake prone (and ridden). Since the display module would no longer fit into my project box, I chose an "outrigger" display approach, housing the display in its own box, which is attached to the top of the sonar box. For ease of use, I found two 74LS164 serial-in/parallel-out shift registers. Handy little buggers! This project probably could have been implemented on a lower pin-count device but it requires both an 8-bit and 16-bit timer with input capture on the 16-bit timer. The Program occupies a little over 1K of code space, a few bytes of RAM and EEPROM.


It took a week to work on but what follows is the result of the project.


I have included a more detailed description of hardware and software in the project files.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
