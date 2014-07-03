#ifndef UTAPPLICATION_H_
#define UTAPPLICATION_H_

#include <RenderEngineHeaders.h>
#include <EngineApplicationContext.h>

using namespace nextar;

class UTApplication: public FrameListener, public EngineApplicationContext {
public:

	UTApplication();
	~UTApplication();
	virtual void ConfigureExtendedInterfacesImpl();

	virtual void ReleaseResources();
	virtual void BeginFrame(uint32 frameNumber);
	virtual void EndFrame(uint32 timeElapsed);

protected:

	virtual void _SetupScene(SceneAssetPtr&);
	virtual void _SetupRenderDriver();
	virtual SceneAssetPtr _CreateDefaultScene();

	SceneAssetPtr scene;
	RenderTargetPtr window;
};

#endif //UTAPPLICATION_H_
