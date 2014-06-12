#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

#include "serial.h"

static int fd;

int readByte()
{
	unsigned char buf[1];
	int i, s = 0;
	for (i=0; i<3; i++) {
		s = read(fd, buf, 1);
		if (s && buf[0] != 0xFF)
			break;
		usleep(5);
	}

	if (s) {
		i = buf[0];
		//printf("read: %X\n", i);
		return i;
	}
	//printf("failed read\n");
	return -1;
}

void writeByte(char byte)
{
	//printf("wrote: %x\n", (int)byte);
	write(fd, &byte, 1);
}

int initSerial(char* loc)
{
	struct termios options;
	printf("Opening port %s... \n", loc);
	fd = open(loc, O_RDWR | O_NOCTTY);
	if (fd == -1) {
		perror("Unable to open port ");
		return 1;
	}
	
	fcntl(fd, F_SETFL, 0);
		
	// Get the current options for the port...
	tcgetattr(fd, &options);
	// Set the baud rates to 115200
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);
	// Enable the receiver and set local mode...
	options.c_cflag |= (CLOCAL | CREAD);

	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;

	options.c_cc[VMIN] = 0; //Character number blocking disabled
	options.c_cc[VTIME] = 5;

	// Set the new options for the port...
	tcsetattr(fd, TCSANOW, &options);
	return 0;
}

void closeSerial(void)
{
	printf("Closing port...\n");
	close(fd);
}

void hcsleep(unsigned long tme) {
	usleep(tme);
}
