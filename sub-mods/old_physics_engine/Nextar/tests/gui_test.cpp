
#include <windowsx.h>
#include <nextar.h>
#include <conio.h>

using namespace nextar;

void buildComplex()
{
	cXuiTabControl* ctrl = cXuiManager::XSELFPTR->crtTabControl(0,sizex(250,300),coordx(0,10),232112,cXuiContainer::CloseBox);

	cXuiManager::XSELFPTR->crtContainer(ctrl,sizex(370,280),coordx(0,10),0,"TabOne")->release();
	cXuiManager::XSELFPTR->crtContainer(ctrl,sizex(370,280),coordx(0,10),0,"TabTwo")->release();
	cXuiManager::XSELFPTR->crtContainer(ctrl,sizex(370,280),coordx(0,10),0,"TabThree")->release();
	cXuiManager::XSELFPTR->crtContainer(ctrl,sizex(370,280),coordx(0,10),0,"TabFour")->release();
	cXuiManager::XSELFPTR->crtContainer(ctrl,sizex(370,280),coordx(0,10),0,"TabFive")->release();
	cXuiManager::XSELFPTR->crtContainer(ctrl,sizex(370,280),coordx(0,10),0,"TabSix")->release();
	cXuiManager::XSELFPTR->crtContainer(ctrl,sizex(370,280),coordx(0,10),0,"TabSeven")->release();
	cXuiManager::XSELFPTR->crtContainer(ctrl,sizex(370,280),coordx(0,10),0,"TabEight")->release();

	ctrl->update();
	ctrl->release();
}

void buildSimple()
{
	const tchar* List[] = 
	{
		"one","two","three","foue","five","x","seve","eit","nine","10"
	};
	const tchar *Combo[] =
	{
		"1","2","3",":)",":(",":|","7","8",":0",":|"
	};

	cXuiContainer* c = (cXuiContainer*)cXuiManager::XSELFPTR->crtContainer(0,sizex(250,250),coordx(0,0),cXuiContainer::CloseBox|cXuiContainer::BorderFrame|cXuiContainer::TitleBar,"Window");
	c->release();

	cXuiManager::XSELFPTR->crtButton(c,sizex(80,20),coordx(30,55),1,"Button")->release();
	cXuiManager::XSELFPTR->crtCheckbox(c,sizex(80,20),coordx(30,80),1,"Good")->release();
	cXuiManager::XSELFPTR->crtRadioButton(c,sizex(80,20),coordx(30,150),4231,12,"One")->release();
	cXuiManager::XSELFPTR->crtRadioButton(c,sizex(80,20),coordx(30,170),211,12,"Two")->release();
	cXuiManager::XSELFPTR->crtRadioButton(c,sizex(80,20),coordx(30,190),11,12,"Three")->release();
	cXuiManager::XSELFPTR->crtSlider(c,sizex(80,20),coordx(30,210),121,0,100)->release();
	cXuiManager::XSELFPTR->crtEditBox(c,sizex(120,30),coordx(90,25),23)->release();
	cXuiManager::XSELFPTR->crtProgressBar(c,sizex(120,20),coordx(30,100),121)->release();

	cXuiListBox* box1 = (cXuiListBox*)cXuiManager::XSELFPTR->crtListBox(c,sizex(90,60),coordx(150,150),311,true,"Simple");
	cXuiComboBox* box2 = cXuiManager::XSELFPTR->crtComboBox(c,sizex(90,60),coordx(120,50),311);
	
	for(int i=0;i<10;i++)
	{
		box2->addItem(Combo[i]);
		box1->addItem(List[i]);
	}

	box1->release();
	box2->release();
	
}

void build()
{
	iCamera* cam = cEngine::XSELFPTR->crtFPSCamera(cEngine::XSELFPTR->getRootSpace());
	cEngine::XSELFPTR->setActiveCamera(cam);
	cam->release();
	buildSimple();
}

// draw a simple windowc
void main()
{
	iSystem::newSystem("D:|Nextar Build 1.01|resource");
	iRenderer::XSELFPTR->setBackgroundColor(0xff00ff00);

	build();

	// lets load the eidt and static font
	while(iSystem::XSELFPTR->run())
	{
		iSystem::XSELFPTR->mainLoop();
	}
	iSystem::XSELFPTR->release();
	_getch();
}