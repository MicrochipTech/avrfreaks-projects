# AVRStudio4 plugin gdb-target

Uploaded by PRosendahl on 2010-10-20 18:11:00 (rating 0 out of 5)

## Summary

This is a plugin for gdb-targets for AVRStudio4. It works as follows:  

- AVRStudio4: open project (eg an ATTiny25 project)  

- AVRStudio4: Debug / Select Platform and Device : myCompany.myPlugin / ATTiny25  

- simavr: ./run\_avr -g -m attiny25 -f 8000000  

- AVRStudio4: compile/assemble the project  

- AVRStudio4: run the project  

- AVRStudio4: enter ip and port of the gdb-server (eg localhost:1234)  

- AVRStudio4: debug the project...


AVRStudio4 will connect to the simavr-gdb-server via this gdb-target-plugin.


Following commands work:  

- run  

- break  

- stop  

- reset  

- set/remove breakpoint  

- single step (everything else is mapped to single step)


You need to download the bin-package and execute the AVRSTUDIO\_REGISTER.CMD to register the plugin with AVRStudio4. Before deleting the files, you should UNREGISTER.CMD the files.


It comes with the sources.  

Thread about simavr is here:  

[https://www.avrfreaks.net/index.p...](https://www.avrfreaks.net/index.php?module=PNphpBB2&file=viewtopic&t=86665)  

simavr git repo is here:  

<http://gitorious.org/simavr>


Discussion is here:  

[https://www.avrfreaks.net/index.p...](https://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&p=756341#756341)


2010.10.20: initial release v1

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
