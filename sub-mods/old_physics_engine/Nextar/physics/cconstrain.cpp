//@ Abhishek Dey
//@ Nov 8 2007

#include "cconstrain.h"

using namespace nextar;
cConstrain::cConstrain() : mpkNext(0), mpkPrev(0),muFlags(0)
{
	mpkBody[0] = 0;
	mpkBody[1] = 0;
}