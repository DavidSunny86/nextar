
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
	
	virtual void _SetupScene(ScenePtr&);
	virtual void _SetupRenderDriver();
	virtual ScenePtr _CreateDefaultScene();

	RenderWindowPtr window;
};
