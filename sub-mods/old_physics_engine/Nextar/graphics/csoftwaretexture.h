
//@ Abhishek Dey
//@ September 03 2007

#ifndef __XSOFTWAREIMAGE_H__
#define __XSOFTWAREIMAGE_H__

#include <iobject.h>
#include <xtypesex.h>
#include <graphics/xcolor.h>

namespace nextar
{

class cSoftwareTexture : public iObject
{
private:
	void*		mpData;
	size2_x		mkDimension;
	COLORFORMAT meColorFormat;
	s32			miBitsPerPixel;
	s32			miPitch;
	bool		mbLocked;
	bool		mbExternalPtr;
public:
	/************************************
	./ cSoftwareTexture: dtor
	************************************/
	virtual ~cSoftwareTexture();
	cSoftwareTexture( COLORFORMAT, const size2_x& rkDimn, void* pData, bool bKeepPtr );

	_n_inline void* lock() { 
		if( mbLocked ) 
			return 0;
		mbLocked = true; 
		return mpData; 
	}
	_n_inline void  unlock() { mbLocked = false; }
	_n_inline bool  isLocked() { return mbLocked; }
	_n_inline void  canDeleteExternalPtr(bool b) { mbExternalPtr = !b; }

	_n_inline const size2_x&	 getDimension() { return mkDimension; }
	_n_inline COLORFORMAT		 getColorFormat() { return meColorFormat; }
	_n_inline s32				 getBitsPerPixel() { return miBitsPerPixel; }
	_n_inline s32				 getPitch() { return miPitch; }

	/* returns a color value equvalent to the color format in the texture */
	cColor	getPixel( s32 x, s32 y ) const;
	/* texture creation factory */
	static cSoftwareTexture* create( COLORFORMAT, const size2_x& rkDimn, void* pData = 0, bool bKeepPtr = false );

private:

	/* Does everything else after size and colorformat are set */
	void _create();
};

}

#endif //__XSOFTWAREIMAGE_H__