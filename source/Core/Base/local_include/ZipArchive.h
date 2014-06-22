#ifndef NEXTAR_ARCHIVESUPPORT_H
#define NEXTAR_ARCHIVESUPPORT_H

#include <ZipHeaders.h>

#ifndef _NexZipApi
#define _NexZipApi
#endif

#define NEX_ZIP_DIRTY		0x2

namespace nextar {

namespace zip {
struct ZipFile;
class ZipArchive;
// ---------------------------

struct ZipFile {
	char* filename;
	zCentralDirFileHdr info;

	char* buffer;
	size_t bufferPos;
	size_t bufferSize;

	char* extrafield;
	char* comment;

	struct ZipFile* next;
	struct ZipFile* prev;

};

typedef vector<ZipFile*>::type FileList;

struct StrNode {
	char* name;
	struct StrNode* next;
};

// ---------------------------

#define AddArchiveRef(arch) ++arch->numRefs
#define ReleaseArchiveRef(arch) {				\
	if(arch->numRefs > 1) --arch->numRefs;	\
	else zip::CloseArchive(arch);		}
#define GetNumArchiveRefs(arch)	(arch->numRefs)
//! open an existing archieve
_NexZipApi ZipArchive* OpenArchive(const String& fullname);
//! Create a new archieve
_NexZipApi ZipArchive* CreateArchive(const String& fullname);
//! close, this will clear all data allocated during opening
//! or creating, flush everything and free the allocated mem
_NexZipApi void CloseArchive(ZipArchive*);
//! flush pending writes, returns true on success
_NexZipApi bool FlushArchive(ZipArchive*);
//! Find a file in the archive
_NexZipApi ZipFile* FindArchiveFile(ZipArchive*, const char*);
//! Add a new file to archive, the buffer will be resized if more
//! is written.
_NexZipApi ZipFile* AddNewFileToArchive(ZipArchive*, const char* filename,
		size_t buffsize, bool pack);
//! append to file, before writing to a file just read, call this.
_NexZipApi bool AppendToReadArchiveFile(ZipArchive*, ZipFile*);
//! delete existing file from archive
_NexZipApi bool DeleteFileFromArchive(ZipArchive*, const char* filename);
//! file exists
_NexZipApi bool FileExistsInArchive(ZipArchive*, const char* filename);
//! read , do not call NextarFree on the returned buffer
_NexZipApi char* ReadFromArchiveFile(ZipArchive*, ZipFile*, size_t*);
//! write
_NexZipApi bool WriteToArchiveFile(ZipArchive*, ZipFile*, const char*, size_t);
//! get/set file time
_NexZipApi void GetArchiveFileTime(ZipFile*, FileTime*);
_NexZipApi void SetArchiveFileTime(ZipFile*, const FileTime*);
//! seek
_NexZipApi void SeekArchiveFile(ZipFile*, std::ios_base::seekdir,
		std::streamoff offset);
//! get file list
_NexZipApi void GetFileList(ZipArchive*, FileList& arr);
//! get file name list
_NexZipApi void GetFileNameList(ZipArchive*, StringVector& arr);
//! tell

class ZipArchive: public AllocGeneral {
public:
	int16 numRefs;
	uint16 flags;

	InputStreamPtr infile;
	OutputStreamPtr outfile;
	URL fileName;

	char* comment;
	size_t commentLength;

	StrNode* delHead;

	ZipFile* entriesAdded;

	ZipFile* entriesHead;
	ZipFile* entriesTail;
	int32 numFiles;

	ZipArchive() :
			numRefs(1), flags(0), comment(0), commentLength(0), delHead(0), entriesAdded(
					0), entriesHead(0), entriesTail(0), numFiles(0) {
	}

	// handy functions
	_NexInline void AddRef();_NexInline void Release();_NexInline int32 GetRefCount();
};

void ZipArchive::AddRef() {
	AddArchiveRef(this);
}

void ZipArchive::Release() {
	ReleaseArchiveRef(this);
}

int32 ZipArchive::GetRefCount() {
	return GetNumArchiveRefs(this);
}

typedef RefPtr<ZipArchive> ZipArchivePtr;
//! get file name by file ptr

_NexInline const char* GetFileName(ZipFile* file) {
	return file->filename;
}

//! returns uncompressed size of file
_NexInline size_t GetFileSizeUncomp(ZipFile* file) {
	return file->info.ucsize;
}
//! returns compressed size of file

_NexInline size_t GetFileSizeComp(ZipFile* file) {
	return file->info.csize;
}

_NexInline std::streamoff TellArchiveFile(ZipFile* file) {
	return file->bufferPos;
}
}
}

#endif //NEXTAR_ARCHIVESUPPORT_H