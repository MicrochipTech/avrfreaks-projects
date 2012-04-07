all: counter.sbi pow.sbi blink.sbi jumps.sbi fastblink.sbi progressivedelay.sbi
sasmc.exe:
	cp ../../../sasmc/sasmc.exe sasmc.exe
counter.sbi: sasmc.exe counter.sasm
	./sasmc.exe counter.sasm counter.sbi
pow.sbi: sasmc.exe pow.sasm
	./sasmc.exe pow.sasm pow.sbi
blink.sbi: sasmc.exe blink.sasm
	./sasmc.exe blink.sasm blink.sbi
jumps.sbi: sasmc.exe jumps.sasm
	./sasmc.exe jumps.sasm jumps.sbi
fastblink.sbi: sasmc.exe fastblink.sasm
	./sasmc.exe fastblink.sasm fastblink.sbi
progressivedelay.sbi:  sasmc.exe fastblink.sasm progressivedelay.sasm
	./sasmc.exe progressivedelay.sasm progressivedelay.sbi
clean:
	rm -f sasmc.exe counter.sbi pow.sbi libtest.sbi jumps.sbi fastblink.sbi progressivedelay.sbi
