#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define MYPORT 9219 /*YOUR PORT HERE*/
                /* the port users will be connecting to */

#define BACKLOG 10  /* how many pending connections queue will hold */


/*
    Packet format:
    Control type (1 byte) (stick / button)
    Control number (1 byte) (stick number / button number)
    Value          (2 bytes signed) new value
*/

void runInput(int fd)
{
    unsigned char buf[64];
    unsigned char type = 0;
    unsigned char num = 0;
    signed short val = 0;

    while(1)
    {
        if(recv(fd, &type, 1, 0) != 1)
            exit(1);

        recv(fd, &num, 1, 0);
        recv(fd, &val, 2, 0);   /* We're both Intel, so who cares about byte order */

        printf("%d %d: %d\n", type, num, val);
    }
}


main()
{
    int sockfd, new_fd;/*listen on sock_fd, new connection on new_fd */
    struct sockaddr_in my_addr;    /*my address information */
    struct sockaddr_in their_addr; /*connector's address information */
    int sin_size;
    int opt_val = 1, ret;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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

    ret = listen(sockfd, BACKLOG);
    if (ret < 0)
    {
        perror("listen");
        return ret;
    }



/* put the correct code segments HERE in the correct order */

    while(1)
    {  /* main accept() loop */
        sin_size = sizeof(struct sockaddr_in);
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd < 0) {
            perror("accept");
            continue;
        }
        printf("server: got connection\n");

        //if (!fork())
        {
            runInput(new_fd);
            exit(0);
        }
        close(new_fd);  /* parent doesn't need this */


        while(waitpid(-1,NULL,WNOHANG) > 0); /*clean up child proc*/
    }
}


