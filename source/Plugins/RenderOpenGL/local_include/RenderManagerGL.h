/*
 * RenderManagerGL.h
 *
 *  Created on: 15-Aug-2013
 *      Author: obhi
 */

#ifndef RENDERMANAGERGL_H_
#define RENDERMANAGERGL_H_

#include <RenderOpenGL.h>
#include <BaseRenderManager.h>

namespace RenderOpenGL {

class RenderManagerGL: public nextar::BaseRenderManager {
public:
	RenderManagerGL();
	virtual ~RenderManagerGL();

	virtual RenderManager::ShaderProgramLanguage GetProgramLanguage();
	virtual nextar::String GetInfo();

protected:

	virtual RenderDriverPtr CreateDriverImpl(
			RenderManager::DriverCreationParams&);
	virtual void ConfigureImpl(const NameValueMap&);
	virtual void CloseImpl();
};

} /* namespace RenderOpenGL */
#endif /* RENDERMANAGERGL_H_ */
