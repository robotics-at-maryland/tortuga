/****************send_packet.c******************/
/* Header files needed to use the sockets API. */
/* File contain Macro, Data Type and Structure */
/* definitions along with Function prototypes. */
/***********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#define SERVER "127.0.0.1"
#define SERVPORT 3794

int main(int argc, char *argv[])
{
    /* Variable and structure definitions. */
    int sd, rc;
    struct sockaddr_in serveraddr, clientaddr;
    int serveraddrlen = sizeof(serveraddr);
    char server[255];
    char buffer[4096];
    char *bufptr = buffer;
    int buflen = sizeof(buffer);
    struct hostent *hostp;
    memset(buffer, 0x00, sizeof(buffer));

    /* 36 characters + terminating NULL */
    int len = 0;
    while(!feof(stdin))
    {
        len += fread(buffer+len, 1, 1, stdin);
    }


    if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("UDP Client - socket() error");
        /* Just exit lol! */
        exit(-1);
    } else
        printf("UDP Client - socket() is OK!\n");

    /* If the hostname/IP of the server is supplied */
    /* Or if(argc = 2) */
    if(argc > 1)
       strcpy(server, argv[1]);
    else
    {
        /*Use default hostname or IP*/
        printf("UDP Client - Usage %s <Server hostname or IP>\n", argv[0]);
        printf("UDP Client - Using default hostname/IP!\n");
        strcpy(server, SERVER);
    }

    memset(&serveraddr, 0x00, sizeof(struct sockaddr_in));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERVPORT);

    if((serveraddr.sin_addr.s_addr = inet_addr(server)) == (unsigned long)INADDR_NONE)
    {
        /* Use the gethostbyname() function to retrieve */
        /* the address of the host server if the system */
        /* passed the host name of the server as a parameter. */
        /************************************************/

        /* get server address */
        hostp = gethostbyname(server);

        if(hostp == (struct hostent *)NULL)
        {
            printf("HOST NOT FOUND --> ");
            /* h_errno is usually defined */
            /* in netdb.h */
            printf("h_errno = %d\n", h_errno);
            exit(-1);
        } else
        {
            printf("UDP Client - gethostname() of the server is OK... \n");
            printf("Connected to UDP server %s on port %d.\n", server, SERVPORT);
        }

        memcpy(&serveraddr.sin_addr, hostp->h_addr, sizeof(serveraddr.sin_addr));
    }


    /* send request to server */
    rc = sendto(sd, bufptr, buflen, 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

    if(rc < 0)
    {
        perror("UDP Client - sendto() error");
        close(sd);
        exit(-1);
    } else
        printf("UDP Client - sendto() is OK!\n");

    close(sd);
    exit(0);
}
