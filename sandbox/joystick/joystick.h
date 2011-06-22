
void processAxis(int fd, int axis, int val)
{
    switch(axis)
    {
    case AXIS_SPEED:
    {
        val = SCALE_SPEED(val);

        if(val != lastAxisSpeed)
        {
            printf("New speed: %d\n", val);
            lastAxisSpeed = val;
            sendCmd(fd, CMD_SETSPEED, val);
        }
        break;
    }


    case AXIS_YAW:
    {
        val = SCALE_YAW(val);
        if(val == 1 || val == -1)
            val = 0;
        yawCmd = -val;

        if(val != lastAxisYaw)
        {
            printf("New Yaw: %d\n", val);
            lastAxisYaw = val;
        }

        break;
    }


    case AXIS_TSPEED:
    {
        val = SCALE_TSPEED(val);
//            rollCmd = val;

        if(val != lastAxisTSpeed)
        {
            printf("New tspeed: %d\n", val);
            lastAxisTSpeed = val;
            sendCmd(fd, CMD_TSETSPEED, val);
        }

        break;
    }

    case AXIS_THROTTLE:
    {
        val = SCALE_THROTTLE(val);

        if(val != lastThrottleSpeed)
        {
            printf("New speed: %d\n", val);
            lastThrottleSpeed = val;
            sendCmd(fd, CMD_SETSPEED, val);
        }
        break;
    }
    }
}
