/*

	Capertech Autopilot Version 1.0
	
	Servo.h
	
	Handles control of the Output Compare PWM pins
	
*/

#ifndef SERVO_H
#define SERVO_H

#include <p30fxxxx.h>

#include "typedef.h"

////////////////////////////////////
//GLOBAL DEVICE PARAMTERS

#define DEV_FOSC	(10000000)
#define DEV_FCY		(DEV_FOSC/4)
#define DEV_TCY		(4/(float)DEV_FOSC)



// SERVO OUTPUT POLARITY
// (enable to make servo outputs active low
#define PWM_INVERT

// SERVO CHANNEL SELECTION
// disable if OC channel is not implemented or not used as a servo
#define SERVO_USE_OC1
#define SERVO_USE_OC2
#define SERVO_USE_OC3
#define SERVO_USE_OC4
#define SERVO_USE_OC5
#define SERVO_USE_OC6
#define SERVO_USE_OC7
#define SERVO_USE_OC8


// Determine buffer size
#if   defined SERVO_USE_OC8
	#define NUM_SERVOS	8
#elif defined SERVO_USE_OC7
	#define NUM_SERVOS	7
#elif defined SERVO_USE_OC6
	#define NUM_SERVOS	6
#elif defined SERVO_USE_OC5
	#define NUM_SERVOS	5
#elif defined SERVO_USE_OC4
	#define NUM_SERVOS	4
#elif defined SERVO_USE_OC3
	#define NUM_SERVOS	3
#elif defined SERVO_USE_OC2
	#define NUM_SERVOS	2
#elif defined SERVO_USE_OC1
	#define NUM_SERVOS	1
#else
	#define NUM_SERVOS	0
	#warning No Servo channels selected. Module disabled.
#endif


// Timer2 period value
// 22ms Refresh rate
#define PWM_PERIOD 		        (WORD16)(0.022/(DEV_TCY))

#define PWM_CLICK_TIME			(DEV_TCY)

// allow 333 us delay at beginning
// alot each servo 2.6ms
// min is 0.5ms
// center is 1.5ms
// max is 2.5ms

#define SERVO_DELTA_us			(float)0.0026
#define SERVO_CENTER_us			(float)0.0015
#define SERVO_MAX_us			(float)0.0025
#define SERVO_MIN_us			(float)0.0005
#define SERVO_COUNTS_PER_us		(0.000001/PWM_CLICK_TIME)


// Servo pulse allignment
// Time when first pulse starts
#define PWM_PULSE_ORIGIN		(WORD16)(0.001/PWM_CLICK_TIME)
// Time between pulse starts
#define PWM_PULSE_DELTA			(WORD16)(SERVO_DELTA_us/PWM_CLICK_TIME)
// Length of pulse at min position
#define PWM_PULSE_ZERO			(WORD16)(SERVO_MIN_us/PWM_CLICK_TIME)
// Command pulse at center position
#define PWM_PULSE_CENTER		(WORD16)(SERVO_CENTER_us/PWM_CLICK_TIME)
// Command pulse at max position
#define PWM_MAX_POSITION		(WORD16)(SERVO_MAX_us/PWM_CLICK_TIME)

#define SERVO_CENTER			PWM_PULSE_CENTER
#define SERVO_MAX				PWM_MAX_POSITION
#define SERVO_MIN				PWM_PULSE_ZERO




// Initializes PWM timer and enables all 8 output channels
void InitServos(void);

// Updates servo pulse values after all pulses have completed
#ifdef SERVO_USE_OC1
void _ISR _OC1Interrupt(void);
#endif
#ifdef SERVO_USE_OC2
void _ISR _OC2Interrupt(void);
#endif
#ifdef SERVO_USE_OC3
void _ISR _OC3Interrupt(void);
#endif
#ifdef SERVO_USE_OC4
void _ISR _OC4Interrupt(void);
#endif
#ifdef SERVO_USE_OC5
void _ISR _OC5Interrupt(void);
#endif
#ifdef SERVO_USE_OC6
void _ISR _OC6Interrupt(void);
#endif
#ifdef SERVO_USE_OC7
void _ISR _OC7Interrupt(void);
#endif
#ifdef SERVO_USE_OC8
void _ISR _OC8Interrupt(void);
#endif

// Set the given channel to the given position
// Channel = 0:7
// Position = 0:PWM_MAX_POSITION-1
void SetServo( BYTE Channel, WORD16 Position );

void EnableServo( BYTE Channel );
void DisableServo( BYTE Channel );

// Returns the servo's current position
WORD16 GetServo( BYTE Channel );





#endif

