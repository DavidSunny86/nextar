//@ Abhishek Dey
//@ 5/1/2006 2:19:31 AM
//@ September 04 2007

#ifndef XLIGHT_H
#define XLIGHT_H

#include <xbasictypes.h>
#include <graphics/xcolor.h>

namespace nextar
{

/* cLight */
struct cLight : public cBaseClass
{
	enum Type
	{
		Point,
		Directional
	};

	cColor4		mkAmbient;
	cColor4		mkDiffuse;
	cColor4		mkSpecular;
	f32			mkVector[4]; // multipurpose used as origin for point source and direction for directional light
	f32			mfRadius;
	bool		mbCastShadows;
	Type		meType;
	_n_inline cLight() : meType(Point), mfRadius(100.0f), mbCastShadows(true)	{ mkVector[3] = 0; }
}; 

} // end of nextar


#endif //XLIGHT_H