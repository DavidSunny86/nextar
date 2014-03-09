/* 
 * File:   GlVertexElement.cpp
 * Author: obhi
 * 
 * Created on March 7, 2011, 3:28 AM
 */
#include <RenderOpenGL.h>
#include <VertexElementGL.h>
#include <RenderContextGL.h>
#include <BufferManager.h>
#include <PassGL.h>

namespace RenderOpenGL {

	/************************ VertexLayoutGL ************************/

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

	void VertexLayoutGL::Update(nextar::RenderContext* rc, UpdateParamPtr param) {
		if(param) {
			VertexLayout::UpdateParam* up = reinterpret_cast<UpdateParam*>(param);
			attributes.resize(up->numElements);
			const VertexElement* el = up->elements;
			for(auto& a : attributes) {
				a.element = *el++;
				Populate(a);
			}
		}
	}

	/************************ VertexLayoutStaticGL ************************/
	VertexLayoutStaticGL::VertexLayoutStaticGL() : 
		cachedIndex(-1), bindingNumber(-1),
		VertexLayoutGL() {
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

	void VertexLayoutStaticGL::Update(nextar::RenderContext* rc, UpdateParamPtr param) {
		VertexLayoutGL::Update(rc, param);
		RenderContextGL* rcGL = static_cast<RenderContextGL*>(rc);
		for (auto &p : passVertexArrayPairs) {
			rcGL->DestroyVAO(p.second);
		}
		passVertexArrayPairs.clear();
		
		cachedIndex = -1;
		bindingNumber = -1;
	}

	void VertexLayoutStaticGL::Enable(VertexBufferBinding& binding, PassGL* pass, RenderContextGL* rc) {
		uint32 id = pass->GetInputLayoutID();
		if (binding.GetBindingNumber() != bindingNumber) {
			bindingNumber =  binding.GetBindingNumber();
			passVertexArrayPairs.clear();
			cachedIndex = -1;
		}
		uint32 numPairs = passVertexArrayPairs.size();
		uint32 i = 0;
		cachedIndex = (cachedIndex+1)%numPairs;
		for(; i < numPairs; ++i) {
			if (passVertexArrayPairs[cachedIndex].first == id) {
				break;
			}
			cachedIndex = (cachedIndex+1)%numPairs;
		}
		if (i == numPairs) {
			GLuint layout = rc->CreateVAO(binding, attributes, pass->GetInputSemantics());
			cachedIndex = numPairs;
			passVertexArrayPairs.push_back(PassLayoutVertexArrayPair(id , layout));
		}

		rc->EnableVertexArrayObject(passVertexArrayPairs[cachedIndex].second);
	}

	void VertexLayoutStaticGL::Disable(RenderContextGL* rc) {
		rc->DisableVertexArrayObject();
	}

	/************************ VertexLayoutDynamicGL ************************/
	VertexLayoutDynamicGL::VertexLayoutDynamicGL() : numberOfStoredPassMapping(0), cached(0),
		VertexLayoutGL() {
	}

	VertexLayoutDynamicGL::~VertexLayoutDynamicGL() {
	}

	void VertexLayoutDynamicGL::Enable(VertexBufferBinding& binding, PassGL* pass, RenderContextGL* rc) {

		uint16 id = pass->GetInputLayoutID();

		MappedVertexAttribList* selected = nullptr;
		// todo cache??
		if (!cached || cached->passId != id) {
			uint16* current = indices.data();
			uint16 i = 0;
			for(; i < numberOfStoredPassMapping; ++i) {
				selected = reinterpret_cast<MappedVertexAttribList*>(current);
				if (selected->passId == id) {
					cached = selected;
					break;
				}
				current = current + 2 + selected->numAttributes*2;
			}
			if (i == numberOfStoredPassMapping) {
				uint16 outElements[RenderConstants::MAX_VERTEX_ELEMENT];
				const VertexSemanticListGL& semantics = pass->GetInputSemantics();
				uint32 numMapped = BufferManager::MapSignatureToSemantics(
					&semantics[0].semantic, sizeof(VertexSemanticGL), semantics.size(),
					&attributes[0].element, sizeof(VertexAttribGL), attributes.size(),
					outElements);
				if (numMapped) {
					numberOfStoredPassMapping++;
					indices.resize(indices.size() + 2 + numMapped*2);
					cached = reinterpret_cast<MappedVertexAttribList*>((&indices.back())  - 2 - numMapped*2);
					cached->numAttributes = numMapped;
					cached->passId = id;
					for(uint32 j = 0; j < numMapped; ++j) {
						NEX_ASSERT(semantics[j].index < (uint16)-1);
						cached->attribs[j].attribIndex = outElements[j];
						cached->attribs[j].index = (uint16)semantics[j].index;
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
		for(uint16 s = 0; s < numAttributes; ++s) {
			MappedVertexAttrib& mva = cached->attribs[s];
			VertexAttribGL& vegl = attributes[mva.attribIndex];
			if( vegl.element.streamIndex != stream ) {
				stream = vegl.element.streamIndex;
				rc->SetVertexBuffer(binding.GetBuffer(stream));
			}
			rc->EnableVertexAttribute(mva.index, vegl);
		}
	}


	void VertexLayoutDynamicGL::Disable(RenderContextGL* rc) {
		NEX_ASSERT(cached);
		uint16 numAttributes = cached->numAttributes;
		for(uint16 s = 0; s < numAttributes; ++s) {
			MappedVertexAttrib& mva = cached->attribs[s];
			rc->DisableVertexAttribute(mva.index);
		}
	}
}

