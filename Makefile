all: main

clean:
	@rm -f main

main: main.cc
	g++ -std=c++11 main.cc -lpthread -o main

run: main
