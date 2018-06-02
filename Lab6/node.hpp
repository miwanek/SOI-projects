#ifndef NODE_H
#define NODE_H

#define BLOCK_SIZE 2048
#define NAME_SIZE 22

#include <iostream>
#include <fstream>

using namespace std;

class node
{
	public:
	bool used;
	bool start;
	char name[NAME_SIZE];
	unsigned int size;
	unsigned int next_node;
	node();
	void clean();
};




#endif
