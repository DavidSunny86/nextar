#include <BaseHeaders.h>
#include <OsDefs.h>

namespace nextar {

String Platform::GetModuleLocation() {
#if NEX_WINDOWS == 1
	char result[ MAX_PATH ] = "";
	GetModuleFileNameA(NULL, result, MAX_PATH);
	char* w = std::strrchr(result, '\\');
	if (w)
	*w = 0;
	return String( result );
#elif NEX_LINUX == 1
	char result[PATH_MAX] = "";
	size_t count = readlink("/proc/self/exe", result, PATH_MAX);
	char* w = std::strrchr(result, '/');
	if (w)
		*w = 0;
	return String(result);
#else
#error Dont know how to set a path here.
	return StringUtils::Null;
#endif
}

}
