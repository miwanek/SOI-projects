#include <iostream>
#include <thread> // biblioteka, która od c++11 udostępnia wygodną obsługę wątków
#include <vector> // biblioteka do używania vector'a
#include "monitor.h" // monitor od kruka

#include <cstdio>
#include <cstdlib>


#include <ctime>
#include <string> // atoi

using namespace std;

unsigned int maker_number;
unsigned int queue_size;
unsigned int item_limit;

class Cyclic_Buffer // bufor cykliczny, który realizuje kolejkę fifo
{
	public:
		Cyclic_Buffer(unsigned int queue_size): current_size(0), buffer_size(queue_size), place_to_insert(0), place_to_extract(0)
		{
			buff= new int[queue_size];
		}
		~Cyclic_Buffer()
		{
			delete buff;
		}
		unsigned int get_current_size()
		{
			return current_size;
		}
		unsigned int get_max_size()
		{
			return buffer_size;
		}
		bool buffer_full()
		{
			if(current_size == buffer_size) return true;
			else return false;
		}
		bool buffer_empty()
		{
			if(current_size == 0 ) return true;
			else return false;
		}
		void insert_element( int item)
		{
			if(buffer_full() == true )
			{
				std::cout<<"Blad zapisu";
				return;
			}
			buff[place_to_insert]= item;
			place_to_insert= (place_to_insert +1) % buffer_size ;
			++current_size;
		}
		int extract_element()
		{
			if(buffer_empty()== true  )
			{
				std::cout<<"Blad odczytu";
				return -1;
			}
			int extracted_element = buff[place_to_extract];
			buff[place_to_extract]= 0;
			place_to_extract= (place_to_extract +1) % buffer_size ;
			--current_size;
			return extracted_element;
		}
		
	private:
		unsigned int current_size;
		unsigned int buffer_size;
		int* buff;
		unsigned int place_to_insert;
		unsigned int place_to_extract;
};

class Extended_Monitor :  Monitor
{
	public:
		Extended_Monitor( unsigned int queue_size) :  item_queue(queue_size) {}
		
		void add_element( int new_item, thread::id maker_id)
		{
			enter();
			
			if(item_queue.buffer_full() == true )
			{
				wait(Not_Full);
			}
			item_queue.insert_element(new_item);
			cout<< "Producent o id "<<maker_id<<" wyprodukował produkt "<<new_item<<endl;
			if(item_queue.get_current_size() == 1)
			{
				signal(Not_Empty);
			}
			leave();
		}
		void get_element()
		{
			enter();
			
			if(item_queue.buffer_empty() == true )
			{	
				wait(Not_Empty);
			}	
			
			unsigned int extracted_item = item_queue.extract_element();
			cout<<"Klient odebral produkt  "<< extracted_item << endl;
			if(item_queue.get_current_size() == item_queue.get_max_size() - 1)
			{
				signal(Not_Full);
			}
			
			leave();
		}
	private:
		Condition Not_Full; // zmienna zawieszająca wątek jeśli kolejka pełna
		Condition Not_Empty; // zmienna zawieszająca wątek jeśli kolejka pusta
		//Condition Queue_not_used; 
		
		Cyclic_Buffer item_queue; // nasza kolejka FIFO realizowana jako bufor cykliczny
		
};



void maker_task( unsigned int item_limit, Extended_Monitor* used_monitor )
{

   	unsigned int sent_items = 0; // na starcie producent nie wytworzył jeszcze nic
	unsigned int necessary_time; // dodatkowo, losowy czas do odczekania
	unsigned int ret ;
	unsigned int item;
	std::thread::id maker_id= this_thread::get_id(); // Wyciągamy id naszego wątku

	cout<< "Producent o id "<<maker_id<<" rozpoczyna prace"<<endl;
	while(sent_items < item_limit)
	{
		usleep(50000);
		item = rand() % 1000; // + maker_pid % 1000;     // generujemy produkt
		necessary_time= ( rand()%item ) *500; // losujemy czas do odczekania
		usleep(necessary_time);

		used_monitor->add_element( item, maker_id); // wstawiamy  element do kolejki
        	sent_items++;
    	}
	cout<< "Wątek producenta o id "<<maker_id<<" zakończony"<<endl;
}

void client_task( unsigned int maker_number, unsigned int item_limit, Extended_Monitor* used_monitor )
{
  	 cout<<"Wątek klienta uruchomiony"<<endl;
   	 unsigned int received_items = 0; // na starcie nie odebrał żadnych produktów
	 unsigned int necessary_time;
	 unsigned int ret ;


    while(received_items < maker_number * item_limit) // do odebrania jest tyle produktów co producent*limit towarów
    {
	usleep(30000);
	necessary_time= ( rand()%1000 ) *20; // losujemy czas do odczekania
	usleep(necessary_time);
	
        used_monitor->get_element();         //zabieramy produkty z kolejki
        received_items++;
    }
    cout<<"Proces klienta zakonczony"<<endl;

}



int main(int argc, char* argv[] )
{
    	if(argc!= 4) 
	{
	 	cout << "Podano zbyt mala liczbe argumentow "<< endl;
		cout<<"Nalezy podac kolejno: liczbe producentow, rozmiar bufora oraz liczbe towarow do wyprodukowania przez kazdego producent"<<endl;
		exit(0);
	}
	srand(time(NULL));
	maker_number= atoi(argv[1]); // odbieramy dane podane przez użytkownika
	queue_size= atoi(argv[2]); // rozmiar kolejki
	item_limit= atoi(argv[3]); // liczba przedmiotów do wyprodukowania przez każdego producenta

	if(queue_size <=0 ) 
	{
		cout<<"To chyba żart. Kolejka musi miec rozmiar co najmniej 1"<<endl;
		return -1;
	}
	Extended_Monitor used_monitor(queue_size); // tworzymy sobie statyczny monitor widoczny dla wszystkich wątków
	std::thread client(client_task,maker_number, item_limit, &used_monitor); // tworzymy proces konsumenta
	vector<thread> producers(maker_number);	 // tworzymy wektor wątków o rozmiarze podanym przez użytkownika, na razie nie mamy tu żadnych wątków
	for( size_t i=0 ; i< maker_number; i++ ) // wrzucamy procesy producentów
	{
		producers[i]= thread(maker_task ,item_limit, &used_monitor ); // dodajemy wątki do wektora
	}
	for( size_t i=0 ; i< maker_number; i++ ) // wrzucamy procesy producentów
	{
		producers[i].join(); // czekamy na zakończenie procesów producenta
	}
	client.join(); //czekamy na zakończenie procesów konsumenta
    return 0;
}
