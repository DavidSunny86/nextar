#include <NexEngine.h>
#include <StreamData.h>
#include <RenderConstants.h>

namespace nextar {

/** 
 * 
 */
StreamData::StreamData() :
		flags(0),
		type(PT_TRI_LIST),
		instanceCount(1) {
}

StreamData::~StreamData() {
	if ((flags & DELETE_BINDING) && vertices.binding)
		NEX_DELETE(vertices.binding);
	if ((flags & DELETE_LAYOUT) && vertices.layout)
		NEX_DELETE(vertices.layout);
}

VertexBufferBinding::VertexBufferBinding() :
bindingNumber(0), transientBufferCount(0), listOfBuffers(fixedBuffer), numBuffers(0) {
}

}
