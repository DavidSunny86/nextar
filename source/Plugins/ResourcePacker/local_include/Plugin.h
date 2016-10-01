/*
 * Plugin.h
 *
 *  Created on: Sep 18, 2016
 *      Author: obhi
 */

#ifndef PLUGINS_RESOURCEPACKER_LOCAL_INCLUDE_PLUGIN_H_
#define PLUGINS_RESOURCEPACKER_LOCAL_INCLUDE_PLUGIN_H_

#include <ResourcePacker.h>
#include <FactoryPlugin.h>
#include <ResourcePackerServiceImpl.h>

namespace ResourcePacker {

class Plugin: public nextar::FactoryPlugin {
public:
	NEX_DECLARE_PLUGIN(Plugin);
	NEX_LOG_HELPER(Plugin);

	Plugin();
	virtual ~Plugin();

	/** @remarks Called after the plugin is loaded */
	virtual void Open();
	/** @remarks Called when license is renewed. Service registration
	 *           can be done in this call.
	 */
	virtual void LicenseRenewed();
	/** @remarks Called when license expires. Returning true
	 *           means every instance of objects in this plugin
	 *           are dead, and the plugin can be safely unloaded.
	 */
	virtual bool LicenseExpired();
	/** @remarks Called before the plugin is unloaded */
	virtual void Close();
	/** @remarks Dispose should free up the pointer, implements delete this as appropriate */
	virtual void Dispose();
	/** @remarks Query service supported */
	virtual PluginService* Query(const String& name, const String& impl);

protected:
	ResourcePackerServiceImpl _packerServiceImpl;
};

} /* namespace ResourcePacker */

#endif /* PLUGINS_RESOURCEPACKER_LOCAL_INCLUDE_PLUGIN_H_ */
