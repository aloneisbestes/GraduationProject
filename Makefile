# Makefile
CC := g++

objs = ErrorMessage.o HttpRequest.o

all: test main

main : main.o ErrorMessage.o HttpRequest.o 
	$(CC) -o main main.o $(objs)

test : test.o $(objs)
	$(CC) -o test test.o $(objs)

ErrorMessage.o : ./src/ErrorMessage.cpp
	$(CC) -I./include -c ./src/ErrorMessage.cpp -o ErrorMessage.o 

HttpRequest.o : ./src/HttpRequest.cpp
	$(CC) -I./include -c ./src/HttpRequest.cpp -o HttpRequest.o 

test.o : ./src/test.cpp
	$(CC) -I./include -c ./src/test.cpp -o test.o 

main.o : ./src/main.cpp
	$(CC) -I./include -c ./src/main.cpp -o main.o 

.PHONY: clean
clean:
	rm -rf main main.o test test.o $(objs)