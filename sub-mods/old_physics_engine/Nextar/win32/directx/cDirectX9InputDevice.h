//#@ Abhishek Dey
//#@ 1/9/2007	AT:		12:53:31 AM
//#@ 1/9/2007	AT:		4:20:42 PM
//#@ 27 September 2007

#ifndef __DIRECTX9INPUTDEVICE_H__
#define __DIRECTX9INPUTDEVICE_H__

#include <cinputsystem.h>
#include <iinputdevice.h>


/* forward declarations */

#define MAX_KEY_BUFFER_SIZE 16
#define MAX_EVENT			17

namespace nextar
{

class cDirectX9InputDevice : public iInputDevice
{
private:
	
	u32						muDevicesObtained;
	u32 					muWndHandle;
	u32						muEventCount;
	bool					mbPaused;

	/* keyboard */
	bool					mbNumLockOn;
	bool					mbCapsLockOn;
	bool					mbScrollLockOn;
	/* mouse */
	/* joystick */
	
private:

	virtual ~cDirectX9InputDevice();

	void	createKeyboard();
	void	createMouse();
//	void	createJoystick();

	void	destroy( IDEVTYPE );

public:

	/************************************
	./ ctor	
	************************************/
	cDirectX9InputDevice( u32 uWindowHandle );
	/************************************
	./ activate	
	************************************/
	virtual void activate();
	/************************************
	./ deactivate	
	************************************/
	virtual void deactivate();
	/************************************
	./ pollData	
	************************************/
	virtual void pollData();
	/************************************
	./ distributeData	
	************************************/
	virtual void distributeData( list_t<iInputHandler*>& );
	/************************************
	./ getDevicesObtained
	************************************/
	virtual u32 getDevicesObtained();
};

}


#endif //__DIRECTX9INPUTDEVICE_H__