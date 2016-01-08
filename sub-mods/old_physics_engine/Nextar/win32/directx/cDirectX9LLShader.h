
/* created : feb 17, 07 */
/* by : Abhishek Dey */
#ifndef DIRECTX9LLSHADER_H
#define DIRECTX9LLSHADER_H
/* #ifdef COMPILE_WITH_DIRECT3D9 */

#include <imaterial.h>
#include <d3d9.h>
#include <d3dx9shader.h>

namespace nextar
{

class cDirectX9LLShader : public iMaterial
{
protected:

	iSetShaderConstCallback*	mpkCallback;
	IDirect3DVertexShader9*		mpkVertexShader;
	IDirect3DPixelShader9*		mpkPixelShader;
	IDirect3DVertexShader9*		mpkOldVertexShader;
	iShader*					mpkBaseStates;

public:

	cDirectX9LLShader( IDirect3DVertexShader9*, IDirect3DPixelShader9*, iSetShaderConstCallback*, iShader* );
	~cDirectX9LLShader();	/* will release all standing interfaces */

	xinline void setCallback( iSetShaderConstCallback* pkCall ) { mpkCallback = pkCall; }
	virtual void initMaterial( cMaterial* m ) { if( mpkBaseStates ) mpkBaseStates->initMaterial(m); }
	virtual void set( cMaterial*, bool );
	virtual void unset();
};

}

/* #endif // COMPILE_WITH_DIRECT3D9 */
#endif 
