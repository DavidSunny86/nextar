#ifndef NEXTAR_DATASTREAM_H
#define NEXTAR_DATASTREAM_H

#include <NexBase.h>

namespace nextar {

/**
 * @class	InputStream
 *
 * @brief	Generic input data stream.
 *
 * @author	Abhishek Dey
 * @date	11/28/2009
 **/
class _NexBaseAPI InputStream: public Referenced<InputStream, AllocGeneral> {
protected:
	size_t size;
public:

	inline InputStream() :
			size(0) {
	}

	inline void Rewind() {
		Seek(0, std::ios_base::beg);
	}

	/** Reads, throws EXCEPT_READ_ERROR on faliure. Returns the actual amount of bytes read. **/
	virtual size_t Read(void* buffer, size_t size) = 0;
	/** Gets the full size of this stream. */
	virtual size_t GetSize() const;
	/** Changes the position of read pointer. */
	virtual bool Seek(std::streamoff offset, std::ios_base::seekdir dir =
			std::ios_base::cur) = 0;
	/** Tells the position of the marker **/
	virtual std::streamoff Tell() = 0;
	/**	Query if the end of file is reached. **/
	virtual bool IsEndOfStream() const = 0;
	/** Read into a local read only buffer, returns amount of bytes that was read
	 * and a pointer to the buffer allocated. The buffer is read only. This call
	 * will alter the stream position and seek it to end */
	virtual size_t AcquireBuffer(const void*& readOnlyData);
	/** Free the acquired read only buffer  */
	virtual void ReleaseBuffer(const void* readOnlyData);
};

class _NexBaseAPI OutputStream: public Referenced<OutputStream, AllocGeneral> {
protected:
	size_t size;
public:

	inline OutputStream() :
			size(0) {
	}

	/** Gets the current size of this stream.**/
	virtual size_t GetSize() const;
	/** Writes data to stream, throws EXCEPT_WRITE_ERROR on faliure. Returns
	 * actual amount of bytes written. **/
	virtual void Write(const void* buffer, size_t size) = 0;
	/** Changes the position of read pointer. **/
	virtual bool Seek(std::streamoff offset, std::ios_base::seekdir dir =
			std::ios_base::cur) = 0;
	/** Returns the position of the marker. **/
	virtual std::streamoff Tell() = 0;
};

}

#endif //NEXTAR_DATASTREAM_H