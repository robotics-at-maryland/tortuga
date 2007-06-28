
#include "demo_controller.h"

void translationalController(MeasuredState* measuredState,
                             DesiredState* desiredState,
                             ControllerState* controllerState,
                             double dt,
                             double* translationalForces)
{
    translationalForces[0] = 1;
    translationalForces[1] = 1;
    translationalForces[2] = 1;
}

void testMeasured(MeasuredState* measuredState)
{
    measuredState->depth = 20;

    for (int i = 0; i < 3; ++i)
        measuredState->linearAcceleration[i] = i * 2;
    for (int i = 0; i < 4; ++i)
        measuredState->quaternion[i] = i * 8;
    for (int i = 0; i < 3; ++i)
        measuredState->angularRate[i] = i * 3;
}

void testDesired(DesiredState* desiredState)
{
    desiredState->speed = 5;
    desiredState->depth = 20;

    for (int i = 0; i < 4; ++i)
        desiredState->quaternion[i] = i * 10;
    for (int i = 0; i < 3; ++i)
        desiredState->angularRate[i] = i * 5;
}

void testController(ControllerState* controllerState)
{
    for (int i = 0; i < 6; ++i)
        controllerState->inertiaEstimate[0] = i * 4;

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
            controllerState->adaptationGain[i][j] = i * j * 3;
    }

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
            controllerState->angularGain[i][j] = i * j * 17;
    }

    controllerState->depthPGain = 87;
    controllerState->speedPGain = 26;
}

void testStruct(TestStruct* test)
{
    test->depth = 5;
    for (int i = 0; i < 4; ++i)
        test->quaternion[i] = i * 10;
}
