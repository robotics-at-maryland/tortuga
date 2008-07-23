#include <stdio.h>
#include <time.h>
#include "report.h"


int main(int argc, char ** argv)
{
    int fd = openDevice();
    if(fd < 0)
    {
        printf("Could not open device!\n");
        return -1;
    }

	while(1)
	{

    int ret = reportPing(fd, argc, -0.0039, 0.0, -0.98765,
                         43210, time(0), 0xDEADBEEF);

    printf("result was %d\n", ret);
		sleep(1);
	}
    closeDevice(fd);
    return 0;
}
