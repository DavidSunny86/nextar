/**
 * ScreenQuad.cpp
 * Created on: Sun, Nov  1, 2015  1:36:52 AM
 * @author: obhi
*/

#include <EngineHeaders.h>
#include <UTApplication.h>

NEX_IMPLEMENT_APP()
;

using namespace nextar;

class UTApplicationImageLoader : public UTApplication {
	
public:
	virtual void _SetupScene(SceneAssetPtr& scene) {
		UTApplication::_SetupScene(scene);
		
	}
};

int NextarMain(int argc, char* argv[]) {
	UTApplicationImageLoader application;
	application.InitializeContext(argc, argv);
	application.Run();
	application.DestroyContext();
	return 0;
}
