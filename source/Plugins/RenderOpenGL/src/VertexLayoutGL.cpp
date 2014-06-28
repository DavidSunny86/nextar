/*
 * VertexLayoutGL.cpp
 *
 *  Created on: 14-Jun-2014
 *      Author: obhi
 */
#include <RenderOpenGL.h>
#include <VertexLayoutGL.h>
#include <RenderContextGL.h>

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
	RenderContextGL* rcGL = static_cast<RenderContextGL*>(rc);
	for (auto &p : passVertexArrayPairs) {
		rcGL->DestroyVAO(p.second);
	}
	passVertexArrayPairs.clear();
}

void VertexLayoutStaticGL::ClearAllVAO(RenderContextGL* rcGL) {
	for (auto &p : passVertexArrayPairs) {
		rcGL->DestroyVAO(p.second);
	}
	passVertexArrayPairs.clear();
}

void VertexLayoutStaticGL::Update(nextar::RenderContext* rc, uint32 msg,
		ContextObject::ContextParamPtr param) {
	if (msg == VertexLayout::MSG_CREATE) {
		VertexLayoutGL::Update(rc, msg, param);
		RenderContextGL* rcGL = static_cast<RenderContextGL*>(rc);
		ClearAllVAO(rcGL);
		cachedIndex = 0;
		bindingNumber = -1;
	}
}

void VertexLayoutStaticGL::Enable(VertexBufferBinding& binding,
		PassViewGL* pass, RenderContextGL* rc) {
	uint32 id = pass->GetInputLayoutID();
	if (binding.GetBindingNumber() != bindingNumber) {
		bindingNumber = binding.GetBindingNumber();
		ClearAllVAO(rc);
		passVertexArrayPairs.clear();
		cachedIndex = 0;
	}
	uint32 numPairs = passVertexArrayPairs.size();
	uint32 count = 0;
	for (; count < numPairs; cachedIndex = (cachedIndex+1)%numPairs, count++) {
		if (passVertexArrayPairs[cachedIndex].first == id) {
			break;
		}
	}
	if (count == numPairs) {
		GLuint layout = CreateLayout(rc, binding, pass->GetInputSemantics());
		cachedIndex = numPairs;
		passVertexArrayPairs.push_back(PassLayoutVertexArrayPair(id, layout));
	}

	rc->EnableVertexArrayObject(passVertexArrayPairs[cachedIndex].second);
}

void VertexLayoutStaticGL::Disable(RenderContextGL* rc) {
	rc->DisableVertexArrayObject();
}

GLuint VertexLayoutStaticGL::CreateLayout(RenderContextGL* gl,
		VertexBufferBinding& binding,
		const VertexSemanticListGL& semantics) {

	uint16 outElements[RenderConstants::MAX_VERTEX_ELEMENT];
	uint32 numMapped = VertexSemantic::MapSignatureToSemantics(
			&semantics[0].semantic, sizeof(VertexSemanticGL), semantics.size(),
			&attributes[0].element, sizeof(VertexAttribGL), attributes.size(),
			outElements);

	if (numMapped) {
		uint32 stream = -1;
		uint32 stride = 0;
		GLuint vao;
		gl->GlGenVertexArrays(1, &vao);
		GL_CHECK();
		gl->GlBindVertexArray(vao);
		GL_CHECK();

		for (uint32 j = 0; j < numMapped; ++j) {
			NEX_ASSERT(semantics[j].index < (uint16 )-1);
			VertexAttribGL& vegl = attributes[outElements[j]];
			if (vegl.element.streamIndex != stream) {
				stream = vegl.element.streamIndex;
				GpuBufferViewGL* bufferView = static_cast<GpuBufferViewGL*>(
											gl->GetView(binding.GetBufferPtr(stream)));
				stride = bufferView->GetStride();
				gl->SetVertexBuffer(bufferView);
			}
			gl->EnableVertexAttribute(semantics[j].index, stride, vegl);
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
	RenderContextGL* rcGL = static_cast<RenderContextGL*>(rc);
	for (auto &p : passVertexArrayPairs) {
		rcGL->DestroyVAO(p.second);
	}
	passVertexArrayPairs.clear();
}

void VertexLayoutFlexibleGL::ClearAllVAO(RenderContextGL* rcGL) {
	for (auto &p : passVertexArrayPairs) {
		rcGL->DestroyVAO(p.second);
	}
	passVertexArrayPairs.clear();
}

void VertexLayoutFlexibleGL::Update(nextar::RenderContext* rc, uint32 msg,
		ContextObject::ContextParamPtr param) {
	if (msg == VertexLayout::MSG_CREATE) {
		VertexLayoutGL::Update(rc, msg, param);
		RenderContextGL* rcGL = static_cast<RenderContextGL*>(rc);
		ClearAllVAO(rcGL);
		cachedIndex = 0;
	}
}

void VertexLayoutFlexibleGL::Enable(VertexBufferBinding& binding,
		PassViewGL* pass, RenderContextGL* rc) {
	numSyncRequired = 0;
	for(uint32 stream = 0; stream < binding.GetBufferCount(); ++stream) {
		GpuBufferViewGL* buffer = static_cast<GpuBufferViewGL*>(
							rc->GetView(binding.GetBufferPtr(stream)));
		rc->GlBindVertexBuffer(stream, buffer->GetBufferId(), 0, buffer->GetStride());
		if (buffer->IsSyncRequired())
			transients[numSyncRequired++] = buffer;
		GL_CHECK();
	}
	uint32 id = pass->GetInputLayoutID();
	uint32 numPairs = passVertexArrayPairs.size();
	uint32 count = 0;
	for (; count < numPairs; cachedIndex = (cachedIndex+1)%numPairs, count++) {
		if (passVertexArrayPairs[cachedIndex].first == id) {
			break;
		}
	}
	if (count == numPairs) {
		GLuint layout = CreateLayout(rc, pass->GetInputSemantics());
		cachedIndex = numPairs;
		passVertexArrayPairs.push_back(PassLayoutVertexArrayPair(id, layout));
	}
	rc->EnableVertexArrayObject(passVertexArrayPairs[cachedIndex].second);
}

void VertexLayoutFlexibleGL::Disable(RenderContextGL* rc) {
	rc->DisableVertexArrayObject();
	for (uint32 i  = 0; i < numSyncRequired; ++i) {
		static_cast<GpuTransientBufferViewGL*>(transients[i])->Sync(rc);
	}
}

GLuint VertexLayoutFlexibleGL::CreateLayout(RenderContextGL* gl,
		const VertexSemanticListGL& semantics) {
	uint16 outElements[RenderConstants::MAX_VERTEX_ELEMENT];
	uint32 numMapped = VertexSemantic::MapSignatureToSemantics(
				&semantics[0].semantic, sizeof(VertexSemanticGL), semantics.size(),
				&attributes[0].element, sizeof(VertexAttribGL), attributes.size(),
				outElements);
	if (numMapped) {
		uint16 stream = -1;
		GLuint vao;
		gl->GlGenVertexArrays(1, &vao);
		GL_CHECK();
		gl->GlBindVertexArray(vao);
		GL_CHECK();
		for (uint32 j = 0; j < numMapped; ++j) {
			NEX_ASSERT(semantics[j].index < (uint16 )-1);
			VertexAttribGL& vegl = attributes[outElements[j]];
			gl->GlVertexAttribBinding(semantics[j].index, vegl.element.streamIndex);
			gl->EnableVertexAttribute(semantics[j].index, vegl);
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
		PassViewGL* pass, RenderContextGL* rc) {

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
			current = current + 2 + selected->numAttributes * 2;
		}
		if (i == numberOfStoredPassMapping) {
			uint16 outElements[RenderConstants::MAX_VERTEX_ELEMENT];
			const VertexSemanticListGL& semantics = pass->GetInputSemantics();
			uint32 numMapped = VertexSemantic::MapSignatureToSemantics(
					&semantics[0].semantic, sizeof(VertexSemanticGL),
					semantics.size(), &attributes[0].element,
					sizeof(VertexAttribGL), attributes.size(), outElements);
			if (numMapped) {
				numberOfStoredPassMapping++;
				indices.resize(indices.size() + 2 + numMapped * 2);
				cached =
						reinterpret_cast<MappedVertexAttribList*>((&indices.back())
								- 2 - numMapped * 2);
				cached->numAttributes = numMapped;
				cached->passId = id;
				for (uint32 j = 0; j < numMapped; ++j) {
					NEX_ASSERT(semantics[j].index < (uint16 )-1);
					cached->attribs[j].attribIndex = outElements[j];
					cached->attribs[j].index = (uint16) semantics[j].index;
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
	numSyncRequired = 0;
	for (uint16 s = 0; s < numAttributes; ++s) {
		MappedVertexAttrib& mva = cached->attribs[s];
		VertexAttribGL& vegl = attributes[mva.attribIndex];
		if (vegl.element.streamIndex != stream) {
			stream = vegl.element.streamIndex;
			GpuBufferViewGL* buffer = static_cast<GpuBufferViewGL*>(
								rc->GetView(binding.GetBufferPtr(stream)));
			stride = buffer->GetStride();
			rc->SetVertexBuffer(buffer);
			if (buffer->IsSyncRequired())
				transients[numSyncRequired++] = buffer;
		}
		rc->EnableVertexAttribute(mva.index, stride, vegl);
	}
}

void VertexLayoutDynamicGL::Disable(RenderContextGL* rc) {
	NEX_ASSERT(cached);
	uint16 numAttributes = cached->numAttributes;
	for (uint16 s = 0; s < numAttributes; ++s) {
		MappedVertexAttrib& mva = cached->attribs[s];
		rc->DisableVertexAttribute(mva.index);
	}
	for (uint32 i  = 0; i < numSyncRequired; ++i) {
		static_cast<GpuTransientBufferViewGL*>(transients[i])->Sync(rc);
	}
}

} /* namespace RenderOpenGL */
