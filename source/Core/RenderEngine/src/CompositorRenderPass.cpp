/*
 * CopyRenderPass.cpp
 *
 *  Created on: 08-Feb-2016
 *      Author: obhi
 */


#include <CompositorRenderPass.h>
#include <RenderSystem.h>

namespace nextar {

CompositorRenderPass::CompositorRenderPass() : 
numTextureToResolve(0)
, renderSystemTicket(-1) {
	_rtBunchOf = nullptr;
}

CompositorRenderPass::~CompositorRenderPass() {
	if (numTextureToResolve > 1 && _rtBunchOf) {
		NEX_FREE(_rtBunchOf, MEMCAT_GENERAL);
	}
}

RenderPass* CompositorRenderPass::CreateInstance() {
	return NEX_NEW(CompositorRenderPass());
}

void CompositorRenderPass::OnMaterialLoad() {
	ShaderAssetPtr shader = material->GetShader();
	if (shader) {
		const ParamEntryTableItem& peti = shader->GetParamTableItem(ParameterContext::CTX_OBJECT);
		if (!(flags & PARAMS_LOADED) || !parameters.Validate(peti))
			parameters.Prepare(peti);
		return;
	}

	NEX_THROW_GracefulError(EXCEPT_FAILED_TO_CREATE_OBJECT);
}

void CompositorRenderPass::Commit(CommitContext& context) {
	
	if (context.rsys->GetTicket() != this->renderSystemTicket)	{
		if (numTextureToResolve == 1) {
			TextureUnit tu;
			RenderTarget* target = context.GetTargetByName(_rtJustOne.name);
			if (target && target->GetRenderTargetType() == RenderTargetType::TEXTURE) {
				tu.texture = static_cast<RenderTexture*>(target);
				parameters.SetData(&tu, _rtJustOne.offset);
			}
		} else {
			for (uint32 i = 0; i < numTextureToResolve; ++i) {
				TextureUnit tu;
				RenderTarget* target = context.GetTargetByName(_rtBunchOf[i].name);
				if (target && target->GetRenderTargetType() == RenderTargetType::TEXTURE) {
					tu.texture = static_cast<RenderTexture*>(target);
					parameters.SetData(&tu, _rtBunchOf[i].offset);
				}
			}
		}
		this->renderSystemTicket = context.rsys->GetTicket();
	}

	BeginRender(context);
	RenderPrimitive(context, (uint32)(ptrdiff_t)this, &primitive);
	EndRender(context);
}

void CompositorRenderPass::CreateResources() {
	BaseMaterialPass::CreateResources();
	if (material) {
		primitive.SetMaterial(material);
		primitive.SetParameters( &parameters );
		primitive.SetStreamData( &RenderManager::Instance().GetFullScreenQuad() );
	}
}

void CompositorRenderPass::DestroyResources() {
	MaterialAssetPtr invalid;
	primitive.SetMaterial(invalid);
	primitive.SetStreamData( nullptr );
	primitive.SetParameters( nullptr );
	parameters.Clear();
	BaseMaterialPass::DestroyResources();
}

void CompositorRenderPass::AddTexturesToResolve(const TexturesToResolve* toResolve,
	uint32 numToResolve) {
	numTextureToResolve = numToResolve;
	if (numToResolve == 1) {
		_rtJustOne = *toResolve;
	} else if (numToResolve > 0) {
		_rtBunchOf = static_cast<TexturesToResolve*>(
			NEX_ALLOC(sizeof(TexturesToResolve) * numToResolve, MEMCAT_GENERAL));
		std::memcpy(_rtBunchOf, toResolve, numToResolve*sizeof(TexturesToResolve));
	}
}

void CompositorRenderPass::Save(RenderSystem* rsysPtr, OutputSerializer& ser) {
	BaseMaterialPass::Save(rsysPtr, ser);
	parameters.AsyncSave(ser);
	ser << numTextureToResolve;
	TexturesToResolve* resolv = (numTextureToResolve == 1) ? &_rtJustOne : _rtBunchOf;
	
	for (uint32 i = 0; i < numTextureToResolve; ++i) {
		uint32 name = (uint32)resolv[i].name;
		uint32 offset = resolv[i].offset;
		ser << name << offset;
	}
}

void CompositorRenderPass::Load(RenderSystem* rsysPtr, InputSerializer& ser) {
	BaseMaterialPass::Load(rsysPtr, ser);
	parameters.AsyncLoad(ser);
	flags |= PARAMS_LOADED;
	uint32 numUnres = 0;
	TexturesToResolve t[RenderTargetName::RT_NAME_COUNT];
	ser >> numUnres;
	for (uint32 i = 0; i < numUnres; ++i) {
		uint32 name = 0;
		uint32 offset = 0;
		ser >> name >> offset;
		t[i].name = (RenderTargetName)name;
		t[i].offset = offset;
	}

	AddTexturesToResolve(t, numUnres);
}

} /* namespace nextar */
