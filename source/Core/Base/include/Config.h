#ifndef NEXTAR_NEXCONF_H
#define NEXTAR_NEXCONF_H

#include <NexBase.h>
#include <RefPtr.h>
#include <DataStream.h>
#include <Convert.h>
#include <StringUtils.h>
#include <URL.h>

namespace nextar {

/**
 * @class Config
 * @brief Simple ini based configuration, no unicode support. XML is for unicode configurations.
 **/
class _NexBaseAPI Config: public NonCopyableObject, public Referenced<Config,
		AllocGeneral> {
	NEX_LOG_HELPER(Config)
	;

public:

	Config();
	~Config();

	const String& GetValue(const String& section, const String& entry,
			const String& defaultValue = StringUtils::Null) const;

	void SetValue(const String& section, const String& entry,
			const String& value);

	const NameValueMap& GetSectionMap(const String& section) const;

	/* Save */
	void SaveConfiguration(const URL& path) const;
	/* Load */
	void LoadConfiguration(const URL& path);
private:

	mutable NEX_THREAD_MUTEX(contentLock);
	typedef map<String, NameValueMap>::type SectionMap;
	SectionMap sections;
};

typedef RefPtr<Config> ConfigPtr;
}

#endif //NEXTAR_NEXCONF_H