
/* created : feb 17, 07 */
/* by : Abhishek Dey */

#ifndef CHLSHADER_H
#define CHLSHADER_H

#include "cDirectX9LLShader.h"

namespace nextar
{

class cDirectX9HLShader : public cDirectX9LLShader
{
public:

	cDirectX9HLShader( 
		IDirect3DVertexShader9*, IDirect3DPixelShader9*, 
		iSetShaderConstCallback*, iShader*, 
		ID3DXConstantTable* pkVsTable, ID3DXConstantTable* pkPsTable );
	
	~cDirectX9HLShader(); /* release the contst table */

	virtual bool getShaderConstHandle( bool bVs, const s8* sVarName, h32 &out );
	virtual bool setShaderConst( bool bVs, h32 in, const f32* val, s32 count );

protected:

	ID3DXConstantTable*	mpkPsTable;
	ID3DXConstantTable* mpkVsTable;
};

}

#endif /* CHLSHADER_H */