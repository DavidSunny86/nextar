
//@ Abhishek Dey
//@ 1/9/2007
//@ September 04 2007
//@ September 23 2007

#ifndef NEXTAR_IRENDERER_H_1_01
#define NEXTAR_IRENDERER_H_1_01

#include <iobject.h>
#include <xtypesex.h>
#include <xmemory.h>
#include <graphics/itexture.h>
#include <graphics/csoftwaretexture.h>
#include <graphics/imaterial.h>
#include <graphics/clight.h>
#include <graphics/xvertex.h>
#include <cstr.h>
#include <list_t.h>
#include <ui/ixuifont.h>

namespace nextar
{
//============================================================================
// some constants
#define N3DMaxTextureStage 7

//============================================================================
// For generality, we issue that all matrices are row major 4x4 D3D type.
enum TRANSFORMSTATE
{
	//@ the matrices u can set 
	TS_VIEW = 0,
	TS_PROJECTION,
	TS_WORLDVIEW,
	TS_WORLDVIEWPROJECTION,
	TS_COUNT
};
//============================================================================
enum RENDERDEVICE
{
	RD_DIRECT3D9,
	RD_OPENGL
};
//============================================================================
enum FEATURE3D
{
	FS_RENDER_TO_MULTIPLE_TEXTURE = 0,
	FS_HARDWARE_TL,
	FS_MIP_MAPS,
	FS_AUTOGEN_MIP_MAPS,
	FS_STENCIL_BUFFER,
	FS_ANY_SIZE_TEXTURE,
	FS_INDEXED_VERTEX_BLENDING,
	// pixel shader
	FS_PS_1_1,
	FS_PS_1_2,
	FS_PS_1_3,
	FS_PS_1_4,
	FS_PS_2_0,
	FS_PS_3_0,
	// vertex shader
	FS_VS_1_1,
	FS_VS_2_0,
	FS_VS_3_0,
	FS_COUNT
};
//============================================================================
enum VSHADER_TARGET
{
	VST_1_1,
	VST_2_0,
	VST_2_a,
	VST_3_0,
};
//============================================================================
enum PSHADER_TARGET
{
	PST_1_1,
	PST_1_2,
	PST_1_3,
	PST_1_4,
	PST_2_0,
	PST_2_a,
	PST_2_b,
	PST_3_0,
};
//============================================================================
enum RENDERSTATEMODE
{
	RSC_INVALID = 0,
	RSC_2D,
	RSC_3D,
	RSC_STENCIL_FILL_WITH_ALPHA,
	RSC_STENCIL_FILL_WITHOUT_ALPHA,
	RSC_SHADOW_VOLUME_ZPASS,
	RSC_SHADOW_VOLUME_ZFAIL,
	RSC_COUNT
};
//============================================================================


class X_Api iRenderer : public iObject
{
public:

public:
	// creation flags
	enum { FullScreen = 1, StencilBuffer = 2, AntiAlias = 4, VSync = 8, HardwareVP = 16, LastFlag = 32};

protected:
	// more internal flags 
	enum { 
		ResetTransforms = iRenderer::LastFlag<<0, 
		DeviceLost		= iRenderer::LastFlag<<1 
	};

	SharedSingleInterface(iRenderer);
public:
	//@ sec 1 : ctor/dtor 
	//@ ctor 
	iRenderer();
	//@ dtor 
	virtual ~iRenderer();
	//@ end sec 1 
	//@ sec 2 : get/set functions 
	//@ sec 2.1 : get functions 
	const f32*				getTransform( TRANSFORMSTATE  t ) const;
	void					setTransform( TRANSFORMSTATE t, const f32* mat );
	//@ sec 2.1.1 : inlines 
	_n_inline const rect2_x&	getViewport( ) const { return mkViewport; }
	_n_inline const size2_x&	getScreenSize( ) const { return mkScreenSize; }
	_n_inline u32				getBitsPerPixel() const { return muBitsPerPixel; }
	//@ end sec 2.1.1 
	//@ sec 2.1.2 : virtual 
	virtual RENDERDEVICE	getDriverType() const = 0;
	virtual const tchar*	getDriverName() const = 0;
	virtual const u32		getTotalPolysRendered() const = 0;
	virtual const u32		getTotalRenderStateChanges() const = 0;
	virtual s32				getMaxDynamicLight() const = 0;
	virtual const f32*		getWorld() const = 0;
	//@ end sec 2.1.2 
	//@ end sec 2.1 
	//@ sec 2.2 : set functions 
	//@ sec 2.2.1 : inlines 
	//@ end sec 2.2.1 
	//@ sec 2.2.2 : virtual 
	virtual void			setBackgroundColor( s32 c ) = 0;
	virtual void			setMode( RENDERSTATEMODE ) = 0;
	virtual void			setViewport( const rect2_x &vp ) = 0;
	virtual void			setScreenSize( const size2_x& rkScreenSize , u32 uBits=-1 ) = 0;	//@ change mode 
	virtual	void			setTexture( s32 stage, iTexture *tex ) = 0;
	virtual void			setVertexShader( VERTEXTYPE ) = 0;
	virtual void			setWorld( const f32* ) = 0;
	virtual void			setAmbientLight( s32 color ) = 0;
	//@ end sec 2.2.2 
	//@ end sec 2.2 
	//@ end sec 2 
	//@ sec 3 : add/clear/get dynamic lights 
	//@ the ptr to the added light shoud remain valid till end scene is called 
	virtual void			addDynamicLight( const cLight* ) = 0;
	virtual void			finalizeLights() = 0; //@ call when all lights are added 
	virtual const cLight*	findAddedLight( s32 idx ) = 0; //@ call this after finalizing to avoid errors 
	//@ end sec 3 
	//@ sec 4 : general funcs 
	virtual void			reset() = 0;
	virtual void			onSwitchMode( bool bFullScreen = true ) = 0;	//@ sys call 
	virtual void			clearZBuffer() = 0;
	virtual void			beginScene( bool bClearBack = 1, bool bClearZbuff = 1 ) = 0;
	virtual	void			endScene( ) = 0;
	virtual bool			isFeatureSupported( FEATURE3D ) = 0;
	virtual iXuiFont*		createFont() = 0;
	virtual	iTexture*		createRenderTarget( const s32 iWidth, const s32 iHeight ) = 0;
	virtual	iTexture*		createTexture( cSoftwareTexture* image, u32 flags ) = 0;
	virtual	iTexture*		createTexture( const s32 iWidth, const s32 iHeight, u32 flags ) = 0;
	//@ end sec 4 
	//@ sec 5 : drawing functions 
	//@ sec 5.1 : draw 3d triangle lists 
	virtual void			drawIndexedTriangleList( void *vb, u16* ib, u32 numv, u32 nump, u32 sizeofvert ) = 0;
	virtual void			drawIndexedTriangleFan( void *vb, u16* ib, u32 numv, u32 nump, u32 sizeofvert ) = 0;
	//@ end sec 5.1 
	//@ sec 5.2 : draw 2d 
	virtual void			draw2DImage( iTexture *, const float* src, const rect2_x &dest, s32 color ) = 0; // src = u1,u2,v1,v2
	virtual void			draw2DPoly(const coord2_x &centre, const f32 radius, s32 outerC, s32 centreC) = 0;
	
	virtual void			draw2DTri(s32 color, const coord2_x& p1, const coord2_x& p2, const coord2_x& p3) = 0;
	virtual void			draw2DQuad(const rect2_x &rc, s32 iColTL, s32 iColTR, s32 iColBL, s32 iColBR) = 0;
	virtual void			draw2DQuad(s32 color, s32 left, s32 top, s32 right, s32 bottom) = 0;
	virtual void			draw2DRect(s32 color, s32 left, s32 top, s32 right, s32 bottom) = 0;
	virtual void			draw2DLine(coord2_x &start, coord2_x &end, s32 color) = 0;
	_n_inline void			draw2DImage(iTexture *t, const float* src, const coord2_x &pos, const size2_x &s, s32 color) {
		draw2DImage(t, src, rect2_x( pos, s ), color); 
	}
	//@ end sec 5.2 
	//@ sec 5.2 : debug draw 
#ifdef X_DEBUG
	virtual void			draw3DLine(const vec3_t &start,const vec3_t &end, s32 color) = 0;
	virtual void			draw3DBone(const vec3_t &start,const vec3_t &end, s32 color) = 0;
	virtual void			draw3DSphere(const vec3_t &center, const f32 rad, s32 color ) = 0;
	virtual void			draw3DBox(const vec3_t &box_max, const vec3_t &box_min, s32 color, bool wireframe) = 0;
	virtual void			draw3DTriangle(const vec3_t &p1,const vec3_t &p2,const vec3_t &p3,s32 c, bool wframe ) = 0;
	virtual void			draw3DTransform(const f32 *t, const f32 length = 100.0f) = 0;
	virtual void			draw3DPlane(const plane_t &p,const vec3_t &pos,f32 size, s32 color) = 0;
	virtual void			draw3DFrustum(const plane_t p[], s32 color) = 0;
#endif
	//@ end sec 5.2 
	//@ end sec 5 
	//@ sec 6: shaders 
	virtual bool			setPSShaderConst( u32 startReg, const f32* b, s32 count ) = 0;
	virtual bool			setVSShaderConst( u32 startReg, const f32* b, s32 count ) = 0;
	virtual iShader*		compileHighLevelShader( const s8* sVSProg, const s8* sVSEntry, 
		const s8* sPSProg, const s8* sPSEntry, VSHADER_TARGET, PSHADER_TARGET, 
		iSetShaderConstCallback* pkCallback, iShader* pkBase ) = 0;
	virtual iShader*		assembleLowLevelShader( const s8* sVSProg, const s8* sPSProg, 
		iSetShaderConstCallback* pkCallback, iShader* pkBase ) = 0;
	//@ end sec 6 
protected:
	//@ state variables 
	rect2_x			mkViewport;		//@ mrViewport 
	size2_x			mkScreenSize;	//@ msScreenSize 
	u32				muBitsPerPixel;
	u32				muRenderFlags;
	mutable bool	mbMatricesCal;
};

}

#endif //NEXTAR_IRENDERER_H_1_01