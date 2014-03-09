#ifndef NEXTAR_NEXSYS_H__
#define NEXTAR_NEXSYS_H__

#include "BaseTypes.h"
#include "RefPtr.h"
//#define NEX_MAX_STREAMS 8
//#define NEX_MAS_VERTEX_ELEMENTS 32

namespace nextar {

//typedef RefPtr<Material> MaterialPtr;

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
	class WindowManager;
	class LogManager;

	class Frustum;

	typedef RefPtr<Archive> ArchivePtr;
	typedef RefPtr<InputStream> InputStreamPtr;
	typedef RefPtr<OutputStream> OutputStreamPtr;

}

#endif //NEXTAR_NEXSYS_H__

