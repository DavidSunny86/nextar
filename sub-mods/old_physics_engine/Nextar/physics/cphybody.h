//@ Abhishek Dey
//@ Oct 21 2007
//@ Nov 09 2007

#ifndef NEXTAR_XPBODY
#define NEXTAR_XPBODY

#include "math/xmath_c.h"
#include "xtypes.h"
#include "physics/csurface.h"

namespace nextar
{

struct iCollisionObject;
struct cPhyBody : public cAlignedBaseClass
{
	enum {
		Enabled = 1 << 0,
		//IsConstrained = 1 << 1,
		AutoDisable = 1 << 2,
		GroundStaticObject = 1 << 3, // a single instance of this 
		FiniteRotation	= 1 << 4,
		FiniteRotationAxis = 1 << 5,
		//type can be shared between objects which do have the capeablility
		//of movement but will not move on any collision or anything.
		LastFlag = 1 << 6,
	};
	union {
		f32				mkTmp[8];	// internally used					0
		f32				mkAcc[8];
		f32				mkA[8];
		struct {
			vec4_t		mkFacc;		// constrain force
			vec4_t		mkTacc;		// constrain torque
		};
	};
	mat34_t			mkInvI;		// inverse inertial tensor			0
	mat34_t			mkInvIWorld;// world inverse inertial tensor 	0
	// data that concerns us
	transform		mkWorld;	// world							0
	quat_t			mkQ;		// orientation in world				0
	vec4_t			mkPdot;		// linear velocity					0
	vec4_t			mkOmega;	// angular velocity					0
	union {
		struct {
			vec4_t			mkFext;		// force accumulator		0
			vec4_t			mkText;		// torque accumulator		0
		};
		f32					mkAext[8];					//			0
	};
	f32					mfMass;		// mass							3
	f32					mfInvMass;	// inverse of mass				2
	f32					mfDensity;  // 
	x_time				mtTimeToIdle;	// in seconds				1
	u32					muFlags;	//								0
	iCollisionObject*	mpkObjects; //								3
	cPhyBody*			mpkNext;						//			2
	cPhyBody*			mpkPrev;						//			1
	
	cPhyBody();
	~cPhyBody();

	_n_inline		bool	isDisabled() { return !(muFlags & Enabled); }
	_n_noinline	void	setPosition(f32 x,f32 y,f32 z);
	_n_noinline	void	setRotationAxisAngle(f32 ax,f32 ay,f32 az,f32 val);
	_n_noinline	void	setRotationEuler(f32 yaw,f32 pitch,f32 roll); 
	_n_noinline	void	enable();
	_n_noinline	void	disable();

	// set mass utility
	_n_noinline	void	setCubeMass(f32 density,f32 lx,f32 ly,f32 lz);
};

}

#endif //NEXTAR_XPBODY