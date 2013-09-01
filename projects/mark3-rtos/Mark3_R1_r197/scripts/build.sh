#!/bin/bash

# Shell script to build a clean Mark3 kernel, all demo and test apps, with
# full Doxygen documentation.
if [ "${ROOT_DIR}" == "" ]; then
    ROOT_DIR=$(pwd)
fi
echo "ROOT=${ROOT_DIR}"

export ROOT_DIR=${ROOT_DIR}

# Clean out development stage
rm ./stage/inc/*
rm ./stage/app/*
rm ./stage/lib/*
rm ./stage/drv/*
rm ./stage/src/*
rm ./stage/sa/*

# Purge object files for all components
make clean -j 4

# Build working set of headers, libraries, and applications (will not stop on errors!)
make headers -j 4
make library -j 4
make binary -j 4



