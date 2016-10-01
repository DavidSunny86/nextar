/**
 * ApplicationContext.cpp
 * Created on: Wed Sep 28 01:43:19 IST 2016
 * @author: obhi
 */
#include <BaseHeaders.h>

NEX_IMPLEMENT_PLATFORM_APP()

int NextarMain(int argc, char* argv[]) {
	nextar::ACBaseImpl impl;
	nextar::ApplicationContext application("AppContext", impl);
	application.InitializeContext(argc, argv);
	application.DestroyContext();
	return 0;
}
