#include <BaseHeaders.h>

NEX_IMPLEMENT_PLATFORM_APP()
;

void TestArchiveOpenClose(nextar::ApplicationContext& application) {
	nextar::FileSystem& fileSys = nextar::FileSystem::Instance();
	fileSys.AddArchive("StringData", nextar::Archive::TYPE_ARC,
		nextar::FileSystem::ArchiveEngineData + "/Configs/localization.zip");
	nextar::InputStreamPtr inputFile = fileSys.OpenRead(
			nextar::URL(nextar::String("{StringData}/Config.cfg")));
	nextar::OutputStreamPtr out = nextar::Assign(
			NEX_NEW(nextar::GenericOutputStream(&std::cout)));
	nextar::FileSystem::CopyStream(inputFile, out);
	std::cout << std::endl;
	inputFile.Clear();
}

int NextarMain(int argc, char* argv[]) {
	nextar::ApplicationContext application("FileSystem");
	application.InitializeContext(argc, argv);
	TestArchiveOpenClose(application);
	application.DestroyContext();
	std::cin.get();
	return 0;
}

