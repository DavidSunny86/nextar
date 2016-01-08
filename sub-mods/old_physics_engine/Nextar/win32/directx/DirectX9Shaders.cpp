
/* created : feb 17, 07 */
/* by : Abhishek Dey */

#include "DirectX9Shaders.h"
#include <d3d9.h>
#include <xmath_c.h>

namespace nextar
{

/************************************************************/
extern IDirect3D9*						gpkDirect3D;
extern IDirect3DDevice9*				gpkDirect3DDevice;
/************************************************************/

/************************SOLID SHADER************************/
cDirectX9SolidShader::cDirectX9SolidShader()
{
	XREFOBJECT(cDirectX9SolidShader);
}

void cDirectX9SolidShader::set( cMaterial*, bool reset  )
{
	if(reset)
	{
		gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	}
}

cDirectX9SolidShader::~cDirectX9SolidShader()
{
/*	reinterpret_cast<IDirect3DStateBlock9*>(mpvStateBlock)->Release(); */
}
/************************************************************/

/**********************LIGHT MAP*****************************/
cDirectX9LightMap::cDirectX9LightMap( iShader::STDSHADERS eType )
{
	XREFOBJECT(cDirectX9LightMap);
	X_BREAK_IF( eType != iShader::SHADER_LIGHTMAP && 
		eType != iShader::SHADER_LIGHTMAP2X && 
		eType != iShader::SHADER_LIGHTMAP4X && 	
		eType != iShader::SHADER_LIGHTMAPDYNAMIC );

	IDirect3DStateBlock9* pkBlock = 0;
	
	if( FAILED(gpkDirect3DDevice->BeginStateBlock()) )
	{
		ReportBug("Failed to record state blocks.B","cDirectX9LightMap");
		return;
	}

	if( eType == iShader::SHADER_LIGHTMAPDYNAMIC )
	{
		gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	}
	else
	{
		gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	}

	gpkDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
	gpkDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );

	if( eType == iShader::SHADER_LIGHTMAP || eType == iShader::SHADER_LIGHTMAPDYNAMIC )
		gpkDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );
	else if( eType == iShader::SHADER_LIGHTMAP2X )
		gpkDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE2X );
	else
		gpkDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE4X );

	if( FAILED(gpkDirect3DDevice->EndStateBlock(&pkBlock)) )
	{
		ReportBug("Failed to record state blocks.E","cDirectX9LightMap");
		return;
	}

	mpvStateBlock = (void*)pkBlock;
	X_BREAK_IF(!mpvStateBlock);
}

cDirectX9LightMap::~cDirectX9LightMap()
{
	reinterpret_cast<IDirect3DStateBlock9*>(mpvStateBlock)->Release();
}

void cDirectX9LightMap::set( cMaterial*,bool reset  )
{
	if(reset)
		reinterpret_cast<IDirect3DStateBlock9*>(mpvStateBlock)->Apply();
}

void cDirectX9LightMap::unset()
{
	gpkDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
}
/************************************************************/

/*********************SHPERE MAP*****************************/
cDirectX9SphereMap::cDirectX9SphereMap( const f32* m )
{
	XREFOBJECT(cDirectX9SphereMap);

	IDirect3DStateBlock9* pkBlock = 0;
	
	if( FAILED(gpkDirect3DDevice->BeginStateBlock()) )
	{
		ReportBug("Failed to record state blocks.B","cDirectX9SphereMap");
		return;
	}

	gpkDirect3DDevice->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	gpkDirect3DDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	gpkDirect3DDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	gpkDirect3DDevice->SetTransform( D3DTS_TEXTURE0, reinterpret_cast<const D3DMATRIX*>(m) );
	gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );

	if( FAILED(gpkDirect3DDevice->EndStateBlock(&pkBlock)) )
	{
		ReportBug("Failed to record state blocks.E","cDirectX9SphereMap");
		return;
	}

	mpvStateBlock = (void*)pkBlock;
	X_BREAK_IF(!mpvStateBlock);
}

cDirectX9SphereMap::~cDirectX9SphereMap()
{
	reinterpret_cast<IDirect3DStateBlock9*>(mpvStateBlock)->Release();
}

void cDirectX9SphereMap::set( cMaterial*, bool reset  )
{
	if(reset)
		reinterpret_cast<IDirect3DStateBlock9*>(mpvStateBlock)->Apply();
}

void cDirectX9SphereMap::unset()
{
	gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	gpkDirect3DDevice->SetTransform( D3DTS_TEXTURE0, reinterpret_cast<const D3DMATRIX*>(N3D4x4IdentityMatrix) );
}
/************************************************************/

/************************DETAIL MAP**************************/
cDirectX9DetailMap::cDirectX9DetailMap()
{
	XREFOBJECT(cDirectX9DetailMap);

	IDirect3DStateBlock9* pkBlock = 0;
	
	if( FAILED(gpkDirect3DDevice->BeginStateBlock()) )
	{
		ReportBug("Failed to record state blocks.B","cDirectX9DetailMap");
		return;
	}

	gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	/* use the low-frequency texture coordinates */
	/* assumed */
	gpkDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	gpkDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
	gpkDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADDSIGNED );
	// use the high-frequency texture coordinates
	gpkDirect3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
	
	if( FAILED(gpkDirect3DDevice->EndStateBlock(&pkBlock)) )
	{
		ReportBug("Failed to record state blocks.E","cDirectX9DetailMap");
		return;
	}

	mpvStateBlock = (void*)pkBlock;
	X_BREAK_IF(!mpvStateBlock);
}

cDirectX9DetailMap::~cDirectX9DetailMap()
{
	reinterpret_cast<IDirect3DStateBlock9*>(mpvStateBlock)->Release();
}

void cDirectX9DetailMap::set( cMaterial*, bool reset  )
{
	if(reset)
		reinterpret_cast<IDirect3DStateBlock9*>(mpvStateBlock)->Apply();
}

void cDirectX9DetailMap::unset()
{
	gpkDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
}
/************************************************************/

/**************************GLOW MAP**************************/
cDirectX9GlowMap::cDirectX9GlowMap()
{
	XREFOBJECT(cDirectX9GlowMap);
	IDirect3DStateBlock9* pkBlock = 0;
	
	if( FAILED(gpkDirect3DDevice->BeginStateBlock()) )
	{
		ReportBug("Failed to record state blocks.B","cDirectX9DetailMap");
		return;
	}

	gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	gpkDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	gpkDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
	gpkDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADD );
	
	if( FAILED(gpkDirect3DDevice->EndStateBlock(&pkBlock)) )
	{
		ReportBug("Failed to record state blocks.E","cDirectX9DetailMap");
		return;
	}

	mpvStateBlock = (void*)pkBlock;
	X_BREAK_IF(!mpvStateBlock);
}

cDirectX9GlowMap::~cDirectX9GlowMap()
{
	reinterpret_cast<IDirect3DStateBlock9*>(mpvStateBlock)->Release();
}

void cDirectX9GlowMap::set( cMaterial*,bool reset  )
{
	if(reset)
		reinterpret_cast<IDirect3DStateBlock9*>(mpvStateBlock)->Apply();
}

void cDirectX9GlowMap::unset()
{
	gpkDirect3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
}
/************************************************************/


}