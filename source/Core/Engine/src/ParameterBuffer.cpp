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
			size(0), data(nullptr), paramTable(0) {
	}

	ParameterBuffer::~ParameterBuffer() {
	}

	void ParameterBuffer::Prepare(ParamEntryTableItem* table) {
		paramTable = table;
		auto it = table->first;
		size_t size = 0;
		while(it) {
			size += (*it).descriptor->size;
			++it;
		}
		data = (uint8*)NEX_ALLOC(size, MEMCAT_GENERAL);
	}

	void ParameterBuffer::Prepare(void* data, size_t size) {
		this->size = size;
		this->data.reset((uint8*)NEX_ALLOC(size, MEMCAT_GENERAL));
		std::memcpy(this->data.get(), data, size);
	}

	void ParameterBuffer::Prepare(DataPtr&& data, size_t size) {
		this->size = size;
		this->data = std::move(data);
	}

	const ParamEntry* ParameterBuffer::_GetParameter(const String& name) const {
		auto it = paramTable->first;
		while(it != paramTable->second) {
			if((*it).descriptor->name == name)
				return &(*it);
		}
		return nullptr;
	}
} /* namespace nextar */
