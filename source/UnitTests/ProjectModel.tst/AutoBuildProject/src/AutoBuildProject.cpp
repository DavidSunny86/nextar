/**
 * AutoBuildProject.cpp
 * Created on: Wed Sep 21 01:05:06 IST 2016
 * @author: obhi
 */
#include <ProjectModelHeaders.h>
#include <UTApplication.h>

NEX_IMPLEMENT_APP()
;

using namespace nextar;

class UTApplicationAutoBuildProject: public UTApplication {

	NEX_LOG_HELPER(UTApplicationAutoBuildProject);
public:

	UTApplicationAutoBuildProject() {
	}

	virtual void _SetupScene(SceneAssetPtr& scene) {
		UTApplication::_SetupScene(scene);
	}

	void Execute() {
		ResourcePackerService* service =
				PluginRegistry::Instance().QueryService<ResourcePackerService>();
		if (service) {
			service->BuildProject(GetProjectContext());
			service->Destroy();
		}
	}
};

int NextarMain(int argc, char* argv[]) {
	UTApplicationAutoBuildProject impl;
	nextar::ApplicationContext application("AutoBuildProject", impl);
	application.InitializeContext(argc, argv);
	impl.Execute();
	application.DestroyContext();
	return 0;
}
