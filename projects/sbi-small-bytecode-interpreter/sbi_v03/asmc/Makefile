sasmc.exe: sasmc.o tokenizer.o
	g++ -o sasmc.exe sasmc.o tokenizer.o
sasmc.o: sasmc.cpp tokenizer.h
	g++ -c -o sasmc.o sasmc.cpp
tokenizer.o: tokenizer.cpp tokenizer.h
	g++ -c -o tokenizer.o tokenizer.cpp
clean:
	rm -f sasmc.exe sasmc.o tokenizer.o