
#include "BaseHeaders.h"
#include "Serializer.h"

namespace nextar {

	InputSerializer& InputSerializer::operator >>(Chunk& object) {
		if (object.first.first != MARKER_INVALID_CHUNK)
			Skip(object);
		uint16 globalMarker;
		(*this) >> globalMarker;
		if (globalMarker == MARKER_GLOBAL_CHUNK_ID) {
			(*this) >> object.first.first >> object.first.second;
			// rewind and tell
			if (left) {
				inStream->Seek(-left, std::ios_base::cur);
				streamSize += left;
				left = 0;
			}
			object.second = inStream->Tell();
		}
		return *this;
	}

	InputSerializer& InputSerializer::operator >> (String& object) {
		uint8 stringMarker;
		(*this) >> stringMarker;
		if (stringMarker == MARKER_STRING_UTF8) {
			uint32 length;
			(*this) >> length;
			object.reserve(length);
			while (length > 0) {
				if (!left)
					Fill();
				uint32 read = std::min<uint32> (left, length);
				object.append((const char*)moving, read);
				left -= read;
				length -= read;
				moving += read;
			}
		} else
			Warn("Invalid string read!");
		return *this;
	}

	InputSerializer& InputSerializer::operator >> (UniString& object) {
		uint8 stringMarker;
		(*this) >> stringMarker;
		if (stringMarker == MARKER_STRING_UTF16) {
			uint32 length;
			(*this) >> length;
			object.reserve(length);
			length <<= 1; // 2 bytes per char
			if (left && left%2==1) { // just rwind
				inStream->Seek(-1, std::ios_base::cur);
				left--;
				streamSize += 1;
			}
			while (length > 0) {
				if (!left)
					Fill();
				uint32 read = std::min<uint32> (left, length);
	#if NEX_NEED_ENDIAN_SWAPPING
				uint16* data = static_cast<uint16*>(moving);
				for(uint32 i = 0; i < (read/2); ++i, ++data)
					*data = SWAP16(*data);
	#endif
				object.append((UniString::value_type*)moving, read);
				left -= read;
				length -= read;
				moving += read;
			}
		} else
			Warn("Invalid string read!");
		return *this;
	}


    const InputSerializer::Chunk ChunkInputStream::First(InputSerializer::ChunkHeader(MARKER_INVALID_CHUNK,0), 0);

    ChunkInputStream& ChunkInputStream::ReadChunk(uint16 header, InputSerializer::Chunk& read,
    		const InputSerializer::Chunk& prevChunk) {
    	read = prevChunk;
        do {
        	MoveAfter(read) >> read;
        } while(read.first.first != MARKER_INVALID_CHUNK && read.first.first != header);

        return (*this);
    }
}

