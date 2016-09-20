/*
 * File:   pluginManager.h
 * Author: obhi
 *
 * Created on January 19, 2011, 7:08 PM
 */

#ifndef PLUGIN_MANAGER_H
#define	PLUGIN_MANAGER_H

#include <FactoryPlugin.h>
#include <OsDefs.h>

namespace nextar {

/**
 * @remarks Handles regular plugin loads. Plugins should have the following
 *          function(s) defined:
 *
 * @par     FactoryPlugin* pluginNameCreateFactory();
 *          Substitute PluginName by the actual plugin name.
 *
 * @par     System extention for plugins are automatically added, i.e. a plugin
 *          named PluginName should have an output file like PluginName.dll
 *          or PluginName.so depending upon platform.
 *
 *          Plugins should have the following metadata in the plugin configuration files, the default values
 *          are provided:
 * @par     <plugins>                                                                             \n
 *            <basePath>${Bin}/plugins</basePath>                                                 \n
 *            <plugin>                                                                            \n
 *            	<name>PluginName</name>                                                           \n
 *            	<context>EApplicationContextType</context>                                        \n
 *              <build>NextarBuildVersion</build>                                                 \n
 *              <type>Custom</type>				                                                  \n
 *              <optional>true</optional>                                                         \n
 *            </plugin>
 *         </plugins>
 */
class PluginRegistry: public Singleton<PluginRegistry>,
public AllocGeneral {
	NEX_LOG_HELPER (PluginRegistry);
public:

	class DynLib: public AllocGeneral {
		bool optional;DYN_LIB_TYPE libptr;
		String fullName;
		String name;
		String serviceNames;
		uint32 buildVersion;
		PluginLicense license;
		FactoryPlugin* plugin;
		ApplicationContextType context;

		typedef FactoryPlugin* (*PluginCreateFactory)();

		NEX_LOG_HELPER(DynLib)
		;
	public:

		DynLib(const String& path, const String& name, const String& serviceName,
			const PluginLicense&, uint32 build, ApplicationContextType context, bool optional);
		~DynLib();

		_NexInline bool IsAccepted(ApplicationContextType appType) const {
			return appType.IsAccepted(context._type);
		}

		/** @remarks Get plugin name */
		_NexInline const String& GetName() const {
			return name;
		}

		/**
		 * @remarks Return factory plugin
		 **/
		_NexInline FactoryPlugin* GetFactoryPlugin() {
			return plugin;
		}

		void Request(PluginLicenseType pluginType, const String& name = StringUtils::Null, bool load = false);
		void Request(bool load = false);
		PluginService* Query(const String& name);

	protected:

		bool _LoadLib();
		void _UnloadLib();

	};

public:

	/** @remarks This call will configure the registry and look for
	 *           plugin details.
	 **/
	PluginRegistry();
	~PluginRegistry();

	/** @remarks Load plugin **/
	void Configure(const Config&);

	/* Add plugins from configuration */
	void AddPlugins(const URL& pluginConfig);

	/** @remarks Load plugin manually **/
	void AddPlugin(const String& path, const String& name, const String& services, 
		const PluginLicense& li, uint32 version,
		ApplicationContextType context,
			bool optional = false);

	/** Post a license event */
	void RequestPlugins(PluginLicenseType le, const String& typeName, bool loadPlugins);

	/** Query service */
	PluginService* QueryService(const char* name);

	/** Unload */
	void UnloadPlugins();

	template <typename ServiceClass>
	inline ServiceClass* QueryService() {
		return static_cast<ServiceClass*>(QueryService(ServiceClass::GetMeta().GetName()));
	}
private:

	static PluginLicenseType _ParsePluginLicenseType(const String& type);
	void _ParseAndLoadPlugins(const URL&);
	void _ParsePluginConfiguration(const URL&);

	typedef vector<DynLib*>::type DynLibList;
	String pluginSearchPath;
	DynLibList libraries;
};

}

#endif	/* PLUGIN_MANAGER_H */
