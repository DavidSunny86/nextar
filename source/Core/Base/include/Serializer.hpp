/*
 * Serializer.hpp
 *
 *  Created on: 18-Aug-2013
 *      Author: obhi
 */

#ifndef SERIALIZER_HPP_
#define SERIALIZER_HPP_

#if NEX_NEED_ENDIAN_SWAPPING == 0
#define SWAP16(object) object
#define SWAP32(object) object
#define SWAP64(object) object
#define WRITE_ARRAY(object, T) Flush(); outStream->Write(object.first, object.second*sizeof(T)); totalSizeWritten += object.second*sizeof(T)
#define READ_ARRAY(object, T) { \
	std::streamoff readAmt = (object.second*sizeof(T)); \
	streamSize -= (readAmt-left); \
	NEX_ASSERT(streamSize >= 0); \
	if (left) { \
		inStream->Seek(-left, std::ios_base::cur); left = 0; \
	} \
	inStream->Read(object.first, (size_t)readAmt); }
#else
#define SWAP16(object) Endian::Swap16(object)
#define SWAP32(object) Endian::Swap32(object)
#define SWAP64(object) Endian::Swap64(object)
#define WRITE_ARRAY(object, T) for(size_t i = 0; i < object.second; ++i) (*this) << object.first[i]; totalSizeWritten += object.second*sizeof(T)
#define READ_ARRAY(object, T) for(size_t i = 0; i < object.second; ++i) (*this) >> object.first[i];
#endif

namespace nextar {

/**********************************************************
 * Output Serializer
 ***********************************************************/
inline OutputSerializer::OutputSerializer() : full(0), totalSizeWritten(0) {
}

inline OutputSerializer::OutputSerializer(const OutputSerializer& other) :
outStream(other.outStream), full(other.full), totalSizeWritten(other.totalSizeWritten) {
	std::memcpy(localBuffer, other.localBuffer, LOCAL_BUFFER_SIZE);
}

inline OutputSerializer::OutputSerializer(OutputSerializer&& other) :
outStream(other.outStream), full(other.full), totalSizeWritten(other.totalSizeWritten) {
	std::memcpy(localBuffer, other.localBuffer, LOCAL_BUFFER_SIZE);
	other.outStream.Clear();
}

inline OutputSerializer::OutputSerializer(OutputStreamPtr& _outStream) :
		outStream(_outStream), full(0), totalSizeWritten(0) {
}

inline OutputSerializer::~OutputSerializer() {
	Flush();
}

inline OutputSerializer& OutputSerializer::operator = (OutputSerializer&& other) {
	outStream = other.outStream;
	full = other.full;
	std::memcpy(localBuffer, other.localBuffer, LOCAL_BUFFER_SIZE);
	other.outStream.Clear();
	return *this;
}

inline OutputSerializer& OutputSerializer::operator <<(
		const SerializableObject& object) {
	(*this) << (uint8) (MARKER_OBJECT);
	(*this) << (uint32) object.GetSerializableVersion();
	object.WriteObject(*this);
	return *this;
}

inline OutputSerializer& OutputSerializer::operator <<(bool object) {
	return (*this << *reinterpret_cast<uint8*>(&object));
}

inline OutputSerializer& OutputSerializer::operator <<(int8 object) {
	return (*this << *reinterpret_cast<uint8*>(&object));
}

inline OutputSerializer& OutputSerializer::operator <<(int16 object) {
	return (*this << *reinterpret_cast<uint16*>(&object));
}

inline OutputSerializer& OutputSerializer::operator <<(int32 object) {
	return (*this << *reinterpret_cast<uint32*>(&object));
}

inline OutputSerializer& OutputSerializer::operator <<(int64 object) {
	return (*this << *reinterpret_cast<uint64*>(&object));
}

inline OutputSerializer& OutputSerializer::operator <<(float object) {
	return (*this << *reinterpret_cast<uint32*>(&object));
}

inline OutputSerializer& OutputSerializer::operator <<(double object) {
	return (*this << *reinterpret_cast<uint64*>(&object));
}

inline OutputSerializer& OutputSerializer::operator <<(uint8 object) {
	if ((LOCAL_BUFFER_SIZE - full) < 1)
		Flush();
	localBuffer[full++] = object;
	return *this;
}

inline OutputSerializer& OutputSerializer::operator <<(uint16 object) {
	if ((LOCAL_BUFFER_SIZE - full) < 2)
		Flush();
	*reinterpret_cast<uint16*>(localBuffer + full) = SWAP16(object);
	full += 2;
	return *this;
}

inline OutputSerializer& OutputSerializer::operator <<(uint32 object) {
	if ((LOCAL_BUFFER_SIZE - full) < 4)
		Flush();
	*reinterpret_cast<uint32*>(localBuffer + full) = SWAP32(object);
	full += 4;
	return *this;
}

inline OutputSerializer& OutputSerializer::operator <<(uint64 object) {
	if ((LOCAL_BUFFER_SIZE - full) < 8)
		Flush();
	*reinterpret_cast<uint64*>(localBuffer + full) = SWAP64(object);
	full += 8;
	return *this;
}

inline OutputSerializer& OutputSerializer::operator <<(
		const ByteArray& object) {
	Flush();
	outStream->Write(object.first, object.second);
	totalSizeWritten += object.second;
	return *this;
}

inline OutputSerializer& OutputSerializer::operator <<(
		const UByteArray& object) {
	Flush();
	outStream->Write(object.first, object.second);
	totalSizeWritten += object.second;
	return *this;
}

inline OutputSerializer& OutputSerializer::operator <<(
		const ShortArray& object) {
	WRITE_ARRAY(object, uint16);
	return *this;
}

inline OutputSerializer& OutputSerializer::operator <<(
		const UShortArray& object) {
	WRITE_ARRAY(object, uint16);
	return *this;
}

inline OutputSerializer& OutputSerializer::operator <<(const IntArray& object) {
	WRITE_ARRAY(object, uint32);
	return *this;
}

inline OutputSerializer& OutputSerializer::operator <<(
		const UIntArray& object) {
	WRITE_ARRAY(object, uint32);
	return *this;
}

inline OutputSerializer& OutputSerializer::operator <<(
		const Int64Array& object) {
	WRITE_ARRAY(object, uint64);
	return *this;
}

inline OutputSerializer& OutputSerializer::operator <<(
		const UInt64Array& object) {
	WRITE_ARRAY(object, uint64);
	return *this;
}

inline OutputSerializer& OutputSerializer::operator <<(
		const FloatArray& object) {
	WRITE_ARRAY(object, float);
	return *this;
}

inline OutputSerializer& OutputSerializer::operator <<(
		const DoubleArray& object) {
	WRITE_ARRAY(object, double);
	return *this;
}

inline OutputSerializer& OutputSerializer::operator <<(
		const ChunkHeader& object) {
	(*this) << (uint16) MARKER_GLOBAL_CHUNK_ID << object.first << object.second;
	return *this;
}

inline OutputSerializer& OutputSerializer::operator <<(const String& object) {
	ByteArray arr(reinterpret_cast<const int8*>(object.c_str()),
			(uint32)object.length());
	(*this) << (uint8) MARKER_STRING_UTF8 << arr.second << arr;
	return *this;
}

inline OutputSerializer& OutputSerializer::operator <<(
		const UniString& object) {
	ShortArray arr(reinterpret_cast<const int16*>(object.c_str()),
			(uint32)object.length());
	(*this) << (uint8) MARKER_STRING_UTF16 << arr.second << arr;
	return *this;
}

inline OutputSerializer& OutputSerializer::operator <<(const StringID object) {
	return *this << object._value;
}

inline void OutputSerializer::Flush() {
	if (full) {
		totalSizeWritten += full;
		outStream->Write(localBuffer, full);
		full = 0;
	}
}

inline size_t OutputSerializer::GetTotalWrittenBytes() const {
	return full + totalSizeWritten;
}

/**********************************************************
 * Input Serializer
 ***********************************************************/
inline InputSerializer::InputSerializer(InputStreamPtr& _inStream) :
		inStream(_inStream), left(0), moving(localBuffer) {
	streamSize = inStream->GetSize();
}

inline InputSerializer::~InputSerializer() {
}

inline InputSerializer& InputSerializer::operator >>(
		SerializableObject& object) {
	uint8 marker;
	(*this) >> marker;
	if (marker == MARKER_OBJECT) {
		uint32 version;
		(*this) >> version;
		if (!TestVersion(version, object.GetSerializableVersion()))
			Warn("Version conflict for serializable object!");
		object.ReadObject((*this), version);
	}
	return *this;
}

inline InputSerializer& InputSerializer::operator >>(bool& object) {
	return (*this >> *reinterpret_cast<uint8*>(&object));
}

inline InputSerializer& InputSerializer::operator >>(int8& object) {
	return (*this >> *reinterpret_cast<uint8*>(&object));
}

inline InputSerializer& InputSerializer::operator >>(int16& object) {
	return (*this >> *reinterpret_cast<uint16*>(&object));
}

inline InputSerializer& InputSerializer::operator >>(int32& object) {
	return (*this >> *reinterpret_cast<uint32*>(&object));
}

inline InputSerializer& InputSerializer::operator >>(int64& object) {
	return (*this >> *reinterpret_cast<uint64*>(&object));
}

inline InputSerializer& InputSerializer::operator >>(float& object) {
	return (*this >> *reinterpret_cast<uint32*>(&object));
}

inline InputSerializer& InputSerializer::operator >>(double& object) {
	return (*this >> *reinterpret_cast<uint64*>(&object));
}

inline InputSerializer& InputSerializer::operator >>(uint8& object) {
	if (left < 1)
		Fill();
	NEX_ASSERT(left >= 1);
	object = *(moving++);
	--left;
	return *this;
}

inline InputSerializer& InputSerializer::operator >>(uint16& object) {
	if (left < 2)
		Fill();
	NEX_ASSERT(left >= 2);
	object = SWAP16(*reinterpret_cast<uint16*>(moving));
	moving += 2;
	left -= 2;
	return *this;
}

inline InputSerializer& InputSerializer::operator >>(uint32& object) {
	if (left < 4)
		Fill();
	NEX_ASSERT(left >= 4);
	object = SWAP32(*reinterpret_cast<uint32*>(moving));
	moving += 4;
	left -= 4;
	return *this;
}

inline InputSerializer& InputSerializer::operator >>(uint64& object) {
	if (left < 8)
		Fill();
	NEX_ASSERT(left >= 8);
	object = SWAP64(*reinterpret_cast<uint64*>(moving));
	moving += 8;
	left -= 8;
	return *this;
}

inline InputSerializer& InputSerializer::operator >>(ByteArray& object) {
	uint8* b = (uint8*) object.first;
	int32 r = (int32) object.second;
	if (left) {
		size_t c = std::min(r, left);
		std::memcpy(b, moving, c);
		moving += c;
		left -= (int32) c;
		r -= (int32) c;
		b += c;
	}

	if (r)
		inStream->Read(b, r);
	return *this;
}

inline InputSerializer& InputSerializer::operator >>(UByteArray& object) {
	(*this) >> *reinterpret_cast<ByteArray*>(&object);
	return *this;
}

inline InputSerializer& InputSerializer::operator >>(ShortArray& object) {
	READ_ARRAY(object, uint16);
	return *this;
}

inline InputSerializer& InputSerializer::operator >>(UShortArray& object) {
	READ_ARRAY(object, uint16);
	return *this;
}

inline InputSerializer& InputSerializer::operator >>(IntArray& object) {
	READ_ARRAY(object, uint32);
	return *this;
}

inline InputSerializer& InputSerializer::operator >>(UIntArray& object) {
	READ_ARRAY(object, uint32);
	return *this;
}

inline InputSerializer& InputSerializer::operator >>(Int64Array& object) {
	READ_ARRAY(object, uint64);
	return *this;
}

inline InputSerializer& InputSerializer::operator >>(UInt64Array& object) {
	READ_ARRAY(object, uint64);
	return *this;
}

inline InputSerializer& InputSerializer::operator >>(FloatArray& object) {
	READ_ARRAY(object, float);
	return *this;
}

inline InputSerializer& InputSerializer::operator >>(DoubleArray& object) {
	READ_ARRAY(object, double);
	return *this;
}

inline InputSerializer& InputSerializer::operator >>(StringID& object) {
	return (*this) >> object._value;
}

inline void InputSerializer::Fill() {
	if (left)
		inStream->Seek(-left, std::ios_base::cur);
	streamSize += left;
	size_t readAmt = std::min((size_t) LOCAL_BUFFER_SIZE, (size_t) streamSize);
	readAmt = inStream->Read(localBuffer, readAmt);
	left = (int32) readAmt;
	streamSize -=  readAmt;
	moving = localBuffer;
}

inline bool InputSerializer::IsEndOfStream() const {
	return !left && !streamSize;
}

inline bool InputSerializer::IsValid(const InputSerializer::Chunk& c) {
	return c.header.headerName != MARKER_INVALID_CHUNK;
}

inline void InputSerializer::Skip(Chunk& object) {
	// rewind and tell
	left = 0;
	if (inStream->Seek(object.startInFile + object.header.chunkSize, std::ios_base::beg))
		streamSize = inStream->GetSize()
				- (object.startInFile + object.header.chunkSize);
	else
		Error("Seeking stream!");
}

inline void InputSerializer::Seek(std::streamoff offset,
		std::ios_base::seekdir dir) {
	if (dir == std::ios_base::cur || dir == std::ios_base::end) {
		inStream->Seek(offset, dir);
	} else {
		inStream->Seek(offset - left, dir);
	}
	left = 0;
	streamSize = inStream->GetSize() - inStream->Tell();
}

inline std::streamoff InputSerializer::Tell() {
	return inStream->Tell() - left;
}

inline bool InputSerializer::IsEndianCorrected() {
	return NEX_NEED_ENDIAN_SWAPPING == 0;
}

}

#endif /* SERIALIZER_HPP_ */

