
#include <BaseHeaders.h>
#include <Archive.h>
#include <GenericStreams.h>
#include <Directory.h>
#include <URL.h>
#include <ApplicationContext.h>
#include <FileSystem.h>

#if defined(NEX_MSVC)
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <Windows.h>

#define NEX_S_IFDIR(mode) (_S_IFDIR & mode)
#define NEX_S_IWRITE(mode) (_S_IWRITE & mode)
#define NEX_S_IREAD(mode) (_S_IREAD & mode)

#elif defined(NEX_GCC)

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define NEX_S_IFDIR(mode) (S_IFDIR & mode)
#define NEX_S_IWRITE(mode) (S_IWRITE & mode)
#define NEX_S_IREAD(mode) (S_IREAD & mode)

#else
#error Has no implementation for directory structure api.
#endif

namespace nextar {

	Directory::Factory Directory::Factory::directoryFactoryObject;

#if NEX_WINDOWS == 1
	static void WriteFileTime(FileTime& t, const FILETIME& ft) {
		SYSTEMTIME sysTime;
		FileTimeToSystemTime(&ft, &sysTime);
		t.day = (int8) sysTime.wDay;
		t.mon = (int8) sysTime.wMonth;
		t.year = (int8) sysTime.wYear;
		t.hrs = (int8) sysTime.wHour;
		t.min = (int8) sysTime.wMinute;
		t.year = (int8) sysTime.wYear;
	}
#endif

	static void StatToAttribute(struct stat& filestat, FileAttribute& attr) {

		if (NEX_S_IFDIR(filestat.st_mode))
			attr.flags = FileAttribute::ATTRIB_DIR;
		if (!NEX_S_IWRITE(filestat.st_mode))
			attr.flags |= FileAttribute::ATTRIB_READONLY;
		NEX_SET_FILE_TIME(attr.fileTime, filestat.st_mtime);
		attr.compressedSize = filestat.st_size;
		attr.uncompressedSize = filestat.st_size;
	}

//------------------------------------------------------------
	void Directory::Factory::Register(FileSystem* fileSys) {
		fileSys->AddFactory(Directory::TYPE, &directoryFactoryObject);
	}

	Directory::Directory(const String& name, const String& _paths) :
			Archive(name), paths(_paths) {
	}

	Directory::~Directory() {
	}

	InputStreamPtr Directory::OpenRead(const String& path,
			const String& filename) {
		String fullpath = URL::GetAppendedPath(path, filename);
		std::FILE* file = 0;
#if NEX_LINUX == 1
		file = std::fopen(fullpath.c_str(), "rb");
#elif NEX_WINDOWS == 1
		FOPEN_W(file, StringUtils::ToUtf16(fullpath).c_str(), L"rb");
#else
#error no standard way to open files?
#endif
		if (!file) {
			return InputStreamPtr();
		}
		FileInputStream* ptr = NEX_NEW FileInputStream(file,
				CONST_INVALID_SIZE);
		return Assign<InputStream>(ptr);
	}

	OutputStreamPtr Directory::OpenWrite(const String& path,
			const String& filename, bool append) {
		String fullpath = URL::GetAppendedPath(path, filename);
		std::FILE* file = 0;
#if NEX_LINUX == 1
		file = std::fopen(fullpath.c_str(), append ? "ab" : "wb");
#elif NEX_WINDOWS == 1
		FOPEN_W(file, StringUtils::ToUtf16(fullpath).c_str(), append ? L"ab" : L"wb");
#else
#error no standard way to open files?
#endif
		if (!file) {
			return OutputStreamPtr();
		}
		FileOutputStream* ptr = NEX_NEW FileOutputStream(file,
				CONST_INVALID_SIZE);
		return Assign<OutputStream>(ptr);
	}

	InputStreamPtr Directory::OpenRead(const String& fileName) {
		StringUtils::TokenIterator it = 0;
		String path;
		if (paths.length()) {
			while ((it = StringUtils::NextWord(paths, path, it)) != String::npos) {
				InputStreamPtr ptr = OpenRead(path, fileName);
				if (ptr)
					return ptr;
			}
		} else
			return OpenRead("", fileName);
		return InputStreamPtr();
	}

	OutputStreamPtr Directory::OpenWrite(const String& fileName, bool append) {
		StringUtils::TokenIterator it = 0;
		String path;
		String pathStr;
		while ((it = StringUtils::NextWord(paths, path, it)) != String::npos) {
			OutputStreamPtr ptr = OpenWrite(path, fileName, append);
			if (ptr)
				return ptr;
		}
		return OutputStreamPtr();
	}

	void Directory::Scan(Archive::ScanCallback* callback, const String& pattern,
			bool recursive) {
		StringVector pathstack;
		StringUtils::TokenIterator it = 0;
		String path;
		while ((it = StringUtils::NextWord(paths, path, it)) != String::npos) {
			_VisitDirectory(path, pattern, StringUtils::Null, callback,
					recursive);
		}
	}

	void Directory::_VisitDirectory(const String& path, const String& pattern,
			const String& subPath, ScanCallback* callback, bool recursive) {
		FileAttribute attr;

		String fullPath;
		if (pattern.length())
			fullPath = URL::GetAppendedPath(path, pattern);
		else
			fullPath = path;
#if defined(NEX_MSVC)

		UniString fullPathUni = StringUtils::ToUtf16(fullPath);
		intptr_t fileHandle, res;
		struct _wfinddata_t data;
		fileHandle = _wfindfirst((const wchar_t*)fullPathUni.c_str(), &data);
		res = 0;
		while (fileHandle != -1 && res != -1) {
			String utf8path = StringUtils::ToUtf8((const char16_t*)data.name);
			String fullPath = URL::GetAppendedPath(path, utf8path);
			attr.fileName = URL(Directory::TYPE, GetName(), URL::GetAppendedPath(subPath, utf8path));
			attr.flags = 0;
			if ((data.attrib & _A_SUBDIR)) {
				if ((!URL::IsReservedPath(utf8path.c_str())) && recursive)
				_VisitDirectory(fullPath, pattern, attr.fileName.GetRelativePath(), callback, true);
				attr.flags = FileAttribute::ATTRIB_DIR;
			} else if (data.attrib * _A_ARCH)
			attr.flags = FileAttribute::ATTRIBUTE_ARCH;
			if (data.attrib * _A_RDONLY)
			attr.flags |= FileAttribute::ATTRIB_READONLY;
			NEX_SET_FILE_TIME(attr.fileTime, data.time_write);
			attr.compressedSize = data.size;
			attr.uncompressedSize = data.size;
			callback->FoundFile(attr, this);
			res = _wfindnext(fileHandle, &data);
		}

		if (fileHandle != -1)
		_findclose(fileHandle);
#elif defined(NEX_GCC)
		/** todo Fix this */
		DIR *dp;
		struct dirent *dirp;
		dp = opendir(fullPath.c_str());
		if (dp) {
			while ((dirp = readdir(dp)) != NULL) {
				if (!pattern.length()
						|| StringUtils::PatternMatch(pattern.c_str(),
								dirp->d_name, true)) {
					String fullPath = URL::GetAppendedPath(path, dirp->d_name);
					attr.fileName = URL(Directory::TYPE, GetName(),
							URL::GetAppendedPath(subPath, dirp->d_name));
					attr.flags = 0;
					struct stat filestat;
					int ret = stat(fullPath.c_str(), &filestat);
					if (ret == 0)
						StatToAttribute(filestat, attr);
					if (recursive && (attr.flags & FileAttribute::ATTRIB_DIR)
							&& !URL::IsReservedPath(dirp->d_name))
						_VisitDirectory(fullPath, pattern,
								attr.fileName.GetRelativePath(), callback,
								true);
				}
			}
			closedir(dp);
		}
#endif
	}

	class CollectFileNames: public Archive::ScanCallback {
		StringVector& strArr;
	public:
		CollectFileNames(StringVector& arr) :
				strArr(arr) {
		}

		virtual void FoundFile(const FileAttribute& attr, Archive*) override {
			strArr.push_back(attr.fileName.GetRelativePath());
		}
	};

	void Directory::GetFileList(StringVector& arr, bool listOnlyFiles) {
		CollectFileNames cfn(arr);
		Scan(&cfn, listOnlyFiles ? "*.*" : StringUtils::Null, false);
	}

	bool Directory::GetAttribute(const String& fileName,
			nextar::FileAttribute & attr) {

		StringUtils::TokenIterator it = 0;
		String path;
		while ((it = StringUtils::NextWord(paths, path, it)) != String::npos) {
			String fullPath = URL::GetAppendedPath(path, fileName);

#if NEX_WINDOWS==1
			if (StringUtils::IsAscii(fullPath)) {
				struct stat filestat;
				int ret = stat(fullPath.c_str(), &filestat);
				if (ret == 0) {
					attr.fileName = URL(Directory::TYPE, GetName(), fileName);
					attr.flags = 0;
					StatToAttribute(filestat, attr);
				} else
				return false;
			} else {
				UniString fullPathUni = StringUtils::ToUtf16(fullPath);
				DWORD flags = GetFileAttributesW((const wchar_t*)fullPathUni.c_str());
				if (flags == INVALID_FILE_ATTRIBUTES)
				return false;
				else
				attr.fileName = URL(Directory::TYPE, GetName(), fileName);
				if (flags & FILE_ATTRIBUTE_DIRECTORY)
				attr.flags = FileAttribute::ATTRIB_DIR;
				if ((flags & FILE_ATTRIBUTE_READONLY))
				attr.flags |= FileAttribute::ATTRIB_READONLY;
				if ((flags & FILE_ATTRIBUTE_ARCHIVE))
				attr.flags |= FileAttribute::ATTRIBUTE_ARCH;
				if ((flags & FILE_ATTRIBUTE_COMPRESSED))
				attr.flags |= FileAttribute::ATTRIBUTE_COMPRESSED;
				HANDLE hFile = CreateFileW((const wchar_t*)fullPathUni.c_str(), GENERIC_READ,
						FILE_SHARE_READ,
						NULL,
						OPEN_EXISTING,
						0,
						NULL);
				if (hFile != INVALID_HANDLE_VALUE) {
					FILETIME lastWriteTime;
					if (GetFileTime(hFile, NULL, NULL, &lastWriteTime))
					WriteFileTime(attr.fileTime, lastWriteTime);
				}

				attr.uncompressedSize = GetFileSize(hFile, NULL);
				if ((flags & FILE_ATTRIBUTE_COMPRESSED))
				attr.compressedSize = GetCompressedFileSizeW((const wchar_t*)fullPathUni.c_str(), NULL);
			}

#elif NEX_LINUX==1
			struct stat filestat;
			int ret = stat(fullPath.c_str(), &filestat);
			if (ret == 0) {
				attr.fileName = URL(Directory::TYPE, GetName(), fileName);
				attr.flags = 0;
				StatToAttribute(filestat, attr);
			} else
				return false;
#else //problem
#	error Platform error
#endif
			return true;
		}
		return false;
	}

	bool Directory::Exists(const String& filename) {

		StringUtils::TokenIterator it = 0;
		String path;
		while ((it = StringUtils::NextWord(paths, path, it)) != String::npos) {
			String fullPath = URL::GetAppendedPath(path, filename);
#if NEX_WINDOWS==1
			DWORD attribs = 0;
			if (StringUtils::IsAscii(fullPath))
			attribs = GetFileAttributesA(fullPath.c_str());
			else
			attribs = GetFileAttributesW((const wchar_t*)StringUtils::ToUtf16(fullPath).c_str());
			return (attribs != INVALID_FILE_ATTRIBUTES && !(attribs & FILE_ATTRIBUTE_DIRECTORY));
#elif NEX_LINUX==1
			struct stat filestat;
			bool ret = (stat(fullPath.c_str(), &filestat) == 0);
			return ret;
#else
#endif
		}
		return false;
	}

	Directory::Factory::Factory() {
	}

	ArchivePtr Directory::Factory::CreateInstance(const String& name,
			const String& params) {
		return Assign<Archive>(NEX_NEW Directory(name, params));
	}
}
