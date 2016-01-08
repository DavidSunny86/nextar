
/* created : feb 17, 07 */
/* by : Abhishek Dey */
#include "cDirectX9LLShader.h"

namespace nextar
{

/************************************************************/
extern IDirect3D9*						gpkDirect3D;
extern IDirect3DDevice9*				gpkDirect3DDevice;
/************************************************************/

cDirectX9LLShader::cDirectX9LLShader( 
	IDirect3DVertexShader9* pkVs, IDirect3DPixelShader9* pkPs, 
	iSetShaderConstCallback* pkCb, iShader* pkB ) : mpkVertexShader(pkVs), 
	mpkPixelShader(pkPs), mpkCallback(pkCb), mpkBaseStates(pkB), mpkOldVertexShader(0)
{
	XREFOBJECT(cDirectX9LLShader);
	if( mpkBaseStates ) mpkBaseStates->addRef();
}

cDirectX9LLShader::~cDirectX9LLShader()
{
	if( mpkVertexShader ) mpkVertexShader->Release();
	if( mpkPixelShader ) mpkPixelShader->Release();
	if( mpkBaseStates ) mpkBaseStates->release();
}

void cDirectX9LLShader::set( cMaterial* m, bool reset )
{	
	if(reset)
	{

		if( mpkVertexShader ) 
		{
			gpkDirect3DDevice->GetVertexShader(&mpkOldVertexShader);
			if( FAILED(gpkDirect3DDevice->SetVertexShader(mpkVertexShader)) )
				ReportBug( "Error setting vertex shader", "cDirectX9LLShader::set" );
		}

		if( mpkPixelShader )
		{
			if( FAILED(gpkDirect3DDevice->SetPixelShader(mpkPixelShader)) )
				ReportBug( "Error setting pixel shader", "cDirectX9LLShader::set" );
		}

	}

	if( mpkCallback )
		mpkCallback->setShaderConst(this);
	if( mpkBaseStates )
		mpkBaseStates->set(m,reset);
}

void cDirectX9LLShader::unset()
{
	if( mpkVertexShader )
		gpkDirect3DDevice->SetVertexShader(mpkOldVertexShader);
	if( mpkPixelShader )
		gpkDirect3DDevice->SetPixelShader(0);
	if( mpkBaseStates )
		mpkBaseStates->unset();
}

}