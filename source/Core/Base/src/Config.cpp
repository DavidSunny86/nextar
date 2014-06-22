#include <BaseHeaders.h>
#include <Config.h>
#include <ApplicationContext.h>
#include <FileSystem.h>
#include <StreamLexer.h>

namespace nextar {
Config::Config() {
}

Config::~Config() {

}

const String& Config::GetValue(const String& section, const String& entry,
		const String& defaultValue) const {
	NEX_THREAD_LOCK_GUARD_MUTEX(contentLock);
	SectionMap::const_iterator it = sections.find(section);
	if (it != sections.end()) {
		NameValueMap::const_iterator nvIt = (*it).second.find(entry);
		if (nvIt != (*it).second.end())
			return (*nvIt).second;
	}
	return defaultValue;
}

void Config::SetValue(const String& section, const String& entry,
		const String& value) {
	NEX_THREAD_LOCK_GUARD_MUTEX(contentLock);
	sections[section][entry] = value;
}

const NameValueMap& Config::GetSectionMap(const String& section) const {
	static const NameValueMap emptyObject;
	SectionMap::const_iterator it = sections.find(section);
	return it == sections.end() ? emptyObject : (*it).second;
}

void Config::SaveConfiguration(const URL& url) const {
	NEX_THREAD_LOCK_GUARD_MUTEX(contentLock);
	FileSystem& fileSys = FileSystem::Instance();
	OutputStreamPtr output = fileSys.OpenWrite(url);
	if (output) {
		String sectionName;
		for (SectionMap::const_iterator it = sections.begin();
				it != sections.end(); ++it) {
			const String& name = (*it).first;
			sectionName = "[" + name + "]\n";
			output->Write(sectionName.c_str(), sectionName.length());
			const NameValueMap& vnMap = (*it).second;
			for (NameValueMap::const_iterator nvIt = vnMap.begin();
					nvIt != vnMap.end(); ++nvIt) {
				String line = (*nvIt).first + " = " + (*nvIt).second + "\n";
				output->Write(line.c_str(), line.length());
			}
		}
	}
}

void Config::LoadConfiguration(const URL& path) {
	NEX_THREAD_LOCK_GUARD_MUTEX(contentLock);
	FileSystem& fileSys = FileSystem::Instance();
	InputStreamPtr inputStream = fileSys.OpenRead(path);
	if (inputStream) {

		const void* readOnlyBuffer = 0;
		size_t size = inputStream->AcquireBuffer(readOnlyBuffer);
		StreamLexer input(static_cast<const char*>(readOnlyBuffer), size);
		// read
		String section;
		NameValueMap* current = &sections[""];
		input.SkipWhite();
		while (!input.IsEndOfStream()) {
			switch (input.Forward()) {
			case '[': {
				input.SkipWhite();
				section = input.ReadWord("] \n\t\r");
				current = &sections[section];
				if (NEX_IS_DEBUG_MODE()) {
					input.SkipWhite();
					if (input.Current() != ']') {
						Warn(
								"expected ']', in line: "
										+ Convert::ToString(input.Line())
										+ " in config: "
										+ path.GetRelativePath());
					}
				}
				input.SkipLine();
			}
				break;
			default: {
				input.Backward();
				String name = input.ReadWord("= \n\t\r");
				input.SkipWhite();
				if (input.Forward() == '=') {
					const String& value = input.ReadStringLiteral();
					(*current)[name] = value;
				} else {
					Warn(
							"illegal expression in line: "
									+ Convert::ToString(input.Line())
									+ " in config: " + path.GetRelativePath());
					input.SkipLine();
				}
			}
				break;
			}
			input.SkipWhite();
		}

		inputStream->ReleaseBuffer(readOnlyBuffer);
	}
}
}

