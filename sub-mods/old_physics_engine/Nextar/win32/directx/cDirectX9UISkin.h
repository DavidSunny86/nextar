//@ Abhishek Dey
//@ Sep 23 07
//@ Nov 23 07

#ifndef NEXTAR_DIRECTX9_UISKIN
#define NEXTAR_DIRECTX9_UISKIN

#include <xtypesex.h>
#include <win32/directx/cdirectx9font.h>
#include <win32/directx/cdirectx9texture.h>
#include <xml/ixmldocument.h>
#include <ui/ixuiskin.h>

namespace nextar
{
// check on the example default-skin.xml to see the 
// organisation of the xml

// ui skin

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
	cDirectX9UISkin(iXMLDocument* config);
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

}

#endif //NEXTAR_DIRECTX9_UISKIN