# Include common prelude make file
include $(ROOT_DIR)base.mak

# If we're building a library, set IS_LIB and LIBNAME
# If we're building a driver, set IS_DRV and DRVNAME
# If we're building an app, set IS_APP and APPNAME
IS_LIB=1
LIBNAME=random

#this is the list of the objects required to build the kernel
CPP_SOURCE=rand_lfsr.cpp

# Include the rest of the script that is actually used for building the 
# outputs
include $(ROOT_DIR)build.mak
