#ifndef NEXTAR_VERTEX_ELEMENT_H
#define NEXTAR_VERTEX_ELEMENT_H

#include <NexBase.h>
#include <RenderConstants.h>

namespace nextar {

enum VertexComponentSemantic {
	/* Basic position component.  */
	COMP_POSITION,
	/* Transformed 2D position component, in normalized
	 * coordinates (+1,-1).  */
	COMP_2D_POSITION,
	/* Normals.  */
	COMP_NORMAL,
	/* BiNormals.  */
	COMP_BINORMAL,
	/* Tangent */
	COMP_TANGENT,
	/* Texture coordinates */
	COMP_TEXTURE_COORDINATE,
	/* Dword vertex index */
	COMP_VERTEX_INDEX,
	/* Dword vertex color component.  */
	COMP_COLOR,
	/* Position feedback component */
	COMP_FEEDBACK_POSITION,
	/* Normal */
	COMP_FEEDBACK_NORMAL,
	/* Predefined components count.  */
	COMP_RESERVED_COUNT,
};

enum VertexComponentBaseType {
	COMP_BASE_TYPE_FLOAT,
	COMP_BASE_TYPE_UNSIGNED_INT,
	COMP_BASE_TYPE_UNSIGNED_SHORT,
	COMP_BASE_TYPE_INVALID,
};

enum VertexComponentType {
	COMP_TYPE_FLOAT1,
	COMP_TYPE_FLOAT2,
	COMP_TYPE_FLOAT3,
	COMP_TYPE_FLOAT4,
	COMP_TYPE_UNSIGNED_INT,
	COMP_TYPE_COLOR,
	COMP_TYPE_INVALID
};

struct SemanticDef;
struct VertexSemantic;
struct VertexDesc;
struct VertexElement;
/**
 * Semantic definition provides specific information on how to interpret
 * a given semantic name. These informations are the @ComponentSemantic in the
 * vertex layout that this name identifies to, the index of the
 * @ComponentSemantic, if this semantic name is an instance specific data,
 * if this semantic name is an input or output semantic.
 */
struct SemanticDef {
	bool instanceData;
	/* eg: true if 'attribute in vec4 someVar;' in shader */
	bool inputSemantic;
	/* eg: 0,1, for COMP_TEXTURE_COORDINATE */
	uint8 semanticIndex;
	/* eg: COMP_TEXTURE_COORDINATE */
	uint16 semantic;
	//uint16 feedBackRemap;

	typedef map<String, SemanticDef>::type SemanticMap;
	static SemanticMap semanticMap;

	SemanticDef(uint16 sem, uint8 idx, bool input = true, bool inst = false,
			uint16 fbRem = COMP_RESERVED_COUNT) :
			semantic(sem), semanticIndex(idx) /*, feedBackRemap(fbRem) */{
		instanceData = inst;
		inputSemantic = input;
	}
};

struct _NexEngineAPI VertexSemantic: public AllocGraphics {
	/* Indicates if this is an instance/vertex data*/
	bool instanceData;
	/* Semantic index for elements with same semantic but diff indices
	 * eg: 0,1, for COMP_TEXTURE_COORDINATE */
	uint8 semanticIndex;
	/* Semantic that maps with the shader
	 * eg: COMP_TEXTURE_COORDINATE */
	uint16 semantic;
	/* Feedback semantic remap */
	//uint16 feedBackRemap;
	/* component type
	 * eg: COMP_TYPE_FLOAT1 */
	uint16 type;

	inline VertexSemantic() :
	semantic(-1), type(-1), semanticIndex(-1), instanceData(false) {
	}

	inline VertexSemantic(uint16 sem, uint8 semIndex, uint16 elementType,
			bool instData = false) :
	semantic(sem), semanticIndex(semIndex), type(elementType), instanceData(
			instData) /*, feedBackRemap(fbRemap) */{

	}

	friend bool operator ==(const VertexSemantic& v1,
			const VertexSemantic& v2) {
		// types may vary depending upon normalization is used

		return (v1.instanceData == v2.instanceData
				&& v1.semanticIndex == v2.semanticIndex
				&& v1.semantic == v2.semantic) != 0;
	}

	friend bool operator < (const VertexSemantic& v1,
			const VertexSemantic& v2) {
		if( v1.semantic == v2.semantic )
		return v1.semanticIndex < v2.semanticIndex;
		return v1.semantic < v2.semantic;
	}

	/* This convertes the semantic into a unique string representation,
	 * that can be used to match signatures.
	 * The chosen name format is:
	 * (semantic).(semanticIndex).(type).(instanceData);
	 * All included in brackets are numeric integers converted to string.
	 */
	String ToString();

	/* Called by RenderSys */
	static void BuildSemanticMap();

	/** @remarks
	 * This function can be used to add custom semantic delcarations.
	 * Custom semantics once thus added, can be mapped to any pre-def
	 * semantic of type ComponentSemantic, this mapping is needed by
	 * shaders to identify the vertex element name. The index variable
	 * can be used if the same pre-def semantic is to be used multiple
	 * times by the shader or vertex layout.
	 * @par
	 * This function can be called by the application. It is also
	 * called by the render system implementation to register
	 * render sys specific semantics.
	 * @par
	 * List of predefined mapping can be found in the documentation.
	 **/
	static void AddSemantic(const char* name, SemanticDef);

	/** Called by render system to map a name to a given semantic def.
	 * The name passed is a string semantic in the shader. This in turn 
	 * is mapped to the shader input via vertex layout using the semantic
	 * definition.
	 **/
	static SemanticDef MapSemantic(const String & name);

	/* Called by render system */
	static void ClearSemanticMap();

	static VertexComponentBaseType GetComponentBaseType(
			VertexComponentType);

	static size_t MapSignatureToSemantics(const VertexSemantic* signature,
			size_t semanticStride, uint32 numSemantics,
			const VertexElement* elements, size_t elementsStride,
			size_t numElements, uint16* outElements);
};

struct VertexDesc: public AllocGraphics {
	/* Basic vertex semantic */
	VertexSemantic semantic;
	/* Offset of this element within a single vertex of a vertex buffer */
	uint16 offset;

	inline VertexDesc() :
			offset(-1) {
	}

	inline VertexDesc(const VertexSemantic sem, uint16 _offset) :
			semantic(sem), offset(_offset) {
	}
};

/** @remarks
 * Specifies a single vertex element on a specific vertex buffer.
 * The layout is build upon this, and the vertex buffer should also
 * bind to the output via that same index (or bind point) as specified.
 */
struct VertexElement: public AllocGraphics {

	enum {
		MAX_VERTEX_ELEMENT = (uint32) RenderConstants::MAX_VERTEX_ELEMENT,
	};

	/* Vertex description */
	VertexDesc desc;
	/* No of instances to draw using this instance data before advancing
	 * to the next element.
	 */
	uint16 stepRate;
	/* Stream index of the vertex buffer in the vertex buffer array */
	uint16 streamIndex;

	inline VertexElement() :
			stepRate(0), streamIndex(-1) {
	}

	inline VertexElement(const VertexDesc vdesc, uint16 _stream_index,
			uint16 _step_rate = 0) :
			desc(vdesc), stepRate(_step_rate), streamIndex(_stream_index) {
	}

	// for std::sort
	inline bool operator <(const VertexElement& other) const {
		return streamIndex < other.streamIndex;
	}
};

/** VertexElementList can be passed to the buffer manager
 *  along with the technique to generate the stream binding.
 */
typedef vector<VertexElement>::type VertexElementList;

}

#endif //NEXTAR_VERTEX_ELEMENT_H

