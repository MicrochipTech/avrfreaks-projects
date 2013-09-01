# Include common prelude make file
include $(ROOT_DIR)base.mak

# If we're building a library, set IS_LIB and LIBNAME
# If we're building a driver, set IS_DRV and DRVNAME
# If we're building an app, set IS_APP and APPNAME
IS_LIB=1
LIBNAME=mark3

#this is the list of the objects required to build the kernel
CPP_SOURCE= \
	blocking.cpp \
	driver.cpp \
    eventflag.cpp \
	ll.cpp \
	message.cpp \
	mutex.cpp \
	profile.cpp \
	quantum.cpp \
	scheduler.cpp \
	ksemaphore.cpp \
	thread.cpp \
	threadlist.cpp \
	kernel.cpp \
	timerlist.cpp \
	tracebuffer.cpp \
    writebuf16.cpp \
    kernel_aware.cpp

# These files are built from source files in their respective 
# subdirectories under ./cpu/arch/variant/toolchain
PORT_CPP_SOURCE= \
	threadport.cpp \
	kernelswi.cpp \
	kerneltimer.cpp \
	kprofile.cpp


# Include the rest of the script that is actually used for building the 
# outputs
include $(ROOT_DIR)build.mak
