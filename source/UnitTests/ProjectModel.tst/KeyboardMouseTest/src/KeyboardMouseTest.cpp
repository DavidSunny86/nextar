/** Created on: Sun May 10 15:31:02 IST 2015
 *  @author: obhi
 */
#include <EngineHeaders.h>
#include <TaskSchedular.h>
#include <InputManager.h>
#include <UTApplication.h>

using namespace nextar;
NEX_IMPLEMENT_PLATFORM_APP();

int NextarMain(int argc, char* argv[]) {
	UTApplication impl;
	nextar::ApplicationContext application("KeyboardAndMouseTest", impl);
	application.InitializeContext(argc, argv);

	uint32 nController = nextar::InputManager::Instance().GetNumController();
	for(uint32 i = 0; i < nController; ++i) {
		auto& desc = nextar::InputManager::Instance().GetControllerDesc(i);
		if (desc.type == ControllerType::TYPE_KEYBOARD_AND_MOUSE)
			nextar::InputManager::Instance().RegisterController(desc.deviceId);
	}
	application.Run();
	application.DestroyContext();

	std::cin.get();
	return 0;
}
