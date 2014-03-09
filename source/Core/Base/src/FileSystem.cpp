#include "BaseHeaders.h"
#include "FileSystem.h"
#include "CompressedArchive.h"
#include "Directory.h"
#include "OsDefs.h"
#include "Config.h"

namespace nextar {

	NEX_DEFINE_SINGLETON_PTR(FileSystem);
	const String FileSystem::ArchiveEngineData = "EngineData";
	FileSystem::FileSystem() {

		// register common mount point factories
		Directory::Factory::Register(this);
		CompressedArchive::Factory::Register(this);
		// register common paths
		String moduleLocation = Platform::GetModuleLocation();
		URL::AddMacroEntry("Bin", moduleLocation);
		AddArchive("", Archive::TYPE_DIR, "");
		AddArchive(ArchiveEngineData, Archive::TYPE_DIR,
				moduleLocation + "/../../data");
	}

	FileSystem::~FileSystem() {
	}

	void FileSystem::Configure(const Config& config) {
		// load all macros
		const NameValueMap& macros = config.GetSectionMap("FileSys.Macros");
		for (NameValueMap::const_iterator it = macros.begin();
				it != macros.end(); ++it) {
			URL::AddMacroEntry((*it).first, (*it).second);
		}

		// load all archives
		// format:
		// archiveName = #type:path;
		const NameValueMap& archives = config.GetSectionMap("FileSys.Archives");
		for (NameValueMap::const_iterator it = archives.begin();
				it != archives.end(); ++it) {
			uint32 type = Archive::TYPE_DIR;
			const String& path = (*it).second;
			if (path.length()) {
				size_t pos = 0;
				if (path[0] == '#') {
					pos = path.find_first_of(':');
					if (pos != String::npos && pos == 4) {
						type = (path[0] << 24) | (path[1] << 16)
								| (path[2] << 8) | 0;
						pos = 5;
					} else {
						pos = 0;
					}
				}
				AddArchive((*it).first, type, pos ? path.substr(pos) : path);
			}
		}
	}

	void FileSystem::AddFactory(uint32 type, Archive::Factory* factory) {
		NEX_THREAD_LOCK_GUARD_RECURSIVE_MUTEX(accessLock);
		factories[type] = factory;
	}

	void FileSystem::RemoveFactory(uint32 type) {
		NEX_THREAD_LOCK_GUARD_RECURSIVE_MUTEX(accessLock);
		factories.erase(type);
	}

	InputStreamPtr FileSystem::OpenRead(const URL& url) {
		NEX_THREAD_LOCK_GUARD_RECURSIVE_MUTEX(accessLock);
		ArchiveDescriptorMap::iterator it = descriptorMap.find(
				url.GetArchiveName());
		if (it != descriptorMap.end()) {
			if (!(*it).second.openArchive)
				(*it).second.Open(url.GetArchiveName(), this);
			NEX_ASSERT((*it).second.openArchive);
			return (*it).second.openArchive->OpenRead(url.GetRelativePath());
		} else {
			Warn("archive is invalid: " + url.GetArchiveName());
		}
		return InputStreamPtr();
	}

	OutputStreamPtr FileSystem::OpenWrite(const URL& url, bool append) {
		NEX_THREAD_LOCK_GUARD_RECURSIVE_MUTEX(accessLock);
		ArchiveDescriptorMap::iterator it = descriptorMap.find(
				url.GetArchiveName());
		if (it != descriptorMap.end()) {
			if (!(*it).second.openArchive)
				(*it).second.Open(url.GetArchiveName(), this);
			NEX_ASSERT((*it).second.openArchive);
			return (*it).second.openArchive->OpenWrite(url.GetRelativePath(),
					append);
		} else {
			Warn("archive is invalid: " + url.GetArchiveName());
		}
		return OutputStreamPtr();
	}

	ArchivePtr FileSystem::OpenArchive(const String& name, uint32 type,
			const String& path) {
		NEX_THREAD_LOCK_GUARD_RECURSIVE_MUTEX(accessLock);
		ArchiveDescriptorMap::iterator it = descriptorMap.find(name);
		if (it == descriptorMap.end()) {
			_AddArchive(name, type, path);
			it = descriptorMap.find(name);
		}

		if (it != descriptorMap.end()) {
			if (!(*it).second.openArchive)
				(*it).second.Open(name, this);
			return (*it).second.openArchive;
		} else if (path.length() <= 0 || type == Archive::TYPE_INVALID) {
			Warn("can't open null and unregistered archive!: " + name);
		}
		return ArchivePtr();
	}

	void FileSystem::CloseArchive(const String& name) {
		NEX_THREAD_LOCK_GUARD_RECURSIVE_MUTEX(accessLock);
		ArchiveDescriptorMap::iterator it = descriptorMap.find(name);
		if (it != descriptorMap.end()) {
			if (!(*it).second.openArchive)
				(*it).second.Close();
		}
	}

	void FileSystem::AddArchive(const String& name, uint32 type, const String& path) {
		NEX_THREAD_LOCK_GUARD_RECURSIVE_MUTEX(accessLock);
		_AddArchive(name, type, path);
	}

	void FileSystem::_AddArchive(const String& name, uint32 type,
			const String& path) {
		if (type == Archive::TYPE_INVALID)
			return;
		ArchiveFactoryMap::iterator it2 = factories.find(type);
		if (it2 == factories.end()) {
			Warn("invalid type for archive: " + name);
			return;
		}

		ArchiveDesc& d = descriptorMap[name];
		d.type = type;
		d.archiveParam = path;
	}

	void FileSystem::RemoveArchive(const String& name) {
		descriptorMap.erase(name);
	}

	void FileSystem::ArchiveDesc::Open(const String& name, FileSystem* fileSys) {
		NEX_ASSERT(!openArchive);
		ArchiveFactoryMap::iterator it = fileSys->factories.find(type);
		if (it != fileSys->factories.end()) {
			openArchive = (*it).second->CreateInstance(name, archiveParam);
			if (!openArchive) {
				Warn("failed to open archive: " + name);
				NEX_THROW_GracefulError(EXCEPT_CAN_NOT_OPEN_FILE);
			}
		}
	}

	void FileSystem::ArchiveDesc::Close() {
		openArchive.Clear();
	}

	void FileSystem::CopyStream(InputStreamPtr& input, OutputStreamPtr& output) {
		const size_t MAX_BUFF = 2048;
		char buffer[MAX_BUFF];
		size_t writing = input->GetSize() - (size_t) input->Tell();
		while (writing) {
			size_t read = input->Read(buffer, std::min(writing, MAX_BUFF));
			output->Write(buffer, read);
			writing -= read;
		}
	}

}

