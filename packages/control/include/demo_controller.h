
#ifndef DEMO_CONTROLLER_H
#define DEMO_CONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

// DesiredState = struct('speed',1,
//                       'depth',1,
//                       'quaternion',[1 2 3 4]',
//                       'angularRate',[1 2 3]');
typedef struct
{
    double speed;
    double depth;
    double quaternion[4];
    double angularRate[3];
} DesiredState;

// MeasuredState = struct('depth',1,
//                        'linearAcceleration',[1 2 3]',
//                        'quaternion',[1 2 3 4]',
//                        'angularRate',[1 2 3]');
typedef struct
{
    double depth;
    double linearAcceleration[3];
    double quaternion[4];
    double angularRate[3];
} MeasuredState;

// ControllerState = struct('inertiaEstimate',[1 2 3 4 5 6]',
//                          'adaptationGain',[1 0 0; 0 1 0; 0 0 1],
//                          'angularGain',[1 2 3; 4 5 6; 7 8 9],
//                          'depthPGain',1,
//                          'speedPGain',1);
typedef struct
{
    double inertiaEstimate[6];
    double adaptationGain[3][3];
    double angularGain[3][3];
    double depthPGain;
    double speedPGain;
} ControllerState;

void translationalController(MeasuredState* measuredState,
                             DesiredState* desiredState,
                             ControllerState* controllerState,
                             double dt,
                             double* translationalForces);

void testMeasured(MeasuredState* measuredState);
void testDesired(DesiredState* desiredState);
void testController(ControllerState* controllerState);


typedef struct
{
    double depth;
    double quaternion[4];
} TestStruct;

void testStruct(TestStruct* test);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // DEMO_CONTROLLER_H
