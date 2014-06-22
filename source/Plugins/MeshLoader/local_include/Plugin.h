/* 
 * File:   GenImageLoader.h
 * Author: obhi
 *
 * Created on May 8, 2011, 2:23 AM
 */

#ifndef MESLOADER_PLUGIN_H_
#define	MESLOADER_PLUGIN_H_

#include <EngineHeaders.h>

using namespace nextar;

namespace MeshLoader {

class Plugin: public FactoryPlugin {
public:
	static Plugin plugin;

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

private:
};

}

#endif	/* MESLOADER_PLUGIN_H_ */

