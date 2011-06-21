
#ifndef JOYSTICK_H
#define JOYSTICK_H

// Button mappings
#define BTN_INCSPEED  10
#define BTN_DECSPEED  13

#define BTN_TURNLEFT -1
#define BTN_TURNRIGHT -2

#define BTN_ASCEND  6
#define BTN_DESCEND 7

#define BTN_EMERGSTOP -3
#define BTN_ZEROSPEED 2

#define BTN_ARM_TORPEDO 1
#define BTN_ARM_MARKER 3
#define BTN_FIRE 0

#define AXIS_TSPEED 0
#define AXIS_SPEED 1
#define AXIS_YAW 2
#define AXIS_THROTTLE -1

int scaleAxis(int val, int negRange, int posRange, int offset, int outRange)
{
    val += offset;
    if(val < 0) /* Forward, range up to 15677 */
        val = outRange * val / negRange;
    else
        val = outRange * val / posRange;

    if(val > outRange)
        val = outRange;

    if(val < -outRange)
        val = -outRange;

    return val;
}

// Joystick Specific Range Calibration Values
#define SCALE_SPEED(val)                                \
    scaleAxis(val, -14000, -15000, 6811, SPEED_RANGE)
#define SCALE_YAW(val)                              \
    scaleAxis(val, 19275, 19532, 5269, YAW_RANGE)
#define SCALE_TSPEED(val)                       \
    scaleAxis(val, 15934, 17476, 10152, TSPEED_RANGE)
#define SCALE_THROTTLE(val)                     \
    scaleAxis(val, -19789, -19789, 8867, SPEED_RANGE)

#include "joystick.h"

#define INFO_MESSAGE                                                    \
    "Using Saitek mapping\n\tThumb L-Ascend\n\tThumb R-Descend\n\tPOV Hat - Fwd/Back, Turn\n\tZ trigger- Emergency stop\n\tButton under POV Hat - zer speed to 0\n"

#endif // JOYSTICK_H
