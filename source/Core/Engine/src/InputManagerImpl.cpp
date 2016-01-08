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

InputManagerImpl::InputManagerImpl() : numController(0) {
}

InputManagerImpl::~InputManagerImpl() {
}

void InputManagerImpl::Configure(const Config& config) {
	uint32 baseId = BASE_DEV_ID;
	for(auto p : providers) {
		p->SetBaseDevId(baseId);
		baseId <<= 1;
		p->Configure(config);
		p->EnumControllers();
	}
	ApplicationContext::Listener l(this, ApplicationContext::PRIORITY_HIGH);
	ApplicationContext::Instance().RegisterListener(l);
	ApplicationContext::Instance().Subscribe(ApplicationContext::EVENT_DESTROY_RESOURCES,
			ReleaseControllers, this);
}

void InputManagerImpl::ReleaseControllers(void* me) {
	reinterpret_cast<InputManagerImpl*>(me)->UnregisterProviders();
}

void InputManagerImpl::RegisterProvider(InputControllerProvider* provider) {
	this->providers.push_back(provider);
}

void InputManagerImpl::UnregisterProviders() {
	while(providers.size()) {
		UnregisterProvider(providers.back());
	}
}

void InputManagerImpl::UnregisterProvider(InputControllerProvider* provider) {
	for(auto it = controllerMap.begin(); it != controllerMap.end(); ) {
		auto& c = (*it);
		if (provider == c.second.provider) {
			c.second.provider->DestroyController(c.second.controller);
			it = controllerMap.erase(it);
		} else
			++it;
	}
	providers.remove(provider);
}

InputControllerProvider* InputManagerImpl::GetProvider(uint16 deviceId) {
	uint32 baseId = BASE_DEV_ID;
	for(auto& p : providers) {
		if (baseId & deviceId) {
			if (!((baseId << 1) & deviceId)) {
				return p;
			}
		}
		baseId <<= 1;
	}
	return nullptr;
}

void InputManagerImpl::RegisterController(uint16 deviceId) {
	ControllerMap::iterator it = controllerMap.find(deviceId);
	if (it != controllerMap.end()) {
		Error("Device ID already registered for a different device!");
		return;
	}

	// get provider id
	InputControllerProvider* p = GetProvider(deviceId);
	if (p) {
		InputController* controls = p->CreateController(deviceId);
		if (controls)
			controllerMap.insert(
					ControllerMap::value_type(deviceId,
							InputDevice(p, controls)));
	}
}

void InputManagerImpl::UnregisterController(uint16 deviceId) {
	ControllerMap::iterator it = controllerMap.find(deviceId);
	if (it != controllerMap.end()) {
		if ((*it).second.provider)
			(*it).second.provider->DestroyController((*it).second.controller);
		controllerMap.erase(it);
	}
}

const InputControllerDesc& InputManagerImpl::GetControllerDesc(uint32 n) {
	uint32 runningCount = 0;
	for(auto p : providers) {
		uint32 numController = p->GetNumController();
		if (runningCount + numController > n)
			return p->GetControllerDesc(n - runningCount);
		runningCount += numController;
	}

	return InputControllerDesc::Null;
}

const InputControllerDesc& InputManagerImpl::GetControllerDescById(uint16 devId) {
	for (auto p : providers) {
		uint32 numController = p->GetNumController();
		for (uint32 n = 0; n < numController; ++n) {
			const InputControllerDesc& desc = p->GetControllerDesc(n);
			if (desc.deviceId == devId) {
				return desc;
			}
		}
	}

	return InputControllerDesc::Null;
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

void InputManagerImpl::SetProperty(uint16 deviceId,
	const String& propertyName, const String& value) {
	auto it = controllerMap.find(deviceId);
	if (it != controllerMap.end()) {
		if ((*it).second.controller)
			(*it).second.controller->SetProperty(propertyName, value);
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

uint32 InputManagerImpl::GetNumController() {
	if (!numController) {
		for(auto p : providers) {
			numController += p->GetNumController();
		}
	}
	return numController;
}

}
