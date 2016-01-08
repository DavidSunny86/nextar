
//@ Abhishek Dey
//@ Oct 18 2007

//@ Info: iObject as a base class is not suitable for the purpose of,
//@ plugin communication, this pure interface implementation makes things 
//@ correct. Also all classes with self ptrs should be derived from here.

#ifndef X_PUREINTERFACE_H
#define X_PUREINTERFACE_H
#include <xtypes.h>

namespace nextar
{

#define INTERFACE_VERSION(Major,Minor,Micro)	pi_version(Major << 24 | Minor << 16 | Micro)
#define IMPLEMENT_INTERFACE( Name, Major, Minor, Micro ) public: \
	struct Traits \
	{\
		static const char* gName() {\
			return #Name;\
		}\
		static pi_version gVersion() {\
			return INTERFACE_VERSION(Major,Minor,Micro);\
		}\
	};


class iPureInterface
{
	IMPLEMENT_INTERFACE(iPureInterface,1,0,0);
protected:
	virtual ~iPureInterface() {}
public:
	//
	virtual void	addRef() = 0;
	virtual void	release() = 0;
	virtual u32		getRefCount() = 0;
};

}