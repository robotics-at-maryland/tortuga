/*

	Capertech Autopilot Version 1.0
	
	Servo.h
	
	Handles control of the Output Compare PWM pins
	
*/

#include "Servo.h"

#if NUM_SERVOS != 0

#warning SERVO MODULE ACTIVE

// Module-level variables
WORD16 ServoPositions[NUM_SERVOS];


/* Turns on pulses to the desired servo */
void EnableServo( BYTE Channel )
{
	switch( Channel ) {
#ifdef SERVO_USE_OC1
		case 0:
#endif
#ifdef SERVO_USE_OC2
		case 1:
#endif
#ifdef SERVO_USE_OC3
		case 2:
#endif
#ifdef SERVO_USE_OC4
		case 3:
#endif
#ifdef SERVO_USE_OC5
		case 4:
#endif
#ifdef SERVO_USE_OC6
		case 5:
#endif
#ifdef SERVO_USE_OC7
		case 6:
#endif
#ifdef SERVO_USE_OC8
		case 7:
#endif
			// Set the bits in the appropriate OCxCON register
			// (uses pointers to generate address rather than 
			//   hard-coded cases)
			*((&OC1CON)+3*Channel) |= 0x0005;
			TRISD &= 0xFFFE<<Channel;
			break;
	}

}

/* Turns off pulses to the desired servo */
void DisableServo( BYTE Channel )
{
	switch( Channel ) {
#ifdef SERVO_USE_OC1
		case 0:
#endif
#ifdef SERVO_USE_OC2
		case 1:
#endif
#ifdef SERVO_USE_OC3
		case 2:
#endif
#ifdef SERVO_USE_OC4
		case 3:
#endif
#ifdef SERVO_USE_OC5
		case 4:
#endif
#ifdef SERVO_USE_OC6
		case 5:
#endif
#ifdef SERVO_USE_OC7
		case 6:
#endif
#ifdef SERVO_USE_OC8
		case 7:
#endif
			*((&OC1CON)+3*Channel) &= 0xFFF8;
			TRISD |= 0x0001<<Channel;
			break;
	}

}

/* Returns the current command value for the desired servo */
WORD16 GetServo( BYTE Channel )
{
	switch( Channel ) {
#ifdef SERVO_USE_OC1
		case 0:
#endif
#ifdef SERVO_USE_OC2
		case 1:
#endif
#ifdef SERVO_USE_OC3
		case 2:
#endif
#ifdef SERVO_USE_OC4
		case 3:
#endif
#ifdef SERVO_USE_OC5
		case 4:
#endif
#ifdef SERVO_USE_OC6
		case 5:
#endif
#ifdef SERVO_USE_OC7
		case 6:
#endif
#ifdef SERVO_USE_OC8
		case 7:
#endif
			return ServoPositions[Channel];

		default:
			return 0xFFFF;
	}
}


// Set the given channel to the given position
// Channel = 0:7
// Position = 0:PWM_MAX_POSITION-1
void SetServo( BYTE Channel, WORD16 Position )
{
	switch( Channel ) {
#ifdef SERVO_USE_OC1
		case 0:
#endif
#ifdef SERVO_USE_OC2
		case 1:
#endif
#ifdef SERVO_USE_OC3
		case 2:
#endif
#ifdef SERVO_USE_OC4
		case 3:
#endif
#ifdef SERVO_USE_OC5
		case 4:
#endif
#ifdef SERVO_USE_OC6
		case 5:
#endif
#ifdef SERVO_USE_OC7
		case 6:
#endif
#ifdef SERVO_USE_OC8
		case 7:
#endif
			if( ServoPositions[Channel] != Position )
			{
				if( Position < PWM_MAX_POSITION )
				{
					// Update the holding registers
					ServoPositions[Channel] = Position;
				}
			}
	}		
}

/* OUTPUT COMPARE INTERRUPTS ARE COMPILED ONLY FOR THE DESIRED SERVOS */

#ifdef SERVO_USE_OC1
void _ISR __attribute__((no_auto_psv)) _OC1Interrupt(void)
{
	// Update servo 1
	#ifdef PWM_INVERT
	SetPulseOC1( PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*0+PWM_PULSE_ZERO+ServoPositions[0],
		PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*0 );
	#else
	SetPulseOC1( PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*0,
		PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*0+PWM_PULSE_ZERO+ServoPositions[0] );
	#endif	
	_OC1IF = 0;
}
#endif

#ifdef SERVO_USE_OC2
void _ISR __attribute__((no_auto_psv)) _OC2Interrupt(void)
{
	#ifdef PWM_INVERT
	SetPulseOC2( PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*1+PWM_PULSE_ZERO+ServoPositions[1],
		PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*1 );
	#else
	SetPulseOC2( PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*1,
		PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*1+PWM_PULSE_ZERO+ServoPositions[1] );
	#endif
	_OC2IF = 0;
}
#endif

#ifdef SERVO_USE_OC3
void _ISR __attribute__((no_auto_psv)) _OC3Interrupt(void)
{
	#ifdef PWM_INVERT
	SetPulseOC3( PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*2+PWM_PULSE_ZERO+ServoPositions[2],
		PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*2 );
	#else
	SetPulseOC3( PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*2,
		PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*2+PWM_PULSE_ZERO+ServoPositions[2] );
	#endif
	_OC3IF = 0;
}
#endif

#ifdef SERVO_USE_OC4
void _ISR __attribute__((no_auto_psv)) _OC4Interrupt(void)
{
	#ifdef PWM_INVERT
	SetPulseOC4( PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*3+PWM_PULSE_ZERO+ServoPositions[3],
		PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*3 );
	#else
	SetPulseOC4( PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*3,
		PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*3+PWM_PULSE_ZERO+ServoPositions[3] );
	#endif
	_OC4IF = 0;
}
#endif

#ifdef SERVO_USE_OC5
void _ISR __attribute__((no_auto_psv)) _OC5Interrupt(void)
{
	#ifdef PWM_INVERT
	SetPulseOC5( PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*4+PWM_PULSE_ZERO+ServoPositions[4],
		PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*4 );
	#else
	SetPulseOC5( PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*4,
		PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*4+PWM_PULSE_ZERO+ServoPositions[4] );
	#endif
	_OC5IF = 0;
}
#endif

#ifdef SERVO_USE_OC6
void _ISR __attribute__((no_auto_psv)) _OC6Interrupt(void)
{
	#ifdef PWM_INVERT
	SetPulseOC6( PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*5+PWM_PULSE_ZERO+ServoPositions[5],
		PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*5 );
	#else
	SetPulseOC6( PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*5,
		PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*5+PWM_PULSE_ZERO+ServoPositions[5] );
	#endif
	_OC6IF = 0;
}
#endif

#ifdef SERVO_USE_OC7
void _ISR __attribute__((no_auto_psv)) _OC7Interrupt(void)
{
	#ifdef PWM_INVERT
	SetPulseOC7( PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*6+PWM_PULSE_ZERO+ServoPositions[6],
		PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*6 );
	#else
	SetPulseOC7( PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*6,
		PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*6+PWM_PULSE_ZERO+ServoPositions[6] );
	#endif
	_OC7IF = 0;
}
#endif

#ifdef SERVO_USE_OC8
void _ISR __attribute__((no_auto_psv)) _OC8Interrupt(void)
{
	#ifdef PWM_INVERT
	SetPulseOC8( PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*7+PWM_PULSE_ZERO+ServoPositions[7],
		PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*7 );
	#else
	SetPulseOC8( PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*7,
		PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*7+PWM_PULSE_ZERO+ServoPositions[7] );
	#endif
	_OC8IF = 0;
}
#endif


void InitServos()
{

// Start all channels with servos centered

#ifdef SERVO_USE_OC1
	OpenOC1(	OC_IDLE_CON & 
				OC_TIMER2_SRC &
				OC_OFF,
	#ifdef PWM_INVERT
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*0+PWM_PULSE_ZERO+PWM_PULSE_CENTER,
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*0 );
	#else
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*0,
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*0+PWM_PULSE_ZERO+PWM_PULSE_CENTER );
	#endif
#endif

#ifdef SERVO_USE_OC2
	OpenOC2(	OC_IDLE_CON & 
				OC_TIMER2_SRC &
				OC_OFF,
	#ifdef PWM_INVERT
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*1+PWM_PULSE_ZERO+PWM_PULSE_CENTER,
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*1 );
	#else
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*1,
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*1+PWM_PULSE_ZERO+PWM_PULSE_CENTER );
	#endif
#endif

#ifdef SERVO_USE_OC3
	OpenOC3(	OC_IDLE_CON & 
				OC_TIMER2_SRC &
				OC_OFF,
	#ifdef PWM_INVERT
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*2+PWM_PULSE_ZERO+PWM_PULSE_CENTER,
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*2 );
	#else
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*2,
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*2+PWM_PULSE_ZERO+PWM_PULSE_CENTER );
	#endif
#endif

#ifdef SERVO_USE_OC4
	OpenOC4(	OC_IDLE_CON & 
				OC_TIMER2_SRC &
				OC_OFF,
	#ifdef PWM_INVERT
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*3+PWM_PULSE_ZERO+PWM_PULSE_CENTER,
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*3 );
	#else
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*3,
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*3+PWM_PULSE_ZERO+PWM_PULSE_CENTER );
	#endif
#endif

#ifdef SERVO_USE_OC5
	OpenOC5(	OC_IDLE_CON & 
				OC_TIMER2_SRC &
				OC_OFF,
	#ifdef PWM_INVERT
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*4+PWM_PULSE_ZERO+PWM_PULSE_CENTER,
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*4 );
	#else
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*4,
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*4+PWM_PULSE_ZERO+PWM_PULSE_CENTER );
	#endif
#endif

#ifdef SERVO_USE_OC6
	OpenOC6(	OC_IDLE_CON & 
				OC_TIMER2_SRC &
				OC_OFF,
	#ifdef PWM_INVERT
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*5+PWM_PULSE_ZERO+PWM_PULSE_CENTER,
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*5 );
	#else
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*5,
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*5+PWM_PULSE_ZERO+PWM_PULSE_CENTER );
	#endif
#endif

#ifdef SERVO_USE_OC7
	OpenOC7(	OC_IDLE_CON & 
				OC_TIMER2_SRC &
				OC_OFF,
	#ifdef PWM_INVERT
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*6+PWM_PULSE_ZERO+PWM_PULSE_CENTER,
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*6 );
	#else
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*6,
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*6+PWM_PULSE_ZERO+PWM_PULSE_CENTER );
	#endif
#endif

#ifdef SERVO_USE_OC8
	OpenOC8(	OC_IDLE_CON & 
				OC_TIMER2_SRC &
				OC_OFF,
	#ifdef PWM_INVERT
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*7+PWM_PULSE_ZERO+PWM_PULSE_CENTER,
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*7 );
	#else
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*7,
				PWM_PULSE_ORIGIN+PWM_PULSE_DELTA*7+PWM_PULSE_ZERO+PWM_PULSE_CENTER );
	#endif
#endif


// Initialize holding registers with center positions
#ifdef SERVO_USE_OC1
	ServoPositions[0] = PWM_PULSE_CENTER;
#endif
#ifdef SERVO_USE_OC2
	ServoPositions[1] = PWM_PULSE_CENTER;
#endif
#ifdef SERVO_USE_OC3
	ServoPositions[2] = PWM_PULSE_CENTER;
#endif
#ifdef SERVO_USE_OC4
	ServoPositions[3] = PWM_PULSE_CENTER;
#endif
#ifdef SERVO_USE_OC5
	ServoPositions[4] = PWM_PULSE_CENTER;
#endif
#ifdef SERVO_USE_OC6
	ServoPositions[5] = PWM_PULSE_CENTER;
#endif
#ifdef SERVO_USE_OC7
	ServoPositions[6] = PWM_PULSE_CENTER;
#endif
#ifdef SERVO_USE_OC8
	ServoPositions[7] = PWM_PULSE_CENTER;
#endif

// Set up Timer2 as the PWM timebase
	OpenTimer2( T2_ON &
				T2_IDLE_CON &
				T2_GATE_OFF &
				T2_PS_1_8 &
				T2_SOURCE_INT,
				PWM_PERIOD );

#ifdef SERVO_USE_OC1
	ConfigIntOC1( OC_INT_ON & OC_INT_PRIOR_7 );
#endif
#ifdef SERVO_USE_OC2
	ConfigIntOC2( OC_INT_ON & OC_INT_PRIOR_7 );
#endif
#ifdef SERVO_USE_OC3
	ConfigIntOC3( OC_INT_ON & OC_INT_PRIOR_7 );
#endif
#ifdef SERVO_USE_OC4
	ConfigIntOC4( OC_INT_ON & OC_INT_PRIOR_7 );
#endif
#ifdef SERVO_USE_OC5
	ConfigIntOC5( OC_INT_ON & OC_INT_PRIOR_7 );
#endif
#ifdef SERVO_USE_OC6
	ConfigIntOC6( OC_INT_ON & OC_INT_PRIOR_7 );
#endif
#ifdef SERVO_USE_OC7
	ConfigIntOC7( OC_INT_ON & OC_INT_PRIOR_7 );
#endif
#ifdef SERVO_USE_OC8
	ConfigIntOC8( OC_INT_ON & OC_INT_PRIOR_7 );
#endif

	ConfigIntTimer2( T2_INT_OFF );

}


#endif
