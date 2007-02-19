# my notes are in /home/html/hjh/cc/english/avrstud.htm
# Meine Notizen zu AVR sind in /home/html/hjh/cc/deutsch/avrstud.htm

PRG		= qqtsfloa

# all objects required to link this project
#  
#

OBJ		= qqtsfloa.o \
				_addsf3.o _divsf3.o _eqsf2.o \
				_floatsi.o _isnan.o _mulsf3.o \
				asctofl.o asctohx2.o fmulpow2.o \
				getticks.o \
				tshar.o tsser.o \
				tssfldec.o tssflout.o \
				tssgetln.o tsshexo.o \
				tssuchex.o tssulout.o tssuwout.o 

SPARE = \


# Amount of parallel timesharing tasks
NPRGPATH = 2
				
include ../../../avrbat/makedefs

qqtsfloa.o : ../../qqtsfloa.cpp
	@echo compiling special input=$< output=$@
	$(CC) -g -c $(HJH_IDIR) $(CFLAGS) $< -o $@
	$(CC) -g -S -c $(HJH_IDIR) $(CFLAGS) $< -o $@.tmp


_addsf3.o : ../../../lq/_addsf3.s
	@echo compiling special input=$< output=$@
	$(CC) -g -c $(HJH_IDIR) $(ASFLAGS) $< -o $@

_divsf3.o : ../../../lq/_divsf3.s
	@echo compiling special input=$< output=$@
	$(CC) -g -c $(HJH_IDIR) $(ASFLAGS) $< -o $@

_eqsf2.o : ../../../lq/_eqsf2.s
	@echo compiling special input=$< output=$@
	$(CC) -g -c $(HJH_IDIR) $(ASFLAGS) $< -o $@

_floatsi.o : ../../../lq/_floatsi.s
	@echo compiling special input=$< output=$@
	$(CC) -g -c $(HJH_IDIR) $(ASFLAGS) $< -o $@

_isnan.o : ../../../lq/_isnan.s
	@echo compiling special input=$< output=$@
	$(CC) -g -c $(HJH_IDIR) $(ASFLAGS) $< -o $@

_mulsf3.o : ../../../lq/_mulsf3.s
	@echo compiling special input=$< output=$@
	$(CC) -g -c $(HJH_IDIR) $(ASFLAGS) $< -o $@

asctofl.o : ../../../lq/asctofl.cpp
	@echo compiling special input=$< output=$@
	$(CC) -g -c $(HJH_IDIR) $(CFLAGS) $< -o $@

asctohx2.o : ../../../lq/asctohx2.cpp
	@echo compiling special input=$< output=$@
	$(CC) -g -c $(HJH_IDIR) $(CFLAGS) $< -o $@

fmulpow2.o : ../../../lq/fmulpow2.s
	@echo compiling special input=$< output=$@
	$(CC) -g -c $(HJH_IDIR) $(ASFLAGS) $< -o $@

getticks.o : ../../../lq/getticks.cpp
	@echo compiling special input=$< output=$@
	$(CC) -g -c $(HJH_IDIR) $(CFLAGS) $< -o $@

tshar.o : ../../../lq/tshar.cpp
	@echo compiling special input=$< output=$@
	$(CC) -g -c $(HJH_IDIR) $(CFLAGS) $< -o $@

tsser.o : ../../../lq/tsser.cpp
	@echo compiling special input=$< output=$@
	$(CC) -g -c $(HJH_IDIR) $(CFLAGS) $< -o $@

tssfldec.o : ../../../lq/tssfldec.cpp
	@echo compiling special input=$< output=$@
	$(CC) -g -c $(HJH_IDIR) $(CFLAGS) $< -o $@

tssgetln.o : ../../../lq/tssgetln.cpp
	@echo compiling special input=$< output=$@
	$(CC) -g -c $(HJH_IDIR) $(CFLAGS) $< -o $@

tsshexo.o : ../../../lq/tsshexo.cpp
	@echo compiling special input=$< output=$@
	$(CC) -g -c $(HJH_IDIR) $(CFLAGS) $< -o $@

tssflout.o : ../../../lq/tssflout.cpp
	@echo compiling special input=$< output=$@
	$(CC) -g -c $(HJH_IDIR) $(CFLAGS) $< -o $@
	$(CC) -g -S -c $(HJH_IDIR) $(CFLAGS) $< -o $@.tmp

tssuchex.o : ../../../lq/tssuchex.cpp
	@echo compiling special input=$< output=$@
	$(CC) -g -c $(HJH_IDIR) $(CFLAGS) $< -o $@

tssulout.o : ../../../lq/tssulout.cpp
	@echo compiling special input=$< output=$@
	$(CC) -g -c $(HJH_IDIR) $(CFLAGS) $< -o $@

tssuwout.o : ../../../lq/tssuwout.cpp
	@echo compiling special input=$< output=$@
	$(CC) -g -c $(HJH_IDIR) $(CFLAGS) $< -o $@
