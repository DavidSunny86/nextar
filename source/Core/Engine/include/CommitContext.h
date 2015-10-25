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
#include <RenderConstants.h>

namespace nextar {

enum class ClearFlags : uint16 {
	CLEAR_NONE = 0,
	CLEAR_COLOR_0 = 1 << 0,
	CLEAR_COLOR_1 = 1 << 1,
	CLEAR_COLOR_2 = 1 << 2,
	CLEAR_COLOR_3 = 1 << 3,
	CLEAR_COLOR_4 = 1 << 4,
	CLEAR_COLOR_5 = 1 << 5,
	CLEAR_DEPTH = 1 << 6,
	CLEAR_STENCIL = 1 << 7,
	CLEAR_COLOR = CLEAR_COLOR_0|CLEAR_COLOR_1|CLEAR_COLOR_2|CLEAR_COLOR_3|CLEAR_COLOR_4|CLEAR_COLOR_5,
	CLEAR_ALL = CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL
};
NEX_ENUM_FLAGS(ClearFlags, uint16);

struct ClearBufferInfo {
	ClearFlags clearFlags;
	uint16 clearStencil;
	float clearDepth;
	Color clearColor[RenderConstants::MAX_COLOR_TARGETS];
	ClearBufferInfo() : clearFlags(ClearFlags::CLEAR_NONE), clearDepth(1.0f), clearStencil(0) {}
};

struct RenderInfo {
	RenderTarget* rt;
	ClearBufferInfo info;
	RenderInfo() : rt(0) {}
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

	typedef array<const ParameterBuffer*, (uint32)ParameterContext::CTX_COUNT>::type ParamBufferList;
	ParamBufferList paramBuffers;
	// rename the
	ParamContext paramContext;
	ParameterGroup* paramGroup;
	void* groupDataPtr;
	const FrameTimer* frameTimer;
		
	TextureBase* albedoMap;
	TextureBase* normalMap;
	TextureBase* specularMap;
	TextureBase* depthMap;

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

	// camera and planes
	Camera* camera;

	const Matrix4x4* viewMatrix;
	const Matrix4x4* viewProjectionMatrix;
	const Matrix4x4* projectionMatrix;
	const Matrix4x4* invProjectionMatrix;
	const Matrix4x4* invViewProjectionMatrix;
};

} /* namespace nextar */
#endif /* COMMITCONTEXT_H_ */
