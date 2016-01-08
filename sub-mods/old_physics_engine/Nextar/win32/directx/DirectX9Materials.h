
/* created : feb 17, 07 */
/* by : Abhishek Dey */

#ifndef NEXTAR_DIRECTX9SHADERS_H
#define NEXTAR_DIRECTX9SHADERS_H

#include <graphics/imaterial.h>
#include <win32/directx/cdirectx9texture.h>
#include <d3d9.h>

namespace nextar
{
class cDirectX9Texture;
class cDirectX9MtlStub : public iMaterial
{
public:
	static cDirectX9MtlStub*	spkCurShader;
protected:
	D3DMATERIAL9		mkMaterial;
public:
	cDirectX9MtlStub();
	virtual ~cDirectX9MtlStub() {}

	virtual void setDiffuse(const cColor4&);
	virtual void setSpecular(const cColor4&,const f32 power);
	virtual void setAmbient(const cColor4&);
	virtual void setEmissive(const cColor4&);
	virtual void unset() {}
};

/* make state blocks if state changes more than 5 */
class cDirectX9SolidMtl : public cDirectX9MtlStub
{
	cDirectX9Texture*	mpkTexture;
public:
	cDirectX9SolidMtl(float r=0.5f,float g=0.5f,float b=0.5f,cDirectX9Texture* pkT=0);
	~cDirectX9SolidMtl();
	
	//@ set as the current material
	virtual void set(s32);
	virtual u32 getClassDesc() { return Solid; }
	//@ get texture
	virtual iTexture* getType(s32) { return mpkTexture; }
	virtual void setTexture(iTexture* pkT,s32) { 
		if(mpkTexture) mpkTexture->release();
		mpkTexture = (cDirectX9Texture*)pkT; 
		if(mpkTexture) mpkTexture->addRef();
	}
};

class cDirectX9MaterialAlphaMtl : public cDirectX9MtlStub
{
	cDirectX9Texture*	mpkTexture;
	static IDirect3DStateBlock9* spkBlock;
public:

	static void Initialize();
	static void Destroy();

	cDirectX9MaterialAlphaMtl(float a=1.0f,float r=0.5f,float g=0.5f,float b=0.5f,cDirectX9Texture* pkT=0);
	~cDirectX9MaterialAlphaMtl();
	//@ set as the current material
	virtual void set(s32);
	virtual u32 getClassDesc() { return TransparentMtlAlpha; }
	//@ unset material
	virtual void unset();
	//@ get texture
	virtual iTexture* getType(s32) { return mpkTexture; }
	virtual void setTexture(iTexture* pkT,s32) { 
		if(mpkTexture) mpkTexture->release();
		mpkTexture = (cDirectX9Texture*)pkT; 
		if(mpkTexture) mpkTexture->addRef();
	}
};

}

#endif /*DIRECTX9SHADERS_H*/