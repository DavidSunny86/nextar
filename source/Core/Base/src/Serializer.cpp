#include <BaseHeaders.h>
#include <Serializer.h>

namespace nextar {

const InputSerializer::Chunk InputSerializer::First(
		InputSerializer::ChunkHeader(MARKER_INVALID_CHUNK, 0), 0);
const InputSerializer::Chunk InputSerializer::Invalid(
		InputSerializer::ChunkHeader(MARKER_INVALID_CHUNK, 0), 0);

enum {
	CHUNK_HEADER_SIZE = 8
};

/*********************************************************
 * InputSerializer
*********************************************************/
InputSerializer& InputSerializer::operator >>(Chunk& object) {
	if (object.header.headerName != MARKER_INVALID_CHUNK)
		Skip(object);
	uint16 globalMarker;
	if (left < 2)
		Fill();
	if (left < 2) {
		object = Invalid;
		return *this;
	}
	(*this) >> globalMarker;
	if (globalMarker == MARKER_GLOBAL_CHUNK_ID) {
		(*this) >> object.header.headerName >> object.header.chunkSize;
		// rewind and tell
		//if (left) {
		//	inStream->Seek(-left, std::ios_base::cur);
		//	streamSize += left;
		//	left = 0;
		//}
		//object.second = inStream->Tell();
		object.startInFile = inStream->Tell()-left;
	} else
		object = Invalid;
	return *this;
}

InputSerializer& InputSerializer::operator >>(String& object) {
	uint8 stringMarker;
	(*this) >> stringMarker;
	if (stringMarker == MARKER_STRING_UTF8) {
		uint32 length;
		(*this) >> length;
		object.clear();
		object.reserve(length);
		while (length > 0) {
			if (!left)
				Fill();
			uint32 read = std::min<uint32>(left, length);
			object.append((const char*) moving, read);
			left -= read;
			length -= read;
			moving += read;
		}
	} else
		Warn("Invalid string read!");
	return *this;
}

InputSerializer& InputSerializer::operator >>(UniString& object) {
	uint8 stringMarker;
	(*this) >> stringMarker;
	if (stringMarker == MARKER_STRING_UTF16) {
		uint32 length;
		(*this) >> length;
		object.reserve(length);
		length <<= 1; // 2 bytes per char
		if (left && left % 2 == 1) { // just rwind
			inStream->Seek(-1, std::ios_base::cur);
			left--;
			streamSize += 1;
		}
		while (length > 0) {
			if (!left)
				Fill();
			uint32 read = std::min<uint32>(left, length);
#if NEX_NEED_ENDIAN_SWAPPING
			uint16* data = static_cast<uint16*>(moving);
			for(uint32 i = 0; i < (read/2); ++i, ++data)
			*data = SWAP16(*data);
#endif
			object.append((UniString::value_type*) moving, read);
			left -= read;
			length -= read;
			moving += read;
		}
	} else
		Warn("Invalid string read!");
	return *this;
}

ChunkInputStream& ChunkInputStream::ReadChunk(uint16 header,
		InputSerializer::Chunk& read, const InputSerializer::Chunk& prevChunk) {
	read = prevChunk;
	do {
		*this >> read;
	} while (read.header.headerName != MARKER_INVALID_CHUNK
			&& read.header.headerName != header);

	return (*this);
}

/*********************************************************
 * ChunkOutputStream
*********************************************************/
ChunkOutputStream::ChunkOutputStream(OutputStreamPtr& mainOut) : 
	outStream(mainOut)
	,cursor(nullptr)
	,writeOffset(0) {
	pseudoOutput = Assign<OutputStream>(NEX_NEW(MemoryOutputStream()));
}

ChunkOutputStream::~ChunkOutputStream() {
	BeginChunk(MARKER_CHUNK_FILE_END_HEADER);
	EndChunk();
	Write(roots);
	outStream->Write(pseudoOutput->GetData(), pseudoOutput->GetSize());
}

OutputSerializer& ChunkOutputStream::BeginChunk(uint16 header) {
	OutChunkList* l;
	
	if (cursor) {
		cursor->serializer.Flush();
		l = &(cursor->childrens);
	}
	else
		l = &(roots);
			
	l->push_back(Internal());
	auto &chunkItem = l->back();
	chunkItem.completed = false;
	chunkItem.calculatedSize = 0;
	chunkItem.writeLenghtPos = pseudoOutput->Tell();
	OutputStreamPtr pseudoOut = pseudoOutput;
	chunkItem.serializer = std::move(OutputSerializer(pseudoOut));
	chunkItem.parentChunk = cursor;
	chunkItem.header = header;
	cursor = &chunkItem;
	OutputSerializer::ChunkHeader chkheader;
	chkheader.first = header;
	chkheader.second = 0;
	chunkItem.serializer << chkheader;
	return chunkItem.serializer;
}

void ChunkOutputStream::EndChunk() {
	NEX_ASSERT(cursor);
	if (cursor) {
		cursor->serializer.Flush();
		Process(cursor);
		cursor->completed = true;
		cursor = cursor->parentChunk;
	}
}

void ChunkOutputStream::Process(ChunkOutputStream::Internal* _internal) {
	size_t writtenBytes = _internal->serializer.GetTotalWrittenBytes() - CHUNK_HEADER_SIZE;
	_internal->calculatedSize = (uint32)writtenBytes +
		Process(_internal->childrens);
}

uint32 ChunkOutputStream::Process(ChunkOutputStream::OutChunkList& childrens) {
	uint32 totalSize = 0;
	for(auto &c : childrens) {
		if (!c.completed) {
			Error("A certain chunk was not ended properly!");
			NEX_ASSERT(0);
		}
		totalSize += (c.calculatedSize + CHUNK_HEADER_SIZE);
	}
	return totalSize;
}

void ChunkOutputStream::Write(ChunkOutputStream::Internal* _internal) {
	OutputSerializer::ChunkHeader chkHdr;
	chkHdr.first = _internal->header;
	chkHdr.second = _internal->calculatedSize;
	pseudoOutput->Seek(_internal->writeLenghtPos, std::ios_base::beg);
	OutputStreamPtr pseudoOut = pseudoOutput;
	{
		OutputSerializer ser(pseudoOut);
		ser << chkHdr;
	}

	Write(_internal->childrens);
}

void ChunkOutputStream::Write(ChunkOutputStream::OutChunkList& childrens) {
	for(auto &c : childrens) {
		Write(&c);
	}
}

}

