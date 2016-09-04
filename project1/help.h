#ifndef HELP_H
#define HELP_H


//stathera gia to megethos tou shm segment
#define SHM_SIZE 1024

//stathera gia to onoma tou socket
#define UNIXSTR_PATH "/tmp/socket.str"

//stathera pou  orizei to megethos ths ouras tou server
#define LISTENQ 640

#define SOCKET_NAME "A"

#define SEM_NAME_P "SEMP"

#define SEM_NAME_D "SEMD"

#define SEM_NAME_R "SEMR"

#define SEM_NAME_T "SEMT"

#define SEM_NAME_B "SEMB"

#define SEM_NAME_K "SEMK"

//STATHERES SYSTHMATOS
#define NTHL 10
#define NBANK 4
#define TSEATFIND 6
#define TCARDCHECK 2
#define TWAIT 10
#define TTRANSFER 30
#define Z_A 100
#define Z_B 130
#define Z_C 180
#define Z_D 230


//struct of the reservation of the client
struct list
{
	int *val; //pinakas pou tha periexei thn krathsh tou client
                //val[0] : number of tickets
                //val[1] : zone
                //val[2] : credit card (10 numbers)

	struct list *next;
		
};

//queue of the lists of reservations of the clients
struct queue
{
	struct list *head;
	struct list *tail;
}q_res; 


//return value from shmget()
int shmid;


//global pointer sthn prwth thesh ths koinhs mnhmhs
int *data;


//global pointer gia oura//global pointer gia oura
int *v; 


//semaphores
sem_t *my_semp, *my_semd, *my_semr, *my_semt, *my_semb, *my_semk;

//boithitikh metablhth
int temp;



//Socket descriptors
int listenfd, connfd; 



//PROCESSES

pid_t req; // process for responding to the request of a client

pid_t val; // process for checking the credit card of the client




//FUNCTION PROTOTYPES

int cost_calc(int *r);

int validation_function();

int *queue_get(struct queue *q);

void queue_put(struct queue *q, int **v);

void catch_int(int sig_num);

#endif