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
	virtual void UnregisterProvider(InputControllerProvider*);
	virtual void UnregisterProviders();

	virtual void RegisterController(uint16 deviceId);
	virtual void UnregisterController(uint16 deviceId);

	virtual void RegisterListener(InputListener*, uint16 deviceId = -1);
	virtual void UnregisterListener(InputListener*, uint16 deviceId = -1);

	virtual uint32 GetNumController();
	virtual const InputControllerDesc& GetControllerDesc(uint32 n);

	virtual void Execute(const FrameTimer& frameTimer);
	virtual void Close();

private:

	static void ReleaseControllers(void* me);

	enum {
		BASE_DEV_ID = 1024
	};

	InputControllerProvider* GetProvider(uint16 devId);
	void ProcessDevices();

	typedef list<InputListener*>::type ListenerList;
	typedef list<InputControllerProvider*>::type ProviderList;

	struct InputDevice {
		InputController* controller;
		ListenerList listeners;
		InputControllerProvider* provider;

		InputDevice()
		: provider(nullptr), controller(nullptr) {
		}
		InputDevice(InputControllerProvider* p, InputController* c)
		: provider(p), controller(c) {
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

	uint32 numController;
	ProviderList providers;
	ControllerMap controllerMap;
};

}



#endif /* CORE_ENGINE_LOCAL_INCLUDE_INPUTMANAGERIMPL_H_ */
