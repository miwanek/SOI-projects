#include <lib.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
int main(int ArgC, char ** ArgV)
{
	int new_tickets = atoi(ArgV[1]);
	int my_pid= getpid();
	message m;
	if( new_tickets >20 || new_tickets < 1) 
	{
		printf("Liczba losow musi byc z zakresu 1-20 \n");
		return 0;
	}
	m.m1_i1 = my_pid;
	m.m2_l2 = new_tickets;
	_syscall(MM, SETLOT, &m);
	_syscall(MM, SHOWLOT, &m);
	return 0;
}
