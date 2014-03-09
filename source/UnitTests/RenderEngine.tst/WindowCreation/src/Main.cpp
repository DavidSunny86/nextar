
#include "BaseHeaders.h"

NEX_IMPLEMENT_APP();

using namespace nextar;

void SetupDriver(RenderManager& renderManager) {
	RenderManager::DriverCreationParams dcp;
	dcp.gpuIndex = 0;
	dcp.createDefaultContext = true;
	dcp.defaultContextParams.createDefaultWindow = true;
	dcp.defaultContextParams.defaultWindowWidth = 800;
	dcp.defaultContextParams.defaultWindowHeight = 600;
	dcp.defaultContextParams.depthBits = 24;
	dcp.defaultContextParams.stencilBits = 8;
	dcp.defaultContextParams.fullScreen = false;
	dcp.defaultContextParams.monitorIndex = 0;
	dcp.defaultContextParams.multiSamples = 0;
	dcp.defaultContextParams.reqOpenGLVersionMajor = 3;
	dcp.defaultContextParams.reqOpenGLVersionMinor = 2;
	dcp.defaultContextParams.sharedContextIndex = -1;
	dcp.defaultContextParams.extraParams["IsMainWindow"] = "true";
	dcp.defaultContextParams.extraParams["ExitOnClose"] = "true";
	dcp.defaultContextParams.extraParams["WindowLeft"] = "0";
	dcp.defaultContextParams.extraParams["WindowTop"] = "0";
	RenderDriverPtr driver = renderManager.AsyncCreateDriver(dcp);
	NameValueMap params;
	params["WindowTitle"] = "Secondary Window";
	params["WindowLeft"] = "801";
	params["WindowTop"] = "0";
	driver->AsyncGetContext(0)->CreateGraphicsWindow(800, 600, false, &params);
}

int NextarMain(int argc, char* argv[]) {
	nextar::ApplicationContext application("WindowCreation");
	application.InitializeContext();
	RenderManager& renderManager = application.GetRenderManager();
	SetupDriver(renderManager);
	application.Run();
	application.DestroyContext();
	return 0;
}

