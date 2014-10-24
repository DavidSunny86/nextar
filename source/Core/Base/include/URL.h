#ifndef NEXTAR_PATHUTILS_H
#define NEXTAR_PATHUTILS_H

#include <NexBase.h>
#include <NexBase.h>
#include <NameValueMap.h>

namespace nextar {

class OutputSerializer;
class InputSerializer;
/* macro = relative_path
 * {archive}/${macro}/to.file
 * ArchiveEntry = #type:path;
 * Common macros:
 * ${Bin} refers to the location where the binary exists.
 * Common archives: (all should be used without the braces)
 * {FileSystem::ArchiveEngineData} 
 */
class _NexBaseAPI URL {

	NEX_LOG_HELPER (URL);
public:
	static const URL Invalid;

	URL();
	URL(const UniString& _path);
	URL(const String&);
	URL(const URL&);
	URL(const String& archive, const String& relativePath);

	inline const String& GetArchiveName() const {
		return archiveName;
	}

	inline const String& GetRelativePath() const {
		return relativePath;
	}

	/* operator == */
	bool operator ==(const URL& other) const;
	bool operator !=(const URL& other) const {
		return !(other == *this);
	}

	String ToString() const;

	static String GetAppendedPath(const String& base, const String& subPath);
	/* Reserved path */
	static bool IsReservedPath(const char* path);

	/* extension */
	String GetExtension() const;
	String GetComputedName() const;
	// returns the relative path where the file exists.
	// For Eg: If URL is {Archive}/mesh/file.fbx, this function returns -> {Archive}/mesh/
	String GetComputedFilePath() const;

	_NexBaseAPI friend OutputSerializer& operator << (OutputSerializer& ser, const URL& url);
	_NexBaseAPI friend InputSerializer& operator >> (InputSerializer& ser, URL& url);

protected:

	/* this is only called by file system */
	static void AddMacroEntry(const String& name, const String& value) {
		_macroTable[name] = value;
	}

	String _Resolve(const String& path, size_t offset);
	void _Parse(const String& path);

	String archiveName;
	String relativePath;

	friend class FileSystem;

	static NameValueMap _macroTable;
};

}

#endif //NEXTAR_PATHUTILS_H