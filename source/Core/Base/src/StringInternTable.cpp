#include <BaseHeaders.h>
#include <StringUtils.h>
#include <StringInternTable.h>
#include <Serializer.h>

namespace nextar {

StringInternTable::StringInternTable(const String& _name) :
		loadedFromCache(false), saveToCache(false), name(_name) {
	NEX_THREAD_MUTEX(accessLock);
}

StringInternTable::~StringInternTable() {
	NEX_THREAD_MUTEX(accessLock);
	if (saveToCache)
		_SaveToCache();
}

URL StringInternTable::_GetURL() {
	return URL(
			FileSystem::ArchiveEngineData + "/StringTable_" + name
					+ ".table");
}

StringID StringInternTable::AsyncStringID(const String& value) {
	NEX_THREAD_MUTEX(accessLock);
	if (!loadedFromCache)
		_LoadFromCache();
	ReverseMap::iterator it = reverseMap.find(std::cref(value));
	if (it != reverseMap.end())
		return (*it).second;
	auto insertIt =
			forwardMap.emplace(
					(StringID) (forwardMap.size()
							+ StringUtils::STRING_ID_START_OFFSET), value).first;
	saveToCache = true;
	reverseMap.emplace(std::cref((*insertIt).second), (*insertIt).first);
	return (*insertIt).first;
}

StringRef StringInternTable::AsyncString(StringID v) {
	NEX_THREAD_MUTEX(accessLock);
	if (!loadedFromCache)
		_LoadFromCache();
	ForwardMap::iterator it = forwardMap.find(v);
	if (it != forwardMap.end())
		return std::cref((*it).second);
	return StringUtils::NullRef;
}

void StringInternTable::_LoadFromCache() {
	NEX_ASSERT(!saveToCache);
	Warn("Performance warning - strings loaded from cache!");

	forwardMap.clear();
	reverseMap.clear();

	auto it =
			forwardMap.insert(
					ForwardMap::value_type(StringUtils::NullID,
							StringUtils::Null)).first;
	reverseMap.emplace(std::cref((*it).second), (*it).first);
	it = forwardMap.insert(
			ForwardMap::value_type(StringUtils::DefaultID,
					StringUtils::Default)).first;
	reverseMap.emplace(std::cref((*it).second), (*it).first);

	InputStreamPtr input = FileSystem::Instance().OpenRead(_GetURL());
	if (input) {
		InputSerializer ser(input);
		while (!ser.IsEndOfStream()) {
			StringID key;
			String value;
			ser >> key >> value;
			auto it = forwardMap.emplace(key, value).first;
			reverseMap.emplace(std::cref((*it).second), (*it).first);
		}
	}

	loadedFromCache = true;
}

void StringInternTable::_SaveToCache() {
	if (!FileSystem::InstancePtr())
		return;
	OutputStreamPtr output = FileSystem::Instance().OpenWrite(_GetURL());
	if (output) {
		OutputSerializer ser(output);
		for (auto& it : forwardMap) {
			ser << it.first << it.second;
		}
	}
	saveToCache = false;
}

void StringInternTable::SaveToCache() {
	NEX_THREAD_MUTEX(accessLock);
	if (saveToCache)
		_SaveToCache();
}

void StringInternTable::LoadFromCache() {
	NEX_THREAD_MUTEX(accessLock);
	_LoadFromCache();
}
}
