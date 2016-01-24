//@ Abhishek Dey
//@ Sep 23 07
//@ Nov 23 07
#include <xtypesex.h>
#include <ctimer.h>
#include <graphics/irenderer.h>
#include <win32/directx/cdirectx9font.h>
#include <win32/directx/cdirectx9renderer.h>
#include <win32/directx/cdirectx9texture.h>
#include <win32/directx/cdirectx9uiskin.h>
#include <loaders/cfontfactory.h>
#include <io/ifilesystem.h>
#include <xml/ixmldocument.h>
#include <isystem.h>
//#include <graphics/xvertex.h>
#include <loaders/itexturefactory.h>

#include <ui/cxuimanager.h>
#include <ui/xuicommonvars.h>
#include <ui/ixuiskin.h>


using namespace nextar;
// check on the example default-skin.xml to see the 
// organisation of the xml

namespace nextar
{
extern cDxVertex2D v[];
}
#define v_x(a) ((f32)((a)))
#define v_y(a) ((f32)((a)))
// ui skin
/*
class cDirectX9UISkin : public iXuiSkin
{
	enum CommonVar {
		eCVWinXOffsetX,
		eCVWinXOffsetY,
		eCVXButtonWidth,
		eCVXButtonHeight,
		eCVWinEdgeLeft,
		eCVWinEdgeTop,
		eCVWinEdgeRight,
		eCVWinEdgeBottom,
		eCVScrollButtonWidth,
		eCVScrollButtonHeight,
		eCVLBEdgeLeft,
		eCVLBEdgeTop,
		eCVLBEdgeRight,
		eCVLBEdgeBottom,
		eCVLBMargin,
		eCVLBBorder,
		eCVDropDownButtonWidth,
		eCVDropDownButtonHeight,
		eCVCBDropDownBorer,
		eCVSliderButtonWidth,
		eCVSliderButtonHeight,
		eCVSliderTrackHeight,
		eCVProgressBarHeight,
		eCVProgressBarBorderWidth,
		eCVProgressBarBorderHeight,
		eCVTabHeight,
		eCVTabBorder,
		eCVTabEdgeLeft,
		eCVTabEdgeTop,
		eCVTabEdgeRight,
		eCVTabEdgeBottom,
		eCVEditBoxHeight,
		eCVEditBoxBorderWidth,
		eCVEditBoxBorderHeight,

		// internal flags
		eCVCBTextButtonBorder,
		eCVNormalButtonBorder,
		eCVCheckboxWidth,
		eCVCheckboxHeight,
		eCVRadioButtonWidth,
		eCVRadioButtonHeight,
		eCVSliderTrackBorder,
		// end of internal flags

		eCVStaticTextHeight,

		eCVCount
	};

	// uvs
	//{ u[4],v[4] };	
	f32		win_uv[8];	// diagonal
	f32		lbox_uv[8];
	f32		tabwnd_uv[8];
	//{ u[4],v[2] }
	f32		button_uv[18]; //normal/mouseover/pressed
	f32		combobox_uv[18];
	f32		tab_uv[24];
	//{ u[2],v[2] }
	f32		combobutton_uv[3][4];
	f32		wndclose_uv[3][4]; //normal/mouseover/pressed
	f32		checkbox_uv[3][4];	//unchecked/mouseover/checked
	f32		radio_uv[3][4];
	f32		scrollup_uv[3][4];	//normal/mouseover/pressed
	f32		scrolldown_uv[3][4];
	f32		thumb_uv[3][4];
	f32		sliderbutton_uv[3][4];
	//{ u[2],v[2] }
	f32		scroll_uv[4];
	// selection 
	f32		lbselhighlight_uv[4];
	f32		cbselhighlight_uv[4];
	f32		progressbar_uv[4];
	f32		editsel_uv[4];
	f32		caret_uv[4];
	// combo drop down
	f32		comboddown_uv[6];
	f32		pbarborder_uv[6];
	f32		editborder_uv[6];
	f32		slider_uv[6];
	

	// cursor
	float		cursor_uvs[CT_Count*4];
	size2_x		cursor_size[CT_Count];

	// other details
	cDirectX9Texture*	mpkSkin;
	cDirectX9Font*		edit_font;
	cDirectX9Font*		static_font;
	i2d					mkVariables[eCVCount];
	s32					col_gui;
	s32					col_background;
	s32					col_statictext;
	s32					col_edittext;
	s32					col_caret;

public:
		
	cDirectX9UISkin(const tchar* config);
	cDirectX9UISkin(iFileReader* reader);
	~cDirectX9UISkin();

	void _readCursors(iXMLElement*);
	void _readWindowPackage(iXMLElement*,f32* store);
	void _ReadButtonRect(iXMLElement*,f32* store);
	void _ReadExButtonRect(iXMLElement*,f32* store,s32 iStateCount=3);
	void _ReadRect(iXMLElement*,f32* store,size2_x* s=0);
	void _readAllRects(iXMLElement*);
	void _readAllVars(iXMLElement*);
	void _ReadRect(iXMLElement*,rect2_x&);
	void _readPathAndKey(iXMLElement*,cStr&,u32&);
	void _readAndLoadSettings(iXMLElement*);
	bool _load(iXMLDocument* config);

	// draw 
	void _drawRect(const rect2_x& r,bool bTitlebar,bool bClient,const tchar* sCaption,f32* tex,s32*);
	void _drawRect2(const rect2_x &r,i2d border,f32 *set,const tchar* sCaption);

public:

	// overrides
	virtual void		initSkinParameters();
	virtual iXuiFont*	getStaticFont() { return static_font; }
	virtual iXuiFont*	getEditFont() { return edit_font; }
	virtual size2_x&		getCursorSize(ECursorType t) { X_BreakOutIf(t<0 || t>CT_Count);  return cursor_size[t]; }
	// draw 
	virtual void		drawControl(const rect2_x& r,CommonID); // for controls with only normal state
	virtual void		drawWindow(const rect2_x& r,bool bTitlebar,bool bClient,const tchar* sCaption);
	virtual void		drawTabWindow(const rect2_x& r);
	virtual void		drawListBox(const rect2_x& r,const tchar* sCaption);
	virtual void		drawControlState(const rect2_x& r,CommonID uId,EControlState,const tchar* sCaption);
	virtual void		drawCursor( ECursorType, const coord2_x& );
	virtual void		drawStaticText(const tchar* sText,const rect2_x&,u32 Style);
	virtual void		drawComboDropDownArea(const rect2_x &r);
	virtual void		drawSliderTrack(const rect2_x& r);
	virtual void		drawProgressBarBorder(const rect2_x& r,const tchar* sCaption);
	virtual void		drawEditControlBorder(const rect2_x& r);
};
*/
cDirectX9UISkin::cDirectX9UISkin(const tchar* config) : 
mpkSkin(0), edit_font(0), static_font(0)
{
	iXMLDocument* doc = iXMLDocument::create(config);
	if(doc && doc->load()==X_SUCCESS)
	{
		_load(doc);
	}
}

cDirectX9UISkin::cDirectX9UISkin(iFileReader* reader) : 
mpkSkin(0), edit_font(0), static_font(0)
{
	iXMLDocument* doc = iXMLDocument::create("");
	if(doc && doc->load(reader)==X_SUCCESS)
	{
		_load(doc);
	}
}

cDirectX9UISkin::cDirectX9UISkin(iXMLDocument* reader) : 
mpkSkin(0), edit_font(0), static_font(0)
{
	_load(doc);
}

cDirectX9UISkin::~cDirectX9UISkin()
{
	if(mpkSkin) mpkSkin->release();
	if(edit_font) edit_font->release();
	if(static_font) static_font->release();
}

// init params
void cDirectX9UISkin::initSkinParameters()
{
	CV_WinXOffsetX				=	mkVariables[eCVWinXOffsetX];
	CV_WinXOffsetY				=	mkVariables[eCVWinXOffsetY];
	CV_XButtonWidth				=	mkVariables[eCVXButtonWidth];
	CV_XButtonHeight			=	mkVariables[eCVXButtonHeight];
	CV_WinEdgeLeft				=	mkVariables[eCVWinEdgeLeft];
	CV_WinEdgeTop				=	mkVariables[eCVWinEdgeTop];
	CV_WinEdgeRight				=	mkVariables[eCVWinEdgeRight];
	CV_WinEdgeBottom			=	mkVariables[eCVWinEdgeBottom];
	CV_ScrollButtonWidth		=	mkVariables[eCVScrollButtonWidth];
	CV_ScrollButtonHeight		=	mkVariables[eCVScrollButtonHeight];
	CV_LBEdgeLeft				=	mkVariables[eCVLBEdgeLeft];
	CV_LBEdgeTop				=	mkVariables[eCVLBEdgeTop];
	CV_LBEdgeRight				=	mkVariables[eCVLBEdgeRight];
	CV_LBEdgeBottom				=	mkVariables[eCVLBEdgeBottom];
	CV_LBMargin					=	mkVariables[eCVLBMargin];
	CV_LBBorder					=	mkVariables[eCVLBBorder];
	CV_DropDownButtonWidth		=	mkVariables[eCVDropDownButtonWidth];
	CV_DropDownButtonHeight		=	mkVariables[eCVDropDownButtonHeight];
	CV_CBDropDownBorer			=	mkVariables[eCVCBDropDownBorer];
	CV_SliderButtonWidth		=	mkVariables[eCVSliderButtonWidth];
	CV_SliderButtonHeight		=	mkVariables[eCVSliderButtonHeight];
	CV_SliderTrackHeight		=	mkVariables[eCVSliderTrackHeight];
	CV_ProgressBarHeight		=	mkVariables[eCVProgressBarHeight];
	CV_ProgressBarBorderWidth	=	mkVariables[eCVProgressBarBorderWidth];
	CV_ProgressBarBorderHeight	=	mkVariables[eCVProgressBarBorderHeight];
	CV_TabHeight				=	mkVariables[eCVTabHeight];
	CV_TabBorder				=	mkVariables[eCVTabBorder];
	CV_TabEdgeLeft				=	mkVariables[eCVTabEdgeLeft];
	CV_TabEdgeTop				=	mkVariables[eCVTabEdgeTop];
	CV_TabEdgeRight				=	mkVariables[eCVTabEdgeRight];
	CV_TabEdgeBottom			=	mkVariables[eCVTabEdgeBottom];
	CV_EditBoxHeight			=	mkVariables[eCVEditBoxHeight];
	CV_EditBoxBorderWidth		=	mkVariables[eCVEditBoxBorderWidth];
	CV_EditBoxBorderHeight		=	mkVariables[eCVEditBoxBorderHeight];
	CV_StaticTextHeight			=	mkVariables[eCVStaticTextHeight];
	// colors
	CV_StaticFontColor			=	col_statictext;
	CV_EditFontColor			=	col_edittext;
	CV_CaretColor				=	col_caret;
}

// read var
// read rect
void cDirectX9UISkin::_ReadRect(iXMLElement *e, rect2_x &store)
{
	X_Assert(e);

	iXMLAttribute* left = e->find("Left");
	iXMLAttribute* top = e->find("Top");
	iXMLAttribute* right = e->find("Right");
	iXMLAttribute* bottom = e->find("Bottom");

	X_Assert(left && top && right && bottom);

	store.left = (i2d)left->getAsSignedInt();
	store.top = (i2d)top->getAsSignedInt();
	store.right = (i2d)right->getAsSignedInt();
	store.bottom = (i2d)bottom->getAsSignedInt();
}

// path and key
void cDirectX9UISkin::_readPathAndKey(iXMLElement *e, cStr &rkPath,u32 &ruKey)
{
	X_Assert(e);

	iXMLAttribute* pkPath = e->find("Path");
	iXMLAttribute* pkKey  = e->find("KeyName");
	
	X_Assert(pkPath && pkKey);
	rkPath = pkPath->getValue();
	ruKey  = pkKey->getAsUnsignedInt();
}

// read cursors
void cDirectX9UISkin::_readCursors(iXMLElement *e)
{
	X_Assert(e);

	const tchar* sNames[] = {
		X_Text("CT_Normal"),
		X_Text("CT_CrossHair"),
	};

	for(s32 i=0;i<CT_Count;i++)
		_ReadRect((iXMLElement*)e->findChild(sNames[i]),&cursor_uvs[i*4],&cursor_size[i]);
}

// read the window settings
void cDirectX9UISkin::_readWindowPackage(iXMLElement *e,f32 *win)
{
	X_Assert(e);
	size2_x d = mpkSkin->getDimension();
	f32 dx  = 1.f / (f32)d.dx;
	f32 dy  = 1.f / (f32)d.dy;

	rect2_x rc[3];
	const tchar* sNames[] = { 
		X_Text("EdgeTopLeft"),
		X_Text("Client"),
		X_Text("EdgeBottomRight")
	};
	for(s32 i=0;i<3;i++)
		_ReadRect((iXMLElement*)e->findChild(sNames[i]),rc[i]);
	win[0] = (f32)rc[0].left * dx;
	win[1] = (f32)rc[1].left * dx;
	win[2] = (f32)rc[2].left * dx;
	win[3] = (f32)rc[2].right * dx;
	win[4] = (f32)rc[0].top * dy;
	win[5] = (f32)rc[1].top * dy;
	win[6] = (f32)rc[2].top * dy;
	win[7] = (f32)rc[2].bottom * dy;
}

// 
void cDirectX9UISkin::_ReadButtonRect(iXMLElement *e,f32 *store)
{
	X_Assert(e);

	size2_x d = mpkSkin->getDimension();
	f32 dx  = 1.f / (f32)d.dx;
	f32 dy  = 1.f / (f32)d.dy;

	rect2_x rc[3];
	const tchar* sNames[] = { 
		X_Text("State0"),
		X_Text("State1"),
		X_Text("State2"),
	};
	for(s32 i=0;i<3;i++)
		_ReadRect((iXMLElement*)e->findChild(sNames[i]),rc[i]);

	for(s32 j=0; j<3; j++)
	{
		store[j*4+0] = (f32)rc[j].left  * dx;
		store[j*4+1] = (f32)rc[j].right * dx;
		store[j*4+2] = (f32)rc[j].top * dy;
		store[j*4+3] = (f32)rc[j].bottom * dy;
	}
}

//
void cDirectX9UISkin::_ReadExButtonRect(iXMLElement *e,f32 *store,s32 iStateCount)
{
	X_Assert(e);

	rect2_x rc[3];
	size2_x d = mpkSkin->getDimension();
	f32 dx  = 1.f / (f32)d.dx;
	f32 dy  = 1.f / (f32)d.dy;

	const tchar* sNames[] = { 
		X_Text("State0"),
		X_Text("State1"),
		X_Text("State2"),
		X_Text("State3"),
	};

	const tchar* sUv[] = { 
		X_Text("Left"),
		X_Text("Center"), 
		X_Text("Right")
	};

	for(s32 i=0; i<iStateCount; i++)
	{
		iXMLElement* e2 = (iXMLElement*)e->findChild(sNames[i]);
		X_Assert(e2);
		for(s32 j=0;j<3;j++)
			_ReadRect((iXMLElement*)e2->findChild(sUv[j]),rc[j]);

		store[i*6+0] = (f32)rc[0].left * dx;
		store[i*6+1] = (f32)rc[1].left * dx;
		store[i*6+2] = (f32)rc[2].left * dx;
		store[i*6+3] = (f32)rc[2].right * dx;
		store[i*6+4] = (f32)rc[0].top * dy;
		store[i*6+5] = (f32)rc[0].bottom * dy;
	}
}

//
void cDirectX9UISkin::_ReadRect(iXMLElement *e,f32 *store,size2_x* sz)
{
	size2_x d = mpkSkin->getDimension();
	f32 dx  = 1.f / (f32)d.dx;
	f32 dy  = 1.f / (f32)d.dy;
	rect2_x rc;

	_ReadRect(e,rc);
	store[0] = (f32)rc.left  * dx;
	store[1] = (f32)rc.right * dx;
	store[2] = (f32)rc.top * dy;
	store[3] = (f32)rc.bottom * dy;
	if(sz) *sz = rc.dimension();
}

void cDirectX9UISkin::_readAllRects(iXMLElement *e)
{
	X_Assert(e);
	X_Assert(mpkSkin);
	_readCursors(e);
	_readWindowPackage((iXMLElement*)e->findChild("DlgWindow"),win_uv);
	_readWindowPackage((iXMLElement*)e->findChild("ListBoxWindow"),lbox_uv);
	_readWindowPackage((iXMLElement*)e->findChild("TabControl"),tabwnd_uv);
	// read buttons
	_ReadButtonRect(cgs->GetConfigGroupSet("ComboDropDownButton")),(f32*)combobutton_uv);
	_ReadButtonRect(cgs->GetConfigGroupSet("WCloseButton")),(f32*)wndclose_uv);
	_ReadButtonRect(cgs->GetConfigGroupSet("RadioButton")),(f32*)radio_uv);
	_ReadButtonRect(cgs->GetConfigGroupSet("Checkbox")),(f32*)checkbox_uv);
	_ReadButtonRect(cgs->GetConfigGroupSet("ScrollButtonUp")),(f32*)scrollup_uv);
	_ReadButtonRect(cgs->GetConfigGroupSet("ScrollButtonDn")),(f32*)scrolldown_uv);
	_ReadButtonRect(cgs->GetConfigGroupSet("ScrollThumb")),(f32*)thumb_uv);
	_ReadButtonRect(cgs->GetConfigGroupSet("SliderButton")),(f32*)sliderbutton_uv);
	// buttons
	_ReadExButtonRect(cgs->GetConfigGroupSet("NormalButton")),(f32*)button_uv);
	_ReadExButtonRect(cgs->GetConfigGroupSet("ComboBoxArea")),(f32*)combobox_uv);
	_ReadExButtonRect(cgs->GetConfigGroupSet("TabArea")),(f32*)tab_uv,4);
	_ReadExButtonRect(cgs->GetConfigGroupSet("ComboDropDownArea")),(f32*)comboddown_uv,1);
	_ReadExButtonRect(cgs->GetConfigGroupSet("SliderTrack")),slider_uv,1);
	_ReadExButtonRect(cgs->GetConfigGroupSet("ProgressBarBorder")),pbarborder_uv,1);
	_ReadExButtonRect(cgs->GetConfigGroupSet("EditBoxBorder")),editborder_uv,1);
	// tracks
	_ReadRect(cgs->GetConfigGroupSet("ScrollTrack")),scroll_uv);
	// select Highlight
	_ReadRect(cgs->GetConfigGroupSet("SelectedLBItem")),lbselhighlight_uv);
	_ReadRect(cgs->GetConfigGroupSet("SelectedCBItem")),cbselhighlight_uv);
	_ReadRect(cgs->GetConfigGroupSet("ProgressBar")),progressbar_uv);
	_ReadRect(cgs->GetConfigGroupSet("SelectedEditText")),editsel_uv);
	_ReadRect(cgs->GetConfigGroupSet("Caret")),caret_uv);
}

void cDirectX9UISkin::_readAllVars(iXMLElement *e)
{
	X_Assert(e);
	const tchar* sVars[] = 
	{
		"CV_WinXOffsetX",
		"CV_WinXOffsetY",
		"CV_XButtonWidth",
		"CV_XButtonHeight",
		"CV_WinEdgeLeft",
		"CV_WinEdgeTop",
		"CV_WinEdgeRight",
		"CV_WinEdgeBottom",
		"CV_ScrollButtonWidth",
		"CV_ScrollButtonHeight",
		"CV_LBEdgeLeft",
		"CV_LBEdgeTop",
		"CV_LBEdgeRight",
		"CV_LBEdgeBottom",
		"CV_LBMargin",
		"CV_LBBorder",
		"CV_DropDownButtonWidth",
		"CV_DropDownButtonHeight",
		"CV_CBDropDownBorer",
		"CV_SliderButtonWidth",
		"CV_SliderButtonHeight",
		"CV_SliderTrackHeight",
		"CV_ProgressBarHeight",
		"CV_ProgressBarBorderWidth",
		"CV_ProgressBarBorderHeight",
		"CV_TabHeight",
		"CV_TabBorder",
		"CV_TabEdgeLeft",
		"CV_TabEdgeTop",
		"CV_TabEdgeRight",
		"CV_TabEdgeBottom",
		"CV_EditBoxHeight",
		"CV_EditBoxBorderWidth",
		"CV_EditBoxBorderHeight",
		"CV_CBTextButtonBorder",
		"CV_NormalButtonBorder",
		"CV_CheckboxWidth",
		"CV_CheckboxHeight",
		"CV_RadioButtonWidth",
		"CV_RadioButtonHeight",
		"CV_SliderTrackBorder",
		0
	};

	for(s32 i=0; sVars[i]; i++)
	{
		iXMLAttribute* attr = e->find(sVars[i]);
		X_Assert(attr);
		mkVariables[i] = attr->getAsSignedInt();
	}

	if(static_font)
		mkVariables[eCVStaticTextHeight] = (i2d)static_font->getCharacterHeight();
}

// load settings
void cDirectX9UISkin::_readAndLoadSettings(iXMLElement* e)
{
	X_Assert(e);

	cStr kPath;
	u32  uKey;
	iXMLAttribute* pkColor;

	// read colors
	iXMLElement* pkCol = (iXMLElement*)e->findChild("Colors");
	X_Assert(pkCol);

	pkColor = pkCol->find("GuiColor");
	X_Assert(pkColor);
	col_gui = (s32)pkColor->getAsHexValue();
	
	pkColor = pkCol->find("StaticTextColor");
	X_Assert(pkColor);
	col_statictext = (s32)pkColor->getAsHexValue();

	pkColor = pkCol->find("EditTextColor");
	X_Assert(pkColor);
	col_edittext = (s32)pkColor->getAsHexValue();

	pkColor = pkCol->find("CaretColor");
	X_Assert(pkColor);
	col_caret = (s32)pkColor->getAsHexValue();

	pkColor = pkCol->find("GuiBackground");
	X_Assert(pkColor);
	col_background = (s32)pkColor->getAsHexValue();

	_readPathAndKey((iXMLElement*)e->findChild("Texture"),kPath,uKey);
	mpkSkin = (cDirectX9Texture*)X_SharedPtr(iTextureFactory)->load(kPath,uKey);	
	_readPathAndKey((iXMLElement*)e->findChild("FontStatic"),kPath,uKey);
	static_font = (cDirectX9Font*)cFontFactory::XSELFPTR->load(kPath,uKey);

	_readPathAndKey((iXMLElement*)e->findChild("FontEdit"),kPath,uKey);
	edit_font = (cDirectX9Font*)cFontFactory::XSELFPTR->load(kPath,uKey);

	X_Assert(static_font && edit_font && mpkSkin);

}

bool cDirectX9UISkin::_load(iXMLDocument* doc)
{
	iXMLElement* xGui = (iXMLElement*)doc->findChild(X_Text("XGUI"));
	X_Assert(xGui);
	_readAndLoadSettings((iXMLElement*)xGui->findChild(X_Text("Settings")));
	_readAllVars((iXMLElement*)xGui->findChild(X_Text("Variables")));
	_readAllRects((iXMLElement*)xGui->findChild(X_Text("Rects")));
	doc->release();
	return true;
}

// drawing
void cDirectX9UISkin::drawControl(const rect2_x &rc, iXuiSkin::CommonID id)
{
	const size2_x& rkRm = ((cDirectX9Renderer*)X_SharedPtr(iRenderer))->getScreenSize();
	((cDirectX9Renderer*)X_SharedPtr(iRenderer))->setTexture(0,mpkSkin);

	v[0].x = v[1].x =
		v_x(rc.left);
		
	v[2].x = v[3].x = 
		v_x(rc.right);

	v[1].y = v[3].y = 
		v_y(rc.top);

	v[0].y = v[2].y = 
		v_y(rc.bottom);

	v[0].color = v[1].color = 
	v[2].color = v[3].color = col_gui;
	f32* set = 0;

	switch(id)
	{
	case ID_Caret:
		set = caret_uv; break;
	case ID_Progress:
		set = progressbar_uv; break;
	case ID_SelectedLBItem:
		set = lbselhighlight_uv; break;
	case ID_SelectedCBItem:
		set = cbselhighlight_uv; break;
	case ID_ScrollTrack:
		set = scroll_uv; break;
	case ID_EditTextSelection:
		set = editsel_uv; break;
	}
	v[0].u = v[1].u = set[0];
	v[2].u = v[3].u = set[1];
	v[1].v = v[3].v = set[2];
	v[0].v = v[2].v = set[3];

	((cDirectX9Renderer*)X_SharedPtr(iRenderer))->draw2DTriStrip(v,2);
}

// draw rect
void cDirectX9UISkin::_drawRect(const rect2_x &r, bool bTitlebar, bool bClient, const tchar *sCaption, f32* fTex,s32* piCoord)
{
	const size2_x& rkRm = ((cDirectX9Renderer*)X_SharedPtr(iRenderer))->getScreenSize();

	((cDirectX9Renderer*)X_SharedPtr(iRenderer))->setTexture(0,mpkSkin);

	v[0].color = v[1].color = 
	v[2].color = v[3].color = 
	v[4].color = v[5].color = 
	v[6].color = v[7].color =
	col_gui;

	v[0].x = v[1].x = v_x(r.left);
	v[2].x = v[3].x = v_x(r.left+piCoord[0]);
	v[4].x = v[5].x = v_x(r.right-piCoord[2]);
	v[6].x = v[7].x = v_x(r.right);

	v[0].u = v[1].u = fTex[0];
	v[2].u = v[3].u = fTex[1];
	v[4].u = v[5].u = fTex[2];
	v[6].u = v[7].u = fTex[3];

	f32 clary1 = v_y(r.top+piCoord[1]);
	f32 clary2 = v_y(r.bottom-piCoord[3]);

	// draw the client area
	if( bClient )
	{
		v[0].y = 
		v[2].y =
		v[4].y =
		v[6].y = clary2;

		v[1].y = 
		v[3].y =
		v[5].y =
		v[7].y = clary1;

		v[1].v = 
		v[3].v =
		v[5].v =
		v[7].v = fTex[5];

		v[0].v = 
		v[2].v =
		v[4].v =
		v[6].v = fTex[6];

		((cDirectX9Renderer*)X_SharedPtr(iRenderer))->draw2DTriStrip(v,6);
	}

	// draw the bottom
	v[1].y = 
	v[3].y =
	v[5].y =
	v[7].y = clary2;

	v[0].y = 
	v[2].y =
	v[4].y =
	v[6].y = v_y(r.bottom);

	v[1].v = 
	v[3].v =
	v[5].v =
	v[7].v = fTex[6];

	v[0].v = 
	v[2].v =
	v[4].v =
	v[6].v = fTex[7];

	((cDirectX9Renderer*)X_SharedPtr(iRenderer))->draw2DTriStrip(v,6);

	// draw the title bar if requested
	v[0].y = 
	v[2].y =
	v[4].y =
	v[6].y = clary1;

	v[1].y = 
	v[3].y =
	v[5].y =
	v[7].y = v_y(r.top);

	if(bTitlebar)
	{
		v[1].v = 
		v[3].v =
		v[5].v =
		v[7].v = fTex[4];

		v[0].v = 
		v[2].v =
		v[4].v =
		v[6].v = fTex[5];
		
		((cDirectX9Renderer*)X_SharedPtr(iRenderer))->draw2DTriStrip(v,6);
		if(sCaption)
		{
			rect2_x kCapRect( r.left+piCoord[0]+1,r.top,r.right-piCoord[2],r.top+piCoord[1]);
			static_font->drawSingleLineText(kCapRect,iXuiFont::DrawYCentered,col_statictext,sCaption);
		}
	}
	else
	{
		v[1].v = 
		v[3].v =
		v[5].v =
		v[7].v = fTex[7];
		v[0].v = 
		v[2].v =
		v[4].v =
		v[6].v = fTex[6];
		((cDirectX9Renderer*)X_SharedPtr(iRenderer))->draw2DTriStrip(v,6);
	}
}

// draw rect2
void cDirectX9UISkin::_drawRect2(const rect2_x &r,i2d var,f32 *set,const tchar* sCaption)
{
	((cDirectX9Renderer*)X_SharedPtr(iRenderer))->setTexture(0,mpkSkin);
	
	v[0].color = v[1].color = 
	v[2].color = v[3].color = 
	v[4].color = v[5].color = 
	v[6].color = v[7].color =
	col_gui;

	v[0].x = v[1].x = v_x(r.left);
	v[2].x = v[3].x = v_x(r.left+var);
	v[4].x = v[5].x = v_x(r.right-var);
	v[6].x = v[7].x = v_x(r.right);

	v[0].u = v[1].u = set[0];
	v[2].u = v[3].u = set[1];
	v[4].u = v[5].u = set[2];
	v[6].u = v[7].u = set[3];

	v[0].y = 
	v[2].y =
	v[4].y =
	v[6].y = v_y(r.bottom);

	v[1].y = 
	v[3].y =
	v[5].y =
	v[7].y = v_y(r.top);

	v[1].v = 
	v[3].v =
	v[5].v =
	v[7].v = set[4];

	v[0].v = 
	v[2].v =
	v[4].v =
	v[6].v = set[5];
	((cDirectX9Renderer*)X_SharedPtr(iRenderer))->draw2DTriStrip(v,6);
	if(sCaption)
		static_font->drawSingleLineText(r,iXuiFont::DrawXCentered|iXuiFont::DrawYCentered,col_statictext,sCaption);
}

//draw window
void cDirectX9UISkin::drawWindow(const rect2_x &r, bool bTitlebar, bool bClient, const tchar *sCaption)
{
	_drawRect(r,bTitlebar,bClient,sCaption,win_uv,&mkVariables[eCVWinEdgeLeft]);
}

//list box
void cDirectX9UISkin::drawListBox(const nextar::rect2_x &r, const tchar *sCaption)
{
	// draw window
	_drawRect(r,true,true,sCaption,lbox_uv,&mkVariables[eCVLBEdgeLeft]);
}

// control states
void cDirectX9UISkin::drawControlState(const rect2_x &r, iXuiSkin::CommonID uId, EControlState s, const tchar *sCaption)
{
	((cDirectX9Renderer*)X_SharedPtr(iRenderer))->setTexture(0,mpkSkin);
	s32 d = 0;
	switch(s)
	{
	case CS_MouseOver:
		d = 1; break;
	case CS_Checked:
	case CS_Pressed:
		d = 2; break;
	case CS_Disabled:
		d = 3; break;
	}
	if( uId <= ID_ComboBox )
	{
		switch(uId)
		{
		case ID_ComboBox:
			_drawRect2(r,mkVariables[eCVCBTextButtonBorder],&combobox_uv[d*6],sCaption);
			break;
		case ID_NormalButton:
			_drawRect2(r,mkVariables[eCVNormalButtonBorder],&button_uv[d*6],sCaption);
			break;
		case ID_Tab:
			_drawRect2(r,mkVariables[eCVTabBorder],&tab_uv[d*6],sCaption);
			break;
		}
	}
	else
	{
	
		f32* set = 0;
		bool caption = false;
		rect2_x cap;

		switch(uId)
		{
		case ID_SliderButton: set = sliderbutton_uv[d]; break;
		case ID_ComboDropDownButton: set = combobutton_uv[d]; break;
		case ID_BWinCloseButton: set = wndclose_uv[d]; break;
		case ID_Checkbox: caption = true; set = checkbox_uv[d]; 
			cap.left = r.left+mkVariables[eCVCheckboxWidth];
			cap.top = (r.height()-(mkVariables[eCVCheckboxHeight]))>>1; // misuse	
			goto set_rest;
		case ID_RadioButton: caption = true; set = radio_uv[d]; 
			cap.left = r.left+mkVariables[eCVRadioButtonWidth];
			cap.top = (r.height()-(mkVariables[eCVRadioButtonHeight]))>>1; // misuse
			goto set_rest;
		case ID_ScrollButtonUp: set = scrollup_uv[d]; break;
		case ID_ScrollButtonDown: set = scrolldown_uv[d]; break;
		case ID_ScrollThumb: set = thumb_uv[d]; break;
		default: 
			return;
set_rest:
			v[0].x = v[1].x = v_x(r.left);
			v[2].x = v[3].x = v_x(cap.left);
			v[1].y = v[3].y = v_y(r.top+cap.top); 
			v[0].y = v[2].y = v_y(r.bottom-cap.top);
			cap.top = r.top; cap.right = r.right; cap.bottom = r.bottom;
			cap.left += 4;
			goto donotset;
		}

	
		v[0].x = v[1].x = v_x(r.left);
		v[2].x = v[3].x = v_x(r.right);
		v[1].y = v[3].y = v_y(r.top); 
		v[0].y = v[2].y = v_y(r.bottom);
donotset:
		v[0].color = v[1].color = 
		v[2].color = v[3].color = col_gui;

		v[0].u = v[1].u = set[0];
		v[2].u = v[3].u = set[1];
		v[1].v = v[3].v = set[2];
		v[0].v = v[2].v = set[3];
		((cDirectX9Renderer*)X_SharedPtr(iRenderer))->draw2DTriStrip(v,2);
		if(caption && sCaption)
			static_font->drawSingleLineText(cap,iXuiFont::DrawYCentered,col_statictext,sCaption);
	}
}

// combo
void cDirectX9UISkin::drawComboDropDownArea(const rect2_x &r)
{
	_drawRect2(r,mkVariables[eCVCBDropDownBorer],comboddown_uv,0);
}

// slider track
void cDirectX9UISkin::drawSliderTrack(const rect2_x &r)
{
	_drawRect2(r,mkVariables[eCVSliderTrackBorder],slider_uv,0);
}

// tab wnd
void cDirectX9UISkin::drawTabWindow(const rect2_x& r)
{
	_drawRect(r,true,true,0,tabwnd_uv,&mkVariables[eCVTabEdgeLeft]);
}

// cursor
void cDirectX9UISkin::drawCursor(nextar::iXuiSkin::ECursorType type, const nextar::coord2_x &c)
{
	rect2_x	r(c,cursor_size[type]);
	((cDirectX9Renderer*)X_SharedPtr(iRenderer))->draw2DImage(mpkSkin,&cursor_uvs[type*4],r,col_gui);
}

//static text
void cDirectX9UISkin::drawStaticText(const tchar *sText, const nextar::rect2_x &rc, u32 Style)
{
	static_font->drawSingleLineText(rc,Style,col_statictext,sText);
}

// pg
void cDirectX9UISkin::drawProgressBarBorder(const rect2_x& r,const tchar* sCap)
{
	_drawRect2(r,mkVariables[eCVProgressBarBorderWidth],pbarborder_uv,sCap);
}

void cDirectX9UISkin::drawEditControlBorder(const rect2_x& r)
{
	_drawRect2(r,mkVariables[eCVEditBoxBorderWidth],editborder_uv,0);
}
