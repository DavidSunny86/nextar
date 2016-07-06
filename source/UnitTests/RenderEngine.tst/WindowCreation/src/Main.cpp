#include <EngineHeaders.h>
#include <UTApplication.h>

NEX_IMPLEMENT_APP()
;

using namespace nextar;

void SetupDriver(RenderManager& renderManager) {
	RenderDriverPtr driver = renderManager.AsyncGetDriver(0);
	NameValueMap params;
	params["WindowTitle"] = "Secondary Window";
	params["WindowLeft"] = "801";
	params["WindowTop"] = "0";
	driver->AsyncGetContext(0)->CreateRenderWindow(800, 600, false, &params);
}

int NextarMain(int argc, char* argv[]) {
	UTApplication impl;
	nextar::ApplicationContext application("WindowCreation", impl);
	application.InitializeContext(argc, argv);
	RenderManager& renderManager = RenderManager::Instance();
	SetupDriver(renderManager);
	application.Run();
	application.DestroyContext();
	return 0;
}

