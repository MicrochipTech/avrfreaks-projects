====================================
FunkOS r2: WinAVR sample project readme:
====================================

	This demo is the basic starting point demonstrating how to set up the RTOS -
	create tasks, initialize the libraries, and start the scheduler.

	This version of MyFirstApp is assembled as a minimal WinAVR project for use
	as a project template, demonstrating how to build FunkOS into an application.  
	This project is also set up to be simulated using the V2 simulator, so you can
	see how the kernel works without needing actual hardware.	
	
	To modify this project for other targets, copy the contents from the appropriate
	port folder (somewhere under "ports\AVR_GCC") into the project directory, select
	the appropriate device from the Project->Options dialog, and modify the Debug 
	platform in the Debug->Select Platform and Device dialog.

	If you want to build a FunkOS WinAVR project from scratch, simply copy the contents
	of the "config", "kernel", and "ports\AVR_GCC\XXX" folder from the source release 
	into an empty folder. Next, create a new project in WinAVR, and add all of the source files
	to the project.
	
	A project should also include a global -DARCH_AVR compiler flag define, or a #define ARCH_AVR
	statement inside "types.h" to ensure that the WORD type compiles to the appropriate size.
	
	Configure the kernel features by modifying kernelcfg.h as necessary.  

	That should be it!


	
