#ifndef NEXTAR_FILESYS_H
#define NEXTAR_FILESYS_H

#include "NexSys.h"
#include "Archive.h"

namespace nextar {

//! Basic file system data types.

	/**
	 * @class	FileSystem
	 *
	 * @brief	File system.
	 *
	 * \@detail  @par
	 */

	class _NexBaseExport FileSystem: public Singleton<FileSystem>,
		public AllocGeneral {
		NEX_LOG_HELPER(FileSystem);

	public:
		//! Engine data mount point
		static const String ArchiveEngineData;

		FileSystem();
		~FileSystem();

		/** @remarks Load plugin **/
		void Configure(const Config&);

		// add factory
		void AddFactory(uint32 type, Archive::Factory*);
		void RemoveFactory(uint32 type);
		// open file for reading
		InputStreamPtr OpenRead(const URL&);
		// open file for writing
		OutputStreamPtr OpenWrite(const URL&, bool append = false);

		// open archive 
		ArchivePtr OpenArchive(const String& name, uint32 type =
				Archive::TYPE_INVALID,
				const String& params = StringUtils::Null);
		// close archive
		void CloseArchive(const String& name);
		// This function supports resolving archive names to full path within the archive path
		// that is passed as parameter to the added archive.
		void AddArchive(const String& name, uint32 type, const String& param);
		void RemoveArchive(const String& name);

		static void CopyStream(InputStreamPtr& input, OutputStreamPtr& output);

	protected:

		void _AddArchive(const String& name, uint32 type, const String& param);

		class ArchiveDesc: public AllocGeneral {

			NEX_LOG_HELPER(ArchiveDesc)
			;
		public:
			uint32 type;
			String archiveParam;
			ArchivePtr openArchive;

			~ArchiveDesc() {
				Close();
			}

			void Open(const String& name, FileSystem* fileSys);
			void Close();
		};

		friend class ArchiveDesc;

		NEX_THREAD_RECURSIVE_MUTEX(accessLock);

		typedef map<uint32, Archive::Factory*>::type ArchiveFactoryMap;
		typedef map<String, ArchiveDesc>::type ArchiveDescriptorMap;

		/** factories of mountpoints */
		ArchiveFactoryMap factories;
		//! The mount point map.
		ArchiveDescriptorMap descriptorMap;
	};

}

#endif //NEXTAR_FILESYS_H
