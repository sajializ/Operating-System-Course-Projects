all : fileDir store city providence command main

fileDir:
	@mkdir -p bin
	@mkdir -p bin/pipes

store : code/store.cpp code/utils-inl.h 
	@echo "Making store executable file..."
	g++ -std=c++11 code/store.cpp -o bin/store.o

city : code/city.cpp code/utils-inl.h 
	@echo "Making city executable file..."
	g++ -std=c++11 code/city.cpp -o bin/city.o

providence : code/providence.cpp code/utils-inl.h 
	@echo "Making providence executable file..."
	g++ -std=c++11 code/providence.cpp -o bin/providence.o

command : code/command.cpp code/utils-inl.h 
	@echo "Making command executable file..."
	g++ -std=c++11 code/command.cpp -o bin/command.o

main : code/main.cpp code/utils-inl.h 
	@echo "Making Main executable file..."
	g++ -std=c++11 code/main.cpp -o StoreCalculator.out

clean : 
	rm -f -r bin
	rm -f -r StoreCalculator.out


