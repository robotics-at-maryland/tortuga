
#ifndef JOYSTICK_H
#define JOYSTICK_H

/* Button mappings for Steve's Xbox controller and kernel */
#define BTN_INCSPEED    2
#define BTN_DECSPEED    3
#define BTN_TURNLEFT    8
#define BTN_TURNRIGHT   6

#define BTN_ASCEND      9
#define BTN_DESCEND     5

#define BTN_EMERGSTOP   11
#define BTN_ZEROSPEED   4

#define BTN_ARM_TORPEDO -1
#define BTN_ARM_MARKER  -2
#define BTN_FIRE        -3

#define AXIS_SPEED      1

void processAxis(int fd, int axis, int val)
{
    switch(axis)
    {
    case AXIS_SPEED:
    {
        val = SPEED_RANGE * val / 32767;
        if(val != lastAxisSpeed)
        {
            printf("New speed: %d\n", val);
            lastAxisSpeed = val;
            sendCmd(fd, CMD_SETSPEED, val);
        }
        break;
    }

    };
}

#define INFO_MESSAGE                            \
    "Using XBox mapping\n"

#endif // JOYSTICK_H
