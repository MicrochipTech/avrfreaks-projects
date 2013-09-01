#!/bin/bash

#----------------------------------------------------------------------------
# Mark3 RTOS - copyright 2012, Funkenstein Software Consulting
# All rights reserved.
#----------------------------------------------------------------------------
# Script for generating/regenerating sets of bitmapped fonts 
# from TTF files.  The output of this script are .cpp/.h files,
# along with Mark3 makefiles which allow the fonts to be built
# as generic libraries, which can then be built into projects using
# the font/graphics framework.
#----------------------------------------------------------------------------

# Generate some basic fonts
./FontRenderConsole.exe arial_10 Arial 10 
./FontRenderConsole.exe arial_12 Arial 12

./FontRenderConsole.exe courier_10 "Courier New" 10 
./FontRenderConsole.exe courier_12 "Courier New" 12

./FontRenderConsole.exe tahoma_10 Tahoma 10 
./FontRenderConsole.exe tahoma_12 Tahoma 12

dirs_to_create=`ls -1 *.h | sed "s/\(.*\)\..*/\1/"`
for my_dir in ${dirs_to_create}; do
	# Make directories if necessary
	if [ -d ${my_dir} ]; then
		rm ./${my_dir}/*.cpp
		rm ./${my_dir}/public/*.h
	else
		mkdir ${my_dir}
		mkdir ${my_dir}/public
	fi
	
	# Move the source to the subdirectory
	mv ./${my_dir}.h ./${my_dir}/public
	mv ./${my_dir}.cpp ./${my_dir}
	
	# build the makefile for this library
	echo "include \$(ROOT_DIR)base.mak" > ./${my_dir}/makefile
	echo "IS_LIB=1" >> ./${my_dir}/makefile
	echo "LIBNAME="${my_dir} >> ./${my_dir}/makefile
	echo "CPP_SOURCE=${my_dir}.cpp" >> ./${my_dir}/makefile
	echo "include \$(ROOT_DIR)build.mak" >> ./${my_dir}/makefile
	
done