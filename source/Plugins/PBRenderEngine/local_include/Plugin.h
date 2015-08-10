/* 
 * File:   GenImageLoader.h
 * Author: obhi
 *
 * Created on May 8, 2011, 2:23 AM
 */

#ifndef GENIMAGELOADER_PLUGIN_H
#define	GENIMAGELOADER_PLUGIN_H

#include <EngineHeaders.h>
#include <Image.h>
#include <JPEGImageCodec.h>
#include <PNGImageCodec.h>
#include <DDSImageCodec.h>

using namespace nextar;

namespace ImageLoader {

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

#endif	/* GENIMAGELOADER_PLUGIN_H */

