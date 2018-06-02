
#define BLOCK_SIZE 2048
#include "vfs.hpp"
#include <iostream>
#include <fstream>
#include "node.hpp"

using namespace std;


node::node()
{
	used = false;
	start = false;
	size = 0;
	next_node = 0;
}
void node::clean()
{
	used = false;
	start = false;
	size = 0;
	next_node = 0;
}


