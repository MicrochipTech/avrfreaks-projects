# makefile, written by guido socher
all: ethcom
#-------------------
help: 
	@echo "Usage: make all"
	@echo "or"
	@echo "Usage: make clean"
#-------------------
ethcom: ethcom.cpp
	g++ -Wall -o ethcom ethcom.cpp
	strip ethcom
#-------------------
clean:
	rm -f *.o ethcom
#-------------------
