//@# Abhishek Dey
//@# 1/9/2007
//@# September 03 2007

#ifndef __IXMATERIALSHADER_H__
#define __IXMATERIALSHADER_H__

#include <iobject.h>
#include <cmaterial.h>

namespace nextar
{

/* for std shaders using multiple stages make sure u disable stages above 0 */
/* or alpha stages at 0 when unset is called */ 
class iShader : public iObject
{
public:

	enum STDSHADERS
	{
		SHADER_SOLID,
		/* light maps and slight variations */
		SHADER_LIGHTMAP,			/* a.k.a Dark Maps */
		SHADER_LIGHTMAP2X,			/* MODULATE2X */
		SHADER_LIGHTMAP4X,			/* MODULATE4X */
		SHADER_LIGHTMAPDYNAMIC,		/* uses (the diffuse color) * (the texture color) in 0 stage */
		/* enviornment maps */
		SHADER_SPHEREMAP,
		SHADER_DETAILMAP,
		/* glow maps */
		SHADER_GLOWMAP,
		/* vs toon shader */
		SHADER_TOON,
		SHADER_USER
	};

public:
	virtual ~iShader() {}
	virtual void initMaterial( cMaterial* m )  {
		m->muFlags &= ~cMaterial::MATERIAL_IS_TRANSPARENT; 
		m->mpkShader = this; 
	}
	virtual void set( cMaterial*, bool reset = true ) = 0;
	/* this will return a unique handle identifying the parameter */
	/* in OpenGl this handle will be ptr to uniform object */
	/* while Dx will return the handle used by constant table */
	virtual bool getShaderConstHandle( bool bVs, const s8* sVarName, h32 &out ) { return false; }
	virtual bool setShaderConst( bool bVs, h32 in, const f32* val, s32 count ) { return false; }
	virtual void unset() {}
};

/* shader constant set callbak */
/* can be used to set constants to shader */
class iSetShaderConstCallback
{
public:
	virtual ~iSetShaderConstCallback() {}
	virtual void setShaderConst( iShader* ) = 0;
};

}

#endif //__IXMATERIALSHADER_H__