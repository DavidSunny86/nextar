//@ Abhishek Dey
//@ Jan 8 2007
//@ Sep 17 07

#ifndef NEXTAR__IXUIFONT_H__
#define NEXTAR__IXUIFONT_H__

#include "xtypesex.h"
#include "iobject.h"
#include "graphics/itexture.h"
#include "io/ifilereader.h"

#define WRONG_CHAR			95
#define TAB_SIZE			5

namespace nextar
{

enum FONTSTYLE
{
	FONTSTYLE_ITALIC = 0x1,
	FONTSTYLE_UNDERLINED = 0x2,
	FONTSTYLE_STRIKEOUT = 0x4
};

//@ array values : 
//@ 0 to 95 = ascii codes ' ' to  WRONG_CHAR 
//@ 96 to rest = symbols 
//@ Ascii key identified.
//@ >= 32 
//@ < 128 

//@ Symbols usage 
//@ To insert a symbol into the text 
//@ just insert the index of the symbol 
//@ in the symbol table + 32 into the text 
//@ where ever you want to see the symbol 
//@ Eg. Suppose a file icon has an index 96 
//@ in the symbol table. i.e. it is your 0th 
//@ symbol. To insert it in the text "<icon> Filebname.dat"
//@ you call drawTextXXX with the text "\x80 Filename.dat" 
//@ 0x80 is equal to 128 which comes from 96+32 

//@ Multicolor\Color usage 
//@ The drawTextXXX call must be done using at least one 
//@ color pointer. To draw multicolor text use an array_t 
//@ of 32 bit color values. In the text indicate where 
//@ you want the color change by piPos paramter. Until 
//@ this paramter equat_tes to -1 color changes 
//@ at respective positions will take place 
//@ Make sure the color array_t contains 1 more than as many  
//@ position identifiers to make sure the 0th position has 
//@ some color and that the last pos identifier is -1. 
//@ Note that both piPos and piColor must be valid; no null allowed 
class iXuiFont : public iObject
{
public:
	// flags for draw method
	enum { DrawXCentered = 0x1, DrawYCentered = 0x2 };
	enum { TabCount = 5 };

	struct cFontChar
	{
		s32 width;
		f32 mfu1, mfv1, mfu2, mfv2;
	};

protected:

	//@ the placement of  variables 
	//@ is important for the asm codes 
	//@ to work properly 

	cFontChar*	mpkChars;
	s32			miMaxIndex;
	s32			char_height;
	
	u32			muFontStyle;
	bool		mbHasSymbols;

public:

	iXuiFont();
	virtual ~iXuiFont();
	
	_n_inline iXuiFont::cFontChar*	getFontCharacters() { return mpkChars; }
	_n_inline s32						getWidth(const tchar c) { X_Assert(c>=32 && c<128); return mpkChars[c-32].width; }
	_n_inline s32						getMaximumCharacter( ) { return miMaxIndex; }
	_n_inline s32						getCharacterHeight( ) { return char_height; }
	_n_inline u32						getFontStyle() { return muFontStyle; }

	//@ returns the total space the sub text [startIdx, endIdx) takes in pixels,
	//@ should not include symbols. 
	X_Api s32	getTextExtents( const tchar* text, s32 startIdx, s32 endIdx, s32& lines ) const;
	X_Api s32	getTextExtents( const tchar* text, s32 startIdx, s32 endIdx ) const;
	// returns the position of the starting character given an end index to start the
	//@ draws a static text to the screen 
	virtual void drawSingleLineText( const i2d x, const i2d y, const s32 iColor,const tchar* sText, s32 iCount ) = 0;
	virtual void drawSingleLineText( const rect2_x&, u32 uDrawFlags, const s32 iColor,const tchar* sText ) = 0;
	//@ draws a static text to the screen 
	virtual void drawMultilineText( const rect2_x&, u32 uDrawFlags, const s32 iColor,const tchar* sText ) = 0;
	//@ draws a static text to the screen 
//	X_Api void drawSingleLineText( const rect2_x&, u32 uDrawFlags, const s32 *piColors, const s32 *piPos, const tchar* sText );
	//@ draws a static text to the screen 
//	X_Api void drawMultilineText( const rect2_x&, u32 uDrawFlags, const s32 *piColors, const s32 *piPos, const tchar* sText );

protected:

	virtual void setTexture(iTexture*) = 0;
	bool	_createFromFile( iFileReader* );
	friend class cFontFactory;
};

}

#endif //NEXTAR__IXUIFONT_H__