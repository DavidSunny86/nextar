/* 
 * File:   InputManagerImpl.cpp
 * Author: obhi
 * 
 * Created on June 10, 2011, 8:28 PM
 */
#include <InputManagerImpl.h>
#include <ApplicationContext.h>

namespace nextar {

NEX_DEFINE_SINGLETON_PTR(InputManager);

const InputControllerDesc InputControllerDesc::Null;

InputManagerImpl::InputManagerImpl() : provider(nullptr) {
}

InputManagerImpl::~InputManagerImpl() {
}

void InputManagerImpl::Configure(const Config& config) {
	if (provider)
		provider->Configure(config);
	ApplicationContext::Listener l(this, ApplicationContext::PRIORITY_HIGH);
	ApplicationContext::Instance().RegisterListener(l);
}

void InputManagerImpl::RegisterProvider(InputControllerProvider* provider) {
	this->provider = provider;
	this->provider->EnumControllers();
}

void InputManagerImpl::UnregisterProvider() {
	if (provider) {
		for(auto& c : controllerMap) {
			provider->DestroyController(c.second.controller);
		}
	}
	controllerMap.clear();
}

void InputManagerImpl::RegisterController(uint16 deviceId) {
	ControllerMap::iterator it = controllerMap.find(deviceId);
	if (it != controllerMap.end()) {
		Error("Device ID already registered for a different device!");
		return;
	}

	if(provider) {
		InputController* controls = provider->CreateController(deviceId);
		controllerMap.insert(
			ControllerMap::value_type(deviceId,
			InputDevice(controls)));
	}
}

void InputManagerImpl::UnregisterController(uint16 deviceId) {
	ControllerMap::iterator it = controllerMap.find(deviceId);
	if (it != controllerMap.end()) {
		if (provider)
			provider->DestroyController((*it).second.controller);
		controllerMap.erase(it);
	}
}

void InputManagerImpl::ProcessDevices() {
	ControllerMap::iterator en = controllerMap.end();
	for (ControllerMap::iterator it = controllerMap.begin(); it != en; ++it) {
		InputChangeBuffer change = (*it).second.controller->UpdateSettings();
		if (change.second) {
			ListenerList& ll = (*it).second.listeners;
			ListenerList::iterator len = ll.end();
			for (ListenerList::iterator l = ll.begin(); l != len; ++l)
				(*l)->ProcessEvents(change);
		}
	}
}

void InputManagerImpl::RegisterListener(InputListener* l, uint16 deviceId) {
	if (deviceId == (uint16) - 1) {
		ControllerMap::iterator en = controllerMap.end();
		for (ControllerMap::iterator it = controllerMap.begin(); it != en; ++it)
			_Register(l, it);
	} else {
		_Register(l, controllerMap.find(deviceId));
	}
}

void InputManagerImpl::UnregisterListener(InputListener* l, uint16 deviceId) {
	if (deviceId == (uint16) - 1) {
		ControllerMap::iterator en = controllerMap.end();
		for (ControllerMap::iterator it = controllerMap.begin(); it != en; ++it)
			_Unregister(l, it);
	} else {
		_Unregister(l, controllerMap.find(deviceId));
	}
}

void InputManagerImpl::Close() {
	ApplicationContext::Listener l(this, ApplicationContext::PRIORITY_HIGH);
	ApplicationContext::Instance().UnregisterListener(l);
	controllerMap.clear();
}

void InputManagerImpl::Execute(const FrameTimer& frameTimer) {
	ProcessDevices();
}

}
