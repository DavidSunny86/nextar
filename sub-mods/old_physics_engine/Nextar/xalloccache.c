//@ Abhishek Dey
//@ Oct 25 2007

#include <xmath_c.h>
#include <xtypes_c.h>

static transform*	sparrkPRAlloc[2] = {0};
//@ cached allocation for posr 
transform* allocTransform()
{
	transform* ret = 0;
	if(sparrkPRAlloc[0])
	{
		ret = sparrkPRAlloc[0];
		sparrkPRAlloc[0] = sparrkPRAlloc[1];
		sparrkPRAlloc[1] = 0;
	}
	else
		ret = (transform*)allocMemoryAligned(sizeof(transform));

	return ret;
}

//@ free posr
void freeTransform(transform* p)
{
	if(sparrkPRAlloc[1])
	{
		freeMemoryAligned(p);
	}
	else
	{
		if(sparrkPRAlloc[0]) sparrkPRAlloc[1] = p;
		else sparrkPRAlloc[0] = p;
	}
}
