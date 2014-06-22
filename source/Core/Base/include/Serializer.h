#ifndef NEXTAR_SERIALIZER_H
#define NEXTAR_SERIALIZER_H

#include <NexBase.h>
#include <DataStream.h>
#include <Endian.h>
#include <Logger.h>

namespace nextar {
class OutputSerializer;
class InputSerializer;
class SerializableObject;

enum {
	MARKER_INVALID_CHUNK = (uint16) 0xffff,
	MARKER_GLOBAL_CHUNK_ID = 0xc437,
	MARKER_STRING_UTF16 = 0x16,
	MARKER_STRING_UTF8 = 0x08,
	MARKER_OBJECT = 0x0b,
};

/** @remarks Classes having serialization support
 *			 should derive from this class
 **/
class SerializableObject {
public:

	virtual ~SerializableObject() {
	}

	/** @remarks Get serializable version **/
	virtual VersionID GetSerializableVersion() const = 0;

	/** @remarks This function is called by the serializer.
	 *          It should implement the basic save via
	 *          serializer.
	 **/
	virtual void WriteObject(OutputSerializer& serializer) const = 0;

	/** @remarks This function is called by the serializer.
	 *           It should implement the basic read via
	 *           serializer.
	 **/
	virtual void ReadObject(InputSerializer& serializer, VersionID) = 0;
};

/** @remarks Output serialization
 * data is always in little endian format (unless configured otherwise) **/
class _NexBaseAPI OutputSerializer: public AllocGeneral {

public:

	typedef std::pair<const int8*, size_t> ByteArray;
	typedef std::pair<const uint8*, size_t> UByteArray;
	typedef std::pair<const int16*, size_t> ShortArray;
	typedef std::pair<const uint16*, size_t> UShortArray;
	typedef std::pair<const int32*, size_t> IntArray;
	typedef std::pair<const uint32*, size_t> UIntArray;
	typedef std::pair<const int64*, size_t> Int64Array;
	typedef std::pair<const uint64*, size_t> UInt64Array;
	typedef std::pair<const float*, size_t> FloatArray;
	typedef std::pair<const double*, size_t> DoubleArray;

	typedef std::pair<uint16, size_t> ChunkHeader;

	inline OutputSerializer(OutputStreamPtr& _outStream);
	inline ~OutputSerializer();

	inline OutputSerializer& operator <<(const SerializableObject& object);
	inline OutputSerializer& operator <<(bool object);
	inline OutputSerializer& operator <<(int8 object);
	inline OutputSerializer& operator <<(int16 object);
	inline OutputSerializer& operator <<(int32 object);
	inline OutputSerializer& operator <<(int64 object);
	inline OutputSerializer& operator <<(uint8 object);
	inline OutputSerializer& operator <<(uint16 object);
	inline OutputSerializer& operator <<(uint32 object);
	inline OutputSerializer& operator <<(uint64 object);
	inline OutputSerializer& operator <<(float object);
	inline OutputSerializer& operator <<(double object);

	inline OutputSerializer& operator <<(const ByteArray& object);
	inline OutputSerializer& operator <<(const UByteArray& object);
	inline OutputSerializer& operator <<(const ShortArray& object);
	inline OutputSerializer& operator <<(const UShortArray& object);
	inline OutputSerializer& operator <<(const IntArray& object);
	inline OutputSerializer& operator <<(const UIntArray& object);
	inline OutputSerializer& operator <<(const Int64Array& object);
	inline OutputSerializer& operator <<(const UInt64Array& object);
	inline OutputSerializer& operator <<(const FloatArray& object);
	inline OutputSerializer& operator <<(const DoubleArray& object);
	inline OutputSerializer& operator <<(const ChunkHeader& object);

	inline OutputSerializer& operator <<(const String& object);
	inline OutputSerializer& operator <<(const UniString& object);

protected:

	inline void Flush();

	enum {
		LOCAL_BUFFER_SIZE = 64,
	};
	OutputStreamPtr outStream;
	uint8 localBuffer[LOCAL_BUFFER_SIZE];
	size_t full;

};

/** @remarks input serialization **/
class _NexBaseAPI InputSerializer: public AllocGeneral {

	NEX_LOG_HELPER(InputSerializer)
	;

public:

	typedef std::pair<int8*, size_t> ByteArray;
	typedef std::pair<uint8*, size_t> UByteArray;
	typedef std::pair<int16*, size_t> ShortArray;
	typedef std::pair<uint16*, size_t> UShortArray;
	typedef std::pair<int32*, size_t> IntArray;
	typedef std::pair<uint32*, size_t> UIntArray;
	typedef std::pair<int64*, size_t> Int64Array;
	typedef std::pair<uint64*, size_t> UInt64Array;
	typedef std::pair<float*, size_t> FloatArray;
	typedef std::pair<double*, size_t> DoubleArray;
	typedef std::pair<uint16, size_t> ChunkHeader;
	typedef std::pair<ChunkHeader, std::streamoff> Chunk;

	inline InputSerializer(InputStreamPtr& _inStream);
	inline ~InputSerializer();

	inline InputSerializer& operator >>(SerializableObject& object);
	inline InputSerializer& operator >>(bool& object);
	inline InputSerializer& operator >>(int8& object);
	inline InputSerializer& operator >>(int16& object);
	inline InputSerializer& operator >>(int32& object);
	inline InputSerializer& operator >>(int64& object);
	inline InputSerializer& operator >>(uint8& object);
	inline InputSerializer& operator >>(uint16& object);
	inline InputSerializer& operator >>(uint32& object);
	inline InputSerializer& operator >>(uint64& object);
	inline InputSerializer& operator >>(float& object);
	inline InputSerializer& operator >>(double& object);

	inline InputSerializer& operator >>(ByteArray& object);
	inline InputSerializer& operator >>(UByteArray& object);
	inline InputSerializer& operator >>(ShortArray& object);
	inline InputSerializer& operator >>(UShortArray& object);
	inline InputSerializer& operator >>(IntArray& object);
	inline InputSerializer& operator >>(UIntArray& object);
	inline InputSerializer& operator >>(Int64Array& object);
	inline InputSerializer& operator >>(UInt64Array& object);
	inline InputSerializer& operator >>(FloatArray& object);
	inline InputSerializer& operator >>(DoubleArray& object);

	InputSerializer& operator >>(Chunk& object);

	InputSerializer& operator >>(String& object);
	InputSerializer& operator >>(UniString& object);

	inline void Seek(std::streamoff offset, std::ios_base::seekdir dir =
			std::ios_base::cur);
	inline std::streamoff Tell();

	inline void Skip(Chunk& object);
	inline bool IsEndOfStream() const;
	static inline bool IsValid(const Chunk& c);

	static inline bool IsEndianCorrected();

protected:
	inline void Fill();

	enum {
		LOCAL_BUFFER_SIZE = 64,
	};

	uint8 localBuffer[LOCAL_BUFFER_SIZE];
	std::streamoff streamSize;
	int32 left;
	uint8* moving;

	InputStreamPtr inStream;
};

class _NexBaseAPI ChunkInputStream: public InputSerializer {
	static const Chunk First;

public:
	/** @remarks Constructor **/
	inline ChunkInputStream(InputStreamPtr& o) :
			InputSerializer(o) {
	}
	/** @remarks This will skip the chunk specified and place the file
	 *           file pointer right after that
	 **/
	inline ChunkInputStream& MoveAfter(const Chunk& v) {
		if ((int32) v.first.second >= 0) {
			inStream->Seek(v.second + v.first.second, std::ios_base::beg);
			streamSize = inStream->GetSize() - inStream->Tell();
		}
		return *this;
	}

	/** @remarks This will skip any subsequent chunks until the header
	 *          is found, or the stream is closed.
	 */
	ChunkInputStream& ReadChunk(uint16 header, Chunk& read,
			const Chunk& prevChunk = ChunkInputStream::First);
};
}

#include <Serializer.hpp>

#endif //NEXTAR_SERIALIZER_H

