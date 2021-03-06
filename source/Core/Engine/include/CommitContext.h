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

struct ClearBufferInfo {
	int16 clearStencil;
	float clearDepth;
	Color clearColor[RenderConstants::MAX_COLOR_TARGETS];
	ClearBufferInfo() : clearDepth(1.0f), clearStencil(0) {}
};

struct RenderInfo {
	RenderTarget* rt;
	ClearBufferInfo info;
	RenderInfo() : rt(0) {}
};


class RenderSystem;
class _NexEngineAPI CommitContext: public AllocGeneral {
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
	Size viewDimensions;

	//Vec4::type sunLightIntensity;
	//Vec4::type sunLightPosition;
	//Color sunLightColor;

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
	RenderTarget* viewRenderTarget;
	RenderTarget* lastRenderTarget;
	RenderSystem* rsys;

	// camera and planes
	Camera* camera;

	const Mat4::type* viewMatrix;
	const Mat4::type* viewProjectionMatrix;
	const Mat4::type* projectionMatrix;
	const Mat4::type* invProjectionMatrix;
	const Mat4::type* invViewProjectionMatrix;

	static RenderTargetName ParseTargetName(const String& target);
	RenderTarget* GetTargetByName(RenderTargetName toLastSubTarget);
};

} /* namespace nextar */
#endif /* COMMITCONTEXT_H_ */
