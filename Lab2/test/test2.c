#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <lib.h>

int main(int ArgC, char ** ArgV)
{
	
	long i,j, max;
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
	max= 5*new_tickets;
	for(i=0; i<max; i++)
	{
		for(j=0; j<100000000; j++) {}
		printf("Wybrano proces o PID: %d i liczbie losow %d \n", my_pid, new_tickets);
	}
	return 0;
}
