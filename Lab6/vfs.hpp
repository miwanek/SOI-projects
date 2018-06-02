#ifndef VFS_H
#define VFS_H 

#define BLOCK_SIZE 2048

#include <iostream>
#include <fstream>
#include "node.hpp"
#include <cstdlib>
#include <cstdio>
#include <unistd.h>

using namespace std;

class vfs
{
	public:
	vfs( const char* file_name );
	vfs( const char* file_name, size_t file_size);
	void copy_to_vfs( const char * source_file_name );
	void copy_from_vfs( const char * file_to_copy, const char * destination );
	void show_list();
	void dump();
	unsigned int get_free_nodes();
	void save_vfs();
	static void destroy_vfs(char * vfs_name);
	void delete_from_vfs( const char * file_to_delete );
	~vfs();


	private:
	size_t get_block_position(const unsigned int node_number);
	string vfs_name;
	size_t vfs_size;
	size_t nodes_count;
	node* nodes;
};

#endif
