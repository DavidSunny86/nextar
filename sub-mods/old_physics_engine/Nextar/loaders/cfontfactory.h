//@ Abhishek Dey
//@ Nov 23 2007

#ifndef NEXATR_IFONTFACTORY_H
#define NEXATR_IFONTFACTORY_H

#include <ui/ixuifont.h>
#include <loaders/iloaderfactory.h>

namespace nextar
{
class X_Api cFontFactory : public iFactory<iXuiFont>
{
protected:
	SharedSingleInterface(cFontFactory);
public:
	cFontFactory();
	virtual ~cFontFactory();
	virtual iObject* load(iFileReader*,u32 uKey = X_InvalidKey);
public:
};
}

#endif //NEXATR_IFONTFACTORY_H
