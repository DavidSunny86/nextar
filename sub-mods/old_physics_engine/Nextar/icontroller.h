//@ Abhishek Dey
//@ May 30 2007
//@ September 23 2007

// a global controller, that can control
// things like animation. etc and work
// on user inputs
#ifndef IXCONTROLLER_H
#define IXCONTROLLER_H

#include <xtypes.h>

namespace nextar
{
class iController
{
public:
	virtual ~iController() {}
	virtual void update(x_time);
};
}

#endif//IXCONTROLLER_H