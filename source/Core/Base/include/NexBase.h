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
class Stream;
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
typedef RefPtr<Stream> StreamPtr;
typedef RefPtr<InputStream> InputStreamPtr;
typedef RefPtr<OutputStream> OutputStreamPtr;
typedef RefPtr<MemoryInputStream> MemoryInputStreamPtr;
typedef RefPtr<MemoryOutputStream> MemoryOutputStreamPtr;
typedef RefPtr<GenericInputStream> GenericInputStreamPtr;
typedef RefPtr<GenericOutputStream> GenericOutputStreamPtr;
typedef RefPtr<FileInputStream> FileInputStreamPtr;
typedef RefPtr<FileOutputStream> FileOutputStreamPtr;

enum class BaseConstants
	: uint32 {
	NUM_POINTER_PER_POOL_BLOCK = 128,
	NUM_16B_PER_BLOCK = 128,
	NUM_64B_PER_BLOCK = 32,
	NUM_MATRIX_PER_BLOCK = NUM_64B_PER_BLOCK,
	NUM_BV_PER_BLOCK = NUM_64B_PER_BLOCK,
	NUM_EVENT_PER_BLOCK = 16
};

}

#endif //NEXBASE_H__
