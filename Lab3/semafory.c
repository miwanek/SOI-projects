#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // biblioteka m.in z funkcją fork tworzącą proces
#include <sys/wait.h> // biblioteka z funkcją wait, która czeka na zakończenie procesów potomnych
#include <sys/shm.h> // biblioteka do pamięci współdzielonej
#include <semaphore.h> // POSIX'owe semafory
#include <time.h>
#include <string.h> // atoi
//#include <ipc.h>

unsigned int maker_number;
unsigned int queue_size;
unsigned int item_limit;

struct fifo_queue // tworzymy sobie obiekt do trzymania klejki fifo
{
	unsigned int end, start;
	int * items;
};

struct fifo_queue * get_queue() 
{
	static int shmid = 0; // przydzielamy pamieć do kolejki albo po prostu dostajemy identyfikator obaszru pamieci
	if(shmid == 0)
		shmid = shmget(IPC_PRIVATE,  sizeof(struct fifo_queue) + queue_size * sizeof(int), SHM_W | SHM_R);
	
	if(shmid <= 0)
	{
		printf("shmget zwrocil blad, kod znajduje sie w errno\n");
		abort();
	}

	void* data= shmat(shmid, NULL, 0 );
	struct fifo_queue * given_queue = (struct fifo_queue *) data;
	given_queue->items=  data+ sizeof(struct fifo_queue);
	
	return given_queue;
}
void insert_element (struct fifo_queue * given_queue,  int item)
{
	given_queue->items[given_queue->end] = item;
	given_queue->end = (given_queue->end + 1 )% queue_size;
}
unsigned int remove_element (struct fifo_queue * given_queue)
{
	unsigned int ret = given_queue->items[given_queue->start];
	given_queue->start = (given_queue->start + 1 )% queue_size;
	return ret;
}

struct semaphores
{
	sem_t mutex; // mutex blokujący producentom dostęp do bufora
	sem_t full; // semafor wskazujący na liczbe gotowych produktów
	sem_t empty; // semafor wskazujący na liczbe wolnych miejsc
};

struct semaphores* get_semaphores()
{
	static int  shmid = 0; 
	if(shmid == 0) // IPC_PRIVATE zapewnia nam unikalny identyfikator, który jest dziedzicony przez procesy potomne
	{
       		shmid = shmget(IPC_PRIVATE, sizeof(struct semaphores) ,SHM_W|SHM_R ); // Przydzielamy pamięć współdzieloną, jeśli już istnieje to dostajemy tylko adres
	} // SHM_W | SHM_R przydziela permijse do odczytu/ zapisu dla użytkownika !!!! Można też użyć 0666

   	if(shmid < 0)
    	{
        printf("shmget zwrocil blad, kod znajduje sie w errno\n");
        abort();
    	}
	
								// shmat doczepia do procesu pamięć współdzieloną/zwraca adres przdzielonej pamięci
    	struct semaphores * used_semaphores = (struct semaphores *) shmat(shmid, NULL, 0); // rzutujemy pamięć współdzieloną na  semaphores * 

   	return used_semaphores;
}

void maker_task()
{
	struct fifo_queue * given_queue = get_queue(); // dołączamy kolejkę i semafory do procesu
  	struct semaphores * used_semaphores = get_semaphores();
   	unsigned int sent_items = 0; // na starcie producent nie wytworzył jeszcze nic
	unsigned int necessary_time;
	unsigned int ret ;
	unsigned int item;

	unsigned short maker_pid;
	maker_pid= getpid();

    	printf("Producent o pid %d zaczyna prace \n", maker_pid);

	while(sent_items < item_limit)
	{
		usleep(50000);
        	sem_wait(&used_semaphores->empty);      // jeżeli nie ma pustych miejsc w semaforze wolnych miejsc
		sem_wait(&used_semaphores->mutex);	// blokujemy dostęp innym producentom
		
		item = rand() % 1000 + maker_pid % 1000;     // generujemy produkt
		necessary_time= rand()%(item+maker_pid) *500;
		insert_element(given_queue, item); // wstawiamy  element do kolejki
		printf("Producent o pid %d przekazal produkt %d \n",maker_pid, item);
		sem_post(&used_semaphores->mutex);	// odblokowujemy dostęp innym producentom
		sem_post(&used_semaphores->full);       // podnosimy semafor zajętych miejsc
        	sent_items++;
    	}
	printf("Proces producenta o pid %d zakonczony \n", maker_pid);

}

void client_task()
{
  	 printf("Proces klienta uruchomiony \n");
	 struct fifo_queue * given_queue = get_queue(); // dołączamy kolejkę i semapfory do procesu
  	 struct semaphores * used_semaphores = get_semaphores();
   	 unsigned int received_items = 0; // na starcie nie odebrał żadnych produktów
	 unsigned int necessary_time;
	 unsigned int ret ;


    while(received_items < maker_number * item_limit) // do odebrania jest tyle produktów co producent*limit towarów
    {
	usleep(30000);
        sem_wait(&used_semaphores->full); //blokujemy proces jeśli  wartość semafora mniejsza od zera
	sem_wait(&used_semaphores->mutex);	// nie potrzebujemy tu mutexa
	necessary_time= rand()%1000 *20;
	usleep(necessary_time);
	
        ret = remove_element( given_queue );         //zabieramy produky z kolejki
	printf("Klient odebral produkt %d \n", ret);

	sem_post(&used_semaphores->mutex);	// nie potrzebujemy tu mutexa
        sem_post(&used_semaphores->empty);                //podnosimy semafor ogolnej liczby miejsc

        received_items++;
    }
    printf("Proces klienta zakonczony \n");

}

void create_client()
{
	int fork_result= fork();
	if(fork_result == 0 )
	{
		client_task();
		exit(0);
	}
}

void create_maker()
{
	int fork_result= fork();
	if(fork_result == 0 )
	{
		maker_task();
		exit(0);
	}
}

void prepare_queue()
{
	struct fifo_queue* given_queue = get_queue();
	memset(given_queue, 0, sizeof( struct fifo_queue));
	given_queue->start= 0;
	given_queue->end= 0;
}

void prepare_semaphores()
{
	struct semaphores* used_semaphores = get_semaphores();
	sem_init(&used_semaphores->full,7, 0 ); // wskazanie na semafor/ 0 dzielone tylko na wątkach !=0 dzielone na procesach/ inicjalizacja liczba
	sem_init(&used_semaphores->empty,7, queue_size );
	sem_init(&used_semaphores->mutex,7, 1);
}

void destroy_semaphores()
{
	struct semaphores* used_semaphores = get_semaphores();
	sem_destroy(&used_semaphores->mutex);
	sem_destroy(&used_semaphores->empty);
	sem_destroy(&used_semaphores->full);
}

int main(int argc, char* argv[] )
{
    	if(argc!= 4) 
	{
	 	printf("Podano zbyt mala liczbe argumentow \n");
		printf("Nalezy podac kolejno: liczbe producentow, rozmiar bufora oraz liczbe towarow do wyprodukowania przez kazdego producent \n");
		exit(0);
	}
	srand(time(NULL));
	maker_number= atoi(argv[1]); // odbieramy dane podane przez użytkownika
	queue_size= atoi(argv[2]);
	item_limit= atoi(argv[3]);
	if(queue_size <=0 ) 
	{
		printf("To chyba żart. Kolejka musi miec rozmiar co najmniej 1 \n");
		return -1;
	}

	prepare_semaphores();
	prepare_queue();

	create_client(); // tworzymy proces konsumenta
	int i;
	for(i=0 ; i< maker_number; i++ ) // wrzucamy procesy producentów
	{
		create_maker();
	}
	while(wait(NULL) >0 ); // czekamy aż wszystkie procesy potomne znikną
	destroy_semaphores();
	
    return 0;
}
