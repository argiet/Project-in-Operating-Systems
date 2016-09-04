#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> /* basic system data types */
#include <sys/socket.h> /* basic socket definitions */
#include <errno.h> /* for the EINTR constant */
#include <sys/wait.h> /* for the waitpid() system call */
#include <sys/un.h> /* for Unix domain sockets */
#include <sys/ipc.h>
#include <assert.h>

//header files for semaphores
#include <semaphore.h>
#include <sys/sem.h>
#include <fcntl.h>     
#include <sys/stat.h>     
#include <pthread.h>

//header file for signals
#include <signal.h>                                                          

//header file me tis katallhles dhlwseis statherwn
#include "help.h" 

//clean_up function: kill processes, close socket, shared memory, semaphores and finally the program

void clean_up(int cond){
       printf("server exiting now!!!!!\n");
       close(connfd); //Parent closes connected socket
       close(listenfd); //Close listening socket
       shmctl(shmid,IPC_RMID, NULL);
       sem_close(my_semp);
       sem_close(my_semd);
       sem_close(my_semr);
       sem_close(my_semt);
       sem_close(my_semb);
       sem_unlink(SEM_NAME_P);
       sem_unlink(SEM_NAME_D);
       sem_unlink(SEM_NAME_R);
       sem_unlink(SEM_NAME_T);
       sem_unlink(SEM_NAME_B);
       kill(val,SIGKILL);
       kill(req, SIGKILL);
       exit(cond);
}

/* The use of this function avoids the generation of "zombie" processes. */
void sig_chld( int signo )
{
       pid_t req, val;
       int stat;

       while ( ( req = waitpid( -1, &stat, WNOHANG ) ) > 0 ) 
       {
              printf( "Child %d terminated.\n", req );
       }
       while ( ( val = waitpid( -1, &stat, WNOHANG ) ) > 0 ) 
       {
              printf( "Child %d terminated.\n", val );
       }
}

//SIGINT handler
void catch_int(int sig_num)
{


       printf("server exiting now!!!!!\n");
       close(connfd); //Parent closes connected socket
       close(listenfd); //Close listening socket
       shmctl(shmid,IPC_RMID, NULL);
       sem_close(my_semp);
       sem_close(my_semd);
       sem_close(my_semr);
       sem_close(my_semt);
       sem_close(my_semb);
       sem_close(my_semk);
       sem_unlink(SEM_NAME_K);
       sem_unlink(SEM_NAME_P);
       sem_unlink(SEM_NAME_D);
       sem_unlink(SEM_NAME_R);
       sem_unlink(SEM_NAME_T);
       sem_unlink(SEM_NAME_B);
       kill(val,SIGKILL);
       kill(req, SIGKILL);
       fflush(stdout);
       exit(1);

}

/* MAIN FUNCTION */

int main( int argc, char **argv )
{

       // VARIABLES

       int reservation[3] = {0};   //pinakas pou tha apothikeuei thn krathsh tou client
                                   //reservation[0] : number of tickets
                                   //reservation[1] : zone
                                   //reservation[2] : credit card (10 numbers)

       

       int *res; //pinakas ston opoio apothikeuetai h krathsh tou client pou einai prwtos sthn oura
       int *p; //pointer ston pinaka reservation

       p = reservation; // pointer ston pinaka reservation 

       int ready = 0; //if a reservation is completed
       int valid = 0; //if the validation is completed


       int tmp; //boithitikh metablhth
       int zone; //boithitikh metablhth gia random epilogh zwnhs
       

       //FOR SOCKETS

       socklen_t clilen; // length of socket
       
       struct sockaddr_un cliaddr, servaddr; //Structs for the client and server socket addresses.




       //Avoid "zombie" process generation
       signal( SIGCHLD, sig_chld ); 

       //Signal handler gia SIGINT
       signal( SIGINT, catch_int); 



       
       //SEMAPHORE CREATION

       // create semafore p 
       
       my_semp = sem_open(SEM_NAME_P, O_CREAT, 0600,1);
       if(my_semp == SEM_FAILED)
       {
              perror("couldn't open semaphore p\n");
              exit(1);
       }
       //else printf("open semaphore p success\n");



       // create semafore d 

       my_semd = sem_open(SEM_NAME_D, O_CREAT, 0600,1);
       if(my_semd == SEM_FAILED)
       {
              perror("couldn't open semaphore d \n");
              exit(1);
       }
       //else printf("open semaphore d success\n");



       // create semafore r 

       my_semr = sem_open(SEM_NAME_R, O_CREAT, 0600,1);
       if(my_semr == SEM_FAILED)
       {
              perror("couldn't open semaphore r \n");
              exit(1);
       }
       //else printf("open semaphore r success\n"); 



       my_semt = sem_open(SEM_NAME_T, O_CREAT, 0600,1);
       if(my_semt == SEM_FAILED)
       {
              perror("couldn't open semaphore t \n");
              exit(1);
       }
       //else printf("open semaphore t success\n"); 



       my_semb = sem_open(SEM_NAME_B, O_CREAT, 0600,1);
       if(my_semb == SEM_FAILED)
       {
              perror("couldn't open semaphore b \n");
              exit(1);
       }
       //else printf("open semaphore b success\n"); 



       my_semk = sem_open(SEM_NAME_K, O_CREAT, 0600,1);
       if(my_semk == SEM_FAILED)
       {
              perror("couldn't open semaphore k \n");
              exit(1);
       }
       //else printf("open semaphore k success\n"); 


       //SEMAPHORE INITIALIZATION TO NTHL
       sem_init(my_semt, 0, NTHL);

       //SEMAPHORE INITIALIZATION TO NBANK
       sem_init(my_semk, 0, NBANK);

       //sem_init(my_semp, 0, LISTENQ);




       //CONNECTION BETWEEN SERVER AND CLIENT

       listenfd = socket( AF_LOCAL, SOCK_STREAM, 0 ); // Create the server's endpoint

       unlink( UNIXSTR_PATH ); // Remove any previous socket with the same filename

       bzero( &servaddr, sizeof( servaddr ) ); // Zero all fields of servaddr

       servaddr.sun_family = AF_LOCAL; // Socket type is local (Unix Domain)

       strcpy( servaddr.sun_path, UNIXSTR_PATH ); // Define the name of this socket

       bind( listenfd, ( struct sockaddr* ) &servaddr, sizeof( servaddr ) ); // Create the file for the socket and register it as a socket

       listen( listenfd, LISTENQ ); // Create request queue



       //SHAREDMEMORY CREATION
              
       key_t key;    //key to be passed to shmget() 

       key = 5678;  //We'll name our shared memory segment "5678"

       int *data1; //pointer pou tha deixnei sthn prwth thesh ths koinhs mnhmhs


       //Create the segment.
       if ((shmid = shmget(key, SHM_SIZE, 0600 | IPC_CREAT)) < 0)
       {
              perror("shmget");
              exit(1);
       }


       //Now we attach the segment to our data space. 
       if ((data1 = shmat(shmid, NULL, 0)) == (char *) -1)
       {
              perror("shmat");
              exit(1);
       }


       // kanoume ton global pointer na deixnei kai autos sthn prwth thesh ths koinhs mnhmhs
       data = (int *)data1; 



       //ARXIKOPOIHSH TWN DATA STH SHARED MEMORY MESW TOU POINTER data POU DEIXNEI STHN PRWTH THESH THS SHARED MEMORY

       //0. to plithos tvn apasxolhmenwn thlefwnhtwn 
       //1. to plhthos twn reguest pou eksuphretei h trapeza
       //2. oi eisprakseis ap ta eishthria sto company account
       //3. oi eisparkseis ap ta eishthria sto theater account
       //4. poses theseis exoun desmeytei se kathe zone

       *data = 0; //sum of the reserved seats of the theater
       *(data+2) = 0; //set the company account to zero
       *(data+3) = 0; //set the theater account to zero
       *(data+4) = 0; //set the number of reserved seats of zone A to zero
       *(data+5) = 0; //set the number of reserved seats of zone B to zero
       *(data+6) = 0; //set the number of reserved seats of zone C to zero
       *(data+7) = 0; //set the number of reserved seats of zone D to zero
       *(data+8) = 0; //set the number of clients to zero
       *(data+9) = 0; //credit card is valid
       *(data+10) = 0; //reservation is ready
       *(data+1) = 0; //Den to xrhsimopoioume



       printf("Starting Server\n");

       /* WAITING FOR A REQUEST FROM THE CLIENT */

       for ( ; ; ) 
       {
              clilen = sizeof( cliaddr );

              // Copy next request from the queue to connfd and remove it from the queue
              connfd = accept( listenfd, ( struct sockaddr * ) &cliaddr, &clilen );

              if ( connfd < 0 ) 
              {
                     if ( errno == EINTR ) /* Something interrupted us. */
                     {      //continue; /* Back to for()... */
                            printf("It's not ok\n");

                     }
                     else 
                     {
                            fprintf( stderr, "Accept Error\n" );
                            exit( 0 );
                     }
              }


              //RECIEVE DATA FROM CLIENT

              read(connfd, &tmp, sizeof(int));

              if(tmp == 1)
              {


              //read the number of the tickets
              if((read(connfd, &reservation[0], sizeof(int))) > 0) printf("read number of tickets: %d\n\n", reservation[0]);
              //read(connfd, &reservation[0], sizeof(int));       


              //read the zone of the theater             
              if((read(connfd, &reservation[1], sizeof(int))) > 0) printf("read zone: %d\n\n", reservation[1]); 
              //read(connfd, &reservation[1], sizeof(int));


              //read the number of the credit card;
              if((read(connfd, &reservation[2], sizeof(int))) > 0) printf("read credit card: %d\n\n", reservation[2]); 
              //read(connfd, &reservation[2], sizeof(int));



              }     

              //RANDOM

                    
              
    //random number of tickets          
    srand((unsigned int) getpid());
    reservation[0] = rand() % 4 + 1;



//random zone

/* apo tous 100 clients 10% apo autous tha dialeksoun rand() th zoni A, 
20% apo autous tha dialeksoun tuxaia th zoni B , 
30% apo autous tha dialeksoun th zoni C 
kai 40% apo autous tha dialeksoun th zoni D. 
Se mia sunartisi rand() epilegetai enas arithmos apo to 1-4 kai analoga me ton arithmo mesw ths switch epilegetai i zoni */

    srand((unsigned int) getpid());
    zone = rand() % 4 + 1;
    switch(zone)
    {
        case 1 :

    
            if ((*(data+4))++ <= 0.1*LISTENQ)
            {
                reservation[1] = 1;
                break;
            }

        case 2 :
        if ( (*(data+5))++ <= 0.2*LISTENQ)
        {
            reservation[1] = 2;
            break;
        }
        case 3 :
        if ( (*(data+6))++ <= 0.3*LISTENQ)
        {
            reservation[1] = 3;
            break;
        }
        case 4 :
        if ((*(data+7))++ <= 0.4*LISTENQ)
        {
            reservation[1] = 4;
        }
        break;

    }//switch

 


              //PUT THIS RESERVATION IN THE END OF THE QUEUE OF THE SERVER
              queue_put(&q_res, &p);


              //sem_wait(my_semp); //lock semaphore my_semp for protection of shared memory


              (*(data+8))++; //increase the number of clients

              printf("%d\n", *(data+8)); //print client's id
              
              //sem_post(my_semp); //unlock semaphore my_semp



              //PROCESS req starting for responding to the request of a client

              req = fork(); // Spawn a child for responding to the request of a client

              if(req == 0) // REQ PROCESS
              {
              

                                   sem_wait(my_semt); // decrease value of semaphore my_semt
                                                        //when it is -1 blocks other processes
                                                        //if 10 telephonists are occupied

                                   //printf("There is an available telephonist\n\n");

                                   
                                   res = queue_get(&q_res); //pairnoume thn krathsh tou pelath pou einai prwtos sthn oura
                                                               //kai thn afairoume ap thn queue



                                   //PROCESS for checking if the number of the credit card is valid or if there is any money in the account
                                   
                                   val = fork(); // Spawn a child for checking if the number of the credit card is valid or if there is any money in the account
                                   
                                   if (val == 0) //VAL PROCESS
                                   {
                                          sem_wait(my_semk); // decrease value of semaphore my_semk
                                                               //when it is -1 blocks other processes
                                                               //if 4 bank machines are occupied
                                          


                                          sem_wait(my_semb);//lock semaphore my_semb for protection of shared memory at this critical section


                                          valid = validation_function(); //call validation_function for randomly checking if the credit card is valid 
                                          
                                          *(data+9) = valid; // credit card is valid
                                          

                                          sem_post(my_semb); //unlock semaphore my_semb (for validation_function function)
                                   
                                   
                                   if(*(data+9) == 1)
                                   {
                                          
                                          sem_wait(my_semr); //lock semaphore my_semr for protection of shared memory at this critical section
                     

                                          ready = cost_calc(res); //call cost_calc function
                                          
                                          *(data+9) = 0; // again not valid for next client

                                          *(data+10) = ready; //reservation is ready


                                          sem_post(my_semr); //unlock semaphore my_semr (for cost_calc function)




                                          sem_post(my_semk); //increase value of semaphore my_semk
                                                        //when it is > 0 lets other processes
                                                        //if a bank machine is available
                                   }

                                   else if(*(data+9) == 0) //if the credit card is not valid
                                   {      
                                          continue; //continue with another client
                                   }



                            }//end of val process    
                            

                            if(*(data+10) == 1)
                            {

                                   
                                   sem_wait(my_semr); //lock semaphore my_semd for protection of shared memory

                                   *(data+10) = 0; //reservation not ready for next client

                                   

                                   sem_post(my_semr); //unlock semaphore my_semd

                                   
                            }
                            
                     
                     //else continue with other clients' requests

                     waitpid(val, NULL, 0);  // father waits childs' proccess, not to block them

                     sem_post(my_semt); //increase value of semaphore my_semt
                                          //when it is > 0 lets other processes
                                          //if a telephonist is available



              }//if req process




              if(*(data+8) > LISTENQ - 1) //if the server has no other requests
              {
                            waitpid(req, NULL, 0);  // father waits childs' proccess, not to block them

                            temp = 10;
                            write(connfd, &temp, sizeof(int)); //send a value to client to print a message


                            break; //break from infinite loop , end of all
              }


 /*             if((*(data+4) == Z_A) && (*(data+5) == Z_B) && (*(data+6) == Z_C) && (*(data+7) == Z_D)) //if theater is full
              {
                            waitpid(req, NULL, 0);  // father waits childs' proccess, not to block them

                            temp = 10;
                            write(connfd, &temp, sizeof(int)); //send a value to client to print a message


                            break; //break from infinite loop , end of all
              }

*/


       }//infinite loop: waiting a request from a client  

       printf("Money :::: %d\n", *(data+2) );  

       //waitpid(req, NULL, 0);  // father waits childs' proccess, not to block them

       clean_up(0);

       return 0;
}


int validation_function()
{
       //LOCAL VARIABLES

       int valid = 0; //if 1 : validation completed and credit card is valid

       //variables for the validation percentages
       int num_valid = 0;
       int num_invalid = 0;

       int validation = 0; //for calculating randomly if credit card is valid
                   
              //random validation of credit card

              /* elegxetai h egkurothta ths kartas : 
              90 % twn clients exoun pithanotita na exoun eggurh karta 
              kai 10% twn clients na dwsoun akuri analoga 
              me to an epilegetai 1 h' 2 apo th random sunarthsh. */
              
              srand((unsigned int) getpid());

              validation=rand() %2 +1;

              switch(validation)
              {
                     case 1:
                            if(num_invalid++ <= 0.1*LISTENQ)
                            {    
                                   temp = 1;
                                   write(connfd, &temp, sizeof(int)); //send a value to client to print a message
                                   valid = 0;
                            }
                     case 2:
                            if(num_valid++ <= 0.9*LISTENQ)
                            {
                                   temp = 3;
                                   write(connfd, &temp, sizeof(int)); //send a value to client to print a message
                                   valid = 1;
                                   break;
                            }
                     default:
                            if(num_invalid++ <= 0.1*LISTENQ)
                            {      
                                   temp = 1;
                                   write(connfd, &temp, sizeof(int)); //send a value to client to print a message
                                   //clean_up(0);
                                   valid = 0;

                            }

              
              }//switch

       return valid;

}//reservation_function

int cost_calc(int *r)
{

       int cost; //cost of the reservation

       switch (r[1])
       {
              case 1:
                     cost = 50 * r[0];
                     if (*(data+4) < Z_A)
                     {
                           (*(data+4)) += r[0];
                           break; 
                     } 
                     else 
                     {
                            temp = 2;
                            write(connfd, &temp, sizeof(int)); //send a value to client to print a message
                     }               
                                                        
              case 2:
                     cost = 40 * r[0];
                     if(*(data+5) < Z_B)
                     {
                            (*(data+5)) += r[0];
                            break;  
                     } 
                     else 
                     {
                            temp = 2;
                            write(connfd, &temp, sizeof(int)); //send a value to client to print a message
                     }
                                   
              case 3:
                     cost = 35 * r[0];
                     if(*(data+6) < Z_C)
                     {
                            (*(data+6)) += r[0];  
                            break;
                     }
                     else
                     {
                            temp = 2;
                            write(connfd, &temp, sizeof(int)); //send a value to client to print a message 
                     }
                                   
              case 4:
                     cost = 30 * r[0];
                     if (*(data+7) < Z_D)
                     {
                            (*(data+7)) += r[0]; 
                            break;
                     } 
                     else
                     {
                            temp = 2;
                            write(connfd, &temp, sizeof(int)); //send a value to client to print a message
                     }
       }//switch  

      
       (*(data)) ++; // id pelath

       (*(data+2)) += cost; //add the cost to the company account


       (*(data+3)) += (*(data+2)) ; //transfer the money from company account to theater account

       //send these values to client
       
       temp = 4;
       write(connfd, &temp, sizeof(int)); //send a value to client to print a message

       write(connfd, &(*(data)), sizeof(int)); //send id pelath

       write(connfd, &(r[1]), sizeof(int)); //zone

       write(connfd, &cost, sizeof(int)); //cost

       return 1;          

}//cost_calc function

/* Add an element to the queue */
void queue_put(struct queue *q, int **v)
{
       struct list *p;

       p = (struct list *)malloc(sizeof(struct list));
       p->next = NULL;
       p->val = *v;

       if (q->tail != NULL)
              q->tail->next = p;   /* Add element to queue tail */
       else
              q->head = p;         /* Special case for empty queue */

       q->tail = p;
       return;
}


/* Remove and return an element from a non-empty queue */
int *queue_get(struct queue *q)
{
       
       struct list *tmp;

       assert(q->head != NULL);

       v = q->head->val;

       if (q->head->next == NULL)
              q->tail = NULL;             /* Special case for empty queue */
       tmp = q->head->next;
       free(q->head);
       q->head = tmp;
       return (v);
}





















