/*
The zlib/libpng License

Copyright (c) 2006 Phillip Castaneda (pjcast -- www.wreckedgames.com)

This software is provided 'as-is', without any express or implied warranty. In no event will
the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial 
applications, and to alter it and redistribute it freely, subject to the following
restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that 
		you wrote the original software. If you use this software in a product, 
		an acknowledgment in the product documentation would be appreciated but is 
		not required.

    2. Altered source versions must be plainly marked as such, and must not be 
		misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/
#include "Win32/Win32JoyStick.h"
#include "Win32/Win32InputManager.h"
#include "OISEvents.h"
#include "OISException.h"

using namespace OIS;

//-------------------------------------------------------------------//
Win32JoyStick::Win32JoyStick( IDirectInput8* pDI, bool buffered, DWORD coopSettings, const JoyStickInfo &info )
{
	mBuffered = buffered;
	mDirectInput = pDI;
	coopSetting = coopSettings;
	mJoyStick = 0;
	mType = OISJoyStick;

	deviceGuid = info.deviceID;
	mVendor = info.vendor;
	mDevID = info.devId;

	listener = 0;
}

//-------------------------------------------------------------------//
Win32JoyStick::~Win32JoyStick()
{
	if(mJoyStick)
	{
		mJoyStick->Unacquire();
		mJoyStick->Release();
		mJoyStick = 0;
	}
}

//-------------------------------------------------------------------//
void Win32JoyStick::_initialize()
{
	//Clear joy old state
	mState.clear();

	DIPROPDWORD dipdw;

	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = DX_BUFFERSIZE;

	if(FAILED(mDirectInput->CreateDevice(deviceGuid, &mJoyStick, NULL)))
		OIS_EXCEPT( E_General, "Win32JoyStick::_initialize() >> Could not init joy device!");

	if(FAILED(mJoyStick->SetDataFormat(&c_dfDIJoystick2)))
		OIS_EXCEPT( E_General, "Win32JoyStick::_initialize() >> format error!");

	HWND hwin = ((Win32InputManager*)(InputManager::getSingletonPtr()))->getWindowHandle();
	
	if(FAILED(mJoyStick->SetCooperativeLevel( hwin, coopSetting)))
		OIS_EXCEPT( E_General, "Win32JoyStick::_initialize() >> coop error!");

	if( mBuffered )
		if( FAILED(mJoyStick->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)) )
			OIS_EXCEPT( E_General, "Win32Mouse::Win32Mouse >> Failed to set property" );

	//Enumerate all axes/buttons/sliders/etc before aquiring
    _enumerate();

	capture();
}

//-------------------------------------------------------------------//
void Win32JoyStick::_enumerate()
{
	//We can check force feedback here too
	DIDEVCAPS  DIJoyCaps; 

	DIJoyCaps.dwSize = sizeof(DIDEVCAPS);

	HRESULT hr = mJoyStick->GetCapabilities(&DIJoyCaps); 
	numAxes = (short)DIJoyCaps.dwAxes;
	numButtons = (short)DIJoyCaps.dwButtons;

	mJoyStick->EnumObjects(_DIEnumDeviceObjectsCallback, this, DIDFT_AXIS);
}

//-------------------------------------------------------------------//
BOOL CALLBACK Win32JoyStick::_DIEnumDeviceObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
	DIPROPRANGE diprg;
	Win32JoyStick* _this = (Win32JoyStick*)pvRef;

	diprg.diph.dwSize       = sizeof(DIPROPRANGE);
	diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	diprg.diph.dwHow        = DIPH_BYID;
	diprg.diph.dwObj        = lpddoi->dwType;
	diprg.lMin              = -32767;
	diprg.lMax              = 32768;

	if (FAILED(_this->mJoyStick->SetProperty(DIPROP_RANGE, &diprg.diph)))
		OIS_EXCEPT( E_General, "Win32JoyStick::_DIEnumDeviceObjectsCallback >> Failed to set property" );

	return DIENUM_CONTINUE;
}

//-------------------------------------------------------------------//
void Win32JoyStick::capture()
{
	if( mBuffered )
		_readBuffered();
	else
		_read();
}

//-------------------------------------------------------------------//
void Win32JoyStick::_read()
{
    HRESULT     hr;
    DIJOYSTATE2 state;           // Direct Input joystick state 

    // Poll the device to read the current state
    hr = mJoyStick->Poll();
	if( SUCCEEDED(hr) || ((hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) &&
            SUCCEEDED(mJoyStick->Acquire()) && SUCCEEDED( mJoyStick->Poll() )))
    {
		hr = mJoyStick->GetDeviceState(sizeof(DIJOYSTATE2), &state);

		if (FAILED(hr))
			OIS_EXCEPT(E_General, "Win32JoyStick::_read() >> Device Read Problem 1");

		mState.buttons = 0;
		for( int i = 0; i < numButtons; ++i )
			mState.buttons |= ((state.rgbButtons[i] & 0x80) != 0) << i;

		for( int i = 0; i < 4; ++i )
			mState.mPOV[i] = state.rgdwPOV[i];

		mState.mAxes[JoyStickState::Stick].setAbsValues(state.lX, state.lY, state.lZ);
		mState.mAxes[JoyStickState::Rotational].setAbsValues(state.lRx, state.lRy, state.lRz);
		mState.mAxes[JoyStickState::Velocity].setAbsValues(state.lVX, state.lVY, state.lVZ);
		mState.mAxes[JoyStickState::AngularVelocity].setAbsValues(state.lVRx, state.lVRy, state.lVRz);
		mState.mAxes[JoyStickState::Acceleration].setAbsValues(state.lAX, state.lAY, state.lAZ);
		mState.mAxes[JoyStickState::AngularAcceleration].setAbsValues(state.lARx, state.lARy, state.lARz);
		mState.mAxes[JoyStickState::Force].setAbsValues(state.lFX, state.lFY, state.lFZ);
		mState.mAxes[JoyStickState::Torque].setAbsValues(state.lFRx, state.lFRy, state.lFRz);
	}
}

//-------------------------------------------------------------------//
void Win32JoyStick::_readBuffered()
{
	DIDEVICEOBJECTDATA diBuff[DX_BUFFERSIZE];
	DWORD entries = DX_BUFFERSIZE;

	// Poll the device to read the current state
    HRESULT hr = mJoyStick->Poll();
	if( hr == DI_OK )
		hr = mJoyStick->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), diBuff, &entries, 0 );

	if( hr != DI_OK )
	{
		hr = mJoyStick->Acquire();
		while( hr == DIERR_INPUTLOST ) 
			hr = mJoyStick->Acquire();

		// Poll the device to read the current state
	    mJoyStick->Poll();
		hr = mJoyStick->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), diBuff, &entries, 0 );
		if( FAILED(hr) )
			OIS_EXCEPT(E_General, "Win32JoyStick::_readBuffered() >> Device Error");
	}

#undef DIJOFS_BUTTON
#undef DIJOFS_POV
#define DIJOFS_BUTTON(n) (FIELD_OFFSET(DIJOYSTATE2, rgbButtons) + (n))
#define DIJOFS_POV(n) (FIELD_OFFSET(DIJOYSTATE2, rgdwPOV)+(n)*sizeof(DWORD))

	bool moved[8] = {false,false,false,false,false,false,false,false};
	for(unsigned int i = 0; i < entries; ++i)
	{
		switch(diBuff->dwOfs)
		{
		case FIELD_OFFSET(DIJOYSTATE2, lX):
			moved[JoyStickState::Stick] = true;
			mState.mAxes[JoyStickState::Stick].abX = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lY):
			moved[JoyStickState::Stick] = true;
			mState.mAxes[JoyStickState::Stick].abY = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lZ):
			moved[JoyStickState::Stick] = true;
			mState.mAxes[JoyStickState::Stick].abZ = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lRx):
			moved[JoyStickState::Rotational] = true;
			mState.mAxes[JoyStickState::Rotational].abX = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lRy):
			moved[JoyStickState::Rotational] = true;
			mState.mAxes[JoyStickState::Rotational].abY = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lRz):
			moved[JoyStickState::Rotational] = true;
			mState.mAxes[JoyStickState::Rotational].abZ = diBuff[i].dwData;
			break;
		//----- Max 4 POVs Next ---------------//
		case DIJOFS_POV(0):
			_changePOV(0,diBuff[i]);
			break;
		case DIJOFS_POV(1):
			_changePOV(1,diBuff[i]);
			break;
		case DIJOFS_POV(2):
			_changePOV(2,diBuff[i]);
			break;
		case DIJOFS_POV(3):
			_changePOV(3,diBuff[i]);
			break;
		//------ The remaining Axes ------------//
		case FIELD_OFFSET(DIJOYSTATE2, lVX):	
			moved[JoyStickState::Velocity] = true;
			mState.mAxes[JoyStickState::Velocity].abX = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lVY):
			moved[JoyStickState::Velocity] = true;
			mState.mAxes[JoyStickState::Velocity].abY = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lVZ):
			moved[JoyStickState::Velocity] = true;
			mState.mAxes[JoyStickState::Velocity].abZ = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lVRx):
			moved[JoyStickState::AngularVelocity] = true;
			mState.mAxes[JoyStickState::AngularVelocity].abX = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lVRy):
			moved[JoyStickState::AngularVelocity] = true;
			mState.mAxes[JoyStickState::AngularVelocity].abY = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lVRz):
			moved[JoyStickState::AngularVelocity] = true;
			mState.mAxes[JoyStickState::AngularVelocity].abZ = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lAX):
			moved[JoyStickState::Acceleration] = true;
			mState.mAxes[JoyStickState::Acceleration].abX = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lAY):
			moved[JoyStickState::Acceleration] = true;
			mState.mAxes[JoyStickState::Acceleration].abY = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lAZ):
			moved[JoyStickState::Acceleration] = true;
			mState.mAxes[JoyStickState::Acceleration].abZ = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lARx):
			moved[JoyStickState::AngularAcceleration] = true;
			mState.mAxes[JoyStickState::AngularAcceleration].abX = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lARy):
			moved[JoyStickState::AngularAcceleration] = true;
			mState.mAxes[JoyStickState::AngularAcceleration].abY = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lARz):
			moved[JoyStickState::AngularAcceleration] = true;
			mState.mAxes[JoyStickState::AngularAcceleration].abZ = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lFX):
			moved[JoyStickState::Force] = true;
			mState.mAxes[JoyStickState::Force].abX = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lFY):
			moved[JoyStickState::Force] = true;
			mState.mAxes[JoyStickState::Force].abY = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lFZ):
			moved[JoyStickState::Force] = true;
			mState.mAxes[JoyStickState::Force].abZ = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lFRx):
			moved[JoyStickState::Torque] = true;
			mState.mAxes[JoyStickState::Torque].abX = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lFRy):
			moved[JoyStickState::Torque] = true;
			mState.mAxes[JoyStickState::Torque].abY = diBuff[i].dwData;
			break;
		case FIELD_OFFSET(DIJOYSTATE2, lFRz):
			moved[JoyStickState::Torque] = true;
			mState.mAxes[JoyStickState::Torque].abZ = diBuff[i].dwData;
			break;
		default:
			//Handle Button Events Easily using the DX Offset Macros
			if( diBuff->dwOfs >= DIJOFS_BUTTON(0) && diBuff->dwOfs <= DIJOFS_BUTTON(30) )
			{
				if(!_doButtonClick((diBuff->dwOfs - DIJOFS_BUTTON0), diBuff[i])) 
					return;
			}
			break;
		} //end case
	} //end for

	//Check to see if any of the axes values have changed.. if so send events
	if( listener )
		for( int i = 0; i < 8; ++i )
			if( moved[i] )
				listener->axisMoved( JoyStickEvent(this, diBuff[i].dwTimeStamp, mState), i );
}
//-------------------------------------------------------------------//
bool Win32JoyStick::_doButtonClick( int button, DIDEVICEOBJECTDATA& di )
{
	bool ret = true;
	if( listener )
	{
		if( di.dwData & 0x80 )
		{
			mState.buttons |= 1 << button; //turn the bit flag on
			ret = listener->buttonPressed( JoyStickEvent( this, di.dwTimeStamp, mState ), button );
		}
		else
		{
			mState.buttons &= ~(1 << button); //turn the bit flag off
			ret = listener->buttonReleased( JoyStickEvent( this, di.dwTimeStamp, mState ), button );
		}
	}
	return ret;
}

//-------------------------------------------------------------------//
bool Win32JoyStick::_changePOV( int pov, DIDEVICEOBJECTDATA& di )
{
	mState.mPOV[pov] = di.dwData;

	if( listener )
		return listener->povMoved( JoyStickEvent( this, di.dwTimeStamp, mState ), pov );

	return true;
}

//-------------------------------------------------------------------//
void Win32JoyStick::setBuffered(bool buffered)
{
	if( buffered != mBuffered )
	{
		if(mJoyStick)
		{
			mJoyStick->Unacquire();
			mJoyStick->Release();
			mJoyStick = 0;
		}
		
		mBuffered = buffered;
		_initialize();
	}
}

//-------------------------------------------------------------------//
JoyStickInfo Win32JoyStick::_getJoyInfo()
{
	JoyStickInfo js;
	js.deviceID = deviceGuid;
	js.devId = mDevID;
	js.vendor = mVendor;

	return js;
}