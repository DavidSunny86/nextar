/*
 * FbxInputStreamBridge.h
 *
 *  Created on: 31-Aug-2014
 *      Author: obhi
 */

#ifndef FBXINPUTSTREAMBRIDGE_H_
#define FBXINPUTSTREAMBRIDGE_H_

#include <FbxMeshImporter.h>

namespace FbxMeshImporter {

/*
 *
 */
class FbxInputStreamBridge: public FbxStream {
public:
	FbxInputStreamBridge(InputStreamPtr& inputStream);
	virtual ~FbxInputStreamBridge();

	virtual bool Open(void* pStreamData);
	virtual bool Flush();
	virtual EState GetState();
	virtual int Write(const void* /*pData*/, int /*pSize*/);
	virtual int Read(void* /*pData*/, int /*pSize*/) const;
	virtual int GetReaderID() const;
	virtual int GetWriterID() const;
	virtual void Seek(const FbxInt64& pOffset, const FbxFile::ESeekPos& pSeekPos);
	virtual long GetPosition() const;
	virtual void SetPosition(long pPosition);
	virtual int GetError() const;
	virtual void ClearError();
	virtual bool Close();

protected:
	mutable bool errorFlag;
	std::streamoff inputLocation;
	mutable InputStreamPtr inputStream;
};

} /* namespace FbxMeshImporter */

#endif /* FBXINPUTSTREAMBRIDGE_H_ */
