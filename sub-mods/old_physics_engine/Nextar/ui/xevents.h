//#@	Abhishek Dey
//#@	12/26/2006		AT:		10:24:14 AM
//#@	1/9/2007		AT:		3:11:57 PM
//#@	27 sep 2007

#ifndef __XEVENT_H__
#define __XEVENT_H__

#define INVALID_CALLBACK_EVENT_ID -1
#include <io/xkeycode.h>

namespace nextar
{

class iXuiWnd;

struct cKeyEvent
{
	// flags
	enum { KeyPressed = 1, Shift = 2, Ctrl = 4, Alt = 8, RepeatMask = 16 };
	u8		muKeyValue;
	tchar	mcChar;
	u32		muFlags;
};

struct cMouseEvent
{
	// mouse events
	enum { Shift = 2, Ctrl = 4, Alt = 8 };
	enum MouseEvents { LBDown, LBUp, RBDown, RBUp, MScroll, MDrag, MMove };
	s16			x; // mouse coords;
	s16			y;
	s16			m; // scroll coords;
	s16			meType;
	s32			muCtrlKeys;
};

struct cUserEvent
{
	// user event
	s16			miEventId;
	s16			miValue;
	f32			mfValue;
};

}

#endif //__XEVENT_H__