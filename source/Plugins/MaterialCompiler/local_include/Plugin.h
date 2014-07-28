/*
 * Plugin.h
 *
 *  Created on: 30-Sep-2013
 *      Author: obhi
 */

#ifndef PLUGIN_H_
#define PLUGIN_H_

#include <RenderEngineHeaders.h>
#include <MaterialCompilerImplv10.h>

using namespace nextar;
namespace MaterialCompiler {

class Plugin: public FactoryPlugin {
	NEX_LOG_HELPER(Plugin)
	;
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
protected:
	static MaterialCompilerImplv1_0 materialCompilerImpl;
};

} /* namespace ShaderCompiler */
#endif /* PLUGIN_H_ */
