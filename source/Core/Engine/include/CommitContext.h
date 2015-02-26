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

enum class ClearFlags {
	CLEAR_NONE = 0,
	CLEAR_COLOR = 1 << 0,
	CLEAR_DEPTH = 1 << 1,
	CLEAR_STENCIL = 1 << 2,
	CLEAR_ALL = CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL
};

struct RenderInfo {
	RenderTarget* rt;
	ClearFlags clearFlags;
	Color clearColor;
	float clearDepth;
	uint16 clearStencil;

	RenderInfo() : rt(0), clearFlags(ClearFlags::CLEAR_NONE), clearDepth(1.0f), clearStencil(0) {}
};

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
	float frameTime;
	Size targetDimension;

	Color color;

	const ParameterBuffer* paramBuffers[(uint32) ParameterContext::CTX_COUNT];
	// rename the
	ParamContext paramContext;
	ParameterGroup* paramGroup;
	void* groupDataPtr;
	const FrameTimer* frameTimer;
		
	TextureBase* albedoAndGlossMap;
	TextureBase* depthMap;
	TextureBase* normalMap;

	Light* light; // for deferred
	Pass::View* pass;
	ShaderAsset* shader;
	VisibilitySet* visibiles;
	LightSystem* lightSystem;
	//RenderTarget* renderTarget;
	Viewport* viewport;
	VisiblePrimitive* primitive;
	MaterialAsset* material;
	RenderContext* renderContext;
	RenderInfo renderTargetInfo;

	const Matrix4x4* viewMatrix;
	const Matrix4x4* viewProjectionMatrix;
	const Matrix4x4* projectionMatrix;
	Matrix4x4* invProjectionMatrix;
};

} /* namespace nextar */
#endif /* COMMITCONTEXT_H_ */
