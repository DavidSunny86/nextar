/*
 * PluginGL.h
 *
 *  Created on: 03-Oct-2013
 *      Author: obhi
 */

#ifndef PLUGINGL_H_
#define PLUGINGL_H_

#include <RenderManagerGL.h>

namespace RenderOpenGL {

class PluginGL: public nextar::FactoryPlugin, public nextar::AllocGeneral {
public:
	NEX_DECLARE_PLUGIN(PluginGL);

	PluginGL() {}
	virtual ~PluginGL() {}

	virtual void Open();
	virtual void LicenseRenewed();
	virtual bool LicenseExpired();
	virtual void Close();
	virtual void Dispose();

	PluginService* Query(const String& name, const String& impl);
protected:
};

} /* namespace RenderOpenGL */
#endif /* PLUGINGL_H_ */
