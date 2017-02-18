#ifndef UTAPPLICATION_H_
#define UTAPPLICATION_H_

#include <RenderEngineHeaders.h>
#include <EngineApplicationContext.h>

using namespace nextar;

class UTApplication: public FrameListener, public EngineApplicationContext {
public:

	UTApplication();
	~UTApplication();
	virtual void ConfigureExtendedInterfacesImpl(Config& config);

	virtual void ReleaseResources();
	virtual void Execute(const FrameTimer& frameTimer);
	

	static void SetupScene(void* callback);
protected:

	virtual String GetRenderSystemsToInstantiate() const;
	virtual void SetupScene();
	virtual void _SetupScene(SceneAssetPtr&);
	virtual void _SetupRenderDriver();
	virtual SceneAssetPtr _CreateDefaultScene();

	Size windowDimensions;
	SceneAssetPtr scene;
	RenderTargetPtr window;
};

#endif //UTAPPLICATION_H_
