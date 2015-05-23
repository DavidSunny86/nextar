/**
 * UxGenSymToKeyMap.cpp
 * Created on: Mon May 11 15:47:54 IST 2015
 * @author: obhi
 */

#include <EngineHeaders.h>
#include <KeyCode.h>

using namespace nextar;
NEX_IMPLEMENT_PLATFORM_APP();

int NextarMain(int argc, char* argv[]) {
	EngineApplicationContext application("GenSymMap");
	application.InitializeContext(argc, argv);

	URL path = URL(FileSystem::ArchiveEngineData_Name, "Configs/UxKeySyms.dat");
	OutputStreamPtr ostr = FileSystem::Instance().OpenWrite(path, false);
	OutputSerializer oser(ostr);
	String name = "UxKeyMap";
	VersionID ver = NEX_MAKE_VERSION(1,0,1);
	String version = Convert::ToVersionString(ver);
	oser << name << version;
	uint32 keySym[NEX_KEYBOARD_KEY_COUNT] = {0};
	OutputSerializer::UIntArray a(keySym, NEX_KEYBOARD_KEY_COUNT);

#define MAP_KEY(a, b) keySym[(uint32)Key::a] = b;
#include <KeySymMap.h>
#undef MAP_KEY

	oser << a;
	application.DestroyContext();

	std::cin.get();
	return 0;
}
