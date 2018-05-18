#include <lib.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
int main(int ArgC, char ** ArgV)
{
	int chosen_pid= atoi(ArgV[1]);
	message m;
	int new_tickets = atoi(ArgV[2]);
	if( ArgC !=3)
	{
		printf("Pierwszy musi być być pid, potem liczba losow\n");
		return 0;
	}
	if( new_tickets >20 || new_tickets < 1) 
	{
		printf("Liczba losow musi byc z zakresu 1-20 \n");
		return 0;
	}
	m.m1_i1 = chosen_pid;
	m.m2_l2 = new_tickets;
	_syscall(MM, SETLOT, &m);
	return 0;
}
