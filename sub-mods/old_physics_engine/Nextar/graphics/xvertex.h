//@ Abhishek Dey
//@ 1/17/2007
//@ September 04 2007

#ifndef __SVERTEX_H__
#define __SVERTEX_H__

#include <xbasictypes.h>

namespace nextar
{
enum VERTEXTYPE
{
	VERTEX_UNUSED = -1,
	VERTEX_2D,
	VERTEX_STD,
	VERTEX_2TEX,
	VERTEX_TAN
};

/******************************************************************************
./ Standard Vertex:
./ Elements:
./ mkPosition
./ mkNormal
./ Diffuse
./ Texture Coordinate
******************************************************************************/
struct cVertexStd
{
	vec3_t	mkPosition;
	vec3_t	mkNormal;
	s32		miColor;
	vec2_t	mkTexCoord;
};

/******************************************************************************
./ Vertex with 2 texture coordinate
./ Elements:
./ mkPosition
./ mkNormal
./ Diffuse
./ Texture Coordinate 1
./ Texture Coordinate 2
******************************************************************************/
struct cVertex2Tex
{
	vec3_t	mkPosition;
	vec3_t	mkNormal;
	s32		miColor;
	vec2_t	mkTexCoord1;
	vec2_t	mkTexCoord2;
};

/******************************************************************************
./ Vertex with tangent and binormal
./ for bump mapping using shaders
./ Elements:
./ mkPosition
./ mkNormal
./ Diffuse
./ Texture Coordinate
./ Tangent
./ Cross Factor
******************************************************************************/
struct cVertexTan
{
	vec3_t	mkPosition;
	vec3_t	mkNormal;
	s32		miColor;
	vec2_t	mkTexCoord;
	vec3_t	mkTangent;
	f32		mfCrossFactor; // binormal = cross factor * ( tangent <cross> normal )
};

}

#endif //.h