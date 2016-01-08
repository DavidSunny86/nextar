//
// File Name :: cDirectX9Texture.cpp
// Author    :: Abhishek Dey
// Created   :: 10/4/2006 5:00:30 PM
//-------------------------------------------------------------------------------------

#include "cdirectx9texture.h"
#include <cstr.h>
#include <irenderer.h>
#include <windows.h>
#include <d3dx9core.h>

#define ERROR1 "cDirectX9Texture::cDirectX9Texture(FromFile) := Unable to create mpkTexture!"
#define ERROR2 "cDirectX9Texture::_createTexture() := format not supported."
#define ERROR3 "cDirectX9Texture::_createTexture := could not create mpkTexture"
#define ERROR4 "cDirectX9Texture :: _createRenderTarget := Could not create render target"
#define ERROR5 "cDirectX9Texture :: _createRenderTarget := Could not get back buffer"
#define ERROR6 "cDirectX9Texture::createMipLevels := Could not load X lib. Failed to generate mip levels."
#define ERROR7 "cDirectX9Texture::createMipLevels := Failed to get func pointer."
#define ERROR8 "cDirectX9Texture::createMipLevels := Could not generate mip levels"
#define ERROR9 "cDirectX9Texture::lock := Could not lock mpkTexture."
#define ERROR10 "cDirectX9Texture::_copyTexture := Unsupported texture format"
#define ERROR11 "cDirectX9Texture::_copyTexture := Unable to lock texture"
#define ERROR12 "cDirectX9Texture::_copyTo16BitTexture := Could not unlock mpkTexture!"
#define ERROR13 "cDirectX9Texture::_copyTexture := Unable to lock texture"
#define ERROR14 "cDirectX9Texture::_copyTo16BitTexture := Could not unlock mpkTexture!"
#define D3DXFilterTexture_FuncName "D3DXFilterTexture"
typedef HRESULT (WINAPI *D3DXFilterTexturePtr) ( LPDIRECT3DBASETEXTURE9 pBaseTexture, CONST PALETTEENTRY *pPalette, UINT SrcLevel, DWORD MipFilter );

namespace nextar
{

extern IDirect3DDevice9* gpkDirect3DDevice;
//------------------------------------------------------------------------------
// filterFlag
//------------------------------------------------------------------------------
static TEXTUREFLAGS filterFlag( u32 flags )
{
	if(flags & TCF_FORCE_16B)
		return TCF_16B;
	if (flags & TCF_SPEED)
		return TCF_SPEED;
	if (flags & TCF_16B)
		return TCF_16B;
	if (flags & TCF_32B)
		return TCF_32B;
	if (flags & TCF_QUALITY)
		return TCF_QUALITY;
	return TCF_SPEED;
}

//------------------------------------------------------------------------------
// getPow2Dimension
//------------------------------------------------------------------------------
s32 getPow2Dimension( s32 from )
{
	s32 r = 1;
	while( r < from )
		r <<= 1;
	if( r > from && r > 64 )
		r >>= 1;
	return r;
}

//------------------------------------------------------------------------------
// ctor
//------------------------------------------------------------------------------
cDirectX9Texture::cDirectX9Texture( cSoftwareTexture *Image, u32 flags )
{
	X_BreakOutIf(!gpkDirect3DDevice);
	RefObjectInit(cDirectX9Texture);
	
	bool generateMipLevels = (flags & TCF_MIPMAPS) != 0;

	if (Image)
	{
		_createTexture( Image, flags );

		if(mpkTexture)
		{
			if ( _copyTexture( Image ) && generateMipLevels )
			{
				_createMipMaps();
				muFlags |= HasMipMaps;
			}				
		}
		else
			ReportBug( ERROR1, "cDirectX9Texture" );
	}
}

//------------------------------------------------------------------------------
// ctor
//------------------------------------------------------------------------------
cDirectX9Texture::cDirectX9Texture( s32 iWidth, s32 iHeight )
{
	X_BreakOutIf(!gpkDirect3DDevice);
	RefObjectInit(cDirectX9Texture);
	mkDimension.dx = (i2d)iWidth;
	mkDimension.dy = (i2d)iHeight;
	_createRenderTarget();
}

//------------------------------------------------------------------------------
// ctor
//------------------------------------------------------------------------------
cDirectX9Texture::cDirectX9Texture( s32 iWidth, s32 iHeight, u32 flags )
{
	X_BreakOutIf(!gpkDirect3DDevice);
	RefObjectInit(cDirectX9Texture);
	mkDimension.dx = (i2d)iWidth;
	mkDimension.dy = (i2d)iHeight;
	_createTexture( 0, flags );
}

//------------------------------------------------------------------------------
// dtor :: ~cDirectX9Texture
//------------------------------------------------------------------------------
cDirectX9Texture::~cDirectX9Texture()
{
	if( mpkTexture )
		mpkTexture->Release();
}

//------------------------------------------------------------------------------
// _createTexture
//------------------------------------------------------------------------------
void cDirectX9Texture::_createTexture( cSoftwareTexture *image, u32 flags )
{
	size2_x d = image? image->getDimension() : mkDimension;
	mkDimension.dx = (i2d)getPow2Dimension(d.dx);
	mkDimension.dy = (i2d)getPow2Dimension(d.dy);
	D3DFORMAT Format = D3DFMT_A1R5G5B5;
	
	switch( filterFlag(flags) )
	{
	case TCF_32B:	
		Format = D3DFMT_A8R8G8B8; break;
	case TCF_QUALITY:
		if( image )
		{
			switch( image->getColorFormat() )
			{
			case D3DFMT_X8R8G8B8:
			case D3DFMT_A8R8G8B8:
			case D3DFMT_R8G8B8:
				Format = D3DFMT_A8R8G8B8; break;
			case D3DFMT_A1R5G5B5:
			case D3DFMT_R5G6B5:
				Format = D3DFMT_A1R5G5B5; break;
			default:
				ReportBug( ERROR2, "_createTexture" );
				return;
			};
		}
		else
		{
			Format = flags & TCF_32B? D3DFMT_A8R8G8B8 : D3DFMT_A1R5G5B5;
		}
		break;
	case TCF_16B:
	case TCF_SPEED:

		Format = D3DFMT_A1R5G5B5; break;
	};
	if(Format == D3DFMT_A1R5G5B5 ) meColorFormat = CF_A1R5G5B5;
	else meColorFormat = CF_A8R8G8B8;

	bool miplevels = ( flags & TCF_MIPMAPS ) ? 1 : 0;
	u32 usage = ( ( flags & TCF_USE_HARDWARE_MIPMAPS ) && X_SharedPtr(iRenderer)->isFeatureSupported(FS_AUTOGEN_MIP_MAPS) ) ? D3DUSAGE_AUTOGENMIPMAP : 0;
	if(usage)
		muFlags |= HasHardwareMipMaps;

	HRESULT rval = gpkDirect3DDevice->CreateTexture( mkDimension.dx, mkDimension.dy, miplevels ? 0 : 1, usage, Format, D3DPOOL_MANAGED, &mpkTexture, 0 );

	if( FAILED( rval ) )
	{
		if( Format == D3DFMT_A8R8G8B8 )
			rval = gpkDirect3DDevice->CreateTexture( mkDimension.dx, mkDimension.dy, miplevels ? 0 : 1, usage, D3DFMT_A1R5G5B5, D3DPOOL_MANAGED, &mpkTexture, 0 );
	}

	if( FAILED( rval ) )
	{
		ReportBug( ERROR3, "_createTexture" );
		return;
	}

	D3DLOCKED_RECT rc;
	mpkTexture->LockRect( 0, &rc, 0, 0 );
	miPitch = rc.Pitch;
	mpkTexture->UnlockRect(0);
}

//------------------------------------------------------------------------------
// _createRenderTarget
//------------------------------------------------------------------------------
void cDirectX9Texture::_createRenderTarget()
{
	mkDimension.dx = (i2d)getPow2Dimension(mkDimension.dx);
	mkDimension.dy = (i2d)getPow2Dimension(mkDimension.dy);

	IDirect3DSurface9*	bbuff = 0;
	if( SUCCEEDED( gpkDirect3DDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &bbuff ) ) )
	{
		D3DSURFACE_DESC desc;
		bbuff->GetDesc(&desc);
		D3DFORMAT d3dformat = desc.Format;

		if ( d3dformat == D3DFMT_X8R8G8B8 )
			d3dformat = D3DFMT_A8R8G8B8;

		bbuff->Release();

		if( FAILED( gpkDirect3DDevice->CreateTexture(	mkDimension.dx, mkDimension.dy, 1, D3DUSAGE_RENDERTARGET,	d3dformat, D3DPOOL_DEFAULT, &mpkTexture, 0 ) ) )
			ReportBug( ERROR4, "_createRenderTarget" );
	}
	else
		ReportBug( ERROR5, "_createRenderTarget" );
}

//------------------------------------------------------------------------------
// _createMipMaps
//------------------------------------------------------------------------------
void cDirectX9Texture::_createMipMaps( )
{
	if( mpkTexture )
	{
		if( muFlags & HasHardwareMipMaps )
			mpkTexture->GenerateMipSubLevels();
		return;
		
		cStr DllName;
		DllName.format( X_Text("d3dx9_%d.dll"), 15, ((s32)D3DX_SDK_VERSION) );
	
		HINSTANCE h = LoadLibrary( DllName.c_str() );
		if( !h )
		{
			ReportBug( ERROR6, "_createMipMaps" );
			goto quit;
		}
	
		D3DXFilterTexturePtr FilterFunc = (D3DXFilterTexturePtr)GetProcAddress( (HMODULE)h, D3DXFilterTexture_FuncName );
		if( !FilterFunc ) 
		{
			ReportBug( ERROR7, "_createMipMaps" );
			goto quit;
		}

		if( FAILED( FilterFunc( mpkTexture, 0, (UINT)-1, (UINT)-1) ) )
		{
			ReportBug( ERROR8, "_createMipMaps" );
			goto quit;
		}

quit:
		FreeLibrary(h);
		return;
	}

	return;
}

//------------------------------------------------------------------------------
// lock
//------------------------------------------------------------------------------
void* cDirectX9Texture::lock( bool bwriteaccess )
{
	if (!mpkTexture)
		return 0;

	D3DLOCKED_RECT rect;
	HRESULT hr = mpkTexture->LockRect(0, &rect, 0, bwriteaccess ? 0 : D3DLOCK_READONLY );

	if (FAILED(hr))
	{
		ReportBug( ERROR9, "_createMipMaps" );
		return 0;
	}

	return rect.pBits; 
}

//------------------------------------------------------------------------------
// unlock
//------------------------------------------------------------------------------
void cDirectX9Texture::unlock()
{
	if (!mpkTexture)
		return;

	mpkTexture->UnlockRect(0);
}

//------------------------------------------------------------------------------
// ReCreateMipMaps
//------------------------------------------------------------------------------
void cDirectX9Texture::recreateMipMaps()
{
	if(muFlags & HasMipMaps)
		_createMipMaps();
}

//------------------------------------------------------------------------------
// getRenderTargetPtr
//------------------------------------------------------------------------------
IDirect3DSurface9* cDirectX9Texture::getRenderTargetPtr() 
{
	if( muFlags & IsRenderTarget )
	{
		IDirect3DSurface9 *pSurface = 0;
		if (mpkTexture)
			mpkTexture->GetSurfaceLevel(0, &pSurface);
		
		if (pSurface)
			pSurface->Release();

		return pSurface;
	}
	return 0;
}

//------------------------------------------------------------------------------
// _copyTexture
//------------------------------------------------------------------------------
bool cDirectX9Texture::_copyTexture( cSoftwareTexture* Image )
{
	if ( mpkTexture && Image )
	{
		D3DSURFACE_DESC desc;

		mpkTexture->GetLevelDesc(0, &desc);

		mkDimension.dx = desc.Width;
		mkDimension.dy = desc.Height;

		bool SurfaceHasSameSize = (mkDimension == Image->getDimension());

		if ( desc.Format == D3DFMT_A1R5G5B5 )
			return _copyTo16BitTexture( Image, SurfaceHasSameSize );
		else if (desc.Format == D3DFMT_A8R8G8B8)
			return _copyTo32BitTexture( Image, SurfaceHasSameSize );
		else
			ReportBug( ERROR10, "_copyTexture" );
	}
	return true;
}

//------------------------------------------------------------------------------
// _copyTo32BitTexture
//------------------------------------------------------------------------------
bool cDirectX9Texture::_copyTo32BitTexture( cSoftwareTexture *Image, bool SurfaceHasSameSize )
{
	D3DLOCKED_RECT rect;
	HRESULT hr = mpkTexture->LockRect(0, &rect, 0, 0);

	if (FAILED(hr))
	{
		ReportBug( ERROR11, "_copyTo32BitTexture" );
		return false;
	}

	s32* dest	 = (s32*)rect.pBits;
	s32* source = (s32*)Image->lock();
	miPitch = rect.Pitch;
	s32 pitch = rect.Pitch / 4;
	size2_x	ImageSize = Image->getDimension();

	if (SurfaceHasSameSize)
	{
		if (Image->getColorFormat() == CF_A8R8G8B8)
		{
			for ( s32 x=0; x<ImageSize.dx; ++x )
				for ( s32 y=0; y<ImageSize.dy; ++y )
					dest[x + y*pitch] = source[x + y*ImageSize.dx];
		}
		else
		{
			for ( s32 x=0; x<ImageSize.dx; ++x )
				for ( s32 y=0; y<ImageSize.dy; ++y )
				{
					cColor pix = Image->getPixel(x,y);
					dest[x + y*pitch] = pix.toA8R8G8B8();
				}
		}
	}
	else
	{
		f32 factorX = (f32)ImageSize.dx / (f32)mkDimension.dx;
		f32 factorY = (f32)ImageSize.dy / (f32)mkDimension.dy;
		f32 sy;

		if (Image->getColorFormat() == CF_A8R8G8B8)
		{
			for (s32 x=0; x<mkDimension.dx; ++x)
			{
				sy = 0.0f;
				for (s32 y=0; y<mkDimension.dy; ++y)
				{
					dest[(s32)(y*pitch + x)] = source[(s32)(((s32)sy)*ImageSize.dx + x*factorX)];
					sy+=factorY;
				}
			}
		}
		else
		{
			for (s32 x=0; x<mkDimension.dx; ++x)
			{
				sy = 0.0f;
				for (s32 y=0; y<mkDimension.dy; ++y)
				{
					cColor pix = Image->getPixel((s32)(x*factorX), (s32)sy);
					dest[(s32)(y*pitch + x)] = pix.toA8R8G8B8();
					sy+=factorY;
				}
			}
		}
	}		

	hr = mpkTexture->UnlockRect(0);
	if (FAILED(hr))
	{
		ReportBug( ERROR12, "_copyTo32BitTexture" );
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
// _copyTo16BitTexture
//------------------------------------------------------------------------------
bool cDirectX9Texture::_copyTo16BitTexture( cSoftwareTexture *Image, bool SurfaceHasSameSize )
{
	D3DLOCKED_RECT rect;
	HRESULT hr = mpkTexture->LockRect(0, &rect, 0, 0);

	if (FAILED(hr))
	{
		ReportBug( ERROR13, "_copyTo16BitTexture" );
		return false;
	}

	s16* dest	 = (s16*)rect.pBits;
	s16* source = (s16*)Image->lock();
	miPitch = rect.Pitch;
	s16 pitch = rect.Pitch / 2;
	size2_x ImageSize = Image->getDimension();
	
	if (SurfaceHasSameSize)
	{
		if (Image->getColorFormat() == CF_A1R5G5B5)
		{
			for ( s32 x=0; x<ImageSize.dx; ++x )
				for ( s32 y=0; y<ImageSize.dy; ++y )
					dest[x + y*pitch] = source[x + y*ImageSize.dx];
		}
		else
		{
			for ( s32 x=0; x<ImageSize.dx; ++x )
				for ( s32 y=0; y<ImageSize.dy; ++y )
				{
					cColor pix = Image->getPixel(x,y);
					dest[x + y*pitch] = pix.toA1R5G5B5();
				}
		}
	}
	else
	{
		f32 factorX = (f32)ImageSize.dx / (f32)mkDimension.dx;
		f32 factorY = (f32)ImageSize.dy / (f32)mkDimension.dy;
		f32 sy;

		if (Image->getColorFormat() == CF_A1R5G5B5)
		{
			for (s32 x=0; x<mkDimension.dx; ++x)
			{
				sy = 0.0f;
				for (s32 y=0; y<mkDimension.dy; ++y)
				{
					dest[(s32)(y*pitch + x)] = source[(s32)(((s32)sy)*ImageSize.dx + x*factorX)];
					sy+=factorY;
				}
			}
		}
		else
		{
			for (s32 x=0; x<mkDimension.dx; ++x)
			{
				sy = 0.0f;
				for (s32 y=0; y<mkDimension.dy; ++y)
				{
					cColor pix = Image->getPixel((s32)(x*factorX), (s32)sy);
					dest[(s32)(y*pitch + x)] = pix.toA1R5G5B5();
					sy+=factorY;
				}
			}
		}
	}		

	hr = mpkTexture->UnlockRect(0);
	if (FAILED(hr))
	{
		ReportBug( ERROR14, "_copyTo16BitTexture" );
		return false;
	}

	return true;
}

}