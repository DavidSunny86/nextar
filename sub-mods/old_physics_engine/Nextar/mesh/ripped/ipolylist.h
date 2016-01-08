//@# Abhishek Dey
//@# Oct 22 2007

#ifndef NEXTAR_IPOLYLIST_H
#define NEXTAR_IPOLYLIST_H

namespace nextar
{

struct iPolyList
{
	virtual ~iPolyList() {}
	virtual u32		getClassFlags() = 0;
	virtual s32		getVertexCount() = 0;
	virtual void*	getVertices(s32 &stride) = 0;
	virtual void*	getTextureVertices(s32 level,s32 &stride) = 0;
	virtual u16*	getIndices(s32 &count) = 0;
};

}

#endif //NEXTAR_IPOLYLIST_H