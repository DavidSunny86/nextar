/*
 * InputManagerImpl.h
 *
 *  Created on: 21-Mar-2015
 *      Author: obhi
 */

#ifndef CORE_ENGINE_LOCAL_INCLUDE_INPUTMANAGERIMPL_H_
#define CORE_ENGINE_LOCAL_INCLUDE_INPUTMANAGERIMPL_H_

#include <InputManager.h>

namespace nextar {

class InputManagerImpl:
		public InputManager,
		public FrameListener {
	NEX_LOG_HELPER(InputManagerImpl);

public:

	InputManagerImpl();
	virtual ~InputManagerImpl();

	virtual void Configure(const Config&);

	virtual void RegisterProvider(InputControllerProvider*);
	virtual void UnregisterProvider();

	virtual void RegisterController(uint16 deviceId);
	virtual void UnregisterController(uint16 deviceId);

	virtual void RegisterListener(InputListener*, uint16 deviceId = -1);
	virtual void UnregisterListener(InputListener*, uint16 deviceId = -1);

	virtual uint32 GetNumController() {
		if (provider)
			provider->GetNumController();
		return 0;
	}

	virtual const InputControllerDesc& GetControllerDesc(uint32 n) {
		if (provider)
			provider->GetControllerDesc(n);
		return InputControllerDesc::Null;
	}

	virtual void Execute(const FrameTimer& frameTimer);
private:

	void ProcessDevices();

	typedef list<InputListener*>::type ListenerList;

	struct InputDevice {
		InputController* controller;
		ListenerList listeners;

		InputDevice()
		: controller(0) {
		}
		InputDevice(InputController* c)
		: controller(c) {
		}
	};

	typedef map<uint16, InputDevice>::type ControllerMap;

	void _Register(InputListener* l, ControllerMap::iterator it) {
		if (it != controllerMap.end())
		(*it).second.listeners.push_back(l);
	}

	void _Unregister(InputListener* l, ControllerMap::iterator it) {
		if (it != controllerMap.end())
		(*it).second.listeners.remove(l);
	}

	virtual void Close();
	InputControllerProvider* provider;
	ControllerMap controllerMap;

};

}



#endif /* CORE_ENGINE_LOCAL_INCLUDE_INPUTMANAGERIMPL_H_ */
