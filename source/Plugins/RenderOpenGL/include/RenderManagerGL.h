/*
 * RenderManagerGL.h
 *
 *  Created on: 15-Aug-2013
 *      Author: obhi
 */

#ifndef RENDERMANAGERGL_H_
#define RENDERMANAGERGL_H_

#include "RenderOpenGL.h"
#include "RenderManager.h"

namespace RenderOpenGL {

	class RenderManagerGL: public nextar::RenderManager {
	public:
		RenderManagerGL();
		virtual ~RenderManagerGL();

		virtual RenderManager::ShaderProgramLanguage GetProgramLanguage();
		virtual nextar::String GetInfo();

	protected:

		virtual RenderDriverPtr CreateDriverImpl(RenderManager::DriverCreationParams&) = 0;
		virtual void ConfigureImpl(const NameValueMap&) = 0;
		virtual void CloseImpl() = 0;
	};

} /* namespace RenderOpenGL */
#endif /* RENDERMANAGERGL_H_ */
