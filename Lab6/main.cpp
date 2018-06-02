#include <iostream>
#include <cstdlib>
#include <cstring>
#include "vfs.hpp"
#include "node.hpp"

using namespace std;

int main(int argc, char* argv[] )
{	
	if(argc < 3 || argc > 5)
	{
		cout << "Welcome to virtual file system" << endl;
		cout << "Available commands: " << endl;
		cout << "create <disc name> <size> ( creates new disk with given name and size)" << endl;
		cout << "push  <disc name> <source_file> ( adds a file to vls) " << endl;
		cout << "pull  <disc name> <file_name> <destination>  ( copies a file from vls and copies content to destination)" << endl;
		cout << "list  <disc name> ( shows all names of files inside a disk) " << endl;
		cout << "dump  <disc name> ( shows all sectors of disc ) " << endl;
		cout << "remove <disc name> <file to erase> ( erases given file from vls)" << endl;
		cout << "delete <disc name> ( deletes virtual disc ) " << endl;

		return 1;
	}
	char* command = argv[1];
	char* vfs_name = argv[2];
	
	if(strcmp("create", command) == 0)
	{
		if(argc == 4)
		{
			size_t size = atoi(argv[3]);
			vfs disc(vfs_name, size );
		}
		else cout << "Invalid arguments" << endl;
	}
	else if(strcmp("push", command) == 0)
	{
		if(argc == 4)
		{
			vfs disc(vfs_name);
			disc.copy_to_vfs(argv[3]);
		}
		else cout << "Invalid arguments" << endl;	
	}

	else if(strcmp("pull", command) == 0)
	{
		if(argc == 5)
		{
			vfs disc(vfs_name);
			disc.copy_from_vfs(argv[3], argv[4]);
			
		}
		else cout << "Invalid arguments" << endl;	
	}
	else if(strcmp("list", command) == 0)
	{
		if(argc == 3)
		{
			vfs disc(vfs_name);
			disc.show_list();	
		}
		else cout << "Invalid arguments" << endl;
	}
	else if(strcmp("dump", command) == 0)
	{
		if(argc == 3)
		{
			vfs disc(vfs_name);
			disc.dump();

		}
		else cout << "Invalid arguments" << endl;
	}
	else if(strcmp("remove", command) == 0)
	{
		if(argc == 4)
		{
			vfs disc(vfs_name);
			disc.delete_from_vfs( argv[3] );
		}
		else cout << "Invalid arguments" << endl;
	}
	else if(strcmp("delete", command) == 0)
	{
		if(argc == 3)
		{
			vfs::destroy_vfs(vfs_name);
		}
		else cout << "Invalid arguments" << endl;
	}
	else
	{
		cout << "Invalid arguments" << endl;
	}

    return 0;
}
