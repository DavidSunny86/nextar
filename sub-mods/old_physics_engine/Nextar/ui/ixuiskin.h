//@ Abhishek Dey
//@ Sep 23 07

#ifndef IXUISKIN_H
#define IXUISKIN_H

#include "cstr.h"
#include "xtypesex.h"
#include "iobject.h"
#include "graphics/xcolor.h"
#include "ui/ixuifont.h"

namespace nextar
{
// iskin can be derived from plugin so it does not inherit iObject
class iXuiFont;
class iTexture;
class iXuiSkin : public iObject
{
public:
	enum ECursorType {
		CT_Normal,
		CT_CrossHair,
		CT_Count
	};

	enum EControlState	{
		CS_Normal = 0,
		CS_MouseOver,
		CS_Pressed,
		CS_Checked,
		CS_HasFocus,
		CS_Disabled,
		CS_CtrlStateCount 
	};
	enum CommonID	{ 
		
		ID_NormalButton,
		ID_Tab,
		ID_ComboBox,

		ID_BWinCloseButton, 
		ID_Checkbox, 
		ID_RadioButton, 
		ID_ScrollButtonUp,
		ID_ScrollButtonDown,
		ID_ScrollTrack,
		ID_ScrollThumb,
		ID_SelectedLBItem,
		ID_ComboDropDownButton,
		ID_SelectedCBItem,
		ID_SliderTrack,
		ID_SliderButton,
		ID_Progress,
		ID_EditTextSelection,
		ID_Caret,
	};

public:
	// skin change notification
	virtual void		initSkinParameters() = 0;
	virtual iXuiFont*	getStaticFont() = 0;
	virtual iXuiFont*	getEditFont() = 0;
	virtual size2_x&		getCursorSize(ECursorType t) = 0;
	// draw 
	virtual void		drawControl(const rect2_x& r,CommonID) = 0; // for controls with only normal state
	virtual void		drawWindow(const rect2_x& r,bool bTitlebar,bool bClient,const tchar* sCaption) = 0;
	virtual void		drawTabWindow(const rect2_x& r) = 0;
	virtual void		drawListBox(const rect2_x& r,const tchar* sCaption) = 0;
	virtual void		drawComboDropDownArea(const rect2_x& r) = 0;
	virtual void		drawControlState(const rect2_x& r,CommonID uId,EControlState,const tchar* sCaption) = 0;
	virtual void		drawCursor( ECursorType, const coord2_x& ) = 0;
	virtual void		drawStaticText(const tchar* sText,const rect2_x&,u32 Style = iXuiFont::DrawXCentered|iXuiFont::DrawYCentered) = 0;
	virtual void		drawSliderTrack(const rect2_x& r) = 0;
	virtual void		drawProgressBarBorder(const rect2_x& r,const tchar* sCaption) = 0;
	virtual void		drawEditControlBorder(const rect2_x& r) = 0;
	// internals
	virtual ~iXuiSkin() {}
public:
};

}
#endif //IXUISKIN_H