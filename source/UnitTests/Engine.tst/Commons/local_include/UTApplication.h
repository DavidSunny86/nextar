#ifndef UTAPPLICATION_H_
#define UTAPPLICATION_H_

#include <EngineHeaders.h>
#include <EngineApplicationContext.h>

using namespace nextar;

class UTApplication: public FrameListener, public EngineApplicationContext {
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
	virtual SceneAssetPtr _CreateDefaultScene();

	SceneAssetPtr scene;
};

#endif //UTAPPLICATION_H_
