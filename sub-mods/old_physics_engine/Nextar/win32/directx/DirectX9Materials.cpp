
/* created : feb 17, 07 */
/* by : Abhishek Dey */

#include <win32/directx/directx9materials.h>
#include <win32/directx/cdirectx9texture.h>
#include <win32/directx/cdirectx9renderer.h>
#include <d3d9.h>
#include <xmemory.h>

using namespace nextar;
IDirect3DStateBlock9* cDirectX9MaterialAlphaMtl::spkBlock = 0;
/************************************************************/
extern IDirect3D9*						gpkDirect3D;
extern IDirect3DDevice9*				gpkDirect3DDevice;
/************************************************************/
#define DXSetColor(c,va,vr,vg,vb) c.a = va; c.r = vr; c.g = vg; c.b = vb;
cDirectX9MtlStub* cDirectX9MtlStub::spkCurShader = 0;

namespace nextar
{
extern IDirect3DDevice9* gpkDirect3DDevice;
}

cDirectX9MtlStub::cDirectX9MtlStub()
{
	zeroMemory(&mkMaterial,sizeof(mkMaterial));
	DXSetColor(mkMaterial.Ambient,1.0f,0.2f,0.2f,0.2f);
	DXSetColor(mkMaterial.Diffuse,1.0f,0.2f,0.2f,0.2f);
	DXSetColor(mkMaterial.Emissive,0.0f,0.0f,0.0f,0.0f);
	DXSetColor(mkMaterial.Specular,0.0f,0.0f,0.0f,0.0f);
	mkMaterial.Power = 0.0f;
	muFlags = Lighting|ZBuffer|ZWrite|BilinearFilter|GouraudShading;
}

void cDirectX9MtlStub::setDiffuse(const cColor4& c)
{
	DXSetColor(mkMaterial.Diffuse,c.a,c.r,c.g,c.b);
}

void cDirectX9MtlStub::setAmbient(const cColor4& c)
{
	DXSetColor(mkMaterial.Ambient,c.a,c.r,c.g,c.b);
}

void cDirectX9MtlStub::setSpecular(const cColor4& c,const f32 pwr)
{
	DXSetColor(mkMaterial.Specular,c.a,c.r,c.g,c.b);
	mkMaterial.Power = pwr;
}

void cDirectX9MtlStub::setEmissive(const cColor4& c)
{
	DXSetColor(mkMaterial.Emissive,c.a,c.r,c.g,c.b);
}

cDirectX9SolidMtl::cDirectX9SolidMtl(float r,float g,float b,cDirectX9Texture* pkT) : mpkTexture(pkT)
{
	DXSetColor(mkMaterial.Diffuse,1.0f,r,g,b);
	if(mpkTexture) mpkTexture->addRef();
}

void cDirectX9SolidMtl::set(s32)	// we r not concerned abt the pass
{
	// set material flags
	((cDirectX9Renderer*)X_SharedPtr(iRenderer))->setMaterialStates(muFlags);
	if(cDirectX9MtlStub::spkCurShader != this)// for multiple passes, do not use this checking
	{
		if(cDirectX9MtlStub::spkCurShader) cDirectX9MtlStub::spkCurShader->unset();
		cDirectX9MtlStub::spkCurShader = this; 
		
		((cDirectX9Renderer*)X_SharedPtr(iRenderer))->setTexture(0,mpkTexture);
		gpkDirect3DDevice->SetMaterial(&mkMaterial);
#ifdef X_DEBUG
		if( mpkTexture )
		{
#endif
			gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
			gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
#ifdef X_DEBUG
		}
		else
			gpkDirect3DDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
#endif
	}
}

cDirectX9SolidMtl::~cDirectX9SolidMtl()
{
	if(mpkTexture) mpkTexture->release();
}

void cDirectX9MaterialAlphaMtl::Initialize()
{
	if( FAILED(gpkDirect3DDevice->BeginStateBlock()) )
	{
		X_BREAK;	// we will replace this later
		return;
	}

	gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	gpkDirect3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	gpkDirect3DDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
	gpkDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	gpkDirect3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA); 
	gpkDirect3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	if( FAILED(gpkDirect3DDevice->EndStateBlock(&spkBlock)) )
	{
		X_BREAK;	// we will replace this later
		return;
	}
}

void cDirectX9MaterialAlphaMtl::Destroy()
{
	if(spkBlock) {
		spkBlock->Release();
		spkBlock = 0;
	}
}

cDirectX9MaterialAlphaMtl::cDirectX9MaterialAlphaMtl(float a,float r,float g,float b,cDirectX9Texture* pkT) : mpkTexture(pkT)
{
	DXSetColor(mkMaterial.Diffuse,a,r,g,b);
	if(mpkTexture) mpkTexture->addRef();
}

cDirectX9MaterialAlphaMtl::~cDirectX9MaterialAlphaMtl()
{
	if(mpkTexture) mpkTexture->release();
}

void cDirectX9MaterialAlphaMtl::set(s32)
{
	// set material flags
	((cDirectX9Renderer*)X_SharedPtr(iRenderer))->setMaterialStates(muFlags);
	if(cDirectX9MtlStub::spkCurShader != this)// for multiple passes, do not use this checking
	{
		if(cDirectX9MtlStub::spkCurShader) cDirectX9MtlStub::spkCurShader->unset();
		cDirectX9MtlStub::spkCurShader = this; 
		
		((cDirectX9Renderer*)X_SharedPtr(iRenderer))->setTexture(0,mpkTexture);
		gpkDirect3DDevice->SetMaterial(&mkMaterial);
#ifdef X_DEBUG
		X_BreakOutIf( FAILED(spkBlock->Apply()) );
#else
		spkBlock->Apply();
#endif
	}
}

void cDirectX9MaterialAlphaMtl::unset()
{
	gpkDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}