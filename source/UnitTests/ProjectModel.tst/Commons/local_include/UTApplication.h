#ifndef UTAPPLICATION_H_
#define UTAPPLICATION_H_

#include <ProjectModelHeaders.h>
#include <ProjectApplicationContext.h>

using namespace nextar;

class UTApplication: public FrameListener, public ProjectApplicationContext {
public:

	UTApplication();
	~UTApplication();
	virtual void ConfigureExtendedInterfacesImpl();

	virtual void ReleaseResources();
	virtual void BeginFrame(uint32 frameNumber);
	virtual void EndFrame(uint32 timeElapsed);

protected:

	virtual void _SetupScene(SceneAssetPtr&);
	virtual SceneAssetPtr _CreateDefaultScene();

	SceneAssetPtr scene;
};

#endif //UTAPPLICATION_H_
