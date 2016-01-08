//@ Abhishek Dey
//@ Nov 2 2007

#include <cbasiclight.h>
#include <cengine.h>

using namespace nextar;

cBasicLight::cBasicLight(iSpace* pkParent,cLight::Type t,const f32* dir,f32 rad,u32 color)
{
	mkLight.meType = t;
	mkLight.mfRadius = rad;
	mkLight.mkDiffuse = cColor4(color);
	mkLight.mkAmbient = cColor4(0.2f,0.2f,0.2f);
	mkLight.mkSpecular = cColor4(1.0f,1.0f,1.0f);
	if(dir)
	{
		mkLight.mkVector[0]=dir[0];
		mkLight.mkVector[1]=dir[1];
		mkLight.mkVector[2]=dir[2];
	}
	if(pkParent) setParent(pkParent);
}

void cBasicLight::onUpdate(nextar::iCamera *pkC)
{
	X_SharedPtr(cEngine)->addHalo(&mkLight);
}