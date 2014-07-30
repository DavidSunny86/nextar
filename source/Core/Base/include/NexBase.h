#ifndef NEXBASE_H__
#define NEXBASE_H__

#include <BaseTypes.h>
#include <RefPtr.h>
#include <OsDefs.h>

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

class MemoryInputStream;
class MemoryOutputStream;
class GenericInputStream;
class GenericOutputStream;
class FileInputStream;
class FileOutputStream;

class Frustum;

typedef RefPtr<Archive> ArchivePtr;
typedef RefPtr<InputStream> InputStreamPtr;
typedef RefPtr<OutputStream> OutputStreamPtr;
typedef RefPtr<MemoryInputStream> MemoryInputStreamPtr;
typedef RefPtr<MemoryOutputStream> MemoryOutputStreamPtr;
typedef RefPtr<GenericInputStream> GenericInputStreamPtr;
typedef RefPtr<GenericOutputStream> GenericOutputStreamPtr;
typedef RefPtr<FileInputStream> FileInputStreamPtr;
typedef RefPtr<FileOutputStream> FileOutputStreamPtr;

}

#endif //NEXBASE_H__
