//@ Abhishek Dey
//@ Aug 18 2007
//@ Sep 23 2007
//@ Oct 10 2007
#include "ctimer.h"
#include "isystem.h"
#include "graphics/irenderer.h"
#include "ui/cxuimanager.h"
#include "ui/cxuiwindows.h"
#include "ui/ixuiskin.h"
#include "loaders/itexturefactory.h"

using namespace nextar;

namespace nextar
{

#define definevariable(variable)	i2d	variable = -1; 
definevariable(CV_WinXOffsetX);
definevariable(CV_WinXOffsetY);
definevariable(CV_XButtonWidth);
definevariable(CV_XButtonHeight);
definevariable(CV_WinEdgeLeft);
definevariable(CV_WinEdgeTop);
definevariable(CV_WinEdgeRight);
definevariable(CV_WinEdgeBottom);
definevariable(CV_ScrollButtonWidth);
definevariable(CV_ScrollButtonHeight);
definevariable(CV_LBEdgeLeft);
definevariable(CV_LBEdgeTop);
definevariable(CV_LBEdgeRight);
definevariable(CV_LBEdgeBottom);
definevariable(CV_LBMargin);
definevariable(CV_LBBorder);
definevariable(CV_StaticTextHeight);
definevariable(CV_DropDownButtonWidth);
definevariable(CV_DropDownButtonHeight);
definevariable(CV_CBDropDownBorer);
definevariable(CV_SliderButtonWidth);
definevariable(CV_SliderButtonHeight);
definevariable(CV_SliderTrackHeight);
definevariable(CV_ProgressBarHeight);
definevariable(CV_ProgressBarBorderWidth);
definevariable(CV_ProgressBarBorderHeight);
definevariable(CV_TabHeight);
definevariable(CV_TabBorder);
definevariable(CV_TabEdgeLeft);
definevariable(CV_TabEdgeTop);
definevariable(CV_TabEdgeRight);
definevariable(CV_TabEdgeBottom);
definevariable(CV_EditBoxHeight);
definevariable(CV_EditBoxBorderWidth);
definevariable(CV_EditBoxBorderHeight);
s32 CV_CaretColor;
s32 CV_StaticFontColor;
s32 CV_EditFontColor;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//					iXuiWnd
// ctor
iXuiWnd::iXuiWnd(cXuiContainer* pkParent,Type eType,u32 uInitFlags) : mkCaption(""),
muFlags(uInitFlags),
meType(eType),mpkNext(0),mpkPrev(0),mpkParent(0),mkAbsRect(0,0,0,0)
{
	mkPos.x = 0;
	mkPos.y = 0;
	mkDim.x = 0;
	mkDim.y = 0;
	if(pkParent) setParent(pkParent);
}

// dtor
iXuiWnd::~iXuiWnd()
{
	// do nothing
}

// set parent
// the concept is to remove from old parent and add to new parent
// no update to mpkParent is made here, its done by the parent itself
void iXuiWnd::setParent(cXuiContainer* mpkP)
{
	if(mpkParent)	
		mpkParent->removeChild(this);
	if(mpkP)	
		mpkP->addChild(this);
	// assert the values were updated
	X_Assert(mpkP == mpkParent);
}

// update rects, should be called only when dirty
void iXuiWnd::update()
{
	// get screen size
	const size2_x& kSDim = X_SharedPtr(iRenderer)->getScreenSize();
	f32 dx = (f32)kSDim.dx;
	f32 dy = (f32)kSDim.dy;
	mkAbsRect.left = (i2d)( mkPos.x * dx );
	mkAbsRect.top = (i2d)( mkPos.y * dy);
	mkAbsRect.right = mkAbsRect.left + (i2d)(mkDim.x * dx);
	mkAbsRect.bottom = mkAbsRect.top + (i2d)(mkDim.y * dy);
}

// register
void iXuiWnd::preRendering()
{
	if(muFlags & iXuiWnd::Visible)
	{
		onDraw();
		if(muFlags & iXuiWnd::IsContainer)
			((cXuiContainer*)this)->onRenderChildren();
	}
}

// we go with the motto first window drawn last
void iXuiWnd::bringToFront()
{
	iXuiWnd* me = this;
	while(me && me->mpkParent)
	{
		if(me->mpkPrev)
		{
			me->mpkPrev->mpkNext = me->mpkNext;
			if(me->mpkNext) 
				me->mpkNext->mpkPrev = me->mpkPrev;
			else
				me->mpkParent->mpkLastChild = me->mpkPrev;

			me->mpkPrev = 0;
			me->mpkNext = me->mpkParent->mpkFirstChild;
			me->mpkParent->mpkFirstChild->mpkPrev = me;
			me->mpkParent->mpkFirstChild = me;
		}
		me = me->mpkParent;
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//					cXuiContainer
// ctor
cXuiContainer::cXuiContainer(cXuiContainer* pkP,u32 uStyle) :
iXuiWnd(pkP,iXuiWnd::xGuiDialog,iXuiWnd::Visible|iXuiWnd::CanHaveFocus|iXuiWnd::IsContainer),
mpkFirstChild(0),mpkLastChild(0),mpkBack(0),mpkCallback(0),
muStyle(uStyle)
{
	RefObjectInit(cXuiContainer);
	muFlags |= iXuiWnd::IsContainer;
}

// dtor
cXuiContainer::~cXuiContainer()
{
	while(mpkFirstChild)
		removeChild(mpkFirstChild);
	// delete all radio group
	for(u32 i=0;i<mkRadioGroups.size();i++)
	{
		cXuiRadioGroup* g = (cXuiRadioGroup*)mkRadioGroups[i];
		delete g;
	}

	//if(mpkBackground) { mpkBackground->mpkImage->release(); delete mpkBackground; }
}

// set back
void cXuiContainer::setBackground(iTexture* pkT,const float* pkSrc,s32 iCol)
{	
	if(mpkBack)
		mpkBack->mpkTexture->release();
	if(	mpkBack && !pkT )
	{
		freeMemory(mpkBack);
		muStyle &= ~BackgroundImage;
	}
	else if( pkT )
	{
		if( !mpkBack )
		{
			mpkBack = (Background*)allocMemory(sizeof(Background));
			muStyle |= BackgroundImage;
			// get the client area
			mpkBack->mkClient.left = mkAbsRect.left + CV_WinEdgeLeft;
			mpkBack->mkClient.top = mkAbsRect.top + CV_WinEdgeTop;
			mpkBack->mkClient.right = mkAbsRect.right - CV_WinEdgeRight;
			mpkBack->mkClient.bottom = mkAbsRect.bottom - CV_WinEdgeBottom;
		}
		mpkBack->mpkTexture = pkT;
		if(pkSrc)
		{
			mpkBack->mkSrc[0] = pkSrc[0];
			mpkBack->mkSrc[1] = pkSrc[1];
			mpkBack->mkSrc[2] = pkSrc[2];
			mpkBack->mkSrc[3] = pkSrc[3];
		}
		else
		{
			mpkBack->mkSrc[0] = 0;
			mpkBack->mkSrc[1] = 0;
			mpkBack->mkSrc[2] = 1;
			mpkBack->mkSrc[3] = 1;
		}
		mpkBack->miColor = iCol;
		pkT->addRef();
	}
}

// add child and set its parent
void cXuiContainer::addChild(iXuiWnd *pkChild)
{
	// check to see if its not already added
	X_Assert(!pkChild->mpkParent && !pkChild->mpkNext && !pkChild->mpkPrev);
	pkChild->mpkNext = mpkFirstChild;
	pkChild->mpkParent = this;
	if(mpkFirstChild) mpkFirstChild->mpkPrev = pkChild;
	mpkFirstChild = pkChild;
	if(!mpkLastChild) mpkLastChild = mpkFirstChild;
	pkChild->addRef();
}

// remove children and clear all its associations
void cXuiContainer::removeChild(iXuiWnd *pkChild)
{
	// check if this is a valid op
	X_Assert(pkChild->mpkParent == this);

	if(pkChild->mpkPrev) 
		pkChild->mpkPrev->mpkNext = pkChild->mpkNext;
	else // first child
		mpkFirstChild = pkChild->mpkNext;

	if(pkChild->mpkNext)
		pkChild->mpkNext->mpkPrev = pkChild->mpkPrev;
	else
		mpkLastChild = pkChild->mpkPrev;

	pkChild->mpkParent = 0;
	pkChild->mpkNext = 0;
	pkChild->mpkPrev = 0;
	pkChild->release();
}

// get wnd at
iXuiWnd* cXuiContainer::getWndAt(i2d x,i2d y)
{
	cXuiContainer* parent = this;

	if( isPointInside(x,y) )
	{
again:
		iXuiWnd* cwnd = parent->mpkFirstChild;
		while(cwnd)
		{
			if(cwnd->getVisible() && cwnd->isPointInside(x,y))
			{
				if(cwnd->muFlags & iXuiWnd::IsContainer)
				{
					parent = (cXuiContainer*)cwnd;
					goto again;
				}
				else
					return cwnd;
			}
			cwnd = cwnd->next();
		}

		return parent;
	}

	return 0;
}

// radio gp
cXuiRadioGroup* cXuiContainer::getRadioGroup(u32 uId)
{
	for(u32 i=0;i<mkRadioGroups.size();i++)
	{
		cXuiRadioGroup* g = (cXuiRadioGroup*)mkRadioGroups[i];
		if(g->getId()==uId)
			return g;
	}
	cXuiRadioGroup* g = new cXuiRadioGroup(uId);
	mkRadioGroups.push_back((array_t::alloc_type)g);
	return g;
}

// update
void cXuiContainer::update()
{
	iXuiWnd::update();
	// obtain the client area
	if( muStyle & BackgroundImage )
	{
		mpkBack->mkClient.left = mkAbsRect.left + CV_WinEdgeLeft;
		mpkBack->mkClient.top = mkAbsRect.top + CV_WinEdgeTop;
		mpkBack->mkClient.right = mkAbsRect.right - CV_WinEdgeRight;
		mpkBack->mkClient.bottom = mkAbsRect.bottom - CV_WinEdgeBottom;
	}
	// update x button
	if( muStyle & CloseBox )
	{
		mkCloseBox.right  = mkAbsRect.right - CV_WinXOffsetX;
		mkCloseBox.top    = mkAbsRect.top   + CV_WinXOffsetY;
		mkCloseBox.left	  = mkCloseBox.right- CV_XButtonWidth;
		mkCloseBox.bottom = mkCloseBox.top  + CV_XButtonHeight;
	}
	// recurse
	iXuiWnd* pkFirst = mpkFirstChild;
	while(pkFirst)
	{
		pkFirst->update();
		pkFirst = pkFirst->mpkNext;
	}
}

// register
void cXuiContainer::onRenderChildren()
{
	if(muFlags & iXuiWnd::Visible)
	{
		iXuiWnd* pkFirst = mpkLastChild;
		while(pkFirst)
		{
			pkFirst->preRendering();
			pkFirst = pkFirst->mpkPrev;
		}
	}
}

// ondraw
void cXuiContainer::onDraw()
{
	// get system skin
	iXuiSkin* pkSkin = X_SharedPtr(cXuiManager)->getSkin();
	X_Assert(pkSkin);
	bool bDrawClient = !(muStyle & BackgroundImage);
	if(!bDrawClient)
	{
		X_Assert(mpkBack);
		// draw the background using irenderer
		X_SharedPtr(iRenderer)->draw2DImage(mpkBack->mpkTexture,mpkBack->mkSrc,mpkBack->mkClient,mpkBack->miColor);
	}
	if(muStyle & BorderFrame)
	{
		pkSkin->drawWindow(mkAbsRect,muStyle & TitleBar,bDrawClient,mkCaption);
	}
	if(muStyle & CloseBox)
	{
		iXuiSkin::EControlState e = iXuiSkin::CS_Normal;
		if(muFlags & CloseButtonPressed)
			e = iXuiSkin::CS_Pressed;
		else if(muFlags & CloseButtonMouseOver)
			e = iXuiSkin::CS_MouseOver;
		pkSkin->drawControlState(mkCloseBox,iXuiSkin::ID_BWinCloseButton,e,0);
	}
}

// handle gui event
void cXuiContainer::onGuiEvent(iXuiWnd::GuiEvent e, iXuiWnd *pkCaller)
{
	switch(e)
	{
	case OnMouseEnter:
		muFlags |= MouseOver;
		break;
	case OnMouseLeave:
		muFlags &=~(CloseButtonPressed|CloseButtonMouseOver|MouseOver);
	default:
		if(mpkCallback)
			mpkCallback->onEvent(e,pkCaller);
	}
}

// handle mouse
void cXuiContainer::onMouseEvent(const cMouseEvent *e)
{
	if( muStyle & CloseBox )
	{
		switch(e->meType)
		{
		case cMouseEvent::MMove:
			if( mkCloseBox.isInside(e->x,e->y) )
				muFlags |= CloseButtonMouseOver;
			else
				muFlags &=~CloseButtonMouseOver;
			break;
		case cMouseEvent::LBDown:
			if(muFlags & CloseButtonMouseOver)
				muFlags |= CloseButtonPressed;
			break;
		case cMouseEvent::LBUp:
			if( mkCloseBox.isInside(e->x,e->y) )
				setVisible(false);
			break;
		}
	}
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//					cXuiButton
cXuiButton::cXuiButton(cXuiContainer *pkParent,u32 uId) :
iXuiWnd(pkParent,xGuiButton)
{
	RefObjectInit(cXuiButton);
	setKey(uId);
}
// event notification
void cXuiButton::onGuiEvent(iXuiWnd::GuiEvent e,iXuiWnd*)
{
	switch(e)
	{
	case OnMouseEnter:
		muFlags |= MouseOver;
		break;
	case OnMouseLeave:
		muFlags &=~MouseOver;
		muFlags &=~Pressed;
		break;
	}
}

void cXuiButton::onKeyEvent(const nextar::cKeyEvent *k)
{
	if(k->muKeyValue == muHotKey)
	{
		if(k->muFlags & cKeyEvent::KeyPressed)		
			muFlags |= Pressed;
		else
			muFlags &=~Pressed;
	}
}

// mouse 
void cXuiButton::onMouseEvent(const cMouseEvent* m)
{
	switch(m->meType)
	{
	case cMouseEvent::LBDown:
		muFlags |= Pressed;break;
	case cMouseEvent::LBUp:
		muFlags &=~Pressed;break;
		mpkParent->onGuiEvent(iXuiWnd::OnButtonClick,this);
		break;
	}
}

// on render
void cXuiButton::onDraw()
{
	iXuiSkin* pkSkin = X_SharedPtr(cXuiManager)->getSkin();
	X_Assert(pkSkin);
	iXuiSkin::EControlState	iState = iXuiSkin::CS_Normal;
	if(muFlags & Disabled)
		iState = iXuiSkin::CS_Disabled;
	else if(muFlags & Pressed)
		iState = iXuiSkin::CS_Pressed;
	else if(muFlags & MouseOver)
		iState = iXuiSkin::CS_MouseOver;
	else if(muFlags & HasFocus)
		iState = iXuiSkin::CS_HasFocus;

	pkSkin->drawControlState(mkAbsRect,iXuiSkin::ID_NormalButton,iState,mkCaption);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//					cXuiCheckbox
cXuiCheckbox::cXuiCheckbox(cXuiContainer *pkParent,u32 uId) :
cXuiButton(pkParent,uId)
{
	RefObjectInit(cXuiCheckbox);
	meType = xGuiCheckbox;
}

// events
void cXuiCheckbox::onKeyEvent(const nextar::cKeyEvent *k)
{
	if(k->muKeyValue == muHotKey)
	{
		if(k->muFlags & cKeyEvent::KeyPressed)		
			toggle();
	}
}

// mouse 
void cXuiCheckbox::onMouseEvent(const cMouseEvent* m)
{
	if(m->meType == cMouseEvent::LBUp)
		toggle();
}

// on render
void cXuiCheckbox::onDraw()
{
	iXuiSkin* pkSkin = X_SharedPtr(cXuiManager)->getSkin();
	X_Assert(pkSkin);
	iXuiSkin::EControlState	iState = iXuiSkin::CS_Normal;
	if(muFlags & Checked)
		iState = iXuiSkin::CS_Checked;
	else if(muFlags & Pressed)
		iState = iXuiSkin::CS_Pressed;
	else if(muFlags & MouseOver)
		iState = iXuiSkin::CS_MouseOver;

	pkSkin->drawControlState(mkAbsRect,iXuiSkin::ID_Checkbox,iState,mkCaption);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//					cXuiRadioButton
cXuiRadioButton::cXuiRadioButton(cXuiContainer *pkParent,u32 uId, u32 gid) :
cXuiCheckbox(pkParent,uId),mpkGroupPtr(0)
{
	RefObjectInit(cXuiRadioButton);
	meType = xGuiRadioButton;
	if( gid != InvalidRBGroup )
	{
		mpkGroupPtr = mpkParent->getRadioGroup(gid);
		mpkGroupPtr->add(this);
	}
}

void cXuiRadioButton::onKeyEvent(const nextar::cKeyEvent *k)
{
	if(k->muKeyValue == muHotKey)
	{
		if(k->muFlags & cKeyEvent::KeyPressed)		
		{
			if(mpkGroupPtr)
				mpkGroupPtr->set(this);
			else 
				muFlags|=Checked;
		}
	}
}

// mouse 
void cXuiRadioButton::onMouseEvent(const cMouseEvent* m)
{
	if(m->meType == cMouseEvent::LBUp)
	{
		check();
		if(mpkGroupPtr)
			mpkGroupPtr->set(this);
	}
}

// on render
void cXuiRadioButton::onDraw()
{
	iXuiSkin* pkSkin = X_SharedPtr(cXuiManager)->getSkin();
	X_Assert(pkSkin);
	iXuiSkin::EControlState	iState = iXuiSkin::CS_Normal;
	if(muFlags & Checked)
		iState = iXuiSkin::CS_Pressed;
	else if(muFlags & MouseOver)
		iState = iXuiSkin::CS_MouseOver;

	pkSkin->drawControlState(mkAbsRect,iXuiSkin::ID_RadioButton,iState,mkCaption);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//					cXuiRadioGroup
cXuiRadioGroup::cXuiRadioGroup(u32 iD) : mpkActive(0), muId(iD)
{
}

// add
void cXuiRadioGroup::add(cXuiRadioButton* b)
{
	mkRadioButtons.push_back((array_t::alloc_type)b);
	if(!mpkActive) {
		mpkActive = b;
		mpkActive->check();
	}
	else if(b->isChecked())
	{
		mpkActive->uncheck();
		mpkActive = b;
	}
}

// rem
void cXuiRadioGroup::rem(cXuiRadioButton* b)
{
	s32 idx = mkRadioButtons.binary_search((array_t::alloc_type)b);
	if(idx!=-1)
	{
		if(mpkActive==b)
		{
			if(mkRadioButtons.size()>1)
			{
				mpkActive = (cXuiRadioButton*)mkRadioButtons.get_last();
				mpkActive->check();
			}
			else
				mpkActive = 0;
		}
		mkRadioButtons.erase((u32)idx);
	}
}

// set
void cXuiRadioGroup::set(cXuiRadioButton* b)
{
	if(mpkActive!=b)
	{
		if(mpkActive) mpkActive->uncheck();
		mpkActive = b;
		if(mpkActive) mpkActive->check();
	}
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//					cXuiStaticText
cXuiStaticText::cXuiStaticText(cXuiContainer *pkParent,const tchar *sCap, bool bDrawBorder) :
iXuiWnd(pkParent,xGuiStaticText,Visible), mpkFont(0), muDrawFlags(iXuiFont::DrawXCentered|iXuiFont::DrawYCentered)
{
	RefObjectInit(cXuiStaticText);
	muFlags &= ~CanHaveFocus;
	setBordered(bDrawBorder);
	setCaption(sCap);
}

cXuiStaticText::~cXuiStaticText()
{
	if(mpkFont) mpkFont->release();
}

void cXuiStaticText::update()
{	
	if( !mpkFont )
	{
		mpkFont = X_SharedPtr(cXuiManager)->getSkin()->getStaticFont();
		miColor = CV_StaticFontColor;
		X_Assert(mpkFont);
		mpkFont->addRef();
	}

	iXuiWnd::update();
	if( mkAbsRect.height() <= mpkFont->getCharacterHeight() )
		mkAbsRect.bottom = mkAbsRect.top + mpkFont->getCharacterHeight();
}

void cXuiStaticText::setFont(u32 ukey,u32 ucol)
{
	iXuiFont* pkF = X_SharedPtr(cXuiManager)->getFont(ukey);
	if( pkF ) {
		if(mpkFont) mpkFont->release();
		mpkFont = pkF;
		mpkFont->addRef();
	}
	miColor = ucol;
	update();
}

void cXuiStaticText::onDraw()
{
	mpkFont->drawSingleLineText(mkAbsRect,muDrawFlags,miColor,mkCaption);
	if(muFlags & Bordered)
	{
		//TODO: we dont do this currently
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//					cXuiVScrollBar
cXuiVScrollBar::cXuiVScrollBar(cXuiContainer *pkParent) : 
iXuiWnd(pkParent,xGuiVScrollBar),miPageSize(1),miStart(0),miEnd(1),miPos(0),muArrowTS(0)
{
	RefObjectInit(cXuiVScrollBar);
	muFlags |= (ShowThumb);
}

// update rects
void cXuiVScrollBar::update()
{
	const size2_x& kSDim = X_SharedPtr(iRenderer)->getScreenSize();
	f32 dx = (f32)kSDim.dx;
	f32 dy = (f32)kSDim.dy;

	mkThumb.left = 
	mkTrack.left = 
	mkDown.left = 
	mkUp.left = 
	mkAbsRect.left = 
	(i2d)(mkPos.x * dx);

	mkUp.top = 
	mkAbsRect.top = 
	(i2d)(mkPos.y * dy);

	mkDown.bottom = 
	mkAbsRect.bottom = 
	mkAbsRect.top + (i2d)(mkDim.y * dy);

	mkThumb.right = 
	mkTrack.right = 
	mkDown.right = 
	mkUp.right = 
	mkAbsRect.right = mkAbsRect.left + CV_ScrollButtonWidth;

	mkTrack.top = 
	mkUp.bottom = 
	mkAbsRect.top + CV_ScrollButtonHeight;

	mkDown.top = 
	mkTrack.bottom = 
	mkAbsRect.bottom - CV_ScrollButtonHeight;
	_updateThumbRect();
}

// update thumb rect
void cXuiVScrollBar::_updateThumbRect()
{
	const i2d MinThumbSize = 8;
	s32 iDiff = miEnd - miStart;
	if( iDiff > miPageSize )
	{
		s32 iH = (s32)mkTrack.height();
		s32 iThumbHeight = __max( iH * miPageSize / iDiff, MinThumbSize );
        s32 iMaxPosition = iDiff - miPageSize;
       
		mkThumb.top = i2d(
		mkTrack.top + (miPos - miStart) * ( iH - iThumbHeight ) / iMaxPosition);
        mkThumb.bottom = mkThumb.top + (i2d)iThumbHeight;
       	muFlags |= ShowThumb;
	}
	else
	{
		muFlags &= ~ShowThumb;
	}
}

// scroll
void cXuiVScrollBar::scroll(s32 delta)
{
	miPos += delta;
	_cap();
	_updateThumbRect();
	// we can post a scroll event to the parent here
	if(mpkParent)
		mpkParent->onGuiEvent(OnScroll,this);
}

// show item
void cXuiVScrollBar::showItem(s32 iIdx)
{
   if( iIdx < 0 )
        iIdx = 0;

    if( iIdx >= miEnd )
        iIdx = miEnd - 1;

    // Adjust position
    if( miPos > iIdx )
        miPos = iIdx;
    else if( miPos + miPageSize <= iIdx )
        miPos = iIdx - miPageSize + 1;

    _updateThumbRect();
}

// onguievent
void cXuiVScrollBar::onGuiEvent(iXuiWnd::GuiEvent e,iXuiWnd *pkCaller)
{
	if(e==iXuiWnd::OnMouseLeave)
		muFlags &=~(ClickedUp|ClickedDown|ThumbPressed|HeldUp|HeldDown);
}
// read mouse
void cXuiVScrollBar::onMouseEvent(const cMouseEvent* e)
{
	static i2d iThumbOffset;
	if(miPageSize <= 0) return;
	switch(e->meType)
	{
	case cMouseEvent::LBDown:
		if( mkUp.isInside(e->x,e->y) )
		{
			if(miPos>miStart)
				--miPos;
			_updateThumbRect();
			muFlags |= (ClickedUp);
			muArrowTS = timer_t::skAsynchTimer.getFrameTime();
		}
		else if( mkDown.isInside(e->x,e->y) )
		{
			if(miPos+miPageSize<miEnd)
				++miPos;
			_updateThumbRect();
			muFlags |= (ClickedDown);
			muArrowTS = timer_t::skAsynchTimer.getFrameTime();
		}
		else if( mkThumb.isInside(e->x,e->y) )
		{
			_updateThumbRect();
			muFlags |= ThumbPressed;
			iThumbOffset = e->y - mkThumb.top;
		}
		else
		{
			// track clicked
			if( mkThumb.top > e->y && e->y >= mkTrack.top )
				scroll( -( miPageSize - 1 ) );
			else if( mkThumb.bottom <= e->y && e->y < mkTrack.bottom )
				scroll( ( miPageSize - 1 ) );
		}
		break;
	case cMouseEvent::LBUp:
		muFlags &=~(ClickedUp|ClickedDown|ThumbPressed|HeldUp|HeldDown);
		_updateThumbRect();
		break;
	case cMouseEvent::MDrag:
		if(muFlags & ThumbPressed)
		{
			register i2d iDiff = e->y - iThumbOffset;
			mkThumb.bottom += iDiff - mkThumb.top;
			mkThumb.top = iDiff;

			if( mkThumb.top < mkTrack.top )
			{
				iDiff = mkTrack.top - mkThumb.top;
				mkThumb.top += iDiff;
				mkThumb.bottom += iDiff;
			}
			else if( mkThumb.bottom > mkTrack.bottom )
			{
				iDiff = mkTrack.bottom - mkThumb.bottom;
				mkThumb.top += iDiff;
				mkThumb.bottom += iDiff;
			}

            // Compute first item index based on thumb position			
			s32 iMaxFirstItem = miEnd - miStart - miPageSize;  // Largest possible index for first item
			s32 iMaxThumb =	mkTrack.height() - mkThumb.height();  // Largest possible thumb position from the top
			miPos = miStart + ( mkThumb.top - mkTrack.top + iMaxThumb / ( iMaxFirstItem << 1 ) ) * iMaxFirstItem  / iMaxThumb;
		}
	}
}

// render
void cXuiVScrollBar::onDraw()
{
	// now render
	iXuiSkin::EControlState up,down;
	up = down = iXuiSkin::CS_Normal;

	if(muFlags & (HeldUp|ClickedUp|HeldDown|ClickedDown))
	{
		u32 uDT = timer_t::skAsynchTimer.getFrameTime() - muArrowTS;
		if(muFlags & HeldUp)
		{
			up = iXuiSkin::CS_Pressed;
			if(N3DScrollbarArrowClickRepeat < uDT)
			{
				scroll(-1);
				muArrowTS = timer_t::skAsynchTimer.getFrameTime();
			}
		}
		else if(muFlags & ClickedUp)
		{
			up = iXuiSkin::CS_Pressed;
			if( N3DScrollbarArrowClickDelay < uDT )
			{
				scroll(-1);
				muFlags |= HeldUp;
				muArrowTS = timer_t::skAsynchTimer.getFrameTime();
			}
		}
		else if(muFlags & HeldDown)
		{
			down = iXuiSkin::CS_Pressed;
			if(N3DScrollbarArrowClickRepeat < uDT)
			{
				scroll(1);
				muArrowTS = timer_t::skAsynchTimer.getFrameTime();
			}
		}
		else if(muFlags & ClickedDown)
		{
			down = iXuiSkin::CS_Pressed;
			if( N3DScrollbarArrowClickDelay < uDT )
			{
				scroll(1);
				muFlags |= HeldDown;
				muArrowTS = timer_t::skAsynchTimer.getFrameTime();
			}
		}
	}

	iXuiSkin* pkSk = X_SharedPtr(cXuiManager)->getSkin();

	pkSk->drawControl(mkTrack,iXuiSkin::ID_ScrollTrack);
	pkSk->drawControlState(mkUp,iXuiSkin::ID_ScrollButtonUp,up, 0 );
	pkSk->drawControlState(mkThumb,iXuiSkin::ID_ScrollThumb,muFlags & ThumbPressed ? iXuiSkin::CS_Pressed : iXuiSkin::CS_Normal, 0 );
	pkSk->drawControlState(mkDown,iXuiSkin::ID_ScrollButtonDown,down, 0 );
}

// set track range
void cXuiVScrollBar::setTrackRange(s32 iStart, s32 iEnd)
{
	miStart = iStart;
	miEnd   = iEnd;
	_cap();
	_updateThumbRect();
}

void cXuiVScrollBar::_cap()
{
	if( miPos < miStart || miEnd - miStart <= miPageSize )
        miPos = miStart;
    else if( miPos + miPageSize > miEnd )
        miPos = miEnd - miPageSize;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//					cXuiListBox
// -- Item
cXuiListBox::Item::Item() : mpvData(0), mbSelected(false)
{
}

cXuiListBox::cXuiListBox(cXuiContainer* pkParent,u32 uId):
iXuiWnd(pkParent,xGuiListBox) , miSelected(-1), miSelStart(0) ,mkScroll(0)
{
	RefObjectInit(cXuiListBox);
	mkScroll.update();
	setKey(uId);
}

//
cXuiListBox::~cXuiListBox()
{
	remAllItems();
}

//
void cXuiListBox::update()
{
	const size2_x& kSDim = X_SharedPtr(iRenderer)->getScreenSize();
	f32 dx = (f32)kSDim.dx;
	f32 dy = (f32)kSDim.dy;
	mkAbsRect.left = (i2d)( mkPos.x * dx );
	mkAbsRect.top = (i2d)( mkPos.y * dy);
	mkAbsRect.right = mkAbsRect.left + (i2d)(mkDim.x * dx);
	mkAbsRect.bottom = mkAbsRect.top + (i2d)(mkDim.y * dy);

	rect2_x	tmp = mkAbsRect;
	tmp.left +=  CV_LBEdgeLeft;
	tmp.top  +=  CV_LBEdgeTop;
	tmp.right -= (CV_LBEdgeRight+CV_ScrollButtonWidth);
	tmp.bottom -= CV_LBEdgeBottom;

	// update the scroll
	mkScroll.setPos( f32(tmp.right) / dx, f32(tmp.top) / dy );
	mkScroll.setDimY( f32(tmp.height()) / dy );
	mkScroll.update();

	// update the area
	mkSelection.left = tmp.left+CV_LBBorder;
	mkSelection.top = tmp.top+CV_LBMargin;
	mkSelection.right = tmp.right-CV_LBBorder;
	mkSelection.bottom = tmp.bottom-CV_LBMargin;
	mkText.left = mkSelection.left+CV_LBBorder;
	mkText.top = mkSelection.top+CV_LBMargin;
	mkText.right = mkSelection.right-CV_LBBorder;
	mkText.bottom = mkSelection.bottom-CV_LBMargin;
	mkScroll.setPageSize(mkText.height()/CV_StaticTextHeight);
	mkScroll.showItem(miSelected);
}

// add item
void cXuiListBox::addItem(const tchar *sText, void *pvData)
{
	Item* it = new Item();
	it->mkString = sText;
	it->mpvData = pvData;
	mkItems.push_back((array_t::alloc_type)it);
	mkScroll.setTrackRange(0,(i2d)mkItems.size());
	if(mpkParent) 
		mpkParent->onGuiEvent(OnAddItemLbox,this);
}

// add 
void cXuiListBox::insertItem(s32 idx, const tchar *sText, void *pvData)
{
	Item* it = new Item();
	it->mkString = sText;
	it->mpvData = pvData;
	mkItems.insert((array_t::alloc_type)it,(array_t::index_type)idx);
	mkScroll.setTrackRange(0,(i2d)mkItems.size());
	if(mpkParent) 
		mpkParent->onGuiEvent(OnAddItemLbox,this);
}

// find item
cXuiListBox::Item* cXuiListBox::findItem(const tchar *sText,bool add)
{
	for( array_t::index_type idx = 0; idx < mkItems.size(); idx++ )
	{
		Item* it = (Item*)mkItems.at(idx);
		if(it->mkString==sText)
			return it;
	}
	if(add)
	{
		addItem(sText);
		return (Item*)mkItems.get_last();
	}
	return 0;
}

// remove item
void cXuiListBox::remItem(s32 idx)
{
	Item* it = (Item*)mkItems.at((array_t::index_type)idx);
	delete it;
	mkItems.erase((array_t::index_type)idx);
	mkScroll.setTrackRange(0,(i2d)mkItems.size());
	if( miSelected >= (i2d)mkItems.size() )
		miSelected = (i2d)mkItems.size()-1;
	if(mpkParent) 
		mpkParent->onGuiEvent(OnRemItemLbox,this);
}

// remove item
void cXuiListBox::remItemByText(const tchar *sText)
{
	for( array_t::index_type idx = 0; idx < mkItems.size(); idx++ )
	{
		Item* it = (Item*)mkItems.at(idx);
		if(it->mkString==sText)
		{
			delete it;
			mkItems.erase((array_t::index_type)idx);
			mkScroll.setTrackRange(0,(i2d)mkItems.size());
			if( miSelected >= (i2d)mkItems.size() )
				miSelected = (i2d)mkItems.size()-1;
			if(mpkParent) 
				mpkParent->onGuiEvent(OnRemItemLbox,this);
		}
	}
}

// remove item
void cXuiListBox::remItemByData(void *pvData)
{
	for( array_t::index_type idx = 0; idx < mkItems.size(); idx++ )
	{
		Item* it = (Item*)mkItems.at(idx);
		if(it->mpvData==pvData)
		{
			delete it;
			mkItems.erase((array_t::index_type)idx);
			mkScroll.setTrackRange(0,(i2d)mkItems.size());
			if( miSelected >= (i2d)mkItems.size() )
				miSelected = (i2d)mkItems.size()-1;
			if(mpkParent) 
				mpkParent->onGuiEvent(OnRemItemLbox,this);
		}
	}
}

// rem all items
void cXuiListBox::remAllItems()
{
	for( array_t::index_type idx = 0; idx < mkItems.size(); idx++ )
	{
		Item* it = (Item*)mkItems.at(idx);
		delete it;
	}
	mkItems.clear();
	mkScroll.setTrackRange(0,1);
}

//
s32 cXuiListBox::getSelectionIdx(s32 iPrev)
{
	if(iPrev<-1) return -1;
	if(muFlags & MultiSelection)
	{
		for(array_t::index_type i=(array_t::index_type)iPrev+1;i<mkItems.size();i++)
		{
			Item* it = (Item*)mkItems.at(i);
			if(it->mbSelected)
				return (s32)i;
		}
	}
	return miSelected;
}

void cXuiListBox::selItem(s32 idx)
{
	if(!mkItems.size()) return;
	s32 iOld = miSelected;
	miSelected = idx;
	if( miSelected < 0 )
        miSelected = 0;
    if( miSelected >= (s32)mkItems.size() )
        miSelected = mkItems.size() - 1;
	if(iOld != miSelected)
	{
		if(muFlags & MultiSelection)
		{
			Item* it = (Item*)mkItems.at(miSelected);
			it->mbSelected = true;
		}
		miSelStart = miSelected;
		mkScroll.showItem(miSelected);
	}
	if(mpkParent)
		mpkParent->onGuiEvent(OnLBSelection,this);
}

void cXuiListBox::onKeyEvent(const nextar::cKeyEvent *k)
{
	if(!mkItems.size()) return;
	if(k->muFlags & cKeyEvent::KeyPressed)
	{
		s32 iOld = miSelected;
		switch(k->muKeyValue)
		{
		case KEY_UP:	--miSelected; break;
		case KEY_DOWN:	++miSelected; break;
		case KEY_NEXT:	miSelected+=mkScroll.getPageSize()-1;break;
		case KEY_PRIOR:	miSelected-=mkScroll.getPageSize()-1;break;
		case KEY_HOME:	miSelected=0;break;
		case KEY_END:	miSelected=(s32)(mkItems.size()-1);break;
		default: return;
		}
		// Perform capping
		if( miSelected < 0 )
			miSelected = 0;
		if( miSelected >= (s32)mkItems.size() )
			miSelected = (s32)mkItems.size() - 1;

		if( iOld != miSelected )
		{
			if( muFlags & MultiSelection )
			{
				// Multiple selection
				// Clear all selection
				for( s32 i = 0; i < (s32)mkItems.size(); ++i )
				{
					Item *pItem = (Item*)mkItems[i];
					pItem->mbSelected = false;
				}

				if( k->muFlags & cKeyEvent::Shift )
				{
					// Select all items from miSelStart to
					// miSelected
					s32 nEnd = __max( miSelStart, miSelected );

					for( s32 n = __min( miSelStart, miSelected ); n <= nEnd; ++n )
					{
						Item *pItem = (Item*)mkItems[n];
						pItem->mbSelected = true;
					}
				}
				else
				{
					Item *pItem = (Item*)mkItems[miSelected];
					pItem->mbSelected = true;

					// Update selection start
					miSelStart = miSelected;
				}
			}
			else
				miSelStart = miSelected;
			
			// Adjust scroll bar
			mkScroll.showItem( (i2d)miSelected );
			if(mpkParent)
				mpkParent->onGuiEvent(OnLBSelection,this);
		}
	}
}

// mouse event
void cXuiListBox::onMouseEvent(const cMouseEvent *e)
{
	if(mkScroll.isPointInside(e->x,e->y))
	{
		mkScroll.onMouseEvent(e);
		muFlags |= MouseOverScroll;
	}
	else if(muFlags & MouseOverScroll)
	{
		mkScroll.onGuiEvent(OnMouseLeave,0);
		muFlags &=~MouseOverScroll;
	}
	// do the massage handling
	switch( e->meType )
	{
	case cMouseEvent::LBDown:
		if( mkItems.size() > 0 && mkSelection.isInside(e->x,e->y) )
		{
			s32 iClicked = mkScroll.getTrackPos() + (e->y-mkText.top) / CV_StaticTextHeight;
			if(
				iClicked >= mkScroll.getTrackPos() &&
				iClicked < (s32)mkItems.size() && 
				iClicked < mkScroll.getTrackPos() + mkScroll.getPageSize() 
			  )
			{
				miSelected = iClicked;
				muFlags |= DragLB;
				if( !(e->muCtrlKeys & cMouseEvent::Shift) )
					miSelStart = iClicked;
				if( muFlags & MultiSelection )
				{
					Item* pkItm = getItem(iClicked);
					u32 tmp = e->muCtrlKeys & (cMouseEvent::Shift|cMouseEvent::Ctrl);
					if(tmp == cMouseEvent::Ctrl)
						pkItm->mbSelected = !pkItm->mbSelected;
					else if(tmp == cMouseEvent::Shift)
					{
						s32 iBegin,iEnd;
						if(miSelected<miSelStart)
						{
							iBegin = miSelected;
							iEnd = miSelStart;
						}
						else
						{
							iBegin = miSelStart;
							iEnd = miSelected;
						}
						for(s32 i=0; i<iBegin; ++i)
							getItem(i)->mbSelected = false;
						for(s32 i=iEnd+1;i<(s32)mkItems.size();++i)
							getItem(i)->mbSelected = false;
						for(s32 i=iBegin;i<=iEnd; ++i)
							getItem(i)->mbSelected = true;
					}
					else if((tmp == (cMouseEvent::Shift|cMouseEvent::Ctrl)))
					{
						s32 iBegin = __min(miSelected,miSelStart);
						s32 iEnd = __max(miSelected,miSelStart);
                        bool bLastSelected = getItem( miSelStart )->mbSelected;
						for( s32 i = iBegin + 1; i < iEnd; ++i )
							getItem(i)->mbSelected = bLastSelected;
                       
						pkItm->mbSelected = true;
                        miSelected = miSelStart;
					}
					else
					{
						for( s32 i = 0; i < (s32)mkItems.size(); ++i )
							getItem(i)->mbSelected = false;
						pkItm->mbSelected = true;
					}
					if(mpkParent)
						mpkParent->onGuiEvent(OnLBSelection,this);
				}
			}
		}
		break;
	case cMouseEvent::LBUp:
		muFlags &=~DragLB;
		if(miSelected != -1)
		{
			s32 iEnd = __max(miSelected,miSelStart);
			bool state = getItem(miSelStart)->mbSelected;
			for(s32 i = __min(miSelected,miSelStart); i<iEnd; ++i)
				getItem(i)->mbSelected = state;
			if(miSelStart != miSelected)
				mpkParent->onGuiEvent(OnLBSelection,this);
		}
		break;
	case cMouseEvent::MDrag:
		if(muFlags & DragLB)
		{		
			s32 iClicked = mkScroll.getTrackPos() + (e->y-mkText.top) / CV_StaticTextHeight;
			s32 iVal;
			if(iClicked < mkScroll.getTrackPos())
			{
				mkScroll.scroll(-1);
				miSelected = mkScroll.getTrackPos();
			}
			else if(iClicked >= (iVal=mkScroll.getTrackPos() + mkScroll.getPageSize()))
			{
				mkScroll.scroll(1);
				miSelected = __min( (s32)mkItems.size(), iVal ) - 1;
			}
			else if(iClicked < (s32)mkItems.size())
			{
				miSelected = iClicked;
			}
			else
				break;
			mpkParent->onGuiEvent(OnLBSelection,this);
		}
		break;
	case cMouseEvent::MScroll:
		mkScroll.scroll(e->m>>1);
		break;
	}
}

// gui event
void cXuiListBox::onGuiEvent(iXuiWnd::GuiEvent e, iXuiWnd *)
{
	switch(e)
	{
	case OnMouseLeave:
		muFlags &=~DragLB;
		break;
	}
}

// render
void cXuiListBox::onDraw()
{
	iXuiSkin* pkSk = X_SharedPtr(cXuiManager)->getSkin();
	pkSk->drawListBox(mkAbsRect,mkCaption);
	if( mkItems.size() )
	{
		rect2_x	txt(mkText.left,mkText.top,mkText.right,CV_StaticTextHeight + mkText.top);
		rect2_x	sel(mkSelection);
		for(s32 i = mkScroll.getTrackPos(); i < (s32)mkItems.size(); ++i)
		{
			if(txt.bottom>mkText.bottom) break;
			Item* itm = getItem(i);
			bool  bSel = false;
			if(muFlags & MultiSelection)
			{
				if(muFlags & DragLB && 
					((i>=miSelected && i<miSelStart) 
					||
					(i<=miSelected && i>miSelStart)))
				{
					bSel = getItem(miSelStart)->mbSelected;
				}
				else
					bSel = itm->mbSelected;
			}
			else if(i==miSelected)
				bSel = true;
			if(bSel)
			{
				sel.left = mkSelection.left; sel.top = txt.top; sel.bottom = txt.bottom;
				pkSk->drawControl(sel,iXuiSkin::ID_SelectedLBItem);
				sel.left+=3;
				pkSk->drawStaticText(itm->mkString,sel,iXuiFont::DrawYCentered);
			}
			else
				pkSk->drawStaticText(itm->mkString,txt,iXuiFont::DrawYCentered);
			txt.top = txt.bottom;
			txt.bottom += CV_StaticTextHeight;
		}
	}
	mkScroll.onDraw();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//					cXuiComboBox
// -- Item
cXuiComboBox::Item::Item() : mpvData(0)
{
}

cXuiComboBox::cXuiComboBox(cXuiContainer* pkParent,u32 uId) :
iXuiWnd(pkParent,xGuiComboBox) , mkScroll(0), 
miFocussed(-1), miSelected(-1), miDropHeight(32)
{
	RefObjectInit(cXuiComboBox);
	mkScroll.update();
	setKey(uId);
}

// dtor
cXuiComboBox::~cXuiComboBox()
{
	remAllItems();
}

// update
void cXuiComboBox::update()
{	
	const size2_x& kSDim = X_SharedPtr(iRenderer)->getScreenSize();

	f32 dx = (f32)kSDim.dx;
	f32 dy = (f32)kSDim.dy;
	mkAbsRect.left = (i2d)( mkPos.x * dx );
	mkAbsRect.top = (i2d)( mkPos.y * dy);
	mkAbsRect.right = mkAbsRect.left + (i2d)(mkDim.x * dx);
	mkAbsRect.bottom = mkAbsRect.top + (i2d)(mkDim.y * dy);

	// fix the button height;
	mkAbsRect.bottom = mkAbsRect.top + CV_DropDownButtonHeight;
	mkButton.left    = mkAbsRect.right - CV_DropDownButtonWidth;
	mkButton.right   = mkAbsRect.right;
	mkButton.top	 = mkAbsRect.top;
	mkButton.bottom  = mkAbsRect.bottom;

	mkDrop.left = mkText.left = mkAbsRect.left;
	mkText.top = mkAbsRect.top;
	mkDrop.right = mkText.right = mkButton.left;
	mkDrop.top = mkText.bottom = mkAbsRect.bottom;
	mkDrop.bottom = mkDrop.top + CV_DropDownButtonHeight + (i2d)miDropHeight;

	// adjust the text area and set up the
	// scroll bar
	mkDropText.left   = mkDrop.left+CV_CBDropDownBorer;
	mkDropText.top    = mkDrop.top+4;
	mkDropText.right  = mkDrop.right-(CV_CBDropDownBorer+CV_ScrollButtonWidth);
	mkDropText.bottom = mkDrop.bottom-4;

	mkScroll.setPos( (f32)mkDropText.right / dx, (f32)mkDropText.top / dy );
	mkScroll.setDimY( (f32)mkDropText.height() / dy );
	mkScroll.update();

	mkScroll.setPageSize(mkDropText.height()/CV_StaticTextHeight);
	mkScroll.showItem(miSelected);
}

// handle gui event
void cXuiComboBox::onGuiEvent(iXuiWnd::GuiEvent e, nextar::iXuiWnd *)
{
	switch(e)
	{
	case OnFocusOut:
		muFlags &= ~(CBOpened|TextAreaPressed|MouseOver);
		// return to old rect
		mkAbsRect.bottom = mkButton.bottom;
		if(miSelected!=miFocussed)
		{
			miSelected = miFocussed;
			mpkParent->onGuiEvent(OnCBSelection,this);
		}
			
		break;
	case OnMouseLeave:
		// this is wrong but has to be done cause of the
		// way messaging is done.
		muFlags &= ~(TextAreaPressed|MouseOverScroll|MouseOver);
		mkAbsRect.bottom = mkButton.bottom;
		break;
	}
}

// selection
void cXuiComboBox::onKeyEvent(const nextar::cKeyEvent *e)
{
	if(e->muFlags & cKeyEvent::KeyPressed)
	{
		switch(e->muKeyValue)
		{
		case KEY_RETURN:
			if(muFlags & CBOpened)
			{
				if(miSelected != miFocussed)
				{
					miSelected = miFocussed;
					mpkParent->onGuiEvent(OnCBSelection,this);
				}
				X_SharedPtr(cXuiManager)->setFocusWnd(mpkParent);
			}
			break;
		case KEY_F4:
			if(muFlags & CBOpened)
			{
				muFlags &= ~CBOpened;
				mkAbsRect.bottom = mkButton.bottom;
			}
			else
			{
				muFlags |= CBOpened;
				mkAbsRect.bottom = mkDrop.bottom;
				miFocussed = miSelected;
			}
			break;
		case KEY_LEFT:
		case KEY_UP:
			if(miFocussed > 0)
			{
				miFocussed--;
				miSelected = miFocussed;
				mkScroll.showItem(miSelected);
				if(!(muFlags & CBOpened))
					mpkParent->onGuiEvent(OnCBSelection,this);
			}
			break;
		case KEY_RIGHT:
		case KEY_DOWN:
			if(miFocussed+1 < (s32)mkItems.size())
			{
				miFocussed++;
				miSelected = miFocussed;
				mkScroll.showItem(miSelected);
				if(!(muFlags & CBOpened))
					mpkParent->onGuiEvent(OnCBSelection,this);
			}
			break;
		}
	}
}

// mouse
void cXuiComboBox::onMouseEvent(const nextar::cMouseEvent *e)
{
	if(mkScroll.isPointInside(e->x,e->y))
	{
		mkScroll.onMouseEvent(e);
		muFlags |= MouseOverScroll;
		return;
	}
	else if(muFlags & MouseOverScroll)
	{
		mkScroll.onGuiEvent(OnMouseLeave,0);
		muFlags &=~MouseOverScroll;
	}
	switch(e->meType)
	{
	case cMouseEvent::MMove:
		if((muFlags & CBOpened) && mkDrop.isInside(e->x,e->y))
		{
			s32 iClicked = mkScroll.getTrackPos() + (e->y-mkDropText.top) / CV_StaticTextHeight;
			if(
				iClicked >= mkScroll.getTrackPos() &&
				iClicked < (s32)mkItems.size() && 
				iClicked < mkScroll.getTrackPos() + mkScroll.getPageSize() 
			  )
				miFocussed = iClicked;
		}
		else
			muFlags |= MouseOver;
		break;
	case cMouseEvent::LBDown:
		if(muFlags & CBOpened)
		{
			if(mkDrop.isInside(e->x,e->y))
			{
				if(miSelected != miFocussed)
				{
					miSelected = miFocussed;
					mpkParent->onGuiEvent(OnCBSelection,this);
					muFlags &= ~CBOpened;
					// return to old rect
					mkAbsRect.bottom = mkButton.bottom;
				}
			}
			else
			{
				muFlags |= TextAreaPressed;
				muFlags &= ~CBOpened;
				// return to old rect
				mkAbsRect.bottom = mkButton.bottom;
			}
		}
		else
		{
			muFlags |= TextAreaPressed;
			muFlags |= CBOpened;
			// return to old rect
			mkAbsRect.bottom = mkDrop.bottom;
		}
		break;
	case cMouseEvent::LBUp:
		muFlags &=~TextAreaPressed;
		break;
	// TODO: Add Scroll Function
	}
}

// render
void cXuiComboBox::onDraw()
{
	iXuiSkin* pkSk = X_SharedPtr(cXuiManager)->getSkin();
	
	iXuiSkin::EControlState e = iXuiSkin::CS_Normal;
	
	if(muFlags & TextAreaPressed)
		e = iXuiSkin::CS_Pressed;
	else if(muFlags & MouseOver)
		e = iXuiSkin::CS_MouseOver;

	{
		const tchar* caption = 0;

		if(miSelected>=0 && miSelected<(s32)mkItems.size())
			caption = ((Item*)mkItems.at(miSelected))->mkString;

		pkSk->drawControlState(mkText,iXuiSkin::ID_ComboBox,e,caption);
	}
	pkSk->drawControlState(mkButton,iXuiSkin::ID_ComboDropDownButton,e,0);

	if(muFlags & CBOpened)
	{
		pkSk->drawComboDropDownArea(mkDrop);
		if( mkItems.size() )
		{
			rect2_x	txt(mkDropText.left+3,mkDropText.top,mkDropText.right,CV_StaticTextHeight + mkDropText.top);
			for(s32 i = mkScroll.getTrackPos(); i < (s32)mkItems.size(); ++i)
			{
				if(txt.bottom>mkDropText.bottom) break;
				Item* itm = getItem(i);
				if(i==miFocussed)
				{
					txt.left=mkDropText.left;
					pkSk->drawControl(txt,iXuiSkin::ID_SelectedCBItem);
					txt.left+=3;
				}
		
				pkSk->drawStaticText(itm->mkString,txt,iXuiFont::DrawYCentered);
				txt.top = txt.bottom;
				txt.bottom += CV_StaticTextHeight;
			}
		}
		mkScroll.onDraw();
	}
}

// add item
void cXuiComboBox::addItem(const tchar *sText, void *pvData)
{
	Item* it = new Item();
	it->mkString = sText;
	it->mpvData = pvData;
	mkItems.push_back((array_t::alloc_type)it);
	mkScroll.setTrackRange(0,(i2d)mkItems.size());
	if(mpkParent) 
		mpkParent->onGuiEvent(OnAddItemCbox,this);
}

// add 
void cXuiComboBox::insertItem(s32 idx, const tchar *sText, void *pvData)
{
	Item* it = new Item();
	it->mkString = sText;
	it->mpvData = pvData;
	mkItems.insert((array_t::alloc_type)it,(array_t::index_type)idx);
	mkScroll.setTrackRange(0,(i2d)mkItems.size());
	if(mpkParent) 
		mpkParent->onGuiEvent(OnAddItemCbox,this);
}

// remove item
void cXuiComboBox::remItem(s32 idx)
{
	Item* it = (Item*)mkItems.at((array_t::index_type)idx);
	delete it;
	mkItems.erase((array_t::index_type)idx);
	mkScroll.setTrackRange(0,(i2d)mkItems.size());
	if( miSelected >= (i2d)mkItems.size() )
		miSelected = (i2d)mkItems.size()-1;
	if(mpkParent) 
		mpkParent->onGuiEvent(OnRemItemCbox,this);
}

// remove item
void cXuiComboBox::remItemByText(const tchar *sText)
{
	for( array_t::index_type idx = 0; idx < mkItems.size(); idx++ )
	{
		Item* it = (Item*)mkItems.at(idx);
		if(it->mkString==sText)
		{
			delete it;
			mkItems.erase((array_t::index_type)idx);
			mkScroll.setTrackRange(0,(i2d)mkItems.size());
			if( miSelected >= (i2d)mkItems.size() )
				miSelected = (i2d)mkItems.size()-1;
			if(mpkParent) 
				mpkParent->onGuiEvent(OnRemItemCbox,this);
		}
	}
}

// remove item
void cXuiComboBox::remItemByData(void *pvData)
{
	for( array_t::index_type idx = 0; idx < mkItems.size(); idx++ )
	{
		Item* it = (Item*)mkItems.at(idx);
		if(it->mpvData==pvData)
		{
			delete it;
			mkItems.erase((array_t::index_type)idx);
			mkScroll.setTrackRange(0,(i2d)mkItems.size());
			if( miSelected >= (i2d)mkItems.size() )
				miSelected = (i2d)mkItems.size()-1;
			if(mpkParent) 
				mpkParent->onGuiEvent(OnRemItemCbox,this);
		}
	}
}

// rem all items
void cXuiComboBox::remAllItems()
{
	for( array_t::index_type idx = 0; idx < mkItems.size(); idx++ )
	{
		Item* it = (Item*)mkItems.at(idx);
		delete it;
	}
	mkItems.clear();
	mkScroll.setTrackRange(0,1);
}

cXuiComboBox::Item* cXuiComboBox::findItem(const tchar *sText,bool add)
{
	for( array_t::index_type idx = 0; idx < mkItems.size(); idx++ )
	{
		Item* it = (Item*)mkItems.at(idx);
		if(it->mkString==sText)
			return it;
	}
	if(add)
	{
		addItem(sText);
		return (Item*)mkItems.get_last();
	}
	return 0;
}

void cXuiComboBox::selItem(s32 idx)
{
	if(!mkItems.size()) return;
	s32 iOld = miSelected;
	miSelected = idx;
	if( miSelected < 0 )
        miSelected = 0;
    if( miSelected >= (s32)mkItems.size() )
        miSelected = mkItems.size() - 1;
	if(iOld != miSelected)
	{
		mkScroll.showItem(miSelected);
	}
	if(mpkParent)
		mpkParent->onGuiEvent(OnLBSelection,this);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//					cXuiSlider
cXuiSlider::cXuiSlider(nextar::cXuiContainer *pkParent, u32 uId) : 
iXuiWnd(pkParent,xGuiSlider),miValue(0),
miMax(100),miMin(0)
{
	RefObjectInit(cXuiSlider);
	setKey(uId);
}

// update
void cXuiSlider::update()
{
	iXuiWnd::update();
	
	mkAbsRect.bottom = mkAbsRect.top+CV_SliderButtonHeight;
	mkTrack.left = mkAbsRect.left;
	mkTrack.top = mkAbsRect.top+((CV_SliderButtonHeight>>1)-(CV_SliderTrackHeight>>1));
	mkTrack.right =  mkAbsRect.right;
	mkTrack.bottom = mkTrack.top+CV_SliderTrackHeight;
	// the left most part of the button is the value
	mkButton.top = mkAbsRect.top;
	mkButton.bottom = mkAbsRect.bottom;
	_updateThumb();
}

void cXuiSlider::_updateThumb()
{
	s32 iValue = ((s32)(mkTrack.width()-CV_SliderButtonWidth)*miValue)/(miMax-miMin);
	mkButton.left  = mkTrack.left + (i2d)iValue;
	mkButton.right = mkButton.left+CV_SliderButtonWidth;
}

void cXuiSlider::_valueFromPos(i2d iAbsX)
{
	miValue = ((miMax-miMin)*((s32)iAbsX-((s32)mkTrack.left+(CV_SliderButtonWidth>>1))))/((s32)mkTrack.width()-CV_SliderButtonWidth);	
	if(miValue < miMin) miValue = miMin;
	if(miValue >=miMax) 
		miValue = miMax;
	_updateThumb();
	if(mpkParent)
		mpkParent->onGuiEvent(OnSliderChange,this);
}

void cXuiSlider::onMouseEvent(const nextar::cMouseEvent *e)
{
	static i2d iHit = 0;
	switch(e->meType)
	{
	case cMouseEvent::MMove:
		if(mkButton.isInside(e->x,e->y))
			muFlags |= MouseOver;
		else
			muFlags &=~MouseOver;
		break;
	case cMouseEvent::MDrag:
		if(muFlags & ButtonPressed)
			_valueFromPos(e->x-(i2d)iHit);
		break;
	case cMouseEvent::LBDown:
		if(mkButton.isInside(e->x,e->y))
		{
			muFlags |= ButtonPressed;
			iHit = ((e->x-mkButton.left)-(i2d)(CV_SliderButtonWidth>>1));
		}
		else if(mkTrack.isInside(e->x,e->y))
			_valueFromPos(e->x);
		break;
	case cMouseEvent::LBUp:
		muFlags &=~(ButtonPressed|MouseOver);
		iHit = 0;
		break;
	}
}

//gui
void cXuiSlider::onGuiEvent(nextar::iXuiWnd::GuiEvent e, nextar::iXuiWnd *pkCaller)
{
	if(e==OnMouseLeave)
		muFlags &=~(ButtonPressed|MouseOver);
}

void cXuiSlider::setRange(s32 iMin, s32 iMax)
{
	miMin = iMin;
	miMax = iMax;
	_updateThumb();
}

void cXuiSlider::setValue(s32 v)
{
	miValue = v;
	_updateThumb();
	if(mpkParent)
		mpkParent->onGuiEvent(OnSliderChange,this);
}

void cXuiSlider::onDraw()
{
	// get the state
	iXuiSkin::EControlState iState = iXuiSkin::CS_Normal;
	iXuiSkin* pkSkin = X_SharedPtr(cXuiManager)->getSkin();
	if(muFlags & ButtonPressed)
		iState = iXuiSkin::CS_Pressed;
	else if(muFlags & MouseOver)
		iState = iXuiSkin::CS_MouseOver;

	pkSkin->drawSliderTrack(mkTrack);
	pkSkin->drawControlState(mkButton,iXuiSkin::ID_SliderButton,iState,0);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//					cXuiProgressBar
// pg bar
cXuiProgressBar::cXuiProgressBar(cXuiContainer* p,u32 uId) : iXuiWnd(p,xGuiProgressBar)
{
	RefObjectInit(cXuiProgressBar);
	setKey(uId);
	mfPercentageComplete = 0;
}

void cXuiProgressBar::update()
{
	iXuiWnd::update();
	mkAbsRect.bottom = mkAbsRect.top+CV_ProgressBarHeight;
	mkProgress.left	= mkAbsRect.left+CV_ProgressBarBorderWidth;
	mkProgress.top	= mkAbsRect.top+CV_ProgressBarBorderHeight;
	mkProgress.bottom= mkAbsRect.bottom-CV_ProgressBarBorderHeight;
	miProgressWidth = (mkAbsRect.right-(CV_ProgressBarBorderWidth+mkProgress.left));
}

void cXuiProgressBar::onDraw()
{
	iXuiSkin* pkS = X_SharedPtr(cXuiManager)->getSkin();
	mkProgress.right = mkProgress.left + (i2d)((f32)miProgressWidth * mfPercentageComplete);
	pkS->drawControl(mkProgress,iXuiSkin::ID_Progress);
	pkS->drawProgressBarBorder(mkAbsRect,mkCaption);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//					cXuiTabControl
cXuiTabControl::Tab::Tab() : mpkWindow(0)
{
}

cXuiTabControl::cXuiTabControl(cXuiContainer *mpkP, u32 uId,u32 uStyle) : cXuiContainer(mpkP,uStyle),
miFocussed(-1),miStartRender(0),miActive(-1),miEndRender(0)
{
	meType = xGuiTabControl;
	RefObjectInit(cXuiTabControl);
	setKey(uId);
}

void cXuiTabControl::addChild(nextar::iXuiWnd *mpkWnd)
{
	if(mpkWnd->getIsContainer())
	{
		cXuiContainer::addChild(mpkWnd);
		_addChild(mpkWnd);
	}
}

// add
void cXuiTabControl::_addChild(nextar::iXuiWnd *mpkWnd)
{
	// add a new tab
	// new style
	((cXuiContainer*)(mpkWnd))->setBorderFrame(false);
	((cXuiContainer*)(mpkWnd))->setCloseBox(false);
	((cXuiContainer*)(mpkWnd))->setTitleBar(false);
	Tab* NewTab = new Tab();
	mkTabs.push_back((array_t::alloc_type)NewTab);
	NewTab->mpkWindow = mpkWnd;
	NewTab->mpkWindow->setVisible(false);

	const cStr& kCap = mpkWnd->getCaption();

	if(!kCap.strlen())
	{
		cStr kNewCap;
		kNewCap.format("newTab %d",20,mkTabs.size());
		mpkWnd->setCaption(kNewCap);
	}

	if(miActive<0)
	{
		miActive = 0;
		NewTab->mpkWindow->setVisible(true);
	}
	update();
}

// rem
void cXuiTabControl::removeChild(nextar::iXuiWnd *wnd)
{
	cXuiContainer::removeChild(wnd);
	for(array_t::index_type i=0; i<mkTabs.size(); ++i)
	{
		Tab* pkChild = (Tab*)mkTabs[i];
		X_Assert(pkChild);
		if(pkChild->mpkWindow == wnd)
		{
			mkTabs.erase(i);
			if(miActive==i)
			{
				if(mkTabs.size())
				{
					miActive = mkTabs.size()-1;
					((Tab*)mkTabs[miActive])->mpkWindow->setVisible(true);
				}
				else
					miActive = -1;
			}
			if(miFocussed == i)
				miFocussed = -1;
			break;
		}
	}
	update();
}
// update
void cXuiTabControl::update()
{
	const size2_x& kSDim = X_SharedPtr(iRenderer)->getScreenSize();
	
	f32 dx = (f32)kSDim.dx;
	f32 dy = (f32)kSDim.dy;
	mkAbsRect.left = (i2d)( mkPos.x * dx );
	mkAbsRect.top = (i2d)( mkPos.y * dy);
	mkAbsRect.right = mkAbsRect.left + (i2d)(mkDim.x * dx);
	mkAbsRect.bottom = mkAbsRect.top + (i2d)(mkDim.y * dy);

	// update x button
	if( muStyle & CloseBox )
	{
		mkCloseBox.right  = mkAbsRect.right - CV_WinXOffsetX;
		mkCloseBox.top    = mkAbsRect.top   + CV_WinXOffsetY;
		mkCloseBox.left	  = mkCloseBox.right- CV_XButtonWidth;
		mkCloseBox.bottom = mkCloseBox.top  + CV_XButtonHeight;
	}

	mkTabClient.left = mkTabRect.left = mkAbsRect.left+CV_TabEdgeLeft;
	mkTabClient.right = mkAbsRect.right-CV_TabEdgeRight;
	mkTabRect.right = mkCloseBox.left-4;

	mkTabRect.bottom = mkTabClient.top = mkAbsRect.top + CV_TabEdgeTop;
	mkTabRect.bottom--;
	mkTabRect.top = mkTabRect.bottom-CV_TabHeight;
	mkTabClient.bottom = mkAbsRect.bottom - CV_TabEdgeBottom;

	miStartRender = 0;
	miEndRender = 0;
	miActive = -1;
	miFocussed = -1;
	// recalculate the widths cause the font might have changed
	iXuiFont* pkFont = X_SharedPtr(cXuiManager)->getSkin()->getStaticFont();
	muFlags &= ~NotAllTabsVisible;
	i2d uRunningSum = 0;
	
	i2d iWidth = mkTabRect.width();


	f32 x = (f32)mkTabClient.left / dx;
	f32 y = (f32)mkTabClient.top / dy;
	f32 w = (f32)mkTabClient.width()/ dx;
	f32 h = (f32)mkTabClient.height()/ dy;

	for(array_t::index_type i=0; i<mkTabs.size(); ++i)
	{
		Tab* pkChild = (Tab*)mkTabs[i];		
		X_Assert(pkChild);
		pkChild->mpkWindow->setPos(x,y);
		pkChild->mpkWindow->setDim(w,h);
		pkChild->mpkWindow->update();

		pkChild->miHitWidth = (i2d)pkFont->getTextExtents(pkChild->mpkWindow->getCaption(),0,-1)+(CV_TabBorder<<1);
		uRunningSum += pkChild->miHitWidth;
		if(uRunningSum < iWidth)
			miEndRender = i+1;
		pkChild->mpkWindow->setVisible(false);
	}

	if(uRunningSum > iWidth)
		muFlags |= NotAllTabsVisible;
#ifdef X_DEBUG
	else
	{
		X_Assert(miEndRender==mkTabs.size());
	}
#endif
	
	if(mkTabs.size())
	{
		miActive = 0;
		Tab* pkChild = (Tab*)mkTabs[0];
		pkChild->mpkWindow->setVisible(true);
	}
}

void cXuiTabControl::onGuiEvent(iXuiWnd::GuiEvent e,iXuiWnd* w)
{
	if(e==OnMouseLeave)
	{
		muFlags &= ~(FocusPressed);
		miFocussed = -1;
	}
	cXuiContainer::onGuiEvent(e,w);
}

void cXuiTabControl::onMouseEvent(const cMouseEvent* e)
{
	// see if clicked 
	if(mkTabs.size() && mkTabRect.isInside(e->x,e->y))
	{
		switch(e->meType)
		{
		case cMouseEvent::LBUp:
			if(miFocussed>=0)
			{
				if(miFocussed != miActive)
				{
					Tab* pkActive = (Tab*)mkTabs[miActive];
					// we assume focus was hit
					//if(pkFocussed->mkHit.isInside(e->x,e->y))
					if(pkActive) 
						pkActive->mpkWindow->setVisible(false);
					pkActive = (Tab*)mkTabs[miFocussed];
					pkActive->mpkWindow->setVisible(true);
					miActive = miFocussed;
					mpkParent->onGuiEvent(OnNewTabActivated,this);
					if(muFlags & NotAllTabsVisible)
					{
						// scroll the tabs here
						// last tab hit
						if(miFocussed == miEndRender-1 && miEndRender<(s32)mkTabs.size())
						{
							miEndRender++;
							_scrollLeft();
						}
						else if(miFocussed == miStartRender && miStartRender)
						{
							miStartRender--;
							_scrollRight();
						}
						_focusOn(e->x);
					}
				}
			}
			muFlags &=~FocusPressed;
			break;
		case cMouseEvent::LBDown:
			if(miFocussed>=0)
			{
				muFlags |= FocusPressed;
			}
			break;
		case cMouseEvent::MMove:
			_focusOn(e->x);
			break;
		}
	}
	else if(e->y < mkTabRect.bottom)
		cXuiContainer::onMouseEvent(e);
	else if(miActive)
	{
		((Tab*)mkTabs[miActive])->mpkWindow->onMouseEvent(e);
	}
}

void cXuiTabControl::_scrollLeft()
{
	miStartRender = miEndRender;
	if(!mkTabs.size()) { return; } 
	// end constant
	// find start
	i2d width = mkTabRect.width();
	i2d sum = 0;
	for(s32 i = miEndRender-1; i >= 0; i--)
	{
		X_Assert(i<(s32)mkTabs.size());

		Tab* pkChild = (Tab*)mkTabs[i];
		sum += pkChild->miHitWidth;
		if(sum > width) break;
		miStartRender--;
	}
}

void cXuiTabControl::_scrollRight()
{
	miEndRender = miStartRender;
	if(!mkTabs.size()) { return; } 
	// start constant
	// find end
	i2d width = mkTabRect.width();
	i2d sum = 0;
	for(s32 i = miStartRender; i < (s32)mkTabs.size(); i++)
	{
		Tab* pkChild = (Tab*)mkTabs[i];
		sum += pkChild->miHitWidth;
		if(sum > width) break;
		miEndRender++;
	}
}

void cXuiTabControl::_focusOn(i2d mx)
{
	mx -= mkTabRect.left;
	miFocussed = -1;
	if(!mkTabs.size()) { return; } 
	if(mx<0) return;

	i2d sum = 0;
	for(s32 i=miStartRender; i < miEndRender; ++i)
	{
		Tab* pkChild = (Tab*)mkTabs[i];
		sum += pkChild->miHitWidth;
		miFocussed = i;
		if(sum > mx) 
			break;
	}
}

void cXuiTabControl::onDraw()
{
	iXuiSkin* pkS = X_SharedPtr(cXuiManager)->getSkin();
	pkS->drawTabWindow(mkAbsRect);

	if(muStyle & CloseBox)
	{
		iXuiSkin::EControlState e = iXuiSkin::CS_Normal;
		if(muFlags & CloseButtonPressed)
			e = iXuiSkin::CS_Pressed;
		else if(muFlags & CloseButtonMouseOver)
			e = iXuiSkin::CS_MouseOver;
		pkS->drawControlState(mkCloseBox,iXuiSkin::ID_BWinCloseButton,e,0);
	}

	rect2_x	rc(mkTabRect);
	rect2_x	active;
	for(s32 i=miStartRender; i<miEndRender; ++i)
	{
		X_Assert(i<(s32)mkTabs.size());

		Tab* pkChild = (Tab*)mkTabs[i];
		rc.right = rc.left + pkChild->miHitWidth;
		if(i != miActive)
		{			
			iXuiSkin::EControlState e = iXuiSkin::CS_Disabled;	
			if(i == miFocussed)
			{
				if(muFlags & FocusPressed)
					e = iXuiSkin::CS_Pressed;
				else
					e = iXuiSkin::CS_MouseOver;
			}
			pkS->drawControlState(rc,iXuiSkin::ID_Tab,e,pkChild->mpkWindow->getCaption());
		}
		else
			active = rc;
		rc.left = rc.right;
	}
	if(miActive>=0)
	{
		pkS->drawControlState(active,iXuiSkin::ID_Tab,iXuiSkin::CS_Normal,((Tab*)mkTabs[miActive])->mpkWindow->getCaption());
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//					cXuiEditBox
cXuiEditBox::cXuiEditBox(cXuiContainer* pkContainer,u32 iD) : 
iXuiWnd(pkContainer,xGuiEditBox),muMaxChar(-1),mpkFont(0)
{
	RefObjectInit(cXuiEditBox);
	// assuming initializations done
	miTextStart = 0;
	miTextEnd = 0;
	miCaretPos = 0;
	miMarkerStart = miMarkerEnd = 0;
	miColor = CV_EditFontColor;
}

cXuiEditBox::~cXuiEditBox()
{
	if( mpkFont ) mpkFont->release();
}

// update
void cXuiEditBox::update()
{
	if(!mpkFont)
	{
		mpkFont = X_SharedPtr(cXuiManager)->getSkin()->getEditFont();
		miColor = CV_EditFontColor;
		miCaretColor = CV_CaretColor;
		X_Assert(mpkFont);
		mpkFont->addRef();
	}

	iXuiWnd::update();
	mkAbsRect.bottom = mkAbsRect.top+CV_EditBoxHeight;
	mkTextRect.left	= mkAbsRect.left+CV_EditBoxBorderWidth;
	mkTextRect.top	= mkAbsRect.top+CV_EditBoxBorderHeight;
	mkTextRect.right = mkAbsRect.right-CV_EditBoxBorderWidth;
	mkTextRect.bottom= mkAbsRect.bottom-CV_EditBoxBorderHeight;
	i2d xTra = ((mkTextRect.height()-mpkFont->getCharacterHeight())>>1);
	mkMarker.top = mkTextRect.top;
	mkMarker.bottom = mkTextRect.bottom;
	mkTextRect.top += xTra;
	mkTextRect.bottom -= xTra;
	// reinitialize variables
}

void cXuiEditBox::setFont(u32 key, u32 color, u32 caret_colr)
{
	iXuiFont* pkF = X_SharedPtr(cXuiManager)->getFont(key);
	if( pkF ) 
	{
		if(mpkFont) mpkFont->release();
		mpkFont = pkF;
		mpkFont->addRef();
	}
	miColor = color;
	miCaretColor = caret_colr;
	update();
}
// onmouseevent
//
void cXuiEditBox::onMouseEvent(const nextar::cMouseEvent *e)
{
	switch(e->meType)
	{
	case cMouseEvent::RBDown:
	case cMouseEvent::LBDown:
		miMarkerStart = miMarkerEnd = miCaretPos = _getCharacter(e->x);
		miDragXStart = miCaretX;
		break;
	case cMouseEvent::MDrag:
		miMarkerEnd = miCaretPos = _getCharacter(e->x);
		miDragXEnd = miCaretX;
		break;
	}
}

void cXuiEditBox::onKeyEvent(const cKeyEvent* k)
{
	if(k->muFlags & cKeyEvent::Shift)
	{
		if(!mkCaption.strlen()) return;
		switch(k->muKeyValue)
		{
		case KEY_LEFT: 
			if(miMarkerEnd)
			{
				miMarkerEnd--;
				miCaretPos = miMarkerEnd;
				if(miCaretPos < miTextStart)
				{
					miTextStart = miCaretPos;
					_scrollRight();
				}
				else
				{
					miCaretX -= mpkFont->getWidth(mkCaption[miCaretPos]);
				}
				_calMarkerRect();
			}
			break;
		case KEY_RIGHT: 
			if(miMarkerEnd<mkCaption.strlen())
			{
				miMarkerEnd++;
				miCaretPos = miMarkerEnd;
				if(miCaretPos > miTextEnd)
				{
					miTextEnd = miCaretPos;
					_scrollLeft();
				}
				else
				{
					miCaretX += mpkFont->getWidth(mkCaption[miCaretPos-1]);
				}
				_calMarkerRect();
			}
			break;
		case KEY_END: 
			miMarkerEnd = mkCaption.strlen();
			miTextEnd = miCaretPos = miMarkerEnd;
			_scrollLeft();
			_calMarkerRect();
			break;
		case KEY_HOME: 
			miMarkerEnd = 0;
			miTextStart = miCaretPos = 0;
			_scrollRight();
			_calMarkerRect();
			break;
		}
	}
	if(k->muFlags & cKeyEvent::Ctrl)
	{
		switch(k->muKeyValue)
		{
		case KEY_C: 
			if(miMarkerEnd != miMarkerStart)
			{
				if(miMarkerStart<miMarkerEnd)
					X_SharedPtr(iSystem)->copyToClipboard(&mkCaption[miMarkerStart],miMarkerEnd-miMarkerStart);
				else
					X_SharedPtr(iSystem)->copyToClipboard(&mkCaption[miMarkerEnd],miMarkerStart-miMarkerEnd);
			}
			break;
		case KEY_V: 
			{
				const tchar* stxt = X_SharedPtr(iSystem)->pasteFromClipboard();
				if(!stxt || !*stxt) break;
				cStr txt = stxt;
				if(miMarkerEnd != miMarkerStart)
				{
					s32 realmb,realme;
					if(miMarkerStart<miMarkerEnd)
					{
						realmb = miMarkerStart;
						realme = miMarkerEnd;
					}
					else
					{
						realmb = miMarkerEnd;
						realme = miMarkerStart;
					}
					
					mkCaption.erase(realmb,realme);
					if((u32)(mkCaption.strlen()+txt.strlen())>=muMaxChar)
					{
						txt.erase_last(muMaxChar-mkCaption.strlen()+txt.strlen());
					}
					mkCaption.insert(realmb,txt);
					miMarkerEnd = miMarkerStart = miCaretPos = realmb + txt.strlen();
					miTextEnd = mkCaption.strlen();
					_scrollLeft();
					
				}
				else
				{
					mkCaption.insert(miCaretPos,txt);
					miCaretPos+=txt.strlen();
					miMarkerEnd = miMarkerStart = miCaretPos;
					miTextEnd = mkCaption.strlen();
					_scrollLeft();
				}
			}
			break;
			miDragXStart = miCaretX;
		case KEY_X: 
			if(miMarkerEnd != miMarkerStart)
			{
				s32 realmb,realme;
				if(miMarkerStart<miMarkerEnd)
				{
					realmb = miMarkerStart;
					realme = miMarkerEnd;
				}
				else
				{
					realmb = miMarkerEnd;
					realme = miMarkerStart;
				}
				cStr substrs;
				mkCaption.substr(substrs,realmb,realme-realmb);
				mkCaption.erase(realmb,realme);
				miMarkerEnd = miMarkerStart = miCaretPos = realmb;
				miTextEnd = mkCaption.strlen();
				_scrollLeft();

				X_SharedPtr(iSystem)->copyToClipboard(substrs,substrs.strlen());
			}
			miDragXStart = miCaretX;
			break;
		case KEY_A: 
			miMarkerStart = 0;
			miTextEnd = miMarkerEnd = miCaretPos = mkCaption.strlen();
			_scrollLeft();
			_calMarkerRect();
			break;
		}
	}
	else if(k->mcChar >= 32 &&  k->mcChar < 128)
	{
		if((u32)mkCaption.strlen()< muMaxChar) 
		{
			// these are printable characters
			if( miMarkerStart == miMarkerEnd )
			{
				mkCaption.insert(miCaretPos++,k->mcChar);
				miMarkerEnd = miMarkerStart = miCaretPos;
				miTextEnd++;
				s32 w = mpkFont->getWidth(k->mcChar) + miCaretX;
				
				if(w+1 >= mkTextRect.right)
					_scrollLeft();
				else if(mpkFont->getTextExtents(mkCaption,miTextStart,miTextEnd) >= mkTextRect.width()-1)
				{
					_scrollRight();
				}
				else
					miCaretX = w;
			}
			else
			{
				s32 realmb,realme;
				if(miMarkerStart<miMarkerEnd)
				{
					realmb = miMarkerStart;
					realme = miMarkerEnd;
				}
				else
				{
					realme = miMarkerStart;
					realmb = miMarkerEnd;
				}
				
				mkCaption.replace(k->mcChar,realmb,realme);
			
				miMarkerEnd = miCaretPos = miMarkerStart = realmb;
				miTextEnd = mkCaption.strlen();
				X_Assert(miCaretPos>=0);
				_scrollLeft();
			}
			miDragXStart = miCaretX;
		}
	}
	else if(!(k->muFlags & cKeyEvent::Shift))
	{
		switch(k->muKeyValue)
		{
		case KEY_LEFT:
			miCaretPos--;
			if(miCaretPos<miTextStart)
			{
				if(miTextStart)
				{
					--miTextStart;
					_scrollRight();
				}
				else
					miCaretPos = 0;
			}
			else
			{
				miCaretX -= mpkFont->getWidth(mkCaption[miCaretPos]);
			}
			miMarkerStart = miMarkerEnd = miCaretPos;
			break;
		case KEY_RIGHT: 
			miCaretPos++;
			if(miCaretPos>miTextEnd)
			{
				if(miTextEnd < mkCaption.strlen())
				{
					miTextEnd++;
					_scrollLeft();
				}
				else
					miCaretPos = miTextEnd;
			}
			else
			{
				miCaretX += mpkFont->getWidth(mkCaption[miCaretPos-1]);
			}
			miMarkerStart = miMarkerEnd = miCaretPos;
			break;
		case KEY_END: 
			miCaretPos = mkCaption.strlen();
			if(miTextEnd < mkCaption.strlen())
			{
				miTextEnd = mkCaption.strlen();
				_scrollLeft();
			}
			else
			{
				_calCaretPos();
			}
			miMarkerStart = miMarkerEnd = miCaretPos;
			break;
		case KEY_HOME: 
			miCaretPos = 0;
			if(miTextStart)
			{
				miTextStart = 0;
				_scrollRight();
			}
			else
				_calCaretPos();
			miMarkerStart = miMarkerEnd = miCaretPos;
			break;
		case KEY_DELETE:
			if( miMarkerStart == miMarkerEnd)
			{
				if(miCaretPos < mkCaption.strlen())
				{
					mkCaption.erase(miCaretPos);
					if(miTextEnd < mkCaption.strlen())
					{
						miTextEnd++;
						_scrollLeft();
					}
					else
						miTextEnd = mkCaption.strlen();
				}
			}
			else
			{
				s32 realmb,realme;
				if(miMarkerStart<miMarkerEnd)
				{
					realmb = miMarkerStart;
					realme = miMarkerEnd;
				}
				else
				{
					realme = miMarkerStart;
					realmb = miMarkerEnd;
				}
				
				mkCaption.erase(realmb,realme);
			
				miMarkerEnd = miCaretPos = miMarkerStart = realmb;
				miTextEnd = mkCaption.strlen();
				_scrollLeft();
				X_Assert(miCaretPos>=0);
			}
			break;
		case KEY_BACK: 
			if( miMarkerStart == miMarkerEnd)
			{
				if(miCaretPos > 0)
				{
					--miCaretPos;
					if(miCaretPos<miTextStart)
					{
						if(miTextStart)
						{
							--miTextStart;
							_scrollRight();
						}
						else
							miCaretPos = 0;
					}
					else
					{
						miCaretX -= mpkFont->getWidth(mkCaption[miCaretPos]);
					}
					mkCaption.erase(miCaretPos);

					if(miTextEnd < mkCaption.strlen())
					{
						miTextEnd++;
						_scrollLeft();
					}
					else
						miTextEnd = mkCaption.strlen();
				}
				miMarkerStart = miMarkerEnd = miCaretPos;
			}
			else 
			{
				s32 realmb,realme;
				if(miMarkerStart<miMarkerEnd)
				{
					realmb = miMarkerStart;
					realme = miMarkerEnd;
				}
				else
				{
					realme = miMarkerStart;
					realmb = miMarkerEnd;
				}
				
				mkCaption.erase(realmb,realme);
			
				miMarkerEnd = miCaretPos = miMarkerStart = realmb;
				miTextEnd = mkCaption.strlen();
				_scrollLeft();
				X_Assert(miCaretPos>=0);
			}
			break;
		}
		miDragXStart = miCaretX;
	}
}

void cXuiEditBox::_scrollLeft()
{
	// in this case we keep
	// the left edge fixed (end ptr)
	// and vary the start
	iXuiFont::cFontChar* f = mpkFont->getFontCharacters();
	const tchar* txt = mkCaption;
	i2d w = 0;
	miTextStart = miTextEnd;
	i2d mw = mkTextRect.width()-1;

	for(s32 i=miTextEnd-1; i>=0; i--)
	{
		u32 ch = txt[i]-32;
		X_Assert(ch<128);
		w += f[ch].width;
		if(w>=mw) break;
		miTextStart--;
	}

	miCaretX = mkTextRect.left;
	for(s32 i=miTextStart; i<miCaretPos; i++)
	{
		u32 ch = txt[i]-32;
		X_Assert(ch<128);
		miCaretX += f[ch].width;
	}
}

void cXuiEditBox::_calCaretPos()
{
	iXuiFont::cFontChar* f = mpkFont->getFontCharacters();
	const tchar* txt = mkCaption;
	miCaretX = mkTextRect.left;
	for(s32 i=miTextStart; i<miCaretPos; i++)
	{
		u32 ch = txt[i]-32;
		X_Assert(ch<128);
		miCaretX += f[ch].width;
	}
}

void cXuiEditBox::_scrollRight()
{
	// the start index is constatn
	iXuiFont::cFontChar* f = mpkFont->getFontCharacters();
	const tchar* txt = mkCaption;
	i2d w = 0;
	i2d mw = mkTextRect.width()-1;

	miCaretX = mkTextRect.left;
	s32 i=miTextStart;
	for(; i<miTextEnd ; i++)
	{
		u32 ch = txt[i]-32;
		X_Assert(ch<128);
		w += f[ch].width;
		if(w>=mw) break;
		if(i==miCaretPos-1)
			miCaretX += w;
	}
	miTextEnd = i;
}

s32 cXuiEditBox::_getCharacter(i2d mouse_x)
{
	iXuiFont::cFontChar* f = mpkFont->getFontCharacters();
	i2d muse = mouse_x - mkTextRect.left;
	s32 idx = miTextStart;
	i2d running_sum = 0;

	miCaretX = 0;
	while(idx < miTextEnd)
	{
		running_sum += f[mkCaption[idx]-32].width;
		if(running_sum>=muse)
			break;
		miCaretX = running_sum;
		++idx;
	}
	miCaretX += mkTextRect.left;
	return idx;
}

void cXuiEditBox::_calMarkerRect()
{
	s32 realmb,realme;
	if(miMarkerStart<miMarkerEnd)
	{
		realmb = miMarkerStart;
		realme = miMarkerEnd;
	}
	else
	{
		realmb = miMarkerEnd;
		realme = miMarkerStart;
	}

	iXuiFont::cFontChar* f = mpkFont->getFontCharacters();
	const tchar* txt = mkCaption;
	
	miDragXStart = mkTextRect.left;
	if(realmb > miTextStart)
	{
		for(s32 i=miTextStart; i<realmb; i++)
		{
			u32 ch = txt[i]-32;
			X_Assert(ch<128);
			miDragXStart += f[ch].width;
		}
	}
	else
		realmb = miTextStart;

	if(realme > miTextEnd)
		miDragXEnd = mkTextRect.right;
	else
	{
		miDragXEnd = miDragXStart;
		for(s32 i=realmb; i<realme; i++)
		{
			u32 ch = txt[i]-32;
			X_Assert(ch<128);
			miDragXEnd += f[ch].width;
		}
	}
}

void cXuiEditBox::onDraw()
{
	static bool visible = false;
	static u32	timestamp = 0;

//	X_SharedPtr(iRenderer)->draw2DRect(0xffff0000,mkTextRect.left,mkTextRect.top,mkTextRect.right,mkTextRect.bottom);
	iXuiSkin* pkS = X_SharedPtr(cXuiManager)->getSkin();
	pkS->drawEditControlBorder(mkAbsRect);

	if(miMarkerStart!=miMarkerEnd)
	{
		if(miDragXStart<miDragXEnd)
		{
			mkMarker.left  = miDragXStart;
			mkMarker.right = miDragXEnd;
		}
		else
		{
			mkMarker.left  = miDragXEnd;
			mkMarker.right = miDragXStart;
		}
		pkS->drawControl(mkMarker,iXuiSkin::ID_EditTextSelection);
	}
		

	if(miTextStart!=miTextEnd)
		mpkFont->drawSingleLineText(mkTextRect.left,mkTextRect.top,miColor,&mkCaption[miTextStart],miTextEnd-miTextStart);

	if(muFlags & HasFocus)
	{
		u32 uCurTime = timer_t::skAsynchTimer.getFrameTime();
		if(uCurTime-timestamp > N3DCaretBlinkRate)
		{
			timestamp = uCurTime;
			visible = !visible;	
		}
		if(visible)
			X_SharedPtr(iRenderer)->draw2DLine(coord2_x(miCaretX,mkTextRect.top),coord2_x(miCaretX,mkTextRect.bottom),miCaretColor);
	}
}


// Read/Write Section
#define ReadFlag(flag)	\
	tmp = e->find(#flag); \
	if(tmp)	set##flag(tmp->getAsBoolean());
#define WriteFlag(flag)	\
	e->addAttr(#flag)->setAsBoolean(get##flag());
#define ReadStyle(flag)	\
	tmp = e->find(#flag); \
	if(tmp)	set##flag(tmp->getAsBoolean());
#define WriteStyle(flag)	\
	e->addAttr(#flag)->setAsBoolean(get##flag()); \

// -------------iXuiWnd---------------
// parse read
void iXuiWnd::read(iXMLElement* e)
{
	iXMLAttribute* pkCap = e->find("Caption");
	iXMLAttribute* pkID = e->find("ID");
	iXMLAttribute* pkPos = e->find("Position");
	iXMLAttribute* pkDim = e->find("Dimension");
	iXMLAttribute* pkHotK = e->find("HotKey");
	
	if(pkCap) mkCaption = pkCap->getValue();
	if(pkPos) { pkPos->getAs2Float((float*)&mkPos); }
	if(pkDim) { pkDim->getAs2Float((float*)&mkDim); }
	if(pkHotK) { muHotKey = pkHotK->getAsUnsignedInt(); }
	if(pkID) { setKey(pkID->getAsHexValue()); }

	// read flags
	iXMLAttribute* tmp = 0;

	ReadFlag(Visible);
	ReadFlag(CanHaveFocus);
	ReadFlag(Default);
	ReadFlag(Disabled);
	
}

void iXuiWnd::write(iXMLElement* e)
{
	if(mkCaption.strlen()) e->addAttr("Caption")->setValue(mkCaption);
	e->addAttr("ID")->setAsHexValue(getKey());
	e->addAttr("Position")->setAs2Float((float*)&mkPos);
	e->addAttr("Dimension")->setAs2Float((float*)&mkDim);
	e->addAttr("HotKey")->setAsUnsignedInt(muHotKey);


	WriteFlag(Visible);
	WriteFlag(CanHaveFocus);
	WriteFlag(Default);
	WriteFlag(Disabled);
}
// -------------cXuiContainer---------------
cXuiContainer::cXuiContainer(cXuiContainer *pkParent,iXMLElement *e) : iXuiWnd(pkParent,xGuiDialog,iXuiWnd::Visible|iXuiWnd::CanHaveFocus|iXuiWnd::IsContainer),
mpkFirstChild(0), mpkLastChild(0),mpkBack(0),mpkCallback(0)
{
	RefObjectInit(cXuiContainer);
	read(e);
}

void cXuiContainer::parseChild(iXMLElement *e)
{
	const tchar* str[] =
	{
		"xGuiDialog",
		"xGuiButton",
		"xGuiCheckbox",
		"xGuiRadioButton",
		"xGuiStaticText",
		"xGuiVScrollBar",
		"xGuiListBox",
		"xGuiComboBox",
		"xGuiSlider",
		"xGuiProgressBar",
		"xGuiTabControl",
		"xGuiEditBox",
		0
	};

	const cStr& cmp = e->getValue();
	s32 i = 0;
	for(; *str[i]; i++)
	{
		if(cmp == str[i])
			break;
	}
	iXuiWnd* wnd = 0;
	switch(i)
	{
	case xGuiDialog:
		wnd = new cXuiContainer(this,e); break;
	case xGuiButton:
		wnd = new cXuiButton(this,e); break;
	case xGuiCheckbox:
		wnd = new cXuiCheckbox(this,e); break;
	case xGuiRadioButton:
		wnd = new cXuiRadioButton(this,e); break;
	case xGuiStaticText:
		wnd = new cXuiStaticText(this,e); break;
	case xGuiVScrollBar:
		wnd = new cXuiVScrollBar(this,e); break;
	case xGuiListBox:
		wnd = new cXuiListBox(this,e); break;
	case xGuiComboBox:
		wnd = new cXuiComboBox(this,e); break;
	case xGuiSlider:
		wnd = new cXuiSlider(this,e); break;
	case xGuiProgressBar:
		wnd = new cXuiProgressBar(this,e); break;
	case xGuiTabControl:
		wnd = new cXuiTabControl(this,e); break;
	case xGuiEditBox:
		wnd = new cXuiEditBox(this,e); break;
	}
	if(wnd)
		wnd->release();
}

void cXuiContainer::read(nextar::iXMLElement *e)
{
	iXuiWnd::read(e);

	iXMLAttribute* tmp = 0;

	ReadStyle(BorderFrame);
	ReadStyle(TitleBar);
	ReadStyle(CloseBox);
	tmp = e->find("BackgroundImage");
	if(tmp) { if(tmp->getAsBoolean()) muStyle |= BackgroundImage; else muStyle &= BackgroundImage; }

	if(muStyle & BackgroundImage)
	{
		cStr path; u32 key = X_InvalidKey;
		u32 imageprop = TCF_16B;
		u32 color = 0xffffffff;
		tmp = e->find("ImagePath");
		if(tmp) path = tmp->getValue();
		tmp = e->find("ImageKey");
		if(tmp) key = tmp->getAsHexValue();
		tmp = e->find("Image32Bit");
		if(tmp && tmp->getAsBoolean()) imageprop = TCF_32B;
		u32 cf = X_SharedPtr(iTextureFactory)->getFlags();
		X_SharedPtr(iTextureFactory)->setFlags(imageprop);
		iTexture* t = X_SharedPtr(iTextureFactory)->load(path,key);
		X_SharedPtr(iTextureFactory)->setFlags(cf);
		if(t)
		{
			tmp = e->find("ImageColor");
			if(tmp) color = tmp->getAsHexValue();
			float uv[4] = {0.f,0.f,1.f,1.f};
			tmp = e->find("ImageRect");
			if(tmp) tmp->getAs4Float(uv);
			setBackground(t,uv,(s32)color);
		}
	}

	iXMLNode::iXMLIterator it = e->firstChildIt();
	while(it.isValid())
	{
		if((*it)->getType()==iXMLNode::XML_ELEMENT)
		{
			parseChild((iXMLElement*)(*it));
		}
		it = it.next();
	}
}

void cXuiContainer::write(nextar::iXMLElement *e)
{
//	p->addChildLast(e);

	iXuiWnd::write(e);
	e->setValue("xGuiDialog");

	WriteStyle(BorderFrame);
	WriteStyle(TitleBar);
	WriteStyle(CloseBox);
	e->addAttr("BackgroundImage")->setAsBoolean(bool(muFlags & BackgroundImage));
	
	if(muStyle & BackgroundImage && mpkBack)
	{
		u32 key = mpkBack->mpkTexture->getKey();
		bool _32bits = false;
		if(mpkBack->mpkTexture->getColorFotmat() == CF_A8R8G8B8)
			_32bits = true;
		s32 color = mpkBack->miColor;

		e->addAttr("ImageKey")->setAsHexValue(key);
		e->addAttr("ImageColor")->setAsHexValue(color);
		e->addAttr("Image32Bit")->setAsBoolean(_32bits);
		e->addAttr("ImageRect")->setAs4Float(mpkBack->mkSrc);
	}

	iXMLDocument* doc = e->getDocument();
	
	iXuiWnd* wnd = mpkFirstChild;
	while(wnd)
	{
		iXMLElement* ec = (iXMLElement*)doc->createNode(iXMLNode::XML_ELEMENT);
		e->addChildLast(ec);
		wnd->write(ec);
		wnd = wnd->next();
	}
}

// -------------cXuiButton---------------
cXuiButton::cXuiButton(cXuiContainer* pkP,iXMLElement* e) : iXuiWnd(pkP,xGuiButton)
{
	RefObjectInit(cXuiButton);
	read(e);
}

void cXuiButton::write(nextar::iXMLElement *e)
{
	iXuiWnd::write(e);
	e->setValue("xGuiButton");
}
// -------------cXuiCheckBox---------------
cXuiCheckbox::cXuiCheckbox(nextar::cXuiContainer *pkP, nextar::iXMLElement *e) : cXuiButton(pkP,X_InvalidKey)
{
	RefObjectInit(cXuiCheckbox);
	read(e);
	meType = xGuiCheckbox;
}

void cXuiCheckbox::read(iXMLElement* e)
{
	cXuiButton::read(e);
	iXMLAttribute* tmp;
	ReadFlag(Checked);
}

void cXuiCheckbox::write(iXMLElement* e)
{
	cXuiButton::write(e);
	e->setValue("xGuiCheckbox");	
	WriteFlag(Checked);
}
// -------------cXuiRadioButton---------------
cXuiRadioButton::cXuiRadioButton(cXuiContainer* pkP,iXMLElement* e) : cXuiCheckbox(pkP,X_InvalidKey),mpkGroupPtr(0)
{
	RefObjectInit(cXuiRadioButton);
	read(e);
	meType = xGuiRadioButton;
}

void cXuiRadioButton::read(nextar::iXMLElement *e)
{
	cXuiCheckbox::read(e);
	iXMLAttribute* attr = e->find("RBGroup");
	if(attr)
	{
		mpkGroupPtr = mpkParent->getRadioGroup(attr->getAsHexValue());
	}
}

void cXuiRadioButton::write(nextar::iXMLElement *e)
{
	cXuiCheckbox::write(e);
	if(mpkGroupPtr)
		e->addAttr("RBGroup")->setAsHexValue(mpkGroupPtr->getId());
}

//--------cXuiStaticText---------
cXuiStaticText::cXuiStaticText(nextar::cXuiContainer *pkParent, nextar::iXMLElement *e) : iXuiWnd(pkParent,xGuiStaticText,Visible),
miColor(CV_StaticFontColor),mpkFont(0)
{
	RefObjectInit(cXuiStaticText);
	read(e);
}

void cXuiStaticText::read(nextar::iXMLElement *e)
{
	iXuiWnd::read(e);
	iXMLAttribute* tmp;
	u32 font,color=CV_StaticFontColor;
	ReadFlag(Bordered);
	tmp = e->find("Font");
	if(tmp)
	{
		font = tmp->getAsHexValue();
		tmp = e->find("FontColor");
		if(tmp)
		{
			color = tmp->getAsHexValue();
		}
		setFont(font,(s32)color);
	}
}

void cXuiStaticText::write(iXMLElement *e)
{
	iXuiWnd::write(e);
	e->setValue("xGuiStaticText");
	WriteFlag(Bordered);
	if(mpkFont)	e->addAttr("Font")->setAsHexValue(mpkFont->getKey());
	e->addAttr("FontColor")->setAsHexValue((u32)miColor);
}

//------cXuiVScrollBar-------
cXuiVScrollBar::cXuiVScrollBar(cXuiContainer* pkP,iXMLElement* e) : iXuiWnd(pkP,xGuiVScrollBar),
miPageSize(1),miStart(0),miEnd(1),miPos(0),muArrowTS(0)
{
	RefObjectInit(cXuiVScrollBar);
	read(e);
}

void cXuiVScrollBar::read(iXMLElement *e)
{
	iXuiWnd::read(e);
	e->addAttr("ScrollPos")->setAsSignedInt(miPos);
	e->addAttr("PageSize")->setAsSignedInt(miPageSize);
	e->addAttr("ScrollStart")->setAsSignedInt(miStart);
	e->addAttr("ScrollEnd")->setAsSignedInt(miEnd);
}

void cXuiVScrollBar::write(iXMLElement *e)
{
	iXuiWnd::write(e);
	e->setValue("xGuiVScrollBar");
	iXMLAttribute* tmp;
#define XRDATT(name,val) tmp = e->find(name); if(tmp) val = tmp->getAsSignedInt();
	XRDATT("ScrollPos",miPos);
	XRDATT("PageSize",miPageSize);
	XRDATT("ScrollStart",miStart);
	XRDATT("ScrollEnd",miEnd);
#undef XRDATT
}

//-------cXuiListBox---------
cXuiListBox::cXuiListBox(nextar::cXuiContainer *pkParent, nextar::iXMLElement *e) : iXuiWnd(pkParent,xGuiListBox),
miSelected(-1), miSelStart(0) ,mkScroll(0)
{
	RefObjectInit(cXuiListBox);
	read(e);
}

void cXuiListBox::read(iXMLElement *e)
{
	iXuiWnd::read(e);
	iXMLAttribute* tmp;
	ReadFlag(MultiSelection);
	s32 iCount = 0;
#define XRDATT(name,val) tmp = e->find(name); if(tmp) val = tmp->getAsSignedInt();
	XRDATT("ItemCount",iCount);
	XRDATT("Selected",miSelected);
	XRDATT("SelectionStart",miSelStart);
#undef XRDATT
	if(iCount > 0)
	{
		mkItems.reserve(iCount);
		iXMLElement* itm = (iXMLElement*)e->findChild("items");
		if(itm)
		{
			for(s32 i = 0; i < iCount; i++)
			{
				cStr item_;
				item_.format("Item%d",20,i);
				iXMLAttribute* att = itm->find(item_);
				if(att) { addItem(att->getValue()); }
			}
		}
	}
}

void cXuiListBox::write(nextar::iXMLElement *e)
{
	iXuiWnd::write(e);
	e->setValue("xGuiListBox");
	WriteFlag(MultiSelection);
	e->addAttr("ItemCount")->setAsSignedInt((s32)mkItems.size());
	e->addAttr("Selected")->setAsSignedInt(miSelected);
	e->addAttr("SelectionStart")->setAsSignedInt(miSelStart);
	if(mkItems.size())
	{
		iXMLDocument* doc = e->getDocument();
		iXMLElement* itm = (iXMLElement*)doc->createNode(iXMLNode::XML_ELEMENT);
		e->addChildLast(itm);
		itm->setValue("items");
		for(s32 i = 0; i < (s32)mkItems.size(); i++)
		{
			cStr item_;
			item_.format("Item%d",20,i);
			Item* t = (Item*)mkItems[i];
			itm->addAttr(item_)->setValue(t->mkString);
		}
	}
}

//-----cXuiComboBox----
cXuiComboBox::cXuiComboBox(nextar::cXuiContainer *pkParent, nextar::iXMLElement *e) : iXuiWnd(pkParent,xGuiComboBox),
mkScroll(0), 
miFocussed(-1), miSelected(-1), miDropHeight(32)
{
	RefObjectInit(cXuiComboBox);
	read(e);
}

void cXuiComboBox::read(nextar::iXMLElement *e)
{
	iXuiWnd::read(e);
	iXMLAttribute* tmp;
	s32 iCount = 0;
#define XRDATT(name,val) tmp = e->find(name); if(tmp) val = tmp->getAsSignedInt();
	XRDATT("ItemCount",iCount);
	XRDATT("Selected",miSelected);
#undef XRDATT
	if(iCount > 0)
	{
		mkItems.reserve(iCount);
		iXMLElement* itm = (iXMLElement*)e->findChild("items");
		if(itm)
		{
			for(s32 i = 0; i < iCount; i++)
			{
				cStr item_;
				item_.format("Item%d",20,i);
				iXMLAttribute* att = itm->find(item_);
				if(att) { addItem(att->getValue()); }
			}
		}
	}
}

void cXuiComboBox::write(nextar::iXMLElement *e)
{
	iXuiWnd::write(e);
	e->setValue("xGuiComboBox");
	e->addAttr("ItemCount")->setAsSignedInt((s32)mkItems.size());
	e->addAttr("Selected")->setAsSignedInt(miSelected);
	if(mkItems.size())
	{
		iXMLDocument* doc = e->getDocument();
		iXMLElement* itm = (iXMLElement*)doc->createNode(iXMLNode::XML_ELEMENT);
		e->addChildLast(itm);
		itm->setValue("items");
		for(s32 i = 0; i < (s32)mkItems.size(); i++)
		{
			cStr item_;
			item_.format("Item%d",20,i);
			Item* t = (Item*)mkItems[i];
			itm->addAttr(item_)->setValue(t->mkString);
		}
	}
}

//----cXuiSlider----
cXuiSlider::cXuiSlider(nextar::cXuiContainer *pkParent, nextar::iXMLElement *e) : iXuiWnd(pkParent,xGuiSlider),
miValue(0),
miMax(100),miMin(0)
{
	RefObjectInit(cXuiSlider);
	read(e);
}

void cXuiSlider::read(nextar::iXMLElement *e)
{
	iXuiWnd::read(e);
	iXMLAttribute*	tmp;
#define XRDATT(name,val) tmp = e->find(name); if(tmp) val = tmp->getAsSignedInt();
	XRDATT("Value",miValue);
	XRDATT("Max",miMax);
	XRDATT("Min",miMin);
#undef XRDATT
}

void cXuiSlider::write(nextar::iXMLElement *e)
{
	iXuiWnd::write(e);
	e->setValue("xGuiSlider");
#define XWRATT(name,val) e->addAttr(name)->setAsSignedInt(val);
	XWRATT("Value",miValue);
	XWRATT("Max",miMax);
	XWRATT("Min",miMin);
#undef XWRATT
}

// -- cXuiProgressBar --
cXuiProgressBar::cXuiProgressBar(nextar::cXuiContainer *pkParent, nextar::iXMLElement *e) : iXuiWnd(pkParent,xGuiProgressBar)
{
	RefObjectInit(cXuiProgressBar);
	read(e);
}

void cXuiProgressBar::write(iXMLElement* e)
{
	iXuiWnd::write(e);
	e->setValue("xGuiProgressBar");
}

// -- cXuiTabControl --
cXuiTabControl::cXuiTabControl(nextar::cXuiContainer *pkParent, nextar::iXMLElement *e) : cXuiContainer(pkParent,cXuiContainer::BorderFrame),
miFocussed(-1),miStartRender(0),miActive(-1),miEndRender(0)
{
	RefObjectInit(cXuiTabControl);
	meType = xGuiTabControl;
	read(e);
}

void cXuiTabControl::read(nextar::iXMLElement *e)
{
	cXuiContainer::read(e);
	e->addAttr("ActiveTab")->setAsSignedInt(miActive);
	iXuiWnd* child = mpkFirstChild;
	while(child)
	{
		_addChild(child);
		child = child->next();
	}
}

void cXuiTabControl::write(nextar::iXMLElement *e)
{
	cXuiContainer::write(e);
	e->setValue("xGuiTabControl");
	iXMLAttribute* att = e->find("ActiveTab");
	if( att )
		miActive = att->getAsSignedInt();
}

// -- cXuiEditBox --
cXuiEditBox::cXuiEditBox(nextar::cXuiContainer *pkParent, nextar::iXMLElement *e) : iXuiWnd(pkParent,xGuiEditBox),
mpkFont(0),muMaxChar(-1)
{
	RefObjectInit(cXuiEditBox);
	read(e);
}

void cXuiEditBox::read(iXMLElement* e)
{
	iXuiWnd::read(e);
	iXMLAttribute* tmp;
	tmp = e->find("MaxChar");
	if(tmp) muMaxChar = tmp->getAsUnsignedInt();

	tmp = e->find("Font");
	u32 font,color(CV_EditFontColor),caret(CV_CaretColor);
	if(tmp)
	{
		font = tmp->getAsHexValue();
		tmp = e->find("FontColor");
		if(tmp)
			color = tmp->getAsHexValue();
		tmp = e->find("CaretColor");
		if(tmp)
			caret = tmp->getAsHexValue();
		setFont(font,(s32)color,(s32)caret);
	}
}

void cXuiEditBox::write(iXMLElement *e)
{
	iXuiWnd::write(e);
	e->setValue("xGuiStaticText");
	e->addAttr("MaxChar")->setAsUnsignedInt(muMaxChar);
	if(mpkFont)	e->addAttr("Font")->setAsHexValue(mpkFont->getKey());
	e->addAttr("FontColor")->setAsHexValue((u32)miColor);
	e->addAttr("CaretColor")->setAsHexValue((u32)miCaretColor);
}

#undef ReadFlag
#undef ReadStyle
#undef WriteFlag
#undef WriteStyle
