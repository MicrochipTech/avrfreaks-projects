# FunkOS

Uploaded by MoSlevin on 2010-03-20 17:52:00 (rating 0 out of 5)

## Summary

FunkOS is a Real-Time Operating Systems (RTOS) for small-to-medium scale embedded systems. It is designed to be lightweight, powerful, and flexible, allowing the programmer to easily customize the kernel for a specific platform or application. 


Features include:  

- fully preemptive scheduler  

- 255 priority levels  

- unlimited tasks per priority level  

- round-robin scheduling per priority group  

- per-task time quantum for round-robin scheduling  

- task synchronization objects (Semaphores)  

- resource protection objects (Mutex)  

- periodic event manager  

- two types of interprocess communications  

- fixed-block heap implementation  

- driver abstraction interface  

- task-monitoring watchdog timer interface  

- software real-time clock library  

- several tutorial applications  

- Several alternate kernel options  

 - Cooperative mode (single-threaded) kernel  

 - Minimal preemptive round-robin kernel (pipsqueak kernel)  

 - C++ Kernel  

- Extensible GUI framework with sample widget library  

- FAT16/32 filesystem driver(read-only)  

- New AVR ports for iccv7, IAR, and codevision (thanks Ahmad Shawqy!)  

- Buffered AVR UART driver  

- Updated documentation and various bugfixes  

- simple API  

- full PDF manual  

- doxygen documentation  

- highly portable, written in C with minimal device-specific code


FunkOS is also quite small. The smallest useful demo app (1 application + idle task) compiles to < 1.6KB Flash / ~400 bytes of RAM on an ATMega644.


Visit the project homepage on SourceForge at: <http://funkos.sourceforge.net>

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
