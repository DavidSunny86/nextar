#ifndef NEXBASE_H__
#define NEXBASE_H__

#include <BaseTypes.h>
#include <RefPtr.h>

namespace nextar {

	namespace debug {
	}


	class FactoryPlugin;
	class PluginRegistry;
	class Config;
	class URL;
	class InputStream;
	class OutputStream;
	class Archive;
	class FileSystem;
	class FrameListener;
	class RenderWindow;
	class WindowManager;
	class LogManager;

	class Frustum;

	typedef RefPtr<Archive> ArchivePtr;
	typedef RefPtr<InputStream> InputStreamPtr;
	typedef RefPtr<OutputStream> OutputStreamPtr;

}

#endif //NEXBASE_H__

