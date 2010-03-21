CC = iccavr
LIB = ilibw
CFLAGS =  -e -D__ICC_VERSION=722 -D__BUILD=50 -DARCH_AVR -D_EE_EXTIO -DATMega328P  -l -g -MLongJump -MHasMul -MEnhanced 
ASFLAGS = $(CFLAGS) 
LFLAGS =  -g -nb:50 -e:0x8000 -ucrtatmega.o -bfunc_lit:0xc8.0x8000 -dram_end:0x8ff -bdata:0x100.0x8ff -dhwstk_size:20 -beeprom:0.1024 -fihx_coff -S2
FILES = driver.o heap.o kernelswi.o kerneltimer.o kernelwdt.o message.o mutex.o myfirstapp.o semaphore.o task.o taskport.o timer.o watchdog.o 

FUNKOS_ICC:	$(FILES)
	$(CC) -o FUNKOS_ICC $(LFLAGS) @FUNKOS_ICC.lk   -lcatmega
driver.o: .\types.h .\driver.h .\task.h .\kernelcfg.h .\mutex.h .\taskport.h C:\iccv7avr\include\iom328pv.h C:\iccv7avr\include\_iom88to328v.h
driver.o:	driver.c
	$(CC) -c $(CFLAGS) driver.c
heap.o: .\types.h .\taskport.h .\task.h .\kernelcfg.h C:\iccv7avr\include\iom328pv.h C:\iccv7avr\include\_iom88to328v.h .\heap.h
heap.o:	heap.c
	$(CC) -c $(CFLAGS) heap.c
kernelswi.o: .\types.h .\task.h .\kernelcfg.h .\kernelswi.h C:\iccv7avr\include\iom328pv.h C:\iccv7avr\include\_iom88to328v.h
kernelswi.o:	kernelswi.c
	$(CC) -c $(CFLAGS) kernelswi.c
kerneltimer.o: .\types.h .\kernelcfg.h C:\iccv7avr\include\iom328pv.h C:\iccv7avr\include\_iom88to328v.h
kerneltimer.o:	kerneltimer.c
	$(CC) -c $(CFLAGS) kerneltimer.c
kernelwdt.o: .\types.h .\kernelcfg.h .\taskport.h .\task.h C:\iccv7avr\include\iom328pv.h C:\iccv7avr\include\_iom88to328v.h
kernelwdt.o:	kernelwdt.c
	$(CC) -c $(CFLAGS) kernelwdt.c
message.o: .\taskport.h .\task.h .\types.h .\kernelcfg.h C:\iccv7avr\include\iom328pv.h C:\iccv7avr\include\_iom88to328v.h .\semaphore.h .\message.h
message.o:	message.c
	$(CC) -c $(CFLAGS) message.c
mutex.o: .\mutex.h .\types.h .\task.h .\kernelcfg.h .\taskport.h C:\iccv7avr\include\iom328pv.h C:\iccv7avr\include\_iom88to328v.h
mutex.o:	mutex.c
	$(CC) -c $(CFLAGS) mutex.c
myfirstapp.o: .\kernelcfg.h .\kernelswi.h .\types.h .\kerneltimer.h .\task.h .\taskport.h C:\iccv7avr\include\iom328pv.h C:\iccv7avr\include\_iom88to328v.h
myfirstapp.o:	myfirstapp.c
	$(CC) -c $(CFLAGS) myfirstapp.c
semaphore.o: .\semaphore.h .\types.h .\task.h .\kernelcfg.h .\taskport.h C:\iccv7avr\include\iom328pv.h C:\iccv7avr\include\_iom88to328v.h
semaphore.o:	semaphore.c
	$(CC) -c $(CFLAGS) semaphore.c
task.o: .\types.h .\kernelcfg.h .\task.h .\taskport.h C:\iccv7avr\include\iom328pv.h C:\iccv7avr\include\_iom88to328v.h .\kernelswi.h .\kerneltimer.h
task.o:	task.c
	$(CC) -c $(CFLAGS) task.c
taskport.o: .\types.h .\taskport.h .\task.h .\kernelcfg.h C:\iccv7avr\include\iom328pv.h C:\iccv7avr\include\_iom88to328v.h .\kernelswi.h .\kerneltimer.h .\timer.h
taskport.o:	taskport.c
	$(CC) -c $(CFLAGS) taskport.c
timer.o: .\types.h .\timer.h .\taskport.h .\task.h .\kernelcfg.h C:\iccv7avr\include\iom328pv.h C:\iccv7avr\include\_iom88to328v.h
timer.o:	timer.c
	$(CC) -c $(CFLAGS) timer.c
watchdog.o: .\types.h .\watchdog.h .\kernelwdt.h .\kernelcfg.h
watchdog.o:	watchdog.c
	$(CC) -c $(CFLAGS) watchdog.c
