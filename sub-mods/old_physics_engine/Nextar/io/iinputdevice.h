//#@ Abhishek Dey
//#@ December 26 06
//#@ September 16 07

#ifndef __IXINPUTDEVICE_H__
#define __IXINPUTDEVICE_H__

#include <iobject.h>
#include <list_t.h>

namespace nextar
{

class iInputHandler;

class iInputDevice : public iObject
{
public:

	virtual ~iInputDevice() {}
	//@ activate the device
	virtual void activate() = 0;
	//@ deactivate the device
	virtual void deactivate() = 0;
	//@ poll devices for data. this simply updates its data cache
	virtual void pollData() = 0;
	//@ distribute polled data to the receivers
	virtual void distributeData( list_t<iInputHandler*>& ) = 0;
	//@ returns the devices supported by this controller
	virtual u32 getDevicesObtained() = 0;
};

}

#endif //__IXINPUTDEVICE_H__