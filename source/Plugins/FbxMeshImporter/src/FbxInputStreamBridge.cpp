/*
 * FbxInputStreamBridge.cpp
 *
 *  Created on: 31-Aug-2014
 *      Author: obhi
 */
#include <FbxMeshImporter.h>
#include <FbxInputStreamBridge.h>

namespace FbxMeshImporter {

FbxInputStreamBridge::FbxInputStreamBridge(InputStreamPtr& input, int _readerId) :
	inputStream(input), inputLocation(-1), readerId(_readerId),
	errorFlag(false) {
	if (inputStream)
		inputLocation = inputStream->Tell();
}

FbxInputStreamBridge::~FbxInputStreamBridge() {
	Close();
}

bool FbxInputStreamBridge::Open(void*) {
	if (inputStream) {
		inputStream->Seek(0, std::ios::beg);
		return true;
	}

	return false;
}

bool FbxInputStreamBridge::Flush() {
	return true;
}

FbxStream::EState FbxInputStreamBridge::GetState() {
	return inputStream ? FbxStream::eOpen : FbxStream::eClosed;
}

int FbxInputStreamBridge::Write(const void*, int numBytes) {
	// nothing
	return 0;
}

int FbxInputStreamBridge::Read(void* buffer, int numBytes) const {
	int numReadBytes = 0;
	if (inputStream && numBytes > 0 ) {
		if( (numReadBytes = (int)inputStream->Read(buffer, (uint32)numBytes)) != numBytes )
			errorFlag = true;
	} else
		errorFlag = true;
	return numReadBytes;
}

int FbxInputStreamBridge::GetReaderID() const {
	return readerId;
}

int FbxInputStreamBridge::GetWriterID() const {
	return -1;
}

void FbxInputStreamBridge::Seek(const FbxInt64& pOffset,
		const FbxFile::ESeekPos& pSeekPos) {
	if (inputStream) {
		switch ( pSeekPos ) {
		case FbxFile::eBegin:
			inputStream->Seek( (long)(inputLocation + pOffset), std::ios::beg );
			break;
		case FbxFile::eCurrent:
			inputStream->Seek( (long)pOffset, std::ios::cur );
			break;
		case FbxFile::eEnd:
			inputStream->Seek( (long)pOffset, std::ios::end );
			break;
		}
	}
}

long FbxInputStreamBridge::GetPosition() const {
	if (inputStream)
		return (long)(inputStream->Tell() - inputLocation);
	return -1;
}

void FbxInputStreamBridge::SetPosition(long pPosition) {
	if (inputStream)
		inputStream->Seek( (long)(inputLocation + pPosition), std::ios::beg );
}

int FbxInputStreamBridge::GetError() const {
	return errorFlag ? -1 : 0;
}

void FbxInputStreamBridge::ClearError() {
	errorFlag = false;
}

bool FbxInputStreamBridge::Close() {
	return true;
}

} /* namespace FbxMeshImporter */
