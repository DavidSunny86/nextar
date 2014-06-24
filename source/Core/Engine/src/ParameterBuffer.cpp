/*
 * PropertyBuffer.cpp
 *
 *  Created on: 12-Mar-2014
 *      Author: obhi
 */
#include <NexEngine.h>
#include <ParameterBuffer.h>

namespace nextar {

ParameterBuffer::ParameterBuffer() :
		size(0), data(nullptr) {
}

ParameterBuffer::ParameterBuffer(const ParameterBuffer& pb) {
	*this = pb;
}

ParameterBuffer::ParameterBuffer(ParameterBuffer&& pb) {
	*this = std::move(pb);
}

ParameterBuffer::~ParameterBuffer() {
}

void ParameterBuffer::Prepare(const ParamEntryTableItem& table) {
	paramTable = table;
	data.reset((uint8*) NEX_ALLOC(table.totalParamBufferSize, MEMCAT_GENERAL));
}

void ParameterBuffer::Prepare(void* data, size_t size) {
	this->size = size;
	this->data.reset((uint8*) NEX_ALLOC(size, MEMCAT_GENERAL));
	if (data)
		std::memcpy(this->data.get(), data, size);
}

void ParameterBuffer::Prepare(DataPtr&& data, size_t size) {
	this->size = size;
	this->data = std::move(data);
}

const ParamEntry* ParameterBuffer::_GetParameter(const String& name) const {
	auto it = paramTable.beginIt;
	while (it != paramTable.endIt) {
		if ((*it).name == name)
			return &(*it);
	}
	return nullptr;
}

void ParameterBuffer::SetData(const void* data, size_t offset, size_t size) {
	if (data && this->data)
		std::memcpy(this->data.get() + offset, data, size);
}

ParameterBuffer& ParameterBuffer::operator=(const ParameterBuffer& pb) {
	size = pb.size;
	paramTable = pb.paramTable;
	this->data.reset((uint8*) NEX_ALLOC(size, MEMCAT_GENERAL));
	if (pb.data)
		std::memcpy(this->data.get(), pb.data.get(), size);
	return *this;
}

ParameterBuffer& ParameterBuffer::operator=(ParameterBuffer&& pb) {
	size = pb.size;
	paramTable = std::move(pb.paramTable);
	data = std::move(pb.data);
	return *this;
}

} /* namespace nextar */
