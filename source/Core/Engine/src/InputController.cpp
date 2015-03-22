
#include <InputController.h>

namespace nextar {

namespace InputUtils {

static const char asciiForKey[NEX_KEY_CONVERTABLE_COUNT] = {
		27, // KEY_ESCAPE,
		'a', // KEY_A,
		'b', // KEY_B,
		'c', // KEY_C,
		'd', // KEY_D,
		'e', // KEY_E,
		'f', // KEY_F,
		'g', // KEY_G,
		'h', // KEY_H,
		'i', // KEY_I,
		'j', // KEY_J,
		'k', // KEY_K,
		'l', // KEY_L,
		'm', // KEY_M,
		'n', // KEY_N,
		'o', // KEY_O,
		'p', // KEY_P,
		'q', // KEY_Q,
		'r', // KEY_R,
		's', // KEY_S,
		't', // KEY_T,
		'u', // KEY_U,
		'v', // KEY_V,
		'w', // KEY_W,
		'x', // KEY_X,
		'y', // KEY_Y,
		'z', // KEY_Z,
		' ', // KEY_SPACE,
		'\t', // KEY_TAB,
		13, // KEY_RETURN,
		13, // KEY_NUMPADENTER,

		48, // KEY_0,
		49, // KEY_1,
		50, // KEY_2,
		51, // KEY_3,
		52, // KEY_4,
		53, // KEY_5,
		54, // KEY_6,
		55, // KEY_7,
		56, // KEY_8,
		57, // KEY_9,

		'`', // KEY_GRAVE,
		'-', // KEY_MINUS,
		'=', // KEY_EQUALS,
		'\\', // KEY_BACKSLASH,
		'[', // KEY_LBRACKET,
		']', // KEY_RBRACKET,
		';', // KEY_SEMICOLON,
		'\'', // KEY_APOSTROPHE,
		',', // KEY_COMMA,
		'.', // KEY_PERIOD,
		'/', // KEY_SLASH,

		'=', // KEY_NUMPADEQUALS,
		'+', // KEY_ADD,
		'-', // KEY_SUBTRACT,
		'*', // KEY_MULTIPLY,
		'/', // KEY_DIVIDE,
		'.', // KEY_DECIMAL,

		',', // KEY_NUMPADCOMMA,
		'^', // KEY_CIRCUMFLEX,
		':', // KEY_COLON,
		'_' // KEY_UNDERLINE,
		};

/**********************************************
 ./ Lookup table for shifting ascii codes as per numeric keys
 **********************************************/
static const char shiftNumKeys[10] = { ')', '!', '@', '#', '$', '%', '^', '&',
		'*', '(' };

/**********************************************
 ./ Lookup table for ascii codes as per other keys
 **********************************************/
static const char shiftOtherKeys[] = { '~', '_', '+', '|', '{', '}', ':', '"',
		'<', '>', '?' };

_NexEngineAPI int32 GetCookedCode(KeyID k, bool shift, bool numlock) {
	if (k >= 0 && k < NEX_KEY_CONVERTABLE_COUNT) {
		if (shift) {
			if (k >= KEY_A && k <= KEY_Z)
				return (int32)('A' + (k - KEY_A));
			else if (k >= KEY_0 && k <= KEY_9)
				return (int32) shiftNumKeys[k - KEY_0];
			else if (k >= KEY_GRAVE && k <= KEY_SLASH)
				return (int32) shiftOtherKeys[k - KEY_GRAVE];
		}

		return asciiForKey[k];
	} else if (numlock
			&& (k >= NEX_KEY_PADKEYS_START && k < NEX_KEY_PADKEYS_END))
		return (int32)('0' + (k - NEX_KEY_PADKEYS_START));
	return (int32) NEX_KEY_INVALID_COOKED_CODE;
}

}
}
