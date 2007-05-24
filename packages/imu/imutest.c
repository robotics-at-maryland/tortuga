#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "imuapi.h"







int main(int argc, char ** argv)
{
    int fd = open("/dev/ttyUSB0", O_RDWR); // | O_NDELAY | O_NOCTTY);

    struct imuMeasurements imuData;

    while(1)
    {
      //  printf("\nWaiting for sync...\n");
        waitSync(fd);
        readIMUData(fd, &imuData);
        printf("%9f %9f %9f\n", imuData.accelX, imuData.accelY, imuData.accelZ);
//        printf("%11f %11f %11f\n", imuData.gyroX, imuData.gyroY, imuData.gyroZ);
//        printf("%9f\n", imuData.gyroX);
    }



    printf("\nIMU Test\n");
    return 0;
}
