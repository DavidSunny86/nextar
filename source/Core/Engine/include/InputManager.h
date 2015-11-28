/* 
 * File:   InputManager.h
 * Author: obhi
 *
 * Created on June 10, 2011, 8:28 PM
 */

#ifndef INPUTMANAGER_H
#define	INPUTMANAGER_H

#include <InputController.h>

namespace nextar {

class _NexEngineAPI InputControllerProvider {
public:

	virtual void Configure(const Config&) = 0;
	virtual void SetBaseDevId(uint32 base) = 0;
	virtual void EnumControllers() = 0;
	virtual uint32 GetNumController() = 0;
	virtual const InputControllerDesc& GetControllerDesc(uint32 n) = 0;
	virtual InputController* CreateController(uint16 deviceId) = 0;
	virtual void DestroyController(InputController*) = 0;

protected:
	~InputControllerProvider() {}
};

class _NexEngineAPI InputManager:
	public Singleton<InputManager>,
	public AllocGeneral {
public:

	virtual ~InputManager() {}

	virtual void RegisterProvider(InputControllerProvider*) = 0;
	virtual void UnregisterProvider(InputControllerProvider*) = 0;
	virtual void UnregisterProviders() = 0;

	virtual uint32 GetNumController() = 0;
	virtual const InputControllerDesc& GetControllerDesc(uint32 n) = 0;
	virtual const InputControllerDesc& GetControllerDescById(uint16 n) = 0;

	virtual void Configure(const Config&) = 0;
	virtual void RegisterController(uint16 deviceId) = 0;
	virtual void UnregisterController(uint16 deviceId) = 0;

	virtual void RegisterListener(InputListener*, uint16 deviceId = -1) = 0;
	virtual void UnregisterListener(InputListener*, uint16 deviceId = -1) = 0;

	virtual void Close() = 0;
};

}
#endif	/* INPUTMANAGER_H */

