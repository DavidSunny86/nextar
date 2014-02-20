/*
 * Pass.cpp
 *
 *  Created on: 23-Nov-2013
 *      Author: obhi
 */

#include "Pass.h"

namespace nextar {

	Pass::Pass() {
		// TODO Auto-generated constructor stub

	}

	Pass::~Pass() {
		// TODO Auto-generated destructor stub
	}

	bool Pass::NotifyUpdated(RenderContext* rc) {
		bool useFallback = false;
		/** todo Change to bool inliners */
		if( (flags & DIRTY_RASTER_STATE) )
			useFallback &= UpdateRasterStates(rc);
		if( (flags & DIRTY_BLEND_STATE) && !useFallback)
			useFallback &= UpdateBlendStates(rc);
		if( (flags & DIRTY_DEPTHSTENCIL_STATE) && !useFallback)
			useFallback &= UpdateDepthStencilStates(rc);
		if( (flags & DIRTY_TEXUNIT_STATE) && !useFallback )
			useFallback &= UpdateTextureStates(rc);
		if( (flags & COMPILE_NEEDED) && !useFallback )
			useFallback &= Compile(rc);
		return useFallback;

	}

	void Pass::NotifyDestroyed(RenderContext* rc) {
		Decompile(rc);
		flags |= COMPILE_NEEDED;
	}

	void Pass::FinalizeUpdate() {
		_CalculateParamBufferSize();
	}

	void Pass::_CalculateParamBufferSize() {
		/* gather up */
		shaderParamBufferSize = 0;
		for (auto &p : shaderParams) {
			shaderParamBufferSize += ParamSizeFromType((ParamDataType)p.type) * p.count;
		}
	}

} /* namespace nextar */
