/*
 * InputControllerProviderv10.h
 *
 *  Created on: 21-Mar-2015
 *      Author: obhi
 */

#ifndef PLUGINS_INPUTCONTROLLER_INPUTCONTROLLERPROVIDERV10_H_
#define PLUGINS_INPUTCONTROLLER_INPUTCONTROLLERPROVIDERV10_H_

#include <EngineHeaders.h>
#include <InputController.h>
#include <InputManager.h>

using namespace nextar;
namespace InputService {

class InputControllerProviderv1_0 : public InputControllerProvider {
public:

	InputControllerProviderv1_0();
	virtual ~InputControllerProviderv1_0();

	virtual void Configure(const Config&);
	virtual void EnumControllers();
	virtual uint32 GetNumController();
	virtual const InputControllerDesc& GetControllerDesc(uint32 n);
	virtual InputController* CreateController(uint16 deviceId);
	virtual void DestroyController(InputController*);
	virtual void SetBaseDevId(uint32 base);

protected:
	virtual void EnumDevicesImpl() = 0;

	uint32 baseId;

};

} /* namespace InputProvider */

#endif /* PLUGINS_INPUTCONTROLLER_INPUTCONTROLLERPROVIDERV10_H_ */
