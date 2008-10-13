TARGET=vcdTrans

#CFLAGS = -O2 -Wall -Wa,-adhlns=$(<:%.c=%.lst) -gstabs

CFLAGS = -O2 -Wall
OBJS   = $(TARGET).o
CLIBS  = 

ifeq ($(OSTYPE),cygwin)
    CLIBS  = -lioperm
    EXE = $(TARGET).exe
else ifeq ($(OSTYPE),linux)
    EXE = $(TARGET)
else
    $(error "System 'OSTYPE' is unset: Please set to either 'linux' or 'cygwin'")
endif

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(CLIBS)
	cp $(EXE) ../../bin

	
$(TARGET).o: $(TARGET).c

.PHONY: clean install test test_v24 test_spi test_i2c

clean:
	rm -f $(OBJS) $(EXE) $(TARGET).lst
install: $(EXE)
	install -m+x+r $(EXE) /usr/local/bin


test_i2c: $(EXE)
	./$(EXE) --ifile=sample_i2c.vcd --type=i2c
test_spi: $(EXE)
	./$(EXE) --ifile=sample_spi.vcd --type=spi
test_v24: $(EXE)
	./$(EXE) --ifile=sample_v24.vcd --type=v24 --rx=RX --tx=TX

test: test_i2c test_spi test_v24 
