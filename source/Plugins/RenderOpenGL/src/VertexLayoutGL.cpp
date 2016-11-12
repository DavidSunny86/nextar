/*
 * VertexLayoutGL.cpp
 *
 *  Created on: 14-Jun-2014
 *      Author: obhi
 */
#include <RenderOpenGL.h>
#include <VertexLayoutGL.h>
#include <RenderContext_Base_GL.h>

namespace RenderOpenGL {

/************************ VertexLayoutGL ************************/
VertexLayoutGL::VertexLayoutGL() {
}

uint16 VertexLayoutGL::GetFromGlType(GLenum e) {
	switch (e) {
	case GL_FLOAT:
		return COMP_TYPE_FLOAT1;
	case GL_FLOAT_VEC2:
		return COMP_TYPE_FLOAT2;
	case GL_FLOAT_VEC3:
		return COMP_TYPE_FLOAT3;
	case GL_FLOAT_VEC4:
		return COMP_TYPE_FLOAT4;
	case GL_UNSIGNED_INT:
		return COMP_TYPE_UNSIGNED_INT;
	default:
		NEX_ASSERT(0);
	}
	return -1;
}

void VertexLayoutGL::Populate(VertexAttribGL& attrib) {
	attrib.normalize = false;
	attrib.integer = false;
	switch (attrib.element.desc.semantic.type) {
	case COMP_TYPE_FLOAT1:
		attrib.dttype = GL_FLOAT;
		attrib.elementCount = 1;
		break;
	case COMP_TYPE_FLOAT2:
		attrib.dttype = GL_FLOAT;
		attrib.elementCount = 2;
		break;
	case COMP_TYPE_FLOAT3:
		attrib.dttype = GL_FLOAT;
		attrib.elementCount = 3;
		break;
	case COMP_TYPE_FLOAT4:
		attrib.dttype = GL_FLOAT;
		attrib.elementCount = 4;
		break;
	case COMP_TYPE_UNSIGNED_INT:
		attrib.dttype = GL_UNSIGNED_INT;
		attrib.elementCount = 1;
		attrib.integer = true;
		break;
	case COMP_TYPE_COLOR:
		attrib.dttype = GL_UNSIGNED_BYTE;
		attrib.elementCount = 4;
		attrib.normalize = true;
		break;
	default:
		NEX_ASSERT(0);
	}
}

void VertexLayoutGL::Update(nextar::RenderContext* rc, uint32 msg,
		ContextObject::ContextParamPtr param) {
	if (param && msg == VertexLayout::MSG_CREATE) {
		const VertexLayout::CreateParam* up =
				reinterpret_cast<const VertexLayout::CreateParam*>(param);
		attributes.resize(up->numElements);
		const VertexElement* el = up->elements;
		for (auto& a : attributes) {
			a.element = *el++;
			Populate(a);
		}
	}
}

/************************ VertexLayoutStaticGL ************************/
VertexLayoutStaticGL::VertexLayoutStaticGL() :
		cachedIndex(0), bindingNumber(-1), VertexLayoutGL() {
}

VertexLayoutStaticGL::~VertexLayoutStaticGL() {
}

void VertexLayoutStaticGL::Destroy(nextar::RenderContext* rc) {
	RenderContext_Base_GL* rcGL = static_cast<RenderContext_Base_GL*>(rc);
	for (auto &p : passVertexArrayPairs) {
		rcGL->DestroyVAO(p.second);
	}
	passVertexArrayPairs.clear();
}

void VertexLayoutStaticGL::ClearAllVAO(RenderContext_Base_GL* rcGL) {
	for (auto &p : passVertexArrayPairs) {
		rcGL->DestroyVAO(p.second);
	}
	passVertexArrayPairs.clear();
}

void VertexLayoutStaticGL::Update(nextar::RenderContext* rc, uint32 msg,
		ContextObject::ContextParamPtr param) {
	if (msg == VertexLayout::MSG_CREATE) {
		VertexLayoutGL::Update(rc, msg, param);
		RenderContext_Base_GL* rcGL = static_cast<RenderContext_Base_GL*>(rc);
		ClearAllVAO(rcGL);
		cachedIndex = 0;
		bindingNumber = -1;
	}
}

void VertexLayoutStaticGL::Enable(VertexBufferBinding& binding,
		PassViewGL* pass, RenderContext_Base_GL* rc) {
	uint32 id = pass->GetInputLayoutID();
	if (binding.GetBindingNumber() != bindingNumber) {
		bindingNumber = binding.GetBindingNumber();
		ClearAllVAO(rc);
		passVertexArrayPairs.clear();
		cachedIndex = 0;
	}
	uint32 numPairs = (uint32)passVertexArrayPairs.size();
	uint32 count = 0;
	for (;
		count < numPairs && passVertexArrayPairs[cachedIndex].first != id;
		cachedIndex = (cachedIndex + 1) % numPairs, count++);

	if (count == numPairs) {
		GLuint layout = CreateLayout(rc, binding, pass->GetInputLayoutID());
		cachedIndex = numPairs;
		passVertexArrayPairs.push_back(PassLayoutVertexArrayPair(id, layout));
		rc->DisableVertexArrayObject();
	}

	rc->EnableVertexArrayObject(passVertexArrayPairs[cachedIndex].second);
}

void VertexLayoutStaticGL::Disable(RenderContext_Base_GL* rc) {
	rc->DisableVertexArrayObject();
}

GLuint VertexLayoutStaticGL::CreateLayout(RenderContext_Base_GL* gl,
		VertexBufferBinding& binding,
		VertexSemanticID semanticID) {

	uint16 outElements[RenderConstants::MAX_VERTEX_ELEMENT];
	VertexSemanticDataGL data = gl->GetInputSemanticsDataFromID(semanticID);
	uint32 numMapped = VertexSemantic::MapSignatureToSemantics(
		&data.rawArray[0].semantic, (uint32)sizeof(VertexSemanticGL), (uint32)data.count,
		&attributes[0].element, (uint32)sizeof(VertexAttribGL), (uint32)attributes.size(),
			outElements);

	if (numMapped) {
		uint32 stream = -1;
		uint32 stride = 0;
		GLuint vao = gl->CreateVAO();
		uint32 offset = 0;
		gl->EnableVertexArrayObject(vao);

		for (uint32 j = 0; j < numMapped; ++j) {
			NEX_ASSERT(data.rawArray[j].index < (uint16)-1);
			VertexAttribGL& vegl = attributes[outElements[j]];
			if (vegl.element.streamIndex != stream) {
				stream = vegl.element.streamIndex;
				GpuBufferViewGL* bufferView = static_cast<GpuBufferViewGL*>(
											gl->GetView(binding.GetBufferPtr(stream)));
				stride = bufferView->GetStride();
				offset = bufferView->GetOffset();
				gl->SetVertexBuffer(bufferView);
			}
			gl->EnableVertexAttribute(data.rawArray[j].index, stride, offset, vegl);
		}
		return vao;
	}
	return 0;
}

/************************ VertexLayoutFlexibleGL ************************/
VertexLayoutFlexibleGL::VertexLayoutFlexibleGL() :
		cachedIndex(0), numSyncRequired(0) {
}

VertexLayoutFlexibleGL::~VertexLayoutFlexibleGL() {
}

void VertexLayoutFlexibleGL::Destroy(nextar::RenderContext* rc) {
	RenderContext_Base_GL* rcGL = static_cast<RenderContext_Base_GL*>(rc);
	for (auto &p : passVertexArrayPairs) {
		rcGL->DestroyVAO(p.second);
	}
	passVertexArrayPairs.clear();
}

void VertexLayoutFlexibleGL::ClearAllVAO(RenderContext_Base_GL* rcGL) {
	for (auto &p : passVertexArrayPairs) {
		rcGL->DestroyVAO(p.second);
	}
	passVertexArrayPairs.clear();
}

void VertexLayoutFlexibleGL::Update(nextar::RenderContext* rc, uint32 msg,
		ContextObject::ContextParamPtr param) {
	if (msg == VertexLayout::MSG_CREATE) {
		VertexLayoutGL::Update(rc, msg, param);
		RenderContext_Base_GL* rcGL = static_cast<RenderContext_Base_GL*>(rc);
		ClearAllVAO(rcGL);
		cachedIndex = 0;
	}
}

void VertexLayoutFlexibleGL::Enable(VertexBufferBinding& binding,
		PassViewGL* pass, RenderContext_Base_GL* rc) {
	numSyncRequired = 0;
	uint32 id = pass->GetInputLayoutID();
	uint32 numPairs = (uint32)passVertexArrayPairs.size();
	uint32 count = 0;
	for (;
		count < numPairs && passVertexArrayPairs[cachedIndex].first != id;
		cachedIndex = (cachedIndex + 1) % numPairs, count++);

	if (count == numPairs) {
		GLuint layout = CreateLayout(rc, pass->GetInputLayoutID());
		cachedIndex = numPairs;
		passVertexArrayPairs.push_back(PassLayoutVertexArrayPair(id, layout));
		rc->DisableVertexArrayObject();
	}

	rc->EnableVertexArrayObject(passVertexArrayPairs[cachedIndex].second);
	for (uint32 stream = 0; stream < binding.GetBufferCount(); ++stream) {
		GpuBufferViewGL* buffer = static_cast<GpuBufferViewGL*>(
			rc->GetView(binding.GetBufferPtr(stream)));
		rc->BindVertexBuffer(stream, binding.GetBufferOffset(stream) + buffer->GetOffset()
			, buffer);
		if (buffer->IsSyncRequired())
			transients[numSyncRequired++] = buffer;
	}

}

void VertexLayoutFlexibleGL::Disable(RenderContext_Base_GL* rc) {
	rc->DisableVertexArrayObject();
	for (uint32 i  = 0; i < numSyncRequired; ++i) {
		static_cast<GpuTransientBufferViewGL*>(transients[i])->Sync(rc);
	}
}

GLuint VertexLayoutFlexibleGL::CreateLayout(RenderContext_Base_GL* gl,
											VertexSemanticID semanticID) {
	uint16 outElements[RenderConstants::MAX_VERTEX_ELEMENT];
	VertexSemanticDataGL data = gl->GetInputSemanticsDataFromID(semanticID);
	uint32 numMapped = VertexSemantic::MapSignatureToSemantics(
		&data.rawArray[0].semantic, (uint32)sizeof(VertexSemanticGL), (uint32)data.count,
		&attributes[0].element, (uint32)sizeof(VertexAttribGL), (uint32)attributes.size(),
				outElements);
	if (numMapped) {
		uint16 stream = -1;
		GLuint vao = gl->CreateVAO();
		gl->EnableVertexArrayObject(vao);

		for (uint32 j = 0; j < numMapped; ++j) {
			NEX_ASSERT(data.rawArray[j].index < (uint16)-1);
			VertexAttribGL& vegl = attributes[outElements[j]];
			gl->VertexAttribBinding(data.rawArray[j].index, vegl.element.streamIndex);
			gl->EnableVertexAttribute(data.rawArray[j].index, 0, vegl);
		}
		return vao;
	}
	return 0;
}

/************************ VertexLayoutDynamicGL ************************/
VertexLayoutDynamicGL::VertexLayoutDynamicGL() :
		numSyncRequired(0), numberOfStoredPassMapping(0), cached(0), VertexLayoutGL() {
}

VertexLayoutDynamicGL::~VertexLayoutDynamicGL() {
}

void VertexLayoutDynamicGL::Enable(VertexBufferBinding& binding,
		PassViewGL* pass, RenderContext_Base_GL* rc) {

	uint16 id = pass->GetInputLayoutID();

	MappedVertexAttribList* selected = nullptr;
	// todo cache??
	if (!cached || cached->passId != id) {
		uint16* current = indices.data();
		uint16 i = 0;
		for (; i < numberOfStoredPassMapping; ++i) {
			selected = reinterpret_cast<MappedVertexAttribList*>(current);
			if (selected->passId == id) {
				cached = selected;
				break;
			}
			current = current + MappedVertexAttribList::HEADER_SIZE + selected->numAttributes * 2;
		}
		if (i == numberOfStoredPassMapping) {
			uint16 outElements[RenderConstants::MAX_VERTEX_ELEMENT];
			VertexSemanticDataGL data = rc->GetInputSemanticsDataFromID(pass->GetInputLayoutID());
			uint32 numMapped = VertexSemantic::MapSignatureToSemantics(
				&data.rawArray[0].semantic, (uint32)sizeof(VertexSemanticGL),
					(uint32)data.count, &attributes[0].element,
					(uint32)sizeof(VertexAttribGL), (uint32)attributes.size(), outElements);
			if (numMapped) {
				numberOfStoredPassMapping++;
				size_t offset = indices.size();
				indices.resize(offset + MappedVertexAttribList::HEADER_SIZE + numMapped * 2);
				cached = reinterpret_cast<MappedVertexAttribList*>(indices.data() + offset);
				cached->layout = rc->CreateVAO();
				cached->numAttributes = numMapped;
				cached->passId = id;
				for (uint32 j = 0; j < numMapped; ++j) {
					NEX_ASSERT(data.rawArray[j].index < (uint16)-1);
					cached->attribs[j].attribIndex = outElements[j];
					cached->attribs[j].index = (uint16)data.rawArray[j].index;
				}
			} else {
				Error("Unable to map vertex layout to semantics.");
				return;
			}
			// add
		}
	}

	NEX_ASSERT(cached && cached->passId == id);

	uint16 numAttributes = cached->numAttributes;
	uint16 stream = -1;
	uint32 stride = 0;
	uint32 offset = 0;
	numSyncRequired = 0;
	rc->EnableVertexArrayObject(cached->layout);
	for (uint16 s = 0; s < numAttributes; ++s) {
		MappedVertexAttrib& mva = cached->attribs[s];
		VertexAttribGL& vegl = attributes[mva.attribIndex];
		if (vegl.element.streamIndex != stream) {
			stream = vegl.element.streamIndex;
			GpuBufferViewGL* buffer = static_cast<GpuBufferViewGL*>(
								rc->GetView(binding.GetBufferPtr(stream)));
			stride = buffer->GetStride();
			offset = buffer->GetOffset();
			rc->SetVertexBuffer(buffer);
			
			if (buffer->IsSyncRequired())
				transients[numSyncRequired++] = buffer;
		}
		rc->EnableVertexAttribute(mva.index, stride, offset, vegl);
	}
}

void VertexLayoutDynamicGL::Disable(RenderContext_Base_GL* rc) {
	NEX_ASSERT(cached);
	uint16 numAttributes = cached->numAttributes;
	for (uint16 s = 0; s < numAttributes; ++s) {
		MappedVertexAttrib& mva = cached->attribs[s];
		rc->DisableVertexAttribute(mva.index);
	}
	for (uint32 i  = 0; i < numSyncRequired; ++i) {
		static_cast<GpuTransientBufferViewGL*>(transients[i])->Sync(rc);
	}
	rc->DisableVertexArrayObject();
}

} /* namespace RenderOpenGL */
