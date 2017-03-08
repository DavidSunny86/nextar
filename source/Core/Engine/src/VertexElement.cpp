#include <NexEngine.h>
#include <VertexElement.h>

namespace nextar {

SemanticDef::SemanticMap SemanticDef::semanticMap;

String VertexSemantic::ToString() {
	OutStringStream buff;
	buff << semantic << "." << semanticIndex << "." << type << "."
			<< (instanceData ? ".1;" : ".0;");
	return buff.str();
}

VertexComponentBaseType VertexSemantic::GetComponentBaseType(
		VertexComponentType type) {
	switch (type) {
	case COMP_TYPE_FLOAT1:
	case COMP_TYPE_FLOAT2:
	case COMP_TYPE_FLOAT3:
	case COMP_TYPE_FLOAT4:
		return COMP_BASE_TYPE_FLOAT;
	case COMP_TYPE_COLOR:
	case COMP_TYPE_UNSIGNED_INT:
		return COMP_BASE_TYPE_UNSIGNED_INT;
	}
	return COMP_BASE_TYPE_INVALID;
}

void VertexSemantic::BuildSemanticMap() {
	/* Custom pre-declared names */
	AddSemantic("_vPos", SemanticDef(COMP_POSITION, 0));
	AddSemantic("_v2DPos", SemanticDef(COMP_2D_POSITION, 0));
	AddSemantic("_vNormal", SemanticDef(COMP_NORMAL, 0));
	AddSemantic("_vBinormal", SemanticDef(COMP_BINORMAL, 0));
	AddSemantic("_vTangent", SemanticDef(COMP_TANGENT, 0));
	AddSemantic("_vColor", SemanticDef(COMP_COLOR, 0));
	AddSemantic("_vTexCoord0", SemanticDef(COMP_TEXTURE_COORDINATE, 0));
	AddSemantic("_vTexCoord1", SemanticDef(COMP_TEXTURE_COORDINATE, 1));
	AddSemantic("_vTexCoord2", SemanticDef(COMP_TEXTURE_COORDINATE, 2));
	AddSemantic("_vTexCoord3", SemanticDef(COMP_TEXTURE_COORDINATE, 3));
	AddSemantic("_vTexCoord4", SemanticDef(COMP_TEXTURE_COORDINATE, 4));
	AddSemantic("_vTexCoord5", SemanticDef(COMP_TEXTURE_COORDINATE, 5));
	AddSemantic("_vTexCoord6", SemanticDef(COMP_TEXTURE_COORDINATE, 6));
	AddSemantic("_vTexCoord7", SemanticDef(COMP_TEXTURE_COORDINATE, 7));
}

void VertexSemantic::ClearSemanticMap() {
	SemanticDef::SemanticMap empty;
	SemanticDef::semanticMap.swap(empty);
}

void VertexSemantic::AddSemantic(const char* name, SemanticDef sem) {
	SemanticDef::semanticMap.insert(
			SemanticDef::SemanticMap::value_type(name, sem));
}

SemanticDef VertexSemantic::MapSemantic(const String& name) {
	SemanticDef::SemanticMap::iterator it = SemanticDef::semanticMap.find(name);
	if (it != SemanticDef::semanticMap.end()) {
		return (*it).second;
	}
	return SemanticDef(COMP_RESERVED_COUNT, -1);
}

uint32 VertexSemantic::MapSignatureToSemantics(const VertexSemantic* signature,
	uint32 semanticStride, uint32 numSemantics,
	const VertexElement* elements, uint32 elementsStride,
		uint32 numElements, uint16* outElements) {
	uint32 numMapped = 0;
	for (uint32 i = 0; i < numSemantics; ++i) {
		const VertexSemantic& vs =
				*(reinterpret_cast<const VertexSemantic*>(((const uint8*) signature)
						+ i * semanticStride));
		uint32 lastMap = numMapped;
		for (uint32 j = 0; j < numElements; ++j) {
			const VertexElement& element =
					*(reinterpret_cast<const VertexElement*>(((const uint8*) elements)
							+ j * elementsStride));
			if (element.desc.semantic == vs) {
				outElements[numMapped++] = (uint16) j;
				break;
			}
		}
		if (lastMap == numMapped) {
			// an attribute was not mapped
			return 0;
		}
	}
	return numMapped;
}

}

