#ifndef NEXTAR_PLUGIN_H__
#define NEXTAR_PLUGIN_H__

#include <NexBase.h>

namespace nextar {

enum PluginLicenseType {
	/* Opens after app begin, closes before app exits. */
	PLUG_TYPE_LIFETIME,
	/* Opens after render manager is ready, and closes before the context is deleted. */
	PLUG_TYPE_GRAPHICS,
	/* Custom load */
	PLUG_TYPE_CUSTOM,
};

struct PluginLicense {
	PluginLicenseType type;
	String customType;
};

/**
 * @remarks Classes derived from this one will be
 *          the main plugin interfaces
 **/
class FactoryPlugin {
public:

	virtual ~FactoryPlugin() {
	}

	/** @remarks Called after the plugin is loaded */
	virtual void Open() = 0;

	/** @remarks Called when license is renewed. Service registration
	 *           can be done in this call.
	 */
	virtual void LicenseRenewed() = 0;

	/** @remarks Called when license expires. Returning true
	 *           means every instance of objects in this plugin
	 *           are dead, and the plugin can be safely unloaded.
	 */
	virtual bool LicenseExpired() = 0;

	/** @remarks Called before the plugin is unloaded */
	virtual void Close() = 0;

	/** @remarks Dispose should free up the pointer, implements delete this as appropriate */
	virtual void Dispose() = 0;
};

#define NEX_DECLARE_PLUGIN(pluginClass) static pluginClass plugin

#define NEX_IMPLEMENT_PLUGIN(pluginName, pluginClass)       \
	pluginClass pluginClass::plugin;						\
	NEX_EXTERNC_SYM _NexExportSym nextar::FactoryPlugin*    \
	NEX_TOKEN_PASTE(pluginName,CreateFactory)() { 	\
		return &pluginClass::plugin;		\
	}

#define NEX_DECLARE_COMPONENT_FACTORY(Impl)					\
	typedef map<String, Impl*>::type FactoryMap;	\
	private:										\
		static FactoryMap sImplMap;					\
	public:	\
	inline static void RegisterFactory(const String& name, uint32 classId, Impl* impl)	{\
		String modifiedName = name + Convert::ToString(classId, ' ', std::ios::hex);	\
		sImplMap[modifiedName] = impl;	\
	}	\
	inline static void UnregisterFactory(const String& name, uint32 classId) {\
		String modifiedName = name + Convert::ToString(classId, ' ', std::ios::hex);	\
		sImplMap.erase(modifiedName);	\
	}	\
	inline static Impl* GetImpl(const String& name, uint32 classId) {\
		String modifiedName = name + Convert::ToString(classId, ' ', std::ios::hex);	\
		FactoryMap::iterator it = sImplMap.find(modifiedName); \
		if (it != sImplMap.end()) \
			return (*it).second; \
		return 0;	\
	}

#define NEX_IMPLEMENT_COMPONENT_FACTORY(Class)	\
		Class::FactoryMap Class::sImplMap;

#define NEX_DECLARE_FACTORY(Impl)					\
	typedef map<String, Impl*>::type FactoryMap;	\
	private:										\
		static FactoryMap sImplMap;					\
	public:	\
	inline static void RegisterFactory(const String& name, Impl* impl)	{\
		sImplMap[name] = impl;	\
	}	\
	inline static void UnregisterFactory(const String& name) {\
		sImplMap.erase(name);	\
	}	\
	inline static Impl* GetImpl(const String& name) {\
		FactoryMap::iterator it = sImplMap.find(name); \
		if (it != sImplMap.end()) \
			return (*it).second; \
		return 0;	\
	}

#define NEX_IMPLEMENT_FACTORY(Class)	\
		Class::FactoryMap Class::sImplMap;

}

#endif //NEXTAR_PLUGIN_H__