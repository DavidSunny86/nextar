
#ifndef UTAPPLICATION_H_
#define UTAPPLICATION_H_

#include <RenderEngineHeaders.h>
#include <ApplicationContext.h>

using namespace nextar;

class UTApplication : public FrameListener,
	public ApplicationContext {
public:

	UTApplication();
	virtual void ConfigureExtendedInterfacesImpl();

	virtual void BeginFrame(uint32 frameNumber);
	virtual void EndFrame(uint32 timeElapsed);

protected:
	
	virtual void _SetupScene(SceneAssetPtr&);
	virtual void _SetupRenderDriver();
	virtual SceneAssetPtr _CreateDefaultScene();

	RenderTargetPtr window;
};

#endif //UTAPPLICATION_H_
