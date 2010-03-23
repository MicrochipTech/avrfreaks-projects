#############################################################################
# This makefile builds the tinyserial library and all of its test programs.
#############################################################################

ECHO= echo

all : build

build :
	@$(ECHO) DID YOU REMEMBER TO MODIFY CONFIGURATION.MK TO 
	@$(ECHO) REFLECT YOUR LOCAL CONFIGURATION AND PUT THE AVR-GCC
	@$(ECHO) BINARIES IN YOUR PATH ENVIRONMENT VARIABLE?
	@$(ECHO)
	@$(ECHO) BUILDING THE TINYSERIAL LIBRARY...
	$(MAKE) -C Lib
	@$(ECHO) BUILDING THE TEST PROGRAMS...
	$(MAKE) -C Test/Getc
	$(MAKE) -C Test/Gets
	$(MAKE) -C Test/Putc
	$(MAKE) -C Test/Puts
	$(MAKE) -C Test/Putsf
	@$(ECHO) DONE.
	@$(ECHO) LIBRARY IS IN Lib/libtinyserial.a

clean :
	@$(ECHO) CLEANING THE TINYSERIAL LIBRARY...
	$(MAKE) clean -C Lib
	@$(ECHO) CLEANING THE TEST PROGRAMS...
	$(MAKE) clean -C Test/Getc
	$(MAKE) clean -C Test/Gets
	$(MAKE) clean -C Test/Putc
	$(MAKE) clean -C Test/Puts
	$(MAKE) clean -C Test/Putsf
	@$(ECHO) DONE.
