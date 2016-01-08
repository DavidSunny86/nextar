//@ Abhishek Dey
//@ Jan 10 2007
//@ Sep 27 2007

#include "cSoftwareTexture.h"
#include <xmemory.h>

namespace nextar
{

/* destructor */
cSoftwareTexture::~cSoftwareTexture()
{
	if( mpData && !mbExternalPtr )
		delete [] (s8*)mpData;
}

/* ctor */
cSoftwareTexture::cSoftwareTexture( COLORFORMAT eFmt, const size2_x& rkDimn, 
									 void* pData, bool bKeepPtr ) : meColorFormat(eFmt),
									 mkDimension(rkDimn), mbExternalPtr(bKeepPtr), mpData(0),
									 mbLocked(false)
{
	RefObjectInit(cSoftwareTexture);

	if( pData == NULL )
		_create();
	else
	{
		if( bKeepPtr )
		{
			mpData = (void*)(0xffffffff);
			_create();
			mpData = pData;
		}
		else
		{
			_create();
			copyMemory( mpData, pData, (rkDimn.dx*rkDimn.dy*miBitsPerPixel)/8);
		}
	}
}

/* get pixel */
cColor cSoftwareTexture::getPixel( s32 x, s32 y ) const
{
	X_BreakOutIf( x >= mkDimension.dx || y >= mkDimension.dy )

	switch(meColorFormat)
	{
	case CF_A1R5G5B5:
		return A1R5G5B5toA8R8G8B8((((s16*)mpData)[y*mkDimension.dx + x]));
	case CF_R5G6B5:
		return R5G6B5toA8R8G8B8((((s16*)mpData)[y*mkDimension.dx + x]));
	case CF_A8R8G8B8:
		return ((s32*)mpData)[y*mkDimension.dx + x];
	case CF_R8G8B8:
		{
			u8* p = &((u8*)mpData)[ (y*mkDimension.dx + x)*3 ];
			return (s32)(A8R8G8B8(0xff,p[0],p[1],p[2]));
		}
	}
	return cColor(0);
}

/* _create */
void cSoftwareTexture::_create()
{
	switch(meColorFormat)
	{
	case CF_R5G6B5:
	case CF_A1R5G5B5:
		miBitsPerPixel = 16;break;
	case CF_R8G8B8:
		miBitsPerPixel = 24;break;
	case CF_A8R8G8B8:
		miBitsPerPixel = 32;break;
	default:
		miBitsPerPixel = -1;break;
	}

	if(miBitsPerPixel <= 0 ) return;
	
	// Pitch should be aligned...
	miPitch = miBitsPerPixel/8 * mkDimension.dx;

	if (!mpData)
		mpData = new s8[mkDimension.dy * miPitch];
}

/* wrapper */
cSoftwareTexture* cSoftwareTexture::create( COLORFORMAT fmt, const size2_x &d, void *dt, bool k )
{
	return new cSoftwareTexture( fmt, d, dt, k );
}

}