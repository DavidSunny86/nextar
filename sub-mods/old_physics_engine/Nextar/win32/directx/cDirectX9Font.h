
#ifndef NEXTAR_CDIRECTX9FONT
#define NEXTAR_CDIRECTX9FONT


#include <xtypesex.h>
#include <iobject.h>
#include <graphics/itexture.h>
#include <ui/ixuifont.h>
#include <win32/directx/cdirectx9texture.h>

namespace nextar
{

class cDirectX9Font : public iXuiFont
{
public:
	cDirectX9Texture*	mpkTexture;
public:

	virtual ~cDirectX9Font();
	cDirectX9Font( const tchar* );

	// set texture (addRefs the texture
	virtual void setTexture(iTexture*);
	//@ draws a static text to the screen 
	virtual void drawSingleLineText( const s32 x, const s32 y, const s32 iColor,const tchar* sText, s32 iCount );
	virtual void drawSingleLineText( const rect2_x&, u32 uDrawFlags, const s32 iColor,const tchar* sText );
	//@ draws a static text to the screen 
	virtual void drawMultilineText( const rect2_x&, u32 uDrawFlags, const s32 iColor,const tchar* sText );
	//@ draws a static text to the screen 
//	X_Api void drawSingleLineText( const rect2_x&, u32 uDrawFlags, const s32 *piColors, const s32 *piPos, const tchar* sText );
	//@ draws a static text to the screen 
//	X_Api void drawMultilineText( const rect2_x&, u32 uDrawFlags, const s32 *piColors, const s32 *piPos, const tchar* sText );
};


}

#endif //NEXTAR_CDIRECTX9FONT