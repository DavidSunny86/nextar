
#include <RenderEngineHeaders.h>
#include <NexRenderEngine.h>
#include <AutoParamDefs.h>
#include <RenderManager.h>

namespace nextar {

#define MAKE_AUTO_PARAM_REGISTER(variable, autoName, type, context, size, description)	\
	Pass::AddParamDef(autoName, type, context, &variable, size/*, description*/)

#define MAKE_AUTO_PARAM_REF(Class)		Class##Apply

#define MAKE_AUTO_PARAM(Class, variableName, type, context, size, description)	\
	MAKE_AUTO_PARAM_REGISTER(MAKE_AUTO_PARAM_REF(Class), variableName, type, context, size, description)

void SkyLightApply(CommitContext& context, const ShaderParameter* d) {
	Light* light = context.lightSystem->GetSkyLight();
	if (light) {
		Vector4A v = light->GetDirectionVector();
		v = Vec3ANegate(Mat4x4RotateVec3A(v, *context.viewMatrix));
		context.paramGroup->WriteRawData(context.renderContext, light->GetLightColor().AsFloatArray(), 0, 16);
		context.paramGroup->WriteRawData(context.renderContext, &v, 16, 12);
	} else {
		Vector3A v = Vec3ASet(0, -1, 0);
		v = Vec3ANegate(Mat4x4RotateVec3A(v, *context.viewMatrix));
		context.paramGroup->WriteRawData(context.renderContext, Color::White.AsFloatArray(), 0, 16);
		context.paramGroup->WriteRawData(context.renderContext, &v, 16, 12);
	}
}

void ObjectTransformsApply(CommitContext& context, const ShaderParameter* d) {
	NEX_ASSERT(context.primitive);
	const Matrix4x4* m = context.primitive->GetWorldMatrices();
	const Matrix4x4* vp = context.viewProjectionMatrix;
	Matrix4x4 mvp = Mat4x4Mul(*m, *vp);
	context.paramGroup->WriteRawData(context.renderContext, &mvp, 0, 16 * 4);
	Matrix4x4 modelView = Mat4x4Mul(*m, *context.viewMatrix);
	context.paramGroup->WriteRawData(context.renderContext, &modelView, 16 * 4, 16 * 4);
}

void ModelViewProjectionApply(CommitContext& context, const ShaderParameter* d) {
	NEX_ASSERT(context.primitive);
	const Matrix4x4* m = context.primitive->GetWorldMatrices();
	const Matrix4x4* vp = context.viewProjectionMatrix;
	Matrix4x4 mvp = Mat4x4Mul(*m, *vp);
	const ConstantParameter* constParam =
		reinterpret_cast<const ConstantParameter*>(d);
	context.paramGroup->SetRawBuffer(context.renderContext, *constParam,
		&mvp);
}

void DiffuseColorApply(CommitContext& context, const ShaderParameter* d) {
	const ConstantParameter* constParam =
		reinterpret_cast<const ConstantParameter*>(d);
	context.paramGroup->SetRawBuffer(context.renderContext, *constParam,
		&context.color);
}

void InvProjectionTransformApply(CommitContext& context, const ShaderParameter* param) {
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
	const ParameterGroup* constParam =
		static_cast<const ParameterGroup*>(param);
	context.paramGroup->WriteRawData(context.renderContext, &params, 0, constParam->size);
}

void AlbedoMapApply(CommitContext& context, const ShaderParameter* param) {
	const SamplerParameter* samplerParam =
		static_cast<const SamplerParameter*>(param);
	context.pass->SetTexture(context.renderContext, *samplerParam,
			context.albedoMap);
}

void SpecularAndGlossMapApply(CommitContext& context, const ShaderParameter* param) {
	const SamplerParameter* samplerParam =
		static_cast<const SamplerParameter*>(param);
	context.pass->SetTexture(context.renderContext, *samplerParam,
			context.specularMap);
}

void DepthMapApply(CommitContext& context, const ShaderParameter* param) {
	const SamplerParameter* samplerParam =
		static_cast<const SamplerParameter*>(param);
	context.pass->SetTexture(context.renderContext, *samplerParam,
			context.depthMap);
}

void NormalMapApply(CommitContext& context, const ShaderParameter* param) {
	const SamplerParameter* samplerParam =
		static_cast<const SamplerParameter*>(param);
	context.pass->SetTexture(context.renderContext, *samplerParam,
			context.normalMap);
}

void ConstantScaleFactorApply(CommitContext& context, const ShaderParameter* param) {
	NEX_ASSERT(context.primitive);
	const ConstantParameter* constParam =
		static_cast<const ConstantParameter*>(param);

	const Matrix4x4* m = context.primitive->GetWorldMatrices();
	float constantSize = context.primitive->GetConstantSize();
	if (m && constantSize > 0.0f && context.camera) {
		Vector3A first = Vec3AFromVec4A(Mat4x4Row(*m, 3));
		Vector3A second = context.camera->GetTranslation();
		float distance = Vec3ADistance(second, first);
		float distanceScale = constantSize * distance * 0.001f;
		context.paramGroup->SetRawBuffer(context.renderContext, *constParam, &distanceScale);
	} else {
		constantSize = 1.0f;
		context.paramGroup->SetRawBuffer(context.renderContext, *constParam, &constantSize);
	}
}

void BaseRenderManager::RegisterAutoParams() {

	MAKE_AUTO_PARAM(ModelViewProjection, AutoParamName::AUTO_MODEL_VIEW_PROJECTION, PDT_MAT4x4,
		ParameterContext::CTX_OBJECT, 64, "Model view projection matrix.");
	MAKE_AUTO_PARAM(DiffuseColor, AutoParamName::AUTO_DIFFUSE_COLOR, PDT_VEC4,
		ParameterContext::CTX_OBJECT, 16, "Diffuse color.");
	MAKE_AUTO_PARAM(ObjectTransforms, AutoParamName::AUTO_OBJECT_TRANSFORM, PDT_STRUCT,
		ParameterContext::CTX_OBJECT, 128, "Model view projection and model view matrix.");
	MAKE_AUTO_PARAM(SkyLight, AutoParamName::AUTO_SKY_LIGHT, PDT_STRUCT,
		ParameterContext::CTX_FRAME, 28, "Sky lighting.");
	MAKE_AUTO_PARAM(InvProjectionTransform, AutoParamName::AUTO_INV_PROJECTION, PDT_MAT4x4,
		ParameterContext::CTX_VIEW, 64, "Inverse projection matrix.");
	MAKE_AUTO_PARAM(OmniLightProperties, AutoParamName::AUTO_OMNI_LIGHT_PROPERTIES, PDT_STRUCT,
		ParameterContext::CTX_OBJECT, 32, "Omni light properties.");
	MAKE_AUTO_PARAM(AlbedoMap, AutoParamName::AUTO_ALBEDO_MAP, PDT_TEXTURE,
		ParameterContext::CTX_PASS, sizeof(TextureUnit), "GBuffer albedo map.");
		MAKE_AUTO_PARAM(SpecularAndGlossMap, AutoParamName::AUTO_SPECULAR_AND_GLOSS_MAP, PDT_TEXTURE,
			ParameterContext::CTX_PASS, sizeof(TextureUnit), "GBuffer specular map.");
	MAKE_AUTO_PARAM(NormalMap, AutoParamName::AUTO_NORMAL_MAP, PDT_TEXTURE,
		ParameterContext::CTX_PASS, sizeof(TextureUnit), "GBuffer normal map.");
	MAKE_AUTO_PARAM(DepthMap, AutoParamName::AUTO_DEPTH_MAP, PDT_TEXTURE,
		ParameterContext::CTX_PASS, sizeof(TextureUnit), "GBuffer depth map.");
	MAKE_AUTO_PARAM(ConstantScaleFactor, AutoParamName::AUTO_CONSTANT_SCALE_FACTOR, PDT_FLOAT,
		ParameterContext::CTX_OBJECT, 4, "Constant scale factor.");
}

}
