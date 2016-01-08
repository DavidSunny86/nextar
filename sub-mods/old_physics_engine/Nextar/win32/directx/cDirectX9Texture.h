//
// File Name :: cDirectX9Texture.h
// Author    :: Abhishek Dey
// Created   :: 10/4/2006 4:55:29 PM
//-------------------------------------------------------------------------------------
#ifndef NEXTART_DIREXTX9TEXTURE
#define NEXTART_DIREXTX9TEXTURE

#include <itexture.h>
#include <csoftwaretexture.h>
#include <d3d9.h>

namespace nextar
{

class cDirectX9Texture : public iTexture
{
private:
	
	s32					miPitch;
	IDirect3DTexture9*	mpkTexture;

public:
	
	cDirectX9Texture( s32 iWidth, s32 iHeight );
	cDirectX9Texture( s32 iWidth, s32 iHeight, u32 flags );
	cDirectX9Texture( cSoftwareTexture *, u32 flags );
	~cDirectX9Texture();

	_n_inline IDirect3DTexture9* getTextureInterface() { return mpkTexture;}

	virtual void* lock( bool bWriteAccess = true );
	virtual void  unlock();
	virtual void  recreateMipMaps();

	// get a ptr to render target
	IDirect3DSurface9*	getRenderTargetPtr();

private:

	void _createRenderTarget();
	void _createTexture( cSoftwareTexture*, u32 flags );
	bool _copyTexture( cSoftwareTexture* );
	bool _copyTo16BitTexture( cSoftwareTexture*, bool sameSize );
	bool _copyTo32BitTexture( cSoftwareTexture*, bool sameSize );
	void _createMipMaps();
};

}

#endif //NEXTART_DIREXTX9TEXTURE