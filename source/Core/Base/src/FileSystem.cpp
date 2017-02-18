#include <BaseHeaders.h>
#include <FileSystem.h>
#include <CompressedArchive.h>
#include <Directory.h>
#include <OsDefs.h>
#include <Config.h>

namespace nextar {

NEX_DEFINE_SINGLETON_PTR(FileSystem);

const String FileSystem::ArchiveEngineData = "{EngineData}";
const String FileSystem::ArchiveEngineData_Name = "EngineData";
const String FileSystem::ArchiveProjectData = "{ProjectData}";
const String FileSystem::ArchiveProjectData_Name = "ProjectData";

FileSystem::FileSystem() {

	// register common mount point factories
	Directory::Factory::Register(this);
	CompressedArchive::Factory::Register(this);
	// register common paths
	String moduleLocation = Platform::GetModuleLocation();
	URL::AddMacroEntry("Bin", moduleLocation);
	AddArchive("", Archive::TYPE_DIR, "");
	AddArchive(ArchiveEngineData_Name, Archive::TYPE_DIR,
			moduleLocation + "/../Data");
	AddArchive(ArchiveProjectData_Name, Archive::TYPE_DIR,
		moduleLocation + "/../Data");
}

FileSystem::~FileSystem() {
	NameValueMap nvm;
	nvm.swap(URL::_macroTable);
}

void FileSystem::Configure(const Config& config) {
	// load all macros
	const NameValueMap& fileSys = config.GetSectionMap("FileSys");
	const String& macros = StringUtils::Find(fileSys, "Macros");

	size_t pos = 0;

	do {
		size_t p = macros.find_first_of(';', pos);
		StringPair macro = StringUtils::Split(macros.substr(pos, p != String::npos ?
			p - pos : String::npos));
		if (macro.first.length() > 0 && macro.second.length() > 0)
			URL::AddMacroEntry(macro.first, macro.second);
		pos = p == String::npos ? String::npos : p + 1;
	} while (pos != String::npos);

	// load all archives
	// format:
	// Name@type:path;
	const String& archives = StringUtils::Find(fileSys, "Archives");
	
	pos = 0;
	do {
		uint32 type = Archive::TYPE_DIR;
		size_t end = archives.find_first_of(';', pos);
		String archive = archives.substr(pos, end != String::npos ?
			end - pos : String::npos);
		String name;
		pos = 0;
		size_t p = archive.find_first_of('@');
		size_t nameIdx = p;
		if (p == String::npos) {
			p = archive.find_first_of(':');
			nameIdx = p;
		} else {
			name = archive.substr(p+1, 3);
			type = (name[0] << 24) | (name[1] << 16) | (name[2] << 8) | 0;
			p = archive.find_first_of(':');
		}

		if (p != String::npos) {
			name = archive.substr(0, nameIdx);
			AddArchive(name, type, archive.substr(p+1));
		}
		pos = (end == String::npos) ? String::npos : end + 1;
	} while (pos != String::npos);
}

void FileSystem::AddFactory(uint32 type, Archive::Factory* factory) {
	NEX_THREAD_LOCK_GUARD_RECURSIVE_MUTEX(accessLock);
	factories[type] = factory;
}

void FileSystem::RemoveFactory(uint32 type) {
	NEX_THREAD_LOCK_GUARD_RECURSIVE_MUTEX(accessLock);
	factories.erase(type);
}

InputStreamPtr FileSystem::OpenRead(const URL& url, bool lookin) {
	NEX_THREAD_LOCK_GUARD_RECURSIVE_MUTEX(accessLock);
	lookin = lookin && (url.GetArchiveName() != FileSystem::ArchiveEngineData_Name);
	ArchiveDescriptorMap::iterator it = descriptorMap.find(
			url.GetArchiveName());
	if (it != descriptorMap.end()) {
		if (!(*it).second.openArchive)
			(*it).second.Open(url.GetArchiveName(), this);
		NEX_ASSERT((*it).second.openArchive);
		InputStreamPtr istr = (*it).second.openArchive->OpenRead(url.GetRelativePath());
		if (istr || !lookin)
			return istr;
	} else {
		Warn("archive is invalid: " + url.GetArchiveName());
	}
	if (lookin) {
		URL belly(FileSystem::ArchiveEngineData_Name, url.GetRelativePath());
		return OpenRead(belly, false);
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

void FileSystem::AddArchive(const String& name, uint32 type,
		const String& path) {
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

bool FileSystem::DoesFileExists(const URL& fileName) {
	if (OpenRead(fileName))
		return true;
	return false;
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

