#ifndef NEXTAR_COMPRESSEDFILE_H__
#define NEXTAR_COMPRESSEDFILE_H__

#include "NexBase.h"
#include "StringUtils.h"
#include "DataStream.h"
#include "GenericStreams.h"
#include "Archive.h"
#include "archive/ZipArchive.h"

namespace nextar {

	class CompressedArchive: public Archive {
		NEX_LOG_HELPER(CompressedArchive)
		;
	public:

		enum {
			TYPE = Archive::TYPE_ARC,
			// is already opened?
			FLAG_ARCHIVE_OPENED = 1 << 0,
			// ready to be opened?
			FLAG_ARCHIVE_READY = 1 << 1,
		};

		class Factory: public Archive::Factory, public AllocGeneral {
		public:
			static Factory compressedArchiveFactoryObject;

			Factory();
			virtual ArchivePtr CreateInstance(const String& name,
					const String& params);
			static void Register(FileSystem*);
		};

		CompressedArchive(const String& name, const String& path);

		virtual InputStreamPtr OpenRead(const String& fileName);
		virtual OutputStreamPtr OpenWrite(const String& fileName, bool append);

		virtual void Scan(ScanCallback* callback, const String& pattern,
				bool recursive);

		virtual void GetFileList(StringVector&, bool listOnlyFiles);
		virtual bool GetAttribute(const String& filename,
				FileAttribute& attributes);

		virtual bool Exists(const String& filename);

	protected:

		inline void _Prepare(bool createIfNotPresent = false) {
			if (!archive) {
				if (flags & FLAG_ARCHIVE_READY)
					_OpenArchive(createIfNotPresent);
			}
		}

		void _OpenArchive(bool createIfNotPresent);
		zip::ZipFile* _Open(const char* filename);

		NEX_THREAD_MUTEX(accessLock);
		uint32 flags;
		String archiveLocation;
		zip::ZipArchivePtr archive;
	};

	typedef RefPtr<CompressedArchive> CompressedArchivePtr;

	class CompressedInputStream: public MemoryInputStream {
	public:
		CompressedInputStream(zip::ZipFile* file, zip::ZipArchivePtr arch);
		virtual ~CompressedInputStream() {
		}
	};

	class CompressedOutputStream: public OutputStream {
		zip::ZipArchivePtr archive;
		zip::ZipFile* zfile;
	public:
		CompressedOutputStream(zip::ZipFile* file, zip::ZipArchivePtr arch);
		virtual ~CompressedOutputStream();

		virtual void Write(const void* buffer, size_t size);
		virtual bool Seek(std::streamoff offset, std::ios_base::seekdir dir);
		virtual std::streamoff Tell();
	};
}

#endif //NEXTAR_COMPRESSEDFILE_H__
