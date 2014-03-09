
#include "NexEngine.h"
#include "StreamData.h"
#include "RenderConstants.h"

namespace nextar {

	/** 
	 * 
	 */
	StreamData::StreamData() : usesOnlyStaticVb(false),
		useIndices(false), isGsdataValid(false), type(PT_TRI_LIST), instanceCount(0), gsDataIndex(-1) {
	}

	StreamData::~StreamData() {
	}

	VertexBufferBinding* VertexBufferBinding::CreateInstance() {
		return NEX_NEW VertexBufferBinding();
	}
}