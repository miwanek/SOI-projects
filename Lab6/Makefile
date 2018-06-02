CC = g++


all:
	make release

release: main virtual node 
	$(CC)  main.o vfs.o node.o -o fsystem
virtual:
	$(CC) -c vfs.cpp
main:	
	$(CC) -c  main.cpp
node:
	$(CC) -c node.cpp

clean:
	-rm *o  vfs
