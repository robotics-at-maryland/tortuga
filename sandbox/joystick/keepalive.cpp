
#include <iostream>
#include <vector>
#include <sstream>


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>


#define PORT 9219


#define CMD_NOTHING     0
#define CMD_ANGLEYAW    10


int sockfd=0;

struct sockaddr_in their_addr; /*connector's address information */


void sendCmd(int fd, unsigned char cmd, signed char param)
{
    signed char buf[2];
    buf[0]=cmd;
    buf[1]=param;

    if(sendto(fd, buf, 2, 0, (struct sockaddr *) &their_addr, sizeof(struct sockaddr_in)) == -1)
    {
        printf("Cant send\n");
    }
}

int main(int argc, char ** argv)
{
    int yawTime = 0;

    if(argc < 2)
    {
        printf("\nNeed hostname\n");
        return -1;
    }


    /* Blatant copy and paste job from 417 */
    int numbytes;
    struct hostent *he;



    if ((he=gethostbyname(argv[1])) == NULL)
    {  /* get the host info */
           herror("gethostbyname");
           exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }

    memset((void *)&their_addr, 0, sizeof(their_addr));/*zero*/
    their_addr.sin_family = AF_INET;    /* host byte order */
    their_addr.sin_port = htons(PORT);  /* short, network byte order */
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);


    printf("\nSocket FD is %d\n", sockfd);
    printf("Running...\n");

	while(1)
	{
		usleep( 250 );

        sendCmd(sockfd, CMD_NOTHING, 0);    /* Ping! */
// 		sendCmd(sockfd, CMD_ANGLEYAW, 0);   /* No yaw angle, if we expect it */
	}
    return 0;
}

