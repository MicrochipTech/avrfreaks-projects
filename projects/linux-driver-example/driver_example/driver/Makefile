MODULE_NAME = spitest
$(MODULE_NAME)-objs = driverentry.o

obj-m := $(MODULE_NAME).o

EXTRA_CFLAGS += -Winline

PWD := $(shell pwd)

default:
	make ARCH=avr32 CROSS_COMPILE=$(PWD)/../../../buildroot/build_avr32_nofpu/staging_dir/bin/avr32-linux- -C ../../../buildroot/project_build_avr32_nofpu/atngw100/linux-2.6.23 M=`pwd`

clean:
	${RM} driverentry.o \
	spitest.ko \
  spitest.o \
	spitest.mod.o \
	spitest.mod.c \
	Module.symvers
