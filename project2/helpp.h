#ifndef HELPP_H
#define HELPP_H


//stathera gia to megethos tou shm segment
#define SHM_SIZE 1024

//stathera gia to onoma tou socket
#define UNIXSTR_PATH "/tmp/socket.str"

//stathera pou  orizei to megethos ths ouras tou server
#define LISTENQ 640

#define SOCKET_NAME "A"


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

#define NUM_THREADS 3

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


//global pointer gia oura
int *v; 


//boithitikh metablhth
int temp;

int tmp; //boithitikh metablhth
int zone; //boithitikh metablhth gia random epilogh zwnhs

//int *ready = 0; //if a reservation is completed


//Socket descriptors
int listenfd, connfd; 

//Threads

pthread_t thread[NUM_THREADS];
pthread_attr_t attr;

//10 mutexes for telephonists
pthread_mutex_t mutex_tel[10];

//4 mutexes for bank
pthread_mutex_t mutex_b[4];

//alla mutexes
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
pthread_mutex_t mutex3;
pthread_mutex_t mutex4;
pthread_mutex_t mutex5;


int reservation[3] = {0};   //pinakas pou tha apothikeuei thn krathsh tou client
                                   //reservation[0] : number of tickets
                                   //reservation[1] : zone
                                   //reservation[2] : credit card (10 numbers)

       
int *res; //pinakas ston opoio apothikeuetai h krathsh tou client pou einai prwtos sthn oura
int *p; //pointer ston pinaka reservation

int valid = 0; //if the validation is completed

int telephonists[10] = {0};

int bank[4] = {0};

time_t t;



//FUNCTION PROTOTYPES

void *accept_client();

void *cost_calc();

void *validation_function();

int *queue_get(struct queue *q);

void queue_put(struct queue *q, int **v);

void catch_int(int sig_num);

#endif