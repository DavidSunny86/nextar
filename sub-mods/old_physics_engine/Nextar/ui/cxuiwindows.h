//@ Abhishek Dey
//@ Sep 23 07
//@ Oct 10 07
//@ Oct 11 07
//@ Oct 12 07

#ifndef XUIWINDOWS_H
#define XUIWINDOWS_H

#include <iobject.h>
#include <xtypesex.h>
#include <array_t.h>
#include <graphics/itexture.h>
#include <ui/xevents.h>
#include <xml/ixmldocument.h>

namespace nextar
{

#define AddFlagGSMethod(flag) \
	_n_inline bool get##flag() { return flag & muFlags; }\
	_n_inline void set##flag(bool b) { if(b) muFlags |= flag; else muFlags &=~flag; }
#define AddStyleGSMethod(flag) \
	_n_inline bool get##flag() { return flag & muStyle; }\
	_n_inline void set##flag(bool b) { if(b) muStyle |= flag; else muStyle &=~flag; }


class iXuiWnd;
class cXuiContainer;
class cXuiButton;
class cXuiCheckbox;
class cXuiRadioButton;
class cXuiStaticText;
class cXuiVScrollBar;
class cXuiRadioGroup;
class cXuiListBox;
class cXuiComboBox;
class cXuiSlider;
class cXuiProgressBar;
class cXuiTabControl;

// constants
#define N3DScrollbarArrowClickDelay 330 // .. millisecs
#define N3DScrollbarArrowClickRepeat 50 // .. millisecs
#define N3DCaretBlinkRate			100

class X_Api cXuiRadioGroup : public cBaseClass
{
	array_t				mkRadioButtons;
	cXuiRadioButton*	mpkActive;
	u32					muId;
public:
	_n_inline	u32 getId() { return muId; }
	_n_inline cXuiRadioButton* get() { return mpkActive; }
	void add(cXuiRadioButton*);
	void rem(cXuiRadioButton*);
	void set(cXuiRadioButton*);
	
	cXuiRadioGroup(u32 Id);
};

// base class for all windows
class X_Api iXuiWnd : public iObject
{
public:
	// gui events
	enum GuiEvent { 
		OnFocusIn = 1,
		OnFocusOut,
		OnMouseEnter,
		OnMouseLeave,
		OnButtonClick,
		OnScroll,
		OnAddItemLbox,
		OnRemItemLbox,
		OnLBSelection,
		OnAddItemCbox,
		OnRemItemCbox,
		OnCBSelection,
		OnSliderChange,
		OnNewTabActivated,
	};
	// flags
	enum {
		Visible = 1, 
		HasFocus = 2,
		CanHaveFocus = 4,
		Default = 8,
		Disabled = 16,
		IsContainer = 64,
		MouseOver = 128,
		LastFlag = 256
	};
	enum Type { 
		xGuiManager,
		xGuiDialog,
		xGuiButton,
		xGuiCheckbox,
		xGuiRadioButton,
		xGuiStaticText,
		xGuiVScrollBar,
		xGuiListBox,
		xGuiComboBox,
		xGuiSlider,
		xGuiProgressBar,
		xGuiTabControl,
		xGuiEditBox,
		xGuiUnknown
	};

public:

	iXuiWnd( cXuiContainer*, Type eType, u32 uInitialFlags = Visible|CanHaveFocus );
	virtual ~iXuiWnd();

	AddFlagGSMethod(Visible);
	AddFlagGSMethod(HasFocus);
	AddFlagGSMethod(CanHaveFocus);
	AddFlagGSMethod(Default);
	AddFlagGSMethod(Disabled);
	AddFlagGSMethod(IsContainer);

	_n_inline void				setCaption(const tchar* cap) { mkCaption = cap; }
	_n_inline void				setCaption(const cStr& cap) { mkCaption = cap; }
	_n_inline const cStr&			getCaption() const { return mkCaption; }
	_n_inline const rect2_x&		getAbsRect() const { return mkAbsRect; }
	_n_inline bool				isPointInside(i2d x, i2d y) { return mkAbsRect.isInside(x,y); }
	_n_inline void				setHotKey(u32 uKey) { muHotKey = uKey; }
	_n_inline u32					getHotKey() { return muHotKey; }
	// internal inlines
	_n_inline iXuiWnd*			next() { return mpkNext; }
	_n_inline iXuiWnd*			prev() { return mpkPrev; }
	_n_inline void				next(iXuiWnd* n) { mpkNext = n; }
	_n_inline void				prev(iXuiWnd* n) { mpkPrev = n; }
	_n_inline cXuiContainer*		getParent() const { return mpkParent; }
	_n_inline Type				getType() const { return meType; }

	// call this after resize or repositioning
	virtual void				update();

	// absoulte position and dimensions with adjustments
	_n_inline void				setPosX(f32 x)		{ mkPos.x = x; }
	_n_inline void				setPosY(f32 x)		{ mkPos.y = x; }
	_n_inline void				setPos(f32 x,f32 y)		{ mkPos.x = x; mkPos.y = y; }
	_n_inline void				setDimX(f32 x) { mkDim.x = x; }
	_n_inline void				setDimY(f32 y) { mkDim.y = y; }
	_n_inline void				setDim(f32 dx,f32 dy)	{ mkDim.x = dx; mkDim.y = dy; }

protected:

	// the key of iObject is used as id
	cStr		mkCaption;
	rect2_x		mkAbsRect;
	// relative position and dimension
	vec2_t		mkPos;
	vec2_t		mkDim;
	u32			muFlags;
	u32			muHotKey;
	Type		meType;
	// parent ptr
	cXuiContainer*	mpkParent;
	// internal
	iXuiWnd		*mpkNext,*mpkPrev;

public:
	
	void preRendering();
	void bringToFront();
	void setParent(cXuiContainer*);//@ set parent 
	// parse
	virtual void read(iXMLElement*);
	virtual void write(iXMLElement*);
	// events are generated by cXuiManipulator and propagated forward
	virtual void onKeyEvent(const cKeyEvent*) {  }
	virtual void onMouseEvent(const cMouseEvent*) {  }
	virtual void onGuiEvent(GuiEvent,iXuiWnd* pkCaller) {}

	virtual void onDraw() = 0;

	friend class cXuiContainer;
};

// frame implementation
class X_Api cXuiContainer : public iXuiWnd
{
	friend class iXuiWnd;
public:
	class iEventCallback {
	public:
		virtual void onEvent(GuiEvent,iXuiWnd* pkCaller) = 0;
	};
	enum { 
		CloseButtonPressed = iXuiWnd::LastFlag, 
		CloseButtonMouseOver = iXuiWnd::LastFlag << 1,
		ContainerLastFlag = iXuiWnd::LastFlag << 2
	};
	enum Style { 
		BorderFrame = 1, 
		TitleBar = 2, 
		CloseBox = 4, 
		BackgroundImage = 8,
		BasicWindow = BorderFrame|TitleBar|CloseBox,
	};

protected:
	
	struct Background {
		iTexture*			mpkTexture;
		float				mkSrc[4];
		rect2_x				mkClient;
		s32					miColor;
	};

	iXuiWnd*			mpkFirstChild;
	iXuiWnd*			mpkLastChild;

	u32					muStyle;
	array_t				mkRadioGroups;
	rect2_x				mkCloseBox;
	Background*			mpkBack;
	iEventCallback*		mpkCallback;

public:

	AddStyleGSMethod(BorderFrame);
	AddStyleGSMethod(TitleBar);
	AddStyleGSMethod(CloseBox);

	//@ set callback
	_n_inline void setCallback(iEventCallback* pkC) { mpkCallback=pkC; }
	//@ background
	void setBackground(iTexture*,const float* src=0,s32 uColor=0xffffffff);
	//@ register
	void onRenderChildren();
	// read/write
	virtual void read(iXMLElement*);
	virtual void write(iXMLElement*);
	void parseChild(iXMLElement*);
	//@ add/rem children
	virtual void		addChild(iXuiWnd*);
	virtual void		removeChild(iXuiWnd*);
	iXuiWnd*			getWndAt(i2d x,i2d y);
	cXuiRadioGroup*		getRadioGroup(u32 uId);	// creates a new group if no matching group found
	//@ rect updates
	virtual void update();
	//@ events are generated by cXuiManipulator and propagated forward
	virtual void onGuiEvent(GuiEvent,iXuiWnd* pkCaller);
	virtual void onMouseEvent(const cMouseEvent*);
	//@ draw
	virtual void onDraw();
	cXuiContainer(cXuiContainer* pkParent,iXMLElement*);
	cXuiContainer(cXuiContainer* pkParent,u32 uStyle);
	virtual ~cXuiContainer();
};

class X_Api cXuiButton : public iXuiWnd
{
	// more states
public:
	enum { Pressed = iXuiWnd::LastFlag, ButtonLastFlag = iXuiWnd::LastFlag << 1 };
protected:
public:

	// read/write
	virtual void write(iXMLElement*);

	//@ events are generated by cXuiManipulator and propagated forward
	virtual void onKeyEvent(const cKeyEvent*);
	virtual void onMouseEvent(const cMouseEvent*);
	virtual void onGuiEvent(GuiEvent,iXuiWnd* pkCaller);
	//@ draw
	virtual void onDraw();
	// init
	cXuiButton(cXuiContainer* pkParent,iXMLElement*);
	cXuiButton(cXuiContainer* pkParent,u32 uId);
};

class X_Api cXuiCheckbox : public cXuiButton
{
public:
	enum { Checked = cXuiButton::ButtonLastFlag };
public:

	AddFlagGSMethod(Checked);

	_n_inline void check()  { muFlags |= Checked; }
	_n_inline void uncheck()  { muFlags &=~Checked; }
	_n_inline void toggle() { if(muFlags & Checked) muFlags&=~Checked; else muFlags|=Checked; }
	_n_inline bool isChecked() { return muFlags & Checked; }
public:

	//@ read write
	virtual void read(iXMLElement*);
	virtual void write(iXMLElement*);

	virtual void onKeyEvent(const cKeyEvent*);
	virtual void onMouseEvent(const cMouseEvent*);
	//@ draw
	virtual void onDraw();
	cXuiCheckbox(cXuiContainer* pkParent,iXMLElement*);
	cXuiCheckbox(cXuiContainer* pkParent,u32 uId);
};

class X_Api cXuiRadioButton : public cXuiCheckbox
{
protected:
	cXuiRadioGroup*		mpkGroupPtr;
public:
	enum { InvalidRBGroup = -1 };
public:

	//@ r/w
	virtual void read(iXMLElement*);
	virtual void write(iXMLElement*);

	virtual void onKeyEvent(const cKeyEvent*);
	virtual void onMouseEvent(const cMouseEvent*);
	//@ draw
	virtual void onDraw();
	cXuiRadioButton(cXuiContainer* pkParent,iXMLElement*);
	cXuiRadioButton(cXuiContainer* pkParent,u32 uId,u32 uGroupId);
};

class X_Api cXuiStaticText : public iXuiWnd
{	
	iXuiFont*	mpkFont;
	s32			miColor;
	u32			muDrawFlags;
	enum { Bordered = iXuiWnd::LastFlag << 0 };

public:

	AddFlagGSMethod(Bordered);

	_n_inline void setTextColor(u32 color) { miColor = color; }
	_n_inline void setDrawFlags(u32 f) { muDrawFlags = f; }
	//@ r/w
	virtual void read(iXMLElement*);
	virtual void write(iXMLElement*);

	void setFont(u32 uKey,u32 uColor);
	virtual void update();
	virtual void onDraw();
	cXuiStaticText(cXuiContainer* pkParent,iXMLElement*);
	cXuiStaticText(cXuiContainer* pkParent,const tchar* sCap,bool bDrawBorder = true);
	~cXuiStaticText();
};

class X_Api cXuiVScrollBar : public iXuiWnd
{

protected:
	s32			miPos;
	s32			miPageSize;
	s32			miStart;
	s32			miEnd;
	u32			muArrowTS;
	rect2_x		mkUp;
	rect2_x		mkDown;
	rect2_x		mkTrack;
	rect2_x		mkThumb;

public:
	// more states
	enum { 
		ClickedUp   = iXuiWnd::LastFlag << 0,
		ClickedDown = iXuiWnd::LastFlag << 1,
		HeldUp		= iXuiWnd::LastFlag << 2,
		HeldDown	= iXuiWnd::LastFlag << 3,
		ShowThumb	= iXuiWnd::LastFlag << 4,
		ThumbPressed= iXuiWnd::LastFlag << 5
	};

public:
	
	void setTrackRange(s32 iStart,s32 iEnd);
	_n_inline s32 getTrackPos() { return miPos; }
	_n_inline void setTrackPos(s32 p) { miPos = p; _cap(); _updateThumbRect(); }
	_n_inline s32 getPageSize() { return miPageSize; }
	_n_inline void setPageSize(s32 p) { miPageSize = p; _cap(); _updateThumbRect(); }

	void scroll(s32 delta);
	void showItem(s32 idx);

	//@ r/w
	virtual void read(iXMLElement*);
	virtual void write(iXMLElement*);

	virtual void update();
	virtual void onDraw();
	virtual void onGuiEvent(GuiEvent,iXuiWnd* pkCaller);
	virtual void onMouseEvent(const cMouseEvent*);
	cXuiVScrollBar(cXuiContainer* pkParent,iXMLElement*);
	cXuiVScrollBar(cXuiContainer* pkParent);
private:
	void _cap();
	void _updateThumbRect();
};

class X_Api cXuiListBox : public iXuiWnd
{
public:
	enum { 
		MultiSelection  = iXuiWnd::LastFlag << 0,
		DragLB			= iXuiWnd::LastFlag << 1,
		MouseOverScroll = iXuiWnd::LastFlag << 2,
	};

	struct Item
	{
		cStr	mkString;
		void*	mpvData;
		bool	mbSelected;
		Item();
	};
	
	s32				miSelected;
	s32				miSelStart;
	rect2_x			mkSelection;
	rect2_x			mkText;
	array_t			mkItems;
	cXuiVScrollBar	mkScroll;
	
protected:
public:

	AddFlagGSMethod(MultiSelection);
	_n_inline s32		getItemCount() { return (s32)mkItems.size(); }
	_n_inline Item*	getItem(s32 i) { return (Item*)mkItems.at((array_t::index_type)i); }

	cXuiListBox(cXuiContainer* pkParent,iXMLElement*);
	cXuiListBox(cXuiContainer* pkParent,u32 uId);
	virtual ~cXuiListBox();
public:

	virtual void update();
	virtual void onDraw();
	virtual void onGuiEvent(GuiEvent,iXuiWnd* pkCaller);
	virtual void onMouseEvent(const cMouseEvent*);
	virtual void onKeyEvent(const cKeyEvent*);

	//@ r/w
	virtual void read(iXMLElement*);
	virtual void write(iXMLElement*);

	void addItem(const tchar*,void *pvData = 0);
	void insertItem(s32 idx,const tchar*,void *pvData = 0);
	
	void remItem(s32 idx);
	void remItemByText(const tchar*);
	void remItemByData(void *pvData);
	void remAllItems();
	
	Item*	findItem(const tchar*,bool addifnotfound = true);
	s32		getSelectionIdx(s32 iPrev=-1);
	Item*	getSelection(s32 iPrev=-1) { return getItem(getSelectionIdx(iPrev)); }
	void	selItem(s32);

};

class X_Api cXuiComboBox : public iXuiWnd
{
public:
	enum {
		CBOpened  = iXuiWnd::LastFlag << 0,
		TextAreaPressed = iXuiWnd::LastFlag << 1,
		MouseOverScroll = iXuiWnd::LastFlag << 2
	};

	struct Item
	{
		cStr	mkString;
		void*	mpvData;
		Item();
	};

private:

	array_t			mkItems;
	cXuiVScrollBar	mkScroll;
	s32				miDropHeight;
	s32				miSelected;
	s32				miFocussed;
	static i2d		miTextHeight;
	rect2_x			mkButton;
	rect2_x			mkText;
	rect2_x			mkDrop;
	rect2_x			mkDropText;

public:

	cXuiComboBox(cXuiContainer* pkParent,iXMLElement*);
	cXuiComboBox(cXuiContainer* pkParent,u32 uId);
	~cXuiComboBox();

	_n_inline s32		getItemCount() { return (s32)mkItems.size(); }
	_n_inline Item*	getItem(s32 i) { return (Item*)mkItems.at((array_t::index_type)i); }
	_n_inline s32		getSelectionIdx() { return miSelected; }
	_n_inline Item*	getSelection() { return getItem(getSelectionIdx()); }

		//@ r/w
	virtual void read(iXMLElement*);
	virtual void write(iXMLElement*);

	virtual void update();
	virtual void onDraw();
	virtual void onGuiEvent(GuiEvent,iXuiWnd* pkCaller);
	virtual void onMouseEvent(const cMouseEvent*);
	virtual void onKeyEvent(const cKeyEvent*);

	void addItem(const tchar*,void *pvData = 0);
	void insertItem(s32 idx,const tchar*,void *pvData = 0);
	void remItem(s32 idx);
	void remItemByText(const tchar*);
	void remItemByData(void *pvData);
	void remAllItems();
	

	Item*	findItem(const tchar*,bool addifnotfound = true);
	void	selItem(s32);

private:

};

class X_Api cXuiSlider : public iXuiWnd
{
private:

	enum {
		ButtonPressed  = iXuiWnd::LastFlag << 0,
	};

	static i2d	miButtonWidth;
	s32		miValue;
	s32		miMax;
	s32		miMin;
	rect2_x	mkButton;
	rect2_x	mkTrack;

public:

	cXuiSlider(cXuiContainer* pkParent,iXMLElement*);
	cXuiSlider(cXuiContainer* pkParent,u32 uId);

	_n_inline s32		getValue() { return miValue; }

	//@ r/w
	virtual void read(iXMLElement*);
	virtual void write(iXMLElement*);

	virtual void update();
	virtual void onDraw();
	virtual void onGuiEvent(GuiEvent,iXuiWnd* pkCaller);
	virtual void onMouseEvent(const cMouseEvent*);
//	virtual void onKeyEvent(const cKeyEvent*);

	void	setRange(s32 iMin,s32 iMax);
	void	setValue(s32 v);
	
private:
	void	_updateThumb();
	void	_valueFromPos(i2d x);
};

class X_Api cXuiProgressBar : public iXuiWnd
{
private:
	f32		mfPercentageComplete;
	rect2_x	mkProgress;
	i2d		miProgressWidth;
public:
	
	cXuiProgressBar(cXuiContainer* pkParent,iXMLElement*);
	cXuiProgressBar(cXuiContainer*, u32 uId);

	//@ r/w
	virtual void write(iXMLElement*);

	_n_inline f32		getComplete() { return mfPercentageComplete; }
	_n_inline void	setComplete(f32 v) { mfPercentageComplete = v;	} // between 0 and 1
	_n_inline void	increment(f32 amt) { mfPercentageComplete += amt; }
	_n_inline void	start() { mfPercentageComplete = 0; }
	_n_inline void	stop() { mfPercentageComplete = 1; }
	virtual void	update();
	virtual void	onDraw();
};

class X_Api cXuiTabControl : public cXuiContainer
{
private:
	enum { FocusPressed = ContainerLastFlag << 0, NotAllTabsVisible = ContainerLastFlag << 1 };
	struct Tab
	{
		iXuiWnd*	mpkWindow;
		i2d			miHitWidth;
		Tab();
	};
	array_t			mkTabs;
	s32				miActive;
	s32				miFocussed;
	s32				miStartRender;
	s32				miEndRender;
	rect2_x			mkTabRect;
	rect2_x			mkTabClient;
public:

	cXuiTabControl(cXuiContainer* pkParent,iXMLElement*);
	cXuiTabControl(cXuiContainer*, u32 uId,u32 uStyle);

	//@ r/w
	virtual void read(iXMLElement*);
	virtual void write(iXMLElement*);

	//@ add/rem children
	virtual void		addChild(iXuiWnd*);
	virtual void		removeChild(iXuiWnd*);
	
	virtual void update();
	virtual void onGuiEvent(GuiEvent e,iXuiWnd* pkC);
	virtual void onMouseEvent(const cMouseEvent*);
	virtual void onDraw();

private:

	void	_addChild(iXuiWnd*);
	void	_scrollLeft(); // end constant
	void	_scrollRight(); // start constant
	void	_focusOn(i2d mx);
};

// all important and hectic class
class X_Api cXuiEditBox : public iXuiWnd
{
private:
	rect2_x		mkTextRect;
	//	caret pos indicator:
	//	indicates the position of the character over which the caret was placed
	//	if the caption is empty the caret pos is 0
	s32			miTextStart;
	s32			miTextEnd;
	s32			miCaretPos;
	s32			miMarkerStart;
	s32			miMarkerEnd;
	i2d			miDragXStart;
	i2d			miDragXEnd;
	rect2_x		mkMarker;
	i2d			miCaretX;
	u32			muMaxChar;
	iXuiFont*	mpkFont;
	s32			miColor;
	s32			miCaretColor;

public:

	cXuiEditBox(cXuiContainer* pkParent,iXMLElement*);
	cXuiEditBox(cXuiContainer*, u32 uId);
	~cXuiEditBox();
	_n_inline void setMaxChar(u32 maxc) { muMaxChar = maxc; }
	_n_inline void setTextColor(u32 color) { miColor = color; }
	_n_inline void setCaretColor(u32 color) { miCaretColor = color; }

	//@ r/w
	virtual void read(iXMLElement*);
	virtual void write(iXMLElement*);
	
	void setFont(u32 key,u32 color,u32 caret_colr);

	virtual void update();
//	virtual void onGuiEvent(GuiEvent e,iXuiWnd* pkC);
	virtual void onMouseEvent(const cMouseEvent*);
	virtual void onKeyEvent(const cKeyEvent*);
	virtual void onDraw();

private:

	void	_scrollLeft();
	void	_scrollRight();
	void	_calCaretPos();
	void	_calMarkerRect();
	s32		_getCharacter(i2d mouse_x);
};

#undef AddFlagGSMethod
}

#endif //XUIWINDOWS_H