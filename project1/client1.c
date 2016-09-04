#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> /* basic system data types */
#include <sys/socket.h> /* basic socket definitions */
#include <errno.h> /* for the EINTR constant */
#include <sys/wait.h> /* for the waitpid() system call */
#include <sys/un.h> /* for Unix domain sockets */
#include <sys/ipc.h>
#include <time.h>

//header files for semaphores
#include <semaphore.h>
#include <sys/sem.h>
#include <fcntl.h>     
#include <sys/stat.h>     

//header file for signals
#include <signal.h> 

//header file me tis katallhles dhlwseis statherwn
#include "help.h" 


int main( int argc, char **argv )
{
	// VARIABLES

    int sockfd;
    struct sockaddr_un servaddr; /* Struct for the server socket address. */

    //boithitikes metablhtes
    int i,x,y,z,w,t,c;
    
    //number of tickets
    int num_tickets; 

    //zone of the theater
    int zone;

    //number of credit card
    int credit_card=1;

    //table for the percentage of choosing a zone
    int zoni[4]={0};

    

    //CONNECTION TO SERVER


    printf("\nstarting client\n");

    sockfd = socket( AF_LOCAL, SOCK_STREAM, 0 ); // Create the client's endpoint

    bzero( &servaddr, sizeof( servaddr ) ); // Zero all fields of servaddr

    servaddr.sun_family = AF_LOCAL; // Socket type is local (Unix Domain)

    strcpy( servaddr.sun_path, UNIXSTR_PATH ); // Define the name of this socket

    //Connect the client's and the server's endpoint
    connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

  //Analoga me argc ---> random H' apo plhktrologio  


if (argc > 1) //random
{                      

    t = 0;        

    write(sockfd, &t, sizeof(int)); //RANDOM

}//if
else
{
        //Apo plhktrologio

    //menou


    printf("kalwsirthate sto theatro!!!\n");
    printf("Posa eisitiria thelete??? \n"); 
    printf("1) Zoni A : 50 euro eisitirio \n"); 
    printf("2) Zoni B : 40 euro eisitirio \n"); 
    printf("3) Zoni C : 35 euro eisitirio \n ");
    printf("4) Zoni D : 30 euro eisitirio \n ");

        printf("Give number of tickets :::\n");
        scanf("%d", &num_tickets);

        printf("Give the zone of the theater you want :::\n");
        scanf("%d", &zone);

        printf("Give the number of your credit_card :::\n");
        scanf("%d", &credit_card);
            
    t = 1; 

    //SEND DATA TO SERVER

        write(sockfd, &t, sizeof(int)); //APO PLHKTROLOGIO
   
        //send the number of tickets
        //write(sockfd, &num_tickets, sizeof(int));
        if(write(sockfd, &num_tickets, sizeof(int)) > 0) printf("%d\n", num_tickets);

        //send the zone that the client chose
        //write(sockfd, &zone, sizeof(int));
        if(write(sockfd, &zone, sizeof(int)) > 0) printf("%d\n", zone);
    
        //send the number of the credit card
        //write(sockfd, &credit_card, sizeof(int));
        if(write(sockfd, &credit_card, sizeof(int)) > 0) printf("%d\n", credit_card);

}

    //READING MESSAGES FROM SERVER

        

    for(c = 0; c<10; c++) 
    {
        read( sockfd, &x , sizeof( int ) );  

        if(x == 10)
        {
            printf("Den uparxoun alles theseis\n");

            break; //END OF PROGRAMM
        } 

        switch(x)
        {
            case 1:
                printf("Your credit card is invalid\n");
                printf("Exiting..\n");
                printf("Thank you very much\n");
                break;
            case 2:
                printf("H zoni auth exei gemisei dustixws..\n");
                printf("Sas kratame tis idies theseis gia thn akrivws epomeni zoni!! \n");
                break;
            case 3:
                printf("Your credit card is valid\n");
                break;
            case 4:
                printf("Reservation Completed\n");
                read( sockfd, &y , sizeof( int )); //id_pelath
                read( sockfd, &z , sizeof( int )); //zone
                read( sockfd, &w , sizeof( int ));  //cost

                printf("Id_Pelath: %d\n", y);
                printf("Zone: %d\n", z);
                printf("Cost: %d\n", w);
                printf("Thank you\n");
                break;

        }//switch


    }

    exit(1);

    return 0;
}


