SST_DIR			= sst
EXAMPLES_DIR	= examples
SST_SOURCES		= sst.c sst_timer.c sst_mutex.c
RB_SOURCES		= ring_buf.c
MAIN_TEST		= main_test
MUTEX_TEST		= mutex_test

OPTIMIZE 	= 	-Os
WARN_LEVEL	= 	-pedantic -Wall -Wpointer-arith -Wstrict-prototypes \
				-Winline -Wundef -Wshadow -Werror
C_STD		=	-std=gnu99

export EXAMPLES_DIR SST_DIR SST_SOURCES \
	RB_SOURCES MAIN_TEST MUTEX_TEST OPTIMIZE WARN_LEVEL C_STD

SUBDIRS = AVR LPC PC

.PHONY: subdirs $(SUBDIRS) all avr avr_prg_main avr_prg_mutex lpc clean

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@
             
all: subdirs

avr:
	$(MAKE) -C AVR all

avr_prg_main:
	$(MAKE) -C AVR main_prog

avr_prg_mutex:
	$(MAKE) -C AVR mutex_prog

lpc:
	$(MAKE) -C LPC all

pc:
	$(MAKE) -C PC all

doc: dox/sst.dox dox/main.dox
	@doxygen dox/sst.dox

clean:
	@-rm -f *~ sst/*~
	$(MAKE) -C AVR clean
	$(MAKE) -C LPC clean
	$(MAKE) -C PC clean

