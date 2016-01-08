//@ Abhishek Dey
//@ Nov 08 2007
//@ Nov 09 2007

#ifndef NEXTAR_SIMULATOR_VERSION_1
#define NEXTAR_SIMULATOR_VERSION_1

#include "iobject.h"
#include "physics/cphybody.h"
#include "physics/cconstrain.h"
#include "physics/cnpconstrain.h"
#include "physics/cfcconstrain.h"
#include "speciallist.h"

namespace nextar
{

class X_Api cSimulator : public iObject
{
	SharedSingleInterface(cSimulator);

private:
	struct cAutoDisable
	{
		f32		mfLinearThreshold;	// linear velocity threshold
		f32		mfAngularThreshold; // angular velocity threshold
		x_time	mtIdleTime;			// time to become idle
	};
	s32						miNumIterations;
	cAutoDisable			mkDisable;
	SpecialList<cConstrain>	mkConstrains;
	SpecialList<cPhyBody>	mkBodies;
public:

	cSimulator(const tchar* sConfig = 0);
	~cSimulator();

	_n_inline void setIterationCount(s32 count) { miNumIterations = count; }
	_n_inline s32  getIterationCount() { return miNumIterations; }
	//@ this call evaluates all the right 
	//@ positions for bodies for given
	//@ constrains.
	void			wSimulate(f32 step);
	void			enableBody(cPhyBody*);	// this will internally call body->enable()
	void			disableBody(cPhyBody*); // this will internally call body->disable()
	void			enableConstrain(cConstrain*); // enables a constrain if it was disabled
	void			disableConstrain(cConstrain*); // make sure all bodies associated are also disabled
	cPhyBody*		addBody();					// add a body to the system
	void			remBody(cPhyBody*);		// rem a body from the system
	cContactConstrain*	addNPConstrain();		// add a non penetration constrain
	cFrictionConstrain*	addFCConstrain();		// add a frictional contact constrain
	void			remConstrain(cConstrain*);	// you should not attempt to remove a NP constrain

	//@ [utility]
	void			setGravity(f32 x,f32 y,f32 z);
private:
	void			_autoDisable(f32 step);
	void			_bodyStep(f32 dt);
};

}

#endif //NEXTAR_SIMULATOR_VERSION_1