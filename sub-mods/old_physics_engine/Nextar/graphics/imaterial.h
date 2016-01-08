//@ Abhishek Dey
//@ Oct 20 2007

#ifndef _XIMATERIAL_H_
#define _XIMATERIAL_H_

#include <xtypes.h>
#include <iobject.h>
#include <graphics/xcolor.h>

namespace nextar
{
//@ texture
class iTexture;
class iXMLElement;
//@ a basic material

struct iMaterial : public iObject
{
	u32 muFlags;
	enum EMATERIALFLAGS
	{
		DrawWireframe = 1 << 1,
		GouraudShading = 1 << 2,
		Lighting = 1 << 3,
		ZBuffer = 1 << 4,
		ZWrite = 1 << 5,
		Is2Sided = 1 << 6,
		BilinearFilter = 1 << 7,
		TrilinearFilter = 1 << 8,
		AnisotropicFilter = 1 << 9,
		FogEnable = 1 << 10,
		SpecularEnable = 1 << 11,
		NormalizeNormals = 1 << 12,
		UnsetFlag = 1 << 14
	};
	enum DefaultMaterialClasses
	{
		Solid,
		TransparentMtlAlpha,
		MtlClassCount
	};

	virtual u32 getClassDesc() = 0;
	virtual ~iMaterial() {}
	//@ set colors [not all interfaces support this]
	virtual void setDiffuse(const cColor4&) {}
	virtual void setSpecular(const cColor4&,const f32 power) {}
	virtual void setAmbient(const cColor4&) {}
	virtual void setEmissive(const cColor4&) {}
	//@ returns the flags describing the material
	_n_inline u32 flags() { return muFlags; }
	_n_inline void flags(u32 f) { muFlags |= f; }
	//@ set as the current material
	virtual void set(s32 pass=0) = 0;
	//@ reset the states set by this material
	virtual s32  passCount() { return 1; }
	//@ get texture
	virtual iTexture* getType(s32 level=0) = 0;
	//@ set texture
	virtual void setTexture(iTexture*,s32 level=0) = 0;
	//@ load from xml file
	virtual bool loadMtl(iXMLElement*) { return false; }
};

//@ a material using a shader
struct iShader : public iMaterial
{
	virtual ~iShader() {}
	virtual void getShaderConstHandle( bool bVs, const s8* sVarName, h32 &out ) = 0;
	virtual void setShaderConst( bool bVs, h32 in, const f32* val, s32 count ) = 0;
};

//@ shader constant set callbak
//@ can be used to set constants to shader
class iSetShaderConstCallback
{
public:
	virtual ~iSetShaderConstCallback() {}
	virtual void setShaderConst( iShader* ) = 0;
};

}
#endif // _XIMATERIAL_H_