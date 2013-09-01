# Platform-specific options

CC=gcc
CPP=g++

CFLAGS=-g2 -DK_ADDR=uint64_t -pthread -c
CPPFLAGS=-g2 -DK_ADDR=uint64_t -pthread -c

LINK=g++
LFLAGS=-lrt -pthread

AR=ar
ARFLAGS=rcs

OBJCOPY=objcopy
OBJCOPY_FLAGS=-O ihex

CLANG=true
CLANGFLAGS=--analyze -fdiagnostics-show-category=name -Weverything  

