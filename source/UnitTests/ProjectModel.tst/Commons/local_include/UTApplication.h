#ifndef UTAPPLICATION_H_
#define UTAPPLICATION_H_

#include <ProjectModelHeaders.h>
#include <ProjectApplicationContext.h>
#include <RenderTarget.h>

using namespace nextar;

class UTApplication: public FrameListener, public ProjectApplicationContext {
public:

	UTApplication();
	~UTApplication();
	virtual void ConfigureExtendedInterfacesImpl();

	virtual void ReleaseResourcesImpl();
	virtual void BeginFrame(uint32 frameNumber);
	virtual void EndFrame(uint32 timeElapsed);

	static void SetupScene(void* callback);
protected:

	virtual void SetupScene();
	virtual void _SetupScene(SceneAssetPtr&);
	virtual void _SetupRenderDriver();
	virtual SceneAssetPtr _CreateDefaultScene();

	SceneAssetPtr scene;
	RenderTargetPtr window;
};

#endif //UTAPPLICATION_H_