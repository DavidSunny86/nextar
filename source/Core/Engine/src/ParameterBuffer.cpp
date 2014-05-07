/*
 * PropertyBuffer.cpp
 *
 *  Created on: 12-Mar-2014
 *      Author: obhi
 */
#include <NexEngine.h>
#include <ParameterBuffer.h>
#include <ParameterIterator.h>

namespace nextar {

	ParameterBuffer::ParameterBuffer() :
			size(0), data(nullptr) {
	}

	ParameterBuffer::~ParameterBuffer() {
	}

	void ParameterBuffer::Prepare(ConstParamEntryTableItem& table) {
		paramTable = table;
		auto it = table.first;
		auto en = table.second;
		size_t size = 0;
		while(it != en) {
			size += (*it).descriptor->size;
			++it;
		}
		data.reset((uint8*)NEX_ALLOC(size, MEMCAT_GENERAL));
	}

	void ParameterBuffer::Prepare(void* data, size_t size) {
		this->size = size;
		this->data.reset((uint8*)NEX_ALLOC(size, MEMCAT_GENERAL));
		if (data)
			std::memcpy(this->data.get(), data, size);
	}

	void ParameterBuffer::Prepare(DataPtr&& data, size_t size) {
		this->size = size;
		this->data = std::move(data);
	}

	const ParamEntry* ParameterBuffer::_GetParameter(const String& name) const {
		auto it = paramTable.first;
		while(it != paramTable.second) {
			if((*it).descriptor->name == name)
				return &(*it);
		}
		return nullptr;
	}
		
	void ParameterBuffer::SetData(const void* data, size_t offset, size_t size) {
		if (data && this->data)
			std::memcpy(this->data.get() + offset, data, size);
	}

} /* namespace nextar */
