all: counter.sbi pow.sbi blink.sbi jumps.sbi fastblink.sbi progressivedelay.sbi interrupts.sbi
sasmc.exe:
	cp ../../../sasmc/sasmc.exe sasmc.exe
counter.sbi: sasmc.exe counter.sasm
	sasmc -i counter.sasm -o counter.sbi -s -cl
pow.sbi: sasmc.exe pow.sasm
	sasmc -i pow.sasm -o pow.sbi -s -cl
blink.sbi: sasmc.exe blink.sasm
	sasmc -i blink.sasm -o blink.sbi -s -cl
jumps.sbi: sasmc.exe jumps.sasm
	sasmc -i jumps.sasm -o jumps.sbi -s -cl
fastblink.sbi: sasmc.exe fastblink.sasm
	sasmc -i fastblink.sasm -o fastblink.sbi -s -cl
progressivedelay.sbi:  sasmc.exe fastblink.sasm progressivedelay.sasm
	sasmc -i progressivedelay.sasm -o progressivedelay.sbi -s -cl
interrupts.sbi:  sasmc.exe interrupts.sasm interrupts.sasm
	sasmc -i interrupts.sasm -o interrupts.sbi -s -cl
clean:
	rm -f sasmc.exe blink.sbi counter.sbi pow.sbi libtest.sbi jumps.sbi fastblink.sbi progressivedelay.sbi interrupts.sbi
