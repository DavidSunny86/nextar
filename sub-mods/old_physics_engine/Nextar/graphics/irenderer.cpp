
//@ Abhishek Dey
//@ 1/9/2007
//@ September 04 2007

#include <irenderer.h>
#include <xmath_c.h>

using namespace nextar;

/*      draw method		*/
/*		1---------3		*/
/*		|		  |		*/
/*		|		  |		*/
/*		|		  |		*/
/*		0---------2		*/
/*		vertex index	*/
/*	  {(0,1,2) (1,3,2)} */
namespace nextar
{

//cVertex2D	gk2DVertices[N3D2DVertPrecache] = {0};
//u16			gu2DIndices[N3D2DIdxPrecache];

SharedSingleInterfaceSetup(iRenderer);

mat44_t	garrkTransforms[TS_COUNT] = {0}; // mtTransforms

}

iRenderer::iRenderer() : muRenderFlags(0),mbMatricesCal(false)
{
	SharedSingleInterfaceInit(iRenderer;
	XTRACE(("\nRenderer at:[0x%x]",(u32)this));
	// initialize the indices
}

iRenderer::~iRenderer()
{
	XTRACE(("\nRenderer at:[0x%x]",(u32)this));
	SharedSingleInterfaceExit(iRenderer;
}

void iRenderer::setTransform(TRANSFORMSTATE t,const f32* mat)
{ 	
	mbMatricesCal = 0;
	muRenderFlags |= ResetTransforms;
	xM16CpyMac( garrkTransforms[t], mat );
}

const f32* iRenderer::getTransform( TRANSFORMSTATE  t ) const
{
	if( t < TS_WORLDVIEW || mbMatricesCal ) 
		return garrkTransforms[t];
	else
	{
		xMul1_44(garrkTransforms[TS_WORLDVIEW],
			getWorld(),
			garrkTransforms[TS_VIEW]);
		xMul1_44(garrkTransforms[TS_WORLDVIEWPROJECTION],
			garrkTransforms[TS_WORLDVIEW],
			garrkTransforms[TS_PROJECTION]);
		mbMatricesCal = true;
		return garrkTransforms[t];
	}
}
