
#ifndef PBRENDERENGINE_PLUGIN_H
#define	PBRENDERENGINE_PLUGIN_H

#include <EngineHeaders.h>
using namespace nextar;

namespace PBRenderEngine {

class Plugin: public FactoryPlugin {
public:
	NEX_DECLARE_PLUGIN(Plugin);

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

#endif	/* PBRENDERENGINE_PLUGIN_H */

