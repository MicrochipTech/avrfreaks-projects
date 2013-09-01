#!/bin/sh

# Build documentation source stage using the kernel and services directories
root_dir=`pwd`
echo "Root_Dir" ${root_dir}
cd ./kernel
make source ROOT_DIR=${root_dir}/
cd ..
cd ./services
make source ROOT_DIR=${root_dir}/
cd ..
cd ./docs
doxygen ./Doxyfile
cd ..
cd ./docs/output/latex/
pdflatex refman
makeindex refman
pdflatex refman
makeindex refman
pdflatex refman
cd ${root_dir}
cp ./docs/output/latex/refman.pdf ./docs
cd ..

