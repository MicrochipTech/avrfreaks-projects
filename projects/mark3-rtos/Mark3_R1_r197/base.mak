#----------------------------------------------------------------------------
# Mark3 Base makefile - defines the common variables for all makefiles
# Copyright (c) 20112012 Funkenstein Software Consulting
#----------------------------------------------------------------------------

# These are target specific, modify to suit your platform and environment
ifeq ($(STAGE), )
    ROOT_DIR=$(shell pwd)/
    STAGE=$(ROOT_DIR)stage/
endif

export STAGE
export ROOT_DIR

ifeq ($(ARCH), )
  ARCH=avr
  VARIANT=atmega328p
  TOOLCHAIN=gcc
endif

include $(ROOT_DIR)build/$(ARCH)/$(VARIANT)/$(TOOLCHAIN)/platform.mak

COPYCMD=cp -p -f 
RMCMD=rm -f

DIRLIST=ls -d */
MAKELIST=ls -d */makefile

LIB_DIR=$(STAGE)lib/
DRV_DIR=$(STAGE)drv/
APP_DIR=$(STAGE)app/
INC_DIR=$(STAGE)inc/
SRC_DIR=$(STAGE)src/
SA_DIR=$(STAGE)sa/

# The rest of these are internal to the build system, do not modify!
CFLAGS+=-I$(INC_DIR)
CPPFLAGS+=-I$(INC_DIR)
LFLAGS+=-I$(INC_DIR)

PUBLIC_DIR=./public/
CFLAGS+=-I$(PUBLIC_DIR)
CFLAGS+=-I$(shell pwd)/

CPPFLAGS+=-I$(PUBLIC_DIR)
CPPFLAGS+=-I$(shell pwd)/

CLANGFLAGS+=-I$(INC_DIR)
CLANGFLAGS+=-I$(PUBLIC_DIR)
CLANGFLAGS+=-I$(shell pwd)/

OBJ_DIR=./obj/

IS_LIB=0
IS_DRV=0
IS_APP=0
LIBNAME=
DRVNAME=
APPNAME=

BASE=$(shell pwd)

SUBDIRS=$(shell $(DIRLIST))
SUBMAKE=$(shell $(MAKELIST))

