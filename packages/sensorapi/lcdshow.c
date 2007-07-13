#include <stdio.h>
#include <stdlib.h>
#include "include/sensorapi.h"

int main(int argc, char ** argv)
{
    if(argc < 2)
    {
        printf("\nlcdshow -t line1 line2\n");
        printf("\nlcdshow -c  (clear screen)\n");
        printf("\nlcdshow -bloff (backlight on)\n");
        printf("\nlcdshow -blon  (backlight off)\n");
        printf("\nlcdshow -blfl  (backlight flash)\n");
        printf("\nlcdshow -s  (start sequence)\n");

	return -1;
    }

    int fd = openSensorBoard("/dev/sensor");


    if(strcmp(argv[1], "-bloff") == 0)
    {
        lcdBacklight(fd, LCD_BL_OFF);
        close(fd);
        return 0;
    }

    if(strcmp(argv[1], "-blon") == 0)
    {
        lcdBacklight(fd, LCD_BL_ON);
        close(fd);
        return 0;
    }

    if(strcmp(argv[1], "-blfl") == 0)
    {
        lcdBacklight(fd, LCD_BL_FLASH);
        close(fd);
        return 0;
    }

    if(strcmp(argv[1], "-c") == 0)
    {
        displayText(fd, 0, "");
        displayText(fd, 1, "");
        close(fd);
        return 0;
    }



    if(strcmp(argv[1], "-s") == 0)
    {
    	displayText(fd, 1, "");
	displayText(fd, 0, "Ready to start");
	while((readStatus(fd) & STATUS_STARTSW) == 0);
	displayText(fd, 0, "Running...");
	close(fd);
	return 0;
    }


    if(strcmp(argv[1], "-t") == 0)
    {
        if(fd == -1)
        {
            printf("\nCould not find device!\n");
            close(fd);
            return -1;
        }

        if(syncBoard(fd) != 0)
        {
            printf("\nCould not sync with board!\n");
            close(fd);
        }

        displayText(fd, 0, argv[2]);

        if(argc == 4)
            displayText(fd, 1, argv[3]);
    }

    close(fd);
    return 0;
}

