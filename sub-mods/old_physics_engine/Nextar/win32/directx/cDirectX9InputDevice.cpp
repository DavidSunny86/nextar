//#@ Abhishek Dey
//#@ 1/9/2007	AT:		12:53:31 AM
//#@ 1/9/2007	AT:		4:20:42 PM
//#@ 27 September 2007
//#@ 12 Oct 2007

#define INITGUID
#define DIRECTINPUT_VERSION	0x0800
#include "cDirectX9InputDevice.h"
#include <iSystem.h>
#include <windows.h>
#include <dinput.h>

#define ERROR1	"Dll not found: dinput8.dll"
#define ERROR2	"Procedure not found: \"DirectInput8Create\" in dinput8.dll"
#define ERROR3	"cDirectX9InputDevice::cDirectX9InputDevice := Failed to create input object!"
#define ERROR4	"cDirectX9InputDevice::createKeyboard := Failed to create input object!"
#define ERROR5	"cDirectX9InputDevice::createKeyboard := Failed to set device format!"
#define ERROR6	"cDirectX9InputDevice::createKeyboard := Failed to set cooperative level!"
#define ERROR7	"cDirectX9InputDevice::createKeyboard := Failed to set device property!"
#define ERROR8	"cDirectX9InputDevice::createMouse := Failed to create input object!"
#define ERROR9	"cDirectX9InputDevice::createMouse := Failed to set device format!"
#define ERROR10	"cDirectX9InputDevice::createMouse := Failed to set cooperative level!"
#define ERROR11	"cDirectX9InputDevice::activate := Could not acquire device"
#define ERROR12	"cDirectX9InputDevice::pollData := Can't poll from Keyboard"
#define ERROR13	"cDirectX9InputDevice::pollData := Can't poll from Mouse"

#pragma comment ( lib, "dinput8.lib" )

namespace nextar
{

/*************************************************************************/
/* dinput8 objects;
/*************************************************************************/
IDirectInput8*			gpkDirectInput = 0;
IDirectInputDevice8*	gpkDevices[ DEVICE_COUNT ] = {0};
/*************************************************************************/
/* dinput8.dll module handle
/*************************************************************************/
static  HMODULE  gpModule = 0;
/*************************************************************************/
#define MAX_MOUSE_BUTTON 3
/*************************************************************************/
/* Device data
/*************************************************************************/
static byte							garruKeyState[256] = {0};
static cKeyboardState				gkKeyboardState = { 0 };
static DIMOUSESTATE					gkMouseData = {0};
static cMouseState					gkMouseState;
static cJoystickState				gkJoystickState;
static cInput						garrkInput[ DEVICE_COUNT ] = {0};
static s32							giCount = 0;
static bool							garrbDeviceActive[DEVICE_COUNT] = {0};
/*************************************************************************/
/* KeyName lookup table
/*************************************************************************/
static byte garrKeyMap[] =
{
	DIK_ESCAPE,
	DIK_A,
	DIK_B,
	DIK_C,
	DIK_D,
	DIK_E,
	DIK_F,
	DIK_G,
	DIK_H,
	DIK_I,
	DIK_J,
	DIK_K,
	DIK_L,
	DIK_M,
	DIK_N,
	DIK_O,
	DIK_P,
	DIK_Q,
	DIK_R,
	DIK_S,
	DIK_T,
	DIK_U,
	DIK_V,
	DIK_W,
	DIK_X,
	DIK_Y,
	DIK_Z,
	DIK_SPACE,
	DIK_TAB,
	DIK_RETURN,
	DIK_NUMPADENTER,
	DIK_0,
	DIK_1,
	DIK_2,
	DIK_3,
	DIK_4,
	DIK_5,
	DIK_6,
	DIK_7,
	DIK_8,
	DIK_9,
	DIK_NUMPAD0,
	DIK_NUMPAD1,
	DIK_NUMPAD2,
	DIK_NUMPAD3,
	DIK_NUMPAD4,
	DIK_NUMPAD5,
	DIK_NUMPAD6,
	DIK_NUMPAD7,
	DIK_NUMPAD8,
	DIK_NUMPAD9,
	DIK_GRAVE,
	DIK_MINUS,
	DIK_EQUALS,
	DIK_BACKSLASH,
	DIK_LBRACKET,
	DIK_RBRACKET,
	DIK_SEMICOLON,
	DIK_APOSTROPHE,
	DIK_COMMA,
	DIK_PERIOD,
	DIK_SLASH,
	DIK_NUMPADEQUALS,
	DIK_ADD,
	DIK_SUBTRACT,
	DIK_MULTIPLY,
	DIK_DIVIDE,
	DIK_DECIMAL,
	DIK_NUMPADCOMMA,
	DIK_CIRCUMFLEX,
	DIK_COLON,
	DIK_UNDERLINE,
	DIK_F1,
	DIK_F2,
	DIK_F3,
	DIK_F4,
	DIK_F5,
	DIK_F6,
	DIK_F7,
	DIK_F8,
	DIK_F9,
	DIK_F10,
	DIK_F11,
	DIK_F12,
	DIK_F13,
	DIK_F14,
	DIK_F15,
	DIK_BACK,
	DIK_CAPITAL,
	DIK_NUMLOCK,
	DIK_SCROLL,
	DIK_KANA,
	DIK_CONVERT,
	DIK_NOCONVERT,
	DIK_YEN,
	DIK_AT,
	DIK_KANJI,
	DIK_STOP,
	DIK_AX,
	DIK_UNLABELED,
	DIK_SYSRQ,
	DIK_PAUSE,
	DIK_HOME,
	DIK_UP,
	DIK_PRIOR,
	DIK_LEFT,
	DIK_RIGHT,
	DIK_END,
	DIK_DOWN,
	DIK_NEXT,
	DIK_INSERT,
	DIK_DELETE,
	DIK_LWIN,
	DIK_RWIN,
	DIK_APPS,
	DIK_POWER,
	DIK_SLEEP,
	DIK_LCONTROL,
	DIK_RCONTROL,
	DIK_LSHIFT,
	DIK_RSHIFT,
	DIK_LMENU,
	DIK_RMENU
};


/************************************
./ cDirectX9InputDevice::cDirectX9InputDevice
./ Decscription: constructor
./ Return type:
./ param: u32 uWin	[]:
************************************/
cDirectX9InputDevice::cDirectX9InputDevice( u32 uWin ) : muWndHandle(uWin), muEventCount(0), mbPaused(false)
{
	RefObjectInit(cDirectX9InputDevice);

	X_BreakOutIf( gpkDirectInput );

	for( s32 i = 0; i < DEVICE_COUNT; i++ )
	{
		gpkDevices[i] = 0;
		garrbDeviceActive[i] = false;
	}

	HRESULT uVal = -1;

	uVal = DirectInput8Create( GetModuleHandle( 0 ), DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&gpkDirectInput, 0 );

	if( FAILED( uVal ) )
	{
		ReportBug( ERROR3, "cDirectX9InputDevice" );
		return;
	}

	muDevicesObtained = true;
	createMouse();
	createKeyboard();
	//createJoystick();
}

/* dtor */
cDirectX9InputDevice::~cDirectX9InputDevice()
{
	destroy(nextar::E_IDEVT_MOUSE);
	destroy(nextar::E_IDEVT_JOYSTICK);
	destroy(nextar::E_IDEVT_KEYBOARD);
}

/* create keyboard */
void cDirectX9InputDevice::createKeyboard()
{
	bool    bFailed = false;
	HRESULT uVal = -1;

	// get some key states
	mbCapsLockOn = (GetKeyState(VK_CAPITAL)&0x1) ? true : false;
	mbNumLockOn = (GetKeyState(VK_NUMLOCK)&0x1) ? true : false;
	mbScrollLockOn = (GetKeyState(VK_SCROLL)&0x1) ? true : false;

	if( !gpkDevices[1] )
	{
		uVal = gpkDirectInput->CreateDevice( GUID_SysKeyboard, &gpkDevices[1], 0 );

		if( (bFailed = FAILED( uVal )) )
			ReportBug( ERROR4, "cDirectX9InputDevice::createKeyboard()" );
	}

	/* TODO: define data formats like c_dfDIKeyboard*/
	uVal = gpkDevices[1]->SetDataFormat( &c_dfDIKeyboard );

	if( (bFailed = FAILED( uVal )) )
		ReportBug( ERROR5, "cDirectX9InputDevice::createKeyboard()" );

	uVal = gpkDevices[1]->SetCooperativeLevel( (HWND)muWndHandle, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND );

	if( (bFailed = FAILED( uVal )) )
		ReportBug( ERROR6, "cDirectX9InputDevice::createKeyboard()" );

	/*
	DIPROPDWORD dipdw =
	{
		{
			sizeof(DIPROPDWORD),	// diph.dwSize
				sizeof(DIPROPHEADER),	// diph.dwHeaderSize
				0,						// diph.dwObj
				DIPH_DEVICE,			// diph.dwHow
		},
		MAX_KEY_BUFFER_SIZE,			// dwData
	};

	uVal = gpkDevices[1]->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph );
	*/

	if( (bFailed = FAILED( uVal )) )
		ReportBug( ERROR7, "cDirectX9InputDevice::createKeyboard()" );

	uVal = gpkDevices[1]->Acquire();
	garrbDeviceActive[1] = FAILED( uVal ) ? 0 : 1;

	if(!bFailed) muDevicesObtained |= E_IDEVT_KEYBOARD;
}

/* create mouse */
void cDirectX9InputDevice::createMouse()
{
	bool    bFailed = false;
	HRESULT uVal = -1;

	if( !gpkDevices[1] )
	{
		uVal = gpkDirectInput->CreateDevice( GUID_SysMouse, &gpkDevices[0], 0 );

		if( (bFailed = FAILED( uVal )) )
			ReportBug( ERROR8, "cDirectX9InputDevice::createMouse()" );
	}

	uVal = gpkDevices[0]->SetDataFormat( &c_dfDIMouse );

	if( (bFailed = FAILED( uVal )) )
		ReportBug( ERROR9, "cDirectX9InputDevice::createMouse()" );

	uVal = gpkDevices[0]->SetCooperativeLevel( (HWND)muWndHandle, DISCL_EXCLUSIVE | DISCL_FOREGROUND );

	if( (bFailed = FAILED( uVal )) )
		ReportBug( ERROR10, "cDirectX9InputDevice::createMouse()" );

	uVal = gpkDevices[0]->Acquire();

	garrbDeviceActive[0] = FAILED( uVal ) ? 0 : 1;

	if(!bFailed) muDevicesObtained |= E_IDEVT_MOUSE;
}

/* create joystick */
/*void cDirectX9InputDevice::createJoystick()
{
}*/

/* isValidDevice */
u32 cDirectX9InputDevice::getDevicesObtained()
{ return muDevicesObtained; }

/* destroy */
void cDirectX9InputDevice::destroy( IDEVTYPE t )
{
	s32 i = -1;
	switch(t)
	{
	case E_IDEVT_MOUSE:
		i = 0; muDevicesObtained &= ~E_IDEVT_MOUSE; break;
	case E_IDEVT_KEYBOARD:
		i = 1; muDevicesObtained &= ~E_IDEVT_KEYBOARD; break;
	case E_IDEVT_JOYSTICK:
		i = 2; muDevicesObtained &= ~E_IDEVT_JOYSTICK; break;
	}

	if( i > 0 && gpkDevices[i] )
	{
		if( garrbDeviceActive[i] )
		{
			gpkDevices[i]->Unacquire();
			garrbDeviceActive[i] = false;
		}

		gpkDevices[i]->Release();
		gpkDevices[i] = 0;
	}
}

/* activate: */
void cDirectX9InputDevice::activate()
{
	if( mbPaused )
	{
		mbPaused = false;

		// changes might have been skipped
		mbCapsLockOn = (GetKeyState(VK_CAPITAL)&0x1) ? true : false;
		mbNumLockOn = (GetKeyState(VK_NUMLOCK)&0x1) ? true : false;
		mbScrollLockOn = (GetKeyState(VK_SCROLL)&0x1) ? true : false;

		for( s32 i = 0; i < DEVICE_COUNT; i++ )
			if( gpkDevices[i] && !garrbDeviceActive[i] )
			{
				if( SUCCEEDED(gpkDevices[i]->Acquire()) )
					garrbDeviceActive[i] = true;
				else
					ReportBug( ERROR11, "cDirectX9InputDevice::activate()" );
			}
	}
}

/* deactivate */
void cDirectX9InputDevice::deactivate()
{
	if( !mbPaused )
	{
		mbPaused = true;
		for( s32 i = 0; i < DEVICE_COUNT; i++ )
			if( gpkDevices[i] && garrbDeviceActive[i]&& SUCCEEDED(gpkDevices[i]->Unacquire()) )
				garrbDeviceActive[i] = false;
	}
}

/* pollData */
void cDirectX9InputDevice::pollData()
{
	if( mbPaused ) return;

	HRESULT uVal = -1;

	giCount = 0;

	/* update keyboard */
	/* check if the device is available */
	if( garrbDeviceActive[1] && gpkDevices[1] )
	{
		/* get buffered data */
		uVal = gpkDevices[1]->GetDeviceState( 256, (void*)&garruKeyState );

		/* if failed set keyboard input to 0 */
		if( FAILED( uVal ) )
		{
			ReportBug( ERROR12, "cDirectX9InputDevice::pollData()" );
		}
		else
		{
			giCount = 1;
			gkKeyboardState.muFlags = 0;
			garrkInput[0].meDevice = E_IDEVT_KEYBOARD;
			garrkInput[0].mpkKBState = &gkKeyboardState;

			bool* pkPtr = gkKeyboardState.muKeysState;
			for( u32 i = 0; i < KEY_COUNT; i++,pkPtr++ )
			{
				if(garruKeyState[garrKeyMap[i]] & 0x80)
					*pkPtr = true;
				else
					*pkPtr = false;
			}

			if(gkKeyboardState.muKeysState[KEY_CAPITAL])
				mbCapsLockOn = !mbCapsLockOn;
			if(gkKeyboardState.muKeysState[KEY_NUMLOCK])
				mbNumLockOn = !mbNumLockOn;
			if(gkKeyboardState.muKeysState[KEY_SCROLL])
				mbScrollLockOn = !mbScrollLockOn;

			if(mbCapsLockOn)
				gkKeyboardState.muFlags = cKeyboardState::CapsLockOn;
			if(mbScrollLockOn)
				gkKeyboardState.muFlags |= cKeyboardState::ScrollLockOn;
			if(mbNumLockOn)
				gkKeyboardState.muFlags |= cKeyboardState::NumLockOn;
		}
	}
	/* end keyboard */

	/* update mouse */
	/* check if the device is ok */
	if( garrbDeviceActive[0] && gpkDevices[0] )
	{
		/* poll data */
		uVal = gpkDevices[0]->GetDeviceState( sizeof(DIMOUSESTATE), &gkMouseData );

		if( FAILED( uVal ) )
		{
			ReportBug( ERROR13, "cDirectX9InputDevice::pollData()" );
		}
		else
		{
			garrkInput[giCount].meDevice = E_IDEVT_MOUSE;
			garrkInput[giCount].mpkMState = &gkMouseState;
			giCount++;

			gkMouseState.mkRelCoord.x = (s16)gkMouseData.lX;
			gkMouseState.mkRelCoord.y = (s16)gkMouseData.lY;
			gkMouseState.miRelWheel   = (s16)gkMouseData.lZ;
			gkMouseState.muLastButtonState = gkMouseState.muButtonState;
			gkMouseState.marrbButtonState[0] = gkMouseData.rgbButtons[0] & 0x80;
			gkMouseState.marrbButtonState[1] = gkMouseData.rgbButtons[1] & 0x80;
			gkMouseState.marrbButtonState[2] = gkMouseData.rgbButtons[2] & 0x80;
			gkMouseState.marrbButtonState[3] = gkMouseData.rgbButtons[3] & 0x80;
		}
	}
	/* end mouse */

	/* update joystick */
	/* end joystick */
}

void cDirectX9InputDevice::distributeData( list_t<iInputHandler*>& lstpHandlers )
{
	list_t<iInputHandler*>::iterator it = lstpHandlers.begin();
	while( it.ok() )
	{
		(*it)->handleInput( garrkInput, giCount );
		++it;
	}
}

}
