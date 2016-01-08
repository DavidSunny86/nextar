
//@ Abhishek Dey
//@ October 2 2006
//@ December 14 2006
//@ September 16 2007

#ifndef XDIRECTX9_RENDERER_H
#define XDIRECTX9_RENDERER_H
/* #ifdef COMPILE_WITH_DIRECT3D9 */

#include <irenderer.h>

struct IDirect3DVertexShader9 abstract;
struct IDirect3DPixelShader9 abstract;

namespace nextar
{

struct cDxVector4
{
	f32 x,y,z,w;
};

struct cDxVector2
{
	f32 x,y;
};

struct cDxVertex2D
{
	cDxVector4	mkPosition;
	u32			miColor;
	cDxVector2	mkTexCoord;
};
extern cDxVertex2D	gk2DVertices[];
extern u16			gu2DIndices[];
// few constants
#define N3D2DQUADPRECACHE	64
#define N3D2DQuadPrecache	N3D2DQUADPRECACHE
#define N3D2DVertPrecache	N3D2DQUADPRECACHE*4
#define N3D2DIdxPrecache	N3D2DQUADPRECACHE*6

class cDirectX9Renderer : public iRenderer
{
public:

	//@ [ctor/dtor]
	cDirectX9Renderer( HWND hWnd, const size2_x& kScreenSize, u32 uBits, u32 Flags );
	virtual ~cDirectX9Renderer();

private:
	XERRORCODE		_create(HWND);
	void			_onResize(const size2_x&);
	void			_recordStates();
private:
	
public:
	/* end sec 1 */
	/* sec 2 : get/set functions */
	/* sec 2.1 : get functions */
	virtual RENDERDEVICE	getDriverType() const;
	virtual const char*	getDriverName() const;
	virtual const u32		getTotalPolysRendered() const;
	virtual const u32		getTotalRenderStateChanges() const;
	virtual s32				getMaxDynamicLight() const;
	virtual const f32*		getWorld() const;
	/* end sec 2.1 */
	/* sec 2.2 : set functions */
	virtual void			setBackgroundColor( s32 color );
	virtual void			setMode( RENDERSTATEMODE );
	virtual void			setViewport( const rect2_x &vp );
	virtual void			setScreenSize( const size2_x& rkScreenSize , u32 uBits=-1 );	/* change mode */
	virtual	void			setTexture( s32 stage, iTexture *tex );
	virtual void			setVertexShader( VERTEXTYPE );
	virtual void			setWorld( const f32* );
	virtual void			setAmbientLight( s32 color );
	/* end sec 2.2 */
	/* end sec 2 */
	/* sec 3 : add/clear dynamic lights */
	virtual void			addDynamicLight( const cLight* );
	virtual void			finalizeLights();
	virtual const cLight*	findAddedLight( s32 idx );
	/* end sec 3 */
	/* sec 4 : general funcs */
	virtual void			reset();
	virtual void			onSwitchMode( bool bFullScreen = true );	/* called by system */
	virtual void			clearZBuffer();
	virtual void			beginScene( bool bClearBack = 1, bool bClearZbuff = 1 );
	virtual	void			endScene( );
	virtual bool			isFeatureSupported( FEATURE3D );
	virtual iXuiFont*		createFont();
	virtual	iTexture*		createTexture( cSoftwareTexture* image, u32 flags );
	virtual	iTexture*		createTexture( const s32 iWidth, const s32 iHeight, u32 flags );
	virtual	iTexture*		createRenderTarget( const s32 iWidth, const s32 iHeight );
	/* end sec 4 */
	/* sec 5 : drawing functions */
	/* sec 5.1 : draw 3d triangle lists */
	virtual void			drawIndexedTriangleList( void*,u16*,u32 v,u32 p, u32 sov );
	virtual void			drawIndexedTriangleFan( void*,u16*,u32 v,u32 p, u32 sov );
	/* end sec 5.1 */
	/* sec 5.2 : draw 2d */
	virtual void			draw2DImage( iTexture*,const float*,const rect2_x&, s32 );
	virtual void			draw2DPoly(const coord2_x&,const f32,s32,s32);
	virtual void			draw2DTri(s32,const coord2_x&,const coord2_x&,const coord2_x&);
	virtual void			draw2DQuad(const rect2_x&,s32,s32,s32,s32);
	virtual void			draw2DQuad(s32 color, s32 left, s32 top, s32 right, s32 bottom);
	virtual void			draw2DRect(s32 color, s32 left, s32 top, s32 right, s32 bottom);
	virtual void			draw2DLine(coord2_x &start, coord2_x &end, s32 color);
	// specific draw functions
	void draw2DLineStrip(cDxVertex2D*,s32); //assumes vertices are prepared
	void draw2DTriFan(cDxVertex2D*,s32);//assumes vertices are prepared
	void draw2DTriStrip(cDxVertex2D*,s32 tricnt); //assumes vertices are prepared
	/* end sec 5.2 */
	/* sec 5.2 : debug draw */
#ifdef X_DEBUG
	virtual void			draw3DLine(const vec3_t &start,const vec3_t &end, s32 color);
	virtual void			draw3DBone(const vec3_t &start,const vec3_t &end, s32 color);
	virtual void			draw3DSphere(const vec3_t &center, const f32 rad, s32 color );
	virtual void			draw3DBox(const vec3_t &box_max, const vec3_t &box_min, s32, bool);
	virtual void			draw3DTriangle(const vec3_t&,const vec3_t&,const vec3_t&,s32, bool);
	virtual void			draw3DTransform(const f32 *t, const f32 length = 100.0f);
	virtual void			draw3DPlane(const plane_t &p,const vec3_t &pos,f32 size, s32 color);
	virtual void			draw3DFrustum(const plane_t p[], s32 color);
#endif
	/* end sec 5.2 */
	/* end sec 5 */
	/* sec 6: shaders */
	virtual bool			setPSShaderConst( u32 startReg, const f32* b, s32 count );
	virtual bool			setVSShaderConst( u32 startReg, const f32* b, s32 count );
	virtual iShader*		compileHighLevelShader( const s8* sVSProg, const s8* sVSEntry, 
		const s8* sPSProg, const s8* sPSEntry, VSHADER_TARGET, PSHADER_TARGET, 
		iSetShaderConstCallback* pkCallback, iShader* pkBase );
	virtual iShader*		assembleLowLevelShader( const s8* sVSProg, const s8* sPSProg, 
		iSetShaderConstCallback* pkCallback, iShader* pkBase );
	/* end sec 6 */
public:
	void setMaterialStates( u32 flags );
	bool compileHighLevelShader( const s8* sVSProg, const s8* sVSEntry, 
		const s8* sPSProg, const s8* sPSEntry, VSHADER_TARGET, PSHADER_TARGET, 
		IDirect3DVertexShader9** pkVShader, IDirect3DPixelShader9** pkPShader,
		void** pkVSConstTable,void** pkPSConstTable ); /* **ConstTable = ptr to ID3DXConstantTable */
	bool assembleLowLevelShader( const s8* sVSProg, const s8* sPSProg, 
		IDirect3DVertexShader9** pkVShader, IDirect3DPixelShader9** pkPShader );
};

}
/* #endif // COMPILE_WITH_DIRECT3D9 */
#endif /* XDIRECTX9_RENDERER_H */