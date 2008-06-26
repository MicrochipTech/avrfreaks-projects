#!/bin/sh

make clean
make
make program
./checksize main.elf
