#ifndef NEXTAR_MOUNTPOINT_DIR_H__
#define NEXTAR_MOUNTPOINT_DIR_H__

#include <Archive.h>

namespace nextar {

	class Directory: public Archive {
		NEX_LOG_HELPER(Directory)
		;
	public:
		enum {
			TYPE = Archive::TYPE_DIR,
		};

		class Factory: public Archive::Factory, public AllocGeneral {
		public:
			static Factory directoryFactoryObject;

			Factory();

			/** @copydoc ArchiveFactory::CreateInstance */
			virtual ArchivePtr CreateInstance(const String& name,
					const String& path);
			static void Register(FileSys*);
		};

		Directory(const String& name, const String& path);
		virtual ~Directory();

		virtual InputStreamPtr OpenRead(const String& fileName);
		virtual OutputStreamPtr OpenWrite(const String& fileName, bool append);

		virtual void Scan(ScanCallback* callback, const String& pattern,
				bool recursive);

		virtual void GetFileList(StringVector&, bool listOnlyFiles);
		virtual bool GetAttribute(const String& filename,
				FileAttribute& attributes);

		virtual bool Exists(const String& filename);

	private:

		void _VisitDirectory(const String& file, const String& pattern,
				const String& subPath, ScanCallback* callback, bool recursive);
		InputStreamPtr OpenRead(const String& path, const String& filename);
		OutputStreamPtr OpenWrite(const String& path, const String& filename,
				bool append);
	public:
		String paths;
	};

}

#endif //NEXTAR_MOUNTPOINT_DIR_H__
