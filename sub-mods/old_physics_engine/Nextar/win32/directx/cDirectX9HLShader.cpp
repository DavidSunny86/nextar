
/* created : feb 17, 07 */
/* by : Abhishek Dey */

#include "cDirectX9HLShader.h"

namespace nextar
{

/************************************************************/
extern IDirect3D9*						gpkDirect3D;
extern IDirect3DDevice9*				gpkDirect3DDevice;
/************************************************************/

cDirectX9HLShader::cDirectX9HLShader( IDirect3DVertexShader9* pkV, IDirect3DPixelShader9* pkP, 
		iSetShaderConstCallback* pkC, iShader* pkB, 
		ID3DXConstantTable* pkVsTable, ID3DXConstantTable* pkPsTable ) : 
cDirectX9LLShader( pkV, pkP, pkC, pkB ), mpkVsTable(pkVsTable), mpkPsTable(pkPsTable)
{
	XREFOBJECT(cDirectX9HLShader);
}

cDirectX9HLShader::~cDirectX9HLShader()
{
	if(mpkVsTable) mpkVsTable->Release();
	if(mpkPsTable) mpkPsTable->Release();
}

bool cDirectX9HLShader::getShaderConstHandle( bool bVs, const s8 *sVarName, h32 &out )
{
	ID3DXConstantTable* pkTable = bVs ? mpkVsTable : mpkPsTable;
		/* check top level params */
	out = (h32)pkTable->GetConstantByName(0,sVarName);
	if( XISVALIDHANDLE(out) )
		return true;
	return false;
}

bool cDirectX9HLShader::setShaderConst( bool bVs, h32 in, const f32 *val, s32 count )
{
	ID3DXConstantTable* pkTable = bVs ? mpkVsTable : mpkPsTable;
	return FAILED( pkTable->SetFloatArray( gpkDirect3DDevice, reinterpret_cast<D3DXHANDLE>(in), val, count ) ) ?false : true;
}

}