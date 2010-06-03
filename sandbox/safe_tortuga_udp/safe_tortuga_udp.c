#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <time.h>
#include <wait.h>

#define PORT_NUM (0xDEAF)
#define BUFF_SZ 256

void Die(char *mess) { perror(mess); exit(1); }

int main(int argc, char *argv[]) {
    int usock, err, received, temp;
    struct sockaddr_in b_addr;
    struct sockaddr_in c_addr;
    unsigned int c_addr_len;
    unsigned char buff[BUFF_SZ];
    fd_set sock_set;
    struct timeval timeout;
    pid_t child_pid;

    usock= socket(AF_INET, SOCK_DGRAM, 0); // Get a socket handle

    if(usock < 0) {
        printf("Error! Could not create socket!\n");
        return 1;
    }

    b_addr.sin_family= AF_INET;
    b_addr.sin_port= htons(PORT_NUM);
    b_addr.sin_addr.s_addr= INADDR_ANY;

    if((err= bind(usock, (struct sockaddr *) &b_addr,
                sizeof(struct sockaddr_in))) != 0) {
        printf("Could not bind to port %x; got err %x\n", PORT_NUM, err);
        return 2;
    }

    // Get some data
    c_addr_len= sizeof(struct sockaddr_in);

    printf("Listening on port %u\n", PORT_NUM);

    while(1) {
        received= (int)recvfrom(usock, buff, BUFF_SZ, 0,
                            (struct sockaddr *) &c_addr, &c_addr_len);

        if(received < 0) {
            printf("We received -1 bytes! OH NOEEEEEEEEEEEES!\n");
            return 3;
        } else if(received > 2) {
            printf("We did not get what we wanted! Got %d wanted 2.\n",
                    received);
            continue;
        }

        printf("I GOTS A PACKET!\n");

        if(buff[0] == 0xFA && buff[1] == 0xDE) {
            printf("WOO! It be the packets I wants!\n");

            temp= ntohl(c_addr.sin_addr.s_addr);
            printf("I gots it from this slut: %d.%d.%d.%d\n",
                    (temp >> 24) & 0xFF, (temp >> 16) & 0xFF,
                    (temp >> 8) & 0xFF, temp & 0xFF);

            printf("Tortuga is now on a timer.  If at any point\
 we stop getting packets, we will run pkill and\
 shut down the motors then kill the whole robot.\n");

            // Setup the fd_set to watch the udp socket
            FD_ZERO(&sock_set);
            FD_SET(usock, &sock_set);

            timeout.tv_sec= 3;
            timeout.tv_usec= 0;

            while(1) {
                if((temp= select(usock + 1, &sock_set, NULL, NULL, &timeout)) > 0) {
                    printf("Got a packet!\n");
                    received= (int)recvfrom(usock, buff, BUFF_SZ, 0,
                                           (struct sockaddr *) &c_addr, &c_addr_len);
                    if(received < 0) {
                        printf("Something went wrong... got 0 or less packets!\n");
                        break;
                    } else if(received > 2) {
                        printf("We did not get what we wanted! Got %d wanted 2.\n",
                                received);
                        continue;
                    }

                    if(buff[0] == 0xFA && buff[1] == 0xDE) {
                        printf("WOO! It be the packets I wants!\n");
                        timeout.tv_sec= 3;
                        timeout.tv_usec= 0;
                        continue;
                    }

                    printf("Got something weird, ignoring...\n");
                    continue;
                }

                printf("Failed with status %x\n", temp);
                printf("Executing \"%s\"...\n", argv[1]);

                if( (child_pid = fork()) == 0) {
                    // Child
                    setpgrp();
                    if(execvp(argv[1], argv + 1)) {
                        printf("Err! Could not execute");
                        exit(1);
                    }
                } else {
                    // Parent
                    waitpid(child_pid, NULL, 0);
                }
                break;
            }

            return 0;
        } else {
            printf("I got a dumb packet.  Ignoring...\n");
        }
    }

    return 0;
}
