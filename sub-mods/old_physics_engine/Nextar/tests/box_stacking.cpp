
#include <windowsx.h>
#include <nextar.h>
#include <conio.h>
#include <xcontact.h>

using namespace nextar;

#define PIC_CRATE	124321
#define PIC_EARTH	112311
#define MTL_CRATE	143211
#define MTL_GRND	123311

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
iSpace*			quad = 0;
cSurface		ground;
cSurface		crate;
cPhyBody*		firstbody = 0;
cXuiSlider*		SliderX = 0;
cXuiSlider*		SliderY = 0;
cXuiSlider*		SliderZ = 0;
cXuiSlider*		SliderA = 0;
cXuiEditBox*	EdX = 0;
cXuiEditBox*	EdY = 0;
cXuiEditBox*	EdZ = 0;
s32				BoxCount = 0;
void loadTexturesAndMaterials()
{
	iTextureFactory::XSELFPTR->setDefaultTexFlags(TCF_MIPMAPS|TCF_32B);
	iTexture* pkT = iTextureFactory::XSELFPTR->loadTexture("D:|Nextar Build 1.01|resource|textures>crate.jpg",TCF_INVALID,PIC_CRATE);
	iMaterial* pkM = iMaterialFactory::XSELFPTR->crtMaterial(iMaterial::Solid,MTL_CRATE);
	pkM->setDiffuse(cColor4((u32)-1));
	pkM->setTexture(pkT);
	pkT->release();
	pkM->release();

	pkT = iTextureFactory::XSELFPTR->loadTexture("D:|Nextar Build 1.01|resource|textures>ground.jpg",TCF_INVALID,PIC_EARTH);
	pkM = iMaterialFactory::XSELFPTR->crtMaterial(iMaterial::Solid,MTL_GRND);
	pkM->setDiffuse(cColor4((u32)-1));
	pkM->setTexture(pkT);
	pkT->release();
	pkM->release();
}

void stackBox(int num,f32& y,const f32 x,f32 dim)
{
	BoxCount += num;
	f32 z =-((f32)num*dim)/2.0f;
	for(int i=0;i<num;i++)
	{
		iSceneNode* cube1 = cEngine::XSELFPTR->crtCube(
			quad,iMaterialFactory::XSELFPTR->getMaterial(MTL_CRATE),
			dim,dim,dim);
		cube1->queryCollisionObject()->getBody()->setPosition(0,dim/2+y,z);
		cube1->queryCollisionObject()->setSurfacePtr(&crate);
		cube1->release();
		z += dim;
	}
	y += (dim*2);
}

void stackTest()
{
	static f32 x = 0.0f;
	f32 dim = 0.50f;

	int stackstart = 19;
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
///	f32 y = dRandReal()*0.5f+0.1f;
//	f32 z = dRandReal()*0.5f+0.1f;
	f32 x = 0.4f;
	f32 y = 0.4f;
	f32 z = 0.4f;
	f32 ax = 1.5f*dRandReal();
	f32 ay = 1.5f*dRandReal();
	f32 az = 1.5f*dRandReal();
	f32 ang = 3.4f*dRandReal()/2;
	iSceneNode* cube1 = cEngine::XSELFPTR->crtCube(
		addto,iMaterialFactory::XSELFPTR->getMaterial(MTL_CRATE),x,y,z);
	// get position from edit
	f32 sx = (f32)atof(EdX->getCaption());
	f32 sy = (f32)atof(EdY->getCaption());
	f32 sz = (f32)atof(EdZ->getCaption());
	
//	cube1->queryCollisionObject()->getBody()->setPosition(px,py,pz);
//	cube1->queryCollisionObject()->getBody()->setRotationAxisAngle(-x,y,z,ang);
	cube1->queryCollisionObject()->getBody()->setPosition(0,1,0);

	cube1->release();
	cube1->queryCollisionObject()->setSurfacePtr(&crate);
	if(!firstbody)
		firstbody = cube1->queryCollisionObject()->getBody();
}

void resetBox()
{
	vec4_t a = {0};
	a[0] = (f32)SliderX->getValue()/100.0f;
	a[1] = (f32)SliderY->getValue()/100.0f;
	a[2] = (f32)SliderZ->getValue()/100.0f;
	f32 ang = (f32)SliderA->getValue()/100.0f;
	if( fabsr(a[0]) < 1e-8 &&
		fabsr(a[1]) < 1e-8 &&
		fabsr(a[2]) < 1e-8 )
	{
		a[0] = 1;
		a[0] = 0;
		a[0] = 0;
	}
	xQ4Normalize(a);
	firstbody->setPosition(3,10,3);
	firstbody->setRotationAxisAngle(a[0],a[1],a[2],ang);
	iSystem::XSELFPTR->getLogger().print(LL_INFO,"box { ax{%f, %f, %f, a(%f)} }\n",
		a[0],a[1],a[2],ang);
}

void printFirstBox()
{
	f32 ax,ay,az,ang;

	ang = 2.0f*acosr(firstbody->mkQ[QW]);
	f32 sina = sinr((ang)/2.0f);
	ax = firstbody->mkQ[QA]/sina;
	az = firstbody->mkQ[QB]/sina;
	ay = firstbody->mkQ[QC]/sina;
	f32* p = firstbody->mkWorld.pos;

	iSystem::XSELFPTR->getLogger().print(LL_INFO,"caught { %f, %f, %f } { ax{%f, %f, %f, a(%f)} }\n",
		p[0],p[1],p[2],ax,ay,az,ang);

}

void createLights(iSpace* light)
{
	vec4_t lightdir={(f32)rand() / (f32)RAND_MAX,(f32)rand() / (f32)RAND_MAX,(f32)rand() / (f32)RAND_MAX,0};
	xQ4Normalize(lightdir);
	iSceneNode* slight = cEngine::XSELFPTR->crtBasicLight(light,cLight::Directional,lightdir,500,-1);
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
	iSpace* root = cEngine::XSELFPTR->getRootSpace();
	quad = cEngine::XSELFPTR->crtQuadTreeSpace(root,center,500,500,4);
	// create a ground plane
	iSceneNode* gnd = cEngine::XSELFPTR->crtGroundPlane(quad,iMaterialFactory::XSELFPTR->getMaterial(MTL_GRND),21);
	gnd->queryCollisionObject()->setSurfacePtr(&ground);
	gnd->release();
	
	iCamera* cam = cEngine::XSELFPTR->crtFPSCamera(root);
	cEngine::XSELFPTR->setActiveCollisionRegion((iCollisionSpace*)quad->queryCollisionObject());
	cEngine::XSELFPTR->setActiveCamera(cam);
	for(int i=0; i<7; i++)
		createLights(quad);
	cam->release();
//	cube1->release();
	quad->release();
}

void createUI()
{
	cXuiManager::XSELFPTR->setCursorMode(cXuiManager::CurMode_CrossHairCenter,true);
	TextWnd1 = cXuiManager::XSELFPTR->crtStaticText(0,sizex(400,20),coordx(0,X_DEFAULT_WIN_HEIGHT-20),"FPS");
	TextWnd2 = cXuiManager::XSELFPTR->crtStaticText(0,sizex(400,20),coordx(0,X_DEFAULT_WIN_HEIGHT-40),"FPS");
	TextWnd3 = cXuiManager::XSELFPTR->crtStaticText(0,sizex(400,20),coordx(0,X_DEFAULT_WIN_HEIGHT-60),"FPS");
	SliderZ = cXuiManager::XSELFPTR->crtSlider(0,sizex(200,20),coordx(0,X_DEFAULT_WIN_HEIGHT-80),104,0,100);
	SliderY = cXuiManager::XSELFPTR->crtSlider(0,sizex(200,20),coordx(0,X_DEFAULT_WIN_HEIGHT-100),10,0,100);
	SliderX = cXuiManager::XSELFPTR->crtSlider(0,sizex(200,20),coordx(0,X_DEFAULT_WIN_HEIGHT-120),110,0,100);
	SliderA = cXuiManager::XSELFPTR->crtSlider(0,sizex(200,20),coordx(0,X_DEFAULT_WIN_HEIGHT-140),140,0,100);
	EdZ = cXuiManager::XSELFPTR->crtEditBox(0,sizex(150,20),coordx(X_DEFAULT_WIN_WIDTH-170 ,X_DEFAULT_WIN_HEIGHT-20),210);
	EdY = cXuiManager::XSELFPTR->crtEditBox(0,sizex(150,20),coordx(X_DEFAULT_WIN_WIDTH-170,X_DEFAULT_WIN_HEIGHT-40),120);
	EdX = cXuiManager::XSELFPTR->crtEditBox(0,sizex(150,20),coordx(X_DEFAULT_WIN_WIDTH-170,X_DEFAULT_WIN_HEIGHT-60),233);

	TextWnd1->release();
	TextWnd1->setTextColor(0xffffff00);
	TextWnd1->setDrawFlags(0);
	TextWnd2->release();
	TextWnd2->setTextColor(0xffffff00);
	TextWnd2->setDrawFlags(0);
	TextWnd3->release();
	TextWnd3->setTextColor(0xffffff00);
	TextWnd3->setDrawFlags(0);
	SliderX->release();
	SliderY->release();
	SliderZ->release();
	SliderA->release();
	EdX->release();
	EdY->release();
	EdZ->release();
}

void showFPS()
{
	float fps = 1.f/cTimer::skGameTimer.getElapsedTime();
	cStr str;
	str.format(" Current FPS: %f",50,fps);
	TextWnd1->setCaption(str);
	str.format(" Boxes In Scene: %d",50,BoxCount);
	TextWnd2->setCaption(str);
	str.format(" State Changes: %d",50,iRenderer::XSELFPTR->getTotalRenderStateChanges());
	TextWnd3->setCaption(str);
}

void setupWorld()
{
	loadTexturesAndMaterials();
	createWorld();
	createUI();
}

class myInputHandler : public iInputHandler
{
	virtual void handleInput( cInput* inp, s32 iCount )
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
				if( inp[i].mpkKBState->muKeysState[KEY_R] )
				{
					if(!wasdownr)
						resetBox();
					wasdownr = true;
				}
				else
					wasdownr = false;
				if(inp[i].mpkKBState->muKeysState[KEY_LCONTROL])
				{
					((cFPSCamera*)cEngine::XSELFPTR->getActiveCamera())->stopInputResponse();
					cXuiManager::XSELFPTR->setCursorMode(cXuiManager::CurMode_Normal,false);
				}
				else
				{
					((cFPSCamera*)cEngine::XSELFPTR->getActiveCamera())->startInputResponse();
					cXuiManager::XSELFPTR->setCursorMode(cXuiManager::CurMode_CrossHairCenter,true);
				}
			}
			
		}
	}
};

int main()
{
	iSystem::newSystem("D:|Nextar Build 1.01|resource");
	iRenderer::XSELFPTR->setBackgroundColor(0xff000000);
	setupWorld();
	myInputHandler handler;
	cInputSystem::XSELFPTR->registerInputHandler(&handler);
	// lets load the eidt and static font
	while(iSystem::XSELFPTR->run())
	{
		iSystem::XSELFPTR->mainLoop();
		showFPS();
	}
	iSystem::XSELFPTR->release();
	//_getch();
	return 0;
}
