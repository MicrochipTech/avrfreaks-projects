BUILDCOMMAND = ../../../buildroot/toolchain_build_avr32_nofpu/gcc-4.2.1-final/gcc/gcc-cross
SOURCES = \
	spitest.c
TARGET = spitest

_spitest:
	echo "Building $(TARGET)..."
	
	$(BUILDCOMMAND) $(SOURCES) -o $(TARGET)

clean:
	${RM} getparams
