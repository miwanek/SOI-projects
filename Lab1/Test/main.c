#include </usr/include/stdio.h>
#include </usr/include/lib.h>
#include </usr/include/minix/callnr.h>
#include </usr/include/stdlib.h>

int getprocnr( int ID );
int main( int argc, char* argv[] )
{
	int i;
	int nr;
	if(argc!=2) printf("Process ID needed\n");
	else
	{
		int ID= atoi(argv[1]);
		printf("Podane ID to %d \n", ID);
		for(i=0; i<10; ++i)
		{
		  	nr=getprocnr(ID+i );
			if( nr!=-1 ) printf("Nr procesu w tablicy o id(pid) %d to %d\n", ID+i, nr);
			else printf("Nie ma procesu o id(pid) %d w tablicy, kod bladu %d \n", ID+i, errno);
		}
	}
	return 0;
}

int getprocnr( int ID )
{
	message msg;
	msg.m1_i1= ID;
	return(_syscall(MM,GETPROCNR,&msg));
}
