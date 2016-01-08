
#include "cDirectX9ToonShader.h"
#include "cDirectX9Renderer.h"

namespace nextar
{

/************************************************************/
extern IDirect3D9*						gpkDirect3D;
extern IDirect3DDevice9*				gpkDirect3DDevice;
/************************************************************/

const s8 sVShader[] = 
"; Name                Reg   Size \n"
"; ------------------- ---   -------"
"; WorldViewMatrix     c0     4\n"
"; WorldViewProjMatrix c4     4\n"
"; Color               c8     1\n"
"; LightDirection      c9     1\n"
" vs_2_0 \n"
" def c10, 0, 0.5, 0, 0 \n"
" dcl_position v0 \n"
" dcl_normal v1 \n"
" dp4 oPos.x, v0, c4 \n"
" mov r1.xyz, c9 \n"
" dp3 r0.x, r1, c0 \n"
" dp3 r0.y, r1, c1 \n"
" dp3 r0.z, r1, c2 \n"
" dp3 r0.w, r1, c3 \n"
" dp3 r1.x, v1, c0 \n"
" dp3 r1.y, v1, c1 \n"
" dp3 r1.z, v1, c2 \n"
" dp3 r1.w, v1, c3 \n"
" dp4 oPos.y, v0, c5 \n"
" dp4 r1.w, r0, r1 \n"
" dp4 oPos.z, v0, c6 \n"
" slt r0.w, r1.w, c10.x \n"
" dp4 oPos.w, v0, c7 \n"
" mad oT0.x, r0.w, -r1.w, r1.w \n"
" mov oT0.y, c10.y \n"
" mov oD0, c8 \n"
"\n";

cDirectX9ToonShader::cDirectX9ToonShader( iShader* pkBase ) : cDirectX9LLShader(0,0,0,pkBase)
{
	XREFOBJECT(cDirectX9ToonShader);
	// at least vs2 is needed
	if( !iRenderer::XSELFPTR->isFeatureSupported(FS_VS_2_0) ||
		! ( reinterpret_cast<cDirectX9Renderer*>(iRenderer::XSELFPTR)->assembleLowLevelShader(
		sVShader,0,&mpkVertexShader,&mpkPixelShader ) ) )
		ReportBug( "Shader did not compile", "cDirectX9ToonShader" );
}

cDirectX9ToonShader::~cDirectX9ToonShader()
{
}

void cDirectX9ToonShader::set( cMaterial* pM, bool reset )
{
	const cLight* pkL = reinterpret_cast<cDirectX9Renderer*>(iRenderer::XSELFPTR)->findAddedLight(0);
	XASSERT(pkL);

	cDirectX9LLShader::set(pM,reset);
	/* use cMaterial::cProperty::mkDiffuse as toon color */
	gpkDirect3DDevice->SetVertexShaderConstantF( 
		0, 
		(f32*)reinterpret_cast<cDirectX9Renderer*>(iRenderer::XSELFPTR)->getTransform(TS_WORLDVIEW), 
		4 );
	gpkDirect3DDevice->SetVertexShaderConstantF(
		4, 
		(f32*)reinterpret_cast<cDirectX9Renderer*>(iRenderer::XSELFPTR)->getTransform(TS_WORLDVIEWPROJECTION), 
		4 );

	gpkDirect3DDevice->SetVertexShaderConstantF( 8, (f32*)&(pM->mkProperty.mkDiffuse), 1 );
	gpkDirect3DDevice->SetVertexShaderConstantF( 9, (f32*)&(pkL->mkVector), 1 );
}

}
