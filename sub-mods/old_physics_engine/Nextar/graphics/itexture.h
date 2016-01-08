//@ Abhishek Dey
//@ 1/9/2007
//@ September 03 2007

#ifndef __IXTEXTURE_H__
#define __IXTEXTURE_H__

#include <iobject.h>
#include <xtypesex.h>
#include <graphics/xcolor.h>

namespace nextar
{

// texture flags
enum TEXTUREFLAGS
{
	TCF_INVALID = 0x0,
	TCF_16B = 1,
	TCF_32B = 2,
	TCF_SPEED = 4,
	TCF_QUALITY = 8,
	TCF_MIPMAPS = 16,
	TCF_FORCE_16B = 32,
	TCF_USE_HARDWARE_MIPMAPS = 0x40,
	FORCE_32BIT = 0x7fffffff
};

class iTexture : public iObject
{
public:
	enum { HasMipMaps = 1, HasHardwareMipMaps = 2, IsRenderTarget = 4 };
protected:
	COLORFORMAT		meColorFormat;
	size2_x			mkDimension;
	u32				muFlags;
public:

	_n_inline COLORFORMAT			getColorFotmat() const { return meColorFormat; }
	_n_inline bool				isRenderTarget() const { return muFlags & IsRenderTarget; }
	_n_inline bool				hasMipMaps() const { return muFlags & HasMipMaps; }
	_n_inline bool				hasHardwareMipMaps() const { return muFlags & HasHardwareMipMaps; }
	_n_inline const size2_x&		getDimension() const { return mkDimension; }

	virtual void* lock( bool bWriteAccess = true ) = 0;
	virtual void  unlock() = 0;
	virtual void  recreateMipMaps() = 0;

	_n_inline iTexture() : muFlags(0) {}
	virtual ~iTexture() {}
};

}


#endif //__IXTEXTURE_H__