/*
 * RenderConstants.cpp
 *
 *  Created on: 05-Mar-2017
 *      Author: obhi
 */

#include <RenderConstants.h>

namespace nextar {

/****************************************************/
/* Pass::ProgramStage
 /****************************************************/
const char* PipelineStage::ToString(PipelineStage::Name type) {
	static const char* stage[] = {
			"Vertex",
			"Hull",
			"Domain",
			"Geometry",
			"Fragment",
			0,
			"GpGpu"
	};
	return stage[type];
}




}
