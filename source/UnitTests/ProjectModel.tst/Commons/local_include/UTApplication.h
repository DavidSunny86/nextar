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
	virtual void ConfigureExtendedInterfacesImpl(Config& config) override;

	virtual void ReleaseResources();
	virtual void Execute(const FrameTimer& frameTimer);
	
	static void SetupScene(void* callback);
protected:

	virtual void SetupScene();
	virtual void _SetupScene(SceneAssetPtr&);
	virtual void _SetupRenderDriver();
	virtual SceneAssetPtr _CreateDefaultScene();

	Size windowDimensions;
	SceneAssetPtr scene;
	RenderTargetPtr window;
};

#endif //UTAPPLICATION_H_
