all: main.o
	g++ -g -o main main.o

main.o: main.cpp
	g++ -c --std=c++20 main.cpp

clean:
	rm main.o main