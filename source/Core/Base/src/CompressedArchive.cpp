#include "BaseHeaders.h"
#include "Archive.h"
#include "CompressedArchive.h"
#include "ApplicationContext.h"
#include "FileSystem.h"

namespace nextar {

//Compressed Archive System ---------------------------------
	CompressedArchive::Factory CompressedArchive::Factory::compressedArchiveFactoryObject;

	void CompressedArchive::Factory::Register(FileSystem* fileSys) {
		fileSys->AddFactory(CompressedArchive::TYPE,
				&compressedArchiveFactoryObject);
	}

	CompressedArchive::CompressedArchive(const String& name, const String& path) :
			Archive(name), flags(FLAG_ARCHIVE_READY) {
		archiveLocation = path;
	}

	InputStreamPtr CompressedArchive::OpenRead(const String& fileName) {
		NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
		_Prepare();
		const char* startPath = fileName.c_str();
		if (*startPath == '/')
			startPath++;
		zip::ZipFile* file = zip::FindArchiveFile(archive, startPath);

		if (!file) {
			Warn("failed to open file(r): " + fileName);
			return InputStreamPtr();
		}

		Trace("opened(r,c): " + fileName);
		return Assign<InputStream>(NEX_NEW CompressedInputStream(file, archive));
	}

	OutputStreamPtr CompressedArchive::OpenWrite(const String& fileName,
			bool append) {
		NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
		_Prepare(true);
		const char* startPath = fileName.c_str();
		if (*startPath == '/')
			startPath++;
		zip::ZipFile* file = zip::FindArchiveFile(archive, startPath);

		if (!file) {
			file = zip::AddNewFileToArchive(archive.GetPtr(), startPath, 0,
					true);
			if (!file) {
				Warn("failed to open file(w): " + fileName);
				return OutputStreamPtr();
			}
		}

		Trace("opened(w,c): " + fileName);
		return Assign<OutputStream>(
				NEX_NEW CompressedOutputStream(file, archive));
	}

	void CompressedArchive::Scan(ScanCallback* callback, const String& pattern,
			bool recursive) {
		NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
		_Prepare();

		FileAttribute attr;
		zip::FileList filelist;
		zip::GetFileList(archive, filelist);
		for (size_t j = 0; j < filelist.size(); ++j) {
			const char* filename = zip::GetFileName(filelist[j]);
			if (pattern.length()
					&& StringUtils::PatternMatch(filename, pattern.c_str())) {
				//! full path not valid
				attr.fileName = URL(CompressedArchive::TYPE, GetName(),
						String(filename));
				attr.flags = FileAttribute::ATTRIB_COMPRESSED;
				attr.uncompressedSize = zip::GetFileSizeUncomp(filelist[j]);
				attr.compressedSize = zip::GetFileSizeComp(filelist[j]);
				zip::GetArchiveFileTime(filelist[j], &attr.fileTime);
				callback->FoundFile(attr, this);
			}
		}
	}

	void CompressedArchive::GetFileList(StringVector& urlList,
			bool listOnlyFiles) {
		NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
		_Prepare();
		zip::GetFileNameList(archive, urlList);
	}

	bool CompressedArchive::GetAttribute(const String& fileName,
			FileAttribute& attr) {
		NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
		_Prepare();

		zip::ZipFile* file = zip::FindArchiveFile(archive, fileName.c_str());

		if (!file)
			return false;
		attr.fileName = URL(CompressedArchive::TYPE, GetName(), fileName);
		attr.flags = FileAttribute::ATTRIB_COMPRESSED;
		attr.uncompressedSize = zip::GetFileSizeUncomp(file);
		attr.compressedSize = zip::GetFileSizeComp(file);
		zip::GetArchiveFileTime(file, &attr.fileTime);
		return true;
	}

	bool CompressedArchive::Exists(const String& fileName) {
		NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
		_Prepare();

		return zip::FindArchiveFile(archive, fileName.c_str()) ? true : false;
	}

	void CompressedArchive::_OpenArchive(bool createIfNotPresent) {
		zip::ZipArchive* arch = zip::OpenArchive(archiveLocation);
		if (!arch) {
			if (createIfNotPresent)
				arch = zip::CreateArchive(archiveLocation);
			if (!arch) {
				Error("failed to open archive: " + archiveLocation);
				NEX_THROW_GracefulError(EXCEPT_CAN_NOT_OPEN_FILE);
			}
		}
		archive = Assign<zip::ZipArchive>(arch);
		flags |= FLAG_ARCHIVE_OPENED;
	}

//Compressed Input Stream------------------------------------

	CompressedInputStream::CompressedInputStream(nextar::zip::ZipFile *file,
			nextar::zip::ZipArchivePtr arch) :
			MemoryInputStream() {
		size_t fileSize;
		char* data = zip::ReadFromArchiveFile(arch.GetPtr(), file, &fileSize);
		if (data)
			LoadMemory(data, fileSize, true);
		else
			NEX_THROW_GracefulError(EXCEPT_READ_ERROR);
	}

//Compressed Output Stream-----------------------------------

	CompressedOutputStream::CompressedOutputStream(zip::ZipFile* file,
			zip::ZipArchivePtr arch) {
		zfile = file;
		archive = arch;
	}

	CompressedOutputStream::~CompressedOutputStream() {
		if (zfile && archive) {
			zip::FlushArchive(archive);
		}
	}

	void CompressedOutputStream::Write(const void *buffer, size_t numbytes) {
		if (!zip::WriteToArchiveFile(archive, zfile, (const char*) buffer,
				numbytes))
			NEX_THROW_GracefulError(EXCEPT_WRITE_ERROR);
		size += numbytes;
	}

	bool CompressedOutputStream::Seek(std::streamoff offset,
			std::ios_base::seekdir dir) {
		zip::SeekArchiveFile(zfile, dir, offset);
		return true;
	}

	std::streamoff CompressedOutputStream::Tell() {
		return zip::TellArchiveFile(zfile);
	}

	CompressedArchive::Factory::Factory() {
	}

	ArchivePtr CompressedArchive::Factory::CreateInstance(const String& baseDir,
			const String& params) {
		return Assign<Archive>(NEX_NEW CompressedArchive(baseDir, params));
	}

}

