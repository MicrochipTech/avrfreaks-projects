TARGET=lanaCOM

#CFLAGS = -O2 -Wall -Wa,-adhlns=$(<:%.c=%.lst) -gstabs

CFLAGS = -O2 -Wall
OBJS   = $(TARGET).o com.o tcnt2.o inout.o allowio.o

ifeq ($(OSTYPE),cygwin)
    CLIBS  = -lioperm
    EXE = $(TARGET).exe
else ifeq ($(OSTYPE),linux)
    EXE = $(TARGET)
else
    $(error "System 'OSTYPE' is unset: Please set to either 'linux' or 'cygwin'")
endif


VIEWER = /c/Programme/GTKwave/gtkwave
COMPORT= -a 0x9000

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(CLIBS)
	cp $(EXE) ../../bin

	
$(TARGET).o: $(TARGET).c allowio.h com.h tcnt2.h
com.o: com.c com.h
tcnt2.o: tcnt2.c tcnt2.h
inout.o: inout.c inout.h
allowio.o: allowio.h allowio.h

.PHONY: clean install sample.vcd view
clean:
	rm -f $(OBJS) $(EXE)
install: $(EXE)
	install -m+x+r $(EXE) /usr/local/bin

sample.vcd: $(EXE)
	./$(EXE) $(COMPORT) -verbose=3 -CTS=SDA -RI=SCL -mode=edge -mask=0x5 -edgedata1=0x05 -edgedata2=0x04 -buffsize=2s -output=sample.vcd

view:
	$(VIEWER) sample.vcd &
	