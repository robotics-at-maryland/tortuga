#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>

#define MYPORT 9219 /*YOUR PORT HERE*/
                /* the port users will be connecting to */

#define BACKLOG 10  /* how many pending connections queue will hold */


/*
    Packet format:
    Control type (1 byte) (stick / button)
    Control number (1 byte) (stick number / button number)
    Value          (2 bytes signed) new value
*/

#define MAX_SPEED 5

#define CMD_NOTHING     0

#define CMD_TURNLEFT    1
#define CMD_TURNRIGHT   2

#define CMD_ASCEND      3
#define CMD_DESCEND     4

#define CMD_INCSPEED    5
#define CMD_DECSPEED    6

#define CMD_ZEROSPEED   7
#define CMD_EMERGSTOP   8

#define CMD_NOTHING     0

#define CMD_TURNLEFT    1
#define CMD_TURNRIGHT   2

#define CMD_ASCEND      3
#define CMD_DESCEND     4

#define CMD_INCSPEED    5
#define CMD_DECSPEED    6

#define CMD_ZEROSPEED   7
#define CMD_EMERGSTOP   8


#define CMD_SETSPEED    9

#define CMD_ANGLEYAW	10

void sigalrmHandler(int i)
{
    printf("\nKeep-alive timer expired!\n");
    system("lcdshow -safe");
    exit(1);
}

void runInput(int fd)
{
    signed char buf[100];
    unsigned char cmd = 0;
    signed char param=0;
    struct sockaddr_in rcv_addr;

    alarm(1);


    while(1)
    {
        int t = sizeof(rcv_addr);
        rcv_addr.sin_port = 0;
        rcv_addr.sin_addr.s_addr = INADDR_ANY;
        int len = recvfrom(fd, buf, 2, 0, (struct sockaddr *) &rcv_addr, &t);

        alarm(1);

        cmd = buf[0];
        param = buf[1];

        switch(cmd)
        {
            case CMD_EMERGSTOP:
            {
                printf("Emergency stop\n");
                break;
            }

            case CMD_TURNLEFT:
            {
                printf("Yaw left\n");
                break;
            }

            case CMD_TURNRIGHT:
            {
                printf("Yaw right\n");
                break;
            }

            case CMD_INCSPEED:
            {
                printf("Inc Speed\n");
                break;
            }

            case CMD_DECSPEED:
            {
                printf("Dec speed\n");
                break;
            }

            case CMD_DESCEND:
            {
                printf("Descend\n");
                break;
            }

            case CMD_ASCEND:
            {
                printf("Ascend\n");
                break;
            }

            case CMD_ZEROSPEED:
            {
                printf("Zero speed\n");
                break;
            }

            case CMD_SETSPEED:
            {
                printf("New speed: %d\n", param);
		break;
            }

            case CMD_ANGLEYAW:
	    {
		printf("Yawing: %d\n", param);
		break;
	    }

            case CMD_NOTHING:
            {
             //   printf("Keep-alive\n");

                break;
            }
        }
    }
}


main()
{
    unsigned char buf[65536];
    int sockfd, new_fd;/*listen on sock_fd, new connection on new_fd */
    struct sockaddr_in my_addr;    /*my address information */
    struct sockaddr_in their_addr; /*connector's address information */
    int sin_size;
    int opt_val = 1, ret;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("socket");
            exit(1);
    }

    memset((void *)&my_addr,0,sizeof(my_addr)); /*zero*/
    my_addr.sin_family = AF_INET;      /* host byte order */
    my_addr.sin_port = htons(MYPORT);  /* short, network byte order */
    my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */


    ret = bind(sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr));
    if (ret < 0)
    {
          perror("bind");
          return ret;
    }

    int t = sizeof(my_addr);
    my_addr.sin_port = 0;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    int len = recvfrom(sockfd, buf, 65536, 0, (struct sockaddr *) &my_addr, &t);

    printf("Recieved first packet.\n");

    signal(SIGALRM, sigalrmHandler);


    runInput(sockfd);
    close(sockfd);
    exit(0);

}


