
#include <RenderEngineHeaders.h>
#include <NexRenderEngine.h>
#include <AutoParamDefs.h>
#include <RenderManager.h>

namespace nextar {

#define MAKE_AUTO_PARAM_REGISTER(variable, variableName, type, autoName, context, description)	\
	Pass::AddParamDef(variableName, type, autoName, context, &variable, description)

#define MAKE_AUTO_PARAM_DECL(Class)	static Class _apply##Class;

#define MAKE_AUTO_PARAM_REF(Class)		_apply##Class

#define MAKE_AUTO_PARAM(Class, variableName, type, autoName, context, description)	\
	MAKE_AUTO_PARAM_DECL(Class);													\
	MAKE_AUTO_PARAM_REGISTER(MAKE_AUTO_PARAM_REF(Class), variableName, type, autoName, context, description)

class ObjectTransforms : public AutoParamProcessor {
public:

	virtual void Apply(CommitContext& context, const ShaderParameter* d) {
		NEX_ASSERT(context.primitive);
		const Matrix4x4* m = context.primitive->GetWorldMatrices();
		context.paramGroup->WriteRawData(context.renderContext, m);
		Matrix4x4 modelView = Mat4x4Mul(*m, *context.viewMatrix);
		context.paramGroup->WriteRawData(context.renderContext, &modelView, 16*4);
	}
};

class InvProjectionTransform : public AutoParamProcessor {
public:

	virtual void Apply(CommitContext& context, const ShaderParameter* param) {
		NEX_ASSERT(context.primitive);
		const Matrix4x4* m = context.invProjectionMatrix;
		const ConstantParameter* constParam =
			reinterpret_cast<const ConstantParameter*>(param);
		context.paramGroup->SetRawBuffer(context.renderContext, *constParam, m);
	}
};

class OmniLightProperties : public AutoParamProcessor {
public:

	virtual void Apply(CommitContext& context, const ShaderParameter* param) {
		NEX_ASSERT(context.light);
		Vector4A params[2];
		const Color& color = context.light->GetLightColor();
		params[0] = Vec4ASet(color.red, color.green, color.blue, color.alpha);
		params[1] = Vec4ASetW(context.light->GetTranslation(), 
			context.light->GetRadius());
		const ConstantParameter* constParam =
			reinterpret_cast<const ConstantParameter*>(param);
		context.paramGroup->SetRawBuffer(context.renderContext, *constParam, params);
	}
};

void BaseRenderManager::RegisterAutoParams() {

	MAKE_AUTO_PARAM(ObjectTransforms, "OBJECTTRANSFORMS", PDT_STRUCT, AutoParamName::AUTO_OBJECT_TRANSFORM,
		ParameterContext::CTX_OBJECT, "Model view projection and model view matrix.");
	MAKE_AUTO_PARAM(InvProjectionTransform, "INVPROJECTION", PDT_MAT4x4, AutoParamName::AUTO_INV_PROJECTION,
		ParameterContext::CTX_VIEW, "Inverse projection matrix.");
	MAKE_AUTO_PARAM(OmniLightProperties, "OMNILIGHTPROPERTIES", PDT_STRUCT, AutoParamName::AUTO_OMNI_LIGHT_PROPERTIES,
		ParameterContext::CTX_OBJECT, "Omni light properties.");
}

}