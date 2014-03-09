#include "BaseHeaders.h"
#include "GenericStreams.h"

namespace nextar {

//--------------------------------------------------------

	MemoryInputStream::MemoryInputStream() :
			data(0), readLeft(0), freedata(false) {
	}

	MemoryInputStream::MemoryInputStream(void *udata, size_t usize,
			bool freedataptr) :
			data(0), readLeft(usize) {
		LoadMemory(udata, usize, freedataptr);
	}

	MemoryInputStream::MemoryInputStream(InputStream* cpy) :
			data(0), readLeft(0), freedata(true) {
		size = cpy->GetSize();
		readLeft = size;
		size_t pos = static_cast<size_t>(cpy->Tell());
		cpy->Seek(0, std::ios_base::beg);
		data = (uint8*) NEX_ALLOC(size, MEMCAT_GENERAL);
		cpy->Read(data, size);
		cpy->Seek(pos);
	}

	MemoryInputStream::~MemoryInputStream() {
		Clear();
	}

	void MemoryInputStream::Clear() {
		if (data && freedata)
			NEX_FREE(data, MEMCAT_GENERAL);
		readLeft = 0;
		data = 0;
	}

	void MemoryInputStream::LoadMemory(void *udata, size_t usize,
			bool freedataptr) {
		Clear();
		data = static_cast<uint8*>(udata);
		readLeft = size = usize;
		freedata = freedataptr;
	}

	size_t MemoryInputStream::Read(void* buff, size_t s) {
		size_t amt = std::min(s, readLeft);
		if (amt) {
			std::memcpy(buff, data + (size-readLeft), amt);
			readLeft -= amt;
		}
		return amt;
	}

	bool MemoryInputStream::Seek(std::streamoff offset,
			std::ios_base::seekdir dir) {
		NEX_ASSERT(data);
		switch (dir) {
		case std::ios_base::beg:
			NEX_ASSERT(offset >= 0);
			readLeft = std::max<int32>((int32) size - (int32) offset, 0);
			break;
		case std::ios_base::cur:
			readLeft = std::max<int32>((int32) readLeft - (int32) offset, 0);
			break;
		case std::ios_base::end:
			readLeft =
					(offset < 0) ?
							std::max<int32>((int32) size + (int32) offset, 0) : 0;
			break;
		}
		return true;
	}

	std::streamoff MemoryInputStream::Tell() {
		NEX_ASSERT(data);
		return static_cast<std::streamoff>(size - readLeft);
	}

	bool MemoryInputStream::IsEndOfStream() const {
		return readLeft == 0;
	}

	size_t MemoryInputStream::AcquireBuffer(const void*& readOnlyData) {
		readOnlyData = data + (size-readLeft);
		size_t bytesRead = readLeft;
		readLeft = 0;
		return bytesRead;
	}

	void MemoryInputStream::ReleaseBuffer(const void*) {
	}
//--------------------------------------------------------
	MemoryOutputStream::MemoryOutputStream() :
			pos(0) {
	}

	MemoryOutputStream::~MemoryOutputStream() {
	}

	const uint8* MemoryOutputStream::GetData() const {
		return buffer.data();
	}

	void MemoryOutputStream::Write(const void* buff, size_t s) {
		buffer.reserve(pos + s);
		void* dst = buffer.data() + pos;
		std::memcpy(dst, buff, s);
		size = std::max(pos + s, size);
	}

	bool MemoryOutputStream::Seek(std::streamoff offset,
			std::ios_base::seekdir dir) {
		switch (dir) {
		case std::ios_base::beg:
			NEX_ASSERT(offset >= 0);
			pos = std::min<int32>((int32) offset, (int32) size);
			break;
		case std::ios_base::cur:
			pos = std::max<int32>(
					(int32) std::min<int32>((int32) pos + (int32) offset, (int32) size),
					0);
			break;
		case std::ios_base::end:
			pos = (offset < 0) ?
					std::max<int32>((int32) size + (int32) offset, 0) : size;
			break;
		}
		return true;
	}

	std::streamoff MemoryOutputStream::Tell() {
		return static_cast<std::streamoff>(pos);
	}

//--------------------------------------------------------
	GenericInputStream::GenericInputStream(std::istream* stream,
			size_t streamSize, bool bfree) :
			streamptr(stream), freedata(bfree) {
		NEX_ASSERT(streamptr);
		if (streamSize == CONST_INVALID_SIZE || !streamSize) {
			std::streamoff off = stream->tellg();
			stream->seekg(0, std::ios_base::end);
			streamSize = static_cast<size_t>(stream->tellg());
			stream->seekg(off, std::ios_base::beg);
		}
		size = streamSize;
	}

	GenericInputStream::~GenericInputStream() {
		if (streamptr && freedata) {
			NEX_FREE_T(std::istream, MEMCAT_GENERAL, streamptr);
			streamptr = 0;
		}
	}

	size_t GenericInputStream::Read(void* buffer, size_t strmSize) {
		try {
			streamptr->read(static_cast<char*>(buffer),
					static_cast<std::streamsize>(strmSize));
		} catch (std::exception&) {
			NEX_THROW_GracefulError(EXCEPT_READ_ERROR);
		}

		return static_cast<size_t>(streamptr->gcount());
	}

	bool GenericInputStream::Seek(std::streamoff offset,
			std::ios_base::seekdir dir) {
		streamptr->seekg(static_cast<std::streamoff>(offset), dir);
		return true;
	}

	std::streamoff GenericInputStream::Tell() {
		return streamptr->tellg();
	}

	bool GenericInputStream::IsEndOfStream() const {
		return streamptr->eof();
	}

//--------------------------------------------------------
	GenericOutputStream::GenericOutputStream(std::ostream* strm,
			size_t streamSize, bool bfreeStream) :
			streamptr(strm), freedata(bfreeStream) {
		NEX_ASSERT(streamptr);
		if (streamSize == CONST_INVALID_SIZE) {
			std::streamoff off = streamptr->tellp();
			streamptr->seekp(0, std::ios_base::end);
			streamSize = static_cast<size_t>(streamptr->tellp());
			streamptr->seekp(off, std::ios_base::beg);
		}
		size = streamSize;
	}

	GenericOutputStream::~GenericOutputStream() {
		if (streamptr && freedata) {
			NEX_FREE_T(std::ostream, MEMCAT_GENERAL, streamptr);
			streamptr = 0;
		}
	}

	void GenericOutputStream::Write(const void* buffer, size_t count) {
		try {
			streamptr->write(static_cast<const char*>(buffer),
					static_cast<std::streamsize>(count));
		} catch (std::exception&) {
			NEX_THROW_GracefulError(EXCEPT_WRITE_ERROR);
		}
		size += count;
	}

	bool GenericOutputStream::Seek(std::streamoff offset,
			std::ios_base::seekdir dir) {
		streamptr->seekp(static_cast<std::streamoff>(offset), dir);
		return true;
	}

	std::streamoff GenericOutputStream::Tell() {
		return streamptr->tellp();
	}

//--------------------------------------------------------
	FileInputStream::FileInputStream(std::FILE* stream, size_t streamSize) :
			streamptr(stream) {
		NEX_ASSERT(streamptr);
		if (streamSize == CONST_INVALID_SIZE || !streamSize) {
			std::streamoff off = std::ftell(stream);
			std::fseek(streamptr, 0, SEEK_END);
			streamSize = static_cast<size_t>(std::ftell(stream));
			std::fseek(streamptr, 0, SEEK_SET);
		}
		size = streamSize;
	}

	FileInputStream::~FileInputStream() {
		if (streamptr) {
			std::fclose(streamptr);
			streamptr = 0;
		}
	}

	size_t FileInputStream::Read(void* buffer, size_t strmSize) {
		return std::fread(static_cast<char*>(buffer), 1, (strmSize), streamptr);
	}

	bool FileInputStream::Seek(std::streamoff offset,
			std::ios_base::seekdir dir) {
		return std::fseek(streamptr, (long) offset, dir) == 0;
	}

	std::streamoff FileInputStream::Tell() {
		return std::ftell(streamptr);
	}

	bool FileInputStream::IsEndOfStream() const {
		return std::feof(streamptr) != 0;
	}
//--------------------------------------------------------
	FileOutputStream::FileOutputStream(std::FILE* strm, size_t streamSize) :
			streamptr(strm) {
		NEX_ASSERT(streamptr);
		if (streamSize == CONST_INVALID_SIZE) {
			std::streamoff off = std::ftell(streamptr);
			std::fseek(streamptr, 0, std::ios_base::end);
			streamSize = std::ftell(streamptr);
			std::fseek(streamptr, (long) off, std::ios_base::beg);
		}
		size = streamSize;
	}

	FileOutputStream::~FileOutputStream() {
		if (streamptr) {
			std::fclose(streamptr);
		}
	}

	void FileOutputStream::Write(const void* buffer, size_t count) {

		if (std::fwrite(static_cast<const char*>(buffer), 1,
				static_cast<std::streamsize>(count), streamptr) != count)
			NEX_THROW_GracefulError(EXCEPT_WRITE_ERROR);
		size += count;
	}

	bool FileOutputStream::Seek(std::streamoff offset,
			std::ios_base::seekdir dir) {
		return std::fseek(streamptr, (long) offset, dir) == 0;
	}

	std::streamoff FileOutputStream::Tell() {
		return std::ftell(streamptr);
	}
}

