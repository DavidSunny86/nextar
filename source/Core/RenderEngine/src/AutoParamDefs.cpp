
#include <RenderEngineHeaders.h>
#include <NexRenderEngine.h>
#include <AutoParamDefs.h>
#include <RenderManager.h>

namespace nextar {

#define MAKE_AUTO_PARAM_REGISTER(variable, autoName, type, context, description)	\
	Pass::AddParamDef(autoName, type, context, &variable, description)

#define MAKE_AUTO_PARAM_REF(Class)		Class##Apply

#define MAKE_AUTO_PARAM(Class, variableName, type, context, description)	\
	MAKE_AUTO_PARAM_REGISTER(MAKE_AUTO_PARAM_REF(Class), variableName, type, context, description)

void ObjectTransformsApply(CommitContext& context, const ShaderParameter* d) {
	NEX_ASSERT(context.primitive);
	const Matrix4x4* m = context.primitive->GetWorldMatrices();
	context.paramGroup->WriteRawData(context.renderContext, m);
	Matrix4x4 modelView = Mat4x4Mul(*m, *context.viewMatrix);
	context.paramGroup->WriteRawData(context.renderContext, &modelView, 16*4);
}


void InvProjectionTransformApply(CommitContext& context, const ShaderParameter* param) {
	NEX_ASSERT(context.primitive);
	const Matrix4x4* m = context.invProjectionMatrix;
	const ConstantParameter* constParam =
		static_cast<const ConstantParameter*>(param);
	context.paramGroup->SetRawBuffer(context.renderContext, *constParam, m);
}

void OmniLightPropertiesApply(CommitContext& context, const ShaderParameter* param) {
	NEX_ASSERT(context.light);
	Vector4A params[2];
	const Color& color = context.light->GetLightColor();
	params[0] = Vec4ASet(color.red, color.green, color.blue, color.alpha);
	params[1] = Vec4ASetW(context.light->GetTranslation(), 
		context.light->GetRadius());
	const ConstantParameter* constParam =
		static_cast<const ConstantParameter*>(param);
	context.paramGroup->SetRawBuffer(context.renderContext, *constParam, params);
}

void AlbedoAndGlossMapApply(CommitContext& context, const ShaderParameter* param) {
	TextureUnit tu;
	tu.texture = context.albedoAndGlossMap;
	const SamplerParameter* samplerParam =
		static_cast<const SamplerParameter*>(param);
	context.pass->SetTexture(context.renderContext, *samplerParam,
		&tu);
}

void DepthMapApply(CommitContext& context, const ShaderParameter* param) {
	TextureUnit tu;
	tu.texture = context.depthMap;
	const SamplerParameter* samplerParam =
		static_cast<const SamplerParameter*>(param);
	context.pass->SetTexture(context.renderContext, *samplerParam,
		&tu);
}

void NormalMapApply(CommitContext& context, const ShaderParameter* param) {
	TextureUnit tu;
	tu.texture = context.normalMap;
	const SamplerParameter* samplerParam =
		static_cast<const SamplerParameter*>(param);
	context.pass->SetTexture(context.renderContext, *samplerParam,
		&tu);
}

void BaseRenderManager::RegisterAutoParams() {

	MAKE_AUTO_PARAM(ObjectTransforms, AutoParamName::AUTO_OBJECT_TRANSFORM, PDT_STRUCT,
		ParameterContext::CTX_OBJECT, "Model view projection and model view matrix.");
	MAKE_AUTO_PARAM(InvProjectionTransform, AutoParamName::AUTO_INV_PROJECTION, PDT_MAT4x4,
		ParameterContext::CTX_VIEW, "Inverse projection matrix.");
	MAKE_AUTO_PARAM(OmniLightProperties, AutoParamName::AUTO_OMNI_LIGHT_PROPERTIES, PDT_STRUCT,
		ParameterContext::CTX_OBJECT, "Omni light properties.");
	MAKE_AUTO_PARAM(AlbedoAndGlossMap, AutoParamName::AUTO_ALBEDO_AND_GLOSS, PDT_TEXTURE,
		ParameterContext::CTX_PASS, "GBuffer albedo and gloss.");
	MAKE_AUTO_PARAM(NormalMap, AutoParamName::AUTO_NORMAL_MAP, PDT_TEXTURE,
		ParameterContext::CTX_PASS, "GBuffer normal map.");
	MAKE_AUTO_PARAM(DepthMap, AutoParamName::AUTO_DEPTH_MAP, PDT_TEXTURE,
		ParameterContext::CTX_PASS, "GBuffer depth map.");
}

}
