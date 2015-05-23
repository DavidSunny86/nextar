//#@ Obi De
//#@ December 26 06
//#@ January 9 06
//#@ September 16 07
//#@ 12 Oct 2007
// xkeycode.h
#ifndef NEXTAR_KEYCODES_H
#define NEXTAR_KEYCODES_H

#include <NexEngine.h>

#define NEX_BUTTON_KEY(val) (val & ((~KEY_ANALOG_BIT)&(~KEY_DIRECTION_BIT)))
#define NEX_ANALOG_KEY(val) (val|KEY_ANALOG_BIT)
#define NEX_DIRECTION_KEY(val) (val|KEY_DIRECTION_BIT)

namespace nextar {

enum KeyState : uint8 {
	KEY_STATE_UP,
	KEY_STATE_DOWN
};

enum KeyAnalogBit {
	KEY_ANALOG_BIT = 0x8000,
	KEY_DIRECTION_BIT = 0x4000
};

/* Standard key codes, can be directly used without input mapper */
enum Key : uint16 {
#define NEX_KEY_CONVERTABLE_START		(uint16)(Key::KB_ESCAPE)
#define NEX_KEYBOARD_KEY_START	(uint16)(Key::KB_ESCAPE)
	KB_ESCAPE = NEX_BUTTON_KEY(1),
#define NEX_KEY_ALPHABETS_START		(uint16)(Key::KB_A)
	KB_A,
	KB_B,
	KB_C,
	KB_D,
	KB_E,
	KB_F,
	KB_G,
	KB_H,
	KB_I,
	KB_J,
	KB_K,
	KB_L,
	KB_M,
	KB_N,
	KB_O,
	KB_P,
	KB_Q,
	KB_R,
	KB_S,
	KB_T,
	KB_U,
	KB_V,
	KB_W,
	KB_X,
	KB_Y,
	KB_Z,
#define NEX_KEY_ALPHABETS_END	(uint16)(Key::KB_SPACE)
	KB_SPACE,
	KB_TAB,
	KB_RETURN,
	KB_NUMPADENTER,

#define NEX_KEY_NUMKEYS_START	(uint16)(Key::KB_0)
	KB_0,
	KB_1,
	KB_2,
	KB_3,
	KB_4,
	KB_5,
	KB_6,
	KB_7,
	KB_8,
	KB_9,
#define NEX_KEY_NUMKEYS_END	(uint16)(Key::KB_GRAVE)

	KB_GRAVE,
	KB_MINUS,
	KB_EQUALS,
	KB_BACKSLASH,
	KB_LBRACKET,
	KB_RBRACKET,
	KB_SEMICOLON,
	KB_APOSTROPHE,
	KB_COMMA,
	KB_PERIOD,
	KB_SLASH,

	KB_NUMPADEQUALS,
	KB_ADD,
	KB_SUBTRACT,
	KB_MULTIPLY,
	KB_DIVIDE,
	KB_DECIMAL,

	KB_NUMPADCOMMA,
	KB_CIRCUMFLEX,
	KB_COLON,
	KB_UNDERLINE,

#define NEX_KEY_CONVERTABLE_END	(uint16)(Key::KB_UNDERLINE)
#define NEX_KEY_CONVERTABLE_COUNT (uint16)(NEX_KEY_CONVERTABLE_START - NEX_KEY_CONVERTABLE_END) + 1
#define NEX_KEY_PADKEYS_START	(uint16)(Key::KB_NUMPAD0)
	KB_NUMPAD0,
	KB_NUMPAD1,
	KB_NUMPAD2,
	KB_NUMPAD3,
	KB_NUMPAD4,
	KB_NUMPAD5,
	KB_NUMPAD6,
	KB_NUMPAD7,
	KB_NUMPAD8,
	KB_NUMPAD9,
#define NEX_KEY_PADKEYS_END		(uint16)(Key::KB_F1)

#define NEX_KEY_FUNCTIONKEYS_START	(uint16)(Key::KB_F1)
	KB_F1,
	KB_F2,
	KB_F3,
	KB_F4,
	KB_F5,
	KB_F6,
	KB_F7,
	KB_F8,
	KB_F9,
	KB_F10,
	KB_F11,
	KB_F12,
	KB_F13,
	KB_F14,
	KB_F15,
#define NEX_KEY_FUNCTIONKEYS_END	(uint16)(Key::KB_F15)
	KB_BACK,

	KB_CAPITAL,
	KB_NUMLOCK,
	KB_SCROLL,
	KB_KANA,
	KB_CONVERT,
	KB_NOCONVERT,
	KB_YEN,
	KB_AT,
	KB_KANJI,
	KB_STOP,
	KB_AX,
	KB_UNLABELED,
	KB_SYSRQ,
	KB_PAUSE,
	KB_HOME,
	KB_UP,
	KB_PRIOR,
	KB_LEFT,
	KB_RIGHT,
	KB_END,
	KB_DOWN,
	KB_NEXT,
	KB_INSERT,
	KB_DELETE,
	KB_LWIN,
	KB_RWIN,
	KB_APPS,
	KB_POWER,
	KB_SLEEP,

#define NEX_KEY_NONCONTROL_COUNT (uint16)(Key::KB_LCONTROL)

#define NEX_KEY_CONTROLKEYS_START (uint16)(Key::KB_LCONTROL)
	KB_LCONTROL,
	KB_RCONTROL,
	KB_LSHIFT,
	KB_RSHIFT,
	KB_LALT,
	KB_RALT,
#define NEX_KEY_CONTROLKEYS_END (uint16)(Key::KB_UNKNOWN_K)
	KB_UNKNOWN_K, // this is never queried

#define NEX_KEYBOARD_KEY_END	(uint16)(Key::KB_UNKNOWN_K)
#define NEX_KEYBOARD_KEY_COUNT ((uint16)(Key::KB_UNKNOWN_K)+1)
#define NEX_MOUSE_BUTTON_START ((uint16)(Key::MOUSE_LEFT))

	MOUSE_LEFT = NEX_BUTTON_KEY(1000),
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_THUMB1,
	MOUSE_THUMB2,

#define NEX_MOUSE_BUTTON_END ((uint16)(Key::MOUSE_THUMB2))
#define NEX_MOUSE_DIGITAL_BUTTON_COUNT (((uint16)Key::MOUSE_THUMB2 - (uint16)Key::MOUSE_LEFT)+1)

	MOUSE_XY_AXIS = NEX_DIRECTION_KEY(1100), /* Direction */
	MOUSE_WHEEL = NEX_ANALOG_KEY(1200), /* Analog */

#define NEX_MOUSE_ANALOG_BUTTON_COUNT 3
/*
	JOY_BUTTON_0,
#define NEX_JOY_BUTTON(i) (JOY_BUTTON_0 + i)
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
*/
#define NEX_XB360_CTRL_BUTTON_START (uint16)(Key::XBOX_A)
	XBOX_A = NEX_BUTTON_KEY(4000),
	XBOX_B,
	XBOX_X,
	XBOX_Y,
	XBOX_LEFT,
	XBOX_RIGHT,
	XBOX_UP,
	XBOX_DOWN,
	XBOX_START,
	XBOX_CONTROLLER_SEL,
	XBOX_BACK,
	XBOX_SHOULDER_BUTTON_LEFT,
	XBOX_SHOULDER_BUTTON_RIGHT,
#define NEX_XB360_CTRL_BUTTON_END ((uint16)XBOX_SHOULDER_BUTTON_RIGHT+1)
	XBOX_TRIG_LEFT = NEX_ANALOG_KEY(4100),
	XBOX_TRIG_RIGHT,
	XBOX_AXIS_LEFT = NEX_DIRECTION_KEY(4200),
	XBOX_AXIS_RIGHT,

	KEY_INVALID_CODE = 0xffff

};

}

#endif //__XKEYCODE_H__
