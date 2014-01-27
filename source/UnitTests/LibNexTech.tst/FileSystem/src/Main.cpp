#include "NexHeaders.h"

NEX_IMPLEMENT_PLATFORM_APP();

void TestArchiveOpenClose(nextar::ApplicationContext& application) {
	nextar::FileSystem& fileSys = nextar::FileSystem::Instance();
	fileSys.AddArchive("Data", nextar::Archive::TYPE_ARC,
			"${EngineData}/config/localization.zip");
	nextar::InputStreamPtr inputFile = fileSys.OpenRead(
			nextar::URL(nextar::String("${Data}/Config.cfg")));
	nextar::OutputStreamPtr out = nextar::Assign(
			NEX_NEW nextar::GenericOutputStream(&std::cout));
	out->AddRef();
	nextar::FileSystem::CopyStream(inputFile, out);
	std::cout << std::endl;
	inputFile.Clear();
}

int NextarMain(int argc, char* argv[]) {
	nextar::ApplicationContext application("FileSystem");
	application.InitializeContext();
	TestArchiveOpenClose(application);
	application.DestroyContext();
	std::cin.get();
	return 0;
}

