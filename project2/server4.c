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
#include <time.h>
   
#include <pthread.h>

//header file for signals
#include <signal.h>                                                          

//header file me tis katallhles dhlwseis statherwn
#include "helpp.h" 

//clean_up function: close socket, shared memory, mutexes and finally the program

void clean_up(int cond){

       int i;
       printf("server exiting now!!!!!\n");
       close(connfd); //Parent closes connected socket
       close(listenfd); //Close listening socket
       shmctl(shmid,IPC_RMID, NULL);
       pthread_attr_destroy(&attr);
       for (i = 0; i < NTHL; i++)
       {
         pthread_mutex_destroy(&mutex_tel[i]);
       }

       for (i = 0; i < NBANK; i++)
       {
         pthread_mutex_destroy(&mutex_b[i]);
       }

       pthread_mutex_destroy(&mutex1);
       pthread_mutex_destroy(&mutex2);
       pthread_mutex_destroy(&mutex3);
       pthread_mutex_destroy(&mutex4);
       pthread_mutex_destroy(&mutex5);
       
       pthread_exit(NULL);
       exit(cond);
}


//SIGINT handler
void catch_int(int sig_num)
{

       int i;

       printf("server exiting now!!!!!\n");
       close(connfd); //Parent closes connected socket
       close(listenfd); //Close listening socket
       shmctl(shmid,IPC_RMID, NULL);
       pthread_attr_destroy(&attr);
       for (i = 0; i < NTHL; i++)
       {
         pthread_mutex_destroy(&mutex_tel[i]);
       }

       for (i = 0; i < NBANK; i++)
       {
         pthread_mutex_destroy(&mutex_b[i]);
       }

       pthread_mutex_destroy(&mutex1);
       pthread_mutex_destroy(&mutex2);
       pthread_mutex_destroy(&mutex3);
       pthread_mutex_destroy(&mutex4);
       pthread_mutex_destroy(&mutex5);
       pthread_exit(NULL);
       fflush(stdout);
       exit(1);

}



/* MAIN FUNCTION */

int main( int argc, char **argv )
{

       // VARIABLES

       int i, rc;

       void *status;
     
       //FOR SOCKETS

       socklen_t clilen; // length of socket
       
       struct sockaddr_un cliaddr, servaddr; //Structs for the client and server socket addresses.


       //Signal handler gia SIGINT
       signal( SIGINT, catch_int); 


       // initialize and set the thread attributes
       pthread_attr_init( &attr );
       pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );

       for (i = 0; i < NTHL; ++i)
       {
          pthread_mutex_init(&mutex_tel[i], NULL);
       }

       for (i = 0; i < NBANK; ++i)
       {
          pthread_mutex_init(&mutex_b[i], NULL);
       }


       pthread_mutex_init(&mutex1, NULL);
       pthread_mutex_init(&mutex2, NULL);
       pthread_mutex_init(&mutex3, NULL);
       pthread_mutex_init(&mutex4, NULL);
       pthread_mutex_init(&mutex5, NULL);

       p = reservation; // pointer ston pinaka reservation 


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

              //thread for accepting a client's request
              pthread_create(&thread[0], &attr, (void*) accept_client, NULL);


              if(*(data+8) > LISTENQ - 1) //if the server has no other requests
              {

                  temp = 10;
                  write(connfd, &temp, sizeof(int)); //send a value to client to print a message

                  /* Free attribute and wait for the threads */ 
                  pthread_attr_destroy(&attr);
                  rc = pthread_join(thread[0], &status);

                  if (rc) 
                  { 
                    printf("ERROR;return code from pthread_join() is %d\n", rc);
                    exit(-1);
                  } 
                  printf("Completed join with thread %d status= %ld\n",0, (long)status);
  
                  printf("break\n");

                  break;
              } 
              

       }//infinite loop: waiting a request from a client  

       
       printf("Money :::: %d\n", *(data+2) );  

       pthread_exit(NULL);

       clean_up(1);

       return 0;
}

void *accept_client ()
{

  int i, j, t, rc;

  void *status;

             //RECEIVE DATA FROM CLIENT

             read(connfd, &tmp, sizeof(int));

              if(tmp == 1)
              {


              //read the number of the tickets
              if((read(connfd, &reservation[0], sizeof(int))) > 0) printf("read number of tickets: %d\n\n", reservation[0]);       


              //read the zone of the theater             
              if((read(connfd, &reservation[1], sizeof(int))) > 0) printf("read zone: %d\n\n", reservation[1]); 


              //read the number of the credit card;
              if((read(connfd, &reservation[2], sizeof(int))) > 0) printf("read credit card: %d\n\n", reservation[2]); 


              }     

              
    //RANDOM      
              
    //random number of tickets          
    //srand((unsigned int) getpid());

    srand((unsigned) time(&t));
    reservation[0] = rand() % 4 + 1;



//random zone

/* apo tous 100 clients 10% apo autous tha dialeksoun rand() th zoni A, 
20% apo autous tha dialeksoun tuxaia th zoni B , 
30% apo autous tha dialeksoun th zoni C 
kai 40% apo autous tha dialeksoun th zoni D. 
Se mia sunartisi rand() epilegetai enas arithmos apo to 1-4 kai analoga me ton arithmo mesw ths switch epilegetai i zoni */

  
    srand((unsigned) time(&t));
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

            //lock mutex for one thread
            pthread_mutex_lock(&mutex1); 

            (*(data+8))++; //increase the number of clients

            printf("%d\n", *(data+8)); //print client's id
              
            //unlock mutex 
            pthread_mutex_unlock(&mutex1); 
              
            
            for (i = 0; i < NTHL; i++)
            {
                if(!telephonists[i])
                {

                      //lock mutex of this telephonist
                      pthread_mutex_lock(&mutex_tel[i]);

                      telephonists[i] = 1;//this telephonist is no longer available

                      printf("prin get\n");
                                   

                      res = queue_get(&q_res); //pairnoume thn krathsh tou pelath pou einai prwtos sthn oura
                                              //kai thn afairoume ap thn queue

                      printf("meta get\n");

                      
                      for(j=0; j<NBANK; j++)
                      {

  
                          if(!bank[j])
                          {

                              pthread_mutex_lock(&mutex_b[j]);

                              bank[j] = 1; //bank machine not available


                              //thread for validating the card
                              pthread_create(&thread[1], &attr, validation_function, NULL); 

                            
                              //lock mutex2
                              pthread_mutex_lock(&mutex2);


                                   
                                   if(*(data+9) == 1) //if the credit card is valid
                                   {
                                          
                                      *(data+9) = 0; // again not valid for next client
                                      bank[j] = 0; //this bank machine is now available

                                      //unlock mutex2
                                      pthread_mutex_unlock(&mutex2);

                                      //thread for calculating the cost
                                      pthread_create(&thread[2], &attr, cost_calc, NULL);

                                   }
                            
                                   else //if the credit card is not valid
                                   {      
                                          pthread_mutex_unlock(&mutex2);
                                   }


                          }//if bank[j]==0

                          else j=0; //looking again for free bank machine


                          
                          pthread_mutex_unlock(&mutex_b[j]);

                      }//for bank machines
                            
                            //lock mutex3
                            pthread_mutex_lock(&mutex3);

                            if(*(data+10) == 1) //if reservation is ready
                            {

                                    *(data+10) = 0; //reservation not ready for next client

                                    telephonists[i] = 0;//this telephonist is now available

                                    pthread_mutex_unlock(&mutex3);

                                    
                                
                                  for(t=1; t<NUM_THREADS - 1; t++) 
                                  { 
                                      rc = pthread_join(thread[t], &status);
                                      if (rc) 
                                      { 
                                        printf("ERROR;return code from pthread_join() is %d\n", rc);
                                        clean_up(1);
                                      } 
                                      printf("Completed join with thread %d status= %ld\n",t, (long)status);
                                  }
                                
                                    //pthread_exit(NULL); //termatizoume to nhma
                                  
                            }

                            //unlock mutex3
                            pthread_mutex_unlock(&mutex3);

                            pthread_mutex_unlock(&mutex_tel[i]);
                            break;

                }//if telephonists[i] == 0

                else i=0; //look for an available telephonist again

                break;

              }//for gia tous thlefwnhtes

              return;

}//accept_client function


void * validation_function()
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
              
              srand((unsigned) time(&t));

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

      //lock mutex3
      pthread_mutex_lock(&mutex3);

      *(data+9) = valid;

      //unlock mutex3
      pthread_mutex_unlock(&mutex3);

      return;

}//validation_function

void * cost_calc()
{

       int cost; //cost of the reservation

       //lock mutex5
       pthread_mutex_lock(&mutex5);

       switch (res[1])
       {
              case 1:
                     cost = 50 * res[0];
                     if (*(data+4) < Z_A)
                     {
                           (*(data+4)) += res[0];
                           break; 
                     } 
                     else 
                     {
                            temp = 2;
                            write(connfd, &temp, sizeof(int)); //send a value to client to print a message
                     }               
                                                        
              case 2:
                     cost = 40 * res[0];
                     if(*(data+5) < Z_B)
                     {
                            (*(data+5)) += res[0];
                            break;  
                     } 
                     else 
                     {
                            temp = 2;
                            write(connfd, &temp, sizeof(int)); //send a value to client to print a message
                     }
                                   
              case 3:
                     cost = 35 * res[0];
                     if(*(data+6) < Z_C)
                     {
                            (*(data+6)) += res[0];  
                            break;
                     }
                     else
                     {
                            temp = 2;
                            write(connfd, &temp, sizeof(int)); //send a value to client to print a message 
                     }
                                   
              case 4:
                     cost = 30 * res[0];
                     if (*(data+7) < Z_D)
                     {
                            (*(data+7)) += res[0]; 
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

       write(connfd, &(res[1]), sizeof(int)); //zone

       write(connfd, &cost, sizeof(int)); //cost


       *(data+10) = 1;

       //unlock mutex5
       pthread_mutex_unlock(&mutex5);

       return;          

}//cost_calc function

/* Add an element to the queue */
void queue_put(struct queue *q, int **v)
{
       struct list *p;

       p = (struct list *)malloc(sizeof(struct list));
       p->next = NULL;
       p->val = *v;

       if (q->tail != NULL){
              q->tail->next = p;   /* Add element to queue tail */
              printf("if\n");
        }
       else {
              q->head = p;         /* Special case for empty queue */
              printf("else\n");
        }
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





















