
#include "NexEngine.h"
#include "VertexElement.h"

namespace nextar {

    SemanticDef::SemanticMap SemanticDef::semanticMap;

	String VertexSemantic::ToString() {
		OutStringStream buff;
		buff << semantic << "." << semanticIndex << "." << type << "."
				<< (instanceData ? ".1;" : ".0;");
		return buff.str();
	}
    
    VertexComponentBaseType VertexSemantic::GetComponentBaseType(VertexComponentType type) {
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
        AddSemantic("vertPos", SemanticDef(COMP_POSITION, 0));
        AddSemantic("vert2DPos", SemanticDef(COMP_2D_POSITION, 0));
        AddSemantic("vertNorm", SemanticDef(COMP_NORMAL, 0));
        AddSemantic("vertBiNorm", SemanticDef(COMP_BINORMAL, 0));
        AddSemantic("vertTangent", SemanticDef(COMP_TANGENT, 0));
        AddSemantic("vertColor", SemanticDef(COMP_COLOR, 0));
        AddSemantic("vertSecColor", SemanticDef(COMP_COLOR, 1));
        AddSemantic("texCoord0", SemanticDef(COMP_TEXTURE_COORDINATE, 0));
        AddSemantic("texCoord1", SemanticDef(COMP_TEXTURE_COORDINATE, 1));
        AddSemantic("texCoord2", SemanticDef(COMP_TEXTURE_COORDINATE, 2));
        AddSemantic("texCoord3", SemanticDef(COMP_TEXTURE_COORDINATE, 3));
        AddSemantic("texCoord4", SemanticDef(COMP_TEXTURE_COORDINATE, 4));
        AddSemantic("texCoord5", SemanticDef(COMP_TEXTURE_COORDINATE, 5));
        AddSemantic("texCoord6", SemanticDef(COMP_TEXTURE_COORDINATE, 6));
        AddSemantic("texCoord7", SemanticDef(COMP_TEXTURE_COORDINATE, 7));
    }

    void VertexSemantic::ClearSemanticMap() {
        SemanticDef::SemanticMap empty;
        SemanticDef::semanticMap.swap(empty);
    }

    void VertexSemantic::AddSemantic(const char* name, SemanticDef sem) {
        SemanticDef::semanticMap.insert(SemanticDef::SemanticMap::value_type(name, sem));
    }

    SemanticDef VertexSemantic::MapSemantic(const String& name) {
        SemanticDef::SemanticMap::iterator it = SemanticDef::semanticMap.find(name);
        if (it != SemanticDef::semanticMap.end()) {
            return (*it).second;
        }
        return SemanticDef(COMP_RESERVED_COUNT, -1);
    }
}

