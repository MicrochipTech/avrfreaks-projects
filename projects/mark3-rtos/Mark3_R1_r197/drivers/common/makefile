# Include common prelude make file
include $(ROOT_DIR)base.mak

# If we're building a library, set IS_LIB and LIBNAME
# If we're building a driver, set IS_DRV and DRVNAME
# If we're building an app, set IS_APP and APPNAME
# if this is just a recursive node, leave it empty.

# Include the rest of the script that is actually used for building the 
# outputs
include $(ROOT_DIR)build.mak
