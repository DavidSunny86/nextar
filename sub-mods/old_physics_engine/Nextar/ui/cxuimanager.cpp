//@ Abhishek Dey
//@ Aug 25	07
//@ Sep 23 07
//@ Oct 02 07

#include <ctimer.h>
#include <ui/cxuimanager.h>
#include <ui/cxuiwindows.h>
#include <graphics/irenderer.h>
#include <io/cinputsystem.h>
#include <loaders/ixuiskinfactory.h>

namespace nextar
{
SharedSingleInterfaceSetup(cXuiManager);

cXuiManager::cXuiManager(const tchar* skinconf) : mpkSkin(0), cXuiContainer(0,(u32)0),
mbCursorVisible(true),meCursorMode(CurMode_Normal),mbCursorCentered(false),mkCursorOffset(0),
meCursor(iXuiSkin::CT_Normal)
{
	RefObjectInit(cXuiManager);
	SharedSingleInterfaceInit(cXuiManager);

	// setup the default key interval
	X_SharedPtr(cInputSystem)->registerInputHandler(this);
	mpkFocusWnd = this;
	mpkMouseOverWnd = this;
	
	mkDim.x = 1.f;
	mkDim.y = 1.f;
	meType = iXuiWnd::xGuiManager;
	mpkSkin = X_SharedPtr(iXuiSkinFactory)->loadType(skinconf,100223);	//TODO:: check the key
	X_Assert(mpkSkin);
	mpkSkin->initSkinParameters();
	update();
}

cXuiManager::~cXuiManager()
{
	if(mpkSkin) 
		mpkSkin->release();

	SharedSingleInterfaceExit(cXuiManager);
}

void cXuiManager::setSkin(iXuiSkin* pkSkin)
{
	X_Assert(pkSkin);
	if(mpkSkin) 
		mpkSkin->release();
	mpkSkin = pkSkin;
	mpkSkin->addRef();
	update();
}

void cXuiManager::setFocusWnd(iXuiWnd* pkFocus)
{
	X_Assert(mpkFocusWnd);
	X_Assert(pkFocus);

	if(mpkFocusWnd!=pkFocus)
	{
		mpkFocusWnd->onGuiEvent(iXuiWnd::OnFocusOut,this);
		mpkFocusWnd->setHasFocus(false);
		mpkFocusWnd = pkFocus;
		mpkFocusWnd->setHasFocus(true);
		mpkFocusWnd->onGuiEvent(iXuiWnd::OnFocusIn,this);
	}
}

void cXuiManager::setCursorType(iXuiSkin::ECursorType t)
{
	X_Assert(mpkSkin);
	meCursor = t;
	mkCursorSize = mpkSkin->getCursorSize(t);
}

void cXuiManager::setCursorMode(cXuiManager::CursorMode m,bool bCentered)
{
	meCursorMode = m;
	mbCursorCentered = bCentered;
	if(meCursorMode==CurMode_CrossHairCenter)
		setCursorType(iXuiSkin::CT_CrossHair);
	else
		setCursorType(iXuiSkin::CT_Normal);
	if(bCentered)
	{
		mkCursorOffset.x =-(mkCursorSize.dx >> 1);
		mkCursorOffset.y =-(mkCursorSize.dy >> 1);
	}
	else
	{
		mkCursorOffset.x = 0;
		mkCursorOffset.y = 0;
	}
}

void cXuiManager::handleInput(nextar::cInput *inp, s32 iCount)
{
	static cMouseEvent  mouse = {0};
	static cKeyEvent key = {0};
	static u32		 keyupdatetime = 0;
	static u8		 lastkeydown = -1;
	static bool		 lastkeyheld = false;

	for(s32 i = 0; i < iCount; i++)
	{
		switch(inp[i].meDevice)
		{
		case E_IDEVT_MOUSE:
			// generate mouse specific data
			{
				cMouseState* s = inp[i].mpkMState;
				mkCursorPos += s->mkRelCoord;
				const size2_x& d = X_SharedPtr(iRenderer)->getScreenSize();
				// normalize
				switch(meCursorMode)
				{
				case CurMode_Normal:
					if( mkCursorPos.x < 0 ) mkCursorPos.x = 0;
					if( mkCursorPos.y < 0 ) mkCursorPos.y = 0;
					if( mkCursorPos.x > d.dx ) mkCursorPos.x = d.dx;
					if( mkCursorPos.y > d.dy ) mkCursorPos.y = d.dy;
					break;
				case CurMode_Wrapped:
					if( mkCursorPos.x < 0 ) mkCursorPos.x = d.dx+(mkCursorPos.x%d.dx);
					if( mkCursorPos.y < 0 ) mkCursorPos.y = d.dy+(mkCursorPos.y%d.dy);
					if( mkCursorPos.x > d.dx ) mkCursorPos.x = d.dx-(mkCursorPos.x%d.dx);
					if( mkCursorPos.y > d.dy ) mkCursorPos.y = d.dy-(mkCursorPos.y%d.dy);
					break;
				case CurMode_CrossHairCenter:
					mkCursorPos.x = d.dx >> 1;
					mkCursorPos.y = d.dy >> 1;
					break;
				};
				mouse.x = (s16)mkCursorPos.x;
				mouse.y = (s16)mkCursorPos.y;
				mouse.muCtrlKeys = key.muFlags;
				if(s->muButtonState!=s->muLastButtonState)
				{
					// button events
					if(s->marrbButtonState[0] ^ s->marrbLastButtonState[0])
					{					
						if(s->marrbButtonState[0])
						mouse.meType = cMouseEvent::LBDown;
						else
						mouse.meType = cMouseEvent::LBUp;
					}
					if(s->marrbButtonState[1] ^ s->marrbLastButtonState[1])
					{
						mouse.meType = s->marrbButtonState[1] ? 
							cMouseEvent::RBDown : cMouseEvent::RBUp;
					}
					onMouseEvent(&mouse);
				}
				else if((s->mkRelCoord.x|s->mkRelCoord.y))
				{
					if(s->marrbButtonState[0])
						mouse.meType = cMouseEvent::MDrag;
						// this is drag
					else
						mouse.meType = cMouseEvent::MMove;
					onMouseEvent(&mouse);
				}
				else if(s->miRelWheel)
				{
					// mouse wheel move
					// scroll notification
					mouse.meType = cMouseEvent::MScroll;
					mouse.m = (s16)s->miRelWheel;		
					onMouseEvent(&mouse);
				}
			}
			break;
		case E_IDEVT_KEYBOARD:
			// generate keyboard data
			{				
				cKeyboardState* k = inp->mpkKBState;
				key.muFlags = 0;

				if(k->muKeysState[KEY_LMENU] || k->muKeysState[KEY_RMENU])
					key.muFlags = cKeyEvent::Alt;

				if(k->muKeysState[KEY_LCONTROL] || k->muKeysState[KEY_RCONTROL])
					key.muFlags |= cKeyEvent::Ctrl;

				bool shift = false;
				if(k->muKeysState[KEY_LSHIFT] || k->muKeysState[KEY_RSHIFT])
				{	
					if(!(k->muFlags & cKeyboardState::CapsLockOn))
						shift = true;
					key.muFlags |= cKeyEvent::Shift;
				}
				else if((k->muFlags & cKeyboardState::CapsLockOn))
					shift = true;

				key.muKeyValue = -1;
				u32 uCurTime = timer_t::skAsynchTimer.getFrameTime();

				// first check if last key is down
				if(lastkeydown!=(u8)-1)
				{
					if(k->muKeysState[lastkeydown])
					{
						// we see the repeat delay and skip
						// if too short repeat
						if(lastkeyheld)
						{
							if(uCurTime-keyupdatetime >= N3DKeyRepeat)
							{
								// so the last key is still the one stealing attention
								key.muKeyValue = lastkeydown;
								key.muFlags |= (cKeyEvent::KeyPressed|cKeyEvent::RepeatMask);
								keyupdatetime = uCurTime;
								goto post;
							}
						}
						else
						{
							if(uCurTime-keyupdatetime >= N3DKeyRepeatInterval)
							{
								// so we want repeat
								key.muKeyValue = lastkeydown;
								key.muFlags |= (cKeyEvent::KeyPressed|cKeyEvent::RepeatMask);
								keyupdatetime = uCurTime;
								lastkeyheld = true;
								goto post;
							}
						}
						goto dontpost;
					}
					else
					{
						lastkeyheld = false;
						lastkeydown = -1;
						// we can post a key up event here
						// currently the engine does not require it
						// so we skip this part.
					}
				}
				
				for(u8 i=0; i<KEY_NONCONTROL_COUNT; i++)
				{	
					if(k->muKeysState[i] && i!=lastkeydown)
					{
						key.muKeyValue = lastkeydown = i;
						keyupdatetime = uCurTime;
						key.muFlags |= (cKeyEvent::KeyPressed);
						goto post;
					}
				}
				break;
post:
				key.mcChar = X_SharedPtr(cInputSystem)->getAsciiForKey((KEYCODE)key.muKeyValue, shift);
				onKeyEvent(&key);

dontpost:;
			}
			break;
		case E_IDEVT_JOYSTICK:
			// generate joystick data
			{
			}
			break;
		}
	}
}

void cXuiManager::onDraw()
{
	onRenderChildren();
	// render the cursor
	if(mbCursorVisible)
		mpkSkin->drawCursor(meCursor,mkCursorPos+mkCursorOffset);
}

// handling key event
void cXuiManager::onKeyEvent(const cKeyEvent *e)
{
	X_Assert(mpkFocusWnd);
	if(mpkFocusWnd != this)
		mpkFocusWnd->onKeyEvent(e);
}

// handling on event
void cXuiManager::onMouseEvent(const cMouseEvent *e)
{
	switch(e->meType)
	{
	case cMouseEvent::MMove:

		{
		X_Assert( mpkMouseOverWnd );
		
		iXuiWnd* at = getWndAt(e->x,e->y);
		if( mpkMouseOverWnd != at )
		{
			mpkMouseOverWnd->onGuiEvent(OnMouseLeave,this);
			mpkMouseOverWnd = at;
			X_Assert( mpkMouseOverWnd );
			mpkMouseOverWnd->onGuiEvent(OnMouseEnter,this);
		}
		else if(mpkMouseOverWnd != this)
			mpkMouseOverWnd->onMouseEvent(e);
		break;
		}

	case cMouseEvent::RBUp:
	case cMouseEvent::LBUp:

		{
		iXuiWnd* at = getWndAt(e->x,e->y);
		if( mpkMouseOverWnd != at )
		{
			mpkMouseOverWnd->onGuiEvent(OnMouseLeave,this);
			mpkMouseOverWnd = at;
			X_Assert( mpkMouseOverWnd );
			mpkMouseOverWnd->onGuiEvent(OnMouseEnter,this);
		}
		else if(mpkMouseOverWnd != this)
			mpkMouseOverWnd->onMouseEvent(e);
		break;
		}

	case cMouseEvent::MScroll:
		X_Assert(mpkFocusWnd);
		if(mpkMouseOverWnd != this)
			mpkFocusWnd->onMouseEvent(e);
		break;
	
	case cMouseEvent::RBDown:
	case cMouseEvent::LBDown:
	case cMouseEvent::MDrag:

		if(mpkMouseOverWnd != this)
		{
			if(mpkMouseOverWnd != mpkFocusWnd)
			{
				X_Assert( mpkMouseOverWnd );
				if(mpkMouseOverWnd->getCanHaveFocus())
					setFocusWnd(mpkMouseOverWnd);
				mpkMouseOverWnd->bringToFront();
			}
			mpkMouseOverWnd->onMouseEvent(e);
		}
		else if(mpkMouseOverWnd != mpkFocusWnd)
		{
			mpkFocusWnd->onGuiEvent(iXuiWnd::OnFocusOut,this);
			mpkFocusWnd->setHasFocus(false);
			mpkFocusWnd = this;
		}
		break;
	}
}

void cXuiManager::update()
{
	cXuiContainer::update();
	setCursorType(meCursor);
}

// create functions
// frame window
cXuiContainer* cXuiManager::crtContainer(cXuiContainer* pkParent,const size2_x& siz,const coord2_x& pos,u32 uStyle,const tchar* sCap)
{
	const size2_x& kSDim = X_SharedPtr(iRenderer)->getScreenSize();
	f32 dx = 1.f/(f32)kSDim.dx;
	f32 dy = 1.f/(f32)kSDim.dy;

	if(!pkParent) pkParent = this;
	cXuiContainer* pkFrame = new cXuiContainer(pkParent,uStyle);
	pkFrame->setPos((f32)pos.x*dx,(f32)pos.y*dy);
	pkFrame->setDim((f32)siz.dx*dx,(f32)siz.dy*dy);
	if(sCap)
		pkFrame->setCaption(sCap);
	pkParent->update();
	pkFrame->update();
	return pkFrame;
}

// button
iXuiWnd* cXuiManager::crtButton(cXuiContainer* pkParent,const size2_x& siz,const coord2_x& pos,u32 uId,const tchar* sCap)
{
	const size2_x& kSDim = X_SharedPtr(iRenderer)->getScreenSize();
	f32 dx = 1.f/(f32)kSDim.dx;
	f32 dy = 1.f/(f32)kSDim.dy;

	if(!pkParent) pkParent = this;
	cXuiButton* pkButton = new cXuiButton(pkParent,uId);
	pkButton->setPos((f32)pos.x*dx,(f32)pos.y*dy);
	pkButton->setDim((f32)siz.dx*dx,(f32)siz.dy*dy);
	if(sCap)
		pkButton->setCaption(sCap);
	pkButton->update();
	return pkButton;
}

// chk box
iXuiWnd* cXuiManager::crtCheckbox(cXuiContainer* pkParent,const size2_x& siz,const coord2_x& pos,u32 uId,const tchar* sCap)
{
	
	const size2_x& kSDim = X_SharedPtr(iRenderer)->getScreenSize();
	f32 dx = 1.f/(f32)kSDim.dx;
	f32 dy = 1.f/(f32)kSDim.dy;

	if(!pkParent) pkParent = this;
	iXuiWnd* pkW = new cXuiCheckbox(pkParent,uId);
	pkW->setPos((f32)pos.x*dx,(f32)pos.y*dy);
	pkW->setDim((f32)siz.dx*dx,(f32)siz.dy*dy);
	pkW->setCaption(sCap);
	pkW->update();
	return pkW;
}

// chk box
iXuiWnd* cXuiManager::crtRadioButton(cXuiContainer* pkParent,const size2_x& siz,const coord2_x& pos,u32 uId,u32 uGId,const tchar* sCap)
{
	const size2_x& kSDim = X_SharedPtr(iRenderer)->getScreenSize();
	f32 dx = 1.f/(f32)kSDim.dx;
	f32 dy = 1.f/(f32)kSDim.dy;

	if(!pkParent) pkParent = this;
	iXuiWnd* pkW = new cXuiRadioButton(pkParent,uId,uGId);
	pkW->setPos((f32)pos.x*dx,(f32)pos.y*dy);
	pkW->setDim((f32)siz.dx*dx,(f32)siz.dy*dy);
	pkW->setCaption(sCap);
	pkW->update();
	return pkW;
}

// static text
cXuiStaticText* cXuiManager::crtStaticText(cXuiContainer* pkParent,const size2_x& siz,const coord2_x& pos,const tchar* sCap)
{
	const size2_x& kSDim = X_SharedPtr(iRenderer)->getScreenSize();
	f32 dx = 1.f/(f32)kSDim.dx;
	f32 dy = 1.f/(f32)kSDim.dy;

	if(!pkParent) pkParent = this;
	cXuiStaticText* pkW = new cXuiStaticText(pkParent,sCap);
	pkW->setPos((f32)pos.x*dx,(f32)pos.y*dy);
	pkW->setDim((f32)siz.dx*dx,(f32)siz.dy*dy);
	pkW->update();
	return pkW;
}

// create list box
cXuiListBox* cXuiManager::crtListBox(cXuiContainer* pkParent,const size2_x& siz,const coord2_x& pos,u32 uId,bool bM,const tchar* sCap)
{
	const size2_x& kSDim = X_SharedPtr(iRenderer)->getScreenSize();
	f32 dx = 1.f/(f32)kSDim.dx;
	f32 dy = 1.f/(f32)kSDim.dy;

	if(!pkParent) pkParent = this;
	cXuiListBox* pkW = new cXuiListBox(pkParent,uId);
	pkW->setPos((f32)pos.x*dx,(f32)pos.y*dy);
	pkW->setDim((f32)siz.dx*dx,(f32)siz.dy*dy);
	pkW->setMultiSelection(bM);
	pkW->setCaption(sCap);
	pkW->update();
	return pkW;
}

cXuiComboBox* cXuiManager::crtComboBox(cXuiContainer* pkParent,const size2_x& siz,const coord2_x& pos,u32 uId)
{
	const size2_x& kSDim = X_SharedPtr(iRenderer)->getScreenSize();
	f32 dx = 1.f/(f32)kSDim.dx;
	f32 dy = 1.f/(f32)kSDim.dy;

	if(!pkParent) pkParent = this;
	cXuiComboBox* pkW = new cXuiComboBox(pkParent,uId);
	pkW->setPos((f32)pos.x*dx,(f32)pos.y*dy);
	pkW->setDim((f32)siz.dx*dx,(f32)siz.dy*dy);
	pkW->update();
	return pkW;
}

// slider
cXuiSlider* cXuiManager::crtSlider(cXuiContainer* pkParent,const size2_x& siz,const coord2_x& pos,u32 uId,s32 iMin,s32 iMax)
{
	const size2_x& kSDim = X_SharedPtr(iRenderer)->getScreenSize();
	f32 dx = 1.f/(f32)kSDim.dx;
	f32 dy = 1.f/(f32)kSDim.dy;

	if(!pkParent) pkParent = this;
	cXuiSlider* pkW = new cXuiSlider(pkParent,uId);
	pkW->setPos((f32)pos.x*dx,(f32)pos.y*dy);
	pkW->setDim((f32)siz.dx*dx,(f32)siz.dy*dy);
	pkW->setRange(iMin,iMax);
	pkW->update();
	return pkW;
}

cXuiProgressBar* cXuiManager::crtProgressBar(cXuiContainer* pkParent,const size2_x& siz,const coord2_x& pos,u32 uId,const tchar* sCap)
{
	const size2_x& kSDim = X_SharedPtr(iRenderer)->getScreenSize();
	f32 dx = 1.f/(f32)kSDim.dx;
	f32 dy = 1.f/(f32)kSDim.dy;

	if(!pkParent) pkParent = this;
	cXuiProgressBar* pkW = new cXuiProgressBar(pkParent,uId);
	pkW->setPos((f32)pos.x*dx,(f32)pos.y*dy);
	pkW->setDim((f32)siz.dx*dx,(f32)siz.dy*dy);
	if(sCap)
		pkW->setCaption(sCap);
	else
		pkW->setCaption("loading...");
	pkW->update();
	return pkW;
}

cXuiTabControl* cXuiManager::crtTabControl(cXuiContainer* pkParent,const size2_x& siz,const coord2_x& pos,u32 uId,u32 uStyle)
{
	const size2_x& kSDim = X_SharedPtr(iRenderer)->getScreenSize();
	f32 dx = 1.f/(f32)kSDim.dx;
	f32 dy = 1.f/(f32)kSDim.dy;

	if(!pkParent) pkParent = this;
	cXuiTabControl* pkFrame = new cXuiTabControl(pkParent,uId,uStyle);
	pkFrame->setPos((f32)pos.x*dx,(f32)pos.y*dy);
	pkFrame->setDim((f32)siz.dx*dx,(f32)siz.dy*dy);
	pkFrame->update();
	return pkFrame;
}

cXuiEditBox* cXuiManager::crtEditBox(cXuiContainer* pkParent,const size2_x& siz,const coord2_x& pos,u32 uId,const tchar* sTxt)
{
	const size2_x& kSDim = X_SharedPtr(iRenderer)->getScreenSize();
	f32 dx = 1.f/(f32)kSDim.dx;
	f32 dy = 1.f/(f32)kSDim.dy;

	if(!pkParent) pkParent = this;
	cXuiEditBox* pkWnd = new cXuiEditBox(pkParent,uId);
	pkWnd->setPos((f32)pos.x*dx,(f32)pos.y*dy);
	pkWnd->setDim((f32)siz.dx*dx,(f32)siz.dy*dy);
	if(sTxt)
		pkWnd->setCaption(sTxt);

	pkWnd->update();
	return pkWnd;
}


}