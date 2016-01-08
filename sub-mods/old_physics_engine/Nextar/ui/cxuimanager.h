
#ifndef CXUIMANAGER_H
#define CXUIMANAGER_H

#include <xtypesex.h>
#include <crepository.h>
#include <iobject.h>
#include <ui/ixuiskin.h>
#include <ui/cxuiwindows.h>
#include <ui/ixuifont.h>
#include <io/cinputsystem.h>


namespace nextar
{
class iXuiWnd;
class cXuiContainer;

#define N3DKeyRepeatInterval 400 // ms
#define N3DKeyRepeat 89 //ms

//@ this is iteself the root window
//@ this is also the font factory
class X_Api cXuiManager : public iInputHandler, public cXuiContainer
{

	SharedSingleInterface(cXuiManager);
	
public:
	enum CursorMode { CurMode_Wrapped,CurMode_CrossHairCenter,CurMode_Normal };
private:
	iXuiWnd*		mpkMouseOverWnd;
	iXuiWnd*		mpkFocusWnd;
	iXuiSkin*		mpkSkin;

	// cursor controller
	//
	CursorMode				meCursorMode;
	bool					mbCursorCentered;
	bool					mbCursorVisible;
	iXuiSkin::ECursorType	meCursor;
	coord2_x					mkCursorPos;
	coord2_x					mkCursorOffset;
	size2_x					mkCursorSize;

public:

	cXuiManager(const tchar* skinconf);
	virtual ~cXuiManager();
	_n_inline iXuiSkin*	getSkin() { return mpkSkin; }
	_n_inline iXuiWnd*	getFocusWnd() { return mpkFocusWnd; }
	_n_inline void		hideCursor() { mbCursorVisible = false; }
	_n_inline void		showCursor() { mbCursorVisible = true; }

	//@ set current cursor
	void		setCursorType(iXuiSkin::ECursorType t);
	//@ set the cursor mode
	//@ if not centered then the top left corner of the image is the cursor
	void		setCursorMode(CursorMode m,bool bCentered); 
	//@on the fly skin change
	void		setSkin(iXuiSkin* ptr); 
	//@ set focus wnd
	void		setFocusWnd(iXuiWnd*);	

	//@ creation functions
	cXuiContainer*		crtContainer(cXuiContainer* pkParent,const size2_x&,const coord2_x&,u32 uStyle,const tchar* sCap = 0);// create functions
	iXuiWnd*			crtButton(cXuiContainer* pkParent,const size2_x&,const coord2_x&,u32 uId,const tchar* sCap = 0);
	iXuiWnd*			crtCheckbox(cXuiContainer* pkParent,const size2_x&,const coord2_x&,u32 uId,const tchar* sCap = 0);
	iXuiWnd*			crtRadioButton(cXuiContainer* pkParent,const size2_x&,const coord2_x&,u32 uId,u32 uGId,const tchar* sCap = 0);
	cXuiStaticText*		crtStaticText(cXuiContainer* pkParent,const size2_x&,const coord2_x&,const tchar* sCap = 0);
	cXuiListBox*		crtListBox(cXuiContainer* pkParent,const size2_x&,const coord2_x&,u32 uId,bool bMultiSelection = false,const tchar* sCap = 0);
	cXuiComboBox*		crtComboBox(cXuiContainer* pkParent,const size2_x&,const coord2_x&,u32 uId);
	cXuiSlider*			crtSlider(cXuiContainer* pkParent,const size2_x&,const coord2_x&,u32 uId,s32 iMin,s32 iMax);
	cXuiProgressBar*	crtProgressBar(cXuiContainer* pkParent,const size2_x&,const coord2_x&,u32 uId,const tchar* sCap = 0);
	cXuiTabControl*		crtTabControl(cXuiContainer* pkParent,const size2_x&,const coord2_x&,u32 uId,u32 uStyle);
	cXuiEditBox*		crtEditBox(cXuiContainer* pkParent,const size2_x&,const coord2_x&,u32 uId,const tchar* sText = 0);

public:

	virtual void onDraw(); //@ draw

private:
	// [internal]
	virtual void update();
	virtual void handleInput( cInput*, s32 iCount ); // input handling
	virtual void onKeyEvent(const cKeyEvent*);
	virtual void onMouseEvent(const cMouseEvent*);
};

}

#endif // CXUIMANAGER_H