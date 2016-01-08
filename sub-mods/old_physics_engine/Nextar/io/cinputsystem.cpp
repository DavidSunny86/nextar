//#@ Abhishek Dey
//#@ December 26 06
//#@ September 16 07

#include <cinputsystem.h>
#include <iinputdevice.h>

namespace nextar
{

/**********************************************
./ Lookup table for ascii codes
**********************************************/
static const tchar garrAsciiForKey[KEY_CONVERTABLE_COUNT] =
{
	27,		// KEY_ESCAPE,
	'a',	// KEY_A,
	'b',	// KEY_B,
	'c',	// KEY_C,
	'd',	// KEY_D,
	'e',	// KEY_E,
	'f',	// KEY_F,
	'g',	// KEY_G,
	'h',	// KEY_H,
	'i',	// KEY_I,
	'j',	// KEY_J,
	'k',	// KEY_K,
	'l',	// KEY_L,
	'm',	// KEY_M,
	'n',	// KEY_N,
	'o',	// KEY_O,
	'p',	// KEY_P,
	'q',	// KEY_Q,
	'r',	// KEY_R,
	's',	// KEY_S,
	't',	// KEY_T,
	'u',	// KEY_U,
	'v',	// KEY_V,
	'w',	// KEY_W,
	'x',	// KEY_X,
	'y',	// KEY_Y,
	'z',	// KEY_Z,
	' ',	// KEY_SPACE,
	'\t',	// KEY_TAB,
	13,		// KEY_RETURN,
	13,		// KEY_NUMPADENTER,

	48,		// KEY_0,
	49,		// KEY_1,
	50,		// KEY_2,
	51,		// KEY_3,
	52,		// KEY_4,
	53,		// KEY_5,
	54,		// KEY_6,
	55,		// KEY_7,
	56,		// KEY_8,
	57,		// KEY_9,
	48,		// KEY_NUMPAD0,
	49,		// KEY_NUMPAD1,
	50,		// KEY_NUMPAD2,
	51,		// KEY_NUMPAD3,
	52,		// KEY_NUMPAD4,
	53,		// KEY_NUMPAD5,
	54,		// KEY_NUMPAD6,
	55,		// KEY_NUMPAD7,
	56,		// KEY_NUMPAD8,
	57,		// KEY_NUMPAD9,

	'`',	// KEY_GRAVE,
	'-',	// KEY_MINUS,
	'=',	// KEY_EQUALS,
	'\\',	// KEY_BACKSLASH,
	'[',	// KEY_LBRACKET,
	']',	// KEY_RBRACKET,
	';',	// KEY_SEMICOLON,
	'\'',	// KEY_APOSTROPHE,
	',',	// KEY_COMMA,
	'.',	// KEY_PERIOD,
	'/',	// KEY_SLASH,
	
	'=',	// KEY_NUMPADEQUALS,
	'+',	// KEY_ADD,	
	'-',	// KEY_SUBTRACT,
	'*',	// KEY_MULTIPLY,
	'/',	// KEY_DIVIDE,
	'.',	// KEY_DECIMAL,
	
	',',		// KEY_NUMPADCOMMA,	
	'^',	// KEY_CIRCUMFLEX,
	':',	// KEY_COLON,
	'_'		// KEY_UNDERLINE,	
};

/**********************************************
./ Lookup table for key names
**********************************************/
static const tchar* garrpKeyboardKeyNames[] =
{
	X_Text("Escape"),
	X_Text("A"),
	X_Text("B"),
	X_Text("C"),
	X_Text("D"),
	X_Text("E"),
	X_Text("F"),
	X_Text("G"),
	X_Text("H"),
	X_Text("I"),
	X_Text("J"),
	X_Text("K"),
	X_Text("L"),
	X_Text("M"),
	X_Text("N"),
	X_Text("O"),
	X_Text("P"),
	X_Text("Q"),
	X_Text("cnum"),
	X_Text("S"),
	X_Text("T"),
	X_Text("U"),
	X_Text("V"),
	X_Text("W"),
	X_Text("X"),
	X_Text("Y"),
	X_Text("Z"),
	X_Text("Space"),
	X_Text("Tab"),
	X_Text("Return"),
	X_Text("Numpad Enter"),
	X_Text("0"),
	X_Text("1"),
	X_Text("2"),
	X_Text("3"),
	X_Text("4"),
	X_Text("5"),
	X_Text("6"),
	X_Text("7"),
	X_Text("8"),
	X_Text("9"),
	X_Text("Numpad 0"),
	X_Text("Numpad 1"),
	X_Text("Numpad 2"),
	X_Text("Numpad 3"),
	X_Text("Numpad 4"),
	X_Text("Numpad 5"),
	X_Text("Numpad 6"),
	X_Text("Numpad 7"),
	X_Text("Numpad 8"),
	X_Text("Numpad 9"),
	X_Text("Grave"),
	X_Text("Minus"),
	X_Text("Equals"),
	X_Text("Backslash"),
	X_Text("Left Bracket"),
	X_Text("Right Bracket"),
	X_Text("Semicolon"),
	X_Text("Apostrophe"),
	X_Text("Comma"),
	X_Text("Period"),
	X_Text("Slash"),
	X_Text("Numpad Equals"),
	X_Text("Add"),	
	X_Text("Subtract"),
	X_Text("Multiply"),
	X_Text("Divide"),
	X_Text("Decimal"),
	X_Text("Numpad Comma"),	
	X_Text("Circumflex"),
	X_Text("Colon"),
	X_Text("Underline"),
	X_Text("F1"),
	X_Text("F2"),
	X_Text("F3"),
	X_Text("F4"),
	X_Text("F5"),
	X_Text("F6"),
	X_Text("F7"),
	X_Text("F8"),
	X_Text("F9"),
	X_Text("F10"),
	X_Text("F11"),
	X_Text("F12"),
	X_Text("F13"),
	X_Text("F14"),
	X_Text("F15"),
	X_Text("Left Control"),
	X_Text("Left Shift"),
	X_Text("Backspace"),
	X_Text("Right Shift"),
	X_Text("Left Menu"),
	X_Text("Capital"),
	X_Text("Num Lock"),
	X_Text("Scroll Lock"),
	X_Text("Kana"),
	X_Text("Convert"),
	X_Text("Noconvert"),
	X_Text("Yen"),	
	X_Text("At"),
	X_Text("Kanji"),
	X_Text("Stop"),
	X_Text("Ax"),
	X_Text("Unlabeled"),	
	X_Text("Right Control"),	
	X_Text("SysRQ"),
	X_Text("Right Menu"),
	X_Text("Pause"),
	X_Text("Home"),
	X_Text("Up"),
	X_Text("Prior"),
	X_Text("Left"),
	X_Text("Right"),
	X_Text("End"),
	X_Text("Down"),
	X_Text("Next"),
	X_Text("Insert"),
	X_Text("Delete"),
	X_Text("Left Win"),
	X_Text("Right Win"),
	X_Text("Apps"),
	X_Text("Power"),
	X_Text("Sleep"),
};

/**********************************************
./ Lookup table for shifting ascii codes as per numeric keys
**********************************************/
static const tchar garrShiftNumKeys[10] = { ')', '!', '@', '#', '$', '%', '^', '&', '*', '(' };

/**********************************************
./ Lookup table for ascii codes as per other keys
**********************************************/
static const tchar garrShiftOtherKeys[] = { '~', '_', '+', '|', '{', '}', ':', '"', '<', '>', '?' };

/* static ptr init */
SharedSingleInterfaceSetup(cInputSystem);
/* ctor */
cInputSystem::cInputSystem()
{
	SharedSingleInterfaceInit(cInputSystem);
	RefObjectInit(cInputSystem);
}

/* dtor */
cInputSystem::~cInputSystem()
{
	// release the devices first.
	for( list_t<iInputDevice*>::iterator i = mkDevices.begin(); i != mkDevices.end(); ++i )
		(*i)->release();
	mkDevices.clear();
	mkEventReceiver.clear();
	SharedSingleInterfaceExit(cInputSystem);
}

/* activate: activates all devices */
void cInputSystem::activate()
{
	for( list_t<iInputDevice*>::iterator i = mkDevices.begin(); i != mkDevices.end(); ++i )
		(*i)->activate();
}

/* deactivate: deactivates all devices */
void cInputSystem::deactivate()
{
	for( list_t<iInputDevice*>::iterator i = mkDevices.begin(); i != mkDevices.end(); ++i )
		(*i)->deactivate();
}

/* registerInputHandler: register a new event receiver */
void cInputSystem::registerInputHandler( iInputHandler* pkR )
{
	mkEventReceiver.push_back(pkR);
}

/* removeInputHandler: unregister an old event receiver */
void cInputSystem::removeInputHandler( iInputHandler* pkR )
{
	mkEventReceiver.remove(pkR);
}

/* updateAllDevice: updates all devices attached to it */
void cInputSystem::updateAllDevice()
{
	XTRACE(("\ncXInputSystem::updateAllDevice"));
	for( list_t<iInputDevice*>::iterator i = mkDevices.begin(); i != mkDevices.end(); ++i )
		(*i)->pollData();
}

/* updateReceivers: updates the event receivers */
void cInputSystem::updateReceivers()
{
	XTRACE(("\nonFrameCall::updateReceivers"));
	for( list_t<iInputDevice*>::iterator i = mkDevices.begin(); i != mkDevices.end(); ++i )
		(*i)->distributeData(mkEventReceiver);
}

/* attachDeivice: attaches a deivece to the manager */
void cInputSystem::attachDeivice( iInputDevice* pkDev )
{
	pkDev->addRef();
	mkDevices.push_back(pkDev);
}

/* detachDevice: detaches a deivece from the manager */
void cInputSystem::detachDevice( iInputDevice *pkDev )
{
	for( list_t<iInputDevice*>::iterator i = mkDevices.begin(); i != mkDevices.end(); ++i )
	{
		if((*i)==pkDev)
		{
			mkDevices.erase(i);
			pkDev->release();
			break;
		}
	}
}

/* getKeyName: returns the name of a keyboard key */
const tchar* cInputSystem::getKeyName( KEYCODE k )
{
	if( k < KEY_COUNT )
		return garrpKeyboardKeyNames[k];
	return 0;
}

/* getAsciiForKey: returns the ascii value of a key */
const tchar cInputSystem::getAsciiForKey( KEYCODE k, bool bShift )
{
	if( k >= 0 && k < KEY_CONVERTABLE_COUNT )
	{
		if( bShift )
		{
			if( k >= KEY_A && k <= KEY_Z )
				return ('A' + (k-KEY_A));
			else if( k >= KEY_0 && k <= KEY_9 )
				return garrShiftNumKeys[k-KEY_0];
			else if( k >= KEY_GRAVE && k <= KEY_SLASH )
				return garrShiftOtherKeys[ k-KEY_GRAVE ];
		}

		return garrAsciiForKey[k];
	}
	return -1;
}

}