
#define BLOCK_SIZE 2048
#include "vfs.hpp"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include "node.hpp"

using namespace std;


void vfs::destroy_vfs( char* vfs_name)
{
	unlink(vfs_name );
}

vfs::~vfs()
{
	fstream file;
	
	file.open(vfs_name, std::ios::in |std::ios::out | std::fstream::binary );
	if(file.good() == false ) cout << "nie dziala" << endl;
	file.write( reinterpret_cast <char* > ( &(vfs_size) ), sizeof(vfs_size) );
	file.flush();
	for( size_t i = 0; i < nodes_count; i++ )
	{
		file.write( reinterpret_cast <char* > ( &(nodes[i].used) ), sizeof(bool) );
		file.flush();
		file.write( reinterpret_cast <char* > ( &(nodes[i].start) ), sizeof(bool) );
		file.flush();
		file.write( nodes[i].name, 22 * sizeof(char) );
		file.flush();
		file.write( reinterpret_cast <char* > ( &(nodes[i].size) ), sizeof(unsigned int) );
		file.flush();
		file.write( reinterpret_cast <char* > ( &(nodes[i].next_node) ), sizeof(unsigned int) );
		file.flush();
	}
	file.flush();
	file.close();
	file.clear();
	delete[] nodes;
}

vfs::vfs(const char * file_name) // Wczytujemy nasz system plików
{
	fstream file;
	file.open( file_name, std::ios::in | std::ios::out | std::fstream::binary );
	vfs_name = file_name;
	file.read( reinterpret_cast <char* > (&vfs_size), sizeof(vfs_size) );
	nodes_count =  ( vfs_size - sizeof( vfs_size ) ) / (sizeof( node ) + BLOCK_SIZE);
	nodes = new node[nodes_count];
	for( size_t i = 0; i < nodes_count; i++)
	{
		file.read( reinterpret_cast <char* > ( &(nodes[i].used) ), sizeof(bool) );
		file.read( reinterpret_cast <char* > ( &(nodes[i].start) ), sizeof(bool) );
		file.read( nodes[i].name, 22 * sizeof(char) );
		file.read( reinterpret_cast <char* > ( &(nodes[i].size) ), sizeof(unsigned int) );
		file.read( reinterpret_cast <char* > ( &(nodes[i].next_node) ), sizeof(unsigned int) );
	}
	file.close();
	file.clear();
}

vfs::vfs(const char * file_name, size_t file_size) // Tworzymy system plików
{
	fstream file;
	file.open( file_name, std::ios::out | std::ios::binary);
	if(file.good() == false)
	{
		std::cout << "Unable to open file" << endl;
		exit(0);	
	}
	vfs_name = file_name;
	vfs_size = file_size;
	short to_write = 128;
	char space[128];
	size_t remaining_bytes = file_size;
	while( remaining_bytes > 0 )
	{
		if(remaining_bytes < to_write ) to_write = remaining_bytes;
		file.write(space, to_write);
		remaining_bytes -= to_write;
	}
	nodes_count =  ( file_size - sizeof( vfs_size ) ) / (sizeof( node ) + BLOCK_SIZE);
	nodes = new node[nodes_count];
	file.close();
	file.clear();
}

unsigned int vfs::get_free_nodes()
{
	unsigned int free_nodes = 0;
	for( size_t i = 0; i < nodes_count; i++ )
	{
		if( nodes[i].used == false ) ++free_nodes;
	}
	return free_nodes;
}

size_t vfs::get_block_position(const unsigned int node_number)
{
	return sizeof( vfs_size ) + sizeof( node) * nodes_count + BLOCK_SIZE * node_number;
}

void vfs::copy_to_vfs( const char * source_file_name )
{
	fstream file;
	file.open( vfs_name, std::ios::in | std::ios::out | std::ios::binary);
	size_t source_file_length;
	unsigned int required_nodes;
	unsigned int current_inode;
	char data[BLOCK_SIZE];
	
	if(strlen(source_file_name) == 0 || strlen(source_file_name) > 22 )
	{
		std::cout << " Błędna nazwa pliku" ;
		return;
	}
	
	for( size_t i = 0; i < nodes_count; i++ )
	{
		if( strcmp( nodes[i].name, source_file_name ) == 0  && nodes[i].used == true )
		{
			std::cout << "Taki plik już istnieje" << endl;
			return;
		}
	}
	fstream source_file;
	source_file.open( source_file_name, std::ios::in | std::ios::out | std::ios::binary );
	if( source_file.good() == false )
	{
		std::cout << "Nie udało się otworzyć pliku do przeniesienia" << endl;
		return;
	}
	size_t start = source_file.tellg();
	source_file.seekg(0, source_file.end );
	size_t end = source_file.tellg();
	size_t source_file_size = end - start;
	source_file.seekg(0, source_file.beg );
	
	required_nodes =  source_file_size   /  BLOCK_SIZE;
	if( source_file_size   %  BLOCK_SIZE  != 0 ) ++required_nodes;
	if( source_file_size == 0 ) required_nodes = 1;
	if( required_nodes > get_free_nodes() ) 
	{
		std::cout << "Zbyt mała ilość miejsca na dysku" << endl;
		return;
	}
	bool begin = true;
	unsigned int first;
	unsigned int previous = 0;
	for( size_t i  = 0; i < nodes_count; i++)
	{
		if( nodes[i].used == false ) 
		{
			required_nodes -= 1;
			nodes[i].used = true;
			strcpy( nodes[i].name, source_file_name);
			if( begin == true )
			{
				nodes[i].start = true;
				begin = false;
				first = i;
			}	
			else
			{
				nodes[previous].next_node = i;
			}
			previous = i;
		}
		if( required_nodes <= 0 ) break;
	}
	if( source_file_size == 0 )
	{			
		nodes[first].size = 0;
		file.seekg( sizeof(vfs_size) + first * sizeof(node), file.beg );
		file.write( reinterpret_cast <char* > ( &(nodes[first].used)  ), sizeof(bool) );
		file.write( reinterpret_cast <char* > ( &(nodes[first].start) ), sizeof(bool) );
		file.write( nodes[first].name, 22 * sizeof(char) );
		file.write( reinterpret_cast <char* > ( &(nodes[first].size) ), sizeof(unsigned int) );
		file.write( reinterpret_cast <char* > ( &(nodes[first].next_node) ), sizeof(unsigned int) );		
		first = nodes[first].next_node;
	}
	else
	{
	while( source_file_size > 0 )
	{
		if( source_file_size >= BLOCK_SIZE )
		{
			nodes[first].size = BLOCK_SIZE;
			source_file.read( data, BLOCK_SIZE );
			file.seekg( get_block_position(first), file.beg );
			file.write(data, BLOCK_SIZE );
			source_file_size -= BLOCK_SIZE;
		
		}
		else
		{
			nodes[first].size = source_file_size;
			source_file.read( data, source_file_size );
			file.seekg( get_block_position(first), file.beg );
			file.write(data, source_file_size );
			source_file_size -= source_file_size;
		}
			file.seekg( sizeof(vfs_size) + first * sizeof(node), file.beg );
			file.write( reinterpret_cast <char* > ( &(nodes[first].used)  ), sizeof(bool) );
			file.write( reinterpret_cast <char* > ( &(nodes[first].start) ), sizeof(bool) );
			file.write( nodes[first].name, 22 * sizeof(char) );
			file.write( reinterpret_cast <char* > ( &(nodes[first].size) ), sizeof(unsigned int) );
			file.write( reinterpret_cast <char* > ( &(nodes[first].next_node) ), sizeof(unsigned int) );		
			first = nodes[first].next_node;
	}

	}
	file.close();
	file.clear();
	source_file.close();
	source_file.clear();	
}

void vfs::copy_from_vfs( const char * file_to_copy, const char * destination )
{
	char data[BLOCK_SIZE];
	bool found = false;
	for(size_t i = 0; i < nodes_count; i++)
	{	
		if( strcmp(nodes[i].name, file_to_copy) == 0 && nodes[i].used == true )
		{
			found = true;
			break;
		}
	}
	if( found == false ) 
	{
		std::cout << "Nie ma takiego pliku na dysku" << endl;
		return;
	}
	fstream new_file;
	fstream file; // nasz system plików
	new_file.open( destination, std::ios::out | std::ios::binary);
	file.open( vfs_name, std::ios::in | std::ios::out | std::ios::binary);
	for(size_t i = 0; i < nodes_count; i++)
	{	
		if( strcmp(nodes[i].name, file_to_copy) == 0  && nodes[i].used == true )
		{
			file.seekg( get_block_position(i), file.beg );
			file.read(data, nodes[i].size );
			new_file.write( data, nodes[i].size );
		}
	}
	file.close();
	file.clear();
	new_file.close();
	new_file.clear();
}

void vfs::delete_from_vfs( const char * file_to_delete )
{
	if(strlen(file_to_delete) > 22)
	{
		std::cout << "Nie ma takiego pliku na dysku" << endl;
		return;
	}
	unsigned found = -1;
	for(size_t i = 0; i < nodes_count; i++)
	{	
		if( strcmp(nodes[i].name, file_to_delete) == 0  && nodes[i].used == true )
		{
			found = i;
			break;
		}
	}
	if( found == -1 )
	{
		std::cout << "Nie ma takiego pliku na dysku" << endl;
		return;
	}
	unsigned int pom = nodes[found].next_node ;
	nodes[found].clean();
	while( pom != 0 )
	{
		found = pom;
		pom = nodes[found].next_node ;
		nodes[found].clean();
	}
}

void vfs::show_list()
{
	for( size_t i = 0 ; i < nodes_count; i++)
	{
		if( nodes[i].start == true && nodes[i].used == true ) cout << "Na dysku znajduje się plik: " << nodes[i].name << endl;
	}

}

void vfs::dump()
{
	cout << "Rozmiar virtualnego dysku: " << vfs_size << endl;
	for( size_t i = 0; i < nodes_count; i++)
	{
		cout << endl;
		cout << "Węzeł o ID: " << i << endl;
		cout << "Zajęty: " << nodes[i].used << endl;
		cout << "Przechowywany plik: " << nodes[i].name << endl;
		cout << "Początek pliku: " << nodes[i].start << endl;
		cout << "Rozmar danych: " << nodes[i].size << endl;
		cout << "Następny węzeł: " << nodes[i].next_node << endl << endl;
	}
}


