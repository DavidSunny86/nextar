/*
 * CommitContext.h
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#ifndef COMMITCONTEXT_H_
#define COMMITCONTEXT_H_

#include <NexBase.h>
#include <ShaderParam.h>
#include <ParameterBuffer.h>
#include <Pass.h>

namespace nextar {

class CommitContext: public AllocGeneral {
public:

	typedef std::pair<uint32, const ParameterBuffer*> ParamContext;
	
	CommitContext();
	virtual ~CommitContext();

	void _Reset();

	uint32 frameNumber;
	uint32 viewNumber;
	uint32 passNumber;
	uint32 materialNumber;
	Size targetDimension;

	const ParameterBuffer* paramBuffers[(uint32) ParameterContext::CTX_COUNT];

	ParamContext currentParamContext;
	ParameterGroup* currentGroup;
	void* currentGroupDataPtr;

	Pass::View* pass;
	ShaderAsset* shader;
	DebugDisplay* debugDisplay;
	VisibilitySet* visibiles;
	LightSystem* lightSystem;
	RenderTarget* renderTarget;
	Viewport* viewport;
	VisiblePrimitive* primitive;
	MaterialAsset* material;
	RenderContext* renderContext;
};
} /* namespace nextar */
#endif /* COMMITCONTEXT_H_ */
