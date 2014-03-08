#ifndef NEXTAR_PLUGIN_H__
#define NEXTAR_PLUGIN_H__

#include "NexSys.h"

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

#define NEX_IMPLEMENT_PLUGIN(pluginName)              \
    NEX_EXTERNC_SYM _NexExportSym nextar::FactoryPlugin*      \
    NEX_TOKEN_PASTE(pluginName,CreateFactory)()

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
