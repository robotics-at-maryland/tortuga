#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// UNIX Includes
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#define BUFSZ 512

int openPort(const char* devName);

int main(int argc, char *argv[])
{
    int i= 1, fd= -1;
    int param;
    unsigned char outbuf[4];
    ssize_t ret, wrote= 0, read= 0;

    if(argc < 3 || argc > 4) {
        fprintf(stdout, "Usage: %s /path/to/serialPort command [parameter]\n\n"
                "Example: %s /dev/ttyUSB0 void 1\n\n"
                "Commands:\n"
                "\t* void pairnum\n"
                "\t* fill pairnum\n"
                "\t* off pairnum\n"
                "\t* ext piston\n"
                "\t* ret piston\n"
                "\t* voidall\n"
                "\t* offall\n"
                "\t* init\n",
                argv[0], argv[0]);
        return 0;
    }

    // Process input arguments from commandline
    if((fd= openPort(argv[i])) < 0) {
        fprintf(stderr, "Failed to open port\"%s\"\n", argv[i]);
        exit(-1);
    }

    ++i;
    if(strcmp("void", argv[i]) == 0) {
        outbuf[0]= 'V';
        outbuf[1]= 'O';

        param= 1;
    } else if(strcmp("fill", argv[i]) == 0) {
        outbuf[0]= 'F';
        outbuf[1]= 'L';

        param= 1;
    } else if(strcmp("off", argv[i]) == 0) {
        outbuf[0]= 'T';
        outbuf[1]= 'O';

        param= 1;
    } else if(strcmp("ext", argv[i]) == 0) {
        outbuf[0]= 'E';
        outbuf[1]= 'X';

        param= 1;
    } else if(strcmp("ret", argv[i]) == 0) {
        outbuf[0]= 'R';
        outbuf[1]= 'T';

        param= 1;
    } else if(strcmp("voidall", argv[i]) == 0) {
        outbuf[0]= 'V';
        outbuf[1]= 'A';
        outbuf[2]= '!';

        param= 0;
    } else if(strcmp("offall", argv[i]) == 0) {
        outbuf[0]= 'O';
        outbuf[1]= 'F';
        outbuf[2]= 'F';

        param= 0;
    } else if(strcmp("init", argv[i]) == 0) {
        outbuf[0]= 0xDE;
        outbuf[1]= 0xAF;
        outbuf[2]= 0xBE;
        outbuf[3]= 0xEF;

        param= 2;
    } else {
        fprintf(stderr, "Unknown command \"%s\"\n", argv[i]);

        param= -1;
    }

    // read in the parameter
    if(param == 1) {
        ++i;
        outbuf[2]= atoi(argv[i]);
    }

    // if we need to, calculate the checksum
    if(param == 0 || param == 1)
        outbuf[3]= outbuf[0] + outbuf[1] + outbuf[2];

    // if ther is no error...
    if(param >= 0) {
        // write out the data
        while(wrote < 4 && (ret= write(fd, outbuf + wrote, 4 - wrote)) > 0)
            wrote+= ret;

        if(param != 2) {
            while(read < 1 && (ret= read(fd, outbuf + read, 1 - read)) > 0)
                read+= ret;

            if(outbuf[0] == 'N') {
                param= -2;

                while(read < 4 && (ret= read(fd, outbuf + read, 4 - read)) > 0)
                    read+= ret;

                if(strncmp("PAR", outbuf + 1, 3) == 0) {
                    fprintf(stderr, "Parameter error\n");
                } else if(strncmp("CHK", outbuf + 1, 3) == 0) {
                    fprintf(stderr, "Checksum error!\n");
                } else {
                    fprintf(stderr, "Unknown error \"%c%c%c\"\n",
                            outbuf[1], outbuf[2], outbuf[3]);
                }
            }
        }
    }

    close(fd);
    return 0;
}

/* Some code from cutecom, which in turn may have come from minicom */
int openPort(const char* devName)
{
   int fd= open(devName, O_RDWR, O_ASYNC);

    if(fd == -1)
        return -1;

    struct termios newtio;
    if(tcgetattr(fd, &newtio) != 0)
        printf("\nFirst stuff failed\n");

    unsigned int _baud= B4800;
    cfsetospeed(&newtio, _baud);
    cfsetispeed(&newtio, _baud);


    newtio.c_cflag= (newtio.c_cflag & ~CSIZE) | CS8;
    newtio.c_cflag|= CLOCAL | CREAD;
    newtio.c_cflag&= ~(PARENB | PARODD);

    newtio.c_cflag&= ~CRTSCTS;
    newtio.c_cflag&= ~CSTOPB;

    newtio.c_iflag= IGNBRK;
    newtio.c_iflag&= ~(IXON|IXOFF|IXANY);

    newtio.c_lflag= 0;

    newtio.c_oflag= 0;


    newtio.c_cc[VTIME]= 1;
    newtio.c_cc[VMIN]= 60;

//   tcflush(m_fd, TCIFLUSH);
    if(tcsetattr(fd, TCSANOW, &newtio) != 0)
        printf("tsetaddr1 failed!\n");

    int mcs= 0;

    ioctl(fd, TIOCMGET, &mcs);
    mcs|= TIOCM_RTS;
    ioctl(fd, TIOCMSET, &mcs);

    if(tcgetattr(fd, &newtio) != 0)
        printf("tcgetattr() 4 failed\n");

    newtio.c_cflag&= ~CRTSCTS;

    if(tcsetattr(fd, TCSANOW, &newtio) != 0)
      printf("tcsetattr() 2 failed\n");
    
    return fd;
}

