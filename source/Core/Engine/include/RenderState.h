/*
 * RenderState.h
 *
 *  Created on: 05-Mar-2017
 *      Author: obhi
 */

#ifndef CORE_ENGINE_INCLUDE_RENDERSTATE_H_
#define CORE_ENGINE_INCLUDE_RENDERSTATE_H_

#include <ContextObject.h>
#include <RasterState.h>
#include <BlendState.h>
#include <DepthStencilState.h>

namespace nextar {

class _NexEngineAPI RenderState: public ContextObject,
	public Referenced<RenderState, AllocGeneral> {

public:

	struct Params {
		RasterState raster;
		BlendState blend;
		DepthStencilState dethStencil;
	};

	enum Message {
		MSG_UPDATE,
	};

	class _NexEngineAPI View: public ContextObject::View {
	public:
		// nothing
	};

	inline void Update(const Params& params) {
		RequestUpdate(MSG_UPDATE,
				reinterpret_cast<ContextObject::ContextParamPtr>(&params));
	}

protected:

};

} /* namespace nextar */

#endif /* CORE_ENGINE_INCLUDE_RENDERSTATE_H_ */
