# Change this to reflect the location of the Linux kernel sources
KDIR := $(AVR32_LINUX_SOURCES)

ARCH ?= avr32
CROSS_COMPILE ?= avr32-linux-
export ARCH CROSS_COMPILE

KBUILD_TARGETS := clean help modules modules_install

.PHONY: all $(KBUILD_TARGETS)
all: modules

$(KBUILD_TARGETS):
	$(MAKE) -C $(KDIR) M=$(shell pwd) $
	@dest_dir='/home/marjan/workspace/avr32rootfs/lib/modules/2.6.27.6.atmel.1/kernel';\
    if [ -d $$dest_dir ]; then\
    	cp -f ew24b00yly.ko $$dest_dir;\
    else\
    	echo "Directory $$dest_dir does not exist!";\
    fi
