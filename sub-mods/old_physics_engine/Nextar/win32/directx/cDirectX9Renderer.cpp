
//@ Abhishek Dey
//@ October 2 2006
//@ December 14 2006
//@ September 16 2007
//@ September 23 2007

#include <d3d9.h>
#include <d3dx9shader.h>
#include <isystem.h>
#include <xmemory.h>
#include <math/xmath_c.h>
#include <cstr.h>
#include <win32/directx/cdirectx9renderer.h>
#include <win32/directx/cdirectx9texture.h>
#include <graphics/imaterial.h>
#include <win32/directx/cdirectx9font.h>
#include <win32/directx/directx9materials.h>
//#include "directx9shaders.h"
//#include "cdirectx9toonshader.h"
//#include "cdirectx9llshader.h"
//#include "cdirectx9hlshader.h"

using namespace nextar;
#define XDEBUG_LOG(ll,msg) X_SharedPtr(iSystem)->getLogger().print(ll,msg)
//@ the flag controls whether to compile with dx 
//@ #ifdef COMPILE_WITH_DIRECT3D9 
//@ some functions described here are c type functions that might be called by other 
//@ internal classes, these functions are not exported hence not available 
//@ Some directx interfaces are externally linked but not exported 
namespace nextar
{
/* some state blocks */
enum STATEBLOCKS
{
	SB_BASIC2D = 0,
	SB_STENCIL_FILL,
	SB_STENCIL_FILL_WITH_ALPHA,
	SB_STENCIL_FILL_WITHOUT_ALPHA,
	SB_SHADOW_MODE,
	SB_SHADOW_VOLUME_ZPASS,
	SB_SHADOW_VOLUME_ZFAIL,
	SB_COUNT
};

#define LIGHT_RANGE_MAX 18446743522909402667.065850117074f
#define MAX_2DPOLY_VERT  12
/* the class members as statics */
//---------------------------------------------------------------------------
static D3DPRESENT_PARAMETERS	mkPresentParams;
static D3DCAPS9					gkDeviceCaps;
static IDirect3DStateBlock9*	garrpkStates[SB_COUNT] = {0};
static u32						guBackColor = 0;
static u32						guTotalPolysRendered = 0;
static u32						guTotalStateChanges = 0;
static u32						guCurMaterialFlags = 0;
static u32						guCurLightIdx = 0;
static u32						guLastLightSettings = 0;
static RENDERSTATEMODE			geCurMode = RSC_INVALID;
static VERTEXTYPE				geCurVertexShader = VERTEX_UNUSED;
static iTexture*				garrpkCurTextures[N3DMaxTextureStage] = {0};
static const cLight**			gpkAddedLights = 0;
static cStr						gkDriverName("Unknown");
static D3DLIGHT9				gkLight = { 
	D3DLIGHT_DIRECTIONAL, 
	{0}, {0}, {0}, {0}, {0}, 
	LIGHT_RANGE_MAX,	
	0.0f, 0.0f, 1.0f/200.0f, 0.0f, 0.0f 
};
cDxVertex2D						gk2DVertices[N3D2DVertPrecache];
u16								gu2DIndices[N3D2DIdxPrecache];
IDirect3D9*						mpkDirect3D = 0;
IDirect3DDevice9*				mpkDirect3DDevice = 0;
extern mat44_t					garrkTransforms[TS_COUNT];


} // end of nextar
//---------------------------------------------------------------------------

#pragma comment ( lib, "d3d9.lib" )
#pragma comment ( lib, "d3dx9.lib" )

//---------------------------------------------------------------------------
//@ Function Definitions
//---------------------------------------------------------------------------

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
cDirectX9Renderer::cDirectX9Renderer(HWND hWnd, const size2_x& rkScreenSize, u32 uBits, u32 uFlags)
{
	;

	muRenderFlags = uFlags;
	guCurMaterialFlags = UnsetFlag;
	guBackColor = 0; 
	geCurVertexShader = VERTEX_UNUSED;
	gpkAddedLights = 0;
	guCurLightIdx = 0;
	guLastLightSettings = 0;
	muBitsPerPixel = uBits;

	zeroMemory( garrpkStates, sizeof(garrpkStates) );
	zeroMemory( &mkPresentParams, sizeof(mkPresentParams) );
	zeroMemory( &gkDeviceCaps, sizeof(gkDeviceCaps) );
	zeroMemory( garrpkCurTextures, sizeof(garrpkCurTextures) );

	// initialize the stream of indices
	// and vertices
	for( s32 index = 0; index < N3D2DQuadPrecache; index++ )
	{
		gu2DIndices[0 + index*6] = u16(0 + index*4);
		gu2DIndices[1 + index*6] = u16(1 + index*4);
		gu2DIndices[2 + index*6] = u16(2 + index*4);

		gu2DIndices[3 + index*6] = u16(1 + index*4);
		gu2DIndices[4 + index*6] = u16(3 + index*4);
		gu2DIndices[5 + index*6] = u16(2 + index*4);
	}
	
	for( s32 index = 0; index < N3D2DVertPrecache; index++ )
	{
		gk2DVertices[index].mkPosition.z = 0.5f;
		gk2DVertices[index].mkPosition.w = 1.0f;
	}

	mkScreenSize = rkScreenSize;
	XERRORCODE eRetCode = _create(hWnd);
	if(eRetCode != X_SUCCESS)
	{
		X_SharedPtr(iSystem)->msgBox("Error in Renderer device creation.","Error");
		X_SharedPtr(iSystem)->getLogger().print(LL_FATAL,"Renderer:Error code[%d]",eRetCode);
	}
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
cDirectX9Renderer::~cDirectX9Renderer()
{
	if( gpkAddedLights )
	{
		delete [] gpkAddedLights;
		gpkAddedLights = 0;
	}
	
	if( mpkDirect3DDevice )
	{
		mpkDirect3DDevice->SetTexture(0,0);
		mpkDirect3DDevice->SetTexture(1,0);
	}

	for( s32 i = 0; i < SB_COUNT; i++ )
		SafeRelease( garrpkStates[i] );
	SafeRelease( mpkDirect3DDevice );
	SafeRelease( mpkDirect3D );
	
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
RENDERDEVICE cDirectX9Renderer::getDriverType() const
{
	return RD_DIRECT3D9;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const char* cDirectX9Renderer::getDriverName() const
{
	return gkDriverName;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const u32 cDirectX9Renderer::getTotalPolysRendered() const
{
	return guTotalPolysRendered;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const u32 cDirectX9Renderer::getTotalRenderStateChanges() const
{
	return guTotalStateChanges;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
s32 cDirectX9Renderer::getMaxDynamicLight() const
{
	return (s32)gkDeviceCaps.MaxActiveLights;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const f32* cDirectX9Renderer::getWorld() const
{
	static mat44_t m={0};
	mpkDirect3DDevice->GetTransform(D3DTS_WORLD,reinterpret_cast<D3DMATRIX*>(m));
	return m;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::setBackgroundColor( s32 c )
{
	guBackColor = (u32)c;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::setMode( RENDERSTATEMODE mode )
{
	if( geCurMode != mode )
	{
		switch(mode)
		{
		case RSC_2D:
			garrpkStates[SB_BASIC2D]->Apply();
			setVertexShader(VERTEX_2D);	// set this up before hand
			guTotalStateChanges += 28;
			muRenderFlags |= ResetTransforms;
			break;
		case RSC_3D:
			if(muRenderFlags & ResetTransforms)
			{
				mpkDirect3DDevice->SetTransform( D3DTS_VIEW, (D3DMATRIX*)&garrkTransforms[TS_VIEW] );
				mpkDirect3DDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)&garrkTransforms[TS_PROJECTION] );
				muRenderFlags &=~ResetTransforms;
			}
			mpkDirect3DDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
			guCurMaterialFlags = UnsetFlag;
			cDirectX9MtlStub::spkCurShader = 0;
			guTotalStateChanges += 3;
			break;
		case RSC_STENCIL_FILL_WITH_ALPHA:
			if( geCurMode != RSC_STENCIL_FILL_WITHOUT_ALPHA )
				garrpkStates[SB_STENCIL_FILL]->Apply();
			garrpkStates[SB_STENCIL_FILL_WITH_ALPHA]->Apply();
			muRenderFlags |= ResetTransforms;
			guTotalStateChanges += 23;
			break;
		case RSC_STENCIL_FILL_WITHOUT_ALPHA:
			if( geCurMode != RSC_STENCIL_FILL_WITH_ALPHA )
				garrpkStates[SB_STENCIL_FILL]->Apply();
			garrpkStates[SB_STENCIL_FILL_WITHOUT_ALPHA]->Apply();
			muRenderFlags |= ResetTransforms;
			guTotalStateChanges += 20;
			break;
		case RSC_SHADOW_VOLUME_ZPASS:
			if( geCurMode != RSC_SHADOW_VOLUME_ZFAIL )
				garrpkStates[SB_SHADOW_MODE]->Apply();
			garrpkStates[SB_SHADOW_VOLUME_ZPASS]->Apply();
			guTotalStateChanges += 19;
			break;
		case RSC_SHADOW_VOLUME_ZFAIL:
			if( geCurMode != RSC_SHADOW_VOLUME_ZPASS )
				garrpkStates[SB_SHADOW_MODE]->Apply();
			garrpkStates[SB_SHADOW_VOLUME_ZFAIL]->Apply();
			guTotalStateChanges += 20;
			break;
		}
		
		geCurMode = mode;
	}
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::setViewport( const rect2_x &rkRc )
{
	if( mkViewport != rkRc )
	{
		rect2_x kTemp = rkRc;
		rect2_x rc(0,0,mkScreenSize.dx,mkScreenSize.dy);
		kTemp.clip(rc);
		HRESULT hr = -1;
		if( kTemp.height() > 0 && kTemp.width() > 0 )
		{
			D3DVIEWPORT9 kViewPort;
			kViewPort.X = kTemp.left;
			kViewPort.Y = kTemp.top;
			kViewPort.Width = kTemp.width();
			kViewPort.Height = kTemp.height();
			kViewPort.MinZ = 0.0f;
			kViewPort.MaxZ = 1.0f;
			hr = mpkDirect3DDevice->SetViewport(&kViewPort);
		}
			if(FAILED(hr))	
			{
				X_SharedPtr(iSystem)->getLogger().print( LL_INFO,
					"Renderer:setViewport := viewport could not be set!");
			}
			else
				mkViewport = kTemp;
	}
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::setScreenSize( const size2_x& rkSc , u32 uBits )
{
	if( rkSc == mkScreenSize && uBits == muBitsPerPixel )
		return;
	if( uBits == -1 ) uBits = muBitsPerPixel;
	if( muRenderFlags & iRenderer::FullScreen )
	{
		mkScreenSize = rkSc;
		muBitsPerPixel = uBits;
		mkPresentParams.BackBufferHeight = mkScreenSize.dy;
		mkPresentParams.BackBufferWidth  = mkScreenSize.dx;
		mkPresentParams.BackBufferFormat = uBits == 32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5; /* w/f */
		reset();
	}
	else
	{
		_onResize(rkSc);
	}
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::setTexture( s32 iStage, iTexture *pkTex )
{
	if(iStage<N3DMaxTextureStage && garrpkCurTextures[iStage]!=pkTex)
	{
		guTotalStateChanges++;
		if( pkTex )
		{
			garrpkCurTextures[iStage] = pkTex;
			mpkDirect3DDevice->SetTexture(iStage,
			((cDirectX9Texture*)pkTex)->getTextureInterface());
		}
		else
		{
			garrpkCurTextures[iStage] = 0;
			mpkDirect3DDevice->SetTexture(iStage,0);
		}
	}
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::setVertexShader( VERTEXTYPE eType )
{
	if( geCurVertexShader == eType ) return;
	guTotalStateChanges++;
		
	switch(eType)
	{
	case VERTEX_2D:
		mpkDirect3DDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		break;
	case VERTEX_STD:
		mpkDirect3DDevice->SetFVF( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		break;
	case VERTEX_2TEX:
		mpkDirect3DDevice->SetFVF( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2 );
		break;
	case VERTEX_TAN:
		mpkDirect3DDevice->SetFVF( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE3(1) | D3DFVF_TEXCOORDSIZE3(2) );
		break;
	}

	geCurVertexShader = eType;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::setWorld( const f32* pfMat )
{
	muRenderFlags |= ResetTransforms;
	mpkDirect3DDevice->SetTransform( D3DTS_WORLD, (const D3DMATRIX*)(pfMat) );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::setAmbientLight( s32 uColor )
{
	mpkDirect3DDevice->SetRenderState( D3DRS_AMBIENT , (u32)uColor );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::addDynamicLight( const cLight *pkLight )
{
	X_Assert( guCurLightIdx < gkDeviceCaps.MaxActiveLights );

	if( pkLight->meType == cLight::Point )
	{
		gkLight.Type = D3DLIGHT_POINT;
		gkLight.Position.x = pkLight->mkVector[0];
		gkLight.Position.y = pkLight->mkVector[1];
		gkLight.Position.z = pkLight->mkVector[2];
	}
	else
	{
		gkLight.Type = D3DLIGHT_DIRECTIONAL;
		gkLight.Direction.x = pkLight->mkVector[0];
		gkLight.Direction.y = pkLight->mkVector[1];
		gkLight.Direction.z = pkLight->mkVector[2];
		gkLight.Attenuation1 = 1.0f / pkLight->mfRadius;
	}
	gkLight.Diffuse = *(D3DCOLORVALUE*)(const_cast<cColor4*>(&pkLight->mkDiffuse));
	gkLight.Ambient = *(D3DCOLORVALUE*)(const_cast<cColor4*>(&pkLight->mkAmbient));
	gkLight.Specular = *(D3DCOLORVALUE*)(const_cast<cColor4*>(&pkLight->mkSpecular));

	mpkDirect3DDevice->SetLight( guCurLightIdx, &gkLight );
	gpkAddedLights[guCurLightIdx] = pkLight;
	if( guCurLightIdx >= guLastLightSettings )
		mpkDirect3DDevice->LightEnable( guCurLightIdx, TRUE );
	guCurLightIdx++;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::finalizeLights()
{
	/* disable rest of the lights */
	for( u32 i = guCurLightIdx; i < guLastLightSettings; i++ )
		mpkDirect3DDevice->LightEnable( i, FALSE );

	guLastLightSettings = guCurLightIdx;
	guCurLightIdx = 0;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const cLight* cDirectX9Renderer::findAddedLight( s32 idx )
{
	if( (u32)idx < guLastLightSettings )
		return gpkAddedLights[idx];
	return 0;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::reset()
{
	if( FAILED( mpkDirect3DDevice->Reset(&mkPresentParams) ) ) 
	{
		ReportBug("Resetting device failed!","iRenderer::reset");
		return;
	}
	
	/* 	MAX_TEXTURE_PER_MATERIAL */
	garrpkCurTextures[0] = garrpkCurTextures[1] = 0;
	mpkDirect3DDevice->SetTexture(0,0);
	mpkDirect3DDevice->SetTexture(1,0);
	gpkCurShader = 0;
	muRenderFlags |= (ResetTransforms);
	guCurMaterialFlags = iMaterial::UnsetFlag;
	cDirectX9MtlStub::spkCurShader = 0;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::onSwitchMode( bool bFullScreen )
{
	if( bFullScreen != (muRenderFlags&iRenderer::FullScreen) )
	{
		mkPresentParams.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;
		if(bFullScreen)
		{
			muRenderFlags |= iRenderer::FullScreen;
			mkPresentParams.Windowed = FALSE;
			mkPresentParams.SwapEffect = D3DSWAPEFFECT_FLIP;
			mkPresentParams.BackBufferHeight			= mkScreenSize.dy;
			mkPresentParams.BackBufferWidth				= mkScreenSize.dx;
			mkPresentParams.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
			mkPresentParams.BackBufferCount				= 0;
			if( muRenderFlags & iRenderer::VSync )
				mkPresentParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		}
		else
		{
			muRenderFlags &=~iRenderer::FullScreen;
			mkPresentParams.Windowed = TRUE;
			mkPresentParams.SwapEffect = D3DSWAPEFFECT_COPY;
			mkPresentParams.BackBufferHeight = 0;
			mkPresentParams.BackBufferWidth = 0;
			mkPresentParams.FullScreen_RefreshRateInHz	= 0;
		}

		mkPresentParams.BackBufferFormat = 
			muBitsPerPixel == 32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5;
		reset();
	}
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::clearZBuffer()
{
	mpkDirect3DDevice->Clear( 0, 0, D3DCLEAR_ZBUFFER, 0, 1.0, 0);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::beginScene( bool bClearBack, bool bClearZbuff )
{
	guTotalStateChanges = 0;
	guTotalPolysRendered = 0;
	
	HRESULT hr = -1;
	DWORD flag;

	flag = bClearBack ? D3DCLEAR_TARGET : 0 ;
	if(bClearZbuff) flag |= D3DCLEAR_ZBUFFER;
//	if(muRenderFlags & iRenderer::StencilBuffer) 
//		flag |= D3DCLEAR_STENCIL;//TODO: check why this doesnt work!!
		
	if( muRenderFlags & DeviceLost )
	{
		if( FAILED(hr = mpkDirect3DDevice->TestCooperativeLevel()) )
		{
			if( hr == D3DERR_DEVICELOST )
				return;
			if( hr == D3DERR_DEVICENOTRESET )
				reset();
		}
	}
		
#ifdef X_DEBUG
	hr = mpkDirect3DDevice->Clear( 0, 0, flag, guBackColor, 1.0, 0);
	if( FAILED(hr) )
		X_SharedPtr(iSystem)->getLogger().print(LL_INFO,"Renderer:Clear Failed");
	hr = mpkDirect3DDevice->BeginScene();
	if( FAILED(hr) )
		X_SharedPtr(iSystem)->getLogger().print(LL_INFO,"Renderer:Begin Scene Failed");
#else
	mpkDirect3DDevice->Clear( 0, 0, flag, guBackColor, 1.0, 0);
	mpkDirect3DDevice->BeginScene();
#endif
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::endScene()
{
	if( !(muRenderFlags & DeviceLost) )
	{
		HRESULT hr = mpkDirect3DDevice->EndScene();
		
		if (FAILED(hr))
		{
			XDEBUG_LOG(LL_INFO,"Renderer:End Scene Failed");
			return;
		}
			
		hr = mpkDirect3DDevice->Present( 0, 0, 0, 0);
			
		if(hr == D3DERR_DEVICELOST)
		{
			muRenderFlags |= DeviceLost;
			XDEBUG_LOG(LL_INFO,"Renderer:Device Lost");
		}
		else if (FAILED(hr) && hr != D3DERR_INVALIDCALL)
			XDEBUG_LOG(LL_INFO,"Renderer:Present Failed");
	}
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool cDirectX9Renderer::isFeatureSupported( FEATURE3D f ) 
{
	switch(f)
	{
	case FS_RENDER_TO_MULTIPLE_TEXTURE:
		return (gkDeviceCaps.NumSimultaneousRTs > 1);
	case FS_HARDWARE_TL:
		return (gkDeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT);
	case FS_MIP_MAPS:
		return (gkDeviceCaps.TextureCaps & D3DPTEXTURECAPS_MIPMAP);
	case FS_AUTOGEN_MIP_MAPS:
		return (gkDeviceCaps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP);
	case FS_STENCIL_BUFFER:
		return ((muRenderFlags & iRenderer::StencilBuffer) &&  gkDeviceCaps.StencilCaps);
	case FS_ANY_SIZE_TEXTURE:
		return (gkDeviceCaps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL);
	case FS_INDEXED_VERTEX_BLENDING:
		return (gkDeviceCaps.MaxVertexBlendMatrixIndex);
		// pixel shader
	case FS_PS_1_1:
		return gkDeviceCaps.PixelShaderVersion >= D3DPS_VERSION(1,1);
	case FS_PS_1_2:
		return gkDeviceCaps.PixelShaderVersion >= D3DPS_VERSION(1,2);
	case FS_PS_1_3:
		return gkDeviceCaps.PixelShaderVersion >= D3DPS_VERSION(1,3);
	case FS_PS_1_4:
		return gkDeviceCaps.PixelShaderVersion >= D3DPS_VERSION(1,4);
	case FS_PS_2_0:
		return gkDeviceCaps.PixelShaderVersion >= D3DPS_VERSION(2,0);
	case FS_PS_3_0:
		return gkDeviceCaps.PixelShaderVersion >= D3DPS_VERSION(3,0);
		// vertex shader
	case FS_VS_1_1:
		return gkDeviceCaps.VertexShaderVersion >= D3DVS_VERSION(1,1);
	case FS_VS_2_0:
		return gkDeviceCaps.VertexShaderVersion >= D3DVS_VERSION(2,0);
	case FS_VS_3_0:
		return gkDeviceCaps.VertexShaderVersion >= D3DVS_VERSION(3,0);
	}
	return 0;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
iXuiFont* cDirectX9Renderer::createFont()
{
	return new cDirectX9Font();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
iTexture* cDirectX9Renderer::createTexture( cSoftwareTexture *pkImage, u32 flags )
{
	return new cDirectX9Texture( pkImage, flags );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
iTexture* cDirectX9Renderer::createTexture( const s32 iWidth, const s32 iHeight, u32 flags )
{
	return new cDirectX9Texture( iWidth, iHeight, flags );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
iTexture* cDirectX9Renderer::createRenderTarget( const s32 iWidth, const s32 iHeight )
{
	return new cDirectX9Texture( iWidth, iHeight );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::drawIndexedTriangleList( void *pV, u16 *pI, u32 uVert, u32 uPrim, u32 uSizeOfVert )
{
	mpkDirect3DDevice->DrawIndexedPrimitiveUP( 
		D3DPT_TRIANGLELIST, 0, uVert, uPrim, pI, D3DFMT_INDEX16, pV, uSizeOfVert );
	guTotalPolysRendered += uPrim;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::drawIndexedTriangleFan( void *pV, u16 *pI, u32 uVert, u32 uPrim, u32 uSizeOfVert )
{
	mpkDirect3DDevice->DrawIndexedPrimitiveUP( 
		D3DPT_TRIANGLEFAN, 0, uVert, uPrim, pI, D3DFMT_INDEX16, pV, uSizeOfVert );
	guTotalPolysRendered += uPrim;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::draw2DImage(iTexture *pkTex,const float *rkSrc,const rect2_x &rkDest,
									s32 iColor)
{
	//RENDER_TARGET_MODIFY
#ifdef X_DEBUG
	if( !rkDest.width() || !rkDest.height() ) 
	{
		XDEBUG_LOG(LL_INFO,"cDirectX9Renderer::draw2DImage := Rendering nothing!");
		return;
	}
#endif

	setMode( RSC_2D );	
	setTexture( 0, pkTex );
	
	gk2DVertices[0].mkPosition.x = gk2DVertices[1].mkPosition.x = 
		(f32)(rkDest.left);
	gk2DVertices[2].mkPosition.x = gk2DVertices[3].mkPosition.x = 
		(f32)(rkDest.right);
	gk2DVertices[1].mkPosition.y = gk2DVertices[3].mkPosition.y = 
		(f32)(rkDest.top); 
	gk2DVertices[0].mkPosition.y = gk2DVertices[2].mkPosition.y = 
		(f32)(rkDest.bottom); 	
	gk2DVertices[0].miColor = gk2DVertices[1].miColor = 
	gk2DVertices[2].miColor = gk2DVertices[3].miColor = iColor;
	gk2DVertices[0].mkTexCoord.x = gk2DVertices[1].mkTexCoord.x = 
		rkSrc[0];
	gk2DVertices[2].mkTexCoord.x = gk2DVertices[3].mkTexCoord.x = 
		rkSrc[1];
	gk2DVertices[1].mkTexCoord.y = gk2DVertices[3].mkTexCoord.y = 
		rkSrc[2];
	gk2DVertices[0].mkTexCoord.y = gk2DVertices[2].mkTexCoord.y = 
		rkSrc[3];
	
	if( geCurVertexShader != VERTEX_2D )
		setVertexShader(VERTEX_2D);
	
	mpkDirect3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, gk2DVertices, sizeof( cDxVertex2D ) );
	guTotalPolysRendered += 2;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::draw2DPoly( const coord2_x &rkCenter, const f32 fRadius, 
								   s32 iColOuter, s32 iColCenter)
{
	setMode(RSC_2D);		
	if(garrpkCurTextures[0])
	{
		garrpkCurTextures[0] = 0;
		mpkDirect3DDevice->SetTexture( 0, 0 );
	}
	f32 ang = N3D2Pi / (f32)(MAX_2DPOLY_VERT-1);
	gk2DVertices[0].mkPosition.x = (f32)(rkCenter.x);
	gk2DVertices[0].mkPosition.y = (f32)(rkCenter.y);
	gk2DVertices[0].mkPosition.z = 0;
	gk2DVertices[0].miColor = iColCenter;
		
	for( s32 i = 1; i < MAX_2DPOLY_VERT; i++ )
	{
		gk2DVertices[i].mkPosition.x = (fRadius*cosr(ang*(i-1)));
		gk2DVertices[i].mkPosition.y = (fRadius*sinr(ang*(i-1)));
		gk2DVertices[i].mkPosition.x = 0;
		gk2DVertices[i].miColor = iColOuter;
	}
		
	if( geCurVertexShader != VERTEX_2D )
		setVertexShader(VERTEX_2D);
		
	mpkDirect3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, (UINT)(MAX_2DPOLY_VERT-1), gk2DVertices, sizeof( cDxVertex2D ) );
	guTotalPolysRendered += (MAX_2DPOLY_VERT-1);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::draw2DTri( s32 iColor, const coord2_x& kP1, const coord2_x& kP2, const coord2_x& kP3 )
{
	setMode(RSC_2D);
	if(garrpkCurTextures[0])
	{
		garrpkCurTextures[0] = 0;
		mpkDirect3DDevice->SetTexture( 0, 0 );
	}
	gk2DVertices[0].mkPosition.x = f32(kP1.x);
	gk2DVertices[1].mkPosition.x = f32(kP2.x);
	gk2DVertices[2].mkPosition.x = f32(kP3.x);
	gk2DVertices[0].mkPosition.y = f32(kP1.y);
	gk2DVertices[1].mkPosition.y = f32(kP2.y);
	gk2DVertices[2].mkPosition.y = f32(kP3.y);
		
	gk2DVertices[0].miColor = gk2DVertices[1].miColor = gk2DVertices[2].miColor = gk2DVertices[3].miColor = iColor;
		
	if( geCurVertexShader != VERTEX_2D )
		setVertexShader(VERTEX_2D);
		
	mpkDirect3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 1, gk2DVertices, sizeof( cDxVertex2D ) );
	guTotalPolysRendered++;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::draw2DQuad( const rect2_x &rkRc, s32 iColTopLeft, s32 iColTopRight, s32 iColBotLeft, s32 iColBotRight )
{
	setMode(RSC_2D);
	if(garrpkCurTextures[0])
	{
		garrpkCurTextures[0] = 0;
		mpkDirect3DDevice->SetTexture( 0, 0 );
	}

	mpkDirect3DDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
		
	gk2DVertices[0].mkPosition.x = gk2DVertices[1].mkPosition.x = (f32)(rkRc.left);
	gk2DVertices[2].mkPosition.x = gk2DVertices[3].mkPosition.x = (f32)(rkRc.right);
	gk2DVertices[1].mkPosition.y = gk2DVertices[3].mkPosition.y = (f32)(rkRc.top); 
	gk2DVertices[0].mkPosition.y = gk2DVertices[2].mkPosition.y = (f32)(rkRc.bottom); 
		
	gk2DVertices[0].miColor = iColBotLeft;
	gk2DVertices[1].miColor = iColTopLeft;
	gk2DVertices[2].miColor = iColBotRight;
	gk2DVertices[3].miColor = iColTopRight;
		
	if( geCurVertexShader != VERTEX_2D )
		setVertexShader(VERTEX_2D);
		
	mpkDirect3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, gk2DVertices, sizeof( cDxVertex2D ) );
	mpkDirect3DDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );
	guTotalPolysRendered += 2;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::draw2DQuad( s32 iColor, s32 left, s32 top, s32 right, s32 bottom )
{
	setMode(RSC_2D);
	if(garrpkCurTextures[0])
	{
		garrpkCurTextures[0] = 0;
		mpkDirect3DDevice->SetTexture( 0, 0 );
	}
	gk2DVertices[0].mkPosition.x=gk2DVertices[1].mkPosition.x=(f32)(left);
	gk2DVertices[2].mkPosition.x=gk2DVertices[3].mkPosition.x=(f32)(right);
	gk2DVertices[1].mkPosition.y=gk2DVertices[3].mkPosition.y=(f32)(top); 
	gk2DVertices[0].mkPosition.y=gk2DVertices[2].mkPosition.y=(f32)(bottom);
	
	gk2DVertices[0].miColor = gk2DVertices[1].miColor = 
	gk2DVertices[2].miColor = gk2DVertices[3].miColor = iColor;
		
	if( geCurVertexShader != VERTEX_2D )
		setVertexShader(VERTEX_2D);
		
	mpkDirect3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, gk2DVertices, sizeof( cDxVertex2D ) );
	guTotalPolysRendered += 2;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::draw2DRect( s32 color, s32 left, s32 top, s32 right, s32 bottom )
{
	setMode(RSC_2D);
	if(garrpkCurTextures[0])
	{
		garrpkCurTextures[0] = 0;
		mpkDirect3DDevice->SetTexture( 0, 0 );
	}
	gk2DVertices[0].mkPosition.x = 
	gk2DVertices[3].mkPosition.x = 
	gk2DVertices[4].mkPosition.x = (f32)(left);
	gk2DVertices[1].mkPosition.x = 
	gk2DVertices[2].mkPosition.x = (f32)(right);
	gk2DVertices[0].mkPosition.y = 
	gk2DVertices[1].mkPosition.y = 
	gk2DVertices[4].mkPosition.y = (f32)(top);
	gk2DVertices[2].mkPosition.y = 
	gk2DVertices[3].mkPosition.y = (f32)(bottom);
		
	gk2DVertices[0].miColor = gk2DVertices[1].miColor = 
	gk2DVertices[2].miColor = gk2DVertices[3].miColor = 
	gk2DVertices[4].miColor = color;
	
	if( geCurVertexShader != VERTEX_2D )
		setVertexShader(VERTEX_2D);
	
	mpkDirect3DDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 4, gk2DVertices, sizeof( cDxVertex2D ) );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::draw2DLine( coord2_x &start, coord2_x &end, s32 color ) 
{	
	setMode(RSC_2D);
	if(garrpkCurTextures[0])
	{
		garrpkCurTextures[0] = 0;
		mpkDirect3DDevice->SetTexture( 0, 0 );
	}
	//RENDER_TARGET_MODIFY
	s32 iXPlus = mkScreenSize.dx>>1;
	s32 iYPlus = mkScreenSize.dy>>1;
	f32	fXFact = 1.f/(f32)iXPlus;
	f32 fYFact = 1.f/(f32)iYPlus;
	iXPlus = -iXPlus;
	iYPlus = mkScreenSize.dy - iYPlus;
	
	gk2DVertices[0].mkPosition.x = (f32)( start.x );
	gk2DVertices[0].mkPosition.y = (f32)( start.y );
//	gk2DVertices[0].mkPosition.z = 0.0f;
	gk2DVertices[1].mkPosition.x = (f32)( end.x );
	gk2DVertices[1].mkPosition.y = (f32)( end.y );
//	gk2DVertices[1].mkPosition.z = 0.0f;
	gk2DVertices[0].miColor = gk2DVertices[1].miColor = color;
	
	if( geCurVertexShader != VERTEX_2D )
		setVertexShader(VERTEX_2D);
	
	mpkDirect3DDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 1, gk2DVertices, sizeof( cDxVertex2D ) );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::draw2DLineStrip(nextar::cDxVertex2D *pkV, s32 iCnt)
{
	if(garrpkCurTextures[0])
		garrpkCurTextures[0] = 0;
	mpkDirect3DDevice->SetTexture( 0, 0 );
	if( geCurVertexShader != VERTEX_2D )
		setVertexShader(VERTEX_2D);
	mpkDirect3DDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, iCnt, pkV, sizeof( cDxVertex2D ) );
	guTotalStateChanges += 2;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::draw2DTriFan(nextar::cDxVertex2D *pkV, s32 iCnt)
{
	if( geCurVertexShader != VERTEX_2D )
		setVertexShader(VERTEX_2D);
	// we are unsure about color at this point
	// so make gouraud shading available
//	mpkDirect3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	mpkDirect3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, iCnt, pkV, sizeof( cDxVertex2D ) );
	// return to normal shading
//	mpkDirect3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
	guTotalStateChanges += 1;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::draw2DTriStrip(nextar::cDxVertex2D *pkV, s32 iCnt)
{
	if( geCurVertexShader != VERTEX_2D )
		setVertexShader(VERTEX_2D);
	// we are unsure about color at this point
	// so make gouraud shading available
//	mpkDirect3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	mpkDirect3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, iCnt, pkV, sizeof( cDxVertex2D ) );
	// return to normal shading
//	mpkDirect3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
	guTotalStateChanges += 1;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef X_DEBUG
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::draw3DLine( const vec3_t &start, const vec3_t &end, s32 color )
{
	DWORD ltstate;
	iTexture* Tex = garrpkCurTextures[0];
	garrpkCurTextures[0] = 0;
	mpkDirect3DDevice->SetTexture( 0, 0 );
	setMode(RSC_3D);

	mpkDirect3DDevice->GetRenderState( D3DRS_LIGHTING, &ltstate );
	mpkDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	
	cVertexStd v[2];
	
	v[0].mkPosition = start;
	v[1].mkPosition = end;
	v[1].miColor = v[0].miColor = color;
	
	if( geCurVertexShader != VERTEX_STD )
		setVertexShader(VERTEX_STD);

	mpkDirect3DDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, v, sizeof( cVertexStd ) );
	mpkDirect3DDevice->SetRenderState( D3DRS_LIGHTING, ltstate );
	setTexture( 0, Tex );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::draw3DBone( const vec3_t &start, const vec3_t &end, s32 color )
{
	f32 diffx = end.x - start.x;
	f32 diffy = end.y - start.y;
	f32 diffz = end.z - start.z;

	f32 BoneLength = sqrtr(diffx*diffx + diffy*diffy + diffz*diffz);
	draw3DLine( start, end, color );
	draw3DSphere( start, BoneLength * 0.1f, color );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::draw3DSphere( const vec3_t &center, const f32 rad, s32 color )
{
	static cVertexStd v[33] = 
	{
		{ { 0.000000f, 1.000000f, 0.000000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ { 0.707107f, 0.707107f, 0.000000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ { 1.000000f,-0.000000f, 0.000000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ { 0.707107f,-0.707107f, 0.000000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ {-0.000000f,-1.000000f, 0.000000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ {-0.707107f,-0.707107f, 0.000000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ {-1.000000f, 0.000000f, 0.000000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ {-0.707107f, 0.707107f, 0.000000f }, {0.0f}, 0xffff0000, {0.0f} },

		{ { 0.000000f, 1.000000f, 0.000000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ { 0.500000f, 0.707107f, 0.500000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ { 0.707107f,-0.000000f, 0.707107f }, {0.0f}, 0xffff0000, {0.0f} },
		{ { 0.500000f,-0.707107f, 0.500000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ {-0.000000f,-1.000000f,-0.000000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ {-0.500000f,-0.707107f,-0.500000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ {-0.707107f, 0.000000f,-0.707107f }, {0.0f}, 0xffff0000, {0.0f} },
		{ {-0.500000f, 0.707107f,-0.500000f }, {0.0f}, 0xffff0000, {0.0f} },

		{ { 0.000000f, 1.000000f, 0.000000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ {-0.000000f, 0.707107f, 0.707107f }, {0.0f}, 0xffff0000, {0.0f} },
		{ {-0.000000f,-0.000000f, 1.000000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ {-0.000000f,-0.707107f, 0.707107f }, {0.0f}, 0xffff0000, {0.0f} },
		{ { 0.000000f,-1.000000f,-0.000000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ { 0.000000f,-0.707107f,-0.707107f }, {0.0f}, 0xffff0000, {0.0f} },
		{ { 0.000000f, 0.000000f,-1.000000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ { 0.000000f, 0.707107f,-0.707107f }, {0.0f}, 0xffff0000, {0.0f} },

		{ { 0.000000f, 1.000000f, 0.000000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ {-0.500000f, 0.707107f, 0.500000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ {-0.707107f,-0.000000f, 0.707107f }, {0.0f}, 0xffff0000, {0.0f} },
		{ {-0.500000f,-0.707107f, 0.500000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ { 0.000000f,-1.000000f,-0.000000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ { 0.500000f,-0.707107f,-0.500000f }, {0.0f}, 0xffff0000, {0.0f} },
		{ { 0.707107f, 0.000000f,-0.707107f }, {0.0f}, 0xffff0000, {0.0f} },
		{ { 0.500000f, 0.707107f,-0.500000f }, {0.0f}, 0xffff0000, {0.0f} },

		{ { 0.000000f, 1.000000f, 0.000000f }, {0.0f}, 0xffff0000, {0.0f} }
	};

	DWORD ltstate;
	mat44_t Matrix = {0};
	Matrix[3*4+0] = center.x;
	Matrix[3*4+1] = center.x;
	Matrix[3*4+2] = center.x;
	Matrix[0*4+0] = rad;
	Matrix[1*4+1] = rad;
	Matrix[2*4+2] = rad;

	iTexture* Tex = garrpkCurTextures[0];
	garrpkCurTextures[0] = 0;
	mpkDirect3DDevice->SetTexture( 0, 0 );
	setMode(RSC_3D);

	mpkDirect3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)(Matrix) );
	mpkDirect3DDevice->GetRenderState( D3DRS_LIGHTING, &ltstate );
	mpkDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	for( s32 i = 0; i < 33; i++ )
		v[i].miColor = color;

	if( geCurVertexShader != VERTEX_STD )
		setVertexShader(VERTEX_STD);

	mpkDirect3DDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 16, v, sizeof( cVertexStd ) );
	mpkDirect3DDevice->SetRenderState( D3DRS_LIGHTING, ltstate );
	setTexture( 0, Tex );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::draw3DBox( const vec3_t &box_max, const vec3_t &box_min, s32 color, bool wireframe )
{
	vec3_t v[8] = 
	{
		{ box_max.x, box_max.y, box_min.z },
		{ box_min.x, box_min.y, box_max.z },
		{ box_max.x, box_min.y, box_min.z },
		{ box_min.x, box_max.y, box_max.z },
		{ box_max.x, box_min.y, box_max.z },
		{ box_min.x, box_max.y, box_min.z },
		{ box_min.x, box_min.y, box_min.z },
		{ box_max.x, box_max.y, box_max.z }
	};
	cVertexStd vx[4];

	for( s32 i = 0; i < 8; i++ ) 
	{ 
		vx[i].mkPosition = v[i];
		vx[i].miColor = color;
	}
	
	u32			cull,ltstate;
	D3DMATRIX	World;
	
	setMode(RSC_3D);

	setTexture( 0 , 0 );
	mpkDirect3DDevice->GetRenderState( D3DRS_CULLMODE, &cull );
	mpkDirect3DDevice->GetRenderState( D3DRS_LIGHTING, &ltstate );
	mpkDirect3DDevice->GetTransform( D3DTS_WORLD, &World );
	
	mpkDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	mpkDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	setWorld( N3D4x4IdentityMatrix );
	
	setVertexShader( VERTEX_STD );
	
	//		3--------7
	//		/|		/|
	//	   5-|-----0 |
	//	   | 1_____| 4
	//	   |/	   |/
	//	   6-------2
	//
	
	if( wireframe )
	{
		u16 index[] = 
		{
			5,0, 0,2, 
			2,6, 6,5,
			3,7, 7,4,
			4,1, 1,3,
			7,0, 3,5,
			1,6, 4,2
		};
		
		mpkDirect3DDevice->DrawIndexedPrimitiveUP( D3DPT_LINELIST, 0, 8, 12, index, D3DFMT_INDEX16, vx, sizeof( cVertexStd ) );
	}
	else
	{
		DWORD alphastate;
		DWORD bs1, bs2;

		mpkDirect3DDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &alphastate ); 
		mpkDirect3DDevice->GetRenderState(D3DRS_SRCBLEND, &bs1 ); 
		mpkDirect3DDevice->GetRenderState(D3DRS_DESTBLEND, &bs2 ); 

		mpkDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE); 
		mpkDirect3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA); 
		mpkDirect3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ); 

		u16 index[36] = 
		{
			6,5,2, 5,0,2,
			2,0,4, 0,7,4, 
			4,7,1, 7,3,1, 
			1,3,6, 3,5,6,
			5,3,0, 3,7,0,
			1,6,2, 6,2,4
		};
		
		mpkDirect3DDevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 8, 12, index, D3DFMT_INDEX16, vx, sizeof( cVertexStd ) );

		mpkDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, alphastate ); 
		mpkDirect3DDevice->SetRenderState(D3DRS_SRCBLEND, bs1 ); 
		mpkDirect3DDevice->SetRenderState(D3DRS_DESTBLEND, bs2 );
	}

	mpkDirect3DDevice->SetTransform( D3DTS_WORLD, &World );
	mpkDirect3DDevice->SetRenderState( D3DRS_LIGHTING, ltstate );
	mpkDirect3DDevice->SetRenderState( D3DRS_CULLMODE, cull );
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::draw3DTriangle( const vec3_t &kP1, const vec3_t &kP2, const vec3_t &kP3, s32 color, bool wireframe )
{
	DWORD alphastate;
	DWORD bs1, bs2;

	cVertexStd v[3];
	v[0].mkPosition=kP1; 
	v[1].mkPosition=kP2; 
	v[2].mkPosition=kP3; 
	v[0].miColor = color; v[1].miColor = color; v[2].miColor = color;
	
	setMode(RSC_3D);
	
	u32 cull,ltstate;

	setTexture( 0 , 0 );

	mpkDirect3DDevice->GetRenderState( D3DRS_CULLMODE, &cull );
	mpkDirect3DDevice->GetRenderState( D3DRS_LIGHTING, &ltstate );

	mpkDirect3DDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &alphastate ); 
	mpkDirect3DDevice->GetRenderState(D3DRS_SRCBLEND, &bs1 ); 
	mpkDirect3DDevice->GetRenderState(D3DRS_DESTBLEND, &bs2 ); 

	
	mpkDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	mpkDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	mpkDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE); 
	mpkDirect3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA); 
	mpkDirect3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ); 

	
	setVertexShader( VERTEX_STD );
	
	if( wireframe )
	{
		u16 index[] = { 0,1, 1,2, 2,1 };
		mpkDirect3DDevice->DrawIndexedPrimitiveUP( D3DPT_LINELIST, 0, 3, 3, index, D3DFMT_INDEX16, v, sizeof( cVertexStd ) );
	}
	else
	{
		mpkDirect3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 1, v, sizeof( cVertexStd ) );
	}
	
	mpkDirect3DDevice->SetRenderState( D3DRS_LIGHTING, ltstate );
	mpkDirect3DDevice->SetRenderState( D3DRS_CULLMODE, cull );

	mpkDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, alphastate ); 
	mpkDirect3DDevice->SetRenderState(D3DRS_SRCBLEND, bs1 ); 
	mpkDirect3DDevice->SetRenderState(D3DRS_DESTBLEND, bs2 ); 
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::draw3DTransform( const f32* t, const f32 length )
{
	vec3_t x,y,z;
	vec3_t o = {t[12],t[13],t[14]};
#define scaleit(v,idx) \
	v.x = t[0+idx]*length + o.x;\
	v.y = t[4+idx]*length + o.y;\
	v.z = t[8+idx]*length + o.z;
	scaleit(x,0);
	scaleit(y,1);
	scaleit(z,2);
#undef  scaleit
	draw3DLine(o,x,0xffff0000);
	draw3DLine(o,y,0xff00ff00);
	draw3DLine(o,z,0xff0000ff);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::draw3DPlane( const plane_t &p, const vec3_t &pos, f32 size, s32 color )
{
	DWORD ltstate;
	DWORD cull;
	DWORD alphastate;
	DWORD bs1, bs2;
	
	setMode(RSC_3D);
	setTexture( 0 , 0 );
	
	mpkDirect3DDevice->GetRenderState( D3DRS_CULLMODE, &cull );
	mpkDirect3DDevice->GetRenderState( D3DRS_LIGHTING, &ltstate );

	mpkDirect3DDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &alphastate ); 
	mpkDirect3DDevice->GetRenderState(D3DRS_SRCBLEND, &bs1 ); 
	mpkDirect3DDevice->GetRenderState(D3DRS_DESTBLEND, &bs2 ); 


	mpkDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );	
	mpkDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	
	mpkDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE); 
	mpkDirect3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA); 
	mpkDirect3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ); 

	setVertexShader( VERTEX_STD );

	// draw here

	mpkDirect3DDevice->SetRenderState( D3DRS_LIGHTING, ltstate );
	mpkDirect3DDevice->SetRenderState( D3DRS_CULLMODE, cull );

	mpkDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, alphastate ); 
	mpkDirect3DDevice->SetRenderState(D3DRS_SRCBLEND, bs1 ); 
	mpkDirect3DDevice->SetRenderState(D3DRS_DESTBLEND, bs2 ); 
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::draw3DFrustum( const plane_t p[], s32 color )
{
	// TODO: need implementation
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#endif // X_DEBUG draws
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
XERRORCODE cDirectX9Renderer::_create(HWND kWnd)
{
	cLog& rkLog = X_SharedPtr(iSystem)->getLogger();
	if( 0 == ( mpkDirect3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return X_ERR_COULDNT_CREATE_D3D;
	
	if( muRenderFlags & iRenderer::FullScreen )
	{
		mkPresentParams.Windowed	= FALSE;	// w/f
		mkPresentParams.SwapEffect	= D3DSWAPEFFECT_FLIP; // w/f 
	}
	else
	{
		mkPresentParams.Windowed	= TRUE;
		mkPresentParams.SwapEffect	= D3DSWAPEFFECT_COPY;
	}

	mkPresentParams.EnableAutoDepthStencil	= TRUE;
	mkPresentParams.BackBufferFormat		= muBitsPerPixel == 32 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5; // w/f 
	mkPresentParams.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;
	
	if( !mkPresentParams.Windowed ) // w/f 
	{
		mkPresentParams.BackBufferHeight			= mkScreenSize.dy;
		mkPresentParams.BackBufferWidth				= mkScreenSize.dx;
		mkPresentParams.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
		mkPresentParams.BackBufferCount				= 0;
		
		if( muRenderFlags & iRenderer::VSync )	// w/f 
			mkPresentParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		
	}
	
	D3DDEVTYPE eDevType = D3DDEVTYPE_HAL;
	
	// antialias enabled
	if( muRenderFlags & iRenderer::AntiAlias )
	{
		DWORD uQualityLevels = 0; 
		
		if ( !FAILED( mpkDirect3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, eDevType, mkPresentParams.BackBufferFormat, mkPresentParams.Windowed, D3DMULTISAMPLE_2_SAMPLES, &uQualityLevels)))
		{        
			// enable multi sampling
			mkPresentParams.SwapEffect         = D3DSWAPEFFECT_DISCARD; 
			mkPresentParams.MultiSampleType    = D3DMULTISAMPLE_2_SAMPLES; 
			mkPresentParams.MultiSampleQuality = uQualityLevels-1;
		} 
		else if ( !FAILED( mpkDirect3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, 
			eDevType, mkPresentParams.BackBufferFormat, mkPresentParams.Windowed, D3DMULTISAMPLE_NONMASKABLE, &uQualityLevels))) 
		{        
			// enable non maskable multi sampling
			mkPresentParams.SwapEffect         = D3DSWAPEFFECT_DISCARD; 
			mkPresentParams.MultiSampleType    = D3DMULTISAMPLE_NONMASKABLE; 
			mkPresentParams.MultiSampleQuality = uQualityLevels-1;
		} 		
		else 
		{ 
			rkLog.print(LL_INFO,"Renderer:Anti aliasing disabled because hardware/driver support was not found.");
			muRenderFlags &= ~iRenderer::AntiAlias;
		}
	}
	
	// stencil buffer compatibility test
	if ( muRenderFlags & iRenderer::StencilBuffer )
	{
		mkPresentParams.AutoDepthStencilFormat = D3DFMT_D24S8;

		if( FAILED( mpkDirect3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, eDevType, mkPresentParams.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8 ) ) )
		{
			rkLog.print(LL_INFO,"Renderer:Device does not support stencilbuffer, disabling stencil buffer.");
			muRenderFlags &= ~iRenderer::StencilBuffer;
		}
		else if( FAILED( mpkDirect3D->CheckDepthStencilMatch( D3DADAPTER_DEFAULT, eDevType, mkPresentParams.BackBufferFormat, mkPresentParams.BackBufferFormat, D3DFMT_D24S8) ) )
		{
			rkLog.print(LL_INFO,"Renderer:Depth-stencil format is not compatible with display format, disabling stencil buffer.");
			muRenderFlags &= ~iRenderer::StencilBuffer;
		}
	}
	
	if ( muRenderFlags & iRenderer::StencilBuffer )
		mkPresentParams.AutoDepthStencilFormat = D3DFMT_D16;

	// create the device 
	muRenderFlags |= iRenderer::HardwareVP;
	if( FAILED( mpkDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, kWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &mkPresentParams, &mpkDirect3DDevice ) ) )
	{
		if( FAILED( mpkDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, kWnd, D3DCREATE_MIXED_VERTEXPROCESSING, &mkPresentParams, &mpkDirect3DDevice ) ) )
		{
			muRenderFlags &=~iRenderer::HardwareVP;
			mpkDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, kWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &mkPresentParams, &mpkDirect3DDevice );
		}
	}

	if( !mpkDirect3DDevice )
		return X_ERR_COULDNT_CREATE_D3DDEV;

	mpkDirect3DDevice->GetDeviceCaps( &gkDeviceCaps );
	
	// disable stencilbuffer if necessary 
	if ((muRenderFlags & iRenderer::StencilBuffer) && 
		(
		!( gkDeviceCaps.StencilCaps & D3DSTENCILCAPS_DECRSAT) ||
		!( gkDeviceCaps.StencilCaps & D3DSTENCILCAPS_INCRSAT) ||
		!( gkDeviceCaps.StencilCaps & D3DSTENCILCAPS_KEEP)
		)
		)
	{
		rkLog.print(LL_INFO,"Renderer:Disabling stencil buffer. No device support found.");
		muRenderFlags &= ~iRenderer::StencilBuffer;
	}

	if (muRenderFlags & iRenderer::AntiAlias)
		mpkDirect3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	
	// set maximal anisotropy 
	mpkDirect3DDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, __min( 16, gkDeviceCaps.MaxAnisotropy ) );
	mpkDirect3DDevice->SetSamplerState(1, D3DSAMP_MAXANISOTROPY, __min( 16, gkDeviceCaps.MaxAnisotropy ) );
	
	// get device information 
	D3DADAPTER_IDENTIFIER9 dai;
	if ( !FAILED( mpkDirect3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &dai)) )
	{
		gkDriverName.format(("%s %s %s"), 196, dai.Description, dai.Driver, 
			muRenderFlags&iRenderer::HardwareVP? ("(hw vp)"):("(sw vp)"));
	}

	guTotalStateChanges = 0;
	guTotalPolysRendered = 0;
	_recordStates();
	setMaterialStates(0);

	s32 iMaxDynLight = getMaxDynamicLight();
	X_Assert(iMaxDynLight);
	
	gpkAddedLights = new const cLight*[iMaxDynLight];
	zeroMemory( gpkAddedLights, iMaxDynLight*sizeof(cLight*) );
	return X_SUCCESS;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::_onResize( const size2_x &rkSize )
{
	if( mkScreenSize != rkSize )
	{
		mkScreenSize = rkSize;
		mkViewport.right = mkScreenSize.dx;
		mkViewport.bottom = mkScreenSize.dy;
		setViewport(mkViewport);
		reset();
	}
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::_recordStates()
{
#define DX9BugRS(str) ReportBug(str,"cDirectX9Renderer:_recordState")
	// basic 2d state block
	if( FAILED(mpkDirect3DDevice->BeginStateBlock()) )
	{
		DX9BugRS("Failed to record state blocks.");
		return;
	}
	
	//** TRANSFORM CHANGES
//	mpkDirect3DDevice->SetTransform( D3DTS_VIEW, (D3DMATRIX*)(N3D4x4IdentityMatrix) );
//	mpkDirect3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)(N3D4x4IdentityMatrix) );
//	mpkDirect3DDevice->SetTransform( D3DTS_PROJECTION, ( D3DMATRIX*)(N3D4x4IdentityMatrix) );

	mpkDirect3DDevice->SetRenderState( D3DRS_ZENABLE, FALSE);
	mpkDirect3DDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT);
	mpkDirect3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
	mpkDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
	mpkDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE);
	mpkDirect3DDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE);
	
	mpkDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	mpkDirect3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,  D3DTOP_DISABLE );
	
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
	mpkDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW);
	
	mpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	mpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	mpkDirect3DDevice->SetTransform( D3DTS_TEXTURE0, ( D3DMATRIX*)(N3D4x4IdentityMatrix) );
	
	mpkDirect3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	mpkDirect3DDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	mpkDirect3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	
	mpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	mpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	mpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	
	mpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,  D3DTOP_MODULATE );
	mpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	mpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	
	mpkDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
	mpkDirect3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA); 
	mpkDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ); 
	
	if( FAILED(mpkDirect3DDevice->EndStateBlock(&garrpkStates[SB_BASIC2D]) ) )
	{
		DX9BugRS("Failed to record state blocks. [Block: SB_BASIC2D]");
		return;
	}
	
	// basic shadow mode state block
	if( FAILED(mpkDirect3DDevice->BeginStateBlock()) )
	{
		DX9BugRS("Failed to record state blocks.");
		return;
	}
	
	mpkDirect3DDevice->SetTexture(0,0);
	mpkDirect3DDevice->SetTexture(1,0);
	
	mpkDirect3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	mpkDirect3DDevice->SetTextureStageState(1,  D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	mpkDirect3DDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
	mpkDirect3DDevice->SetTextureStageState(2,  D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	
	mpkDirect3DDevice->SetFVF(D3DFVF_XYZ);
	
	mpkDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE,  FALSE );
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );
	mpkDirect3DDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT);
	
	if( FAILED(mpkDirect3DDevice->EndStateBlock(&garrpkStates[SB_SHADOW_MODE])) )
	{
		DX9BugRS("Failed to record state blocks. [Block: SB_SHADOW_MODE]");
		return;
	}
	
	// shadow_volume_zpass state block
	if( FAILED(mpkDirect3DDevice->BeginStateBlock()) )
	{
		DX9BugRS("Failed to record state blocks.");
		return;
	}
	
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILFUNC,  D3DCMP_ALWAYS );
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILREF, 0x1 );
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILMASK, 0xffffffff );
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );
	mpkDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
	mpkDirect3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
	mpkDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	
	if( FAILED(mpkDirect3DDevice->EndStateBlock(&garrpkStates[SB_SHADOW_VOLUME_ZPASS])) )
	{
		DX9BugRS("Failed to record state blocks.[Block: SB_SHADOW_VOLUME_ZPASS]");
		return;
	}
	
	// shadow_volume_zfail state block
	if( FAILED(mpkDirect3DDevice->BeginStateBlock()) )
	{
		DX9BugRS("Failed to record state blocks.");
		return;
	}
	
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
	
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILREF, 0x0 );
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILMASK, 0xffffffff );
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );
	
	mpkDirect3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	mpkDirect3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
	mpkDirect3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	
	if( FAILED(mpkDirect3DDevice->EndStateBlock(&garrpkStates[SB_SHADOW_VOLUME_ZFAIL])) )
	{
		DX9BugRS("Failed to record state blocks. [Block: SB_SHADOW_VOLUME_ZFAIL]");
		return;
	}
	
	// stencil fill state block
	if( FAILED(mpkDirect3DDevice->BeginStateBlock()) )
	{
		DX9BugRS("Failed to record state blocks.");
		return;
	}
	
	//** TRANSFORM CHANGES
	mpkDirect3DDevice->SetTransform( D3DTS_VIEW, ( D3DMATRIX*)(N3D4x4IdentityMatrix) );
	mpkDirect3DDevice->SetTransform( D3DTS_WORLD, ( D3DMATRIX*)(N3D4x4IdentityMatrix) );
	mpkDirect3DDevice->SetTransform( D3DTS_PROJECTION, ( D3DMATRIX*)(N3D4x4IdentityMatrix) );
	mpkDirect3DDevice->SetRenderState( D3DRS_ZENABLE, FALSE);
	mpkDirect3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
	mpkDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, FALSE);
	mpkDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	mpkDirect3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,  D3DTOP_DISABLE );
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILREF, 0x1 );
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL );
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILMASK, 0xffffffff );
	mpkDirect3DDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );		
	mpkDirect3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	
	if( FAILED(mpkDirect3DDevice->EndStateBlock(&garrpkStates[SB_STENCIL_FILL])) )
	{
		DX9BugRS("Failed to record state blocks. [Block: SB_STENCIL_FILL]");
		return;
	}
	
	// stencil_fill_with_alpha state block
	if( FAILED(mpkDirect3DDevice->BeginStateBlock()) )
	{
		DX9BugRS("Failed to record state blocks.");
		return;
	}
	
	mpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	mpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	mpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	mpkDirect3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	mpkDirect3DDevice->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	mpkDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	mpkDirect3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	mpkDirect3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	
	if( FAILED(mpkDirect3DDevice->EndStateBlock(&garrpkStates[SB_STENCIL_FILL_WITH_ALPHA])) )
	{
		DX9BugRS("Failed to record state blocks. [Block: SB_STENCIL_FILL_WITH_ALPHA]");
		return;
	}
	
	// stencil_fill_without_alpha state block
	if( FAILED(mpkDirect3DDevice->BeginStateBlock()) )
	{
		DX9BugRS("Failed to record state blocks.");
		return;
	}
	
	mpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	mpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	mpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	mpkDirect3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	mpkDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	
	if( FAILED(mpkDirect3DDevice->EndStateBlock(&garrpkStates[SB_STENCIL_FILL_WITHOUT_ALPHA])) )
	{
		DX9BugRS("Failed to record state blocks. [Block: SB_STENCIL_FILL_WITHOUT_ALPHA]");
		return;
	}
#undef DX9BugRS
	return;

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool cDirectX9Renderer::setPSShaderConst( u32 startReg, const f32* b, s32 count )
{
	if(FAILED(mpkDirect3DDevice->SetPixelShaderConstantF(startReg,b,(UINT)count)))
		return false;
	return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool cDirectX9Renderer::setVSShaderConst( u32 startReg, const f32* b, s32 count )
{
	if(FAILED(mpkDirect3DDevice->SetVertexShaderConstantF(startReg,b,(UINT)count)))
		return true;
	return false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool cDirectX9Renderer::assembleLowLevelShader(const s8* sVSProg,const s8* sPSProg,
											   IDirect3DVertexShader9** pkVShader, 
											   IDirect3DPixelShader9 **pkPShader)
{
	// lets first do the vertex shader thing
#ifdef X_DEBUG
#	define D3DSDCOMPILE_FLAGS D3DXSHADER_DEBUG
#else
#	define D3DSDCOMPILE_FLAGS 0
#endif
	
#define D3DSCOMPILE_FLAGS D3DSDCOMPILE_FLAGS
#define DX9BugRS(str) ReportBug(str,"cDirectX9Renderer:assembleLowLevelShader")
	LPD3DXBUFFER pkProg = 0;
	HRESULT rVal = 0;
	
	if(sVSProg)
	{

		rVal = D3DXAssembleShader(sVSProg,(UINT)strlen(sVSProg),0,0,D3DSCOMPILE_FLAGS,&pkProg,0);

		if(FAILED(rVal))
		{	// we have errors
			DX9BugRS("Errors in vshader.");
			return false;
		}

		rVal = mpkDirect3DDevice->CreateVertexShader( (DWORD*)pkProg->GetBufferPointer(),pkVShader);

		if(FAILED(rVal))
			DX9BugRS("Vshader creation failed.");
			
		if(pkProg) pkProg->Release();

	}

	if(sPSProg)
	{

		rVal = D3DXAssembleShader(sPSProg,(UINT)strlen(sPSProg),0,0,D3DSCOMPILE_FLAGS,&pkProg,0);

		if(FAILED(rVal))
		{	// we have errors
			DX9BugRS("Errors in pshader.");
			return false;
		}

		rVal = mpkDirect3DDevice->CreatePixelShader( (DWORD*)pkProg->GetBufferPointer(),pkPShader);

		if(FAILED(rVal))
			DX9BugRS("Could not create pshader.");

		if(pkProg) pkProg->Release();

	}

	return true; // it doesnt matter neway
#undef D3DSDCOMPILE_FLAGS
#undef D3DSCOMPILE_FLAGS
#undef DX9BugRS
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool cDirectX9Renderer::compileHighLevelShader(const s8* sVSProg, const s8* sVSEntry, 
		const s8* sPSProg, const s8* sPSEntry, VSHADER_TARGET eVTar, PSHADER_TARGET ePTar, 
		IDirect3DVertexShader9** pkVShader, IDirect3DPixelShader9** pkPShader,
		void** pkVSConstTable,void** pkPSConstTable)
{
#define DX9BugRS(str) ReportBug(str,"cDirectX9Renderer:compileHighLevelShader")
	const s8* strVSProfile[] =
	{
		"vs_1_1",
		"vs_2_0",
		"vs_2_a",
		"vs_3_0",
		0
	};

	const s8* strPSProfile[] =
	{
		"ps_1_1", 
		"ps_1_2",
		"ps_1_3",
		"ps_1_4",
		"ps_2_0",
		"ps_2_a",
		"ps_2_b",
		"ps_3_0",
		0
	};

	// lets first do the vertex shader thing
#ifdef X_DEBUG
#	define D3DSDCOMPILE_FLAGS D3DXSHADER_DEBUG
#else
#	define D3DSDCOMPILE_FLAGS 0
#endif

	LPD3DXBUFFER pkProg = 0;
	HRESULT rVal = 0;

	if(sVSProg)
	{

		rVal = D3DXCompileShader(sVSProg,(UINT)strlen(sVSProg),0,0,sVSEntry,strVSProfile[eVTar],D3DSDCOMPILE_FLAGS,&pkProg,0,(LPD3DXCONSTANTTABLE*)pkVSConstTable);

		if(FAILED(rVal))
		{	// we have errors
			DX9BugRS("Errors in vshader.");
			return false;
		}

		rVal = mpkDirect3DDevice->CreateVertexShader( (DWORD*)pkProg->GetBufferPointer(),pkVShader);

		if(FAILED(rVal))
			DX9BugRS("Could not create vshader.");
		
		if(pkProg) pkProg->Release();

	}

	if(sPSProg)
	{

		rVal = D3DXCompileShader(sPSProg,(UINT)strlen(sPSProg),0,0,sPSEntry,strPSProfile[eVTar],D3DSDCOMPILE_FLAGS,&pkProg,0,(LPD3DXCONSTANTTABLE*)pkPSConstTable);

		if(FAILED(rVal))
		{	// we have errors
			DX9BugRS("Errors in pshader.");
			return false;
		}

		rVal = mpkDirect3DDevice->CreatePixelShader( (DWORD*)pkProg->GetBufferPointer(),pkPShader);

		if(FAILED(rVal))
			DX9BugRS("Could not create pshader.");

		if(pkProg) pkProg->Release();

	}

	return true; // it doesnt matter neway

#undef D3DSDCOMPILE_FLAGS
#undef D3DSCOMPILE_FLAGS
#undef DX9BugRS
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void cDirectX9Renderer::setMaterialStates( u32 flags )
{
	if(guCurMaterialFlags == iMaterial::UnsetFlag)
		guCurMaterialFlags = ~flags;

	register bool bTemp = false;
	if( (flags & iMaterial::BilinearFilter) && !(guCurMaterialFlags & iMaterial::BilinearFilter) )
	{
		bTemp = true;
		mpkDirect3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		mpkDirect3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		mpkDirect3DDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		mpkDirect3DDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

	}
	else if( (flags & iMaterial::AnisotropicFilter) && !(guCurMaterialFlags & iMaterial::AnisotropicFilter) )
	{
		bTemp = true;
		mpkDirect3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
		mpkDirect3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
		mpkDirect3DDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
		mpkDirect3DDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
	}

	if( (flags & iMaterial::TrilinearFilter) && !(guCurMaterialFlags & iMaterial::TrilinearFilter) )
	{
		bTemp = true;
		mpkDirect3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
		mpkDirect3DDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	}
	
	if( !bTemp )
	{
		mpkDirect3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		mpkDirect3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		mpkDirect3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
				
		mpkDirect3DDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		mpkDirect3DDevice->SetSamplerState(1, D3DSAMP_MIPFILTER,  D3DTEXF_NONE);
		mpkDirect3DDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	}
		
	// wireframe mode 
	if( (flags & iMaterial::DrawWireframe) )
	{
		if( !(guCurMaterialFlags & iMaterial::DrawWireframe)  )
			mpkDirect3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
	else
	{
		if( (guCurMaterialFlags & iMaterial::DrawWireframe) )
			mpkDirect3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
	
	// shade mode 
	if( (flags & iMaterial::GouraudShading) )
	{
		if( !(guCurMaterialFlags & iMaterial::GouraudShading) )
			mpkDirect3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	}
	else
	{
		if( (guCurMaterialFlags & iMaterial::GouraudShading) )
			mpkDirect3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
	}

	// lighting 
	bTemp = (flags & iMaterial::Lighting);
	if( bTemp != (guCurMaterialFlags & iMaterial::Lighting) )
		mpkDirect3DDevice->SetRenderState(D3DRS_LIGHTING, bTemp);
			
	// z buffer 
	bTemp = (flags & iMaterial::ZBuffer);
	if ( bTemp != (guCurMaterialFlags & iMaterial::ZBuffer ) )
		mpkDirect3DDevice->SetRenderState(D3DRS_ZENABLE, bTemp);

	// z write 
	bTemp = (flags & iMaterial::ZWrite);
	if ( bTemp != (guCurMaterialFlags & iMaterial::ZWrite) )
		mpkDirect3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, bTemp );
	
	// back face culling 
	if( (flags & iMaterial::Is2Sided) )
	{
		if( !(guCurMaterialFlags & iMaterial::Is2Sided) )
			mpkDirect3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	}
	else
	{
		if( (guCurMaterialFlags & iMaterial::Is2Sided) )
			mpkDirect3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}

	// fog 
	bTemp = (flags & iMaterial::FogEnable);
	if( bTemp != (guCurMaterialFlags & iMaterial::FogEnable) )
		mpkDirect3DDevice->SetRenderState( D3DRS_FOGENABLE, bTemp ); 
	
	// specular highlights 
	bTemp = (flags & iMaterial::SpecularEnable);
	if ( bTemp != (guCurMaterialFlags & iMaterial::SpecularEnable) )
	{
		mpkDirect3DDevice->SetRenderState( D3DRS_SPECULARENABLE, bTemp );
		mpkDirect3DDevice->SetRenderState( D3DRS_NORMALIZENORMALS, bTemp );
		mpkDirect3DDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL );
	}
	
	// normalization 
	bTemp = (flags & iMaterial::NormalizeNormals);
	if ( bTemp != (guCurMaterialFlags & iMaterial::NormalizeNormals) )
		mpkDirect3DDevice->SetRenderState( D3DRS_NORMALIZENORMALS, bTemp);

	guCurMaterialFlags = flags;
	guTotalStateChanges += 14; // approximately the max value 
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
iShader* cDirectX9Renderer::assembleLowLevelShader(const s8* sVSProg, const s8* sPSProg, 
		iSetShaderConstCallback* pkCallback, iShader* pkBase)
{
	/*
	   IDirect3DVertexShader9* pkVShader = 0; 
	   IDirect3DPixelShader9 * pkPShader = 0;
	   if( assembleLowLevelShader(sVSProg,sPSProg,&pkVShader,&pkPShader) )
	   {
		   // create the shader
		   return new cDirectX9LLShader(pkVShader,pkPShader,pkCallback,pkBase);
	   }
	   if(pkVShader) pkVShader->Release();
	   if(pkPShader) pkPShader->Release();
	   */
	   return 0;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
iShader* cDirectX9Renderer::compileHighLevelShader( const s8* sVSProg, const s8* sVSEntry, 
		const s8* sPSProg, const s8* sPSEntry, VSHADER_TARGET eVT, PSHADER_TARGET ePT, 
		iSetShaderConstCallback* pkCallback, iShader* pkBase )
{
	/*
	   IDirect3DVertexShader9*	pkVShader = 0; 
	   IDirect3DPixelShader9 *	pkPShader = 0;
	   ID3DXConstantTable*		pkVTbl = 0;
	   ID3DXConstantTable*		pkPTbl = 0;

	   if( compileHighLevelShader(sVSProg,sVSEntry,sPSProg,sPSEntry,eVT,ePT,&pkVShader,&pkPShader,(void**)&pkVTbl,(void**)&pkPTbl) )
	   {
		   // create the shader
		   return new cDirectX9HLShader(pkVShader,pkPShader,pkCallback,pkBase,pkVTbl,pkPTbl);
	   }

	   if(pkVShader) pkVShader->Release();
	   if(pkPShader) pkPShader->Release();
	   if(pkVTbl)	 pkVTbl->Release();
	   if(pkPTbl)	 pkPTbl->Release();
	   */
	   return 0;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//@--------------------------------------------------------------------------

