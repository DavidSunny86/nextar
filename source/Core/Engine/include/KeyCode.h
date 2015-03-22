//#@ Obi De
//#@ December 26 06
//#@ January 9 06
//#@ September 16 07
//#@ 12 Oct 2007
// xkeycode.h
#ifndef NEXTAR_KEYCODES_H
#define NEXTAR_KEYCODES_H

#include <NexEngine.h>

namespace nextar {

/* Standard key codes, can be directly used without input mapper */
enum StandardKeyCodes {
	KEY_ESCAPE = 0x0001,
#define NEX_KEY_ALPHABETS_START		KEY_A
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
#define NEX_KEY_ALPHABETS_END	KEY_SPACE
	KEY_SPACE,
	KEY_TAB,
	KEY_RETURN,
	KEY_NUMPADENTER,

#define NEX_KEY_NUMKEYS_START	KEY_0
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
#define NEX_KEY_NUMKEYS_END	KEY_GRAVE

	KEY_GRAVE,
	KEY_MINUS,
	KEY_EQUALS,
	KEY_BACKSLASH,
	KEY_LBRACKET,
	KEY_RBRACKET,
	KEY_SEMICOLON,
	KEY_APOSTROPHE,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_SLASH,

	KEY_NUMPADEQUALS,
	KEY_ADD,
	KEY_SUBTRACT,
	KEY_MULTIPLY,
	KEY_DIVIDE,
	KEY_DECIMAL,

	KEY_NUMPADCOMMA,
	KEY_CIRCUMFLEX,
	KEY_COLON,
	KEY_UNDERLINE,

#define NEX_KEY_CONVERTABLE_COUNT KEY_NUMPAD0

#define NEX_KEY_PADKEYS_START	KEY_NUMPAD0
	KEY_NUMPAD0,
	KEY_NUMPAD1,
	KEY_NUMPAD2,
	KEY_NUMPAD3,
	KEY_NUMPAD4,
	KEY_NUMPAD5,
	KEY_NUMPAD6,
	KEY_NUMPAD7,
	KEY_NUMPAD8,
	KEY_NUMPAD9,
#define NEX_KEY_PADKEYS_END		KEY_F1

#define NEX_KEY_FUNCTIONKEYS_START	KEY_F1
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_F13,
	KEY_F14,
	KEY_F15,
#define NEX_KEY_FUNCTIONKEYS_END	KEY_F15
	KEY_BACK,

	KEY_CAPITAL,
	KEY_NUMLOCK,
	KEY_SCROLL,
	KEY_KANA,
	KEY_CONVERT,
	KEY_NOCONVERT,
	KEY_YEN,
	KEY_AT,
	KEY_KANJI,
	KEY_STOP,
	KEY_AX,
	KEY_UNLABELED,
	KEY_SYSRQ,
	KEY_PAUSE,
	KEY_HOME,
	KEY_UP,
	KEY_PRIOR,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_END,
	KEY_DOWN,
	KEY_NEXT,
	KEY_INSERT,
	KEY_DELETE,
	KEY_LWIN,
	KEY_RWIN,
	KEY_APPS,
	KEY_POWER,
	KEY_SLEEP,

#define NEX_KEY_NONCONTROL_COUNT KEY_LCONTROL

#define NEX_KEY_CONTROLKEYS_START KEY_LCONTROL
	KEY_LCONTROL,
	KEY_RCONTROL,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_LALT,
	KEY_RALT,
#define NEX_KEY_CONTROLKEYS_END KEY_UNKNOWN_K
	KEY_UNKNOWN_K, // this is never queried

#define NEX_KEYBOARD_KEY_COUNT KEY_UNKNOWN_K+1

	MOUSE_LEFT,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_THUMB1,
	MOUSE_THUMB2,

#define NEX_MOUSE_DIGITAL_BUTTON_COUNT ((MOUSE_THUMB2 - MOUSE_LEFT)+1)

	JOY_BUTTON_0,
#define NEX_JOY_BUTTON(i) (JOY_BUTTON_0 + i)

	MOUSE_XY_AXIS = 0x4000, /* Direction */
	MOUSE_WHEEL = 0x8000, /* Analog */

#define NEX_MOUSE_ANALOG_BUTTON_COUNT 3

	JOY_XAXIS_POS,
	JOY_YAXIS_POS,
	JOY_ZAXIS_POS,
	JOY_XAXIS_ROT,
	JOY_YAXIS_ROT,
	JOY_ZAXIS_ROT,
	JOY_PSLIDER1, // extra position slider1
	JOY_PSLIDER2, // extra position slider2
	JOY_POV1,
	JOY_POV2,
	JOY_POV3,
	JOY_POV4,
	JOY_VXAXIS_POS, // velocity
	JOY_VYAXIS_POS, // velocity
	JOY_VZAXIS_POS, // velocity
	JOY_VXAXIS_ROT, // velocity
	JOY_VYAXIS_ROT, // velocity
	JOY_VZAXIS_ROT, // velocity
	JOY_VSLIDER1, // extra velocity slider1
	JOY_VSLIDER2, // extra velocity slider2
	JOY_AXAXIS_POS, // accelaration
	JOY_AYAXIS_POS, // accelaration
	JOY_AZAXIS_POS, // accelaration
	JOY_AXAXIS_ROT, // accelaration
	JOY_AYAXIS_ROT, // accelaration
	JOY_AZAXIS_ROT, // accelaration
	JOY_ASLIDER1, // extra accelaration slider1
	JOY_ASLIDER2, // extra accelaration slider2
	JOY_FXAXIS, // force
	JOY_FYAXIS, // force
	JOY_FZAXIS, // force
	JOY_TXAXIS, // torque
	JOY_TYAXIS, // torque
	JOY_TZAXIS, // torque
	JOY_FSLIDER1, // extra force slider1
	JOY_FSLIDER2, // extra force slider2

	NEX_KEY_INVALID_COOKED_CODE = 0xffff
};

}

#endif //__XKEYCODE_H__
