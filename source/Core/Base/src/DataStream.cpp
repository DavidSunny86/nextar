/*
 * DataStream.cpp
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */

#include <BaseHeaders.h>
#include <DataStream.h>

namespace nextar {

	size_t InputStream::GetSize() const {
		return size;
	}

	size_t InputStream::AcquireBuffer(const void*& readOnlyData) {
		size_t streamSize = GetSize();
		readOnlyData = NEX_ALLOC(streamSize, MEMCAT_GENERAL);
		return Read(const_cast<void*>(readOnlyData), streamSize);
	}

	void InputStream::ReleaseBuffer(const void* readOnlyData) {
		NEX_FREE((const_cast<void*>(readOnlyData)), MEMCAT_GENERAL);
	}

	size_t OutputStream::GetSize() const {
		return size;
	}

}


