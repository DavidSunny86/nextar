//#@ Abhishek Dey
//#@ December 26 06
//#@ September 16 07
#ifndef __XINPUTMANAGER_H__
#define __XINPUTMANAGER_H__

#include <xtypesex.h>
#include <iobject.h>
#include <list_t.h>
#include <io/xkeycode.h>

namespace nextar
{

/*
Input manager class manages input
specific task
*/
enum INPUTDEVICES
{
	ID_DIRECTINPUT8
};

enum IDEVTYPE
{
	E_IDEVT_MOUSE = 1,
	E_IDEVT_KEYBOARD = 2,
	E_IDEVT_JOYSTICK = 4,
	E_IDEVT_UNKOWN = 8
};

#define DEVICE_COUNT 3

class iInputDevice;

struct cKeyboardState
{
	// flags
	enum { 
		NumLockOn		=		1,
		CapsLockOn		=		2,
		ScrollLockOn	=		4,
	};
	bool	muKeysState[KEY_COUNT];
	// toggle key states and more keys
	u32		muFlags;
};

struct cMouseState
{
	coord2_x	mkRelCoord;
	s32		miRelWheel;

	union
	{
		bool	marrbButtonState[4];
		u32		muButtonState;
	};

	union
	{
		bool	marrbLastButtonState[4];
		u32		muLastButtonState;
	};

	cMouseState() : miRelWheel(0), muButtonState(0), muLastButtonState(0)
	{
	}
};

struct cJoystickState
{
	bool	mbUnknown;
};

struct cInput
{
	union
	{
		cKeyboardState*		mpkKBState;
		cMouseState*		mpkMState;
		cJoystickState*		mpkJState;
	};
	IDEVTYPE	meDevice;
};

class iInputHandler
{
public:
	virtual ~iInputHandler() {}
	virtual void handleInput( cInput*, s32 iCount ) = 0;
};

class X_Api cInputSystem : public iObject
{
	SharedSingleInterface( cInputSystem );

public:

	cInputSystem();
	virtual ~cInputSystem();

	_n_inline u32 getDevicesObtained() { return muDevicesObtained; }

	/************************************
	./ activate	
	./ Decscription: activate this device
	./ Return type:	void 
	************************************/
	void activate();

	/************************************
	./ deactivate	
	./ Decscription: deactivate this device
	./ Return type:	void 
	************************************/
	void deactivate();

	/************************************
	./ registerInputHandler	
	./ Decscription:	
	./ Return type:	
	./ param: iInputHandler*	[]:		
	************************************/
	void	registerInputHandler( iInputHandler* );

	/************************************
	./ removeInputHandler	
	./ Decscription:	
	./ Return type:	
	./ param: iInputHandler*	[]:		
	************************************/
	void	removeInputHandler( iInputHandler* );

	/************************************
	./ updateAllDevice	
	./ Decscription:	
	./ Return type:	
	************************************/
	void	updateAllDevice();

	/************************************
	./ updateReceivers	
	./ Decscription:	
	./ Return type:	
	************************************/
	void	updateReceivers();

	/************************************
	./ attachDeivice	
	./ Decscription:	
	./ Return type:	
	./ param: iInputDevice*	[]:		
	************************************/
	void	attachDeivice( iInputDevice* );

	/************************************
	./ detachDevice	
	./ Decscription:	
	./ Return type:	
	./ param: iInputDevice*	[]:		
	************************************/
	void	detachDevice( iInputDevice* );

	/************************************
	./ getKeyName	
	./ Decscription:	
	./ Return type:	const tchar* 
	./ param: KEYCODE	[]:		
	************************************/
	const tchar* getKeyName( KEYCODE );

	/************************************
	./  getAsciiForKey	
	./ Decscription:	
	./ Return type:	const tchar 
	./ param: KEYCODE	[]:		
	./ param: bool bShiftUp []: shift the letter to upper case.
	************************************/
	const tchar  getAsciiForKey( KEYCODE, bool bShiftUp );

private:

	u32						muDevicesObtained;
	list_t<iInputHandler*>	mkEventReceiver;
	list_t<iInputDevice*>	mkDevices;
};

}

#endif //__XINPUTMANAGER_H__