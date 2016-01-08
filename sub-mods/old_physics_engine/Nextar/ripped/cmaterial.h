//@# Abhishek Dey
//@# 1/9/2007
//@# September 04 2007

#ifndef __XMATERIAL_H__
#define __XMATERIAL_H__

#include <color.h>
#include <itexture.h>

namespace nextar
{

#define MAX_TEXTURE_PER_MATERIAL 2
class iShader;

struct cMaterial
{

	struct cProperty {
		cColor4		mkDiffuse;
		cColor4		mkAmbient;
		cColor4		mkSpecular;
		cColor4		mkEmissive;
		f32			mfShine;
	};

	cProperty	mkProperty;
	u32			muFlags;
	iShader*	mpkShader;

	union
	{
		struct
		{
			iTexture*	mpkTexture1;
			iTexture*	mpkTexture2;
		};
		iTexture*		mpTexture[MAX_TEXTURE_PER_MATERIAL];
	};

#define XIFTSET(flag) if(t) muFlags |= flag; else muFlags &=~flag;
#define XTSET(flag,funcend) xinline void set##funcend(bool t) { XIFTSET(flag); }
#define XTGET(flag,funcend) xinline bool get##funcend() { return flag & muFlags; }
#define XTMATFLAG(flag,funcend) XTSET(flag,funcend) XTGET(flag,funcend)

	XTMATFLAG(DRAWWIREFRAME,DrawWireframe);
	XTMATFLAG(GOURAUDSHADING,GouraudShading);
	XTMATFLAG(LIGHTING,Lighting);
	XTMATFLAG(ZBUFFER,ZBuffer);
	XTMATFLAG(ZWRITE,ZWrite);
	XTMATFLAG(IS2SIDED,Is2Sided);
	XTMATFLAG(BILINEARFILTER,BilinearFilter);
	XTMATFLAG(TRILINEARFILTER,TrilinearFilter);
	XTMATFLAG(ANISOTROPICFILTER,AnisotropicFilter);
	XTMATFLAG(FOGENABLE,FogEnable);
	XTMATFLAG(SPECULARHIGHLIGHTS,SpecularHighlights);
	XTMATFLAG(NORMALIZENORMALS,NormalizeNormals);
	XTMATFLAG(MATERIAL_IS_TRANSPARENT,MaterialIsTransparent);

#undef XTMATFLAG
#undef XTSET
#undef XTGET
#undef XIFTSET
	
	xinline cMaterial() : mpkShader(0), mpkTexture1(0), mpkTexture2(0),
		muFlags(LIGHTING|ZBUFFER|ZWRITE|BILINEARFILTER)
	{
		mkProperty.mfShine = 0.f;
	}
};

}

#endif //__XMATERIAL_H__