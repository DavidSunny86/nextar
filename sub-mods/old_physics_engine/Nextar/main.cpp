
#include <windowsx.h>
#include <nextar.h>
#include <conio.h>
#include <xcontact.h>
//#include <cfcconstrain.h>

using namespace nextar;

#define PIC_CRATE	124321
#define PIC_EARTH	112311
#define MTL_CRATE	143211
#define MTL_GRND	123311
#define ITERATIONSLIDER 12212

static unsigned long seed = 0;

unsigned long dRand()
{
  seed = (1664525L*seed + 1013904223L) & 0xffffffff;
  return seed;
}

void dRandSetSeed (unsigned long s)
{
  seed = s;
}

f32 dRandReal()
{
  return ((f32) dRand()) / ((f32) 0xffffffff);
}

cXuiStaticText* TextWnd1 = 0;
cXuiStaticText* TextWnd2 = 0;
cXuiStaticText* TextWnd3 = 0;
cXuiStaticText* TextWnd4 = 0;
iSpace*			quad = 0;
cSurface		ground;
cSurface		crate;
cPhyBody*		firstbody = 0;
cXuiSlider*		SliderY = 0;
cXuiSlider*		SliderZ = 0;
cXuiSlider*		SliderA = 0;
cXuiEditBox*	EdX = 0;
cXuiEditBox*	EdY = 0;
cXuiEditBox*	EdZ = 0;
s32				BoxCount = 0;

class myInputHandler : public iInputHandler,public cXuiContainer::iEventCallback
{
public:
	virtual void onEvent(iXuiWnd::GuiEvent e,iXuiWnd* wnd);
	virtual void handleInput( cInput* inp, s32 iCount );
};
myInputHandler handler;

void loadTexturesAndMaterials()
{
	iTextureFactory::hugabuga->setFlags(TCF_MIPMAPS|TCF_32B);
	iTexture* pkT = iTextureFactory::hugabuga->load("D:|Nextar Build 1.01|resource|textures>crate.jpg",TCF_INVALID,PIC_CRATE);
	iMaterial* pkM = iMaterialFactory::hugabuga->create(iMaterial::Solid,MTL_CRATE);
	pkM->setDiffuse(cColor4((u32)-1));
	pkM->setTexture(pkT);
	pkT->release();
	pkM->release();

	pkT = iTextureFactory::hugabuga->load("D:|Nextar Build 1.01|resource|textures>ground.jpg",TCF_INVALID,PIC_EARTH);
	pkM = iMaterialFactory::hugabuga->create(iMaterial::Solid,MTL_GRND);
	pkM->setDiffuse(cColor4((u32)-1));
	pkM->setTexture(pkT);
	pkT->release();
	pkM->release();
}

void stackBox(int num,f32& y,const f32 x,f32 dim)
{
	BoxCount += num;
	f32 z =-((f32)num*(dim+0.01f))/2.0f;
	for(int i=0;i<1;i++)
	{
		iEntity* cube1 = X_SharedPtr(cEngine)->crtCube(
			quad,iMaterialFactory::hugabuga->getType(MTL_CRATE),
			dim,dim,dim);
		cube1->queryCollisionObject()->getBody()->setPosition(0,dim/2+y,0);
		cube1->queryCollisionObject()->setSurfacePtr(&crate);
		cube1->release();
		z += (dim+0.01f);
	}
	y += (dim+0.01f);
}

void stackTest()
{
	static f32 x = 0.0f;
	f32 dim = 0.50f;

	int stackstart = 30;
	f32 y = 0;
	while(stackstart>0)
	{
		stackBox(stackstart,y,x,dim);
		stackstart-=2;
	}
	x += dim;
}


void randomBoxes(iSpace* addto)
{
	BoxCount++;

	f32 px = dRandReal()*20.5f-10.1f;
	f32 py = fabs(dRandReal()*20.5f);
	f32 pz = dRandReal()*20.5f-10.1f;

//	f32 x = dRandReal()*0.5f+0.1f;
//	f32 y = dRandReal()*0.5f+0.1f;
//	f32 z = dRandReal()*0.5f+0.1f;
	f32 x = 0.4f;
	f32 y = 0.4f;
	f32 z = 0.4f;
	f32 ax = 1.5f*dRandReal();
	f32 ay = 1.5f*dRandReal();
	f32 az = 1.5f*dRandReal();
	f32 ang = 3.4f*dRandReal()/2;
	iEntity* cube1 = X_SharedPtr(cEngine)->crtCube(addto,iMaterialFactory::hugabuga->getType(MTL_CRATE),x,y,z);
	
//	cube1->queryCollisionObject()->getBody()->setPosition(px,py,pz);
	cube1->queryCollisionObject()->getBody()->setRotationAxisAngle(-.4f,.4f,.4f,ang);
	cube1->queryCollisionObject()->getBody()->setPosition(0,3,0);

	cube1->release();
	cube1->queryCollisionObject()->setSurfacePtr(&crate);
	if(!firstbody)
		firstbody = cube1->queryCollisionObject()->getBody();
}

void createLights(iSpace* light)
{
	vec4_t lightdir={(f32)rand() / (f32)RAND_MAX,(f32)rand() / (f32)RAND_MAX,(f32)rand() / (f32)RAND_MAX,0};
	xQ4Normalize(lightdir);
	iEntity* slight = X_SharedPtr(cEngine)->crtBasicLight(light,cLight::Directional,lightdir,500,-1);
	slight->release();
}

void createWorld()
{
	ground.mfBounce = 0.00f;
	ground.mfMu = 0.0f;
	ground.mfSoftness = 0.0f;

	crate.mfBounce = 0.00f;
	crate.mfMu = N3DInfinity;
	crate.mfSoftness = 0.0f;

	vec3_t center = {0,0,0};
	iSpace* root = X_SharedPtr(cEngine)->getRootSpace();
	quad = X_SharedPtr(cEngine)->crtQuadTreeSpace(root,center,500,500,4);
	// create a ground plane
	iEntity* gnd = X_SharedPtr(cEngine)->crtGroundPlane(quad,iMaterialFactory::hugabuga->getType(MTL_GRND),21);
	gnd->queryCollisionObject()->setSurfacePtr(&ground);
	gnd->release();
	
	iCamera* cam = X_SharedPtr(cEngine)->crtFPSCamera(root);
	X_SharedPtr(cEngine)->setActiveCollisionRegion((iCollisionSpace*)quad->queryCollisionObject());
	X_SharedPtr(cEngine)->setActiveCamera(cam);
	for(int i=0; i<7; i++)
		createLights(quad);
	cam->release();
//	cube1->release();
	quad->release();
}

void createUI()
{
	cXuiSlider*		SliderStab = 0;
	cXuiSlider*		SliderIter = 0;

	X_SharedPtr(cXuiManager)->setCallback(&handler);
	X_SharedPtr(cXuiManager)->setCursorMode(cXuiManager::CurMode_CrossHairCenter,true);
	TextWnd1 = X_SharedPtr(cXuiManager)->crtStaticText(0,size2_x(400,20),coord2_x(0,X_DEFAULT_WIN_HEIGHT-20),"FPS");
	TextWnd2 = X_SharedPtr(cXuiManager)->crtStaticText(0,size2_x(400,20),coord2_x(0,X_DEFAULT_WIN_HEIGHT-40),"FPS");
	TextWnd3 = X_SharedPtr(cXuiManager)->crtStaticText(0,size2_x(400,20),coord2_x(0,X_DEFAULT_WIN_HEIGHT-60),"FPS");
	TextWnd4 = X_SharedPtr(cXuiManager)->crtStaticText(0,size2_x(400,20),coord2_x(0,X_DEFAULT_WIN_HEIGHT-80),"FPS");
	SliderStab = X_SharedPtr(cXuiManager)->crtSlider(0,size2_x(500,20),coord2_x(0,X_DEFAULT_WIN_HEIGHT-120),110123,0,1000);
	SliderIter = X_SharedPtr(cXuiManager)->crtSlider(0,size2_x(200,20),coord2_x(0,X_DEFAULT_WIN_HEIGHT-100),ITERATIONSLIDER,0,50);

	TextWnd1->release();
	TextWnd1->setTextColor(0xffffff00);
	TextWnd1->setDrawFlags(0);
	TextWnd2->release();
	TextWnd2->setTextColor(0xffffff00);
	TextWnd2->setDrawFlags(0);
	TextWnd3->release();
	TextWnd3->setTextColor(0xffffff00);
	TextWnd3->setDrawFlags(0);
	TextWnd4->release();
	TextWnd4->setTextColor(0xffff0000);
	TextWnd4->setDrawFlags(0);
	SliderStab->release();
	SliderIter->release();
}

void showFPS()
{
	float fps = 1.f/timer_t::skAsynchTimer.getElapsedTime();
	cStr str;
	str.format(" Current FPS: %f",50,fps);
	TextWnd1->setCaption(str);
	str.format(" Boxes In Scene: %d",50,BoxCount);
	TextWnd2->setCaption(str);
	str.format(" State Changes: %d",50,X_SharedPtr(iRenderer)->getTotalRenderStateChanges());
	TextWnd3->setCaption(str);
	str.format(" Stabalizer: %f;  Iterations = %d",90,cFrictionConstrain::sfStabalizer,X_SharedPtr(cSimulator)->getIterationCount());
	TextWnd4->setCaption(str);
}

void setupWorld()
{
	loadTexturesAndMaterials();
	createWorld();
	createUI();
}
void resetIterations(cXuiSlider* s)
{
	X_SharedPtr(cSimulator)->setIterationCount(s->getValue());
}

void resetStabalizer(cXuiSlider* s)
{
	f32 val = (f32)s->getValue() / 1000.f;
	cFrictionConstrain::setStabalizer(val);
}

void myInputHandler::onEvent(iXuiWnd::GuiEvent e,iXuiWnd* wnd)
{
	switch(e)
	{
	case iXuiWnd::OnSliderChange:
		switch(wnd->getKey())
		{
		case 110123:
			resetStabalizer(((cXuiSlider*)wnd));
			break;
		case ITERATIONSLIDER:
			resetIterations(((cXuiSlider*)wnd));
			break;
		};
		break;
	};
}

void myInputHandler::handleInput( cInput* inp, s32 iCount )
{
	static bool wasdown = false;
	static bool wasdowns = false;
	static bool wasdownr = false;
	for(s32 i=0; i<iCount; i++)
	{
		if(inp[i].meDevice==E_IDEVT_KEYBOARD)
		{
			if( inp[i].mpkKBState->muKeysState[KEY_B] )
			{
				if(!wasdown)
					randomBoxes(quad);
				wasdown = true;
			}
			else
				wasdown = false;
			if( inp[i].mpkKBState->muKeysState[KEY_S] )
			{
				if(!wasdowns)
					//printFirstBox();
					stackTest();
				wasdowns = true;
			}
			else
				wasdowns = false;
			
			if(inp[i].mpkKBState->muKeysState[KEY_LCONTROL])
			{
				((cFPSCamera*)X_SharedPtr(cEngine)->getActiveCamera())->stopInputResponse();
				X_SharedPtr(cXuiManager)->setCursorMode(cXuiManager::CurMode_Normal,false);
			}
			else
			{
				((cFPSCamera*)X_SharedPtr(cEngine)->getActiveCamera())->startInputResponse();
				X_SharedPtr(cXuiManager)->setCursorMode(cXuiManager::CurMode_CrossHairCenter,true);
			}
			
		}
		
	}
}

int main()
{
	iSystem::newSystem("D:|Nextar Build 1.01|resource");
	X_SharedPtr(iRenderer)->setBackgroundColor(0xff000000);
	setupWorld();
	X_SharedPtr(cInputSystem)->registerInputHandler(&handler);
	// lets load the eidt and static font
	while(X_SharedPtr(iSystem)->run())
	{
		X_SharedPtr(iSystem)->mainLoop();
		showFPS();
	}
	X_SharedPtr(iSystem)->release();
	//_getch();
	return 0;
}
