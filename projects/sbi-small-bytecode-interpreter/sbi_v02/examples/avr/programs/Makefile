all: counter.sbi pow.sbi blink.sbi jumps.sbi fastblink.sbi progressivedelay.sbi
sasmc.exe:
	cp ../../../sasmc/sasmc.exe sasmc.exe
counter.sbi: sasmc.exe counter.sasm
	sasmc -i counter.sasm -o counter.sbi -s
pow.sbi: sasmc.exe pow.sasm
	sasmc -i pow.sasm -o pow.sbi -s
blink.sbi: sasmc.exe blink.sasm
	sasmc -i blink.sasm -o blink.sbi -s
jumps.sbi: sasmc.exe jumps.sasm
	sasmc -i jumps.sasm -o jumps.sbi -s
fastblink.sbi: sasmc.exe fastblink.sasm
	sasmc -i fastblink.sasm -o fastblink.sbi -s
progressivedelay.sbi:  sasmc.exe fastblink.sasm progressivedelay.sasm
	sasmc -i progressivedelay.sasm -o progressivedelay.sbi -s
clean:
	rm -f sasmc.exe counter.sbi pow.sbi libtest.sbi jumps.sbi fastblink.sbi progressivedelay.sbi
